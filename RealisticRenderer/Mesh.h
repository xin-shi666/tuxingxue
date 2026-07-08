#pragma once
#include "Vertex.h"
#include "Transform.h"
#include <vector>

class CMesh {
public:
    CMesh();

    // Copy raw data from OBJ loader
    void InitFromRaw(const std::vector<float>& positions,
                     const std::vector<float>& texcoords,
                     const std::vector<Face>& faces,
                     bool hasTexcoords);

    // Compute per-vertex normals from face normals
    void ComputeNormals();

    // Simplify mesh using vertex clustering
    void Simplify(int gridSize);

    // Get bounding box center for view positioning
    Vector4 GetCenter() const;
    float GetRadius() const;

    // Accessors
    size_t GetVertexCount() const { return m_vertexCount; }
    size_t GetFaceCount() const { return m_faces.size(); }
    const Face* GetFaces() const { return m_faces.data(); }
    const float* GetPositions() const { return m_positions.data(); }
    const float* GetNormals() const { return m_normals.data(); }
    const float* GetTexcoords() const { return m_texcoords.data(); }
    bool HasTexcoords() const { return m_hasTexcoords; }
    bool HasNormals() const { return m_hasNormals; }

    // Get vertex data for a specific face corner
    void GetVertex(int faceIdx, int cornerIdx, Vector4& pos, Vector4& normal, float& u, float& v) const;

private:
    std::vector<float> m_positions;   // x,y,z per vertex
    std::vector<float> m_normals;     // nx,ny,nz per vertex
    std::vector<float> m_texcoords;   // u,v per vertex
    std::vector<Face> m_faces;
    size_t m_vertexCount;
    bool m_hasTexcoords;
    bool m_hasNormals;
};
