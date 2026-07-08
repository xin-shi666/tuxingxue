#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Projection.h"
#include "ZBuffer.h"
#include "LightSource.h"
#include "Material.h"
#include "Lighting.h"
#include "Texture.h"
#include "Transform.h"

class CRenderer {
public:
    enum RenderMode { RM_WIREFRAME, RM_FLAT, RM_GOURAUD, RM_TEXTURE };

    CRenderer();
    ~CRenderer();

    void Init(int width, int height);
    void Resize(int width, int height);
    void Clear(unsigned char r = 40, unsigned char g = 40, unsigned char b = 44);

    void SetRenderMode(RenderMode mode) { m_mode = mode; }
    RenderMode GetRenderMode() const { return m_mode; }

    void SetCamera(CCamera* cam) { m_camera = cam; }
    void SetProjection(CProjection* proj) { m_projection = proj; }
    void SetLight(CLightSource* light) { m_light = light; }
    void SetMaterial(CMaterial* mat) { m_material = mat; }
    void SetTexture(CTexture* tex) { m_texture = tex; }
    void SetMesh(CMesh* mesh) { m_mesh = mesh; }

    void SetModelRotation(float rx, float ry, float rz) { m_rotX = rx; m_rotY = ry; m_rotZ = rz; }

    void Render();
    const unsigned char* GetBuffer() const { return m_buffer; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    // Per-vertex data after transformation
    struct TransformedVertex {
        float sx, sy, sz;       // screen position
        float wx, wy, wz;       // world position
        float nx, ny, nz;       // world normal
        float u, v;             // texcoord
        float rhw;              // 1/w for perspective correction
        float litR, litG, litB; // pre-computed lighting color
    };

    // Span for scanline rasterization
    struct Span {
        int xLeft, xRight;
        float zLeft, zRight;
        float rLeft, rRight, gLeft, gRight, bLeft;
        float uLeft, uRight, vLeft, vRight;
        float rhwLeft, rhwRight;
    };

    void DrawWireframe(const std::vector<TransformedVertex>& verts);
    void DrawFilled(const std::vector<TransformedVertex>& verts, bool useTexture);
    void DrawLine(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
    void ProcessTriangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2, bool useTexture);
    void RasterizeSpan(int y, const Span& span, bool useTexture);
    void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

    unsigned char* m_buffer;
    int m_width, m_height;
    int m_pitch; // bytes per row (4 bytes per pixel)

    CZBuffer m_zbuffer;
    RenderMode m_mode;

    CCamera* m_camera;
    CProjection* m_projection;
    CLightSource* m_light;
    CMaterial* m_material;
    CTexture* m_texture;
    CMesh* m_mesh;
    CLighting m_lighting;

    float m_rotX, m_rotY, m_rotZ;
};
