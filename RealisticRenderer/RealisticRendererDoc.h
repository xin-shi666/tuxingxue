#pragma once

class CRealisticRendererDoc : public CDocument {
protected:
    CRealisticRendererDoc() noexcept;
    DECLARE_DYNCREATE(CRealisticRendererDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual ~CRealisticRendererDoc();
    DECLARE_MESSAGE_MAP()
};
