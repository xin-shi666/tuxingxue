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
        DrawLine(pDC, x0, y0, x1, y1, RGB(100, 200, 255));
        DrawLine(pDC, x1, y1, x2, y2, RGB(100, 200, 255));
        DrawLine(pDC, x2, y2, x0, y0, RGB(100, 200, 255));
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
        CP3 faceCenter((wp0.x + wp1.x + wp2.x) / 3, (wp0.y + wp1.y + wp2.y) / 3, (wp0.z + wp1.z + wp2.z) / 3);
        CVector3 e1(wp1.x - wp0.x, wp1.y - wp0.y, wp1.z - wp0.z);
        CVector3 e2(wp2.x - wp0.x, wp2.y - wp0.y, wp2.z - wp0.z);
        CVector3 fn = e1.Cross(e2).Normalize();
        CRGB faceColor = m_pLighting->Illuminate(m_ViewPoint, faceCenter, fn, m_pMaterial);
        s0.c = s1.c = s2.c = faceColor;
    } else if (m_mode == GOURAUD) {
        s0.c = m_pLighting->Illuminate(m_ViewPoint, wp0, n0, m_pMaterial);
        s1.c = m_pLighting->Illuminate(m_ViewPoint, wp1, n1, m_pMaterial);
        s2.c = m_pLighting->Illuminate(m_ViewPoint, wp2, n2, m_pMaterial);
    }
    // TEXTURE mode: don't pre-compute colors, sample per-pixel

    SortVertices(s0, s1, s2);
    FillTriangle(pDC, s0, s1, s2);
}

void CZBuffer::FillTriangle(CDC* pDC, const CPoint2& s0, const CPoint2& s1, const CPoint2& s2) {
    int yMin = std::max(0, s0.y);
    int yMax = std::min(m_height - 1, s2.y);
    if (yMin > yMax) return;

    int totalLines = yMax - yMin + 1;
    double* spanLeft = new double[totalLines * 4];   // x, z, data
    double* spanRight = new double[totalLines * 4];
    int* spanLeftScr = new int[totalLines];
    int* spanRightScr = new int[totalLines];

    // Left and right spans for top-to-bottom
    // top half: s0->s1 (left), s0->s2 (right) or vice versa
    // bottom half: s1->s2 (left), s0->s2 (right) or vice versa

    for (int i = 0; i < totalLines; i++) { spanLeft[i * 4] = 1e30; spanRight[i * 4] = -1e30; }

    // Compute spans using edge walking (simplified: use inverse slope)
    auto computeEdge = [&](const CPoint2& a, const CPoint2& b, bool isLeft) {
        int yA = std::max(yMin, a.y), yB = std::min(yMax, b.y);
        if (yA > yB) return;
        double dy = b.y - a.y;
        if (fabs(dy) < 0.001) return;
        double invDy = 1.0 / dy;
        for (int y = yA; y <= yB; y++) {
            double t = (y - a.y) * invDy;
            int idx = y - yMin;
            double x = a.x + (b.x - a.x) * t;
            double z = a.z + (b.z - a.z) * t;
            if (isLeft) {
                if (x < spanLeft[idx * 4] || spanLeft[idx * 4] > 1e29) {
                    spanLeft[idx * 4] = x;
                    spanLeft[idx * 4 + 1] = z;
                    spanLeft[idx * 4 + 2] = t;
                    spanLeftScr[idx] = (&a == &s0) ? 0 : ((&a == &s1) ? 1 : 2);
                }
            } else {
                if (x > spanRight[idx * 4] || spanRight[idx * 4] < -1e29) {
                    spanRight[idx * 4] = x;
                    spanRight[idx * 4 + 1] = z;
                    spanRight[idx * 4 + 2] = t;
                    spanRightScr[idx] = (&a == &s0) ? 0 : ((&a == &s1) ? 1 : 2);
                }
            }
        }
    };

    // Determine which edge is left/right for top half
    double dx01 = s1.x - s0.x, dy01 = s1.y - s0.y;
    double dx02 = s2.x - s0.x, dy02 = s2.y - s0.y;
    bool topLeft01 = (dx01 * dy02 - dx02 * dy01) < 0; // cross product sign

    if (topLeft01) {
        computeEdge(s0, s1, true);
        computeEdge(s0, s2, false);
    } else {
        computeEdge(s0, s2, true);
        computeEdge(s0, s1, false);
    }

    // Bottom half
    computeEdge(s1, s2, topLeft01 ? true : false);
    if (!topLeft01) computeEdge(s1, s2, false); // need to overwrite

    // Also need opposite edges for bottom
    double dx12 = s2.x - s1.x, dy12 = s2.y - s1.y;
    double dx02b = s2.x - s0.x, dy02b = s2.y - s0.y;
    bool botLeft12 = (dx12 * dy02b - dx02b * dy12) < 0;

    // Recompute bottom half correctly
    for (int i = 0; i < totalLines; i++) { spanLeft[i * 4] = 1e30; spanRight[i * 4] = -1e30; }

    // Use a simpler approach: for each scanline, compute left/right intersection with all 3 edges
    for (int i = 0; i < totalLines; i++) spanLeft[i * 4] = 1e30;
    for (int i = 0; i < totalLines; i++) spanRight[i * 4] = -1e30;

    auto addEdge = [&](const CPoint2& a, const CPoint2& b) {
        int yA = a.y, yB = b.y;
        if (yA > yB) { addEdge(b, a); return; }
        if (yA >= m_height || yB < 0) return;
        double dy = yB - yA;
        if (fabs(dy) < 0.001) return;
        double invDy = 1.0 / dy;
        int yStart = std::max(yMin, yA);
        int yEnd = std::min(yMax, yB);
        for (int y = yStart; y <= yEnd; y++) {
            double t = (y - a.y) * invDy;
            int idx = y - yMin;
            double x = a.x + (b.x - a.x) * t;
            double z = a.z + (b.z - a.z) * t;
            if (x < spanLeft[idx * 4]) {
                spanLeft[idx * 4] = x;
                spanLeft[idx * 4 + 1] = z;
                spanLeft[idx * 4 + 2] = t;
                spanLeft[idx * 4 + 3] = (&a == &s0 && &b == &s1) ? 0.0 : ((&a == &s1 && &b == &s2) ? 1.0 : 2.0);
                spanLeftScr[idx] = (&a == &s0) ? 0 : ((&a == &s1) ? 1 : 2);
            }
            if (x > spanRight[idx * 4]) {
                spanRight[idx * 4] = x;
                spanRight[idx * 4 + 1] = z;
                spanRight[idx * 4 + 2] = t;
                spanRight[idx * 4 + 3] = (&a == &s0 && &b == &s1) ? 0.0 : ((&a == &s1 && &b == &s2) ? 1.0 : 2.0);
                spanRightScr[idx] = (&a == &s0) ? 0 : ((&a == &s1) ? 1 : 2);
            }
        }
    };

    addEdge(s0, s1); addEdge(s1, s2); addEdge(s0, s2);

    // Rasterize each scanline
    for (int i = 0; i < totalLines; i++) {
        int y = yMin + i;
        int xL = (int)ceil(spanLeft[i * 4]);
        int xR = (int)floor(spanRight[i * 4]);
        if (xL > xR) continue;
        xL = std::max(0, xL);
        xR = std::min(m_width - 1, xR);
        if (xL > xR) continue;

        double dx = xR - xL + 1.0;
        for (int x = xL; x <= xR; x++) {
            double t = (dx > 0.001) ? (x - xL) / dx : 0;
            double z = spanLeft[i * 4 + 1] + (spanRight[i * 4 + 1] - spanLeft[i * 4 + 1]) * t;

            if (z < m_zBuffer[y][x]) {
                m_zBuffer[y][x] = z;

                // Interpolate attributes
                double tL = spanLeft[i * 4 + 2];
                double tR = spanRight[i * 4 + 2];

                if (m_mode == TEXTURE && m_pTexture && m_pTexture->IsLoaded()) {
                    // Perspective-correct texture interpolation
                    double uL = s0.u + (s1.u - s0.u) * tL; // approximate for left edge
                    double uR = s0.u + (s2.u - s0.u) * tR;
                    double vL = s0.v + (s1.v - s0.v) * tL;
                    double vR = s0.v + (s2.v - s0.v) * tR;
                    double u = uL + (uR - uL) * t;
                    double v = vL + (vR - vL) * t;
                    CRGB texColor = m_pTexture->Sample(u, v);

                    // Simple diffuse lighting on texture
                    CVector3 nL = s0.n + (s1.n - s0.n) * tL;
                    CVector3 nR = s0.n + (s2.n - s0.n) * tR;
                    CVector3 N = nL + (nR - nL) * t;
                    N = N.Normalize();

                    CVector3 L(200, 300, 400); // light direction
                    L = L.Normalize();
                    double NdotL = std::max(0.0, N.Dot(L));
                    double lighting = 0.3 + 0.7 * NdotL;
                    texColor = texColor * lighting;
                    texColor.Normalize();
                    pDC->SetPixel(x, y, texColor.ToCOLORREF());
                } else if (m_mode == GOURAUD || m_mode == FLAT) {
                    CRGB cL = s0.c + (s1.c - s0.c) * tL;
                    CRGB cR = s0.c + (s2.c - s0.c) * tR;
                    CRGB c = cL + (cR - cL) * t;
                    c.Normalize();
                    pDC->SetPixel(x, y, c.ToCOLORREF());
                } else {
                    // Per-pixel Phong (for future use)
                    CVector3 nL = s0.n + (s1.n - s0.n) * tL;
                    CVector3 nR = s0.n + (s2.n - s0.n) * tR;
                    CVector3 N = nL + (nR - nL) * t;
                    N = N.Normalize();

                    CP3 wpL = s0.worldPos + (s1.worldPos - s0.worldPos) * tL;
                    CP3 wpR = s0.worldPos + (s2.worldPos - s0.worldPos) * tR;
                    CP3 wp = wpL + (wpR - wpL) * t;

                    CRGB c = m_pLighting->Illuminate(m_ViewPoint, wp, N, m_pMaterial);
                    pDC->SetPixel(x, y, c.ToCOLORREF());
                }
            }
        }
    }

    delete[] spanLeft; delete[] spanRight;
    delete[] spanLeftScr; delete[] spanRightScr;
}
