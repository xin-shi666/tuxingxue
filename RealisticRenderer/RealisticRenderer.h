#pragma once
#include "resource.h"

class CRealisticRendererApp : public CWinAppEx {
public:
    CRealisticRendererApp() noexcept;
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};
