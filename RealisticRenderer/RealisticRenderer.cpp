#include "pch.h"
#include "RealisticRenderer.h"
#include "MainFrm.h"
#include "RealisticRendererDoc.h"
#include "RealisticRendererView.h"

BEGIN_MESSAGE_MAP(CRealisticRendererApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

CRealisticRendererApp::CRealisticRendererApp() noexcept {}

CRealisticRendererApp theApp;

BOOL CRealisticRendererApp::InitInstance() {
    // GDI+ initialization
    ULONG_PTR gdiToken;
    Gdiplus::GdiplusStartupInput gdiInput;
    Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, nullptr);

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    InitCommonControls();
    CWinAppEx::InitInstance();
    SetRegistryKey(L"RealisticRenderer");

    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CRealisticRendererDoc),
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CRealisticRendererView));
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    if (!ProcessShellCommand(cmdInfo)) return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

int CRealisticRendererApp::ExitInstance() {
    return CWinAppEx::ExitInstance();
}

// About dialog
class CAboutDlg : public CDialogEx {
public:
    CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}
protected:
    virtual void DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }
    DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CRealisticRendererApp::OnAppAbout() {
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
