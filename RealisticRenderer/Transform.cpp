#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform() { SetIdentity(); }

void CTransform::SetIdentity() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
}

CTransform CTransform::operator*(const CTransform& other) const {
    CTransform result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = m[i][0] * other.m[0][j] + m[i][1] * other.m[1][j]
                           + m[i][2] * other.m[2][j] + m[i][3] * other.m[3][j];
        }
    }
    return result;
}

Vector4 CTransform::TransformPoint(const Vector4& v) const {
    Vector4 r;
    r.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
    r.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
    r.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
    r.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
    return r;
}

Vector4 CTransform::TransformVector(const Vector4& v) const {
    Vector4 r;
    r.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
    r.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
    r.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
    r.w = 0;
    return r;
}

Vector4 CTransform::TransformNormal(const Vector4& n) const {
    // Use inverse-transpose for normals
    // Simplified: works when no non-uniform scale
    return TransformVector(n);
}

void CTransform::SetTranslation(float tx, float ty, float tz) {
    SetIdentity();
    m[3][0] = tx; m[3][1] = ty; m[3][2] = tz;
}

void CTransform::SetRotationX(float angle) {
    SetIdentity();
    float c = cosf(angle), s = sinf(angle);
    m[1][1] = c;  m[1][2] = s;
    m[2][1] = -s; m[2][2] = c;
}

void CTransform::SetRotationY(float angle) {
    SetIdentity();
    float c = cosf(angle), s = sinf(angle);
    m[0][0] = c;  m[0][2] = -s;
    m[2][0] = s;  m[2][2] = c;
}

void CTransform::SetRotationZ(float angle) {
    SetIdentity();
    float c = cosf(angle), s = sinf(angle);
    m[0][0] = c;  m[0][1] = s;
    m[1][0] = -s; m[1][1] = c;
}

void CTransform::SetScale(float sx, float sy, float sz) {
    SetIdentity();
    m[0][0] = sx; m[1][1] = sy; m[2][2] = sz;
}

void CTransform::SetPerspective(float fovY, float aspect, float zn, float zf) {
    SetIdentity();
    float h = 1.0f / tanf(fovY * 0.5f);
    m[0][0] = h / aspect;
    m[1][1] = h;
    m[2][2] = zf / (zf - zn);
    m[2][3] = 1.0f;
    m[3][2] = -zn * zf / (zf - zn);
    m[3][3] = 0.0f;
}

void CTransform::SetLookAt(const Vector4& eye, const Vector4& at, const Vector4& up) {
    Vector4 zaxis = (eye - at).Normalize();
    Vector4 xaxis = up.Cross(zaxis).Normalize();
    Vector4 yaxis = zaxis.Cross(xaxis);
    SetIdentity();
    m[0][0] = xaxis.x; m[0][1] = yaxis.x; m[0][2] = zaxis.x;
    m[1][0] = xaxis.y; m[1][1] = yaxis.y; m[1][2] = zaxis.y;
    m[2][0] = xaxis.z; m[2][1] = yaxis.z; m[2][2] = zaxis.z;
    m[3][0] = -xaxis.Dot(eye);
    m[3][1] = -yaxis.Dot(eye);
    m[3][2] = -zaxis.Dot(eye);
}

void CTransform::SetViewport(int x, int y, int w, int h) {
    SetIdentity();
    float hw = w * 0.5f, hh = h * 0.5f;
    m[0][0] = hw;
    m[1][1] = -hh;
    m[3][0] = x + hw;
    m[3][1] = y + hh;
}
