#include "stdafx.h"
#include "RealisticRenderer.h"
#include "MainFrm.h"
#include "RealisticRendererDoc.h"
#include "RealisticRendererView.h"

BEGIN_MESSAGE_MAP(CRealisticRendererApp, CWinAppEx)
    ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CRealisticRendererApp::CRealisticRendererApp() {}

CRealisticRendererApp theApp;

static ULONG_PTR g_gdiplusToken = 0;

BOOL CRealisticRendererApp::InitInstance() {
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    // Initialize MFC Feature Pack
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

    // Auto-load model
    CRealisticRendererDoc* pDoc = dynamic_cast<CRealisticRendererDoc*>(
        ((CFrameWnd*)m_pMainWnd)->GetActiveDocument());
    if (pDoc) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileName(nullptr, exePath, MAX_PATH);
        CString exeDir = exePath;
        int pos = exeDir.ReverseFind(L'\\');
        if (pos >= 0) exeDir = exeDir.Left(pos); // bin dir
        pos = exeDir.ReverseFind(L'\\');
        if (pos >= 0) exeDir = exeDir.Left(pos); // project root

        CString objPath = exeDir + L"\\海绵宝宝\\tripo_convert_6d23d57d-9d66-40ea-b55a-d3fba1d73181.obj";
        CString texPath = exeDir + L"\\海绵宝宝\\海绵宝宝3d模型_basecolor_2k.JPEG";

        if (GetFileAttributes(objPath) != INVALID_FILE_ATTRIBUTES) {
            pDoc->LoadModel(objPath);
            if (GetFileAttributes(texPath) != INVALID_FILE_ATTRIBUTES)
                pDoc->LoadTexture(texPath);
        }
    }

    m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

int CRealisticRendererApp::ExitInstance() {
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    return CWinAppEx::ExitInstance();
}
