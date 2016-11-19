#include "StdAfx.h"
#include "POP.h"
#include "FrameWndInfo.h"
#define DEFAULT_MSG_WIDTH1 16
#define DEFAULT_MSG_WIDTH2 28

CFrameWndInfo::CFrameWndInfo(void)
: m_nUserData(0)
, m_pWnd(NULL), m_bChildMode(false)
, m_nType(FRAME_WND_CALL_DIALOG)
, m_nUnreadMsgCount(0)
{
	memset(&m_rectClose,0,sizeof(m_rectClose));
}
CFrameWndInfo::CFrameWndInfo(bool bChildMode,const CDlgDialog::pointer& pDialog)
: m_nUserData(0)
, m_pDialog(pDialog)
, m_pWnd(NULL), m_bChildMode(bChildMode)
, m_nType(FRAME_WND_CALL_DIALOG)
, m_nUnreadMsgCount(0)
{
	memset(&m_rectClose,0,sizeof(m_rectClose));
}
CFrameWndInfo::CFrameWndInfo(CWnd* pWnd, bool bChildMode,FRAME_WND_TYPE nType)
: m_nUserData(0)
, m_pWnd(NULL), m_bChildMode(bChildMode)
, m_nType(nType)
, m_nUnreadMsgCount(0)
{
	memset(&m_rectClose,0,sizeof(m_rectClose));
	m_bBtnShowText = true;
}
CFrameWndInfo::~CFrameWndInfo(void)
{
	if (m_pDialog.get()!=NULL)
		m_pDialog->DestroyWindow();
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.DestroyWindow();
	if (m_close.GetSafeHwnd()!=NULL)
		m_close.DestroyWindow();
	if (m_msg.GetSafeHwnd()!=NULL)
		m_msg.DestroyWindow();
}

void CFrameWndInfo::SetUserData(unsigned int nUserData)
{
	if (m_nUserData != nUserData)
	{
		m_nUserData = nUserData;
		if (m_btn.GetSafeHwnd()!=NULL)
			m_btn.SetDlgCtrlID(m_nUserData);
		if (m_close.GetSafeHwnd()!=NULL)
			m_close.SetDlgCtrlID(m_nUserData);
		if (m_msg.GetSafeHwnd()!=NULL)
			m_msg.SetDlgCtrlID(m_nUserData);
	}
}

void CFrameWndInfo::SetCtrlColor(bool bInvalidate)
{
	if (m_pDialog.get()!=NULL)
		m_pDialog->SetCtrlColor(bInvalidate);
	if (m_btn.GetSafeHwnd()!=NULL)
	{
#ifdef USES_NEW_UI_160111
		if (m_bChildMode)
		{
			//m_btn.SetDrawLine(4,4,0,-1,-1,theDefaultFlatLine2Color);
			//m_btn.SetDrawPanel(true,theDefaultFlatLine2Color,RGB(128,128,128),RGB(64,64,64));
#ifdef USES_SUPPORT_UI_STYLE
			const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
#else
			if (theApp.GetHideMainFrame())
#endif
				m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetSelColor());
			else
				m_btn.SetDrawPanel(true,theDefaultFlatBg2Color,theApp.GetHotColor(),theApp.GetHotColor2());
			//m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetSelColor());
			//m_btn.SetDrawPanel(true,RGB(128,128,128),theDefaultFlatLineColor,theDefaultFlatLine2Color);
		}else
			m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetSelColor());
#else
		m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetSelColor());
#endif
		if (bInvalidate)
			m_btn.Invalidate();
	}
}
BOOL CFrameWndInfo::Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, CFont* pFont, bool bShowClose, bool bShowText)
{
	m_rectBtn = rect;
	if (m_btn.GetSafeHwnd()==NULL)
	{
		CString sWindowText;
		const size_t nLen = strlen(lpszCaption);
		for (size_t i=0;i<nLen;i++)
		{
			sWindowText.AppendChar(lpszCaption[i]);
			if ((i+1)<nLen && lpszCaption[i]<0)	// 中文，需要取二个
			{
				sWindowText.AppendChar(lpszCaption[++i]);
			}
			if (i>=13 && (i+1)<nLen)
			{
				//sWindowText.Append(_T("."));
				break;
			}
		}
		m_bBtnShowText = bShowText;
		m_sBtnShowText = sWindowText;
		if (!m_btn.Create(m_bBtnShowText?sWindowText:_T(""),WS_CHILD|WS_VISIBLE,rect,pParent,nID))
			return FALSE;

	}else
	{
		m_btn.SetParent(pParent);
		m_btn.MoveWindow(&m_rectBtn);
		m_btn.SetDlgCtrlID(nID);
	}
	if (pFont!=NULL)
		m_btn.SetFont(pFont);
	//m_btn.SetFont(pParent->GetFont());
	m_btn.SetToolTipText(lpszCaption);
	m_btn.SetTextHotMove(false);
#ifdef USES_NEW_UI_160111
	if (m_bChildMode)
	{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
#else
		if (theApp.GetHideMainFrame())
#endif
		{
			m_btn.SetNorTextColor(theDefaultBtnWhiteColor); 
		}else
		{
			m_btn.SetDrawPanelRgn(false);
			m_btn.SetNorTextColor(RGB(96,96,96));
			//m_btn.SetNorTextColor(RGB(0,0,0));	// theDefaultBtnWhiteColor
			m_btn.SetHotTextColor(theDefaultBtnWhiteColor); 
			m_btn.SetPreTextColor(theDefaultBtnWhiteColor); 
		}
	}else
		m_btn.SetNorTextColor(theDefaultBtnWhiteColor); 
#else
	m_btn.SetNorTextColor(theDefaultBtnWhiteColor); 
#endif
	SetCtrlColor(false);

	m_btn.SetTextTop(1);
	if (m_pDialog.get()!=NULL)
	{
		const bool bGrayImage = m_pDialog->GetFromIsOffLineState();
		m_btn.SetDrawImage(false,bGrayImage,const_cast<Gdiplus::Image*>(m_pDialog->GetFromImage()),Gdiplus::Rect(3,4,28,28));
		m_btn.SetTextLeft(31);	// 31=3+28
	}else if (m_nType==FRAME_WND_WORK_FRAME && theApp.m_imageWorkFrame!=NULL)
	{
		m_btn.SetDrawImage(false,false,theApp.m_imageWorkFrame,Gdiplus::Rect(3,4,28,28));
		m_btn.SetTextLeft(31);	// 31=3+28
	}else if (m_nType==FRAME_WND_MAIN_FRAME && theApp.m_imageMainFrame!=NULL)
	{
		m_btn.SetDrawImage(false,false,theApp.m_imageMainFrame,Gdiplus::Rect(3,4,28,28));
		m_btn.SetTextLeft(31);	// 31=3+28
		//m_btn.SetDrawToolButtonPic(10,RGB(255,64,64),-1,-1,-1,15,15);
		//m_btn.SetWindowText("志勇");
	}

	if (bShowClose)
	{
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-(m_bBtnShowText?2:0);
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_bBtnShowText?((m_rectBtn.Height()-const_size)/2+1):0);
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (m_close.GetSafeHwnd()==NULL)
		{
			if (!m_close.Create(_T(""),WS_CHILD,m_rectClose,pParent,nID))
				return FALSE;
			m_close.SetToolTipText(_T("关闭"));
			m_close.SetDrawPanelRgn(false);
			//m_close.SetDrawClosePic(true,RGB(27,27,27),RGB(237,28,36),-1,-1,2);
		}else if (m_close.GetSafeHwnd()!=NULL)
		{
			m_close.SetParent(pParent);
			m_close.MoveWindow(&m_rectClose);
			m_close.SetDlgCtrlID(nID);
		}
	}

	m_nUnreadMsgCount = m_pDialog.get()==NULL?0:m_pDialog->GetUnreadMsgCount();
	m_rectMsg.left = m_rectBtn.left + (m_bBtnShowText?36:0);
	if (m_nUnreadMsgCount>=100)
		m_rectMsg.right = m_rectMsg.left + DEFAULT_MSG_WIDTH2;
	else
		m_rectMsg.right = m_rectMsg.left + DEFAULT_MSG_WIDTH1;
	m_rectMsg.top = m_rectBtn.top + (m_bBtnShowText?2:0);
	m_rectMsg.bottom = m_rectMsg.top + 16;
	CString sMsgCaption;
	if (m_nUnreadMsgCount>0)
	{
		sMsgCaption.Format(_T("%d"),m_nUnreadMsgCount);
	}
	if (m_msg.GetSafeHwnd()==NULL)
	{
		DWORD nMsgStyle = WS_CHILD;
		if (m_nUnreadMsgCount>0)
		{
			nMsgStyle |= WS_VISIBLE;
		}
		if (!m_msg.Create(sMsgCaption,nMsgStyle,m_rectMsg,pParent,nID))
			return FALSE;
	}else
	{
		m_msg.SetParent(pParent);
		m_msg.MoveWindow(&m_rectMsg);
		m_msg.SetDlgCtrlID(nID);
		if (m_nUnreadMsgCount>0)
			m_msg.ShowWindow(SW_SHOW);
	}
	if (m_nUnreadMsgCount<10)
		m_msg.SetTextLeft(2);
	//m_msg.SetAutoSize(false);
	m_msg.SetFont(pParent->GetFont());
	m_msg.Load(IDB_PNG_REDCIR_16X16,16);
	//m_msg.SetDrawPanel(true,RGB(255,0,0));	// 244,57,11
	m_msg.SetDrawPanelRgn(false);
	m_msg.SetNorTextColor(theDefaultBtnWhiteColor);
	m_msg.SetHotTextColor(theDefaultBtnWhiteColor);
	m_msg.SetPreTextColor(theDefaultBtnWhiteColor);
	//m_msg.SetNorTextColor(RGB(237,28,36));
	//m_msg.SetHotTextColor(RGB(237,28,36));
	return TRUE;
}
//void CFrameWndInfo::SetBtnShowHideText(bool bShow)
//{
//	if (m_bBtnShowText==bShow) return;
//	m_bBtnShowText = bShow;
//	if (m_btn.GetSafeHwnd()!=NULL)
//	{
//		m_btn.SetWindowText(m_bBtnShowText?m_sBtnShowText:_T(""));
//	}
//}
void CFrameWndInfo::SetBtnText(LPCTSTR lpszBtnText, LPCTSTR lpszToolTipText)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		m_sBtnShowText = lpszBtnText;
		if (m_bBtnShowText)
			m_btn.SetWindowText(lpszBtnText);
		m_btn.SetToolTipText(lpszToolTipText);
	}
}
void CFrameWndInfo::SetToolTipText(LPCTSTR lpszToolTipText)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.SetToolTipText(lpszToolTipText);
}

void CFrameWndInfo::ClearUnreadMsg(bool bFromUserClick)
{
	if (m_nUnreadMsgCount>0)
	{
		if (m_pDialog.get()!=NULL)
		{
			if (!m_pDialog->ClearUnreadMsg(bFromUserClick))
				return;
		}
		m_nUnreadMsgCount = 0;
		if (m_msg.GetSafeHwnd()!=NULL)
		{
			m_msg.SetTextLeft(2);
			m_msg.SetWindowText(_T(""));
			m_msg.ShowWindow(SW_HIDE);
			if (m_rectMsg.Width()!=DEFAULT_MSG_WIDTH1)
			{
				m_rectMsg.right = m_rectMsg.left+DEFAULT_MSG_WIDTH1;
				m_msg.MoveWindow(&m_rectMsg);
			}
		}
	}
}
void CFrameWndInfo::AddUnreadMsg(void)
{
	m_nUnreadMsgCount++;
	if (m_nUnreadMsgCount==10)
		m_msg.SetTextLeft(0);
	if (m_msg.GetSafeHwnd()!=NULL)
	{
		CString stext;
		stext.Format(_T("%d"),m_nUnreadMsgCount);
		m_msg.ShowWindow(SW_HIDE);
		m_msg.SetWindowText(stext);
		if (m_nUnreadMsgCount==100 && m_rectMsg.Width()!=DEFAULT_MSG_WIDTH2)
		{
			m_rectMsg.right = m_rectMsg.left+DEFAULT_MSG_WIDTH2;
			m_msg.MoveWindow(&m_rectMsg);
		}
		m_msg.ShowWindow(SW_SHOW);
	}
}
void CFrameWndInfo::SetUnreadMsg(size_t nUnreadMsgCount)
{
	if (m_nUnreadMsgCount==nUnreadMsgCount) return;
	if (nUnreadMsgCount<=0)
	{
		ClearUnreadMsg();
	}else
	{
		m_nUnreadMsgCount = nUnreadMsgCount;
		if (m_nUnreadMsgCount>=10 && m_msg.GetTextLeft()>0)
			m_msg.SetTextLeft(0);
		else if (m_nUnreadMsgCount<10 && m_msg.GetTextLeft()==0)
			m_msg.SetTextLeft(2);

		if (m_msg.GetSafeHwnd()!=NULL)
		{
			CString stext;
			stext.Format(_T("%d"),m_nUnreadMsgCount);
			m_msg.ShowWindow(SW_HIDE);
			m_msg.SetWindowText(stext);
			//if (m_nUnreadMsgCount==100 && m_rectMsg.Width()!=DEFAULT_MSG_WIDTH2)
			//{
			//	m_rectMsg.right = m_rectMsg.left+DEFAULT_MSG_WIDTH2;
			//	m_msg.MoveWindow(&m_rectMsg);
			//}
			m_msg.ShowWindow(SW_SHOW);
		}
	}
}

void CFrameWndInfo::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	if (m_pDialog.get()!=NULL && m_btn.GetSafeHwnd()!=NULL)
	{
		m_pDialog->ChangeDepartmentInfo(pGroupInfo);
		const CString sFullName(m_pDialog->GetFullName());
		const tstring sFromName(m_pDialog->GetFromName());
		CString sWindowText;
		const size_t nLen = sFromName.size();
		for (size_t i=0;i<nLen;i++)
		{
			sWindowText.AppendChar(sFromName.c_str()[i]);
			if ((i+1)<nLen && sFromName.c_str()[i]<0)	// 中文，需要取二个
			{
				sWindowText.AppendChar(sFromName.c_str()[++i]);
			}
			if (i>=13 && (i+1)<nLen)
			{
				//sWindowText.Append(_T("."));
				break;
			}
		}
		m_sBtnShowText = sWindowText;
		if (m_bBtnShowText)
			m_btn.SetWindowText(sWindowText);
		m_btn.SetToolTipText(sFullName);
		//m_btn.SetToolTipText(sFromName.c_str());
	}

}

void CFrameWndInfo::LineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	if (GetGroupId()==0 && GetFromUserId()==nUserId && m_btn.GetSafeHwnd()!=NULL)
	{
		// 更新左边标签
		const bool bGrayImage = bLineState==EB_LINE_STATE_OFFLINE || bLineState==EB_LINE_STATE_UNKNOWN;
		m_btn.SetDrawImage(bGrayImage);
		if (this->m_nUnreadMsgCount>0 && m_msg.GetSafeHwnd()!=NULL)
		{
			m_msg.Invalidate();			
		}
	}
	if (nGroupCode>0 && GetGroupId()==nGroupCode && m_pDialog.get()!=NULL)
	{
		// 更新群组成员
		m_pDialog->LineStateChange(nUserId, bLineState);
	}
}
void CFrameWndInfo::MoveBtnWindow(LPCRECT rect, bool bShowText) {
	m_rectBtn = rect;
	m_bBtnShowText = bShowText;
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		m_btn.MoveWindow(m_rectBtn);
		m_btn.SetWindowText(m_bBtnShowText?m_sBtnShowText:_T(""));
	}
	//this->SetBtnShowHideText(bShowText);
	const int const_size = 12;
	m_rectClose.right = m_rectBtn.right-(m_bBtnShowText?5:0);
	m_rectClose.left = m_rectClose.right - const_size;
	m_rectClose.top = m_rectBtn.top + (m_bBtnShowText?((m_rectBtn.Height()-const_size)/2):0);
	m_rectClose.bottom = m_rectClose.top+const_size;
	if (m_close.GetSafeHwnd()!=NULL)
		m_close.MoveWindow(m_rectClose);
	if (m_msg.GetSafeHwnd()!=NULL)
	{
		const int nWidth = max(DEFAULT_MSG_WIDTH1,m_rectMsg.Width());
		m_rectMsg.left = m_rectBtn.left + (m_bBtnShowText?36:0);
		m_rectMsg.right = m_rectMsg.left + nWidth;
		m_rectMsg.top = m_rectBtn.top + (m_bBtnShowText?2:0);
		m_rectMsg.bottom = m_rectMsg.top + 16;
		m_msg.MoveWindow(&m_rectMsg);
		//CRect rectMsg(rect);
		//rectMsg.left = rectMsg.right - 25;
		//m_msg.MoveWindow(&rectMsg);
	}
}
void CFrameWndInfo::SetMsgBtnText(LPCTSTR lpszText) {
	if (m_msg.GetSafeHwnd()!=NULL)
	{
		m_msg.SetWindowText(lpszText);
		const size_t nSize = strlen(lpszText);
		if (nSize==0 && m_msg.IsWindowVisible())
			m_msg.ShowWindow(SW_HIDE);
		else if (nSize>0 && !m_msg.IsWindowVisible())
			m_msg.ShowWindow(SW_SHOW);
	}
}
void CFrameWndInfo::CheckMousePos(POINT point){
	if (m_btn.GetSafeHwnd()==NULL || m_close.GetSafeHwnd()==NULL)
		return;
	if (m_rectBtn.PtInRect(point))
	{
		if (m_rectClose.PtInRect(point))
		{
			//if (m_btn.GetChecked())
			//	m_close.SetDrawClosePic(true,theApp.GetBgColor3(),-1,-1,-1,2);
			//else
			//	m_close.SetDrawClosePic(true,theApp.GetBgTitle0(),-1,-1,-1,2);
			m_close.SetDrawClosePic(true,theDefaultBtnWhiteColor,-1,-1,-1,2);
			m_close.SetDrawPanel(true,RGB(237,28,36));
		}else
		{
#ifdef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
			const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
#else
			if (theApp.GetHideMainFrame())
#endif
			{
				m_close.SetDrawClosePic(true,RGB(27,27,27),-1,-1,-1,2);
				if (m_btn.GetChecked())
					m_close.SetDrawPanel(true,theApp.GetSelColor());
				else
					m_close.SetDrawPanel(true,theApp.GetHotColor());
			}else
			{
				m_close.SetDrawClosePic(true,theDefaultBtnWhiteColor,-1,-1,-1,2);
				if (m_btn.GetChecked())
					m_close.SetDrawPanel(true,theApp.GetHotColor1());	// RGB(64,64,64)
				else
					m_close.SetDrawPanel(true,theApp.GetHotColor());		// RGB(128,128,128)
			}
#else
			m_close.SetDrawClosePic(true,RGB(27,27,27),-1,-1,-1,2);
			if (m_btn.GetChecked())
				m_close.SetDrawPanel(true,theApp.GetSelColor());
			else
				m_close.SetDrawPanel(true,theApp.GetHotColor());
#endif
			//if (m_btn.GetChecked())
			//	m_close.SetDrawPanel(true,theApp.GetBgColor3());
			//else
			//	m_close.SetDrawPanel(true,theApp.GetBgTitle0());
		}
		m_close.ShowWindow(SW_SHOW);
		m_close.Invalidate();
	}else if (m_close.IsWindowVisible())
	{
		m_close.ShowWindow(SW_HIDE);
	}
}
bool CFrameWndInfo::IsClickClose(POINT point) const
{
	return (m_close.GetSafeHwnd()!=NULL&&m_rectClose.PtInRect(point))?true:false;
}
void CFrameWndInfo::SendClose(void){
	if (m_pDialog.get()!=NULL)
		m_pDialog->PostMessage(WM_CLOSE, 0, 0);
}
void CFrameWndInfo::ShowHide(bool bShowAndChecked)
{
	if (m_pDialog.get()!=NULL && m_pDialog->GetSafeHwnd()!=NULL)
	{
		m_pDialog->ShowWindow(bShowAndChecked?SW_SHOW:SW_HIDE);
		if (bShowAndChecked)
		{
			m_pDialog->SetInputFocus();
		}
		//if (bShowAndChecked)
		//{
		//	CEBCCallInfo::pointer m_pEbCallInfo = m_pDialog->GetCallInfo();
		//	CDlgFuncWindow * pOldFuncWindow = NULL;
		//	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		//		theApp.m_pTempGroupMsg.find(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
		//	else
		//		theApp.m_pTempFromUserMsg.find(m_pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
		//	// **必须放在前面
		//	if (pOldFuncWindow!=NULL)
		//	{
		//		pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
		//		pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
		//		theApp.m_pCloseWnd.add(pOldFuncWindow);
		//	}
		//}
	}
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		if (m_btn.GetChecked()!=bShowAndChecked)
		{
			m_btn.SetChecked(bShowAndChecked);
			m_btn.Invalidate();
		}
	}
	if (bShowAndChecked && m_nUnreadMsgCount>0)
	{
		if (GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG)
			ClearUnreadMsg();
		else if (m_msg.GetSafeHwnd()!=NULL)
			m_msg.Invalidate();
	}else if (!bShowAndChecked && m_nUnreadMsgCount>0 && m_msg.GetSafeHwnd()!=NULL)
	{
		m_msg.Invalidate();
	}
}

CFrameWndInfoProxy::CFrameWndInfoProxy(CWnd* pParent, bool bChildMode, CFrameWndInfoProxyCallback* pCallback)
: m_pParent(pParent), m_bChildMode(bChildMode), m_pCallback(pCallback)
{
	ASSERT(m_pParent!=NULL);
	//ASSERT(m_pCallback!=NULL);
	for (int i=FRAME_BTN_ID_BEGIN;i<=FRAME_BTN_ID_MAX; i++)
	//for (int i=FRAME_BTN_ID_MIN;i<=FRAME_BTN_ID_MAX; i++)
		m_pIndexIdList.add(i);

	m_nCurrentHideIndex = 0;
	m_bShowedLeft = true;
}
CFrameWndInfoProxy::~CFrameWndInfoProxy(void)
{
	if (m_childFont.GetSafeHandle()!=NULL)
		m_childFont.DeleteObject();
}

void CFrameWndInfoProxy::SetShowHideLeft(bool bShow)
{
	if (bShow!=m_bShowedLeft)
	{
		m_bShowedLeft = bShow;
		RebuildBtnSize();
	}
}

void CFrameWndInfoProxy::SetCtrlColor(bool bInvalidate)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			pFrameWndInfo->SetCtrlColor(bInvalidate);
		}
	}
}
void CFrameWndInfoProxy::SetChildFont(int nFontSize, LPCTSTR lpszFace)
{
	if (m_childFont.GetSafeHandle()==NULL)
		m_childFont.CreatePointFont(nFontSize,lpszFace);
}

void CFrameWndInfoProxy::AddWnd(CFrameWndInfo::FRAME_WND_TYPE nType, LPCTSTR lpszName, LPCTSTR lpszTooltip, bool bShow, bool bAutoCall)
{

	//if (!bShow && m_pList.empty())
	//{
	//	bShow = true;
	//}
	int nIndexId = 0;
	CFrameWndInfo::pointer pFrameWndInfo;
	if (nType==CFrameWndInfo::FRAME_WND_WORK_FRAME && !m_pList.empty())
	{
		{
			BoostReadLock rdLock(m_pList.mutex());
			CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
			if (pIter!=m_pList.end())
			{
				pFrameWndInfo = *pIter;
				if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
				{
					if (bShow)
					{
						rdLock.unlock();
						ShowWnd(pFrameWndInfo->GetUserData());
					}
					return;
				}
			}
		}
		nIndexId = FRAME_BTN_ID_WORKFRAME;
		//m_pIndexIdList.front(nIndexId);
		pFrameWndInfo = CFrameWndInfo::create(NULL,m_bChildMode,nType);
		pFrameWndInfo->SetUserData(nIndexId);
		m_pList.pushfront(pFrameWndInfo);
	}

	const bool bRebuildBtnSize = pFrameWndInfo.get()!=NULL?true:false;
	if (pFrameWndInfo.get()==NULL)
	{
		if (nType==CFrameWndInfo::FRAME_WND_WORK_FRAME)
			nIndexId = FRAME_BTN_ID_WORKFRAME;
		else if (nType==CFrameWndInfo::FRAME_WND_MAIN_FRAME)
			nIndexId = FRAME_BTN_ID_MAINFRAME;
		else
			m_pIndexIdList.front(nIndexId);
		pFrameWndInfo = CFrameWndInfo::create(NULL,m_bChildMode,nType);
		pFrameWndInfo->SetUserData(nIndexId);
		m_pList.add(pFrameWndInfo);
	}
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:(nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?const_hide_border_left2:const_hide_border_left1);
#else
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left1;
#endif
	const bool bShowClose = nType>=CFrameWndInfo::FRAME_WND_CALL_DIALOG?true:false;
	CRect rectLable;
	rectLable.left = 4;
	rectLable.top = 5 + (m_pList.size()-1)*const_frame_height;
	rectLable.right = nBorderLeft-3;
#ifdef USES_NEW_UI_160111
	if (m_bChildMode)
	{
		rectLable.left = 1;
		rectLable.top += (NEW_UI_160111_HEIGHT-5);
		rectLable.right = nBorderLeft-1;
	}
#endif
	rectLable.bottom = rectLable.top+const_frame_height;
	CFont * pFont = m_childFont.GetSafeHandle()==NULL?NULL:&m_childFont;
	pFrameWndInfo->Create(lpszName, rectLable, m_pParent, pFrameWndInfo->GetUserData(),pFont,bShowClose,m_bShowedLeft);
	if (lpszTooltip!=NULL && strlen(lpszTooltip)>0)
		pFrameWndInfo->SetToolTipText(lpszTooltip);

	if (bRebuildBtnSize)
		this->RebuildBtnSize();
	if (bShow && !bAutoCall)
		ShowWnd((unsigned int)nIndexId);

}
void CFrameWndInfoProxy::AddWnd(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow, bool bAutoCall, bool bForceAddToList)
{
	if (!bForceAddToList && !bShow)
	{
		m_pHideList.add(pFrameWndInfo);
		return;
	}
	//if (!bShow && m_pList.empty())
	if (m_pList.empty())
	{
		bForceAddToList = true;	// 完善show
		bShow = true;
	}
	int nIndexId = 0;
	m_pIndexIdList.front(nIndexId);
	pFrameWndInfo->SetUserData(nIndexId);
	m_pList.add(pFrameWndInfo);
	if (pFrameWndInfo->GetDialog().get()!=NULL)
	{
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:(nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?const_hide_border_left2:const_hide_border_left1);
#else
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left1;
#endif
		CRect rectLable;
		rectLable.left = 4;
		rectLable.top = 5 + (m_pList.size()-1)*const_frame_height;
		rectLable.right = nBorderLeft-3;
#ifdef USES_NEW_UI_160111
		if (m_bChildMode)
		{
			rectLable.left = 1;
			rectLable.top += (NEW_UI_160111_HEIGHT-5);
			rectLable.right = nBorderLeft-1;
		}
#endif
		rectLable.bottom = rectLable.top+const_frame_height;
		const tstring sFromName(pFrameWndInfo->GetDialog()->GetFromName());
		CFont * pFont = m_childFont.GetSafeHandle()==NULL?NULL:&m_childFont;
		pFrameWndInfo->Create(sFromName.c_str(), rectLable, m_pParent, pFrameWndInfo->GetUserData(),pFont,true,m_bShowedLeft);
		pFrameWndInfo->SetToolTipText(pFrameWndInfo->GetDialog()->GetFullName());

		CRect rect;
		m_pParent->GetWindowRect(&rect);
#ifdef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
#else
		if (theApp.GetHideMainFrame())
#endif
			pFrameWndInfo->GetDialog()->MoveWindow(nBorderLeft,const_border_size,rect.Width()-nBorderLeft-const_border_size,rect.Height()-const_border_size*2);
		else
			pFrameWndInfo->GetDialog()->MoveWindow(nBorderLeft,NEW_UI_160111_HEIGHT,rect.Width()-nBorderLeft-const_border_size,rect.Height()-(NEW_UI_160111_HEIGHT+const_border_size));
#else
		pFrameWndInfo->GetDialog()->MoveWindow(const_border_left,const_border_size,rect.Width()-const_border_left-const_border_size,rect.Height()-const_border_size*2);
#endif
		const bool bCallDialogHide = pFrameWndInfo->GetDialog()->IsWindowVisible()?false:true;
		if (bShow && (bForceAddToList || (!bAutoCall && !bCallDialogHide && m_pParent->IsWindowVisible())))
			ShowWnd((unsigned int)nIndexId);
		else if (!bShow)
		{
			pFrameWndInfo->ShowHide(false);
			 if (this->m_pCallback!=NULL)
				 m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
		}
		pFrameWndInfo->GetDialog()->ScrollToEnd();
	}
	//if (m_nShrinkageWin)
	//{
	//	KillTimer(TIMERID_SHRINKAGE_WIN);
	//	m_nShrinkageWin = false;
	//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	//	this->ShowWindow(SW_SHOW);
	//	SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//}
}
void CFrameWndInfoProxy::SetTitle(CFrameWndInfo::FRAME_WND_TYPE nType, LPCTSTR lpszName, LPCTSTR lpszTooltip)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetType()==nType)
			{
				pFrameWndInfo->SetBtnText(lpszName, lpszTooltip);
				return;
			}
		}
	}
}

void CFrameWndInfoProxy::RebuildBtnSize(void)
{
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:(nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?const_hide_border_left2:const_hide_border_left1);
#else
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left1;
#endif
	int nIndex = 0;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		CRect rectLable;
		rectLable.left = 4;
		rectLable.top = 5 + nIndex*const_frame_height;
		rectLable.right = nBorderLeft-3;
#ifdef USES_NEW_UI_160111
		if (m_bChildMode)
		{
			rectLable.left = 1;
			rectLable.top += (NEW_UI_160111_HEIGHT-5);
			rectLable.right = nBorderLeft-1;
		}
#endif
		rectLable.bottom = rectLable.top+const_frame_height;
		pFrameWndInfo->MoveBtnWindow(&rectLable,m_bShowedLeft);
		nIndex++;
	}
}

bool CFrameWndInfoProxy::AddUnreadMsg(eb::bigint nCallId, eb::bigint nMsgId)
{
	bool bResult = false;
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				pFrameWndInfo->AddUnreadMsg();
				//return true;
				bResult = true;
				goto AddUnreadMsgCount;
			}
		}
	}

	//if (m_pParent->IsWindowVisible())
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				m_pHideList.erase(pIter);
				wtLock.unlock();
				this->AddWnd(pFrameWndInfo,true,false);
				pFrameWndInfo->AddUnreadMsg();
				bResult = true;
				goto AddUnreadMsgCount;
				//return true;
			}
		}
	}
AddUnreadMsgCount:
	if (nMsgId>0)
	{
		// *** read_flag=1 已经读；
		// *** read_flag=2 对方接收回执
		CString sSql;
		sSql.Format(_T("UPDATE msg_record_t SET read_flag=read_flag&2 WHERE msg_id=%lld AND (read_flag&1)=1"),nMsgId);	// 1->0;3->2
		//sSql.Format(_T("UPDATE msg_record_t SET read_flag=0 WHERE msg_id=%lld AND read_flag=1"),nMsgId);
		theApp.m_pBoUsers->execute(sSql);
	}
	return bResult;
	//return false;
}
bool CFrameWndInfoProxy::SetUnreadMsg(eb::bigint nCallId, size_t nUnreadMsgCount)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				pFrameWndInfo->SetUnreadMsg(nUnreadMsgCount);
				return true;
			}
		}
	}
	return false;
}

void CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_TYPE nType, size_t nUnreadMsgCount)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetType()==nType)
			{
				pFrameWndInfo->SetUnreadMsg(nUnreadMsgCount);
				return;
			}
		}
	}
}

void CFrameWndInfoProxy::ClearWnd(void)
{
	m_pHideList.clear();
	//{
	//	BoostWriteLock wtLock(m_pList.mutex());
	//	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	//	for (; pIter!=m_pList.end(); pIter++)
	//	{
	//		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
	//		if (pFrameWndInfo->GetUserData()>0)
	//			m_pIndexIdList.pushfront(pFrameWndInfo->GetUserData());
	//	}
	//	m_pList.clear(false);
	//}
	m_pList.clear();
	RebuildBtnSize();

}
void CFrameWndInfoProxy::DelWnd(eb::bigint nCallId)
{
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				m_pHideList.erase(pIter);
				m_pCloseList.add(pFrameWndInfo);
				break;
			}
		}
	}
	CFrameWndInfo::pointer pPrevFrameWndInfo;
	{
		BoostWriteLock wtLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				m_pIndexIdList.pushfront(pFrameWndInfo->GetUserData());
				m_pList.erase(pIter);
				m_pCloseList.add(pFrameWndInfo);
				//wtLock.unlock();
				//if (m_pList.empty())
				//{
				//	if (m_nShrinkageWin)
				//	{
				//		KillTimer(TIMERID_SHRINKAGE_WIN);
				//		m_nShrinkageWin = false;
				//		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				//	}
				//	this->ShowWindow(SW_HIDE);
				//	return;
				//}else if (pPrevFrameWndInfo.get()!=NULL)
				//	pPrevFrameWndInfo->ShowHide(true);
				//else
				//	ShowFirstWnd();
				//RebuildBtnSize();
				break;
			}
			pPrevFrameWndInfo = pFrameWndInfo;
		}
	}
	if (m_pList.empty())
	{
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndEmpty();
		//if (m_nShrinkageWin)
		//{
		//	KillTimer(TIMERID_SHRINKAGE_WIN);
		//	m_nShrinkageWin = false;
		//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		//}
		//this->ShowWindow(SW_HIDE);
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pPrevFrameWndInfo,true);
	}else
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsChecked())
			{
				pFrameWndInfo->ShowHide(true);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pFrameWndInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			pPrevFrameWndInfo->ShowHide(true);
			if (m_pCallback!=NULL)
				m_pCallback->OnFrameWndShow(pPrevFrameWndInfo,true);
		}
	}
	RebuildBtnSize();
}
void CFrameWndInfoProxy::DelWnd(const CWnd* pWnd)
{
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsWndAddress(pWnd))
			{
				m_pHideList.erase(pIter);
				m_pCloseList.add(pFrameWndInfo);
				break;
			}
		}
	}
	CFrameWndInfo::pointer pPrevFrameWndInfo;
	{
		BoostWriteLock wtLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsWndAddress(pWnd))
			{
				m_pIndexIdList.pushfront(pFrameWndInfo->GetUserData());
				m_pList.erase(pIter);
				m_pCloseList.add(pFrameWndInfo);
				//wtLock.unlock();
				//if (m_pList.empty())
				//{
				//	if (m_nShrinkageWin)
				//	{
				//		KillTimer(TIMERID_SHRINKAGE_WIN);
				//		m_nShrinkageWin = false;
				//		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				//	}
				//	this->ShowWindow(SW_HIDE);
				//	return;
				//}else if (pPrevFrameWndInfo.get()!=NULL)
				//	pPrevFrameWndInfo->ShowHide(true);
				//else
				//	ShowFirstWnd();
				//RebuildBtnSize();
				break;
			}
			pPrevFrameWndInfo = pFrameWndInfo;
		}
	}
	if (m_pList.empty())
	{
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndEmpty();
		//if (m_nShrinkageWin)
		//{
		//	KillTimer(TIMERID_SHRINKAGE_WIN);
		//	m_nShrinkageWin = false;
		//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		//}
		//this->ShowWindow(SW_HIDE);
		//if (m_rectAdjustOld.Width()>0)
		//{
		//	this->MoveWindow(m_rectAdjustOld);
		//}
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pPrevFrameWndInfo,true);
	}else
	{
		{
			BoostReadLock rdLock(m_pList.mutex());
			CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				CFrameWndInfo::pointer pFrameWndInfo = *pIter;
				if (pFrameWndInfo->IsChecked())
				{
					pFrameWndInfo->ShowHide(true);
					if (m_pCallback!=NULL)
						m_pCallback->OnFrameWndShow(pFrameWndInfo,true);

					pPrevFrameWndInfo.reset();
					break;
				}else if (pPrevFrameWndInfo.get()==NULL)
				{
					pPrevFrameWndInfo = pFrameWndInfo;
				}
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			//if (pPrevFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pList.size()==1 && pPrevFrameWndInfo->Get
			pPrevFrameWndInfo->ShowHide(true);
			if (m_pCallback!=NULL)
				m_pCallback->OnFrameWndShow(pPrevFrameWndInfo,true);
		}
	}
	RebuildBtnSize();

}
void CFrameWndInfoProxy::ShowWnd(eb::bigint nCallId)
{
	bool bFindInfo = false;
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				bFindInfo = true;
				pFrameWndInfo->ClearUnreadMsg();
				pFrameWndInfo->ShowHide(true);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
			}else
			{
				pFrameWndInfo->ShowHide(false);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
			}
		}
	}
	if (!bFindInfo)
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				m_pHideList.erase(pIter);
				wtLock.unlock();
				AddWnd(pFrameWndInfo,true,false);
				break;
			}
		}
	}
}

void CFrameWndInfoProxy::ShowWnd(unsigned int nWndIndex)
{
	bool bFindInfo = false;
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetUserData()==nWndIndex)
			{
				bFindInfo = true;
				if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG)
					pFrameWndInfo->ClearUnreadMsg();
				pFrameWndInfo->ShowHide(true);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
			}else
			{
				pFrameWndInfo->ShowHide(false);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
			}
		}
	}
	if (!bFindInfo)
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetUserData()==nWndIndex)
			{
				m_pHideList.erase(pIter);
				wtLock.unlock();
				AddWnd(pFrameWndInfo,true, false);
				break;
			}
		}
	}
}

void CFrameWndInfoProxy::OnFirstWorkFrameWndShow(void) const
{
	if (m_pCallback==NULL) return;

	AUTO_CONST_RLOCK(m_pList);
	CLockList<CFrameWndInfo::pointer>::const_iterator pIter = m_pList.begin();
	if (pIter!=m_pList.end())
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked() && pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
		{
			m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
		}
	}
}

void CFrameWndInfoProxy::ShowFirstWnd(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
		return;
	}
}
bool CFrameWndInfoProxy::ShowUpWnd(void)
{
	if (m_pList.size()<=1) return false;
	CFrameWndInfo::pointer pHideFrameWndInfo;
	CFrameWndInfo::pointer pShowFrameWndInfo;
	CFrameWndInfo::pointer pPrevFrameWndInfo;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pHideFrameWndInfo = pFrameWndInfo;
			if (pPrevFrameWndInfo.get()!=NULL)	// 前面有未显示
			{
				pShowFrameWndInfo = pPrevFrameWndInfo;
				break;
			}
			continue;
		}
		pPrevFrameWndInfo = pFrameWndInfo;
		//if (pHideFrameWndInfo.get()!=NULL)	// 前面已经找到已经显示，后面有未显示
		//{
		//	pShowFrameWndInfo = pPrevFrameWndInfo;
		//	break;	// 不能退出，因为要取最后一个；
		//}
	}
	if (pHideFrameWndInfo.get()!=NULL && pPrevFrameWndInfo.get()!=NULL && pShowFrameWndInfo.get()==NULL)
	{
		pShowFrameWndInfo = pPrevFrameWndInfo;	// 这是最后一个；
	}
	if (pShowFrameWndInfo.get()!=NULL)
	{
		pShowFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pShowFrameWndInfo,true);
		if (pHideFrameWndInfo.get()!=NULL)
		{
			pHideFrameWndInfo->ShowHide(false);
			if (m_pCallback!=NULL)
				m_pCallback->OnFrameWndShow(pHideFrameWndInfo,false);
		}
		return true;
	}
	return false;
}
bool CFrameWndInfoProxy::ShowDownWnd(void)
{
	if (m_pList.size()<=1) return false;
	CFrameWndInfo::pointer pFirstFrameWndInfo;
	CFrameWndInfo::pointer pLastFrameWndInfo;
	CFrameWndInfo::pointer pHideFrameWndInfo;
	CFrameWndInfo::pointer pShowFrameWndInfo;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFirstFrameWndInfo.get()==NULL)
			pFirstFrameWndInfo = pFrameWndInfo;
		pLastFrameWndInfo = pFrameWndInfo;
		if (pFrameWndInfo->IsChecked())
		{
			pHideFrameWndInfo = pFrameWndInfo;
			continue;
		}
		pShowFrameWndInfo = pFrameWndInfo;
		if (pHideFrameWndInfo.get()!=NULL)	// 前面已经找到已经显示，后面有未显示（*前面也会正常）
		{
			break;
		}
	}
	if (pHideFrameWndInfo.get()==pLastFrameWndInfo.get())
	{
		pShowFrameWndInfo = pFirstFrameWndInfo;	// 这是第一个；
	}
	if (pShowFrameWndInfo.get()!=NULL)
	{
		pShowFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pShowFrameWndInfo,true);
		if (pHideFrameWndInfo.get()!=NULL)
		{
			pHideFrameWndInfo->ShowHide(false);
			if (m_pCallback!=NULL)
				m_pCallback->OnFrameWndShow(pHideFrameWndInfo,false);
		}
		return true;
	}
	return false;
}
bool CFrameWndInfoProxy::ShowOffsetWnd(int nShowOffset)
{
	// nShowOffset=-1: end
	if ((nShowOffset<0 && m_pList.size()<=1) || (nShowOffset>=0 && (m_pList.size()-1)<(size_t)nShowOffset)) return false;
	int nCurrentOffset = 0;
	CFrameWndInfo::pointer pShowFrameWndInfo;
	CFrameWndInfo::pointer pHideFrameWndInfo;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (nShowOffset>=0 && (nCurrentOffset++)==nShowOffset)
		{
			pShowFrameWndInfo = pFrameWndInfo;
			if (pShowFrameWndInfo->IsChecked())
				return true;
			else if (pHideFrameWndInfo.get()!=NULL)	// *前面已经找到　hide
				break;
			continue;
		}
		if (pFrameWndInfo->IsChecked())
		{
			pHideFrameWndInfo = pFrameWndInfo;
			if (nShowOffset>=0 && pShowFrameWndInfo.get()!=NULL)		// *前面已经找到　show
				break;
		}
		if (nShowOffset<0)
			pShowFrameWndInfo = pFrameWndInfo;
	}
	if (pShowFrameWndInfo.get()!=NULL && pHideFrameWndInfo.get()!=NULL && pShowFrameWndInfo.get()!=pHideFrameWndInfo.get())
	{
		pShowFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pShowFrameWndInfo,true);
		//if (pHideFrameWndInfo.get()!=NULL)
			pHideFrameWndInfo->ShowHide(false);
			if (m_pCallback!=NULL)
				m_pCallback->OnFrameWndShow(pHideFrameWndInfo,false);
		return true;
	}
	return false;
}
void CFrameWndInfoProxy::ShowWnd(const CWnd* pWnd)
{
	bool bFindInfo = false;
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsWndAddress(pWnd))
			{
				bFindInfo = true;
				if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG)
					pFrameWndInfo->ClearUnreadMsg();
				pFrameWndInfo->ShowHide(true);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
			}else
			{
				pFrameWndInfo->ShowHide(false);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
			}
		}
	}
	if (!bFindInfo)
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsWndAddress(pWnd))
			{
				m_pHideList.erase(pIter);
				wtLock.unlock();
				AddWnd(pFrameWndInfo,true,false);
				break;
			}
		}
	}
}
void CFrameWndInfoProxy::ShowHideCurrent(bool bShow)
{
	//bool bFinded = false;
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->IsChecked() && !bShow)
			{
				m_nCurrentHideIndex = pFrameWndInfo->GetUserData();
				pFrameWndInfo->ShowHide(false);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
				return;
			}else if (bShow && m_nCurrentHideIndex==pFrameWndInfo->GetUserData())
			{
				pFrameWndInfo->ShowHide(true);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
				return;
			}
		}
	}
}

void CFrameWndInfoProxy::ClickFrame(unsigned int nWndIndex)
{
	CFrameWndInfo::pointer pCheckedFrameWndInfo;
	bool bClose = false;
	{
		CPoint pos;
		GetCursorPos(&pos);
		m_pParent->ScreenToClient(&pos);
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetUserData()==nWndIndex)
			{
				if (pFrameWndInfo->IsClickClose(pos))
				{
					bClose = true;
					pFrameWndInfo->SendClose();
					return;
				}else
				{
					if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG)
						pFrameWndInfo->ClearUnreadMsg(true);
					pFrameWndInfo->ShowHide(true);
					if (m_pCallback!=NULL)
						m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
				}
			}else
			{
				if (pFrameWndInfo->IsChecked())
					pCheckedFrameWndInfo = pFrameWndInfo;
				pFrameWndInfo->ShowHide(false);
				if (m_pCallback!=NULL)
					m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
			}
		}
	}
	if (bClose && pCheckedFrameWndInfo.get()!=NULL)
	{
		pCheckedFrameWndInfo->ShowHide(true);
		if (m_pCallback!=NULL)
			m_pCallback->OnFrameWndShow(pCheckedFrameWndInfo,true);
	}
}

bool CFrameWndInfoProxy::ExistCallIdWnd(eb::bigint nCallId) const
{
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CFrameWndInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				return true;
			}
		}
	}
	{
		AUTO_CONST_RLOCK(m_pHideList);
		CLockList<CFrameWndInfo::pointer>::const_iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				return true;
			}
		}
	}
	return false;
}
bool CFrameWndInfoProxy::RemoveHideWnd(eb::bigint nCallId)
{
	BoostWriteLock wtLock(m_pHideList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
	for (; pIter!=m_pHideList.end(); pIter++)
	{
		const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetCallId()==nCallId)
		{
			m_pHideList.erase(pIter);
			return true;
		}
	}
	return false;
}

CDlgDialog::pointer CFrameWndInfoProxy::GetCallIdDialog(eb::bigint nCallId, bool bRemove, bool bAutoCall)
{
	if (theWantExitApp) return NullDlgDialog;
	{
		CFrameWndInfo::pointer pPrevFrameWndInfo;
		BoostWriteLock wtLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				if (bRemove)
				{
					m_pList.erase(pIter);
					wtLock.unlock();

					if (m_pList.empty())
					{
						if (m_pCallback!=NULL)
							m_pCallback->OnFrameWndEmpty();
						//if (m_nShrinkageWin)
						//{
						//	KillTimer(TIMERID_SHRINKAGE_WIN);
						//	m_nShrinkageWin = false;
						//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
						//}
						//this->ShowWindow(SW_HIDE);
						return pFrameWndInfo->GetDialog();
					}else if (pPrevFrameWndInfo.get()!=NULL)
					{
						pPrevFrameWndInfo->ShowHide(true);
						if (m_pCallback!=NULL)
							m_pCallback->OnFrameWndShow(pPrevFrameWndInfo,true);
					}else
						ShowFirstWnd();
					RebuildBtnSize();
				}else// if (pFrameWndInfo->IsChecked() && m_pParent->IsWindowVisible())
				{
					if (!bAutoCall && pFrameWndInfo->IsChecked() && m_pParent->IsWindowVisible())
					{
						pFrameWndInfo->ClearUnreadMsg();
					}
				}
				return pFrameWndInfo->GetDialog();
			}
		}
	}
	{
		BoostWriteLock wtLock(m_pHideList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
		for (; pIter!=m_pHideList.end(); pIter++)
		{
			const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				if (bRemove)
					m_pHideList.erase(pIter);
				return pFrameWndInfo->GetDialog();
			}
		}
	}
	return NullDlgDialog;
}
void CFrameWndInfoProxy::ResetUserData(const CFrameWndInfo::pointer& pFrameWndInfo)
{
	const int nUserData = (int)pFrameWndInfo->GetUserData();
	if (nUserData>0)
	{
		pFrameWndInfo->SetUserData(0);
		if (nUserData>=FRAME_BTN_ID_BEGIN)
			m_pIndexIdList.pushfront(nUserData);
	}
}
void CFrameWndInfoProxy::RefreshChecked(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (!pFrameWndInfo->IsChecked()) continue;
		switch (pFrameWndInfo->GetType())
		{
		case CFrameWndInfo::FRAME_WND_CALL_DIALOG:
			{
				if (pFrameWndInfo->GetDialog().get()!=NULL)
				{
					pFrameWndInfo->GetDialog()->RefreshWeb();
				}
			}break;
		default:
			break;
		}
		break;
	}
}

void CFrameWndInfoProxy::OnMove(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		switch (pFrameWndInfo->GetType())
		{
		case CFrameWndInfo::FRAME_WND_CALL_DIALOG:
			{
				if (pFrameWndInfo->GetDialog().get()!=NULL)
					pFrameWndInfo->GetDialog()->OnMove();
			}break;
		default:
			break;
		}
	}
}
void CFrameWndInfoProxy::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetCallId()==pGroupInfo->m_sGroupCode)
		{
			pFrameWndInfo->ChangeDepartmentInfo(pGroupInfo);
			break;
		}
	}
}

void CFrameWndInfoProxy::UserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE nLineState)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		//if (pFrameWndInfo->GetGroupId()==0 && pFrameWndInfo->GetFromUserId()==nUserId)
		{
			pFrameWndInfo->LineStateChange(nGroupCode, nUserId, nLineState);
		}
	}

}

void CFrameWndInfoProxy::CheckMousePos(void)
{
	CPoint pos;
	GetCursorPos(&pos);
	m_pParent->ScreenToClient(&pos);
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->CheckMousePos(pos);
	}
}

void CFrameWndInfoProxy::SetCurrentFocus(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			if (pFrameWndInfo->GetDialog().get()!=NULL)
				pFrameWndInfo->GetDialog()->SetInputFocus();
			return;
		}
	}

}

#ifdef USES_EBCOM_TEST
void CFrameWndInfoProxy::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
			pFrameWndInfo->GetDialog()->OnUserEmpInfo(pMemberInfo);
	}
}
#else
void CFrameWndInfoProxy::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
			pFrameWndInfo->GetDialog()->OnUserEmpInfo(pMemberInfo, bSort);
	}
}
#endif
void CFrameWndInfoProxy::OnRemoveGroup(eb::bigint nGroupId)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
			pFrameWndInfo->GetDialog()->OnRemoveGroup(nGroupId);
	}
}
void CFrameWndInfoProxy::OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
			pFrameWndInfo->GetDialog()->OnRemoveMember(nGroupId, nMemberId);
	}
}
void CFrameWndInfoProxy::OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			if (pFrameWndInfo->GetDialog()->OnResourceMove(pResourceInfo, nOldParentResId))
				break;
		}
	}
}
void CFrameWndInfoProxy::OnResourceInfo(const EB_ResourceInfo& pResourceInfo)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			if (pFrameWndInfo->GetDialog()->OnResourceInfo(pResourceInfo))
				break;
		}
	}
}
void CFrameWndInfoProxy::OnResourceDelete(const EB_ResourceInfo& pResourceInfo)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			if (pFrameWndInfo->GetDialog()->OnResourceDelete(pResourceInfo))
				break;
		}
	}
}

void CFrameWndInfoProxy::OnExitRD(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->OnExitRD();
	}
}
