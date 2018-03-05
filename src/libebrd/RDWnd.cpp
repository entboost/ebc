#include "StdAfx.h"
#include "RDWnd.h"

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")


CRDWnd::CRDWnd(void)
: m_eventHandler(NULL)
, m_bKilled(false), m_pGetDesktopThread(NULL), m_pProcessMsgThread(NULL)
, m_fps(RD_DEFAULT_FPS)
, m_bEnableControl(0)
/*, m_bWaitShiftUp(false), m_bWaitControlUp(false)*/, m_bWaitAltUp(false)

{
}

CRDWnd::~CRDWnd(void)
{
	StopLocalDesktop();
}

void getdesktop_thread_svr(void* lparam)
{
	CRDWnd * pSvr = (CRDWnd*)lparam;
	pSvr->doGetDesktopInfo();
}
void processmsg_thread_svr(void* lparam)
{
	CRDWnd * pSvr = (CRDWnd*)lparam;
	pSvr->doProcessMessage();
}

bool CRDWnd::StartLocalDesktop(int quant, int fps, CRDEventHandler* pHandler)
{
	if (!m_desktopWindow.Create(0))
		return false;
	
	XInfo xinfo(XVID_CSP_BGRA, m_desktopWindow.GetBitCount(), m_desktopWindow.GetWidth(), m_desktopWindow.GetHeight(),fps*7);	// *
	m_xvid.setenc_info(xinfo);
	// 1: 300K->170K->14K->10K
	// 3: 160K->57K->2.xK->1.xK
	// 4: 160K->40K->2.xK->1.xK
	// 5: 140K->73K->1.xK->1.xK
	// 10: 100K->44K->1.9K->1.6K
	// 12: 78K->...
	m_xvid.setenc_quant(max(1,quant));

	m_eventHandler = pHandler;
	if (m_eventHandler!=NULL)
	{
		m_eventHandler->onLocalDesktopInfo(m_desktopWindow.GetWidth(), m_desktopWindow.GetHeight(), m_desktopWindow.GetBitCount());
	}	

	m_bKilled = false;
	if (m_pGetDesktopThread == NULL)
		m_pGetDesktopThread = new boost::thread(boost::bind(getdesktop_thread_svr, (void*)this));
	if (m_pProcessMsgThread == NULL)
		m_pProcessMsgThread = new boost::thread(boost::bind(processmsg_thread_svr, (void*)this));

	m_fps = fps;
	if (fps < RD_MIN_FPS)
		m_fps = RD_MIN_FPS;
	else if (m_fps > RD_MAX_FPS)
		m_fps = RD_MAX_FPS;
	return true;
}
bool CRDWnd::IsLocalDesktopStart(void) const
{
	return m_pGetDesktopThread==NULL?false:true;
}
void CRDWnd::StopLocalDesktop(void)
{
	m_bKilled = true;
	if (m_pGetDesktopThread != NULL)
	{
		m_pGetDesktopThread->join();
		delete m_pGetDesktopThread;
		m_pGetDesktopThread = NULL;
	}
	if (m_pProcessMsgThread != NULL)
	{
		m_pProcessMsgThread->join();
		delete m_pProcessMsgThread;
		m_pProcessMsgThread = NULL;
	}

	m_xvid.close();
	m_listRtpDatas.clear();
	m_desktopWindow.Release();
}
void CRDWnd::AddRdpMessage(INT64 nDestId, const rdpStruct& rdpMessage)
{
	if (m_bEnableControl==nDestId)
		m_listRtpDatas.add(rdpMessage);
}

void CRDWnd::doGetDesktopInfo(void)
{
	unsigned char* m_pEncBuffer = NULL;
	unsigned int m_nNextSendTimestamp = 0;

	while (!m_bKilled)
	{
		Sleep(50);
		if (m_desktopWindow.IsInvalid())
			break;
		if (m_eventHandler!=NULL)
		{
			if (!m_eventHandler->onCanProcess())
				continue;

			const unsigned int ts = timeGetTime();
			if (m_nNextSendTimestamp>ts)
			{
				continue;
			}

			if (m_pEncBuffer==0)
			{
				m_pEncBuffer = new unsigned char[600*1024];
			}

			int encode_size = 0;
			bool bKeyFrame = false;
			if (m_xvid.encode((const unsigned char*)m_desktopWindow.GetScreenData(), m_pEncBuffer, encode_size, bKeyFrame)!=NULL)
			{
				m_eventHandler->onLocalDesktopData(m_pEncBuffer, encode_size, bKeyFrame, ts);
			}
			m_nNextSendTimestamp = ts+(1000/m_fps)+(encode_size/(32*1024))*400;	// 32KB延迟400mS
			//m_nNextSendTimestamp = timeGetTime()+(1000/m_fps);
		}
	}
	if (m_pEncBuffer!=0)
	{
		delete[] m_pEncBuffer;
		m_pEncBuffer = 0;
	}
}

void CRDWnd::doProcessMessage(void)
{
	while (!m_bKilled)
	{
		rdpStruct rdpMessage;
		memset(&rdpMessage, 0, sizeof(rdpMessage));
		if (!m_listRtpDatas.front(rdpMessage))
		{
			Sleep(10);
			continue;
		}else if (m_bEnableControl==0)
		{
			continue;
		}
		const rdpStruct * rdp = &rdpMessage;
		switch (rdp->m_messageEvent)
		{
		case RDP_MOUSEMOVE:
			{
				//break;	// test
				POINT point;
				point.x = ntohs(rdp->u.mousePoint.m_x);
				point.y = ntohs(rdp->u.mousePoint.m_y);
				if (m_desktopWindow.GetRealWidth()!=m_desktopWindow.GetWidth() && m_desktopWindow.GetRealHeight()!=m_desktopWindow.GetHeight())
				{
					point.x = (m_desktopWindow.GetRealWidth()*point.x)/m_desktopWindow.GetWidth();
					point.y = (m_desktopWindow.GetRealHeight()*point.y)/m_desktopWindow.GetHeight();
				}
				if (abs(m_ptLast.x-point.x)<=3 && abs(m_ptLast.y-point.y)<=3)
				{
					break;
				}
				m_ptLast.x = point.x;
				m_ptLast.y = point.y;
				SetCursorPos (point.x, point.y);	//设鼠标位置
			}break;
		case RDP_LBUTTONDBLCLK:
			{
				POINT point;
				point.x = ntohs(rdp->u.mousePoint.m_x);
				point.y = ntohs(rdp->u.mousePoint.m_y);
				if (m_desktopWindow.GetRealWidth()!=m_desktopWindow.GetWidth() && m_desktopWindow.GetRealHeight()!=m_desktopWindow.GetHeight())
				{
					point.x = (m_desktopWindow.GetRealWidth()*point.x)/m_desktopWindow.GetWidth();
					point.y = (m_desktopWindow.GetRealHeight()*point.y)/m_desktopWindow.GetHeight();
				}

				SetCursorPos (point.x, point.y);//设鼠标位置
				mouse_event (MOUSEEVENTF_LEFTDOWN, point.x,point.y,0,0);//鼠标按下
				mouse_event (MOUSEEVENTF_LEFTUP, point.x,point.y,0,0);//鼠标按下
				Sleep(100);
				mouse_event (MOUSEEVENTF_LEFTDOWN, point.x,point.y,0,0);//鼠标按下
				mouse_event (MOUSEEVENTF_LEFTUP, point.x,point.y,0,0);//鼠标按下
			}break;
		case RDP_LBUTTONDOWN:
		case RDP_LBUTTONUP:
		case RDP_RBUTTONDOWN:
		case RDP_RBUTTONUP:
			{
				POINT point;
				point.x = ntohs(rdp->u.mousePoint.m_x);
				point.y = ntohs(rdp->u.mousePoint.m_y);
				if (m_desktopWindow.GetRealWidth()!=m_desktopWindow.GetWidth() && m_desktopWindow.GetRealHeight()!=m_desktopWindow.GetHeight())
				{
					point.x = (m_desktopWindow.GetRealWidth()*point.x)/m_desktopWindow.GetWidth();
					point.y = (m_desktopWindow.GetRealHeight()*point.y)/m_desktopWindow.GetHeight();
				}

				SetCursorPos (point.x, point.y);//设鼠标位置

				// 点击桌面，会被360驱动防护，拦截，关闭360木马防火墙就可以了；
				// 打开360->木马防火墙->系统防护->驱动防护（防止木马加载驱动获得系统权限）点关闭。
				//if (rdp->m_messageEvent==RDP_RBUTTONUP)
				//{
				//	mouse_event (ToMouseEvent(RDP_RBUTTONDOWN), point.x,point.y,0,0);
				//	Sleep(100);
				//}
				mouse_event (ToMouseEvent(rdp->m_messageEvent), point.x,point.y,0,0);

				if (m_bWaitAltUp)
				{
					m_bWaitAltUp = false;
					::keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);//放开
				}
			}break;
		case RDP_KEYDOWN:
			{
				const UINT wParam = ntohl(rdp->u.keyInfo.m_wParam);
				//const UINT lParam = ntohl(rdp->u.keyInfo.m_lParam);
				//if (wParam==VK_SHIFT)
				{
					::keybd_event(wParam,0,0,0);//按下
				}
				////if (m_nLastKeyDownParam!=-1)
				////{
				////	// lost RDP_KEYUP
				////	::keybd_event(m_nLastKeyDownParam,0,KEYEVENTF_KEYUP,0);//放开
				////}
				//const UINT wParam = ntohl(rdp->u.keyInfo.m_wParam);
				//::keybd_event(wParam,0,0,0);//按下
				//m_nLastKeyDownParam = wParam;
			}break;
		case RDP_KEYUP:
			{
				//if (m_bWaitShiftUp)
				//{
				//	m_bWaitShiftUp = false;
				//	::keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);//放开
				//}
				//if (m_bWaitControlUp)
				//{
				//	m_bWaitControlUp = false;
				//	::keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);//放开
				//}
				if (m_bWaitAltUp)
				{
					m_bWaitAltUp = false;
					::keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);//放开
				}
				const UINT wParam = ntohl(rdp->u.keyInfo.m_wParam);
				const UINT lParam = ntohl(rdp->u.keyInfo.m_lParam);
				//if (wParam!=VK_CONTROL && (lParam&0x1)==0x1)
				//{
				//	::keybd_event(VK_CONTROL,0,0,0);//按下
				//}
				if (wParam!=VK_MENU && (lParam&0x2)==0x2)
				{
					::keybd_event(VK_MENU,0,0,0);//按下
					m_bWaitAltUp = true;
				}
				//if (wParam!=VK_SHIFT && (lParam&0x4)==0x4)
				//{
				//	::keybd_event(VK_SHIFT,0,0,0);//按下
				//}
				::keybd_event(wParam,0,0,0);//按下
				::keybd_event(wParam,0,KEYEVENTF_KEYUP,0);//放开

				//if (wParam!=VK_SHIFT && (lParam&0x4)==0x4)
				//{
				//	::keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);//放开
				//}
				//if (wParam!=VK_MENU && (lParam&0x2)==0x2)
				//{
				//	::keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);//放开
				//}
				//if (wParam!=VK_CONTROL && (lParam&0x1)==0x1)
				//{
				//	::keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);//放开
				//}
			}break;
		default:
			break;
		}
	}
}

