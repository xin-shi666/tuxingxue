#pragma once
#include "CP3.h"
#include "CVector3.h"
#include "CRGB.h"
#include "CLighting.h"
#include "CMaterial.h"
#include "CTexture.h"

struct CPoint2 {
    int x, y;
    double z;             // depth (1/z for perspective correct)
    CRGB c;               // interpolated color
    CVector3 n;           // interpolated normal (world space)
    double u, v;          // texcoord
    CP3 worldPos;         // world position for per-pixel lighting
};

class CZBuffer {
public:
    enum Mode { WIREFRAME, FLAT, GOURAUD, TEXTURE };

    CZBuffer();
    ~CZBuffer();

    void InitBuffer(int w, int h);
    void ClearBuffer();

    void SetLighting(CLighting* pLight) { m_pLighting = pLight; }
    void SetMaterial(CMaterial* pMat) { m_pMaterial = pMat; }
    void SetTexture(CTexture* pTex) { m_pTexture = pTex; }
    void SetViewPoint(const CP3& vp) { m_ViewPoint = vp; }
    void SetMode(Mode m) { m_mode = m; }
    Mode GetMode() const { return m_mode; }

    // Draw a triangle
    void DrawTriangle(CDC* pDC, const CP3& p0, const CP3& p1, const CP3& p2,
                      const CVector3& n0, const CVector3& n1, const CVector3& n2,
                      const CP3& wp0, const CP3& wp1, const CP3& wp2,
                      double u0, double v0, double u1, double v1, double u2, double v2);

    // Draw a line
    void DrawLine(CDC* pDC, int x0, int y0, int x1, int y1, COLORREF color);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    void FillTriangle(CDC* pDC, const CPoint2& s0, const CPoint2& s1, const CPoint2& s2);
    void SortVertices(CPoint2& v0, CPoint2& v1, CPoint2& v2);
    void EdgeFlag(CPoint2& start, CPoint2& end, double* span, int* spanScreen, int yStart, int yEnd, int totalLines);

    double** m_zBuffer;
    int m_width, m_height;
    Mode m_mode;
    CP3 m_ViewPoint;
    CLighting* m_pLighting;
    CMaterial* m_pMaterial;
    CTexture* m_pTexture;
};
