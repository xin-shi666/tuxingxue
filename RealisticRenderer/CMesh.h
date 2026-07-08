#pragma once
#include "CP3.h"
#include "CVector3.h"
#include "CObjLoader.h"
#include <vector>

class CMesh {
public:
    CMesh();

    void LoadFromRaw(const CObjLoader::RawData& raw);
    void ComputeNormals();
    void Simplify(int gridSize);

    int GetVertexCount() const { return m_vertexCount; }
    int GetFaceCount() const { return (int)m_faces.size(); }
    CP3* GetVertices() { return m_vertices; }
    int* GetFaceIndices() { return m_faceIndices; }
    CVector3* GetNormals() { return m_normals; }
    double* GetTexcoords() { return m_texcoords; }
    double* GetWorldPositions() { return m_worldPos; }
    BOOL HasTexcoords() const { return m_hasTex; }
    BOOL HasNormals() const { return m_hasNorms; }
    double GetRadius() const;
    CP3 GetCenter() const;

    // Copy original (untransformed) vertices for animation
    void ResetToOriginal();

private:
    CP3* m_vertices;
    CVector3* m_normals;
    double* m_texcoords;     // u,v per vertex
    double* m_worldPos;      // original world positions (x,y,z)
    int* m_faceIndices;      // 3 indices per face
    std::vector<Face> m_faces;
    int m_vertexCount;
    BOOL m_hasTex;
    BOOL m_hasNorms;
};
