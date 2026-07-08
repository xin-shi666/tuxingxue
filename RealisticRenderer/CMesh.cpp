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
        int tcCount = (std::min)((int)raw.texcoords.size() / 2, m_vertexCount);
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
    if (m_vertexCount < 100) return;

    double minX = 1e30, minY = 1e30, minZ = 1e30;
    double maxX = -1e30, maxY = -1e30, maxZ = -1e30;
    for (int i = 0; i < m_vertexCount; i++) {
        if (m_vertices[i].x < minX) minX = m_vertices[i].x;
        if (m_vertices[i].x > maxX) maxX = m_vertices[i].x;
        if (m_vertices[i].y < minY) minY = m_vertices[i].y;
        if (m_vertices[i].y > maxY) maxY = m_vertices[i].y;
        if (m_vertices[i].z < minZ) minZ = m_vertices[i].z;
        if (m_vertices[i].z > maxZ) maxZ = m_vertices[i].z;
    }

    double rangeX = maxX - minX + 1e-10;
    double rangeY = maxY - minY + 1e-10;
    double rangeZ = maxZ - minZ + 1e-10;

    // Spatial hash key helper
    auto makeKey = [&](int cx, int cy, int cz) -> long long {
        return (long long)cx + (long long)(gridSize + 1) * cy
             + (long long)(gridSize + 1) * (gridSize + 1) * cz;
    };

    // Accumulate vertex positions per grid cell
    std::unordered_map<long long, double> sumX, sumY, sumZ;
    std::unordered_map<long long, int> counts;
    std::vector<long long> cellKeys(m_vertexCount);

    for (int i = 0; i < m_vertexCount; i++) {
        int cx = (int)((m_vertices[i].x - minX) / rangeX * gridSize);
        int cy = (int)((m_vertices[i].y - minY) / rangeY * gridSize);
        int cz = (int)((m_vertices[i].z - minZ) / rangeZ * gridSize);
        if (cx < 0) cx = 0; if (cx > gridSize) cx = gridSize;
        if (cy < 0) cy = 0; if (cy > gridSize) cy = gridSize;
        if (cz < 0) cz = 0; if (cz > gridSize) cz = gridSize;
        long long key = makeKey(cx, cy, cz);
        cellKeys[i] = key;
        sumX[key] += m_vertices[i].x;
        sumY[key] += m_vertices[i].y;
        sumZ[key] += m_vertices[i].z;
        counts[key]++;
    }

    // Create averaged vertex per occupied cell
    std::unordered_map<long long, int> keyToNewIdx;
    int newVC = 0;
    for (auto& kv : counts) {
        long long key = kv.first;
        keyToNewIdx[key] = newVC++;
    }

    CP3* newVerts = new CP3[newVC];
    double* newWorld = new double[newVC * 3];
    for (auto& kv : counts) {
        long long key = kv.first;
        int idx = keyToNewIdx[key];
        int cnt = kv.second;
        newVerts[idx] = CP3(sumX[key] / cnt, sumY[key] / cnt, sumZ[key] / cnt);
        newWorld[idx * 3] = newVerts[idx].x;
        newWorld[idx * 3 + 1] = newVerts[idx].y;
        newWorld[idx * 3 + 2] = newVerts[idx].z;
    }

    // Remap faces (remove degenerate ones)
    std::vector<Face> newFaces;
    newFaces.reserve(m_faces.size());
    for (auto& f : m_faces) {
        if (f.v[0] >= m_vertexCount || f.v[1] >= m_vertexCount || f.v[2] >= m_vertexCount) continue;
        long long k0 = cellKeys[f.v[0]], k1 = cellKeys[f.v[1]], k2 = cellKeys[f.v[2]];
        if (k0 == k1 && k1 == k2) continue; // all same cell => degenerate
        Face nf;
        nf.v[0] = keyToNewIdx[k0]; nf.v[1] = keyToNewIdx[k1]; nf.v[2] = keyToNewIdx[k2];
        nf.t[0] = nf.t[1] = nf.t[2] = -1;
        newFaces.push_back(nf);
    }

    // Replace old data
    delete[] m_vertices; delete[] m_worldPos; delete[] m_normals;
    delete[] m_faceIndices;
    if (m_texcoords) { delete[] m_texcoords; m_texcoords = nullptr; }

    m_vertices = newVerts;
    m_worldPos = newWorld;
    m_normals = new CVector3[newVC];
    m_vertexCount = newVC;
    m_faces = std::move(newFaces);
    m_faceIndices = new int[m_faces.size() * 3];
    for (size_t i = 0; i < m_faces.size(); i++) {
        m_faceIndices[i * 3] = m_faces[i].v[0];
        m_faceIndices[i * 3 + 1] = m_faces[i].v[1];
        m_faceIndices[i * 3 + 2] = m_faces[i].v[2];
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
        maxR = (std::max)(maxR, dx * dx + dy * dy + dz * dz);
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
