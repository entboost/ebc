
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
// BitComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "BitComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_CHECK_HOVER 100
#define TIMER_DOUBLE_DRAW_HOVER 101

int nListBoxTextLeft = 4;

static WNDPROC m_pWndProc = 0;

#define SAFE_DELETEOBJECT(obj) {if (obj) DeleteObject(obj);}

CBitListBox::CBitListBox()
: m_hParent(NULL)

{
}

CBitListBox::~CBitListBox()
{
}


BEGIN_MESSAGE_MAP(CBitListBox, CListBox)
	//{{AFX_MSG_MAP(CBitListBox)
	//ON_WM_PAINT()
	//}}AFX_MSG_MAP
	//ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

//void CBitListBox::OnKillFocus(CWnd* pNewWnd)
//{
//	////AfxMessageBox(_T("CBitListBox::OnKillFocus"));
//	//if (m_hParent!=NULL && m_hParent!=pNewWnd->GetSafeHwnd())
//	//{
//	//	//AfxMessageBox(_T("PostMessage WM_KILLFOCUS"));
//	//	::PostMessage(m_hParent,WM_KILLFOCUS,(WPARAM)pNewWnd->GetSafeHwnd(),0);
//	//}
//}

/////////////////////////////////////////////////////////////////////////////
// CBitListBox message handlers

void CBitListBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
// 	RECT rc;
// 	GetClientRect(&rc);
// 	
// 	CBrush br;
// 	br.CreateSolidBrush(RGB(255,0,0));
// 	dc.FillRect(&rc,&br);
	
	// Do not call CListBox::OnPaint() for painting messages
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:23:46 PM
// Function name	: LRESULT FAR PASCAL BitComboBoxListBoxProc
// Description	    : The list box control in combo box is actually owned by window so we let all the messages
//						handled by windows.
// Return type		: extern "C" 
// Argument         : HWND hWnd
// Argument         : UINT nMsg
// Argument         : WPARAM wParam
// Argument         : LPARAM lParam
//----------------------------------------------------------------------
extern "C" LRESULT FAR PASCAL BitComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//TRACE("BitComboBoxListBoxProc 0x%.4X\n",nMsg);
	//switch(nMsg)
	//{
	//case WM_KILLFOCUS:
	//	{
	//	}break;
	////case WM_MOUSEMOVE:
	////	{
	////		if (!m_bTracking)
	////		{
	////			TRACKMOUSEEVENT Tme;
	////			Tme.cbSize = sizeof(Tme);
	////			Tme.hwndTrack = GetSafeHwnd();
	////			Tme.dwFlags = TME_LEAVE|TME_HOVER;
	////			Tme.dwHoverTime = 1;
	////			if (_TrackMouseEvent(&Tme))
	////				m_bTracking=TRUE;
	////		}
	////		break;				
	////	}
	////case WM_MOUSEHOVER:
	////	m_bHover=TRUE;
	////	OnHoverEnter();
	////	break;
	////case WM_MOUSELEAVE:
	////	m_bTracking=m_bHover=FALSE;
	////	OnHoverLeave();
	////	break;
	//}

	return CallWindowProc(m_pWndProc, hWnd, nMsg, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
// CBitComboBox


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:42 PM
// Function name	: CBitComboBox::CBitComboBox
// Description	    : Constructor
// Return type		: 
//----------------------------------------------------------------------
CBitComboBox::CBitComboBox()
{
	//m_nIDLeft = m_nIDRight = m_nIDCenter = 0;
	//m_nIDListLeft = m_nIDListRight = m_nIDListTop = m_nIDListBot = 0;

	m_colBackGroundNormal = RGB(255,255,255);
	m_colTextNormal = RGB(0,0,0);

	m_bDoubleDrawHover = false;
	m_bTracking = FALSE;
	m_bHover = FALSE;
	m_colorHoverRectangle = 0;
	m_colorNormalRectangle1 = 0;
	m_colorNormalRectangle2 = 0;
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:43 PM
// Function name	: CBitComboBox::~CBitComboBox
// Description	    : Destructor
// Return type		: 
//----------------------------------------------------------------------
CBitComboBox::~CBitComboBox()
{
}


BEGIN_MESSAGE_MAP(CBitComboBox, CComboBox)
	//{{AFX_MSG_MAP(CBitComboBox)
	ON_WM_MOUSEMOVE()
#ifndef USES_MESSAGE_HOVERLEAVE
	//ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
#endif
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//ON_WM_SETFOCUS()
	//ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR_REFLECT()
	//ON_CBN_DROPDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
#ifdef USES_MESSAGE_HOVERLEAVE
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
#endif
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)
	//ON_CONTROL_REFLECT(CBN_DROPDOWN, OnCbnDropdown)

	//ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitComboBox message handlers

//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:38 PM
// Function name	: CBitComboBox::OnCtlColorListBox
// Description	    : It is called when list box item are painted
// Return type		: LRESULT 
// Argument         : WPARAM wParam
// Argument         : LPARAM lParam
//----------------------------------------------------------------------
LRESULT CBitComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	//TRACE("OnCtlColorListBox()\n");
	if (this->m_listbox.m_hWnd == 0) {
		//m_listbox.SetMsgParent(this->GetSafeHwnd());
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) 
		{
			// Save the handle
			m_listbox.m_hWnd = hWnd;

			// Subclass ListBox
			m_pWndProc = (WNDPROC)GetWindowLong(m_listbox.m_hWnd, GWL_WNDPROC);
			SetWindowLong(m_listbox.m_hWnd, GWL_WNDPROC, (LONG)BitComboBoxListBoxProc);
		}
	}
	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}


void CBitComboBox::SetRectangleColor(COLORREF crHover,COLORREF crNormal1,COLORREF crNormal2)
{
	m_colorHoverRectangle=crHover;
	m_colorNormalRectangle1=crNormal1;
	m_colorNormalRectangle2=crNormal2==-1?crNormal1:crNormal2;
}

#define SET_ZERO(X) memset((void *)&X,0,sizeof(X));

//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:36 PM
// Function name	: CBitComboBox::OnPaint
// Description	    : Main painting procedure. Skin the Combo box and skin the list box once its created.
// Return type		: void 
//----------------------------------------------------------------------
void CBitComboBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	RECT rc;
	//CDC MemDC;
	
	GetClientRect(&rc);
	//MemDC.CreateCompatibleDC(&dc);
	//if (MemDC.m_hDC == NULL)
	//	return;

	HGDIOBJ pbmpOld;
	//CBitmap bmpComboRight,bmpComboLeft, bmpComboCenter,bmpComboBot;
	BITMAP bitRight, bitLeft, bitCenter;
	SET_ZERO(bitRight); SET_ZERO(bitLeft); SET_ZERO(bitCenter);
	//if( m_nIDLeft )
	if (m_bmLeft.m_hObject)
	{
		//bmpComboLeft.LoadBitmap(m_nIDLeft);
		m_bmLeft.GetBitmap(&bitLeft);
		//pbmpOld = MemDC.SelectObject(m_bmLeft);
		//dc.BitBlt(rc.left,rc.top,bitLeft.bmWidth,bitLeft.bmHeight,&MemDC,0,0,SRCCOPY);
		//MemDC.SelectObject( pbmpOld );
	}else
	{
		bitLeft.bmWidth = 4;
	}
	//if( m_nIDRight )
	if (m_bmRight.m_hObject)
	{
		//bmpComboRight.LoadBitmap(m_nIDRight);
		m_bmRight.GetBitmap(&bitRight);
		//pbmpOld = MemDC.SelectObject(m_bmRight);
		//dc.BitBlt(rc.right-bitRight.bmWidth,rc.top,bitRight.bmWidth,bitRight.bmHeight,&MemDC,0,0,SRCCOPY);
		//MemDC.SelectObject( pbmpOld );
	}else
	{
		bitRight.bmWidth = 22;
	}
	//if (!this->IsWindowEnabled())
	//{
	//	CBrush
	//	dc.FillRect();
	//}

	//if( m_nIDCenter )
	if (m_bmCenter.m_hObject)
	{
		//bmpComboCenter.LoadBitmap(m_nIDCenter);
		m_bmCenter.GetBitmap(&bitCenter);
		//pbmpOld = MemDC.SelectObject(m_bmCenter);
		//dc.StretchBlt(rc.left+bitLeft.bmWidth,rc.top,rc.right-bitLeft.bmWidth-bitRight.bmWidth,bitCenter.bmHeight,&MemDC,0,0,bitCenter.bmWidth,bitCenter.bmHeight,SRCCOPY);
		//MemDC.SelectObject( pbmpOld );
	}

	// * add by hd
	COLORREF colorRectangle1 = 0;
	COLORREF colorRectangle2 = -1;
	if (m_colorHoverRectangle!=0 && IsHover())
	//if (m_colorHoverRectangle!=0 && (IsHover() || this->GetDroppedState()))	// ÏÔÊ¾
	{
		colorRectangle1 = m_colorHoverRectangle;
	}else if (m_colorNormalRectangle1!=0 && !IsHover())
	{
		colorRectangle1 = m_colorNormalRectangle1;
		colorRectangle2 = m_colorNormalRectangle2;	// Èý½Ç
	}
	if (colorRectangle1!=0)
	{
		//CBrush brushWrite(RGB(255,255,255));
		//CBrush* pOldBrush = dc.SelectObject(&brushWrite);
		//dc.FillRect(&rc,&brushWrite);
		//dc.SelectObject(pOldBrush);

		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, colorRectangle1);		//±ß¿òÑÕÉ«
		pOldPen = dc.SelectObject(&BorderPen);
		// »­Íâ¿ò
		dc.MoveTo(rc.left,rc.top);
		dc.LineTo(rc.right-1,rc.top);
		dc.LineTo(rc.right-1,rc.bottom-1);
		dc.LineTo(rc.left,rc.bottom-1);
		dc.LineTo(rc.left,rc.top);
		// »­ÄÚ¿ò
		dc.MoveTo(rc.right-4,rc.top+3);
		dc.LineTo(rc.right-4,rc.bottom-4);
		dc.LineTo(rc.right-19,rc.bottom-4);	// 19-3=16
		dc.LineTo(rc.right-19,rc.top+3);
		dc.LineTo(rc.right-4,rc.top+3);
		// »­Èý½Ç
		COLORREF colorRectangle = colorRectangle2==-1?colorRectangle1:colorRectangle2;
		CBrush brushBlue(colorRectangle);
		CBrush* pOldBrush = dc.SelectObject(&brushBlue);
		POINT point[3];
		point[0].x = rc.right-16;	// ÉÏ×ó
		point[0].y = rc.top+9;
		point[1].x = point[0].x+8;	// ÉÏÓÒ
		point[1].y = point[0].y;
		point[2].x = point[0].x+4;
		point[2].y = point[0].y+4;
		dc.Polygon(point,3);
		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);
		BorderPen.DeleteObject();
		brushBlue.DeleteObject();
	}

	CString sz;
	GetWindowText(sz);
	rc.left += nListBoxTextLeft;

	// change by hd
	////Select the font only if list box is created
	//if( m_listbox.GetSafeHwnd() )
 //		pbmpOld = dc.SelectObject(m_listbox.GetFont());
	pbmpOld = dc.SelectObject(this->GetFont());

	dc.SetBkMode(TRANSPARENT);
	
	RECT rcRepaint;
	GetClientRect(&rcRepaint);
	rcRepaint.left = rcRepaint.left + bitLeft.bmWidth-(m_bmLeft.m_hObject==NULL?1:0);		// -1 add by hd
	rcRepaint.right = rcRepaint.right - bitRight.bmWidth+(m_bmRight.m_hObject==NULL?3:0);	// +3 add by hd
	rcRepaint.top += 3;
	rcRepaint.bottom -= 3;
	//Set the normal/highlight color when its repainted 
/*
	if( GetFocus()->m_hWnd == m_hWnd )
	{
		dc.FillSolidRect(&rcRepaint,m_colBackGroundHighLight);
		dc.SetTextColor(m_colTextHighLight);
		TRACE("Filling rect again on focus high\n");
	}
	else
*/
	{
		dc.FillSolidRect(&rcRepaint,m_colBackGroundNormal);
		dc.SetTextColor(m_colTextNormal);
		//TRACE("Filling rect again on focus normal\n");
	}
	
	// add by yhz
	rc.top += 1;
	rc.right -= bitRight.bmWidth;
	//rc.bottom += 1;

	dc.DrawText(sz,&rc,DT_VCENTER | DT_SINGLELINE);
	// change by hd
	//if( m_listbox.GetSafeHwnd() )
		dc.SelectObject( pbmpOld );
/*	
	if( m_listbox )
	{

		 //TRACE("Painting the listbox\n");
		 CDC *pDC = m_listbox.GetDC();
		 m_listbox.GetClientRect(&rc);

		 CBrush pBrush;
		 pBrush.CreateSolidBrush(m_colBackGroundNormal);
		 
 	    pDC->FillRect(&rc,&pBrush);
	
		//Draw the left side of the list box
		DrawListLeft();
		//Draw the right side of the list box
		DrawListRight();

		//Draw the top of the list bar
		DrawListTop();

		//Draw the bottom of the list bar
		DrawListBot();
		
		pDC->SetTextColor(m_colTextNormal);
		CBrush brItemData;
		RECT rcItemData;
		CString szItemString;
		brItemData.CreateSolidBrush(m_colBackGroundNormal);
		for(int i =0;i<m_listbox.GetCount();i++)
		{
			m_listbox.GetText(i,szItemString);
			m_listbox.GetItemRect(i,&rcItemData);
			
			rcItemData.left += bitLeft.bmWidth+5;

			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_colTextNormal);

			nListBoxTextLeft = rcItemData.left;
			
			//CFont* def_font = pDC->SelectObject(&m_Font);
			pDC->SelectObject(m_listbox.GetFont());
			pDC->DrawText(szItemString,&rcItemData, DT_VCENTER | DT_SINGLELINE);
			//pDC->SelectObject(def_font);

		}
		m_listbox.ReleaseDC(pDC);
	}
*/	
		//if (m_colorHoverRectangle!=0 && IsHover())
		//{
		//	CRect rcRect;
		//	GetClientRect(&rcRect);

		//	CPen BorderPen,*pOldPen;
		//	BorderPen.CreatePen(PS_SOLID,1, m_colorHoverRectangle);		//±ß¿òÑÕÉ«
		//	pOldPen = dc.SelectObject(&BorderPen);
		//	dc.Rectangle(0,0,rcRect.Width(),rcRect.Height());
		//	dc.SelectObject(pOldPen);
		//}

		//MemDC.DeleteDC();
	// Do not call CComboBox::OnPaint() for painting messages
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:31 PM
// Function name	: CBitComboBox::Create
// Description	    : Sets the combo box properties to owner draw and fix.
// Return type		: BOOL 
// Argument         : LPCTSTR lpszClassName
// Argument         : LPCTSTR lpszWindowName
// Argument         : DWORD dwStyle
// Argument         : const RECT& rect
// Argument         : CWnd* pParentWnd
// Argument         : UINT nID
// Argument         : CCreateContext* pContext
//----------------------------------------------------------------------
BOOL CBitComboBox::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Remove the CBS_SIMPLE and CBS_DROPDOWN styles and add the one I'm designed for
	dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST;

	// Make sure to use the CBS_OWNERDRAWVARIABLE style
	dwStyle |= CBS_OWNERDRAWVARIABLE;

	// Use default strings. We need the itemdata to store the state of the lamps
	dwStyle |= CBS_HASSTRINGS;	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:26 PM
// Function name	: CBitComboBox::OnDestroy
// Description	    : Releases the sub classed control
// Return type		: void 
//----------------------------------------------------------------------
void CBitComboBox::OnDestroy() 
{
      if (m_listbox.GetSafeHwnd() != NULL)
         m_listbox.UnsubclassWindow();

	CComboBox::OnDestroy();	
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:24 PM
// Function name	: CBitComboBox::DrawItem
// Description	    : Draws each item selected and unselected
// Return type		: void 
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//----------------------------------------------------------------------
void CBitComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	//TRACE("DrawItem()\n");
	if( lpDrawItemStruct->itemID == -1 )
		return;

	if( m_listbox.GetSafeHwnd() == NULL )
		return;

	CDC			pDC;	
	pDC.Attach(lpDrawItemStruct->hDC);
	CBrush pBrush;
	CString		sItem;

	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		if (!pBrush.CreateSolidBrush(m_colBackGroundHighLight)) return;
		CRect Rect = lpDrawItemStruct->rcItem;
		pDC.FillRect(&Rect, &pBrush);
		//pDC.SetTextColor(RGB(255, 0, 0));
		pDC.SetTextColor(m_colTextHighLight);
		pBrush.DeleteObject();
	}
	else
	{
		if (!pBrush.CreateSolidBrush(m_colBackGroundNormal)) return;
		CRect Rect = lpDrawItemStruct->rcItem;
		pDC.FillRect(&Rect, &pBrush);
		//pDC.SetTextColor(RGB(255, 0, 0));
		pDC.SetTextColor(m_colTextNormal);
		pBrush.DeleteObject();
	}
	
	//Draw the left image of the list box
	DrawListLeft();
	//Draw the right image of the list box
	DrawListRight();
	//Draw the bottom for the last item
	if( lpDrawItemStruct->itemID == m_listbox.GetCount()-1 )
		DrawListBot();
	//Draw the top for the first item
	if( lpDrawItemStruct->itemID == 0 )
		DrawListTop();

	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text after the images left postion

	// add by yhz
	//lpDrawItemStruct->rcItem.top += 2;
	//lpDrawItemStruct->rcItem.bottom += 2;

	lpDrawItemStruct->rcItem.left = nListBoxTextLeft;
	// change by hd
	//if (m_listbox.GetSafeHwnd())
	//	pDC.SelectObject(m_listbox.GetFont());
	CFont * pOldFont = pDC.SelectObject(this->GetFont());
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);
	pDC.SelectObject(pOldFont);
}

//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:20 PM
// Function name	: CBitComboBox::SetComboBitmap
// Description	    : To save the resource IDs of images for main combo box skin
// Return type		: void 
// Argument         : UINT nIDLeft
// Argument         : UINT nIDRight
// Argument         : UINT nIDCenter
//----------------------------------------------------------------------
void CBitComboBox::SetComboBitmap(HBITMAP nIDLeft, HBITMAP nIDRight, HBITMAP nIDCenter)
{
	SAFE_DELETEOBJECT(m_bmCenter.Detach());
	SAFE_DELETEOBJECT(m_bmLeft.Detach());
	SAFE_DELETEOBJECT(m_bmRight.Detach());

	m_bmCenter.Attach(nIDCenter);
	m_bmLeft.Attach(nIDLeft);
	m_bmRight.Attach(nIDRight);
}
void CBitComboBox::SetComboBitmap(UINT nIDLeft, UINT nIDRight, UINT nIDCenter)
{
	SAFE_DELETEOBJECT(m_bmCenter.Detach());
	SAFE_DELETEOBJECT(m_bmLeft.Detach());
	SAFE_DELETEOBJECT(m_bmRight.Detach());

	m_bmCenter.LoadBitmap(nIDCenter);
	m_bmLeft.LoadBitmap(nIDLeft);
	m_bmRight.LoadBitmap(nIDRight);
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:18:08 PM
// Function name	: CBitComboBox::SetComboListBitmap
// Description	    : To save the resource IDs of images for list box skin
// Return type		: void 
// Argument         : UINT nIDListLeft
// Argument         : UINT nIDListRight
// Argument         : UINT nIDListTop
// Argument         : UINT nIDListBot
//----------------------------------------------------------------------
void CBitComboBox::SetComboListBitmap(HBITMAP nIDListLeft, HBITMAP nIDListRight, HBITMAP nIDListTop, HBITMAP nIDListBot)
{
	SAFE_DELETEOBJECT(m_bmListLeft.Detach());
	SAFE_DELETEOBJECT(m_bmListRight.Detach());
	SAFE_DELETEOBJECT(m_bmListTop.Detach());
	SAFE_DELETEOBJECT(m_bmListBot.Detach());

	m_bmListLeft.Attach(nIDListLeft);
	m_bmListRight.Attach(nIDListRight);
	m_bmListTop.Attach(nIDListTop);
	m_bmListBot.Attach(nIDListBot);
}
void CBitComboBox::SetComboListBitmap(UINT nIDListLeft, UINT nIDListRight, UINT nIDListTop, UINT nIDListBot)
{
	SAFE_DELETEOBJECT(m_bmListLeft.Detach());
	SAFE_DELETEOBJECT(m_bmListRight.Detach());
	SAFE_DELETEOBJECT(m_bmListTop.Detach());
	SAFE_DELETEOBJECT(m_bmListBot.Detach());

	m_bmListLeft.LoadBitmap(nIDListLeft);
	m_bmListRight.LoadBitmap(nIDListRight);
	m_bmListTop.LoadBitmap(nIDListTop);
	m_bmListBot.LoadBitmap(nIDListBot);
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:17 PM
// Function name	: CBitComboBox::DrawListLeft
// Description	    : Draws the left image of the list box
// Return type		: void 
// Argument         : int nHeight
//----------------------------------------------------------------------
void CBitComboBox::DrawListLeft(int nHeight)
{
	RECT rc;
	CDC MemDC;
	CDC *pDC ;

	if( m_listbox.GetSafeHwnd() == NULL )
		return;
	pDC = m_listbox.GetDC();
	MemDC.CreateCompatibleDC(pDC);
	if (MemDC.m_hDC == NULL)
	{
		m_listbox.ReleaseDC(pDC);
		return;
	}
	m_listbox.GetClientRect(&rc);

	//CBitmap bmpImage;
	HGDIOBJ pbmpOld;
	BITMAP bitImage;

	//if( m_nIDListLeft )
	if (m_bmListLeft.m_hObject)
	{
		//bmpImage.LoadBitmap(m_nIDListLeft);
		m_bmListLeft.GetBitmap(&bitImage);
		pbmpOld = MemDC.SelectObject(m_bmListLeft);
		pDC->StretchBlt(rc.left,rc.top,bitImage.bmWidth,rc.bottom,&MemDC,0,0,bitImage.bmWidth,bitImage.bmHeight,SRCCOPY);
		MemDC.SelectObject( pbmpOld );
	}
	m_listbox.ReleaseDC(pDC);
	MemDC.DeleteDC();
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:03:10 PM
// Function name	: CBitComboBox::DrawListRight
// Description	    : Draws the right image of the list box
// Return type		: void 
// Argument         : int nHeight
//----------------------------------------------------------------------
void CBitComboBox::DrawListRight(int nHeight)
{
	RECT rc;
	CDC MemDC;
	CDC *pDC ;

	if( m_listbox.GetSafeHwnd() == NULL )
		return;
	pDC = m_listbox.GetDC();
	MemDC.CreateCompatibleDC(pDC);
	if (MemDC.m_hDC == NULL)
	{
		m_listbox.ReleaseDC(pDC);
		return;
	}
	m_listbox.GetClientRect(&rc);

	//CBitmap bmpImage;
	BITMAP bitImage;
	HGDIOBJ pbmpOld;

	//if( m_nIDListRight )
	if (m_bmListRight.m_hObject)
	{
		//bmpImage.LoadBitmap(m_nIDListRight);
		m_bmListRight.GetBitmap(&bitImage);
		pbmpOld = MemDC.SelectObject(m_bmListRight);
		pDC->StretchBlt(rc.right-bitImage.bmWidth,rc.top,bitImage.bmWidth,rc.bottom,&MemDC,0,0,bitImage.bmWidth,bitImage.bmHeight,SRCCOPY);
		MemDC.SelectObject( pbmpOld );
	}
	m_listbox.ReleaseDC(pDC);
	MemDC.DeleteDC();
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:02:36 PM
// Function name	: CBitComboBox::DrawListTop
// Description	    : Draws the top image of the list box
// Return type		: void 
// Argument         : int nWidth
//----------------------------------------------------------------------
void CBitComboBox::DrawListTop(int nWidth)
{
	RECT rc;
	CDC MemDC;
	CDC *pDC = m_listbox.GetDC();
	MemDC.CreateCompatibleDC(pDC);
	if (MemDC.m_hDC == NULL)
	{
		m_listbox.ReleaseDC(pDC);
		return;
	}
	m_listbox.GetClientRect(&rc);

	//CBitmap bmpImage;
	BITMAP bitImage;
	HGDIOBJ pbmpOld;

	//if( m_nIDListTop )
	if (m_bmListTop.m_hObject)
	{
		//bmpImage.LoadBitmap(m_nIDListTop);
		m_bmListTop.GetBitmap(&bitImage);
		pbmpOld = MemDC.SelectObject(m_bmListTop);
		pDC->StretchBlt(rc.left,rc.top,rc.right,bitImage.bmHeight,&MemDC,0,0,bitImage.bmWidth,bitImage.bmHeight,SRCCOPY);
		MemDC.SelectObject( pbmpOld );
	}		
	m_listbox.ReleaseDC(pDC);
	MemDC.DeleteDC();
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:02:31 PM
// Function name	: CBitComboBox::DrawListBot
// Description	    : Draws the bottom image of the list box
// Return type		: void 
// Argument         : int nWidth
//----------------------------------------------------------------------
void CBitComboBox::DrawListBot(int nWidth)
{
	//Width must be check, sometimes we may not need to stretch to the whole control's width
	RECT rc;
	CDC MemDC;
	CDC *pDC = m_listbox.GetDC();
	MemDC.CreateCompatibleDC(pDC);
	if (MemDC.m_hDC == NULL)
	{
		m_listbox.ReleaseDC(pDC);
		return;
	}
	m_listbox.GetClientRect(&rc);

	//CBitmap bmpImage;
	BITMAP bitImage;
	HGDIOBJ pbmpOld;

	//if( m_nIDListBot )
	if (m_bmListBot.m_hObject)
	{
		//bmpImage.LoadBitmap(m_nIDListBot);
		m_bmListBot.GetBitmap(&bitImage);
		pbmpOld = MemDC.SelectObject(m_bmListBot);
		pDC->StretchBlt(rc.left,rc.bottom-bitImage.bmHeight,rc.right,bitImage.bmHeight,&MemDC,0,0,bitImage.bmWidth,bitImage.bmHeight,SRCCOPY);
		MemDC.SelectObject( pbmpOld );
	}
	m_listbox.ReleaseDC(pDC);
	MemDC.DeleteDC();
}



//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:13:57 PM
// Function name	: CBitComboBox::SetHighlightColor
// Description	    : To save the color values for text and background 
//						to draw the listbos in highlight position. Must be set before using the CBitComboBox
// Return type		: void 
// Argument         : COLORREF colBackGround
// Argument         : COLORREF colText
//----------------------------------------------------------------------
void CBitComboBox::SetHighlightColor(COLORREF colBackGround, COLORREF colText)
{
	m_colBackGroundHighLight = colBackGround; m_colTextHighLight = colText;
}


//----------------------------------------------------------------------
// Author			: Ashok Jaiswal
// Date Time		: 3/23/2004 5:13:54 PM
// Function name	: CBitComboBox::SetNormalPositionColor
// Description	    : To save the color values for text and background 
//						to draw the listbos in normal position. Must be set before using the CBitComboBox
// Return type		: void 
// Argument         : COLORREF colBackGround
// Argument         : COLORREF colText
//----------------------------------------------------------------------
void CBitComboBox::SetNormalPositionColor(COLORREF colBackGround, COLORREF colText)
{
	m_colBackGroundNormal = colBackGround; m_colTextNormal = colText;
}


BOOL CBitComboBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CComboBox::OnEraseBkgnd(pDC);
}

HBRUSH CBitComboBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}

void CBitComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);

   if (lpMeasureItemStruct->itemID != (UINT) -1)
   {
      LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
      //ASSERT(lpszText != NULL);
	  if( lpszText == NULL )
		  return;
      CSize   sz;
      CDC*    pDC = GetDC();
	  CRect		Rect;
	  GetClientRect( &Rect );

      sz = pDC->GetTextExtent(lpszText);

      ReleaseDC(pDC);
	
	  lpMeasureItemStruct->itemWidth = Rect.Width();
      lpMeasureItemStruct->itemHeight = 1+sz.cy;

   }

	
}
void CBitComboBox::OnHoverLeave()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}
void CBitComboBox::OnHoverEnter()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}
//void CBitComboBox::OnSetFocus(CWnd* pOldWnd)
//{
//	//RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
//}
//void CBitComboBox::OnKillFocus(CWnd* pNewWnd)
//{
//	//AfxMessageBox(_T("CBitComboBox::OnKillFocus"));
//	//if (m_bHover && pNewWnd->GetSafeHwnd()!=m_listbox.GetSafeHwnd())
//	//{
//	//	m_bHover = FALSE;
//	//	OnHoverLeave();
//	//}
//	//RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
//	CComboBox::OnKillFocus(pNewWnd);
//}

void CBitComboBox::OnCbnCloseup()
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		OnHoverLeave();
	}
	//SetTimer(TIMER_REDRAW,10,NULL);
	//RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}
//void CBitComboBox::OnCbnDropdown()
//{
//	if (!m_bTracking)
//	{
//		TRACKMOUSEEVENT Tme;
//		Tme.cbSize = sizeof(Tme);
//		Tme.hwndTrack = GetSafeHwnd();
//		Tme.dwFlags = TME_LEAVE|TME_HOVER;
//		Tme.dwHoverTime = 1;
//		if (_TrackMouseEvent(&Tme))
//			m_bTracking=TRUE;
//	}
//}
void CBitComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT Tme;
		Tme.cbSize = sizeof(Tme);
		Tme.hwndTrack = GetSafeHwnd();
		Tme.dwFlags = TME_LEAVE;
		//Tme.dwFlags = TME_LEAVE|TME_HOVER;
		Tme.dwHoverTime = 1;
		if (_TrackMouseEvent(&Tme))
			m_bTracking=TRUE;
	}
	if (!m_bHover)
	{
		m_bHover = TRUE;
		OnHoverEnter();
		SetTimer(TIMER_CHECK_HOVER,500,NULL);
		if (m_bDoubleDrawHover)
		{
			SetTimer(TIMER_DOUBLE_DRAW_HOVER,500,NULL);
		}
	}
	CComboBox::OnMouseMove(nFlags, point);
}
//#ifdef USES_MESSAGE_HOVERLEAVE
//LRESULT CBitComboBox::OnMouseHover(WPARAM wp,LPARAM lp) 
//#else
//void CBitComboBox::OnMouseHover(UINT nFlags, CPoint point) 
//#endif
//{
//	if (!m_bHover)
//	{
//		m_bHover = TRUE;
//		OnHoverEnter();
//	}
//#ifdef USES_MESSAGE_HOVERLEAVE
//	return 0;
//#else
//	CComboBox::OnMouseHover(nFlags, point);
//#endif
//}
#ifdef USES_MESSAGE_HOVERLEAVE
LRESULT CBitComboBox::OnMouseLeave(WPARAM wp,LPARAM lp) 
#else
void CBitComboBox::OnMouseLeave(void) 
#endif
{
	CPoint point;
	GetCursorPos(&point);
	this->ScreenToClient(&point);

	CRect rect;
	GetClientRect(rect);
	rect.left += 1;	// *
	rect.right -= 1;
	if(rect.PtInRect(point))
	{
		//if (!m_bHover)
		//{
		//	m_bHover = TRUE;
		//	OnHoverEnter();
		//	SetTimer(TIMER_CHECK_HOVER,500,NULL);
		//}
	}else
	{
		if (m_bHover)
		{
			m_bHover = FALSE;
			OnHoverLeave();
		}
	}
	m_bTracking = FALSE;
#ifdef USES_MESSAGE_HOVERLEAVE
	return 0;
#else
	CComboBox::OnMouseLeave();
#endif
}
void CBitComboBox::OnTimer(UINT nIDEvent)
{
	if (TIMER_CHECK_HOVER==nIDEvent)
	{
		if (m_bHover)
		{
			if (!this->GetDroppedState())
			{
				CPoint point;
				GetCursorPos(&point);
				this->ScreenToClient(&point);
				CRect rect;
				GetClientRect(rect);
				if(!rect.PtInRect(point))
				{
					m_bHover = FALSE;
					OnHoverLeave();
					KillTimer(nIDEvent);
				}
			}
		}else
		{
			KillTimer(nIDEvent);
		}
	}else if (TIMER_DOUBLE_DRAW_HOVER==nIDEvent)
	{
		KillTimer(nIDEvent);
		if (m_bHover)
		{
			OnHoverLeave();
		}
	}
	CComboBox::OnTimer(nIDEvent);
}

int CBitComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//lpCreateStruct->style &= ~0xF;
	//lpCreateStruct->style |= CBS_DROPDOWNLIST;

	//// Make sure to use the CBS_OWNERDRAWVARIABLE style
	//lpCreateStruct->style |= CBS_OWNERDRAWVARIABLE;

	//// Use default strings. We need the itemdata to store the state of the lamps
	//lpCreateStruct->style |= CBS_HASSTRINGS;	

	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}
