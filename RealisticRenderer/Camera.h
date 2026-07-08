#pragma once
#include "Transform.h"

class CCamera {
public:
    CCamera();
    void SetPosition(float x, float y, float z);
    void SetTarget(float x, float y, float z);
    void SetUp(float x, float y, float z);
    Vector4 GetPosition() const { return m_position; }
    CTransform GetViewMatrix() const;
    void Orbit(float angleX, float angleY);
    void SetDistance(float d) { m_distance = d; }

private:
    Vector4 m_position;
    Vector4 m_target;
    Vector4 m_up;
    float m_distance;
    float m_rotX, m_rotY;
};
