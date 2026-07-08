#include "stdafx.h"
#include "Mesh.h"
#include <algorithm>
#include <unordered_map>

CMesh::CMesh() : m_vertexCount(0), m_hasTexcoords(false), m_hasNormals(false) {}

void CMesh::InitFromRaw(const std::vector<float>& positions,
                        const std::vector<float>& texcoords,
                        const std::vector<Face>& faces,
                        bool hasTexcoords) {
    m_positions = positions;
    m_texcoords = texcoords;
    m_faces = faces;
    m_vertexCount = positions.size() / 3;
    m_hasTexcoords = hasTexcoords;
    m_hasNormals = false;
}

void CMesh::ComputeNormals() {
    m_normals.assign(m_vertexCount * 3, 0.0f);

    for (const auto& face : m_faces) {
        int i0 = face.v[0], i1 = face.v[1], i2 = face.v[2];
        if (i0 < 0 || i1 < 0 || i2 < 0) continue;

        Vector4 v0(m_positions[i0*3], m_positions[i0*3+1], m_positions[i0*3+2]);
        Vector4 v1(m_positions[i1*3], m_positions[i1*3+1], m_positions[i1*3+2]);
        Vector4 v2(m_positions[i2*3], m_positions[i2*3+1], m_positions[i2*3+2]);

        Vector4 e1 = v1 - v0;
        Vector4 e2 = v2 - v0;
        Vector4 fn = e1.Cross(e2);

        for (int j = 0; j < 3; j++) {
            int idx = face.v[j] * 3;
            m_normals[idx + 0] += fn.x;
            m_normals[idx + 1] += fn.y;
            m_normals[idx + 2] += fn.z;
        }
    }

    // Normalize
    for (size_t i = 0; i < m_vertexCount; i++) {
        float x = m_normals[i*3], y = m_normals[i*3+1], z = m_normals[i*3+2];
        float len = sqrtf(x*x + y*y + z*z);
        if (len > 1e-8f) {
            m_normals[i*3] /= len;
            m_normals[i*3+1] /= len;
            m_normals[i*3+2] /= len;
        }
    }
    m_hasNormals = true;
}

void CMesh::Simplify(int gridSize) {
    if (gridSize < 2) gridSize = 2;

    // Find bounding box
    float minX = 1e30f, minY = 1e30f, minZ = 1e30f;
    float maxX = -1e30f, maxY = -1e30f, maxZ = -1e30f;
    for (size_t i = 0; i < m_vertexCount; i++) {
        minX = std::min(minX, m_positions[i*3+0]);
        maxX = std::max(maxX, m_positions[i*3+0]);
        minY = std::min(minY, m_positions[i*3+1]);
        maxY = std::max(maxY, m_positions[i*3+1]);
        minZ = std::min(minZ, m_positions[i*3+2]);
        maxZ = std::max(maxZ, m_positions[i*3+2]);
    }
    float sizeX = maxX - minX, sizeY = maxY - minY, sizeZ = maxZ - minZ;
    float cellX = sizeX / gridSize + 1e-6f;
    float cellY = sizeY / gridSize + 1e-6f;
    float cellZ = sizeZ / gridSize + 1e-6f;

    // Map each vertex to a cluster, accumulate position
    struct Cluster { float sx, sy, sz; int count; };
    std::unordered_map<int, Cluster> clusters;

    std::vector<int> vertexMap(m_vertexCount, -1);

    for (size_t i = 0; i < m_vertexCount; i++) {
        int cx = (int)((m_positions[i*3+0] - minX) / cellX);
        int cy = (int)((m_positions[i*3+1] - minY) / cellY);
        int cz = (int)((m_positions[i*3+2] - minZ) / cellZ);
        int key = cx + cy * (gridSize + 1) + cz * (gridSize + 1) * (gridSize + 1);

        auto& c = clusters[key];
        c.sx += m_positions[i*3+0];
        c.sy += m_positions[i*3+1];
        c.sz += m_positions[i*3+2];
        c.count++;
        vertexMap[i] = key;
    }

    // Build new vertex list
    std::unordered_map<int, int> keyToNewIdx;
    std::vector<float> newPositions;
    int newIdx = 0;
    for (auto& pair : clusters) {
        keyToNewIdx[pair.first] = newIdx++;
        newPositions.push_back(pair.second.sx / pair.second.count);
        newPositions.push_back(pair.second.sy / pair.second.count);
        newPositions.push_back(pair.second.sz / pair.second.count);
    }

    // Remap faces, remove degenerate
    std::vector<Face> newFaces;
    for (auto& face : m_faces) {
        int c0 = vertexMap[face.v[0]], c1 = vertexMap[face.v[1]], c2 = vertexMap[face.v[2]];
        if (c0 == c1 || c1 == c2 || c0 == c2) continue; // degenerate
        Face nf;
        nf.v[0] = keyToNewIdx[c0];
        nf.v[1] = keyToNewIdx[c1];
        nf.v[2] = keyToNewIdx[c2];
        nf.t[0] = nf.t[1] = nf.t[2] = -1;
        newFaces.push_back(nf);
    }

    m_positions = newPositions;
    m_faces = newFaces;
    m_vertexCount = newPositions.size() / 3;
    m_texcoords.clear();
    m_normals.clear();
    m_hasTexcoords = false;
    m_hasNormals = false;
}

Vector4 CMesh::GetCenter() const {
    if (m_vertexCount == 0) return Vector4(0, 0, 0);
    float cx = 0, cy = 0, cz = 0;
    for (size_t i = 0; i < m_vertexCount; i++) {
        cx += m_positions[i*3+0];
        cy += m_positions[i*3+1];
        cz += m_positions[i*3+2];
    }
    return Vector4(cx / m_vertexCount, cy / m_vertexCount, cz / m_vertexCount);
}

float CMesh::GetRadius() const {
    Vector4 center = GetCenter();
    float maxR = 0;
    for (size_t i = 0; i < m_vertexCount; i++) {
        float dx = m_positions[i*3+0] - center.x;
        float dy = m_positions[i*3+1] - center.y;
        float dz = m_positions[i*3+2] - center.z;
        maxR = std::max(maxR, dx*dx + dy*dy + dz*dz);
    }
    return sqrtf(maxR);
}

void CMesh::GetVertex(int faceIdx, int cornerIdx, Vector4& pos, Vector4& normal, float& u, float& v) const {
    const Face& face = m_faces[faceIdx];
    int vi = face.v[cornerIdx];
    pos = Vector4(m_positions[vi*3], m_positions[vi*3+1], m_positions[vi*3+2]);

    if (m_hasNormals && vi >= 0) {
        normal = Vector4(m_normals[vi*3], m_normals[vi*3+1], m_normals[vi*3+2], 0);
    } else {
        normal = Vector4(0, 0, 0, 0);
    }

    int ti = face.t[cornerIdx];
    if (m_hasTexcoords && ti >= 0 && (size_t)ti * 2 + 1 < m_texcoords.size()) {
        u = m_texcoords[ti*2];
        v = m_texcoords[ti*2+1];
    } else {
        u = v = 0;
    }
}
