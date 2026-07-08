#pragma once
#include <vector>

struct Face {
    int v[3];
    int t[3];
};

class CObjLoader {
public:
    CObjLoader();

    struct RawData {
        std::vector<double> vertices;    // x,y,z
        std::vector<double> texcoords;   // u,v
        std::vector<Face> faces;
        BOOL hasTexcoords;
    };

    BOOL Load(const wchar_t* filename, RawData& outData);

private:
    const wchar_t* m_lastError;
};
