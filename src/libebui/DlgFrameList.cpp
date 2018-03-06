// DlgFrameList.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgFrameList.h"
#include "DlgShrinkageBar.h"

#define TIMERID_CHECK_CLOSE_DIALOG		0x128
#define TIMERID_SHRINKAGE_WIN			0x129
#define TIMERID_CHECK_MOUSE_POS			0x130

#define DEFAULT_DLG_WIDTH	710+120
#define DEFAULT_DLG_HEIGHT	588

#define FRAME_BTN_ID_MIN		101
#define FRAME_BTN_ID_MAX		200
//#define FRAME_CLOSE_ID_MIN		301
//#define FRAME_CLOSE_ID_MAX		400
//#define FRAME_CLOSE_ID_OFFSET	100

const int const_frame_height = 36;
const int const_border_size = 2;
const int const_border_left = 135;

BOOL CFrameWndInfo::Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID)
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

		if (!m_btn.Create(sWindowText,WS_CHILD|WS_VISIBLE,rect,pParent,nID))
			return FALSE;
		m_btn.SetFont(pParent->GetFont());
		m_btn.SetToolTipText(lpszCaption);
		m_btn.SetTextHotMove(false);
		m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
		//m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgColor1(),theApp.GetBgColor2());
		m_btn.SetNorTextColor(theApp.GetTitleTextColor());
		m_btn.SetHotTextColor(theApp.GetTitleTextColor());

		if (m_pDialog.get()!=NULL)
		{
			const bool bGrayImage = m_pDialog->GetFromIsOffLineState();
			m_btn.SetDrawImage(false,bGrayImage,const_cast<Gdiplus::Image*>(m_pDialog->GetFromImage()),Gdiplus::Rect(4,4,28,28));
			m_btn.SetTextLeft(33);	// 33=4+28+1
		}
	}
	if (m_close.GetSafeHwnd()==NULL)
	{
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-3;
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (!m_close.Create(_T(""),WS_CHILD,m_rectClose,pParent,nID))
			return FALSE;
		m_close.SetToolTipText(_T("关闭"));
		m_close.SetDrawPanelRgn(false);
		//m_close.SetDrawClosePic(true,RGB(27,27,27),RGB(237,28,36),-1,-1,2);
	}
	if (m_msg.GetSafeHwnd()==NULL)
	{
		CRect rectMsg(rect);
		rectMsg.left = rectMsg.right - 20;
		if (!m_msg.Create(_T(""),WS_CHILD|WS_VISIBLE,rectMsg,pParent,nID))
			return FALSE;
		m_msg.SetTextLeft(2);
		m_msg.SetFont(pParent->GetFont());
		m_msg.SetNorTextColor(RGB(237,28,36));
		m_msg.SetHotTextColor(RGB(237,28,36));
	}
	return TRUE;
}
void CFrameWndInfo::LineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	if (GetGroupId()==0 && GetFromUserId()==nUserId && m_btn.GetSafeHwnd()!=NULL)
	{
		// 更新左边标签
		const bool bGrayImage = bLineState==EB_LINE_STATE_OFFLINE || bLineState==EB_LINE_STATE_UNKNOWN;
		m_btn.SetDrawImage(bGrayImage);
	}
	if (nGroupCode>0 && GetGroupId()==nGroupCode && m_pDialog.get()!=NULL)
	{
		// 更新群组成员
		m_pDialog->LineStateChange(nUserId, bLineState);
	}
}

// CDlgFrameList dialog

IMPLEMENT_DYNAMIC(CDlgFrameList, CEbDialogBase)

CDlgFrameList::CDlgFrameList(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgFrameList::IDD, pParent)
{
	m_nShrinkageWin = false;
	m_pDlgShrinkageBar = NULL;

	//m_nCurrentData = 0;
	for (int i=FRAME_BTN_ID_MIN;i<=FRAME_BTN_ID_MAX; i++)
		m_pIndexIdList.add(i);
}

CDlgFrameList::~CDlgFrameList()
{
}

void CDlgFrameList::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
}

BEGIN_MESSAGE_MAP(CDlgFrameList, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_WM_MOVING()
	ON_MESSAGE(EBWM_FRAME_WND_MIN, OnFrameWndMin)
	ON_MESSAGE(EBWM_FRAME_WND_MAX, OnFrameWndMax)
	ON_MESSAGE(EBWM_FRAME_WND_CLOSE, OnFrameWndClose)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND_RANGE(FRAME_BTN_ID_MIN, FRAME_BTN_ID_MAX, OnFrameBtnClicked)
	//ON_COMMAND_RANGE(FRAME_CLOSE_ID_MIN, FRAME_CLOSE_ID_MAX, OnFrameCloseClicked)
	//ON_COMMAND_RANGE(FRAME_BTN_ID_MIN+FRAME_CLOSE_ID_OFFSET, FRAME_BTN_ID_MAX+FRAME_CLOSE_ID_OFFSET, OnFrameCloseClicked)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &OnEnChangeEditSearch)
END_MESSAGE_MAP()


// CDlgFrameList message handlers

BOOL CDlgFrameList::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	this->SetSplitterBorder();
	//this->EnableToolTips();
	//this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索：用户、帐号、联系人；回车开始聊天"));

	//int nScreenWidth = 0;
	//int nScreenHeight = 0;
	//theApp.GetScreenSize(nScreenWidth, nScreenHeight);
	//CRect rect;
	//rect.left = (nScreenWidth-DEFAULT_DLG_WIDTH)/2;
	//rect.right = rect.left + DEFAULT_DLG_WIDTH;
	//rect.top = (nScreenHeight-DEFAULT_DLG_HEIGHT)/2;
	//rect.bottom = rect.top + DEFAULT_DLG_HEIGHT;
	//this->ScreenToClient(&rect);
	////this->GetParent()->ScreenToClient(&rect);
	//this->MoveWindow(&rect);
	this->MoveWindow(0, 0, DEFAULT_DLG_WIDTH, DEFAULT_DLG_HEIGHT);
	this->SetWindowText(_T("会话窗口"));

	// **暂时不支持搜索；
	m_editSearch.ShowWindow(SW_HIDE);
	//m_editSearch.SetPromptText(_T("搜索：用户、帐号、联系人；回车开始聊天"));
	//m_editSearch.SetRectangleColor(theApp.GetHotColor2(),RGB(255,255,255));
	SetCircle();

	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetTimer(TIMERID_CHECK_CLOSE_DIALOG,100,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFrameList::SetCtrlColor(void)
{
	m_editSearch.SetRectangleColor(theApp.GetHotColor2(),RGB(255,255,255));
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			pFrameWndInfo->SetCtrlColor();
		}
	}
	this->Invalidate();
}

void CDlgFrameList::AddWnd(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow)
{
	if (!bShow)
	{
		m_pHideList.add(pFrameWndInfo);
		return;
	}
	int nIndexId = 0;
	m_pIndexIdList.front(nIndexId);
	pFrameWndInfo->SetUserData(nIndexId);
	m_pList.add(pFrameWndInfo);
	if (pFrameWndInfo->GetDialog().get()!=NULL)
	{
		CRect rectLable;
		rectLable.left = 5;
		rectLable.top = 5 + (m_pList.size()-1)*const_frame_height;
		rectLable.right = const_border_left-3;
		rectLable.bottom = rectLable.top+const_frame_height;
		const tstring sFromName = pFrameWndInfo->GetDialog()->GetFromName();
		pFrameWndInfo->Create(sFromName.c_str(), rectLable, this, pFrameWndInfo->GetUserData());

		CRect rect;
		this->GetWindowRect(&rect);
		pFrameWndInfo->GetDialog()->MoveWindow(const_border_left,const_border_size,rect.Width()-const_border_left-const_border_size,rect.Height()-const_border_size*2);
		ShowWnd((unsigned int)nIndexId);
		pFrameWndInfo->GetDialog()->ScrollToEnd();
	}
	if (m_nShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_SHOW);
		if (!theApp.m_bTopMost)
			SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
void CDlgFrameList::RebuildBtnSize(void)
{
	int nIndex = 0;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		CRect rectLable;
		rectLable.left = 5;
		rectLable.top = 5 + nIndex*const_frame_height;
		rectLable.right = const_border_left-3;
		rectLable.bottom = rectLable.top+const_frame_height;
		pFrameWndInfo->MoveBtnWindow(&rectLable);
		nIndex++;
	}
}

bool CDlgFrameList::AddUnreadMsg(eb::bigint nCallId)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCallId()==nCallId)
			{
				//if (pFrameWndInfo->
				pFrameWndInfo->AddUnreadMsg();
				return true;
			}
		}
	}
	if (this->IsWindowVisible())
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
				pFrameWndInfo->AddUnreadMsg();
				this->AddWnd(pFrameWndInfo,true);
				return true;
			}
		}
	}
	return false;
}
void CDlgFrameList::DelWnd(eb::bigint nCallId)
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
		if (m_nShrinkageWin)
		{
			KillTimer(TIMERID_SHRINKAGE_WIN);
			m_nShrinkageWin = false;
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		}
		this->ShowWindow(SW_HIDE);
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
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
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pFrameWndInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
			pPrevFrameWndInfo->ShowHide(true);
	}
	RebuildBtnSize();
}
//void CDlgFrameList::DelWnd(unsigned int nWndIndex)
//{
//	{
//		BoostWriteLock wtLock(m_pHideList.mutex());
//		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pHideList.begin();
//		for (; pIter!=m_pHideList.end(); pIter++)
//		{
//			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
//			if (pFrameWndInfo->GetUserData()==nWndIndex)
//			{
//				m_pHideList.erase(pIter);
//				break;
//			}
//		}
//	}
//	CFrameWndInfo::pointer pPrevFrameWndInfo;
//	BoostWriteLock wtLock(m_pList.mutex());
//	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
//	for (; pIter!=m_pList.end(); pIter++)
//	{
//		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
//		if (pFrameWndInfo->GetUserData()==nWndIndex)
//		{
//			m_pIndexIdList.pushfront(pFrameWndInfo->GetUserData());
//			m_pList.erase(pIter);
//			wtLock.unlock();
//			if (m_pList.empty())
//			{
//				this->ShowWindow(SW_HIDE);
//				return;
//			}else if (pPrevFrameWndInfo.get()!=NULL)
//				pPrevFrameWndInfo->ShowHide(true);
//			else
//				ShowFirstWnd();
//			RebuildBtnSize();
//			break;
//		}
//		pPrevFrameWndInfo = pFrameWndInfo;
//	}
//}
void CDlgFrameList::DelWnd(const CWnd* pWnd)
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
		if (m_nShrinkageWin)
		{
			KillTimer(TIMERID_SHRINKAGE_WIN);
			m_nShrinkageWin = false;
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		}
		this->ShowWindow(SW_HIDE);
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
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
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pFrameWndInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
			pPrevFrameWndInfo->ShowHide(true);
	}
	RebuildBtnSize();

}
void CDlgFrameList::ShowWnd(eb::bigint nCallId)
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
			}else
			{
				pFrameWndInfo->ShowHide(false);
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
				AddWnd(pFrameWndInfo,true);
				break;
			}
		}
	}
}

void CDlgFrameList::ShowWnd(unsigned int nWndIndex)
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
				pFrameWndInfo->ClearUnreadMsg();
				pFrameWndInfo->ShowHide(true);
			}else
			{
				pFrameWndInfo->ShowHide(false);
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
				AddWnd(pFrameWndInfo,true);
				break;
			}
		}
	}
}
void CDlgFrameList::ShowFirstWnd(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->ShowHide(true);
		return;
	}
}

void CDlgFrameList::ShowWnd(const CWnd* pWnd)
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
				pFrameWndInfo->ClearUnreadMsg();
				pFrameWndInfo->ShowHide(true);
			}else
			{
				pFrameWndInfo->ShowHide(false);
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
				AddWnd(pFrameWndInfo,true);
				break;
			}
		}
	}
}
void CDlgFrameList::ClickFrame(unsigned int nWndIndex)
{
	CFrameWndInfo::pointer pCheckedFrameWndInfo;
	bool bClose = false;
	{
		CPoint pos;
		GetCursorPos(&pos);
		this->ScreenToClient(&pos);
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CFrameWndInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetUserData()==nWndIndex)
			{
				pFrameWndInfo->ClearUnreadMsg();
				if (pFrameWndInfo->IsClickClose(pos))
				{
					bClose = true;
					pFrameWndInfo->SendClose();
					return;
				}else
					pFrameWndInfo->ShowHide(true);
			}else
			{
				if (pFrameWndInfo->IsChecked())
					pCheckedFrameWndInfo = pFrameWndInfo;
				pFrameWndInfo->ShowHide(false);
			}
		}
	}
	if (bClose && pCheckedFrameWndInfo.get()!=NULL)
	{
		pCheckedFrameWndInfo->ShowHide(true);
	}
}

bool CDlgFrameList::ExistCallIdWnd(eb::bigint nCallId) const
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
//bool CDlgFrameList::SetNewCallInfo(eb::bigint nOldCallId, const CEBCCallInfo::pointer & pEbCallInfo)
//{
//	BoostWriteLock wtLock(m_pList.mutex());
//	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
//	for (; pIter!=m_pList.end(); pIter++)
//	{
//		const CFrameWndInfo::pointer pFrameWndInfo = *pIter;
//		if (pFrameWndInfo->GetCallId()==nOldCallId)
//		{
//			pFrameWndInfo->SetCallInfo(pEbCallInfo);
//			return true;
//		}
//	}
//	return false;
//}

CDlgDialog::pointer CDlgFrameList::GetCallIdDialog(eb::bigint nCallId, bool bRemove)
{
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
						if (m_nShrinkageWin)
						{
							KillTimer(TIMERID_SHRINKAGE_WIN);
							m_nShrinkageWin = false;
							m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
						}
						this->ShowWindow(SW_HIDE);
						return pFrameWndInfo->GetDialog();
					}else if (pPrevFrameWndInfo.get()!=NULL)
						pPrevFrameWndInfo->ShowHide(true);
					else
						ShowFirstWnd();
					RebuildBtnSize();
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
void CDlgFrameList::UserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE nLineState)
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

void CDlgFrameList::CheckMousePos(void)
{
	CPoint pos;
	GetCursorPos(&pos);
	this->ScreenToClient(&pos);
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->CheckMousePos(pos);
	}
}

#ifdef USES_EBCOM_TEST
void CDlgFrameList::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo)
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
void CDlgFrameList::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo)
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
#endif
void CDlgFrameList::OnRemoveGroup(eb::bigint nGroupId)
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
void CDlgFrameList::OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId)
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

void CDlgFrameList::OnExitRD(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->OnExitRD();
	}
}

void CDlgFrameList::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgFrameList::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgFrameList::OnDestroy()
{
	CEbDialogBase::OnDestroy();

	m_pList.clear();
	m_pHideList.clear();
	m_pCloseList.clear();

	if (m_pDlgShrinkageBar)
	{
		m_pDlgShrinkageBar->DestroyWindow();
		delete m_pDlgShrinkageBar;
		m_pDlgShrinkageBar = NULL;
	}
}

void CDlgFrameList::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	if (m_editSearch.GetSafeHwnd()!=NULL)
		m_editSearch.MoveWindow(15, 15, const_border_left-28, 22);

	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			pFrameWndInfo->GetDialog()->MoveWindow(const_border_left,const_border_size,cx-const_border_left-const_border_size,cy-const_border_size*2);
		}
	}

	if (m_nShrinkageWin)
	{
		this->GetWindowRect(&m_rectWin);
	}
}

void CDlgFrameList::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CEbDialogBase::OnClose();
}

void CDlgFrameList::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文
	}
	else
	{
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}

}

void CDlgFrameList::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false));	// 0.45
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	//Gdiplus::Graphics graphics(memDC.m_hDC);

	//Gdiplus::Graphics graphics(memDC.m_hDC);

	//const int const_head_size = 24;
	//int nIndex = 0;
	//BoostReadLock rdLock(m_pList.mutex());
	//CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	//for (; pIter!=m_pList.end(); pIter++)
	//{
	//	CFrameWndInfo::pointer pFrameWndInfo = *pIter;

	//	Image * pImage = NULL;
	//	pImage = theApp.m_imageDefaultMember->Clone();
	//	Gdiplus::Rect destRect(7, 7, const_head_size, const_head_size);
	//	graphics.DrawImage(pImage, destRect);
	//}

}

void CDlgFrameList::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);

	int nNeedHideWin = 0;
	if (x<=0 && y>0)
		nNeedHideWin = 1;		// left
	else if (x>0 && y<=0)
		nNeedHideWin = 2;		// top
	else
	{
		CRect rect;
		this->GetWindowRect(&rect);
		int nScreenWidth = 0;
		int nScreenHeight = 0;
		theApp.GetScreenSize(nScreenWidth, nScreenHeight);
		if (rect.right>=nScreenWidth && y>0)
			nNeedHideWin = 3;	// right
	}
	if (nNeedHideWin>0)
	{
		CRect rect;
		this->GetWindowRect(&rect);

		if (m_pDlgShrinkageBar==NULL)
		{
			AFX_SWITCH_INSTANCED();
			CWnd* pParent = GetDesktopWindow();
			m_pDlgShrinkageBar = new CDlgShrinkageBar(pParent);
			m_pDlgShrinkageBar->Create(CDlgShrinkageBar::IDD,pParent);
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
			m_pDlgShrinkageBar->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
		m_rectWin = rect;
		if (nNeedHideWin==1)
		{
			// left
			rect.left = 0;
			rect.right = 2;
		}else if (nNeedHideWin==2)
		{
			// top
			rect.top = 0;
			rect.bottom = 2;
		}else
		{
			// right
			rect.left = rect.right-2;
		}
		m_pDlgShrinkageBar->MoveWindow(&rect);
		if (!m_nShrinkageWin)
		{
			m_nShrinkageWin = true;
			SetTimer(TIMERID_SHRINKAGE_WIN,100,NULL);
			SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}else if (nNeedHideWin==0 && m_nShrinkageWin && x>0 && y>0)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_SHOW);
		if (!theApp.m_bTopMost)
			SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}

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

void CDlgFrameList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMERID_CHECK_CLOSE_DIALOG:
		{
			do
			{
				CFrameWndInfo::pointer pCloseInfo;
				m_pCloseList.front(pCloseInfo);
			}while (m_pCloseList.size()>2);
		}break;
	case TIMERID_CHECK_MOUSE_POS:
		{
			CheckMousePos();
		}break;
	case TIMERID_SHRINKAGE_WIN:
		{
			CPoint pos;
			GetCursorPos(&pos);
			CRect rectWin;
			m_pDlgShrinkageBar->GetWindowRect(&rectWin);
			if (pos.y>0 && !m_rectWin.PtInRect(pos))
			{
				// up
				if (m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_SHOW);
				this->ShowWindow(SW_HIDE);
			}else if (rectWin.PtInRect(pos))
			{
				// down
				if (!m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				this->ShowWindow(SW_SHOWNORMAL);
			}
		}break;
	default:
		break;
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgFrameList::OnMoving(UINT fwSide, LPRECT pRect)
{
	if (pRect!=NULL)
	{
		if (pRect->left<0 && pRect->top>0)
		{
			// 限制左移
			pRect->right -= pRect->left;
			pRect->left = 0;
		}else 
		{
			int nScreenWidth = 0;
			int nScreenHeight = 0;
			theApp.GetScreenSize(nScreenWidth, nScreenHeight);
			if (pRect->left>0 && pRect->right>nScreenWidth)
			{
				// 限制右移
				pRect->left -= (pRect->right-nScreenWidth);
				pRect->right = nScreenWidth;
			}
		}
	}
	CEbDialogBase::OnMoving(fwSide, pRect);

	// TODO: Add your message handler code here
}

LRESULT CDlgFrameList::OnFrameWndMin(WPARAM wParam, LPARAM lParam)
{
	return 1;	// ???
	const CWnd * pWnd = (CWnd*)wParam;
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return 0;
}
LRESULT CDlgFrameList::OnFrameWndMax(WPARAM wParam, LPARAM lParam)
{
	int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	bool bSetBtnMax = false;
	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() == m_nScreenWidth)
	{
		bSetBtnMax = true;
		MoveWindow(&theRestoreRect);
	}else
	{
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + m_nScreenWidth;
		rect.bottom = rect.top + m_nScreenHeight;
		MoveWindow(&rect);
	}

	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CFrameWndInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			pFrameWndInfo->GetDialog()->SetBtnMax(bSetBtnMax);
		}
	}
	return 0;
}

LRESULT CDlgFrameList::OnFrameWndClose(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	DelWnd(pWnd);
	return 0;
}

void CDlgFrameList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnFrameWndMax(0,0);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgFrameList::OnFrameBtnClicked(UINT nID)
{
	ClickFrame(nID);
}
//void CDlgFrameList::OnFrameCloseClicked(UINT nID)
//{
//	const unsigned int nCloseIndex = nID-FRAME_CLOSE_ID_OFFSET;
//	DelWnd(nCloseIndex);
//}
void CDlgFrameList::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static CString theSearchString;
	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	if (sSearchString==theSearchString)
		return;
	theSearchString = sSearchString;

	if (!sSearchString.IsEmpty())
	{
//		m_pContactItem.clear();
//		m_pMemberItem.clear();
//		m_treeSearch.DeleteAllItems();
//#ifdef USES_EBCOM_TEST
//		//IDispatch* pCallback;
//		//((CEBSearchEventsSink*)this)->QueryInterface(IID_IDispatch,(void**)&pCallback);
//		theEBClientCore->EB_SearchUserInfo((IDispatch*)(CEBSearchEventsSink*)this, (LPCTSTR)sSearchString, 1);
//#else
//		theEBAppClient.EB_SearchUserInfo((CEBSearchCallback*)this, sSearchString, 1);
//#endif
//
//		if (m_treeSearch.GetCount() > 0)
//		{
//			ShowSearchResult();
//		}
//	}
//	if (m_treeSearch.GetCount() == 0 || sSearchString.IsEmpty())
//	{
//		m_treeSearch.DeleteAllItems();
//		m_treeSearch.ShowWindow(SW_HIDE);
//		
//		if (theApp.IsEnterpriseuserUser())
//		{
//			m_btnMyEnterprise.ShowWindow(SW_SHOW);
//		}
//		m_btnMyDepartment.ShowWindow(SW_SHOW);
//		m_btnMyContacts.ShowWindow(SW_SHOW);
//		m_btnMySession.ShowWindow(SW_SHOW);
//		if (m_btnMyEnterprise.GetChecked())
//			m_pDlgMyEnterprise->ShowWindow(SW_SHOW);
//		else if (m_btnMyDepartment.GetChecked())
//			m_pDlgMyGroup->ShowWindow(SW_SHOW);
//		else if (m_btnMyContacts.GetChecked())
//			m_pDlgMyContacts->ShowWindow(SW_SHOW);
//		else if (m_btnMySession.GetChecked())
//			m_pDlgMySession->ShowWindow(SW_SHOW);
	}
}
