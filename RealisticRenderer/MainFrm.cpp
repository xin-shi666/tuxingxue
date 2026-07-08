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
    return 0;
}
