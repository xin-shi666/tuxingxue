#include "pch.h"
#include "RealisticRendererView.h"
#include "RealisticRendererDoc.h"
#include "CObjLoader.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(CRealisticRendererView, CView)

BEGIN_MESSAGE_MAP(CRealisticRendererView, CView)
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
    ON_COMMAND(ID_VIEW_FLAT, OnViewFlat)
    ON_COMMAND(ID_VIEW_GOURAUD, OnViewGouraud)
    ON_COMMAND(ID_VIEW_TEXTURE, OnViewTexture)
    ON_COMMAND(ID_ANIMATION_START, OnAnimStart)
    ON_COMMAND(ID_ANIMATION_STOP, OnAnimStop)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

CRealisticRendererView::CRealisticRendererView() noexcept
    : m_pZBuffer(nullptr), m_pMesh(nullptr), m_angleX(0), m_angleY(0),
      m_animating(FALSE), m_modelLoaded(FALSE), m_lastMouseX(0), m_lastMouseY(0),
      m_mode(CZBuffer::WIREFRAME) {
    m_viewPoint = CP3(0, 0, 600);
}

CRealisticRendererView::~CRealisticRendererView() {
    delete m_pZBuffer;
    delete m_pMesh;
}

void CRealisticRendererView::OnInitialUpdate() {
    CView::OnInitialUpdate();
    SetTimer(TIMER_ID, 50, nullptr);

    // Try loading model from common locations
    CString paths[] = {
        L"..\\..\\海绵宝宝\\tripo_convert_6d23d57d-9d66-40ea-b55a-d3fba1d73181.obj",
        L"..\\海绵宝宝\\tripo_convert_6d23d57d-9d66-40ea-b55a-d3fba1d73181.obj",
        L"海绵宝宝\\tripo_convert_6d23d57d-9d66-40ea-b55a-d3fba1d73181.obj",
    };
    for (const auto& p : paths) {
        if (GetFileAttributes(p) != INVALID_FILE_ATTRIBUTES) {
            LoadObjFile(p);
            break;
        }
    }
}

void CRealisticRendererView::LoadObjFile(const CString& objPath) {
    CString dir = objPath;
    int pos = dir.ReverseFind(L'\\');
    if (pos >= 0) dir = dir.Left(pos + 1);

    CObjLoader loader;
    CObjLoader::RawData raw;
    if (!loader.Load(objPath, raw)) {
        AfxMessageBox(L"Failed to load OBJ file.");
        return;
    }

    delete m_pMesh;
    m_pMesh = new CMesh();
    m_pMesh->LoadFromRaw(raw);
    m_pMesh->Simplify(60);

    CP3 center = m_pMesh->GetCenter();
    double radius = m_pMesh->GetRadius();
    m_projection.SetEye(center.x, center.y + radius * 0.3, center.z + radius * 2.0);
    // view distance should be much larger than model size
    m_projection.SetViewDistance((std::max)(500.0, radius * 500));
    m_viewPoint = m_projection.GetEye();

    // Auto-find texture in same directory
    WIN32_FIND_DATA fd;
    CString texSearch = dir + L"*.jp*g";
    HANDLE hFind = FindFirstFile(texSearch, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        CString texFile = dir + fd.cFileName;
        m_texture.Load(texFile);
        FindClose(hFind);
    }

    m_modelLoaded = TRUE;
    Invalidate();
}

void CRealisticRendererView::OnFileOpen() {
    CFileDialog dlg(TRUE, L".obj", nullptr, OFN_FILEMUSTEXIST,
        L"OBJ Files (*.obj)|*.obj|All Files (*.*)|*.*||", this);
    if (dlg.DoModal() == IDOK) {
        LoadObjFile(dlg.GetPathName());
    }
}

void CRealisticRendererView::OnDraw(CDC* pDC) {
    if (!m_modelLoaded) {
        CRect rect; GetClientRect(&rect);
        pDC->FillSolidRect(&rect, RGB(30, 30, 34));
        pDC->SetTextColor(RGB(200, 200, 200));
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(L"File > Open to load an OBJ model", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    // Double buffering
    CRect rect; GetClientRect(&rect);
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    memDC.FillSolidRect(&rect, RGB(30, 30, 34));

    RenderScene(&memDC);

    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);
}

void CRealisticRendererView::RenderScene(CDC* pDC) {
    if (!m_pMesh) return;

    CRect rect; GetClientRect(&rect);
    int w = rect.Width(), h = rect.Height();

    if (!m_pZBuffer || m_pZBuffer->GetWidth() != w || m_pZBuffer->GetHeight() != h) {
        delete m_pZBuffer;
        m_pZBuffer = new CZBuffer();
        m_pZBuffer->InitBuffer(w, h);
    }

    m_pZBuffer->ClearBuffer();
    m_pZBuffer->SetMode(m_mode);
    m_pZBuffer->SetLighting(&m_lighting);
    m_pZBuffer->SetMaterial(&m_material);
    m_pZBuffer->SetTexture(&m_texture);
    m_pZBuffer->SetViewPoint(m_projection.GetEye());

    m_projection.SetScreenSize(w, h);

    // Reset vertices and transform
    m_pMesh->ResetToOriginal();
    int vCount = m_pMesh->GetVertexCount();
    CP3* verts = m_pMesh->GetVertices();

    // Rotate
    CTransform3 rotY, rotX;
    rotY.RotateY(m_angleY);
    rotX.RotateX(m_angleX);
    rotY.MultiplyMatrix(verts, vCount);
    rotX.MultiplyMatrix(verts, vCount);

    // Copy transformed positions for lighting
    CP3* worldVerts = new CP3[vCount];
    for (int i = 0; i < vCount; i++) worldVerts[i] = verts[i];

    // Perspective projection
    m_projection.PerspectiveProjection(verts, vCount);

    // Draw triangles
    CVector3* norms = m_pMesh->GetNormals();
    double* texcoords = m_pMesh->GetTexcoords();
    BOOL hasTex = m_pMesh->HasTexcoords();
    int* indices = m_pMesh->GetFaceIndices();
    int fCount = m_pMesh->GetFaceCount();

    for (int i = 0; i < fCount; i++) {
        int i0 = indices[i * 3], i1 = indices[i * 3 + 1], i2 = indices[i * 3 + 2];
        if (i0 >= vCount || i1 >= vCount || i2 >= vCount) continue;

        const CP3& p0 = verts[i0], &p1 = verts[i1], &p2 = verts[i2];
        const CP3& w0 = worldVerts[i0], &w1 = worldVerts[i1], &w2 = worldVerts[i2];
        const CVector3& n0 = norms[i0], &n1 = norms[i1], &n2 = norms[i2];

        double u0 = 0, v0 = 0, u1 = 0, v1 = 0, u2 = 0, v2 = 0;
        if (hasTex && texcoords) {
            u0 = texcoords[i0 * 2]; v0 = texcoords[i0 * 2 + 1];
            u1 = texcoords[i1 * 2]; v1 = texcoords[i1 * 2 + 1];
            u2 = texcoords[i2 * 2]; v2 = texcoords[i2 * 2 + 1];
        }

        m_pZBuffer->DrawTriangle(pDC, p0, p1, p2, n0, n1, n2, w0, w1, w2, u0, v0, u1, v1, u2, v2);
    }

    delete[] worldVerts;
}

void CRealisticRendererView::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == TIMER_ID && m_animating) {
        m_angleY += 0.03;
        Invalidate(FALSE);
    }
    CView::OnTimer(nIDEvent);
}

void CRealisticRendererView::OnSize(UINT nType, int cx, int cy) {
    CView::OnSize(nType, cx, cy);
    if (cx > 0 && cy > 0 && m_pZBuffer) {
        delete m_pZBuffer;
        m_pZBuffer = new CZBuffer();
        m_pZBuffer->InitBuffer(cx, cy);
    }
}

void CRealisticRendererView::OnDestroy() {
    KillTimer(TIMER_ID);
    CView::OnDestroy();
}

void CRealisticRendererView::OnMouseMove(UINT nFlags, CPoint point) {
    if (nFlags & MK_LBUTTON) {
        double dx = (point.x - m_lastMouseX) * 0.01;
        double dy = (point.y - m_lastMouseY) * 0.01;
        m_angleY += dx;
        m_angleX += dy;
        Invalidate(FALSE);
    }
    m_lastMouseX = point.x;
    m_lastMouseY = point.y;
    CView::OnMouseMove(nFlags, point);
}

BOOL CRealisticRendererView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    double d = m_projection.GetEye().z + zDelta * 0.5;
    if (d < 50) d = 50;
    if (d > 5000) d = 5000;
    m_projection.SetEye(m_projection.GetEye().x, m_projection.GetEye().y, d);
    Invalidate(FALSE);
    return TRUE;
}

BOOL CRealisticRendererView::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

void CRealisticRendererView::SetRenderMode(int mode) {
    m_mode = (CZBuffer::Mode)mode;
    Invalidate(FALSE);
}

void CRealisticRendererView::StartAnimation() { m_animating = TRUE; }
void CRealisticRendererView::StopAnimation() { m_animating = FALSE; }

void CRealisticRendererView::OnViewWireframe() { SetRenderMode(0); }
void CRealisticRendererView::OnViewFlat() { SetRenderMode(1); }
void CRealisticRendererView::OnViewGouraud() { SetRenderMode(2); }
void CRealisticRendererView::OnViewTexture() { SetRenderMode(3); }
void CRealisticRendererView::OnAnimStart() { StartAnimation(); }
void CRealisticRendererView::OnAnimStop() { StopAnimation(); }
