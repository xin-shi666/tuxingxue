#include "pch.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
    ON_WM_CREATE()
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept {}
CMainFrame::~CMainFrame() {}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
    if (!CFrameWndEx::PreCreateWindow(cs)) return FALSE;
    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1) return -1;

    m_wndMenuBar.Create(this);
    m_wndMenuBar.LoadMenu(IDR_MAINFRAME);

    m_wndStatusBar.Create(this);
    m_wndStatusBar.SetIndicators(nullptr, 0);

    return 0;
}
