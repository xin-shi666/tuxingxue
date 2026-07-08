#include "stdafx.h"
#include "ObjLoader.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

CObjLoader::CObjLoader() : m_lastError(L"") {}

bool CObjLoader::Load(const wchar_t* filename, RawMesh& outMesh) {
    FILE* fp = _wfopen(filename, L"rb");
    if (!fp) { m_lastError = L"Cannot open file"; return false; }

    outMesh.positions.clear();
    outMesh.texcoords.clear();
    outMesh.faces.clear();
    outMesh.hasTexcoords = false;

    char line[4096];
    int lineNum = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineNum++;
        if (lineNum % 500000 == 0) continue; // heartbeat, no-op

        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            if (sscanf_s(line + 2, "%f %f %f", &x, &y, &z) == 3) {
                outMesh.positions.push_back(x);
                outMesh.positions.push_back(y);
                outMesh.positions.push_back(z);
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            float u, v, w;
            int n = sscanf_s(line + 3, "%f %f %f", &u, &v, &w);
            if (n >= 2) {
                outMesh.texcoords.push_back(u);
                outMesh.texcoords.push_back(v);
                outMesh.hasTexcoords = true;
            }
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            Face face;
            face.v[0] = face.v[1] = face.v[2] = -1;
            face.t[0] = face.t[1] = face.t[2] = -1;

            char* p = line + 2;
            int vi = 0;
            while (*p && vi < 3) {
                while (*p == ' ') p++;
                if (!*p) break;

                int v = 0, t = 0;
                v = strtol(p, &p, 10);
                if (*p == '/') {
                    p++;
                    if (*p != '/') {
                        t = strtol(p, &p, 10);
                    }
                }
                if (v != 0) {
                    face.v[vi] = (v > 0) ? (v - 1) : (int)(outMesh.positions.size() / 3 + v);
                    if (t != 0)
                        face.t[vi] = (t > 0) ? (t - 1) : (int)(outMesh.texcoords.size() / 2 + t);
                }
                vi++;
            }
            if (face.v[0] >= 0 && face.v[1] >= 0 && face.v[2] >= 0)
                outMesh.faces.push_back(face);
        }
    }
    fclose(fp);
    return true;
}
