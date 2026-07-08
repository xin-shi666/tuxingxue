#pragma once

struct VertexIn {
    float x, y, z;
};

struct VertexOut {
    float x, y, z;          // screen pos (pixels + depth)
    float wx, wy, wz;       // world position
    float nx, ny, nz;       // world normal
    float u, v;             // texcoord
    float rhw;              // 1/w for perspective correction
};

struct Face {
    int v[3];   // vertex indices
    int t[3];   // texcoord indices (-1 if none)
};
