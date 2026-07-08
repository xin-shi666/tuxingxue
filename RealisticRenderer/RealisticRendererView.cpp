#include "stdafx.h"
#include "RealisticRendererView.h"
#include "RealisticRendererDoc.h"

IMPLEMENT_DYNCREATE(CRealisticRendererView, CView)

BEGIN_MESSAGE_MAP(CRealisticRendererView, CView)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CRealisticRendererView::CRealisticRendererView()
    : m_animating(false), m_animAngle(0), m_lastMouseX(0), m_lastMouseY(0), m_mouseDragging(false) {}

CRealisticRendererView::~CRealisticRendererView() {}

CRealisticRendererDoc* CRealisticRendererView::GetRendererDoc() {
    return dynamic_cast<CRealisticRendererDoc*>(GetDocument());
}

void CRealisticRendererView::OnInitialUpdate() {
    CView::OnInitialUpdate();
    CRealisticRendererDoc* pDoc = GetRendererDoc();
    if (pDoc) {
        CRect rect;
        GetClientRect(&rect);
        pDoc->GetProjection()->SetPerspective(45.0f, (float)rect.Width() / std::max(1, rect.Height()), 0.1f, 100.0f);
        pDoc->GetProjection()->SetViewport(0, 0, rect.Width(), rect.Height());
        pDoc->GetRenderer()->Init(rect.Width(), rect.Height());
    }
}

void CRealisticRendererView::OnDraw(CDC* pDC) {
    CRealisticRendererDoc* pDoc = GetRendererDoc();
    if (!pDoc) return;

    CRenderer* renderer = pDoc->GetRenderer();
    if (m_animating) {
        m_animAngle += 0.02f;
        pDoc->SetAnimRotation(0, m_animAngle, 0);
    }

    renderer->Render();
    RenderToScreen(pDC);
}

void CRealisticRendererView::RenderToScreen(CDC* pDC) {
    CRealisticRendererDoc* pDoc = GetRendererDoc();
    if (!pDoc) return;

    CRenderer* renderer = pDoc->GetRenderer();
    int w = renderer->GetWidth();
    int h = renderer->GetHeight();
    if (w <= 0 || h <= 0) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(pDC->GetSafeHdc(),
        0, 0, w, h,
        0, 0, 0, h,
        renderer->GetBuffer(), &bmi, DIB_RGB_COLORS);
}

void CRealisticRendererView::OnSize(UINT nType, int cx, int cy) {
    CView::OnSize(nType, cx, cy);
    if (cx <= 0 || cy <= 0) return;

    CRealisticRendererDoc* pDoc = GetRendererDoc();
    if (pDoc) {
        pDoc->GetProjection()->SetPerspective(45.0f, (float)cx / std::max(1, cy), 0.1f, 100.0f);
        pDoc->GetProjection()->SetViewport(0, 0, cx, cy);
        pDoc->GetRenderer()->Resize(cx, cy);
    }
}

void CRealisticRendererView::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == ID_TIMER_ANIM) {
        Invalidate(FALSE);
    }
    CView::OnTimer(nIDEvent);
}

void CRealisticRendererView::StartAnimation() {
    if (!m_animating) {
        m_animating = true;
        SetTimer(ID_TIMER_ANIM, 30, nullptr);
    }
}

void CRealisticRendererView::StopAnimation() {
    if (m_animating) {
        m_animating = false;
        KillTimer(ID_TIMER_ANIM);
    }
}

void CRealisticRendererView::OnMouseMove(UINT nFlags, CPoint point) {
    if (nFlags & MK_LBUTTON) {
        CRealisticRendererDoc* pDoc = GetRendererDoc();
        if (pDoc) {
            float dx = (float)(point.x - m_lastMouseX) * 0.01f;
            float dy = (float)(point.y - m_lastMouseY) * 0.01f;
            pDoc->GetCamera()->Orbit(dx, dy);
            Invalidate(FALSE);
        }
    }
    m_lastMouseX = point.x;
    m_lastMouseY = point.y;
    CView::OnMouseMove(nFlags, point);
}

void CRealisticRendererView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    CRealisticRendererDoc* pDoc = GetRendererDoc();
    if (pDoc) {
        float d = pDoc->GetCamera()->GetDistance() - zDelta * 0.001f;
        d = std::max(0.5f, std::min(20.0f, d));
        pDoc->GetCamera()->SetDistance(d);
        Invalidate(FALSE);
    }
    CView::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CRealisticRendererView::OnEraseBkgnd(CDC* pDC) {
    return TRUE; // prevent flicker
}
