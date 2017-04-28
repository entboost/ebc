#include "StdAfx.h"
#include "POP.h"
#include "EBFuncButton.h"

CEBFuncButton::CEBFuncButton(void)
: m_nUnreadMsgCount(0)
{
}

CEBFuncButton::~CEBFuncButton(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.DestroyWindow();
	if (m_msg.GetSafeHwnd()!=NULL)
		m_msg.DestroyWindow();
}

bool CEBFuncButton::Create(CWnd* pParentWnd,const RECT& rect,UINT nID,const EB_SubscribeFuncInfo& pFuncInfo,const tstring& sDefaultImage)
{
	if (m_btn.GetSafeHwnd()==NULL)
	{
		if (!m_btn.Create(_T(""),WS_CHILD|WS_VISIBLE,rect,pParentWnd,nID))
			return false;
		m_pFuncInfo = pFuncInfo;
		const tstring sButtunImage(pFuncInfo.m_sResFile.empty()?sDefaultImage:pFuncInfo.m_sResFile);
		if (sButtunImage.empty())
			return false;
		CImage orgImg;
		if (orgImg.Load(sButtunImage.c_str())!=S_OK)
		{
			return false;
		}
		if (orgImg.GetWidth()>(orgImg.GetHeight()*2))
		{
			// 恩布默认风格图片
			m_btn.SetAutoSize(false);
			m_btn.Load(sButtunImage.c_str());
		}else
		{
			// 用户自定义图片
			m_btn.SetAutoSize(true);
			m_btn.SetDrawPanelRgn(false);
			m_btn.Load(sButtunImage.c_str(),orgImg.GetWidth(),orgImg.GetHeight());
			m_btn.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
			//m_btn.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
		}
		m_btn.SetToolTipText(pFuncInfo.m_sFunctionName.c_str());
	}
	if (m_msg.GetSafeHwnd()==NULL)
	{
		m_rectMsg.right = rect.right + 2;
		m_rectMsg.left = m_rectMsg.right - 16;
		m_rectMsg.top = rect.top - 2;
		m_rectMsg.bottom = m_rectMsg.top + 16;
		//m_nUnreadMsgCount = 9;			// for test
		DWORD nMsgStyle = WS_CHILD;
		CString sMsgCaption;
		if (m_nUnreadMsgCount>0)
		{
			nMsgStyle |= WS_VISIBLE;
			sMsgCaption.Format(_T("%d"),m_nUnreadMsgCount);
		}
		m_msg.SetTextHotMove(false); 
		if (!m_msg.Create(sMsgCaption,nMsgStyle,m_rectMsg,pParentWnd,nID))
		{
			//return false;
		}else
		{
			if (m_nUnreadMsgCount<10)
				m_msg.SetTextLeft(2);
			m_msg.SetFont(pParentWnd->GetFont());
			m_msg.Load(IDB_PNG_REDCIR_16X16,16);
			m_msg.SetDrawPanelRgn(false);
			m_msg.SetNorTextColor(theDefaultBtnWhiteColor);
			m_msg.SetHotTextColor(theDefaultBtnWhiteColor);
			m_msg.SetPreTextColor(theDefaultBtnWhiteColor);
		}
	}

	return true;
}

void CEBFuncButton::UpdateResFile(const tstring& sResFile)
{
	if (m_btn.GetSafeHwnd()!=NULL && PathFileExists(sResFile.c_str()))
	{
		if (m_pFuncInfo.m_sResFile!=sResFile)
		{
			m_pFuncInfo.m_sResFile = sResFile;
			m_btn.Load(sResFile.c_str());
			m_btn.Invalidate();
		}
	}
}

bool CEBFuncButton::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		m_btn.MoveWindow(lpRect,bRepaint);
	}
	if (m_msg.GetSafeHwnd()!=NULL)
	{
		m_rectMsg.right = lpRect->right + 2;
		m_rectMsg.left = m_rectMsg.right - 16;
		m_rectMsg.top = lpRect->top - 2;
		m_rectMsg.bottom = m_rectMsg.top + 16;
		m_msg.MoveWindow(&m_rectMsg,bRepaint);
	}
	return m_btn.GetSafeHwnd()!=NULL?true:false;
}
void CEBFuncButton::ShowWindow(bool bShow)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		m_btn.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	}
	if (m_msg.GetSafeHwnd()!=NULL)
	{
		if (!bShow)
			m_msg.ShowWindow(SW_HIDE);
		else if (m_nUnreadMsgCount>0)
			m_msg.ShowWindow(SW_SHOW);
	}
}

void CEBFuncButton::AddUnreadMsg(bool bShowForce)
{
	m_nUnreadMsgCount++;
	if (m_nUnreadMsgCount>=10)
		m_msg.SetTextLeft(0);
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
		if (bShowForce)
			m_msg.ShowWindow(SW_SHOW);
	}
}

void CEBFuncButton::SetUnreadMsg(size_t nUnreadMsgCount, bool bShowForce)
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
			if (bShowForce)
				m_msg.ShowWindow(SW_SHOW);
		}
	}
}

int CEBFuncButton::ClearUnreadMsg(void)
{
	if (m_nUnreadMsgCount>0)
	{
		const int ret = m_nUnreadMsgCount;
		m_nUnreadMsgCount = 0;
		if (m_msg.GetSafeHwnd()!=NULL)
		{
			m_msg.SetTextLeft(2);
			m_msg.SetWindowText(_T(""));
			m_msg.ShowWindow(SW_HIDE);
			//if (m_rectMsg.Width()!=DEFAULT_MSG_WIDTH1)
			//{
			//	m_rectMsg.right = m_rectMsg.left+DEFAULT_MSG_WIDTH1;
			//	m_msg.MoveWindow(&m_rectMsg);
			//}
		}
		return ret;
	}
	return 0;
}

