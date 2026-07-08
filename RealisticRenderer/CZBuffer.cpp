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
    for (int i = 0; i < m_height; i++) {
        m_zBuffer[i] = new double[m_width];
    }
}

void CZBuffer::ClearBuffer() {
    for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++)
            m_zBuffer[y][x] = 1e30;
}

void CZBuffer::DrawLine(CDC* pDC, int x0, int y0, int x1, int y1, COLORREF color) {
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        if (x0 >= 0 && x0 < m_width && y0 >= 0 && y0 < m_height)
            pDC->SetPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
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
        CPen pen(PS_SOLID, 1, RGB(100, 200, 255));
        CPen* oldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(x0, y0); pDC->LineTo(x1, y1);
        pDC->LineTo(x2, y2); pDC->LineTo(x0, y0);
        pDC->SelectObject(oldPen);
        return;
    }

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
        CRGB fc2 = m_pLighting->Illuminate(m_ViewPoint, fc, fn, m_pMaterial);
        s0.c = s1.c = s2.c = fc2;
    } else if (m_mode == GOURAUD) {
        s0.c = m_pLighting->Illuminate(m_ViewPoint, wp0, n0, m_pMaterial);
        s1.c = m_pLighting->Illuminate(m_ViewPoint, wp1, n1, m_pMaterial);
        s2.c = m_pLighting->Illuminate(m_ViewPoint, wp2, n2, m_pMaterial);
    }

    SortVertices(s0, s1, s2);
    FillTriangle(pDC, s0, s1, s2);
}

static void AddSpanEdge(int yMin, int yMax, const CPoint2& a, const CPoint2& b,
                        double* spanL, double* spanR, int totalLines) {
    int yA = a.y, yB = b.y;
    if (yA > yB) { AddSpanEdge(yMin, yMax, b, a, spanL, spanR, totalLines); return; }
    if (yA >= yMax || yB < yMin) return;
    double dy = (double)(yB - yA);
    if (fabs(dy) < 0.001) return;
    double invDy = 1.0 / dy;
    int ys = (std::max)(yMin, yA);
    int ye = (std::min)(yMax, yB);
    for (int y = ys; y <= ye; y++) {
        double t = (y - a.y) * invDy;
        int idx = y - yMin;
        double x = a.x + (b.x - a.x) * t;
        double z = a.z + (b.z - a.z) * t;
        if (x < spanL[idx * 4]) {
            spanL[idx * 4] = x;
            spanL[idx * 4 + 1] = z;
            spanL[idx * 4 + 2] = t;
        }
        if (x > spanR[idx * 4]) {
            spanR[idx * 4] = x;
            spanR[idx * 4 + 1] = z;
            spanR[idx * 4 + 2] = t;
        }
    }
}

void CZBuffer::FillTriangle(CDC* pDC, const CPoint2& s0, const CPoint2& s1, const CPoint2& s2) {
    int yMin = (std::max)(0, s0.y);
    int yMax = (std::min)(m_height - 1, s2.y);
    if (yMin > yMax) return;

    int totalLines = yMax - yMin + 1;
    double* spanL = new double[totalLines * 4];
    double* spanR = new double[totalLines * 4];

    for (int i = 0; i < totalLines; i++) {
        spanL[i * 4] = 1e30; spanR[i * 4] = -1e30;
    }

    AddSpanEdge(yMin, yMax, s0, s1, spanL, spanR, totalLines);
    AddSpanEdge(yMin, yMax, s1, s2, spanL, spanR, totalLines);
    AddSpanEdge(yMin, yMax, s0, s2, spanL, spanR, totalLines);

    for (int i = 0; i < totalLines; i++) {
        int y = yMin + i;
        int xL = (int)ceil(spanL[i * 4]);
        int xR = (int)floor(spanR[i * 4]);
        if (xL > xR) continue;
        xL = (std::max)(0, xL);
        xR = (std::min)(m_width - 1, xR);
        if (xL > xR) continue;

        double dx = (double)(xR - xL);
        if (dx < 0.001) dx = 1.0;

        for (int x = xL; x <= xR; x++) {
            double t = (x - xL) / dx;
            double z = spanL[i * 4 + 1] + (spanR[i * 4 + 1] - spanL[i * 4 + 1]) * t;
            if (z >= m_zBuffer[y][x]) continue;
            m_zBuffer[y][x] = z;

            double tL = spanL[i * 4 + 2];
            double tR = spanR[i * 4 + 2];

            if (m_mode == TEXTURE && m_pTexture && m_pTexture->IsLoaded()) {
                double u = s0.u + (s2.u - s0.u) * (tL + (tR - tL) * t);
                double v = s0.v + (s2.v - s0.v) * (tL + (tR - tL) * t);
                CRGB tc = m_pTexture->Sample(u, v);
                tc.Normalize();
                pDC->SetPixel(x, y, tc.ToCOLORREF());
            } else {
                CRGB cL = s0.c + (s2.c - s0.c) * tL;
                CRGB cR = s0.c + (s2.c - s0.c) * tR;
                CRGB c = cL + (cR - cL) * t;
                c.Normalize();
                pDC->SetPixel(x, y, c.ToCOLORREF());
            }
        }
    }

    delete[] spanL; delete[] spanR;
}
