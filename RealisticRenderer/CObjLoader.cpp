#include "pch.h"
#include "CObjLoader.h"

CObjLoader::CObjLoader() : m_lastError(L"") {}

BOOL CObjLoader::Load(const wchar_t* filename, RawData& outData) {
    FILE* fp = nullptr;
    _wfopen_s(&fp, filename, L"rb");
    if (!fp) { m_lastError = L"Failed to open file"; return FALSE; }

    outData.vertices.clear();
    outData.texcoords.clear();
    outData.faces.clear();
    outData.hasTexcoords = FALSE;

    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'v' && line[1] == ' ') {
            double x, y, z;
            if (sscanf_s(line + 2, "%lf %lf %lf", &x, &y, &z) == 3) {
                outData.vertices.push_back(x);
                outData.vertices.push_back(y);
                outData.vertices.push_back(z);
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            double u, v;
            if (sscanf_s(line + 3, "%lf %lf", &u, &v) >= 2) {
                outData.texcoords.push_back(u);
                outData.texcoords.push_back(v);
                outData.hasTexcoords = TRUE;
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
                int v = strtol(p, &p, 10);
                int t = 0;
                if (*p == '/') {
                    p++;
                    if (*p != '/') t = strtol(p, &p, 10);
                    while (*p && *p != ' ') p++;
                }
                if (v != 0) {
                    face.v[vi] = (v > 0) ? (v - 1) : ((int)outData.vertices.size() / 3 + v);
                    if (t != 0) face.t[vi] = (t > 0) ? (t - 1) : ((int)outData.texcoords.size() / 2 + t);
                }
                vi++;
            }
            if (face.v[0] >= 0 && face.v[1] >= 0 && face.v[2] >= 0)
                outData.faces.push_back(face);
        }
    }
    fclose(fp);
    return TRUE;
}
