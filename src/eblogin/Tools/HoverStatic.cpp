// HoverStatic.cpp : implementation file
//

#include "stdafx.h"
#include "HoverStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverStatic

CHoverStatic::CHoverStatic()
{
	m_colorHoverRectangle = RGB(0,128,255);
}

CHoverStatic::~CHoverStatic()
{
}


BEGIN_MESSAGE_MAP(CHoverStatic, CStatic)
//{{AFX_MSG_MAP(CHoverStatic)
ON_WM_NCPAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoverStatic message handlers

void CHoverStatic::OnHoverEnter()
{
	Redraw();
}

void CHoverStatic::OnHoverLeave()
{
	Redraw();
}

void CHoverStatic::Redraw()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}

void CHoverStatic::OnNcPaint() 
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
	{
		DC.DrawEdge(CRect(0,0,Rect.Width(),Rect.Height()),EDGE_SUNKEN,BF_FLAT|BF_RECT);
	}
}
