/**************CSplitterControl interface***********
*	Class name :CSplitterControl
*	Purpose: Implement splitter control for dialog
*			or any other windows.
*	Author: Nguyen Huy Hung, Vietnamese student.
*	Date:	May 29th 2002.
*	Note: You can use it for any purposes. Feel free
*			to change, modify, but please do not 
*			remove this.
*	No warranty of any risks.
*	(:-)
*/
#if !defined(AFX_SPLITTERCONTROL_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_)
#define AFX_SPLITTERCONTROL_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterControl.h : header file
//
//#include "pub_def.h"

/////////////////////////////////////////////////////////////////////////////
// CSplitterControl window
#define SPN_SIZED WM_USER + 1
#define CW_LEFTALIGN 1
#define CW_RIGHTALIGN 2
#define CW_TOPALIGN 3
#define CW_BOTTOMALIGN 4
#define SPS_VERTICAL 1
#define SPS_HORIZONTAL 2
typedef struct SPC_NMHDR
{
	NMHDR hdr;
	int delta;
} SPC_NMHDR;

#ifndef PUB_DLL_API
#define PUB_DLL_API
#endif


class PUB_DLL_API CSplitterControl : public CStatic
{
// Construction
public:
	CSplitterControl();

// Attributes
public:
	BOOL IsPressed(void) const {return m_bIsPressed;}
protected:
	BOOL		m_bIsPressed;
	int			m_nType;
	int			m_nX, m_nY;
	int			m_nMin, m_nMax;
	int			m_nSavePos;		// Save point on the lbutton down 
								// message
	COLORREF	m_colorBack;
	COLORREF	m_colorBorder;
	bool		m_bTransparent;
// Operations
public:
	void SetColor(COLORREF colorBack, COLORREF colorBorder);
	void SetTransparent(bool bTransparent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	static void ChangePos(CWnd* pWnd, int dx, int dy);
	static void ChangeWidth(CWnd* pWnd, int dx, DWORD dwFlag = CW_LEFTALIGN);
	static void ChangeHeight(CWnd* pWnd, int dy, DWORD dwFlag = CW_TOPALIGN);
public:
	void		GetRange(int & pOutMin, int & pOutMax) const;
	void		SetRange(int nMin, int nMax);
	void		SetRange(int nSubtraction, int nAddition, int nRoot);

	int			GetStyle(void) const;
	int			SetStyle(int nStyle = SPS_VERTICAL);
	void		Create(DWORD dwStyle, const CRect& rect, CWnd* pParent, UINT nID);
	virtual		~CSplitterControl();

	// Generated message map functions
protected:
	virtual void	DrawLine(CDC* pDC, int x, int y);
	void			MoveWindowTo(CPoint pt);
	//{{AFX_MSG(CSplitterControl)
	afx_msg void	OnPaint();
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERCONTROL_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_)
