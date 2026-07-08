#include "stdafx.h"
#include "ControlPanel.h"
#include "RealisticRendererDoc.h"
#include "RealisticRendererView.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CControlPanel, CDialogEx)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_SIMPLIFY_APPLY, OnApplySimplify)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CControlPanel::CControlPanel() : CDialogEx() {}
CControlPanel::~CControlPanel() {}

CRealisticRendererDoc* CControlPanel::GetDoc() {
    CFrameWnd* pFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
    if (!pFrame) return nullptr;
    return dynamic_cast<CRealisticRendererDoc*>(pFrame->GetActiveDocument());
}

void CControlPanel::CreateSlider(int id, int x, int y, int w, int h, int minVal, int maxVal, int initVal) {
    CSliderCtrl* pSlider = nullptr;
    switch (id) {
        case IDC_LIGHT_X: pSlider = &m_sliderLightX; break;
        case IDC_LIGHT_Y: pSlider = &m_sliderLightY; break;
        case IDC_LIGHT_Z: pSlider = &m_sliderLightZ; break;
        case IDC_AMBIENT_R: pSlider = &m_sliderAmbR; break;
        case IDC_AMBIENT_G: pSlider = &m_sliderAmbG; break;
        case IDC_AMBIENT_B: pSlider = &m_sliderAmbB; break;
        case IDC_DIFFUSE_R: pSlider = &m_sliderDifR; break;
        case IDC_DIFFUSE_G: pSlider = &m_sliderDifG; break;
        case IDC_DIFFUSE_B: pSlider = &m_sliderDifB; break;
        case IDC_SPECULAR_R: pSlider = &m_sliderSpecR; break;
        case IDC_SPECULAR_G: pSlider = &m_sliderSpecG; break;
        case IDC_SPECULAR_B: pSlider = &m_sliderSpecB; break;
        case IDC_SHININESS: pSlider = &m_sliderShininess; break;
        case IDC_ZOOM: pSlider = &m_sliderZoom; break;
        case IDC_ANIM_SPEED: pSlider = &m_sliderSpeed; break;
        case IDC_SIMPLIFY: pSlider = &m_sliderSimplify; break;
    }
    if (pSlider) {
        pSlider->Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
            CRect(x, y, x + w, y + h), this, id);
        pSlider->SetRange(minVal, maxVal);
        pSlider->SetPos(initVal);
    }
}

void CControlPanel::CreateLabel(const wchar_t* text, int x, int y, int w, int h) {
    CStatic* label = new CStatic();
    label->Create(text, WS_CHILD | WS_VISIBLE, CRect(x, y, x + w, y + h), this);
}

BOOL CControlPanel::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Use a minimal dialog resource that we created manually
    // Create controls programmatically
    int y = 8;

    CreateLabel(L"Render Mode (menu View):", 8, y, 200, 16); y += 20;
    CreateLabel(L"  Wireframe / Flat / Gouraud / Texture", 8, y, 220, 16); y += 24;

    CreateLabel(L"--- Light Position ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_LIGHT_X, 8, y, 200, 24, -100, 100, 50); CreateLabel(L"X", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_LIGHT_Y, 8, y, 200, 24, -100, 100, 50); CreateLabel(L"Y", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_LIGHT_Z, 8, y, 200, 24, -100, 100, 50); CreateLabel(L"Z", 212, y, 20, 20); y += 28;

    CreateLabel(L"--- Ambient ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_AMBIENT_R, 8, y, 200, 24, 0, 100, 20); CreateLabel(L"R", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_AMBIENT_G, 8, y, 200, 24, 0, 100, 20); CreateLabel(L"G", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_AMBIENT_B, 8, y, 200, 24, 0, 100, 20); CreateLabel(L"B", 212, y, 20, 20); y += 28;

    CreateLabel(L"--- Diffuse ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_DIFFUSE_R, 8, y, 200, 24, 0, 100, 80); CreateLabel(L"R", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_DIFFUSE_G, 8, y, 200, 24, 0, 100, 80); CreateLabel(L"G", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_DIFFUSE_B, 8, y, 200, 24, 0, 100, 80); CreateLabel(L"B", 212, y, 20, 20); y += 28;

    CreateLabel(L"--- Specular ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_SPECULAR_R, 8, y, 200, 24, 0, 100, 50); CreateLabel(L"R", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_SPECULAR_G, 8, y, 200, 24, 0, 100, 50); CreateLabel(L"G", 212, y, 20, 20); y += 26;
    CreateSlider(IDC_SPECULAR_B, 8, y, 200, 24, 0, 100, 50); CreateLabel(L"B", 212, y, 20, 20); y += 28;

    CreateLabel(L"--- Shininess ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_SHININESS, 8, y, 200, 24, 1, 128, 32); y += 28;

    CreateLabel(L"--- Camera Zoom ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_ZOOM, 8, y, 200, 24, 10, 300, 100); y += 28;

    CreateLabel(L"--- Animation Speed ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_ANIM_SPEED, 8, y, 200, 24, 1, 100, 30); y += 28;

    CreateLabel(L"--- Mesh Simplify Grid ---", 8, y, 200, 16); y += 18;
    CreateSlider(IDC_SIMPLIFY, 8, y, 160, 24, 5, 200, 100); y += 28;
    m_btnSimplify.Create(L"Apply Simplify", WS_CHILD | WS_VISIBLE, CRect(8, y, 120, y + 22), this, IDC_SIMPLIFY_APPLY);

    SyncControlsFromDoc();
    return TRUE;
}

void CControlPanel::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

void CControlPanel::SyncControlsFromDoc() {
    CRealisticRendererDoc* pDoc = GetDoc();
    if (!pDoc) return;

    CLightSource* light = pDoc->GetLight();
    m_sliderLightX.SetPos((int)(light->GetPosX() * 10));
    m_sliderLightY.SetPos((int)(light->GetPosY() * 10));
    m_sliderLightZ.SetPos((int)(light->GetPosZ() * 10));

    CMaterial* mat = pDoc->GetMaterial();
    m_sliderAmbR.SetPos((int)(mat->GetKaR() * 100));
    m_sliderAmbG.SetPos((int)(mat->GetKaG() * 100));
    m_sliderAmbB.SetPos((int)(mat->GetKaB() * 100));
    m_sliderDifR.SetPos((int)(mat->GetKdR() * 100));
    m_sliderDifG.SetPos((int)(mat->GetKdG() * 100));
    m_sliderDifB.SetPos((int)(mat->GetKdB() * 100));
    m_sliderSpecR.SetPos((int)(mat->GetKsR() * 100));
    m_sliderSpecG.SetPos((int)(mat->GetKsG() * 100));
    m_sliderSpecB.SetPos((int)(mat->GetKsB() * 100));
    m_sliderShininess.SetPos((int)mat->GetShininess());

    m_sliderZoom.SetPos((int)(pDoc->GetProjection()->GetZoom() * 100));
}

void CControlPanel::SyncControlsToDoc() {
    CRealisticRendererDoc* pDoc = GetDoc();
    if (!pDoc) return;

    CLightSource* light = pDoc->GetLight();
    light->SetPosition(
        m_sliderLightX.GetPos() * 0.1f,
        m_sliderLightY.GetPos() * 0.1f,
        m_sliderLightZ.GetPos() * 0.1f);

    CMaterial* mat = pDoc->GetMaterial();
    mat->SetAmbient(m_sliderAmbR.GetPos() * 0.01f, m_sliderAmbG.GetPos() * 0.01f, m_sliderAmbB.GetPos() * 0.01f);
    mat->SetDiffuse(m_sliderDifR.GetPos() * 0.01f, m_sliderDifG.GetPos() * 0.01f, m_sliderDifB.GetPos() * 0.01f);
    mat->SetSpecular(m_sliderSpecR.GetPos() * 0.01f, m_sliderSpecG.GetPos() * 0.01f, m_sliderSpecB.GetPos() * 0.01f);
    mat->SetShininess((float)m_sliderShininess.GetPos());

    CProjection* proj = pDoc->GetProjection();
    proj->SetZoom(m_sliderZoom.GetPos() * 0.01f);
}

void CControlPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
    SyncControlsToDoc();
    CFrameWnd* pFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
    if (pFrame) {
        CView* pView = pFrame->GetActiveView();
        if (pView) pView->Invalidate();
    }
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CControlPanel::OnApplySimplify() {
    CRealisticRendererDoc* pDoc = GetDoc();
    if (!pDoc) return;

    int gridSize = m_sliderSimplify.GetPos();
    pDoc->ApplySimplification(gridSize);

    // Recenter camera
    Vector4 center = pDoc->GetMesh()->GetCenter();
    float radius = pDoc->GetMesh()->GetRadius();
    pDoc->GetCamera()->SetTarget(center.x, center.y, center.z);
    pDoc->GetCamera()->SetDistance(radius * 2.5f);

    CFrameWnd* pFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
    if (pFrame) {
        CView* pView = pFrame->GetActiveView();
        if (pView) pView->Invalidate();
    }
}

HBRUSH CControlPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    return (HBRUSH)GetStockObject(DKGRAY_BRUSH);
}
