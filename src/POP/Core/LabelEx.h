#if !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
#define AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LabelEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLabelEx window
class CLEDrawInfo
{
public:
	CImage* m_pImage;
	int m_nWidth;
	int m_nHeight;
	CLEDrawInfo(void)
		: m_pImage(NULL)
		, m_nWidth(0), m_nHeight(0)
	{}
	virtual ~ CLEDrawInfo(void)
	{
		if (m_pImage)
			delete m_pImage;
	}
};

class CLabelEx : public CStatic
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
	//inline void SetCheck( BOOL bClicked ){ m_bClicked = bClicked; }; 
	BOOL GetCheck( ) const { return m_bClicked;}; 
	bool LoadBG(UINT IDBkGroup, int width=-1, int height=0, int nSize=3,const CString& resourceType = _T("PNG"));
	bool LoadLabel(UINT IDLabelGroup, int width=-1, int height=0, int nSize=3, const CString& resourceType = _T("PNG"));

	void EnableHighLight(BOOL bHighLight);
	//void SetClickedLabelBitmap(HBITMAP ID);
	//void SetClickedLabelBitmap(UINT ID);
	//void SetMouseOverLabelBitmap(HBITMAP ID);
	//void SetMouseOverLabelBitmap(UINT ID);
	//void SetLabelBitmap(HBITMAP ID);
	//void SetLabelBitmap(UINT ID);
	//void SetClickedBGBitmap(HBITMAP ID);
	//void SetClickedBGBitmap(UINT ID);
	//void SetMouseOverBGBitmap(HBITMAP ID);
	//void SetMouseOverBGBitmap(UINT ID);
	//void SetBGBitmap(HBITMAP ID);
	//void SetBGBitmap(UINT ID);
	void RedrawWindow();
	void EnableAutoUnderLine(BOOL bAutoUnderLine);
	void SetBkColor(COLORREF crBkColor);
	void SetBorder(BOOL bBorder,COLORREF crBorder);
	void SetHoverBorder(BOOL bHoverBorder,COLORREF crBorder);
	void SetCheckBoxHoverBorder(BOOL bHoverBorder,COLORREF crBorder);
	void SetUnderLine(BOOL bUnderLine,COLORREF crUnderLine);
	void SetTextColor(COLORREF crText,COLORREF crHot=-1);
	virtual ~CLabelEx();

	void SetAlignText(AlignText v) {m_alignText = v;}
	void SetTextLeft(int v) {m_nTextLeft = v;}
	void SetTextTop(int v) {m_nTextTop = v;}
	void MoveWindowPoint(int x, int y, BOOL bRepaint = TRUE);
	int GetWidth(void) const;
	int GetHeight(void) const;

	void SetHandCur(HCURSOR pHandCur) {m_hHandCur = pHandCur;}
	void SetToolTipText(LPCTSTR s);

	// Generated message map functions
protected:
	HCURSOR m_hHandCur;
	CFont m_Font;
	BOOL m_bBorder;
	BOOL m_bClicked;
	BOOL m_bOver;
	BOOL m_bUnderLine,m_bBackUnderLine;
	BOOL m_bAutoUnderLine;
	BOOL m_hHoverBorder;
	COLORREF m_crHoverBorder;
	BOOL m_hCheckBoxHoverBorder;
	COLORREF m_crCheckBoxHoverBorder;
	AlignText m_alignText;
	int m_nTextLeft;
	int m_nTextTop;
	BOOL m_bHighLight;
	CLEDrawInfo m_pDrawBGNormal,m_pDrawBGHover,m_pDrawBGClicked;
	CLEDrawInfo m_pDrawLabelNormal,m_pDrawLabelHover,m_pDrawLabelClicked;
	//CBitmap m_BGBm,m_MouseOverBGBm,m_ClickedBGBm;
	//CBitmap m_LabelBm,m_MouseOverLabelBm,m_ClickedLabelBm;
	COLORREF m_crText,m_crBorder,m_crBG,m_crUnderLine;
	COLORREF m_crHotText;
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
	LRESULT OnMsgReInvalidate(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
