// EBSkinVBar.cpp : Implementation of CEBSkinVBar
#include "stdafx.h"
#include "EBSkinVBar.h"

#define TIMER_1 101
#define TIMER_2 102
#define TIMER_3 103	// 用于检测，

// CEBSkinVBar
HRESULT CEBSkinVBar::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	HDC hDC = di.hdcDraw;

	LONG nMax = 0;
	if (m_pCallback != NULL)
	{
		m_pCallback->GetScrollMax(&nMax);
	}
	if (nMax>0)
	{
		CSkinMemoryDC memDC(hDC,rc);
		Gdiplus::Graphics graphics(memDC.GetDC());
		graphics.FillRectangle(&m_brushBackground,memDC.GetLeft(),memDC.GetTop(),memDC.Width(),memDC.Height());
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
		// 画顶上箭头
		DrawTopArrow(graphics,Rect(m_nLeftInterval,m_nTopInterval,m_sizeTopArrow.cx,m_sizeTopArrow.cy));
		// 画上面箭头
		DrawUpArrow(graphics,Rect(m_nLeftInterval,m_nTopInterval+m_sizeTopArrow.cy,m_sizeUpArrow.cx,m_sizeUpArrow.cy));
		// 画下面箭头
		DrawDownArrow(graphics,Rect(m_nLeftInterval,memDC.Height()-m_nTopInterval-m_sizeBottomArrow.cy-m_sizeDownArrow.cy-1,m_sizeDownArrow.cx,m_sizeDownArrow.cy));
		// 画顶下箭头
		DrawBottomArrow(graphics,Rect(m_nLeftInterval,memDC.Height()-m_nTopInterval-m_sizeBottomArrow.cy-1,m_sizeBottomArrow.cx,m_sizeBottomArrow.cy));
		// 画中间选择条
		Gdiplus::Rect rectThumb(m_nLeftInterval+1,m_nThumbTop,m_sizeThumb.cx-2,m_sizeThumb.cy);
		if (m_bMoveInThumb)
		{
			Gdiplus::SolidBrush brushArrow(Gdiplus::Color(104,108,111));
			graphics.FillRectangle(&brushArrow,rectThumb);
		}else
		{
			Gdiplus::SolidBrush brushArrow(Gdiplus::Color(145,148,150));
			graphics.FillRectangle(&brushArrow,rectThumb);
		}
	}
	return S_OK;
}
void CEBSkinVBar::DrawTopArrow(Gdiplus::Graphics& graphics,const Rect& rectTopArrow)
{
	graphics.FillRectangle(&m_brushBackground,rectTopArrow);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
	Gdiplus::Pen penArrow(Gdiplus::Color(145,148,150));
	graphics.DrawLine(&penArrow,rectTopArrow.GetLeft()+1,rectTopArrow.GetTop()+2,rectTopArrow.GetRight()-1,rectTopArrow.GetTop()+2);
	graphics.DrawLine(&penArrow,rectTopArrow.GetLeft()+1,rectTopArrow.GetBottom()-1,rectTopArrow.GetLeft()+rectTopArrow.Width/2,rectTopArrow.GetTop()+2);
	graphics.DrawLine(&penArrow,rectTopArrow.GetLeft()+rectTopArrow.Width/2,rectTopArrow.GetTop()+2,rectTopArrow.GetRight()-1,rectTopArrow.GetBottom()-1);
	if (m_bMoveInTopArrow)
	{
		graphics.DrawRectangle(&penArrow,rectTopArrow);
	}
}
void CEBSkinVBar::DrawUpArrow(Gdiplus::Graphics& graphics,const Rect& rectUpArrow)
{
	graphics.FillRectangle(&m_brushBackground,rectUpArrow);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
	Gdiplus::SolidBrush brushArrow(Gdiplus::Color(145,148,150));
	Gdiplus::Point polygonUp[3];	// 上面箭头
	polygonUp[0].X = rectUpArrow.GetLeft() + rectUpArrow.Width/2;
	polygonUp[0].Y = rectUpArrow.GetTop()+1;
	polygonUp[1].X = rectUpArrow.GetLeft()+1;
	polygonUp[1].Y = rectUpArrow.GetTop() + rectUpArrow.Height-1;
	polygonUp[2].X = rectUpArrow.GetLeft() + rectUpArrow.Width-1;
	polygonUp[2].Y = rectUpArrow.GetTop() + rectUpArrow.Height-1;
	graphics.FillPolygon(&brushArrow,polygonUp,3);
	if (m_bMoveInUpArrow)
	{
		Gdiplus::Pen penArrow(Gdiplus::Color(145,148,150));
		graphics.DrawRectangle(&penArrow,rectUpArrow);
	}
}
void CEBSkinVBar::DrawDownArrow(Gdiplus::Graphics& graphics,const Rect& rectDownArrow)
{
	graphics.FillRectangle(&m_brushBackground,rectDownArrow);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
	Gdiplus::SolidBrush brushArrow(Gdiplus::Color(145,148,150));
	Gdiplus::Point polygonDown[3];	// 下面箭头
	polygonDown[0].X = rectDownArrow.GetLeft() + rectDownArrow.Width/2;
	polygonDown[0].Y = rectDownArrow.GetBottom()-1;
	polygonDown[1].X = rectDownArrow.GetLeft()+1;
	polygonDown[1].Y = rectDownArrow.GetBottom() - rectDownArrow.Height+1;
	polygonDown[2].X = rectDownArrow.GetLeft() + rectDownArrow.Width-1;
	polygonDown[2].Y = rectDownArrow.GetBottom() - rectDownArrow.Height+1;
	graphics.FillPolygon(&brushArrow,polygonDown,3);
	if (m_bMoveInDownArrow)
	{
		Gdiplus::Pen penArrow(Gdiplus::Color(145,148,150));
		graphics.DrawRectangle(&penArrow,rectDownArrow);
	}
}
void CEBSkinVBar::DrawBottomArrow(Gdiplus::Graphics& graphics,const Rect& rectBottomArrow)
{
	graphics.FillRectangle(&m_brushBackground,rectBottomArrow);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
	Gdiplus::Pen penArrow(Gdiplus::Color(145,148,150));
	graphics.DrawLine(&penArrow,rectBottomArrow.GetLeft()+1,rectBottomArrow.GetBottom()-2,rectBottomArrow.GetRight()-1,rectBottomArrow.GetBottom()-2);
	graphics.DrawLine(&penArrow,rectBottomArrow.GetLeft()+1,rectBottomArrow.GetTop()+1,rectBottomArrow.GetLeft()+rectBottomArrow.Width/2,rectBottomArrow.GetBottom()-2);
	graphics.DrawLine(&penArrow,rectBottomArrow.GetLeft()+rectBottomArrow.Width/2,rectBottomArrow.GetBottom()-2,rectBottomArrow.GetRight()-1,rectBottomArrow.GetTop()+1);
	if (m_bMoveInBottomArrow)
	{
		graphics.DrawRectangle(&penArrow,rectBottomArrow);
	}
}

STDMETHODIMP CEBSkinVBar::UpdatePosition(void)
{
	// TODO: Add your implementation code here
	if( this->m_hWnd == NULL )
		return ERROR_INVALID_HANDLE;
	if (m_hMsgHwnd == NULL && m_pCallback == NULL)
		return ERROR_INVALID_HANDLE;

	CRect clientRect;
	GetClientRect(&clientRect);

	LONG nMax = 0;
	LONG nPos = 0;
	if (m_pCallback != NULL)
	{
		m_pCallback->GetScrollMax(&nMax);
		m_pCallback->GetScrollPos(&nPos);
	}
	if (nMax==0) return S_FALSE;
	double nHeight = (clientRect.Height()-m_nTopInterval-m_sizeTopArrow.cy-m_sizeUpArrow.cy-m_sizeThumb.cy-m_sizeDownArrow.cy-m_sizeBottomArrow.cy);
	double m_dbThumbInterval = (nHeight/nMax);
	double nNewdbValue = (m_dbThumbInterval * nPos);
	m_nThumbTop = m_nTopInterval+m_sizeTopArrow.cy+m_sizeUpArrow.cy+(int)nNewdbValue;

	LimitThumbPosition();
	this->FireViewChange();
	return S_OK;
}
void CEBSkinVBar::LimitThumbPosition(void)
{
	CRect clientRect;
	GetClientRect(&clientRect);
	if(m_nThumbTop > (clientRect.bottom-m_sizeDownArrow.cy-m_sizeBottomArrow.cy-m_sizeThumb.cy))
	{
		m_nThumbTop = clientRect.bottom-m_sizeDownArrow.cy-m_sizeBottomArrow.cy-m_sizeThumb.cy;
	}
	if( m_nThumbTop < (clientRect.top+m_sizeUpArrow.cy+m_sizeTopArrow.cy+m_nTopInterval))
	{
		m_nThumbTop = clientRect.top+m_sizeUpArrow.cy+m_sizeTopArrow.cy+m_nTopInterval;
	}
}

void CEBSkinVBar::PageDown()
{
	if (m_pCallback != NULL)
		m_pCallback->ScrollPageDown();
	UpdatePosition();
}

void CEBSkinVBar::PageUp()
{
	if (m_pCallback != NULL)
		m_pCallback->ScrollPageUp();
	UpdatePosition();
}

void CEBSkinVBar::ScrollUp()
{
	if (m_pCallback != NULL)
		m_pCallback->ScrollLineUp();
	UpdatePosition();
}

void CEBSkinVBar::ScrollDown()
{
	if (m_pCallback != NULL)
		m_pCallback->ScrollLineDown();
	UpdatePosition();
}
STDMETHODIMP CEBSkinVBar::SetMsgHwnd(ULONG hWnd)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CEBSkinVBar::SetCallback(IUnknown* pCallback)
{
	pCallback->QueryInterface(IID_IEBSkinVBarCallback,(void**)&m_pCallback);
	return S_OK;
}
STDMETHODIMP CEBSkinVBar::SetBackGroundColor(BYTE r, BYTE g, BYTE b)
{
	m_brushBackground.SetColor(Gdiplus::Color(r,g,b));
	return S_OK;
}
STDMETHODIMP CEBSkinVBar::SetBackGroundColor2(BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_brushBackground.SetColor(Gdiplus::Color(a,r,g,b));
	return S_OK;
}

LRESULT CEBSkinVBar::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	CRect clientRect;
	GetClientRect(&clientRect);
	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	if( m_bMouseDown )
	{
		LONG nMax = 0;
		LONG nMin = 0;
		LONG nPos = 0;
		if (m_pCallback != NULL)
		{
			m_pCallback->GetScrollMax(&nMax);
			m_pCallback->GetScrollMin(&nMin);
			m_pCallback->GetScrollPos(&nPos);
		}
		if (nMax==0) return 0;
		double nHeight = (clientRect.Height()-m_nTopInterval-m_sizeTopArrow.cy-m_sizeUpArrow.cy-m_sizeThumb.cy-m_sizeDownArrow.cy-m_sizeBottomArrow.cy);
		double m_dbThumbInterval = (nHeight/nMax);
		m_nThumbTop = point.y-m_sizeThumb.cy/2;

		int nScrollTimes = (int)((m_nThumbTop-m_sizeTopArrow.cy-m_sizeUpArrow.cy-m_nTopInterval)/m_dbThumbInterval)-nPos;
		if (nScrollTimes != 0 && (abs(nScrollTimes)>=nMin) || m_nThumbTop<=0 || m_nThumbTop>nHeight)
		{
			if (m_pCallback != NULL)
				m_pCallback->ScrollBy(nScrollTimes);
		}

		LimitThumbPosition();
		this->FireViewChange();
	}else
	{
		CRect rectTopArrow(0,m_nTopInterval,m_sizeTopArrow.cx,m_sizeTopArrow.cy+m_nTopInterval);
		CRect rectUpArrow(0,m_nTopInterval+m_sizeTopArrow.cy,m_sizeUpArrow.cx,m_sizeUpArrow.cy+m_nTopInterval+m_sizeTopArrow.cy);
		int nBottomArrowTop = clientRect.Height() - m_sizeBottomArrow.cy-m_nTopInterval;
		CRect rectBottomArrow(0,nBottomArrowTop,m_sizeBottomArrow.cx,nBottomArrowTop+m_sizeBottomArrow.cy);
		CRect rectDownArrow(0,nBottomArrowTop-m_sizeBottomArrow.cy,m_sizeDownArrow.cx,nBottomArrowTop-m_sizeBottomArrow.cy+m_sizeUpArrow.cy);
		CRect rectThumb(0,m_nThumbTop,m_sizeThumb.cx,m_nThumbTop+m_sizeThumb.cy);

		if(rectTopArrow.PtInRect(point))
		{
			m_bMoveInTopArrow = true;
			this->FireViewChange();
			KillTimer(TIMER_3);
			SetTimer(TIMER_3,100,NULL);
			return 0;
		}
		if(rectUpArrow.PtInRect(point))
		{
			m_bMoveInUpArrow = true;
			this->FireViewChange();
			KillTimer(TIMER_3);
			SetTimer(TIMER_3,100,NULL);
			return 0;
		}
		if(rectDownArrow.PtInRect(point))
		{
			m_bMoveInDownArrow = true;
			this->FireViewChange();
			KillTimer(TIMER_3);
			SetTimer(TIMER_3,100,NULL);
			return 0;
		}
		if(rectBottomArrow.PtInRect(point))
		{
			m_bMoveInBottomArrow = true;
			this->FireViewChange();
			KillTimer(TIMER_3);
			SetTimer(TIMER_3,100,NULL);
			return 0;
		}
		if(rectThumb.PtInRect(point))
		{
			m_bMoveInThumb = true;
			this->FireViewChange();
			KillTimer(TIMER_3);
			SetTimer(TIMER_3,100,NULL);
			return 0;
		}
	}
	return 0;
}

LRESULT CEBSkinVBar::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	short zDelta = HIWORD(wParam);
	int nScrollTimes = (-1*zDelta)/120;
	LONG nMax = 0;
	LONG nMin = 0;
	LONG nPos = 0;
	if (m_pCallback != NULL)
	{
		m_pCallback->GetScrollMax(&nMax);
		m_pCallback->GetScrollMin(&nMin);
		m_pCallback->GetScrollPos(&nPos);
	}
	if (nMax==0) return 0;
	nScrollTimes *= nMin;
	CRect clientRect;
	GetClientRect(&clientRect);
	double nHeight = (clientRect.Height()-m_nTopInterval-m_sizeTopArrow.cy-m_sizeUpArrow.cy-m_sizeThumb.cy-m_sizeDownArrow.cy-m_sizeBottomArrow.cy);
	double m_dbThumbInterval = (nHeight/nMax);
	m_nThumbTop += (nScrollTimes*m_dbThumbInterval);
	if (m_pCallback != NULL)
		m_pCallback->ScrollBy(nScrollTimes);
	LimitThumbPosition();
	this->FireViewChange();
	return 0;
}

LRESULT CEBSkinVBar::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	CRect rectTopArrow(0,m_nTopInterval,m_sizeTopArrow.cx,m_sizeTopArrow.cy+m_nTopInterval);
	CRect rectUpArrow(0,m_nTopInterval+m_sizeTopArrow.cy,m_sizeUpArrow.cx,m_sizeUpArrow.cy+m_nTopInterval+m_sizeTopArrow.cy);
	int nBottomArrowTop = clientRect.Height() - m_sizeBottomArrow.cy-m_nTopInterval;
	CRect rectBottomArrow(0,nBottomArrowTop,m_sizeBottomArrow.cx,nBottomArrowTop+m_sizeBottomArrow.cy);
	CRect rectDownArrow(0,nBottomArrowTop-m_sizeBottomArrow.cy,m_sizeDownArrow.cx,nBottomArrowTop-m_sizeBottomArrow.cy+m_sizeUpArrow.cy);
	CRect rectThumb(0,m_nThumbTop,m_sizeThumb.cx,m_nThumbTop+m_sizeThumb.cy);

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);
	if(rectThumb.PtInRect(point))
	{
		m_bMouseDown = true;
	}else if(rectDownArrow.PtInRect(point))
	{
		m_bMouseDownArrowDown = true;
		SetTimer(TIMER_2,250,NULL);
	}else if(rectUpArrow.PtInRect(point))
	{
		m_bMouseDownArrowUp = true;
		SetTimer(TIMER_2,250,NULL);
	}else if(rectTopArrow.PtInRect(point))
	{
		m_bMouseDownArrowTop = true;
		SetTimer(TIMER_2,250,NULL);
	}else if(rectBottomArrow.PtInRect(point))
	{
		m_bMouseDownArrowBottom = true;
		SetTimer(TIMER_2,250,NULL);
	}
	
	return 0;
}

LRESULT CEBSkinVBar::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	UpdatePosition();
	KillTimer(TIMER_1);
	ReleaseCapture();
	
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	CRect rectTopArrow(0,m_nTopInterval,m_sizeTopArrow.cx,m_sizeTopArrow.cy+m_nTopInterval);
	CRect rectUpArrow(0,m_nTopInterval+m_sizeTopArrow.cy,m_sizeUpArrow.cx,m_sizeUpArrow.cy+m_nTopInterval+m_sizeTopArrow.cy);
	int nBottomArrowTop = clientRect.Height() - m_sizeBottomArrow.cy-m_nTopInterval;
	CRect rectBottomArrow(0,nBottomArrowTop,m_sizeBottomArrow.cx,nBottomArrowTop+m_sizeBottomArrow.cy);
	CRect rectDownArrow(0,nBottomArrowTop-m_sizeBottomArrow.cy,m_sizeDownArrow.cx,nBottomArrowTop-m_sizeBottomArrow.cy+m_sizeUpArrow.cy);
	CRect rectThumb(0,m_nThumbTop,m_sizeThumb.cx,m_nThumbTop+m_sizeThumb.cy);

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);
	if(rectTopArrow.PtInRect(point) && m_bMouseDownArrowTop)
	{
		m_nThumbTop = 0;
		if (m_pCallback != NULL)
			m_pCallback->ScrollTo(0);
		LimitThumbPosition();
		this->FireViewChange();
		m_bMouseDownArrowTop = false;
		bInChannel = false;
	}else if(rectBottomArrow.PtInRect(point) && m_bMouseDownArrowBottom)
	{
		m_nThumbTop = clientRect.Height();
		if (m_pCallback != NULL)
			m_pCallback->ScrollTo(-1);
		LimitThumbPosition();
		this->FireViewChange();
		m_bMouseDownArrowBottom = false;
		bInChannel = false;
	}else if(rectUpArrow.PtInRect(point) && m_bMouseDownArrowUp)
	{
		ScrollUp();	
		m_bMouseDownArrowUp = false;
		bInChannel = false;
	}else if(m_bMouseDownArrowDown && rectDownArrow.PtInRect(point))
	{
		ScrollDown();
		m_bMouseDownArrowDown = false;
		bInChannel = false;
	}else if(rectThumb.PtInRect(point))
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
	//m_bDragging = false;
	
	return TRUE;
}

LRESULT CEBSkinVBar::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	UINT nIDEvent = wParam;
	if (TIMER_3 == nIDEvent)
	{
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);
		CRect clientRect;
		GetClientRect(&clientRect);

		CRect rectTopArrow(0,m_nTopInterval,m_sizeTopArrow.cx,m_sizeTopArrow.cy+m_nTopInterval);
		CRect rectUpArrow(0,m_nTopInterval+m_sizeTopArrow.cy,m_sizeUpArrow.cx,m_sizeUpArrow.cy+m_nTopInterval+m_sizeTopArrow.cy);
		int nBottomArrowTop = clientRect.Height() - m_sizeBottomArrow.cy-m_nTopInterval;
		CRect rectBottomArrow(0,nBottomArrowTop,m_sizeBottomArrow.cx,nBottomArrowTop+m_sizeBottomArrow.cy);
		CRect rectDownArrow(0,nBottomArrowTop-m_sizeBottomArrow.cy,m_sizeDownArrow.cx,nBottomArrowTop-m_sizeBottomArrow.cy+m_sizeUpArrow.cy);
		CRect rectThumb(0,m_nThumbTop,m_sizeThumb.cx,m_nThumbTop+m_sizeThumb.cy);

		if (m_bMoveInTopArrow & !rectTopArrow.PtInRect(point) )
		{
			m_bMoveInTopArrow = false;
			this->FireViewChange();
		}
		if (m_bMoveInUpArrow & !rectUpArrow.PtInRect(point) )
		{
			m_bMoveInUpArrow = false;
			this->FireViewChange();
		}
		if (m_bMoveInDownArrow & !rectDownArrow.PtInRect(point) )
		{
			m_bMoveInDownArrow = false;
			this->FireViewChange();
		}
		if (m_bMoveInBottomArrow & !rectBottomArrow.PtInRect(point) )
		{
			m_bMoveInBottomArrow = false;
			this->FireViewChange();
		}
		if (m_bMoveInThumb & !rectThumb.PtInRect(point) )
		{
			m_bMoveInThumb = false;
			this->FireViewChange();
		}

		if (!clientRect.PtInRect(point))
		{
			KillTimer(TIMER_3);
			//this->FireViewChange();
		}
	}else if (nIDEvent == TIMER_1)
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
			SetTimer(TIMER_1, 100, NULL);
		}
		if( m_bMouseDownArrowUp )
		{
			KillTimer(TIMER_2);
			SetTimer(TIMER_1, 100, NULL);
		}
	}
	return 0;
}

LRESULT CEBSkinVBar::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// 不刷新背景
	return TRUE;
	return 0;
}

LRESULT CEBSkinVBar::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pCallback != NULL)
		m_pCallback->OnVBarKillFocus();
	return 0;
}
