#pragma once
#include "CZBuffer.h"
#include "CMesh.h"
#include "CTransform3.h"
#include "CProjection.h"
#include "CLighting.h"
#include "CMaterial.h"
#include "CTexture.h"

class CRealisticRendererView : public CView {
protected:
    CRealisticRendererView() noexcept;
    DECLARE_DYNCREATE(CRealisticRendererView)

public:
    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
    virtual ~CRealisticRendererView();

    void SetRenderMode(int mode);
    void StartAnimation();
    void StopAnimation();

protected:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    afx_msg void OnViewWireframe();
    afx_msg void OnViewFlat();
    afx_msg void OnViewGouraud();
    afx_msg void OnViewTexture();
    afx_msg void OnAnimStart();
    afx_msg void OnAnimStop();
    afx_msg void OnFileOpen();

    DECLARE_MESSAGE_MAP()

private:
    void RenderScene(CDC* pDC);
    void LoadObjFile(const CString& objPath);

    CZBuffer* m_pZBuffer;
    CMesh* m_pMesh;
    CProjection m_projection;
    CLighting m_lighting;
    CMaterial m_material;
    CTexture m_texture;
    CTransform3 m_transform;

    double m_angleX, m_angleY;
    BOOL m_animating;
    BOOL m_modelLoaded;
    int m_lastMouseX, m_lastMouseY;
    CZBuffer::Mode m_mode;
    CP3 m_viewPoint;
};
