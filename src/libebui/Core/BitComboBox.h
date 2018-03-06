
////////////////////////////////////////////////////////////////////////////
//	Copyright : Ashok Jaiswal 2004
//
//	Email :ashok@muvee.com
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description : Description goes here 
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITCOMBO_H__83303B81_8425_4D99_BFE3_D6C89494C75D__INCLUDED_)
#define AFX_BITCOMBO_H__83303B81_8425_4D99_BFE3_D6C89494C75D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitCombo.h : header file
//

class CBitListBox : public CListBox
{
// Construction
public:
	CBitListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitListBox)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBitComboBox window

class CBitComboBox : public CComboBox
{
// Construction
public:
	CBitComboBox();
	CBitListBox   m_listbox;
// Attributes
private:
//-------------Combo Box image scheme-----------------------------
//		-------------------------------------------------------
//	   | m_nIDLeft	-----m_nIDCenter-----			|m_nIDRight|
//		-------------------------------------------------------
//-----------------------------------------------------------------

//------------List Box image scheme--------------------------------

//		ALL IMAGES ARE STRETCHED TO FIT THE DIMENSIONS

//				-------------m_nIDListTop---------------------
//			   |											  |
//			   |											  |
//			   |											  |
//			m_nIDListLeft							     m_nIDListRight
//			   |											  |
//			   |											  |
//			   |											  |
//				-------------m_nIDListBot---------------------
//------------------------------------------------------------------


	CBitmap m_bmLeft;
	CBitmap m_bmRight;
	CBitmap m_bmCenter;
	CBitmap m_bmListLeft;
	CBitmap m_bmListRight;
	CBitmap m_bmListTop;
	CBitmap m_bmListBot;
	//UINT m_nIDLeft, m_nIDRight, m_nIDCenter;							//Resource IDs for ComboBox
	//UINT m_nIDListLeft, m_nIDListRight, m_nIDListTop, m_nIDListBot;		//Resource IDs for ListBox
	COLORREF	m_colBackGroundHighLight, m_colTextHighLight;			//Colors for listbox in highligh position
	COLORREF	m_colBackGroundNormal, m_colTextNormal;					//Colors for listbox in normal position

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitComboBox)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetNormalPositionColor(COLORREF colBackGround, COLORREF colText);
	void SetHighlightColor(COLORREF colBackGround, COLORREF colText);
	void DrawListBot(int nWidth=0);
	void DrawListTop(int nWidth=0);
	void DrawListRight(int nHeight=0);
	void DrawListLeft(int nHeight=0);
	void SetComboListBitmap(HBITMAP nIDListLeft, HBITMAP nIDListRight, HBITMAP nIDListTop, HBITMAP nIDListBot);
	void SetComboListBitmap(UINT nIDListLeft, UINT nIDListRight, UINT nIDListTop, UINT nIDListBot);
	void SetComboBitmap(HBITMAP nIDLeft, HBITMAP nIDRight, HBITMAP nIDCenter);
	void SetComboBitmap(UINT nIDLeft, UINT nIDRight, UINT nIDCenter);
	virtual ~CBitComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitComboBox)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITCOMBO_H__83303B81_8425_4D99_BFE3_D6C89494C75D__INCLUDED_)
