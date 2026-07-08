#include "stdafx.h"
#include "RealisticRendererDoc.h"
#include "ObjLoader.h"

IMPLEMENT_DYNCREATE(CRealisticRendererDoc, CDocument)

BEGIN_MESSAGE_MAP(CRealisticRendererDoc, CDocument)
END_MESSAGE_MAP()

CRealisticRendererDoc::CRealisticRendererDoc() {
    m_camera.SetPosition(0, 0.5f, 3);
    m_camera.SetTarget(0, 0, 0);
    m_camera.SetDistance(3);
}

CRealisticRendererDoc::~CRealisticRendererDoc() {}

bool CRealisticRendererDoc::LoadModel(const wchar_t* objPath) {
    CObjLoader loader;
    CObjLoader::RawMesh raw;
    if (!loader.Load(objPath, raw)) return false;

    m_mesh.InitFromRaw(raw.positions, raw.texcoords, raw.faces, raw.hasTexcoords);
    m_mesh.ComputeNormals();

    // Center the camera on the mesh
    Vector4 center = m_mesh.GetCenter();
    float radius = m_mesh.GetRadius();
    m_camera.SetTarget(center.x, center.y, center.z);
    m_camera.SetDistance(radius * 2.5f);
    m_camera.SetPosition(center.x, center.y + radius * 0.5f, center.z + radius * 2);

    m_renderer.SetMesh(&m_mesh);
    m_renderer.SetCamera(&m_camera);
    m_renderer.SetProjection(&m_projection);
    m_renderer.SetLight(&m_light);
    m_renderer.SetMaterial(&m_material);
    m_renderer.SetTexture(&m_texture);

    return true;
}

bool CRealisticRendererDoc::LoadTexture(const wchar_t* texPath) {
    return m_texture.Load(texPath);
}

void CRealisticRendererDoc::ApplySimplification(int gridSize) {
    m_mesh.Simplify(gridSize);
    m_mesh.ComputeNormals();
}

void CRealisticRendererDoc::SetAnimRotation(float rx, float ry, float rz) {
    m_renderer.SetModelRotation(rx, ry, rz);
}
