#pragma once
#include "Transform.h"

class CProjection {
public:
    CProjection();
    void SetPerspective(float fovYDeg, float aspect, float zn, float zf);
    void SetViewport(int x, int y, int w, int h);
    CTransform GetProjectionMatrix() const { return m_proj; }
    CTransform GetViewportMatrix() const { return m_viewport; }
    void SetZoom(float z) { m_zoom = z; }
    float GetZoom() const { return m_zoom; }

private:
    CTransform m_proj;
    CTransform m_viewport;
    float m_zoom;
};
