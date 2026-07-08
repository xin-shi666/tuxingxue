#include "stdafx.h"
#include "Projection.h"

CProjection::CProjection() : m_zoom(1.0f) {}

void CProjection::SetPerspective(float fovYDeg, float aspect, float zn, float zf) {
    float fovY = fovYDeg * 3.14159265f / 180.0f;
    m_proj.SetPerspective(fovY, aspect, zn, zf);
}

void CProjection::SetViewport(int x, int y, int w, int h) {
    m_viewport.SetViewport(x, y, w, h);
}
