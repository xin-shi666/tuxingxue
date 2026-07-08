#pragma once

class CMainFrame : public CFrameWndEx {
protected:
    CMainFrame() noexcept;
    DECLARE_DYNCREATE(CMainFrame)
public:
    virtual ~CMainFrame();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
private:
    CMFCMenuBar m_wndMenuBar;
    CMFCStatusBar m_wndStatusBar;
};
