#pragma once
#include <cmath>

struct Vector4 {
    float x, y, z, w;
    Vector4() : x(0), y(0), z(0), w(1) {}
    Vector4(float _x, float _y, float _z, float _w = 1.0f) : x(_x), y(_y), z(_z), w(_w) {}

    float Dot(const Vector4& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector4 Cross(const Vector4& v) const {
        return Vector4(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, 0);
    }
    Vector4 operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, 0); }
    Vector4 operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, 0); }
    Vector4 operator*(float s) const { return Vector4(x * s, y * s, z * s, 0); }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    Vector4 Normalize() const { float len = Length(); if (len < 1e-8f) return *this; float inv = 1.0f / len; return Vector4(x * inv, y * inv, z * inv, 0); }
};

class CTransform {
public:
    CTransform();
    void SetIdentity();

    // Multiplication
    CTransform operator*(const CTransform& other) const;
    Vector4 TransformPoint(const Vector4& v) const;
    Vector4 TransformVector(const Vector4& v) const;
    Vector4 TransformNormal(const Vector4& n) const;

    // Build transformation matrices
    void SetTranslation(float tx, float ty, float tz);
    void SetRotationX(float angle);
    void SetRotationY(float angle);
    void SetRotationZ(float angle);
    void SetScale(float sx, float sy, float sz);
    void SetPerspective(float fovY, float aspect, float zn, float zf);
    void SetLookAt(const Vector4& eye, const Vector4& at, const Vector4& up);
    void SetViewport(int x, int y, int w, int h);

    // Access matrix elements [row][col]
    float& operator()(int row, int col) { return m[row][col]; }
    float  operator()(int row, int col) const { return m[row][col]; }

    float* Data() { return &m[0][0]; }

private:
    float m[4][4];  // row-major: m[row][col], row vectors (v' = v * M)
};
