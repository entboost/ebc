#if !defined(AFX_HOVERCOMBOBOX_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_)
#define AFX_HOVERCOMBOBOX_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoverComboBox.h : header file
//
#include "TrackControl.h"
/////////////////////////////////////////////////////////////////////////////
// CHoverComboBox window

class CHoverComboBox : public CTrackControl<CComboBox>
{
// Construction
public:
	CHoverComboBox();

// Attributes
public:
	void SetHoverRectangleColor(COLORREF v) {m_colorHoverRectangle=v;}
	COLORREF GetHoverRectangleColor(void) const {return m_colorHoverRectangle;}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void OnHoverLeave();
	virtual void OnHoverEnter();
	virtual ~CHoverComboBox();

	// Generated message map functions
protected:
	COLORREF m_colorHoverRectangle;
	//{{AFX_MSG(CHoverComboBox)
	afx_msg void OnNcPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	inline void Redraw();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOVERCOMBOBOX_H__372AC76D_6B84_435C_8300_9519EB021C8C__INCLUDED_)
