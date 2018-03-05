#if !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
#define AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LabelEx.h : header file
//
//#include "pub_def.h"

/////////////////////////////////////////////////////////////////////////////
// CLabelEx window

class /*PUB_DLL_API*/ CLabelEx : public CStatic
{
// Construction
public:
	CLabelEx();
	enum AlignText
	{
		Align_Left
		, Align_Center
		, Align_Right
		, Align_Default
	};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetCheck( BOOL bClicked ){ m_bClicked = bClicked; if (!m_bClicked&& m_bOver) m_bOver=FALSE;};
	BOOL GetCheck(void) const { return m_bClicked;}; 
	void EnableHighLight(BOOL bHighLight);
	void SetClickedLabelBitmap(HBITMAP ID);
	void SetClickedLabelBitmap(UINT ID);
	void SetMouseOverLabelBitmap(HBITMAP ID);
	void SetMouseOverLabelBitmap(UINT ID);
	void SetClickedBGBitmap(HBITMAP ID);
	void SetClickedBGBitmap(UINT ID);
	void SetLabelBitmap(HBITMAP ID);
	void SetLabelBitmap(UINT ID);
	void SetMouseOverBGBitmap(HBITMAP ID);
	void SetMouseOverBGBitmap(UINT ID);
	void SetBGBitmap(HBITMAP ID);
	void SetBGBitmap(UINT ID);
	void RedrawWindow();
	void EnableAutoUnderLine(BOOL bAutoUnderLine);
	void SetBkColor(COLORREF crBkColor);
	void SetBorder(BOOL bBorder,COLORREF crBorder);
	void SetUnderLine(BOOL bUnderLine,COLORREF crUnderLine);
	void SetTextColor(COLORREF crText);
	virtual ~CLabelEx();

	void SetAlignText(AlignText v) {m_alignText = v;}
	void MoveWindowPoint(int x, int y, BOOL bRepaint = TRUE);
	int GetImgWidth(void) const {return m_nImgWidth;}
	int GetImgHeight(void) const {return m_nImgHeight;}
	int GetWidth(void) const;
	int GetHeight(void) const;

	void SetHandCur(HCURSOR pHandCur) {m_hHandCur = pHandCur;}
	void SetToolTipText(LPCTSTR s);

	// Generated message map functions
protected:
	HCURSOR m_hHandCur;
	int m_nImgWidth;
	int m_nImgHeight;
	CFont m_Font;
	BOOL m_bBorder;
	BOOL m_bClicked;
	BOOL m_bOver;
	BOOL m_bUnderLine,m_bBackUnderLine;
	BOOL m_bAutoUnderLine;
	AlignText m_alignText;
	BOOL m_bHighLight;
	CBitmap m_BGBm,m_MouseOverBGBm,m_ClickedBGBm;
	CBitmap m_LabelBm,m_MouseOverLabelBm,m_ClickedLabelBm;
	COLORREF m_crText,m_crBorder,m_crBG,m_crUnderLine;
	COLORREF m_crBackText;
	//{{AFX_MSG(CLabelEx)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	CToolTipCtrl m_tooltip;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
