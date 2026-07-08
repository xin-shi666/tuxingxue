#include "pch.h"
#include "CMesh.h"
#include <unordered_map>

CMesh::CMesh() : m_vertices(nullptr), m_normals(nullptr), m_texcoords(nullptr),
    m_worldPos(nullptr), m_faceIndices(nullptr), m_vertexCount(0),
    m_hasTex(FALSE), m_hasNorms(FALSE) {}

void CMesh::LoadFromRaw(const CObjLoader::RawData& raw) {
    m_vertexCount = (int)raw.vertices.size() / 3;
    m_vertices = new CP3[m_vertexCount];
    m_worldPos = new double[m_vertexCount * 3];
    m_normals = new CVector3[m_vertexCount];

    for (int i = 0; i < m_vertexCount; i++) {
        double x = raw.vertices[i * 3];
        double y = raw.vertices[i * 3 + 1];
        double z = raw.vertices[i * 3 + 2];
        m_vertices[i] = CP3(x, y, z);
        m_worldPos[i * 3] = x;
        m_worldPos[i * 3 + 1] = y;
        m_worldPos[i * 3 + 2] = z;
        m_normals[i] = CVector3(0, 0, 0);
    }

    m_hasTex = raw.hasTexcoords;
    if (m_hasTex) {
        m_texcoords = new double[m_vertexCount * 2];
        int tcCount = std::min((int)raw.texcoords.size() / 2, m_vertexCount);
        for (int i = 0; i < tcCount; i++) {
            m_texcoords[i * 2] = raw.texcoords[i * 2];
            m_texcoords[i * 2 + 1] = raw.texcoords[i * 2 + 1];
        }
    } else {
        m_texcoords = nullptr;
    }

    m_faces = raw.faces;
    int fc = (int)m_faces.size();
    m_faceIndices = new int[fc * 3];
    for (int i = 0; i < fc; i++) {
        m_faceIndices[i * 3] = m_faces[i].v[0];
        m_faceIndices[i * 3 + 1] = m_faces[i].v[1];
        m_faceIndices[i * 3 + 2] = m_faces[i].v[2];
    }

    ComputeNormals();
}

void CMesh::ComputeNormals() {
    for (int i = 0; i < m_vertexCount; i++)
        m_normals[i] = CVector3(0, 0, 0);

    for (const auto& f : m_faces) {
        CP3& v0 = m_vertices[f.v[0]];
        CP3& v1 = m_vertices[f.v[1]];
        CP3& v2 = m_vertices[f.v[2]];
        CVector3 e1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
        CVector3 e2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
        CVector3 fn = e1.Cross(e2);
        m_normals[f.v[0]] += fn;
        m_normals[f.v[1]] += fn;
        m_normals[f.v[2]] += fn;
    }

    for (int i = 0; i < m_vertexCount; i++)
        m_normals[i] = m_normals[i].Normalize();

    m_hasNorms = TRUE;
}

void CMesh::Simplify(int gridSize) {
    if (gridSize < 2) gridSize = 2;

    double minX = 1e30, minY = 1e30, minZ = 1e30;
    double maxX = -1e30, maxY = -1e30, maxZ = -1e30;
    for (int i = 0; i < m_vertexCount; i++) {
        minX = std::min(minX, m_vertices[i].x); maxX = std::max(maxX, m_vertices[i].x);
        minY = std::min(minY, m_vertices[i].y); maxY = std::max(maxY, m_vertices[i].y);
        minZ = std::min(minZ, m_vertices[i].z); maxZ = std::max(maxZ, m_vertices[i].z);
    }
    double dx = (maxX - minX) / gridSize + 1e-6;
    double dy = (maxY - minY) / gridSize + 1e-6;
    double dz = (maxZ - minZ) / gridSize + 1e-6;

    std::unordered_map<int, int> vertexMap;
    std::unordered_map<int, CP3> clusters;

    for (int i = 0; i < m_vertexCount; i++) {
        int cx = (int)((m_vertices[i].x - minX) / dx);
        int cy = (int)((m_vertices[i].y - minY) / dy);
        int cz = (int)((m_vertices[i].z - minZ) / dz);
        int key = cx + cy * (gridSize + 1) + cz * (gridSize + 1) * (gridSize + 1);
        vertexMap[i] = key;
        if (clusters.find(key) == clusters.end()) {
            clusters[key] = m_vertices[i];
        } else {
            clusters[key] = clusters[key] + m_vertices[i];
        }
    }

    // Average cluster positions
    std::unordered_map<int, int> counts;
    for (int i = 0; i < m_vertexCount; i++) {
        counts[vertexMap[i]]++;
    }
    for (auto& c : clusters) {
        int cnt = counts[c.first];
        if (cnt > 1) c.second = c.second / (double)cnt;
    }

    // Build new vertex array
    std::unordered_map<int, int> keyToIdx;
    int newCount = 0;
    CP3* newVerts = new CP3[clusters.size()];
    double* newWorld = new double[clusters.size() * 3];
    for (auto& c : clusters) {
        keyToIdx[c.first] = newCount;
        newVerts[newCount] = CP3(c.second.x, c.second.y, c.second.z);
        newWorld[newCount * 3] = c.second.x;
        newWorld[newCount * 3 + 1] = c.second.y;
        newWorld[newCount * 3 + 2] = c.second.z;
        newCount++;
    }

    // Remap faces
    std::vector<Face> newFaces;
    for (auto& f : m_faces) {
        int c0 = vertexMap[f.v[0]], c1 = vertexMap[f.v[1]], c2 = vertexMap[f.v[2]];
        if (c0 == c1 || c1 == c2 || c0 == c2) continue;
        Face nf;
        nf.v[0] = keyToIdx[c0]; nf.v[1] = keyToIdx[c1]; nf.v[2] = keyToIdx[c2];
        nf.t[0] = nf.t[1] = nf.t[2] = -1;
        newFaces.push_back(nf);
    }

    delete[] m_vertices; delete[] m_worldPos; delete[] m_normals;
    delete[] m_faceIndices;
    if (m_texcoords) { delete[] m_texcoords; m_texcoords = nullptr; }

    m_vertices = newVerts;
    m_worldPos = newWorld;
    m_normals = new CVector3[newCount];
    m_vertexCount = newCount;
    m_faces = newFaces;
    m_faceIndices = new int[newFaces.size() * 3];
    for (size_t i = 0; i < newFaces.size(); i++) {
        m_faceIndices[i * 3] = newFaces[i].v[0];
        m_faceIndices[i * 3 + 1] = newFaces[i].v[1];
        m_faceIndices[i * 3 + 2] = newFaces[i].v[2];
    }
    m_hasTex = FALSE;
    ComputeNormals();
}

double CMesh::GetRadius() const {
    CP3 center = GetCenter();
    double maxR = 0;
    for (int i = 0; i < m_vertexCount; i++) {
        double dx = m_vertices[i].x - center.x;
        double dy = m_vertices[i].y - center.y;
        double dz = m_vertices[i].z - center.z;
        maxR = std::max(maxR, dx * dx + dy * dy + dz * dz);
    }
    return sqrt(maxR);
}

CP3 CMesh::GetCenter() const {
    if (m_vertexCount == 0) return CP3(0, 0, 0);
    CP3 sum(0, 0, 0);
    for (int i = 0; i < m_vertexCount; i++) sum = sum + m_vertices[i];
    return sum / (double)m_vertexCount;
}

void CMesh::ResetToOriginal() {
    for (int i = 0; i < m_vertexCount; i++) {
        m_vertices[i].x = m_worldPos[i * 3];
        m_vertices[i].y = m_worldPos[i * 3 + 1];
        m_vertices[i].z = m_worldPos[i * 3 + 2];
    }
}
