// MrSkinVBar.h file here
#pragma once
#ifndef __MrSkinVBar_h__
#define __MrSkinVBar_h__

// CMrSkinVBar
#define MR_WM_SKIN_SCROLL_CY	(WM_USER+0x321)	// int cy = (int)wParam;
#define MR_WM_VSCROLL			(WM_USER+0x322)

class CMrVBarCallback
{
public:
	virtual double GetScrollLimit(void) const = 0;
	virtual double GetScrollPage(void) const = 0;
	virtual double GetScrollPos(void) const = 0;
};

class CMrSkinVBar : public CStatic
{
	DECLARE_DYNAMIC(CMrSkinVBar)

public:
	CMrSkinVBar();
	virtual ~CMrSkinVBar();

// Attributes
public:
	CWnd * m_pScrollParent;
	CMrVBarCallback * m_pCallback;
	void LimitThumbPosition();
	void Draw(CDC * pDC);
	void UpdateThumbPosition();
	bool m_bMouseDownArrowUp, m_bMouseDownArrowDown;
	bool m_bDragging;
	bool m_bMouseDown;

	CBitmap	m_bmVerticalBarThumb;
	CBitmap	m_bmVerticalBarBottom;
	CBitmap	m_bmVerticalBarDownArrow;
	CBitmap	m_bmVerticalBarSpan;
	CBitmap	m_bmVerticalBarUpArrow;
	CBitmap	m_bmVerticalBarTop;
	
	int		m_nThumbTop;
	double	m_dbThumbInterval;

	void ScrollDown();
	void ScrollUp();
	void PageUp();
	void PageDown();
	void CalcBmpSize();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMrSkinVBar)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CMrSkinVBar)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BITMAP	m_stBitmapUpArrow;
	BITMAP	m_stBitmapBottom;
	BITMAP	m_stBitmapDownArrow;
	BITMAP	m_stBitmapThumb;
	BITMAP	m_stBitmapTop;
};


#endif // __MrSkinVBar_h__
