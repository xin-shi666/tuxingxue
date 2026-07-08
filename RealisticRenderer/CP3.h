#pragma once
#include "CRGB.h"

class CVector3;

class CP3 {
public:
    double x, y, z, w;
    CRGB c;
    CVector3* n;

    CP3() : x(0), y(0), z(0), w(1), n(nullptr) {}
    CP3(double _x, double _y, double _z) : x(_x), y(_y), z(_z), w(1), n(nullptr) {}

    CP3 operator+(const CP3& p) const { return CP3(x + p.x, y + p.y, z + p.z); }
    CP3 operator-(const CP3& p) const { return CP3(x - p.x, y - p.y, z - p.z); }
    CP3 operator*(double s) const { return CP3(x * s, y * s, z * s); }
    CP3 operator/(double s) const { return CP3(x / s, y / s, z / s); }
};
