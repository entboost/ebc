// ../../RemoteDesktopTest/ScrollStatic.cpp : implementation file
//

#ifdef WIN32
#pragma warning(disable:4819)
#endif
#include "stdafx.h"
//#include "RemoteDesktopTest.h"
#include "ScrollStatic.h"
#define TIMER_ID_SEND_MOUSEMOVE		102
#define TIMER_ID_SEND_MESSAGE		103


// CScrollStatic

IMPLEMENT_DYNAMIC(CScrollStatic, CStatic)

CScrollStatic::CScrollStatic()
: m_nScrollBarWidth(16)
, m_pCallback(NULL), m_pUserData(0)
, m_nMouseFPS(0)
, m_nWaitMouseMoveCount(0)

{

}

CScrollStatic::~CScrollStatic()
{
}


BEGIN_MESSAGE_MAP(CScrollStatic, CStatic)
	//ON_CONTROL_REFLECT(STN_CLICKED, &CScrollStatic::OnStnClicked)
	//ON_CONTROL_REFLECT(STN_DBLCLK, &CScrollStatic::OnStnDblclick)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

// CScrollStatic message handlers



void CScrollStatic::OnStnClicked()
{
	// TODO: Add your control notification handler code here
}

void CScrollStatic::OnStnDblclick()
{
	// TODO: Add your control notification handler code here
}

void CScrollStatic::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nPos1=m_hbar.GetScrollPos();
	switch (nSBCode)
	{
	case SB_LINERIGHT:
	case SB_PAGERIGHT:
		{
			nPos1 += 80;
		}break;
	case SB_PAGELEFT:
	case SB_LINELEFT:
		{
			nPos1 -= 80;
		}break;
	case SB_THUMBTRACK:
		{
			nPos1 = nPos;
		}break;
	}
	drawScreen();
	m_hbar.SetScrollPos(nPos1,TRUE);

	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScrollStatic::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nPos1=m_vbar.GetScrollPos();
	switch (nSBCode)
	{
	case SB_LINEDOWN:
	case SB_PAGEDOWN:
		{
			nPos1 += 80;
		}break;
	case SB_LINEUP:
	case SB_PAGEUP:
		{
			nPos1 -= 80;
		}break;
	case SB_THUMBTRACK:
		{
			nPos1 = nPos;
		}break;
	}
	drawScreen();
	m_vbar.SetScrollPos(nPos1);
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CScrollStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_hbar.Create(WS_CHILD|WS_VISIBLE|SBS_HORZ, CRect(0, 0, 0, 0), this, 0x10011);
	m_vbar.Create(WS_CHILD|WS_VISIBLE|SBS_VERT, CRect(0, 0, 0, 0), this, 0x10012);
	return TRUE;
}

void CScrollStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	UpdateScreenInfo();
	if (m_hbar.GetSafeHwnd())
	{
		m_hbar.MoveWindow(0, cy-m_nScrollBarWidth, cx-m_nScrollBarWidth, m_nScrollBarWidth);
	}
	if (m_vbar.GetSafeHwnd())
	{
		m_vbar.MoveWindow(cx-m_nScrollBarWidth, 0, m_nScrollBarWidth, cy-m_nScrollBarWidth);
	}
}
void CScrollStatic::SetCallback(CRemoteEventHandler* pCallback, void * pUserData)
{
	m_pCallback = pCallback;
	m_pUserData = pUserData;
}
bool CScrollStatic::CreateWnd(CWnd * pParentWnd)
{
	if (!IsWndValidate())
		Create(_T(""), WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_LEFT, CRect(0, 0, 0, 0), pParentWnd);
	//ModifyStyle(0,SS_NOTIFY);
	return IsWndValidate();
}
/*void CScrollStatic::SetMouseMoveFPS(int fps)
{
	if (this->GetSafeHwnd()==NULL) return;
	if (fps<=0) return;
	KillTimer(TIMER_ID_SEND_MOUSEMOVE);
	SetTimer(TIMER_ID_SEND_MOUSEMOVE, 1000/fps, NULL);
}
*/


bool CScrollStatic::EnableControl(int nMouseFPS)
{
	if (this->GetSafeHwnd()==NULL) return false;
	KillTimer(TIMER_ID_SEND_MOUSEMOVE);
	KillTimer(TIMER_ID_SEND_MESSAGE);
	m_listRtpDatas.clear();
	m_nMouseFPS = nMouseFPS;
	if (m_nMouseFPS>0)
	{
		SetTimer(TIMER_ID_SEND_MOUSEMOVE, 1000/m_nMouseFPS, NULL);
		SetTimer(TIMER_ID_SEND_MESSAGE, 50, NULL);
	}
	return true;
}
int CScrollStatic::EnableControl(void) const
{
	return m_nMouseFPS;
}

bool CScrollStatic::IsWndValidate(void) const
{
	return (GetSafeHwnd() != NULL) ? true : false;
}

int CScrollStatic::GetHScrollBarPos(void) const
{
	return m_hbar.GetSafeHwnd() ? m_hbar.GetScrollPos() : 0;
}

int CScrollStatic::GetVScrollBarPos(void) const
{
	return m_vbar.GetSafeHwnd() ? m_vbar.GetScrollPos() : 0;
}

void CScrollStatic::UpdateScreenInfo(void)
{
	if (this->GetSafeHwnd() == NULL)
		return;

	CRect rect;
	this->GetClientRect(&rect);

	if (m_hbar.GetSafeHwnd()!=NULL)
		m_hbar.SetScrollRange(0, max(0,(m_bitmapManager.GetBitmapWidth()-rect.Width()+m_nScrollBarWidth)));
	if (m_vbar.GetSafeHwnd()!=NULL)
		m_vbar.SetScrollRange(0, max(0,(m_bitmapManager.GetBitmapHeight()-rect.Height()+m_nScrollBarWidth)));
}
void CScrollStatic::MoveParentFullSize(void)
{
	if (this->GetSafeHwnd() == NULL)
		return;

	const CWnd * pWndParent = this->GetParent();
	CRect rect;
	pWndParent->GetClientRect(&rect);
	this->MoveWindow(0,0,rect.Width(),rect.Height());
}

void CScrollStatic::drawScreen(void)
{
	if (this->GetSafeHwnd() == NULL)
		return;

	CDC *pdc=GetDC();
	// Use StretchDIBits to draw the Dib.

	CRect rect;
	this->GetClientRect(&rect);
	const int cwidth = min(m_bitmapManager.GetBitmapWidth(), (rect.Width()-m_nScrollBarWidth));
	const int cheight = min(m_bitmapManager.GetBitmapHeight(), (rect.Height()-m_nScrollBarWidth));
	//const int nDestX = (rect.Width()-m_nScrollBarWidth-m_bitmapManager.GetBitmapWidth())/2;
	//const int nDestY = (rect.Height()-m_nScrollBarWidth-m_bitmapManager.GetBitmapHeight())/2;

	int scrx = m_hbar.GetScrollPos();	//远程屏幕水平起点
	int scry = m_vbar.GetScrollPos();	//远程屏幕垂直起点
	int scrxlm = 0;//水平移动最大值
	int scrylm = m_bitmapManager.GetBitmapHeight()-rect.Height()+m_nScrollBarWidth;//垂直移动最大值

	StretchDIBits( pdc->m_hDC, 0, 0,
		cwidth, cheight,
		scrx, max(0,(scrylm-scry)),
		cwidth, cheight,
		m_bitmapManager.GetDibBits(),
		m_bitmapManager.GetBitmapInfo(),
		BI_RGB, SRCCOPY );
	ReleaseDC(pdc);
}

void CScrollStatic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMER_ID_SEND_MESSAGE:
		{
			rdpStruct rdpMessage;
			memset(&rdpMessage, 0, sizeof(rdpMessage));
			if (!m_listRtpDatas.front(rdpMessage))
				break;
			if (m_nMouseFPS>0 && m_pCallback!=0)
			{
				if (rdpMessage.m_messageEvent == RDP_MOUSEMOVE)
				{
					if (m_nWaitMouseMoveCount>0)
						m_nWaitMouseMoveCount--;
				}else
				{
					m_nWaitMouseMoveCount = 18;
				}
				m_pCallback->onSendRdpMessage(rdpMessage, m_pUserData);
			}
		}break;
	case TIMER_ID_SEND_MOUSEMOVE:
		{
			//break;	// test
			if (m_nMouseFPS<=0)
				break;
			const HWND hWndForground = this->GetForegroundWindow()->GetSafeHwnd();
			if (hWndForground!=this->GetSafeHwnd() && hWndForground!=this->GetParent()->GetSafeHwnd())
				break;
			POINT point;
			::GetCursorPos(&point);
			// **必须注释，开放该代码，会导致某些
			if (m_nWaitMouseMoveCount==0 && abs(m_ptLast.x-point.x)<=3 && abs(m_ptLast.y-point.y)<=3)
			{
				break;
			}
			m_ptLast.x = point.x;
			m_ptLast.y = point.y;

			CRect rectImageStatic;
			GetWindowRect(&rectImageStatic);
			rectImageStatic.right -= GetScrollBarWidth();
			rectImageStatic.bottom -= GetScrollBarWidth();
			if (rectImageStatic.PtInRect(point))
			{
				ScreenToClient(&point);
				rdpStruct rdp;
				memset(&rdp, 0, RDPSTRUCT_SIZE);
				rdp.m_messageEvent = RDP_MOUSEMOVE;
				rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
				rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
				if (m_pCallback!=0)
				{
					m_listRtpDatas.add(rdp);
					//m_pCallback->onSendRdpMessage(rdp, m_pUserData);
				}
			}
		}break;
	default:
		break;
	}

	CStatic::OnTimer(nIDEvent);
}

void CScrollStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_messageEvent = RDP_LBUTTONDOWN;
		rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
		rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
		m_listRtpDatas.add(rdp);
	}
	CStatic::OnLButtonDown(nFlags, point);
}
void CScrollStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_messageEvent = RDP_LBUTTONUP;
		rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
		rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
		m_listRtpDatas.add(rdp);
	}
	CStatic::OnLButtonUp(nFlags, point);
}
void CScrollStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_messageEvent = RDP_LBUTTONDBLCLK;
		rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
		rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
		m_listRtpDatas.add(rdp);
	}
	CStatic::OnLButtonDblClk(nFlags, point);
}
void CScrollStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_messageEvent = RDP_RBUTTONDOWN;
		rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
		rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
		m_listRtpDatas.add(rdp);
	}
	CStatic::OnRButtonDown(nFlags, point);
}
void CScrollStatic::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_messageEvent = RDP_RBUTTONUP;
		rdp.u.mousePoint.m_x = (u_short)htons(point.x+GetHScrollBarPos());
		rdp.u.mousePoint.m_y = (u_short)htons(point.y+GetVScrollBarPos());
		m_listRtpDatas.add(rdp);
	}
	CStatic::OnRButtonUp(nFlags, point);
}
void CScrollStatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		//bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
		//bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		//bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;

		if (nChar==VK_SHIFT || nChar==VK_CONTROL)		//  只发送SHIFT,CTRL按键
		{
			rdpStruct rdp;
			memset(&rdp, 0, RDPSTRUCT_SIZE);
			rdp.m_messageEvent = RDP_KEYDOWN;
			rdp.u.keyInfo.m_wParam = htonl(nChar);
			rdp.u.keyInfo.m_lParam = htonl(nRepCnt);
			m_listRtpDatas.add(rdp);
			//m_pCallback->onSendRdpMessage(rdp, m_pUserData);
		}
	}
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CScrollStatic::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		if (nChar!=VK_MENU)			//  单独ALT按键不发送
		{
			UINT lParam = 0;
			//if (nChar!=VK_CONTROL)
			//{
			//	const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
			//	if (bControl)
			//		lParam |= 0x1;
			//}
			if (nChar!=VK_MENU)
			{
				const bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;
				if (bAlt)
					lParam |= 0x2;
			}
			//if (nChar!=VK_SHIFT)
			//{
			//	const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
			//	if (bShift)
			//		lParam |= 0x4;
			//}

			rdpStruct rdp;
			memset(&rdp, 0, RDPSTRUCT_SIZE);
			rdp.m_messageEvent = RDP_KEYUP;
			rdp.u.keyInfo.m_wParam = htonl(nChar);
			rdp.u.keyInfo.m_lParam = htonl(lParam);
			//rdp.u.keyInfo.m_lParam = htonl(nRepCnt);
			m_listRtpDatas.add(rdp);
		}
	}
	CStatic::OnKeyUp(nChar, nRepCnt, nFlags);
}
void CScrollStatic::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		//bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
		//bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		//bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;

		//if (nChar==VK_MENU)
		//{
		//	rdpStruct rdp;
		//	memset(&rdp, 0, RDPSTRUCT_SIZE);
		//	rdp.m_messageEvent = RDP_KEYDOWN;
		//	rdp.u.keyInfo.m_wParam = htonl(nChar);
		//	rdp.u.keyInfo.m_lParam = htonl(nRepCnt);
		//	m_listRtpDatas.add(rdp);
		//}
		//m_pCallback->onSendRdpMessage(rdp, m_pUserData);
	}
	CStatic::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
void CScrollStatic::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nMouseFPS>0 && m_pCallback!=0)
	{
		if (nChar!=VK_MENU)	// 单独alt键不发送；
		{
			UINT lParam = 0;
			//const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
			//if (bControl)
			//	lParam |= 0x1;
			const bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;
			if (bAlt)
				lParam |= 0x2;
			//const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
			//if (bShift)
			//	lParam |= 0x4;

			rdpStruct rdp;
			memset(&rdp, 0, RDPSTRUCT_SIZE);
			rdp.m_messageEvent = RDP_KEYUP;
			rdp.u.keyInfo.m_wParam = htonl(nChar);
			rdp.u.keyInfo.m_lParam = htonl(lParam);
			m_listRtpDatas.add(rdp);
		}
	}
	CStatic::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

UINT CScrollStatic::OnGetDlgCode(void)
{
	UINT lRet = CStatic::OnGetDlgCode();  
	lRet |= DLGC_WANTALLKEYS;
	//lRet |= (DLGC_WANTTAB|DLGC_WANTARROWS);
	return lRet; 
}
