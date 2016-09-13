#pragma once


// CPanelText dialog

class CPanelText : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelText)

public:
	CPanelText(CWnd* pParent = NULL, int nFontSize=12);   // standard constructor
	virtual ~CPanelText();

	void SetBgColor(COLORREF bg);
	void SetDrawPos(int x, int y);
	void SetBorder(int nBorder=1);
	void SetDrawText(const wchar_t* lpszText, bool bShow = true, int nAutoClose=0);
	int MeasureTextWidth(const wchar_t* lpszText, int nMaxWidth);

// Dialog Data
	enum { IDD = IDD_PAN_TEXT };

protected:
	COLORREF m_colorBg;
	int m_nBorder;
	std::wstring m_sDrawText;
	Gdiplus::PointF m_pPoint;
	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush* m_pBrush;
	//int m_nAutoClose;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
