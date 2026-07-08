#pragma once
#include "Resource.h"

class CRealisticRendererApp : public CWinAppEx {
public:
    CRealisticRendererApp();
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};
