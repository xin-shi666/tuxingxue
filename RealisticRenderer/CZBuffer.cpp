#include "pch.h"
#include "CZBuffer.h"

CZBuffer::CZBuffer() : m_zBuffer(nullptr), m_width(0), m_height(0),
    m_mode(WIREFRAME), m_pLighting(nullptr), m_pMaterial(nullptr), m_pTexture(nullptr) {}

CZBuffer::~CZBuffer() {
    if (m_zBuffer) {
        for (int i = 0; i < m_height; i++) delete[] m_zBuffer[i];
        delete[] m_zBuffer;
    }
}

void CZBuffer::InitBuffer(int w, int h) {
    if (m_zBuffer) {
        for (int i = 0; i < m_height; i++) delete[] m_zBuffer[i];
        delete[] m_zBuffer;
    }
    m_width = w; m_height = h;
    m_zBuffer = new double*[m_height];
    for (int i = 0; i < m_height; i++)
        m_zBuffer[i] = new double[m_width];
}

void CZBuffer::ClearBuffer() {
    for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++)
            m_zBuffer[y][x] = 1e30;
}

void CZBuffer::SortVertices(CPoint2& v0, CPoint2& v1, CPoint2& v2) {
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);
}

void CZBuffer::DrawTriangle(CDC* pDC, const CP3& p0, const CP3& p1, const CP3& p2,
                            const CVector3& n0, const CVector3& n1, const CVector3& n2,
                            const CP3& wp0, const CP3& wp1, const CP3& wp2,
                            double u0, double v0, double u1, double v1, double u2, double v2) {
    if (m_mode == WIREFRAME) {
        int x0 = (int)p0.x, y0 = (int)p0.y;
        int x1 = (int)p1.x, y1 = (int)p1.y;
        int x2 = (int)p2.x, y2 = (int)p2.y;
        CPen pen(PS_SOLID, 1, RGB(0, 220, 255));
        CPen* oldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(x0, y0); pDC->LineTo(x1, y1);
        pDC->LineTo(x2, y2); pDC->LineTo(x0, y0);
        pDC->SelectObject(oldPen);
        return;
    }

    // Back-face culling
    double area = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
    if (area <= 0) return;

    CPoint2 s0, s1, s2;
    s0.x = (int)p0.x; s0.y = (int)p0.y; s0.z = p0.z;
    s1.x = (int)p1.x; s1.y = (int)p1.y; s1.z = p1.z;
    s2.x = (int)p2.x; s2.y = (int)p2.y; s2.z = p2.z;
    s0.worldPos = wp0; s1.worldPos = wp1; s2.worldPos = wp2;
    s0.n = n0; s1.n = n1; s2.n = n2;
    s0.u = u0; s0.v = v0;
    s1.u = u1; s1.v = v1;
    s2.u = u2; s2.v = v2;

    if (m_mode == FLAT) {
        CP3 fc((wp0.x + wp1.x + wp2.x) / 3, (wp0.y + wp1.y + wp2.y) / 3, (wp0.z + wp1.z + wp2.z) / 3);
        CVector3 e1(wp1.x - wp0.x, wp1.y - wp0.y, wp1.z - wp0.z);
        CVector3 e2(wp2.x - wp0.x, wp2.y - wp0.y, wp2.z - wp0.z);
        CVector3 fn = e1.Cross(e2).Normalize();
        s0.c = s1.c = s2.c = m_pLighting->Illuminate(m_ViewPoint, fc, fn, m_pMaterial);
    } else if (m_mode == GOURAUD) {
        s0.c = m_pLighting->Illuminate(m_ViewPoint, wp0, n0, m_pMaterial);
        s1.c = m_pLighting->Illuminate(m_ViewPoint, wp1, n1, m_pMaterial);
        s2.c = m_pLighting->Illuminate(m_ViewPoint, wp2, n2, m_pMaterial);
    }

    FillTriangle(pDC, s0, s1, s2);
}

void CZBuffer::FillTriangle(CDC* pDC, const CPoint2& v0, const CPoint2& v1, const CPoint2& v2) {
    // Bounding box
    int xMin = (std::max)(0, (std::min)({v0.x, v1.x, v2.x}));
    int xMax = (std::min)(m_width - 1, (std::max)({v0.x, v1.x, v2.x}));
    int yMin = (std::max)(0, (std::min)({v0.y, v1.y, v2.y}));
    int yMax = (std::min)(m_height - 1, (std::max)({v0.y, v1.y, v2.y}));
    if (xMin > xMax || yMin > yMax) return;

    // Barycentric denominator
    double denom = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
    if (fabs(denom) < 1e-12) return;
    double invDenom = 1.0 / denom;

    for (int y = yMin; y <= yMax; y++) {
        for (int x = xMin; x <= xMax; x++) {
            // Barycentric coordinates
            double alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) * invDenom;
            double beta  = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) * invDenom;
            double gamma = 1.0 - alpha - beta;
            if (alpha < -1e-8 || beta < -1e-8 || gamma < -1e-8) continue;

            // Depth interpolation
            double z = alpha * v0.z + beta * v1.z + gamma * v2.z;
            if (z < 0 || z >= m_zBuffer[y][x]) continue;
            m_zBuffer[y][x] = z;

            CRGB color;
            if (m_mode == TEXTURE && m_pTexture && m_pTexture->IsLoaded()) {
                double u = alpha * v0.u + beta * v1.u + gamma * v2.u;
                double v = alpha * v0.v + beta * v1.v + gamma * v2.v;
                color = m_pTexture->Sample(u, v);
            } else {
                color.r = alpha * v0.c.r + beta * v1.c.r + gamma * v2.c.r;
                color.g = alpha * v0.c.g + beta * v1.c.g + gamma * v2.c.g;
                color.b = alpha * v0.c.b + beta * v1.c.b + gamma * v2.c.b;
            }
            color.Normalize();
            pDC->SetPixel(x, y, color.ToCOLORREF());
        }
    }
}
