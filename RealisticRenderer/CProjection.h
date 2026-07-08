#pragma once
#include "CP3.h"

class CProjection {
public:
    CProjection();
    void SetEye(double x, double y, double z);
    void PerspectiveProjection(CP3* P, int ptNumber);
    void SetViewDistance(double d) { m_d = d; }
    void SetScreenSize(int w, int h) { m_scrW = w; m_scrH = h; }
    CP3 GetEye() const { return m_eye; }

private:
    double m_d;
    double m_scrW, m_scrH;
    CP3 m_eye;
};
