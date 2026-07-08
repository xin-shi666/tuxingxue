#include "pch.h"
#include "RealisticRendererDoc.h"

IMPLEMENT_DYNCREATE(CRealisticRendererDoc, CDocument)

BEGIN_MESSAGE_MAP(CRealisticRendererDoc, CDocument)
END_MESSAGE_MAP()

CRealisticRendererDoc::CRealisticRendererDoc() noexcept {}
CRealisticRendererDoc::~CRealisticRendererDoc() {}

BOOL CRealisticRendererDoc::OnNewDocument() {
    if (!CDocument::OnNewDocument()) return FALSE;
    return TRUE;
}

void CRealisticRendererDoc::Serialize(CArchive& ar) {}
