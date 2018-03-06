// MrSkinVBar.cpp : implementation file
//

#include "stdafx.h"
#include "MrSkinVBar.h"
#include "SkinMemDC.h"

#define SAFE_DELETEOBJECT(obj) {if (obj) DeleteObject(obj);}

#define TIMER_1 101
#define TIMER_2 102

// CMrSkinVBar

IMPLEMENT_DYNAMIC(CMrSkinVBar, CStatic)

CMrSkinVBar::CMrSkinVBar()
{
	//m_hbmVerticalBarThumb = m_hbmVerticalBarBottom = m_hbmVerticalBarDownArrow = \
	//m_hbmVerticalBarSpan = m_hbmVerticalBarUpArrow = m_hbmVerticalBarTop = 0;
	//m_bmVerticalBarThumb = m_bmVerticalBarBottom = m_bmVerticalBarDownArrow = \
	//m_bmVerticalBarSpan = m_bmVerticalBarUpArrow = m_bmVerticalBarTop = 0;

	m_bMouseDown = false;
	m_bMouseDownArrowUp = false;
	m_bMouseDownArrowDown = false;
	m_bDragging = false;

	m_nThumbTop = 36;
	m_dbThumbInterval = 0.000000;
	m_pScrollParent = NULL;
	m_pCallback = NULL;
	memset(&m_stBitmapUpArrow,0,sizeof(BITMAP));
	memset(&m_stBitmapBottom,0,sizeof(BITMAP));
	memset(&m_stBitmapDownArrow,0,sizeof(BITMAP));
	memset(&m_stBitmapThumb,0,sizeof(BITMAP));
	memset(&m_stBitmapTop,0,sizeof(BITMAP));
	//memset(&m_stBitmapUpArrow,0,sizeof(BITMAP));
}

CMrSkinVBar::~CMrSkinVBar()
{
	SAFE_DELETEOBJECT(m_bmVerticalBarUpArrow.Detach());
	SAFE_DELETEOBJECT(m_bmVerticalBarSpan.Detach());
	SAFE_DELETEOBJECT(m_bmVerticalBarDownArrow.Detach());
	SAFE_DELETEOBJECT(m_bmVerticalBarThumb.Detach());

	SAFE_DELETEOBJECT(m_bmVerticalBarTop.Detach());
	SAFE_DELETEOBJECT(m_bmVerticalBarBottom.Detach());
}


BEGIN_MESSAGE_MAP(CMrSkinVBar, CStatic)
	//{{AFX_MSG_MAP(CMrSkinVBar)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMrSkinVBar message handlers

void CMrSkinVBar::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TIMER_1)
	{
		if( m_bMouseDownArrowDown )
		{
			ScrollDown();
		}
		
		if( m_bMouseDownArrowUp )
		{
			ScrollUp();
		}
	}
	else if(nIDEvent == TIMER_2)
	{
		if( m_bMouseDownArrowDown )
		{
			KillTimer(TIMER_2);
			SetTimer(TIMER_1, 50, NULL);
		}
		
		if( m_bMouseDownArrowUp )
		{
			KillTimer(TIMER_2);
			SetTimer(TIMER_1, 50, NULL);
		}
	}
	
	CStatic::OnTimer(nIDEvent);
}

void CMrSkinVBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if( m_bMouseDown )
	{
		m_nThumbTop = point.y-m_stBitmapThumb.bmHeight/2; //-13 so mouse is in middle of thumb
		
		double nMax = m_pCallback->GetScrollLimit();
		double nPos = m_pCallback->GetScrollPos();
		double nHeight = clientRect.Height()-m_stBitmapThumb.bmHeight-m_stBitmapUpArrow.bmHeight-m_stBitmapDownArrow.bmHeight-m_stBitmapTop.bmHeight-m_stBitmapBottom.bmHeight;
		double nVar = nMax;
		m_dbThumbInterval = nHeight/nVar;

		//figure out how many times to scroll total from top
		//then minus the current position from it
		int nScrollTimes = (int)((m_nThumbTop-m_stBitmapUpArrow.bmHeight-m_stBitmapTop.bmHeight)/m_dbThumbInterval)-nPos;
		if (abs(nScrollTimes) > 20)
			m_pScrollParent->SendMessage(MR_WM_SKIN_SCROLL_CY,nScrollTimes);

		//grab the row height dynamically
		//so if the font size or type changes
		//our scroll will still work properly
		//CRect itemrect;
		//m_pScrollParent->GetItemRect(0,&itemrect);
		//CSize size;
		//size.cx = 0;
		//size.cy = nScrollTimes*itemrect.Height();
		//m_pScrollParent->Scroll(size);

		LimitThumbPosition();

		CClientDC dc(this);
		Draw(&dc);
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CMrSkinVBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	int nHeight = clientRect.Height() - m_stBitmapUpArrow.bmHeight-m_stBitmapTop.bmHeight;

	CRect rectUpArrow(0,m_stBitmapTop.bmHeight,m_stBitmapUpArrow.bmWidth,m_stBitmapUpArrow.bmHeight+m_stBitmapTop.bmHeight);
	CRect rectDownArrow(0,nHeight,m_stBitmapUpArrow.bmWidth,nHeight+m_stBitmapUpArrow.bmHeight);
	CRect rectThumb(0,m_nThumbTop,m_stBitmapThumb.bmWidth,m_nThumbTop+m_stBitmapThumb.bmHeight);

	if(rectThumb.PtInRect(point))
	{
		m_bMouseDown = true;
	}

	if(rectDownArrow.PtInRect(point))
	{
		m_bMouseDownArrowDown = true;
		SetTimer(TIMER_2,250,NULL);
	}

	if(rectUpArrow.PtInRect(point))
	{
		m_bMouseDownArrowUp = true;
		SetTimer(TIMER_2,250,NULL);
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CMrSkinVBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateThumbPosition();
	KillTimer(TIMER_1);
	ReleaseCapture();
	
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	int nHeight = clientRect.Height() - m_stBitmapUpArrow.bmHeight-m_stBitmapTop.bmHeight;
	CRect rectUpArrow(0,m_stBitmapTop.bmHeight,m_stBitmapUpArrow.bmWidth,m_stBitmapUpArrow.bmHeight+m_stBitmapTop.bmHeight);
	CRect rectDownArrow(0,nHeight,m_stBitmapDownArrow.bmWidth,nHeight+m_stBitmapDownArrow.bmHeight);
	CRect rectThumb(0,m_nThumbTop,m_stBitmapThumb.bmWidth ,m_nThumbTop+m_stBitmapThumb.bmHeight);



	if(rectUpArrow.PtInRect(point) && m_bMouseDownArrowUp)
	{
		ScrollUp();	
		bInChannel = false;
	}

	if(rectDownArrow.PtInRect(point) && m_bMouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if(rectThumb.PtInRect(point))
	{
		bInChannel = false;
	}

	if(bInChannel == true && !m_bMouseDown)
	{
		if(point.y > m_nThumbTop)
		{
			PageDown();
		}
		else
		{
			PageUp();
		}
	}

	m_bMouseDown = false;
	m_bDragging = false;
	m_bMouseDownArrowUp = false;
	m_bMouseDownArrowDown = false;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CMrSkinVBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
}

void CMrSkinVBar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	//if(m_nThumbTop+m_stBitmapUpArrow.bmHeight > (clientRect.Height()-m_stBitmapDownArrow.bmHeight-m_stBitmapBottom.bmHeight))
	if(m_nThumbTop > (clientRect.bottom-m_stBitmapDownArrow.bmHeight-m_stBitmapBottom.bmHeight-m_stBitmapThumb.bmHeight))
	{
		//m_nThumbTop = clientRect.Height()-m_stBitmapDownArrow.bmHeight-m_stBitmapBottom.bmHeight-m_stBitmapThumb.bmHeight;
		m_nThumbTop = clientRect.bottom-m_stBitmapDownArrow.bmHeight-m_stBitmapBottom.bmHeight-m_stBitmapThumb.bmHeight;
	}

	if( m_nThumbTop < (clientRect.top+m_stBitmapUpArrow.bmHeight+m_stBitmapTop.bmHeight))
	{
		m_nThumbTop = clientRect.top+m_stBitmapUpArrow.bmHeight+m_stBitmapTop.bmHeight;
	}
}

void CMrSkinVBar::Draw(CDC * pDC)
{
	//CClientDC dc(this);

	//下面这段代码没有实际的用处，欺骗devpartner
//	CFont *pFontDC;
//	CFont fontDC;	
//	LOGFONT logfont;
//	logfont.lfWeight = 700;
//	fontDC.CreateFontIndirect( &logfont );
//	pFontDC = dc.SelectObject( &fontDC );

//	dc.SelectObject( pFontDC );
	//end
	
	CDC bitmapDC;
	if (!bitmapDC.CreateCompatibleDC(pDC))
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	//CSkinMemDC *pmemDC = new CSkinMemDC(&dc, &clientRect);
	//if (pmemDC->m_hDC == NULL)
	//{
	//	bitmapDC.DeleteDC();
	//	delete pmemDC;
	//	return;
	//}

	CSkinMemDC memDC(pDC, &clientRect);
	if (memDC.m_hDC == NULL)
	{
		bitmapDC.DeleteDC();
		return;
	}
	memDC.FillSolidRect(&clientRect,  RGB(74,82,107));


	//CBitmap bitmap;
	BITMAP	stBitmapTop = m_stBitmapTop;
	CBitmap*pOldBitmap = NULL;
/*
	bitmap.LoadBitmap( m_bmVerticalBarTop );
	bitmap.GetBitmap( &stBitmapTop );

	CBitmap* pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,clientRect.top,stBitmapTop.bmWidth,stBitmapTop.bmHeight,&bitmapDC,0,0,SRCCOPY);
	
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
*/
	BITMAP stBitmapUpArrow;
	memset(&stBitmapUpArrow,0,sizeof(BITMAP));
	if (m_bmVerticalBarUpArrow.m_hObject!=NULL)
	{
		m_bmVerticalBarUpArrow.GetBitmap( &stBitmapUpArrow );
		pOldBitmap = bitmapDC.SelectObject(&m_bmVerticalBarUpArrow);
		memDC.BitBlt(clientRect.left,clientRect.top+stBitmapTop.bmHeight,stBitmapUpArrow.bmWidth,stBitmapUpArrow.bmHeight,&bitmapDC,0,0,SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap);
		//bitmap.DeleteObject();
		pOldBitmap = NULL;
	}
	
	//draw the background (span)
	BITMAP stBitmapSpan;
	memset(&stBitmapSpan,0,sizeof(BITMAP));
	int nHeight = 0;
	if (m_bmVerticalBarSpan.m_hObject!=NULL)
	{
		m_bmVerticalBarSpan.GetBitmap( &stBitmapSpan );
		pOldBitmap = bitmapDC.SelectObject(&m_bmVerticalBarSpan);
		nHeight = clientRect.Height() - stBitmapTop.bmHeight-stBitmapUpArrow.bmHeight;
		memDC.StretchBlt(clientRect.left, clientRect.top+stBitmapTop.bmHeight+stBitmapUpArrow.bmHeight, stBitmapSpan.bmWidth,nHeight,&bitmapDC, 0,0, stBitmapSpan.bmWidth, stBitmapSpan.bmHeight, SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap);
		//bitmap.DeleteObject();
		pOldBitmap = NULL;
	}
	
	//draw the down arrow of the scrollbar
	BITMAP stBitmapDownArrow;
	memset(&stBitmapDownArrow,0,sizeof(BITMAP));
	if (m_bmVerticalBarDownArrow.m_hObject!=NULL)
	{
		m_bmVerticalBarDownArrow.GetBitmap( &stBitmapDownArrow );
		pOldBitmap = bitmapDC.SelectObject(&m_bmVerticalBarDownArrow);
		memDC.BitBlt(clientRect.left,nHeight,stBitmapDownArrow.bmWidth , stBitmapDownArrow.bmHeight ,&bitmapDC,0,0,SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap);
		//bitmap.DeleteObject();
		pOldBitmap = NULL;
	}

	//draw the down arrow of the scrollbar
	BITMAP stBitmapBottom = m_stBitmapBottom;
/*
	bitmap.LoadBitmap( m_bmVerticalBarBottom );
	bitmap.GetBitmap( &stBitmapBottom );
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left+1,nHeight+stBitmapDownArrow.bmHeight,stBitmapBottom.bmWidth,stBitmapBottom.bmHeight,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
*/
	if (m_bmVerticalBarThumb.m_hObject!=NULL)
	{
		//If there is nothing to scroll then don't
		//show the thumb control otherwise show it

		int nMax = m_pCallback->GetScrollLimit();
		if (nMax != 0)
		{
			//draw the thumb control
			BITMAP	stBitmapThumb;
			m_bmVerticalBarThumb.GetBitmap( &stBitmapThumb );
			pOldBitmap = bitmapDC.SelectObject(&m_bmVerticalBarThumb);
			memDC.BitBlt(clientRect.left,clientRect.top+m_nThumbTop, stBitmapThumb.bmWidth , stBitmapThumb.bmHeight ,&bitmapDC,0,0,SRCCOPY);
			bitmapDC.SelectObject(pOldBitmap);
			//bitmap.DeleteObject();
			pOldBitmap = NULL;
		}
	}

	//memDC.DeleteDC();
	bitmapDC.DeleteDC();

	//dc.SelectObject( pFontDC );
}

void CMrSkinVBar::UpdateThumbPosition()
{
	if( this->m_hWnd == NULL )
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	if( m_pScrollParent == NULL )
		return;

	double nMax = m_pCallback->GetScrollLimit();
	double nPos = m_pCallback->GetScrollPos();
	//double nPos = m_pScrollParent->GetScrollPos(SB_VERT);
	//double nMax = m_pScrollParent->GetScrollLimit(SB_VERT);
	double nHeight = (clientRect.Height()-m_stBitmapTop.bmHeight-m_stBitmapUpArrow.bmHeight-m_stBitmapThumb.bmHeight-m_stBitmapDownArrow.bmHeight-m_stBitmapBottom.bmHeight/*98*/);//top+uparrow+downarrow+bottom
	double nVar = nMax;

	m_dbThumbInterval = nHeight/nVar;

	double nNewdbValue = (m_dbThumbInterval * nPos);
	int nNewValue = (int)nNewdbValue;

	m_nThumbTop = m_stBitmapTop.bmHeight+m_stBitmapUpArrow.bmHeight+nNewValue; //36 = top+uparrow

	LimitThumbPosition();

	CClientDC dc(this);
	Draw(&dc);
}

void CMrSkinVBar::PageDown()
{
	m_pScrollParent->SendMessage(MR_WM_VSCROLL, SB_PAGEDOWN, NULL);
	//m_pScrollParent->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CMrSkinVBar::PageUp()
{
	m_pScrollParent->SendMessage(MR_WM_VSCROLL, SB_PAGEUP, NULL);
	//m_pScrollParent->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CMrSkinVBar::ScrollUp()
{
	m_pScrollParent->SendMessage(MR_WM_VSCROLL, SB_LINEUP, NULL);
	//m_pScrollParent->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CMrSkinVBar::ScrollDown()
{
	m_pScrollParent->SendMessage(MR_WM_VSCROLL, SB_LINEDOWN, NULL);
	//m_pScrollParent->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CMrSkinVBar::CalcBmpSize()
{
	m_bmVerticalBarUpArrow.GetBitmap( &m_stBitmapUpArrow );
	m_bmVerticalBarBottom.GetBitmap( &m_stBitmapBottom );
	m_bmVerticalBarDownArrow.GetBitmap( &m_stBitmapDownArrow );
	m_bmVerticalBarThumb.GetBitmap( &m_stBitmapThumb );
	m_bmVerticalBarTop.GetBitmap( &m_stBitmapTop );

	m_stBitmapTop.bmHeight = 0;
	m_stBitmapBottom.bmHeight = 0;
}


// CMrSkinVBar message handlers


