#if !defined(AFX_HOVEREDIT_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_)
#define AFX_HOVEREDIT_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoverEdit.h : header file
//
#include "TrackControl.h"
/////////////////////////////////////////////////////////////////////////////
// CHoverEdit window

class CHoverEdit : public CTrackControl<CEdit>
{
// Construction
public:
	CHoverEdit();

// Attributes
public:
	void SetRectangleColor(COLORREF crHover,COLORREF crNormal=0) {m_colorHoverRectangle=crHover; m_colorNormalRectangle=crNormal;}
	COLORREF GetHoverRectangleColor(void) const {return m_colorHoverRectangle;}
	COLORREF GetNormalRectangleColor(void) const {return m_colorNormalRectangle;}

	//void SetFocusSelAll(BOOL v) {m_bFocusSelAll = v;}

// Operations
public:
	COLORREF	GetPromptColor(void) const { return m_crPromptColor; }
	const CString& GetPromptText(void) const { return m_strPromptText; }
	int			GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	void		GetWindowText(CString& rString) const;
	COLORREF	GetTextColor(void) const { return m_crTextColor; }
	void		SetTextColor(COLORREF crText) {m_crTextColor = crText;}
	void		SetPromptColor(COLORREF crText);
	void		SetPromptText(LPCTSTR lpszPrompt);
	void		SetWindowText(LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void OnHoverLeave();
	virtual void OnHoverEnter();
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual ~CHoverEdit();

	// Generated message map functions
protected:
	BOOL		m_bFirstTime;		// TRUE = user has not yet selected this control
	CString		m_strPromptText;	// prompt text to display initially
	COLORREF	m_crPromptColor;	// color to use for prompt text
	CBrush		m_brush;			// background brush for WM_CTLCOLOR
	BOOL		m_bFocusSelAll;
	COLORREF	m_crTextColor;		// color to use for text text

	COLORREF m_colorHoverRectangle;
	COLORREF m_colorNormalRectangle;
	//{{AFX_MSG(CHoverEdit)
	afx_msg void OnNcPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	inline void Redraw();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocus();
public:
	afx_msg void OnEnSetfocus();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOVEREDIT_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_)
