#pragma once


// CDlgShrinkageBar dialog

class CDlgShrinkageBar : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgShrinkageBar)

public:
	CDlgShrinkageBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgShrinkageBar();

	void SetBgColor(COLORREF colorBg=-1);
// Dialog Data
	enum { IDD = IDD_DLG_SHRINKAGEBAR };

protected:
	COLORREF m_colorBg;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
