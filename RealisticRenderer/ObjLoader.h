#pragma once
#include "Vertex.h"
#include <vector>
#include <string>

class CObjLoader {
public:
    CObjLoader();

    struct RawMesh {
        std::vector<float> positions;    // x,y,z triplets
        std::vector<float> texcoords;    // u,v pairs
        std::vector<Face> faces;
        bool hasTexcoords;
    };

    bool Load(const wchar_t* filename, RawMesh& outMesh);
    const wchar_t* GetLastError() const { return m_lastError; }

private:
    const wchar_t* m_lastError;
};
