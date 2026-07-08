#pragma once
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "Projection.h"
#include "LightSource.h"
#include "Material.h"
#include "Lighting.h"
#include "Texture.h"

class CRealisticRendererDoc : public CDocument {
public:
    CRealisticRendererDoc();
    virtual ~CRealisticRendererDoc();

    CMesh* GetMesh() { return &m_mesh; }
    CRenderer* GetRenderer() { return &m_renderer; }
    CCamera* GetCamera() { return &m_camera; }
    CProjection* GetProjection() { return &m_projection; }
    CLightSource* GetLight() { return &m_light; }
    CMaterial* GetMaterial() { return &m_material; }
    CTexture* GetTexture() { return &m_texture; }

    bool LoadModel(const wchar_t* objPath);
    bool LoadTexture(const wchar_t* texPath);
    void ApplySimplification(int gridSize);
    void SetAnimRotation(float rx, float ry, float rz);

    DECLARE_DYNCREATE(CRealisticRendererDoc)
    DECLARE_MESSAGE_MAP()

private:
    CMesh m_mesh;
    CRenderer m_renderer;
    CCamera m_camera;
    CProjection m_projection;
    CLightSource m_light;
    CMaterial m_material;
    CTexture m_texture;
};
