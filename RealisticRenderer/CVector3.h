#pragma once
#include <cmath>

class CVector3 {
public:
    double x, y, z;

    CVector3() : x(0), y(0), z(0) {}
    CVector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    double Magnitude() const { return sqrt(x * x + y * y + z * z); }

    CVector3 Normalize() const {
        double mag = Magnitude();
        if (mag < 1e-15) return CVector3(0, 0, 0);
        return CVector3(x / mag, y / mag, z / mag);
    }

    double Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    CVector3 Cross(const CVector3& v) const {
        return CVector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    CVector3 operator+(const CVector3& v) const { return CVector3(x + v.x, y + v.y, z + v.z); }
    CVector3 operator-(const CVector3& v) const { return CVector3(x - v.x, y - v.y, z - v.z); }
    CVector3 operator*(double s) const { return CVector3(x * s, y * s, z * s); }
    CVector3 operator/(double s) const { return CVector3(x / s, y / s, z / s); }
    CVector3& operator+=(const CVector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    CVector3& operator*=(double s) { x *= s; y *= s; z *= s; return *this; }
};
