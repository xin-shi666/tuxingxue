#pragma once

class CRealisticRendererView : public CView {
public:
    CRealisticRendererView();
    virtual ~CRealisticRendererView();

    void StartAnimation();
    void StopAnimation();
    bool IsAnimating() const { return m_animating; }

    DECLARE_MESSAGE_MAP()

protected:
    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
    void RenderToScreen(CDC* pDC);
    bool m_animating;
    float m_animAngle;
    int m_lastMouseX, m_lastMouseY;
    bool m_mouseDragging;
    CRealisticRendererDoc* GetRendererDoc();
};
