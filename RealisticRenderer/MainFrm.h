#pragma once
#include "ControlPanel.h"

class CMainFrame : public CFrameWndEx {
public:
    CMainFrame();
    virtual ~CMainFrame();

    DECLARE_MESSAGE_MAP()

protected:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewWireframe();
    afx_msg void OnViewFlat();
    afx_msg void OnViewGouraud();
    afx_msg void OnViewTexture();
    afx_msg void OnToggleControlPanel();
    afx_msg void OnAnimationStart();
    afx_msg void OnAnimationStop();
    afx_msg void OnUpdateAnimStart(CCmdUI* pCmdUI);
    afx_msg void OnUpdateAnimStop(CCmdUI* pCmdUI);

private:
    CMFCMenuBar m_menuBar;
    CControlPanel* m_pControlPanel;
};
