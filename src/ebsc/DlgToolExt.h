#pragma once

#include "CDialogSK.h"
#include "DlgSelect.h"

#define EBSC_WM_TOOLEXT_POINT			(WM_USER+0x301)

// CDlgToolExt dialog

class CDlgToolExt : public CDialogSK
{
	DECLARE_DYNAMIC(CDlgToolExt)

public:
	CDlgToolExt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolExt();

	void Reset(void);
	bool SetDrawType(CDrawInfo::DRAW_TYPE nDrawType);

// Dialog Data
	enum { IDD = IDD_DLG_TOOLEXT };

protected:
	CDrawInfo::DRAW_TYPE m_nDrawType;
	int m_nPointType;
	void UpdatePointCtrl(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnPoint1;
	CTraButton m_btnPoint2;
	CTraButton m_btnPoint3;
	CTraButton m_btnPoint4;
	//CLabelEx m_chkColor;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedPoint1();
	afx_msg void OnBnClickedPoint2();
	afx_msg void OnBnClickedPoint3();
	afx_msg void OnBnClickedPoint4();
};
