#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera() : m_position(0, 0, 5), m_target(0, 0, 0), m_up(0, 1, 0),
                     m_distance(5.0f), m_rotX(0), m_rotY(0) {}

void CCamera::SetPosition(float x, float y, float z) {
    m_position = Vector4(x, y, z);
}

void CCamera::SetTarget(float x, float y, float z) {
    m_target = Vector4(x, y, z);
}

void CCamera::SetUp(float x, float y, float z) {
    m_up = Vector4(x, y, z);
}

CTransform CCamera::GetViewMatrix() const {
    CTransform view;
    view.SetLookAt(m_position, m_target, m_up);
    return view;
}

void CCamera::Orbit(float angleX, float angleY) {
    m_rotX += angleX;
    m_rotY += angleY;
    float x = m_distance * sinf(m_rotY) * cosf(m_rotX);
    float y = m_distance * sinf(m_rotX);
    float z = m_distance * cosf(m_rotY) * cosf(m_rotX);
    m_position = Vector4(x, y, z);
}
