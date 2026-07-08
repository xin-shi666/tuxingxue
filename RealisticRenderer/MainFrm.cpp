#include "stdafx.h"
#include "MainFrm.h"
#include "RealisticRendererDoc.h"
#include "RealisticRendererView.h"

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(IDM_VIEW_WIREFRAME, OnViewWireframe)
    ON_COMMAND(IDM_VIEW_FLAT, OnViewFlat)
    ON_COMMAND(IDM_VIEW_GOURAUD, OnViewGouraud)
    ON_COMMAND(IDM_VIEW_TEXTURE, OnViewTexture)
    ON_COMMAND(IDM_VIEW_CONTROLPANEL, OnToggleControlPanel)
    ON_COMMAND(IDM_ANIMATION_START, OnAnimationStart)
    ON_COMMAND(IDM_ANIMATION_STOP, OnAnimationStop)
    ON_UPDATE_COMMAND_UI(IDM_ANIMATION_START, OnUpdateAnimStart)
    ON_UPDATE_COMMAND_UI(IDM_ANIMATION_STOP, OnUpdateAnimStop)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() : m_pControlPanel(nullptr) {}

CMainFrame::~CMainFrame() {
    if (m_pControlPanel) {
        m_pControlPanel->DestroyWindow();
        delete m_pControlPanel;
    }
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1) return -1;

    // Create menu bar
    m_menuBar.Create(this);
    m_menuBar.LoadMenu(IDR_RENDERER_MENU);

    // Create control panel
    m_pControlPanel = new CControlPanel();
    m_pControlPanel->Create(IDD_CONTROLPANEL, this);
    m_pControlPanel->ShowWindow(SW_SHOW);

    return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) {
    return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnViewWireframe() {
    CRealisticRendererDoc* pDoc = dynamic_cast<CRealisticRendererDoc*>(GetActiveDocument());
    if (pDoc) { pDoc->GetRenderer()->SetRenderMode(CRenderer::RM_WIREFRAME); GetActiveView()->Invalidate(); }
}

void CMainFrame::OnViewFlat() {
    CRealisticRendererDoc* pDoc = dynamic_cast<CRealisticRendererDoc*>(GetActiveDocument());
    if (pDoc) { pDoc->GetRenderer()->SetRenderMode(CRenderer::RM_FLAT); GetActiveView()->Invalidate(); }
}

void CMainFrame::OnViewGouraud() {
    CRealisticRendererDoc* pDoc = dynamic_cast<CRealisticRendererDoc*>(GetActiveDocument());
    if (pDoc) { pDoc->GetRenderer()->SetRenderMode(CRenderer::RM_GOURAUD); GetActiveView()->Invalidate(); }
}

void CMainFrame::OnViewTexture() {
    CRealisticRendererDoc* pDoc = dynamic_cast<CRealisticRendererDoc*>(GetActiveDocument());
    if (pDoc) { pDoc->GetRenderer()->SetRenderMode(CRenderer::RM_TEXTURE); GetActiveView()->Invalidate(); }
}

void CMainFrame::OnToggleControlPanel() {
    if (m_pControlPanel) {
        m_pControlPanel->ShowWindow(m_pControlPanel->IsWindowVisible() ? SW_HIDE : SW_SHOW);
    }
}

void CMainFrame::OnAnimationStart() {
    CRealisticRendererView* pView = dynamic_cast<CRealisticRendererView*>(GetActiveView());
    if (pView) pView->StartAnimation();
}

void CMainFrame::OnAnimationStop() {
    CRealisticRendererView* pView = dynamic_cast<CRealisticRendererView*>(GetActiveView());
    if (pView) pView->StopAnimation();
}

void CMainFrame::OnUpdateAnimStart(CCmdUI* pCmdUI) {
    CRealisticRendererView* pView = dynamic_cast<CRealisticRendererView*>(GetActiveView());
    pCmdUI->Enable(pView && !pView->IsAnimating());
}

void CMainFrame::OnUpdateAnimStop(CCmdUI* pCmdUI) {
    CRealisticRendererView* pView = dynamic_cast<CRealisticRendererView*>(GetActiveView());
    pCmdUI->Enable(pView && pView->IsAnimating());
}
