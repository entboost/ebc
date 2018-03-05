#pragma once

#include "CDialogSK.h"

#define EBSC_WM_TOOLS_FINISH			(WM_USER+0x101)
#define EBSC_WM_TOOLS_CLICK_RECTANGLE	(WM_USER+0x102)	// bool bChecked = (bool)(wParam==1)
#define EBSC_WM_TOOLS_CLICK_CIRCLE		(WM_USER+0x103)	// bool bChecked = (bool)(wParam==1)
#define EBSC_WM_TOOLS_CLICK_ARROW		(WM_USER+0x104)	// bool bChecked = (bool)(wParam==1)
#define EBSC_WM_TOOLS_CLICK_TEXT		(WM_USER+0x105)	// bool bChecked = (bool)(wParam==1)
#define EBSC_WM_TOOLS_CLICK_COLOR		(WM_USER+0x106)	// 
#define EBSC_WM_TOOLS_CLICK_CANCEL		(WM_USER+0x107)	// 
#define EBSC_WM_TOOLS_CLICK_SAVEAS		(WM_USER+0x108)	// 

// CDlgTools dialog

class CDlgTools : public CDialogSK
{
	DECLARE_DYNAMIC(CDlgTools)

public:
	CDlgTools(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTools();

	void Reset(void);

// Dialog Data
	enum { IDD = IDD_DLG_TOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_chkColor;
	CLabelEx m_chkText;
	CLabelEx m_chkRectangle;
	CLabelEx m_chkCircle;
	CLabelEx m_chkArrow;
	CLabelEx m_chkCancel;
	CLabelEx m_chkSaveas;
	CTraButton m_btnFinish;

	HWND m_pCurrentHwnd;
	void RefreshToolCtrl(void);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFinish();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnStnClickedStaticColor();
	afx_msg void OnStnClickedStaticText();
	afx_msg void OnStnClickedStaticRectangle();
	afx_msg void OnStnClickedStaticCircle();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedStaticArrow();
	afx_msg void OnStnClickedStaticCancel();
	afx_msg void OnStnClickedStaticSaveas();
};
