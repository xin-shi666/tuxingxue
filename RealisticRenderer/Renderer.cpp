#include "stdafx.h"
#include "Renderer.h"
#include <algorithm>
#include <cmath>

CRenderer::CRenderer() : m_buffer(nullptr), m_width(0), m_height(0), m_pitch(0),
    m_mode(RM_WIREFRAME), m_camera(nullptr), m_projection(nullptr),
    m_light(nullptr), m_material(nullptr), m_texture(nullptr), m_mesh(nullptr),
    m_rotX(0), m_rotY(0), m_rotZ(0) {}

CRenderer::~CRenderer() {
    delete[] m_buffer;
}

void CRenderer::Init(int width, int height) {
    delete[] m_buffer;
    m_width = width;
    m_height = height;
    m_pitch = width * 4; // RGBA
    m_buffer = new unsigned char[m_pitch * height];
    m_zbuffer.Init(width, height);
}

void CRenderer::Resize(int width, int height) {
    Init(width, height);
}

void CRenderer::Clear(unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < m_width * m_height; i++) {
        m_buffer[i * 4 + 0] = b;
        m_buffer[i * 4 + 1] = g;
        m_buffer[i * 4 + 2] = r;
        m_buffer[i * 4 + 3] = 255;
    }
    m_zbuffer.Clear();
}

void CRenderer::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
    int idx = y * m_pitch + x * 4;
    m_buffer[idx + 0] = b;
    m_buffer[idx + 1] = g;
    m_buffer[idx + 2] = r;
    m_buffer[idx + 3] = 255;
}

// Bresenham line
void CRenderer::DrawLine(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        SetPixel(x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void CRenderer::Render() {
    if (!m_buffer || !m_mesh || !m_camera || !m_projection) return;

    // Build model matrix (rotation + scale to fit)
    CTransform matWorld;
    CTransform rotX, rotY, rotZ;
    rotX.SetRotationX(m_rotX);
    rotY.SetRotationY(m_rotY);
    rotZ.SetRotationZ(m_rotZ);
    float s = 2.0f / (m_mesh->GetRadius() + 0.001f);
    CTransform matScale;
    matScale.SetScale(s, s, s);
    matWorld = matScale * rotZ * rotY * rotX;

    // Get view and projection matrices
    CTransform matView = m_camera->GetViewMatrix();
    CTransform matProj = m_projection->GetProjectionMatrix();
    CTransform matViewport = m_projection->GetViewportMatrix();
    float zoom = m_projection->GetZoom();
    matViewport(0, 0) *= zoom;
    matViewport(1, 1) *= zoom;

    // Composite transform: World -> View -> Projection
    CTransform matWVP = matWorld * matView * matProj;

    // Pre-compute world*view for normal transform
    CTransform matWV = matWorld * matView;

    // Transform all vertices
    size_t vcount = m_mesh->GetVertexCount();
    size_t fcount = m_mesh->GetFaceCount();
    std::vector<TransformedVertex> tverts(vcount);

    const float* pos = m_mesh->GetPositions();
    const float* nrm = m_mesh->HasNormals() ? m_mesh->GetNormals() : nullptr;
    const float* tex = m_mesh->HasTexcoords() ? m_mesh->GetTexcoords() : nullptr;

    Vector4 viewPos = m_camera->GetPosition();

    for (size_t i = 0; i < vcount; i++) {
        Vector4 wp(pos[i*3], pos[i*3+1], pos[i*3+2]);
        Vector4 vp = matWVP.TransformPoint(wp);

        tverts[i].wx = 0; tverts[i].wy = 0; tverts[i].wz = 0;
        tverts[i].nx = 0; tverts[i].ny = 0; tverts[i].nz = 0;
        tverts[i].u = 0; tverts[i].v = 0;
        tverts[i].litR = tverts[i].litG = tverts[i].litB = 1.0f;

        if (vp.w > 0.0001f) {
            float invW = 1.0f / vp.w;
            tverts[i].rhw = invW;
            Vector4 ndc(vp.x * invW, vp.y * invW, vp.z * invW, 1);
            Vector4 sc = matViewport.TransformPoint(ndc);
            tverts[i].sx = sc.x;
            tverts[i].sy = sc.y;
            tverts[i].sz = sc.z;

            // World position for lighting
            tverts[i].wx = wp.x;
            tverts[i].wy = wp.y;
            tverts[i].wz = wp.z;

            // World normal
            if (nrm) {
                Vector4 n(nrm[i*3], nrm[i*3+1], nrm[i*3+2], 0);
                n = matWV.TransformNormal(n);
                n = n.Normalize();
                tverts[i].nx = n.x;
                tverts[i].ny = n.y;
                tverts[i].nz = n.z;
            }

            // Texcoord
            if (tex) {
                tverts[i].u = tex[i*2];
                tverts[i].v = tex[i*2+1];
            }
        } else {
            tverts[i].sx = -10000; tverts[i].sy = -10000; tverts[i].sz = 1;
            tverts[i].rhw = 0;
        }
    }

    // Compute per-vertex lighting if Gourand mode
    if (m_mode == RM_GOURAUD) {
        for (size_t i = 0; i < vcount; i++) {
            if (tverts[i].rhw > 0) {
                Vector4 wpos(tverts[i].wx, tverts[i].wy, tverts[i].wz);
                Vector4 norm(tverts[i].nx, tverts[i].ny, tverts[i].nz, 0);
                m_lighting.ComputeVertexColor(wpos, norm, viewPos, *m_light, *m_material,
                    tverts[i].litR, tverts[i].litG, tverts[i].litB);
            }
        }
    }

    // Render according to mode
    if (m_mode == RM_WIREFRAME) {
        DrawWireframe(tverts);
    } else if (m_mode == RM_FLAT || m_mode == RM_GOURAUD) {
        DrawFilled(tverts, false);
    } else if (m_mode == RM_TEXTURE) {
        DrawFilled(tverts, true);
    }
}

void CRenderer::DrawWireframe(const std::vector<TransformedVertex>& verts) {
    const Face* faces = m_mesh->GetFaces();
    size_t fcount = m_mesh->GetFaceCount();

    for (size_t i = 0; i < fcount; i++) {
        const Face& f = faces[i];
        const auto& v0 = verts[f.v[0]];
        const auto& v1 = verts[f.v[1]];
        const auto& v2 = verts[f.v[2]];

        if (v0.rhw <= 0 || v1.rhw <= 0 || v2.rhw <= 0) continue;

        int x0 = (int)v0.sx, y0 = (int)v0.sy;
        int x1 = (int)v1.sx, y1 = (int)v1.sy;
        int x2 = (int)v2.sx, y2 = (int)v2.sy;

        if ((x0 < 0 && x1 < 0 && x2 < 0) || (x0 >= m_width && x1 >= m_width && x2 >= m_width)) continue;
        if ((y0 < 0 && y1 < 0 && y2 < 0) || (y0 >= m_height && y1 >= m_height && y2 >= m_height)) continue;

        DrawLine(x0, y0, x1, y1, 120, 200, 255);
        DrawLine(x1, y1, x2, y2, 120, 200, 255);
        DrawLine(x2, y2, x0, y0, 120, 200, 255);
    }
}

// Sort 3 vertices by Y
static void SortVertsByY(const CRenderer::TransformedVertex*& a,
                          const CRenderer::TransformedVertex*& b,
                          const CRenderer::TransformedVertex*& c) {
    if (a->sy > b->sy) std::swap(a, b);
    if (a->sy > c->sy) std::swap(a, c);
    if (b->sy > c->sy) std::swap(b, c);
}

void CRenderer::ProcessTriangle(const TransformedVertex& v0,
                                const TransformedVertex& v1,
                                const TransformedVertex& v2, bool useTexture) {
    // Back-face culling: signed area in screen space
    float area = (v1.sx - v0.sx) * (v2.sy - v0.sy) - (v2.sx - v0.sx) * (v1.sy - v0.sy);
    if (area <= 0) return; // cull backfaces

    const TransformedVertex *ptop, *pmid, *pbot;
    ptop = &v0; pmid = &v1; pbot = &v2;
    SortVertsByY(ptop, pmid, pbot);

    int yTop = (int)ceilf(ptop->sy);
    int yMid = (int)ceilf(pmid->sy);
    int yBot = (int)ceilf(pbot->sy);

    if (yTop >= m_height || yBot < 0) return;
    yTop = std::max(0, yTop);
    yBot = std::min(m_height - 1, yBot);

    float invDY1 = (pmid->sy - ptop->sy);
    float invDY2 = (pbot->sy - ptop->sy);
    float invDY3 = (pbot->sy - pmid->sy);

    // Process top half
    for (int y = yTop; y < yMid && y <= yBot; y++) {
        float t1 = (invDY1 > 0.0001f) ? (y - ptop->sy) / invDY1 : 0;
        float t2 = (invDY2 > 0.0001f) ? (y - ptop->sy) / invDY2 : 0;

        Span span;
        float xA = ptop->sx + (pmid->sx - ptop->sx) * t1;
        float xB = ptop->sx + (pbot->sx - ptop->sx) * t2;
        if (xA > xB) {
            std::swap(xA, xB);
            std::swap(t1, t2);
        }
        span.xLeft = std::max(0, (int)ceilf(xA));
        span.xRight = std::min(m_width - 1, (int)floorf(xB));

        span.zLeft = ptop->sz + (pmid->sz - ptop->sz) * t1;
        span.zRight = ptop->sz + (pbot->sz - ptop->sz) * t2;

        span.rLeft = ptop->litR + (pmid->litR - ptop->litR) * t1;
        span.rRight = ptop->litR + (pbot->litR - ptop->litR) * t2;
        span.gLeft = ptop->litG + (pmid->litG - ptop->litG) * t1;
        span.gRight = ptop->litG + (pbot->litG - ptop->litG) * t2;
        span.bLeft = ptop->litB + (pmid->litB - ptop->litB) * t1;
        span.bRight = ptop->litB + (pbot->litB - ptop->litB) * t2;

        if (useTexture) {
            span.uLeft = ptop->u + (pmid->u - ptop->u) * t1;
            span.vLeft = ptop->v + (pmid->v - ptop->v) * t1;
            span.uRight = ptop->u + (pbot->u - ptop->u) * t2;
            span.vRight = ptop->v + (pbot->v - ptop->v) * t2;
            span.rhwLeft = ptop->rhw + (pmid->rhw - ptop->rhw) * t1;
            span.rhwRight = ptop->rhw + (pbot->rhw - ptop->rhw) * t2;
        }

        RasterizeSpan(y, span, useTexture);
    }

    // Process bottom half
    for (int y = std::max(yMid, yTop); y <= yBot; y++) {
        float t2 = (invDY2 > 0.0001f) ? (y - ptop->sy) / invDY2 : 1;
        float t3 = (invDY3 > 0.0001f) ? (y - pmid->sy) / invDY3 : 1;

        Span span;
        float xA = pmid->sx + (pbot->sx - pmid->sx) * t3;
        float xB = ptop->sx + (pbot->sx - ptop->sx) * t2;
        if (xA > xB) {
            std::swap(xA, xB);
            std::swap(t3, t2);
        }
        span.xLeft = std::max(0, (int)ceilf(xA));
        span.xRight = std::min(m_width - 1, (int)floorf(xB));

        span.zLeft = pmid->sz + (pbot->sz - pmid->sz) * t3;
        span.zRight = ptop->sz + (pbot->sz - ptop->sz) * t2;

        span.rLeft = pmid->litR + (pbot->litR - pmid->litR) * t3;
        span.rRight = ptop->litR + (pbot->litR - ptop->litR) * t2;
        span.gLeft = pmid->litG + (pbot->litG - pmid->litG) * t3;
        span.gRight = ptop->litG + (pbot->litG - ptop->litG) * t2;
        span.bLeft = pmid->litB + (pbot->litB - pmid->litB) * t3;
        span.bRight = ptop->litB + (pbot->litB - ptop->litB) * t2;

        if (useTexture) {
            span.uLeft = pmid->u + (pbot->u - pmid->u) * t3;
            span.vLeft = pmid->v + (pbot->v - pmid->v) * t3;
            span.uRight = ptop->u + (pbot->u - ptop->u) * t2;
            span.vRight = ptop->v + (pbot->v - ptop->v) * t2;
            span.rhwLeft = pmid->rhw + (pbot->rhw - pmid->rhw) * t3;
            span.rhwRight = ptop->rhw + (pbot->rhw - ptop->rhw) * t2;
        }

        RasterizeSpan(y, span, useTexture);
    }
}

void CRenderer::RasterizeSpan(int y, const Span& span, bool useTexture) {
    if (span.xLeft > span.xRight) return;

    float invDx = 1.0f / (span.xRight - span.xLeft + 0.0001f);

    for (int x = span.xLeft; x <= span.xRight; x++) {
        float t = (x - span.xLeft) * invDx;
        float z = span.zLeft + (span.zRight - span.zLeft) * t;

        if (!m_zbuffer.TestAndSet(x, y, z)) continue;

        float r, g, b;
        if (useTexture && m_texture && m_texture->IsLoaded()) {
            // Perspective-correct texture mapping
            float rhw = span.rhwLeft + (span.rhwRight - span.rhwLeft) * t;
            float w = 1.0f / (rhw + 0.0001f);
            float u_interp = span.uLeft + (span.uRight - span.uLeft) * t;
            float v_interp = span.vLeft + (span.vRight - span.vLeft) * t;
            float u = u_interp * w;
            float v = v_interp * w;

            unsigned char tr, tg, tb;
            m_texture->Sample(u, v, tr, tg, tb);
            r = tr / 255.0f;
            g = tg / 255.0f;
            b = tb / 255.0f;
        } else {
            r = span.rLeft + (span.rRight - span.rLeft) * t;
            g = span.gLeft + (span.gRight - span.gLeft) * t;
            b = span.bLeft + (span.bRight - span.bLeft) * t;
        }

        CLighting::Clamp(r, g, b);
        SetPixel(x, y, (unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255));
    }
}

void CRenderer::DrawFilled(const std::vector<TransformedVertex>& verts, bool useTexture) {
    const Face* faces = m_mesh->GetFaces();
    size_t fcount = m_mesh->GetFaceCount();

    // For flat shading, compute face color once
    Vector4 viewPos = m_camera->GetPosition();

    for (size_t i = 0; i < fcount; i++) {
        const Face& f = faces[i];
        const auto& tv0 = verts[f.v[0]];
        const auto& tv1 = verts[f.v[1]];
        const auto& tv2 = verts[f.v[2]];

        if (tv0.rhw <= 0 || tv1.rhw <= 0 || tv2.rhw <= 0) continue;

        if ((tv0.sx < -100 && tv1.sx < -100 && tv2.sx < -100) ||
            (tv0.sy < -100 && tv1.sy < -100 && tv2.sy < -100)) continue;

        TransformedVertex v0 = tv0, v1 = tv1, v2 = tv2;

        if (m_mode == RM_FLAT) {
            // Compute face normal and face center for flat shading
            Vector4 wp0(v0.wx, v0.wy, v0.wz);
            Vector4 wp1(v1.wx, v1.wy, v1.wz);
            Vector4 wp2(v2.wx, v2.wy, v2.wz);
            Vector4 facePos((wp0.x+wp1.x+wp2.x)/3, (wp0.y+wp1.y+wp2.y)/3, (wp0.z+wp1.z+wp2.z)/3);
            Vector4 e1 = wp1 - wp0, e2 = wp2 - wp0;
            Vector4 fn = e1.Cross(e2).Normalize();

            float lr, lg, lb;
            m_lighting.ComputeVertexColor(facePos, fn, viewPos, *m_light, *m_material, lr, lg, lb);
            v0.litR = v1.litR = v2.litR = lr;
            v0.litG = v1.litG = v2.litG = lg;
            v0.litB = v1.litB = v2.litB = lb;
        }

        ProcessTriangle(v0, v1, v2, useTexture);
    }
}
