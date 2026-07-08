#pragma once

class CControlPanel : public CDialogEx {
public:
    CControlPanel();
    virtual ~CControlPanel();

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnApplySimplify();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    DECLARE_MESSAGE_MAP()

private:
    void SyncControlsFromDoc();
    void SyncControlsToDoc();
    CRealisticRendererDoc* GetDoc();

    CSliderCtrl m_sliderLightX, m_sliderLightY, m_sliderLightZ;
    CSliderCtrl m_sliderAmbR, m_sliderAmbG, m_sliderAmbB;
    CSliderCtrl m_sliderDifR, m_sliderDifG, m_sliderDifB;
    CSliderCtrl m_sliderSpecR, m_sliderSpecG, m_sliderSpecB;
    CSliderCtrl m_sliderShininess, m_sliderZoom, m_sliderSpeed;
    CSliderCtrl m_sliderSimplify;
    CButton m_btnSimplify;

    void CreateSlider(int id, int x, int y, int w, int h, int minVal, int maxVal, int initVal);
    void CreateLabel(const wchar_t* text, int x, int y, int w, int h);
};
