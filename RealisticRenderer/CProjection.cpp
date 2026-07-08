#include "pch.h"
#include "CProjection.h"

CProjection::CProjection() : m_d(800), m_scrW(800), m_scrH(600) {
    SetEye(0, 0, 600);
}

void CProjection::SetEye(double x, double y, double z) {
    m_eye = CP3(x, y, z);
}

void CProjection::PerspectiveProjection(CP3* P, int ptNumber) {
    for (int i = 0; i < ptNumber; i++) {
        double denom = m_eye.z - P[i].z;
        if (fabs(denom) < 0.1) denom = 0.1;
        P[i].x = P[i].x * m_d / denom + m_scrW / 2.0;
        P[i].y = -P[i].y * m_d / denom + m_scrH / 2.0;
        P[i].z = 1.0 / denom; // store 1/z for depth interpolation
    }
}
