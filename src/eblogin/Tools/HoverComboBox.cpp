// HoverComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "HoverComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverComboBox

CHoverComboBox::CHoverComboBox()
{
	m_colorHoverRectangle = RGB(0,128,255);
}

CHoverComboBox::~CHoverComboBox()
{
}


BEGIN_MESSAGE_MAP(CHoverComboBox, CComboBox)
//{{AFX_MSG_MAP(CHoverComboBox)
ON_WM_NCPAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoverComboBox message handlers

void CHoverComboBox::OnHoverEnter()
{
	Redraw();
}

void CHoverComboBox::OnHoverLeave()
{
	Redraw();
}

void CHoverComboBox::Redraw()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}

void CHoverComboBox::OnNcPaint() 
{
	CWindowDC DC(this);
	CRect Rect;
	GetWindowRect(&Rect);
	if (IsHover())
	{
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_colorHoverRectangle);	//±ß¿òÑÕÉ«
		pOldPen = DC.SelectObject(&BorderPen);
		DC.Rectangle(0,0,Rect.Width(),Rect.Height());
		DC.SelectObject(pOldPen);
	}else
		DC.DrawEdge(CRect(0,0,Rect.Width(),Rect.Height()),EDGE_SUNKEN,BF_FLAT|BF_RECT);
}
