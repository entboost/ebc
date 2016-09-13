// DlgChatRight.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgChatRight.h"
#include "DlgUserInfo.h"
#include "PanelVideos.h"
#include "PanelRemoteDesktop.h"
#include "PanelFiles.h"
#include "DlgUserList.h"
#include "DlgMsgRecord.h"
#include "DlgResourceMgr.h"
#include "DlgAppWindow.h"

#define TIMERID_CHECK_MOUSE_POS			0x130
#define TIMERID_CHECK_ADJUST_WIDTH		0x131
#define RIGHC_TAB_ID_MIN			101
#define RIGHC_TAB_ID_ECARD			RIGHC_TAB_ID_MIN
#define RIGHC_TAB_ID_USERLIST		102
#define RIGHC_TAB_ID_VIDEO			103
#define RIGHC_TAB_ID_RD				104
#define RIGHC_TAB_ID_FILE			105
#define RIGHC_TAB_ID_CHAT_RECORD	106	// 聊天记录
#define RIGHC_TAB_ID_GROUP_SHARE	107	// 群共享
#define RIGHC_TAB_ID_APP_FIRST		110
#define RIGHC_TAB_ID_MAX			130

const int const_Tab_Width1	= 95;
const int const_Tab_Width2	= 70;	// 65
const int const_Tab_Height	= 25;

CRightTabInfo::CRightTabInfo(void)
: m_pAppWindow(NULL)

{
}

CRightTabInfo::~CRightTabInfo(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.DestroyWindow();
	if (m_close.GetSafeHwnd()!=NULL)
		m_close.DestroyWindow();
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->DestroyWindow();
		delete m_pAppWindow;
		m_pAppWindow = NULL;
	}
}

bool CRightTabInfo::Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID,bool bShowClose)
{
	m_rectBtn = rect;
	if (m_btn.GetSafeHwnd()==NULL)
	{
		CString sWindowText;
		if (nID<RIGHC_TAB_ID_APP_FIRST)
		{
			sWindowText = lpszCaption;
		}else
		{
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
		}
		if (!m_btn.Create(sWindowText,WS_CHILD|WS_VISIBLE,rect,pParent,nID))
			return false;
		m_btn.SetFont(pParent->GetFont());
		//m_btn.SetToolTipText(lpszCaption);
		//m_btn.SetTextHotMove(false);
		m_btn.SetDrawLine(4,1,0,theDefaultFlatLineColor,theDefaultFlatLineColor,theDefaultFlatLineColor,theDefaultFlatLineColor);
		if (bShowClose)
			m_btn.SetTextLeft(-3);
		m_btn.SetDrawPanelRgn(false);
		m_btn.SetDrawPanel(true,theDefaultFlatBgColor);
		m_btn.SetNorTextColor(RGB(128,128,128));
	}
	if (bShowClose && m_close.GetSafeHwnd()==NULL)
	{
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-1;
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2+1;
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (!m_close.Create(_T(""),WS_CHILD,m_rectClose,pParent,nID))
			return false;
		m_close.SetToolTipText(_T("关闭"));
		m_close.SetDrawPanelRgn(false);
		m_close.SetDrawPanel(true,theDefaultFlatBgColor);
	}
	return true;
}
void CRightTabInfo::SetTabText(LPCTSTR lpszCaption)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		CString sWindowText;
		if (m_btn.GetDlgCtrlID()<RIGHC_TAB_ID_APP_FIRST)
		{
			sWindowText = lpszCaption;
		}else
		{
			const size_t nLen = strlen(lpszCaption);
			for (size_t i=0;i<nLen;i++)
			{
				sWindowText.AppendChar(lpszCaption[i]);
				if ((i+1)<nLen && lpszCaption[i]<0)	// 中文，需要取二个
				{
					sWindowText.AppendChar(lpszCaption[++i]);
				}
				if (i>=8 && (i+1)<nLen)
				{
					//sWindowText.Append(_T("."));
					break;
				}
			}
		}
		m_btn.SetWindowText(sWindowText);
	}
}

bool CRightTabInfo::IsCheckedRefresh(void)
{
	if (m_btn.GetChecked())
	{
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->SetWebFocus(true);
		}
		return true;
	}
	return false;
}
void CRightTabInfo::OnMove(void)
{
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->OnMove();
	}
}

void CRightTabInfo::CheckMousePos(POINT point)
{
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
			m_close.SetDrawClosePic(true,RGB(160,160,160),-1,-1,-1,2);	// theDefaultFlatBlackTextColor
		}else
		{
			m_close.SetDrawClosePic(true,theDefaultFlatLineColor,-1,-1,-1,2);
		}
		m_close.ShowWindow(SW_SHOW);
		m_close.Invalidate();
	}else if (m_close.IsWindowVisible())
	{
		m_close.ShowWindow(SW_HIDE);
	}
}
void CRightTabInfo::ShowHide(bool bShowAndChecked)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		if (m_btn.GetChecked()!=bShowAndChecked)
		{
			m_btn.SetChecked(bShowAndChecked);
			m_btn.Invalidate();
		}
	}
	if (m_pAppWindow!=NULL)
	{
		if (bShowAndChecked && !m_pAppWindow->IsWindowVisible())
		{
			m_pAppWindow->ShowWindow(SW_SHOW);
			m_pAppWindow->SetWebFocus(true);
		}else if (!bShowAndChecked && m_pAppWindow->IsWindowVisible())
			m_pAppWindow->ShowWindow(SW_HIDE);
	}
}
void CRightTabInfo::MoveBtnWindow(LPCRECT rect)
{
	m_rectBtn = rect;
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.MoveWindow(m_rectBtn);
	const int const_size = 12;
	m_rectClose.right = m_rectBtn.right-1;
	m_rectClose.left = m_rectClose.right - const_size;
	m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2+1;
	m_rectClose.bottom = m_rectClose.top+const_size;
	if (m_close.GetSafeHwnd()!=NULL)
	{
		m_close.MoveWindow(m_rectClose);
	}
}
void CRightTabInfo::SetSubscribeFuncInfo(const RECT& rect, const EB_SubscribeFuncInfo& pFuncInfo, const std::string& m_sAppUrl, const std::string& sPostData, bool bSaveBrowseTitle)
{
	if (m_pAppWindow==NULL)
	{
		CWnd * pParent = m_btn.GetParent();
		m_pAppWindow = new CDlgAppWindow(pParent);
		m_pAppWindow->SetSaveBrowseTitle(bSaveBrowseTitle);
		m_pAppWindow->m_pFuncInfo = pFuncInfo;
		m_pAppWindow->m_bOpenNewClose = false;
		m_pAppWindow->Navigate(m_sAppUrl, sPostData);
		if (!m_pAppWindow->Create(CDlgAppWindow::IDD,pParent))
		{
			delete m_pAppWindow;
			m_pAppWindow = NULL;
			return;
		}
		//pParent->ClientToScreen(&rectAppWindow);
		m_pAppWindow->MoveWindow(&rect);
		//m_pAppWindow->Navigate(m_sAppUrl);
		m_pAppWindow->SetUserData(GetCtrlID());
	}
	//m_pAppWindow->ShowWindow(SW_SHOW);
}
eb::bigint CRightTabInfo::GetSubscribeId(void) const {return m_pAppWindow==NULL?0:m_pAppWindow->m_pFuncInfo.m_nSubscribeId;}
bool CRightTabInfo::IsAppWindow(HWND hWnd) const
{
	return (m_pAppWindow!=NULL&&m_pAppWindow->GetSafeHwnd()==hWnd)?true:false;
}

void CRightTabInfo::MoveAppWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->MoveWindow(x,y,nWidth,nHeight,bRepaint);
}
bool CRightTabInfo::SendClose(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->PostMessage(WM_CLOSE, 0, 0);
	return m_pAppWindow==NULL?false:true;
}
int CRightTabInfo::GetAppWidth(void) const
{
	return (m_pAppWindow==NULL)?-1:m_pAppWindow->m_pFuncInfo.m_nWindowWidth;
}

bool CRightTabInfo::ChangeTabText(const CWnd* pWnd, LPCTSTR lpszCaption)
{
	if (m_pAppWindow!=NULL && m_pAppWindow==pWnd)
	{
		if (GetSubscribeId()==0)	// ==0 表示普通网站地址，可以修改，订购应用不可以修改
		{
			SetTabText(lpszCaption);
			m_btn.SetToolTipText(lpszCaption);
		}
		return true;
	}
	return false;
}

// CDlgChatRight dialog

IMPLEMENT_DYNAMIC(CDlgChatRight, CEbDialogBase)

CDlgChatRight::CDlgChatRight(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgChatRight::IDD, pParent)
	, m_pDlgUserList(NULL)
	, m_pDlgUserInfo(NULL)
	, m_pPanVideos(NULL)
	, m_pPanRemoteDesktop(NULL)
	, m_pPanFiles(NULL)
	, m_pMsgRecord(NULL)
	, m_pResourceMgr(NULL)
	//, m_hCurrentSelect(NULL)

{
	for (UINT nID=RIGHC_TAB_ID_APP_FIRST;nID<=RIGHC_TAB_ID_MAX;nID++)
		m_pAppCtrlIDList.add(nID);
}

CDlgChatRight::~CDlgChatRight()
{
}

void CDlgChatRight::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_USERLIST, m_labelUserList);
	//DDX_Control(pDX, IDC_BUTTON_ECARD, m_labelECard);
	//DDX_Control(pDX, IDC_BUTTON_VIDEOS, m_labelVideos);
	//DDX_Control(pDX, IDC_BUTTON_RD, m_labelRemoteDesktop);
	//DDX_Control(pDX, IDC_BUTTON_FILES, m_labelFiles);
}


BEGIN_MESSAGE_MAP(CDlgChatRight, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDC_BUTTON_ECARD, &CDlgChatRight::OnBnClickedButtonEcard)
	//ON_BN_CLICKED(IDC_BUTTON_VIDEOS, &CDlgChatRight::OnBnClickedButtonVideos)
	//ON_BN_CLICKED(IDC_BUTTON_USERLIST, &CDlgChatRight::OnBnClickedButtonUserlist)
	//ON_BN_CLICKED(IDC_BUTTON_FILES, &CDlgChatRight::OnBnClickedButtonFiles)
	//ON_BN_CLICKED(IDC_BUTTON_RD, &CDlgChatRight::OnBnClickedButtonRd)
	ON_COMMAND_RANGE(RIGHC_TAB_ID_MIN, RIGHC_TAB_ID_MAX, OnRightTabClicked)
	//ON_MESSAGE(EB_COMMAND_RAME_WND_CLOSE, OnMessageWndClose)
	ON_MESSAGE(EB_MSG_VIEW_MSG_RECORD, OnMessageMsgRecord)
	ON_MESSAGE(EB_COMMAND_VIEW_GROUP_SHARE, OnMessageGroupShare)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MAX, OnFrameWndMax)
	ON_MESSAGE(EB_COMMAND_RAME_WND_CLOSE, OnFrameWndClose)
	ON_MESSAGE(EB_COMMAND_RAME_WND_TITLE, OnFrameWndTitle)
	ON_MESSAGE(EB_COMMAND_RAME_WND_OPEN, OnFrameWndOpen)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
	ON_MESSAGE(EB_COMMAND_CHANGE_APP_URL, OnMsgChangeAppUrl)
END_MESSAGE_MAP()

void CDlgChatRight::SetCtrlColor(bool bInvalidate)
{
	//m_labelUserList.SetDrawPanel(true,theApp.GetBgColor1());
	//m_labelECard.SetDrawPanel(true,theApp.GetBgColor1());
	//m_labelVideos.SetDrawPanel(true,theApp.GetBgColor1());
	//m_labelRemoteDesktop.SetDrawPanel(true,theApp.GetBgColor1());
	//m_labelFiles.SetDrawPanel(true,theApp.GetBgColor1());

	if (m_pDlgUserList!=NULL && m_pDlgUserList->GetSafeHwnd()!=NULL)
		m_pDlgUserList->SetCtrlColor(bInvalidate);
	if (m_pPanRemoteDesktop!=NULL && m_pPanRemoteDesktop->GetSafeHwnd()!=NULL)
		m_pPanRemoteDesktop->SetCtrlColor(bInvalidate);
	if (m_pPanVideos!=NULL && m_pPanVideos->GetSafeHwnd()!=NULL)
		m_pPanVideos->SetCtrlColor(bInvalidate);
	if (bInvalidate)
		this->Invalidate();
}

// CDlgChatRight message handlers

BOOL CDlgChatRight::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//this->SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetMouseMove(FALSE);
	this->EnableToolTips(TRUE);	// *没用

	//m_labelUserList.SetAutoSize(false);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
		CRect rectTab;
		rectTab.left = 1;
		rectTab.right = rectTab.left + const_Tab_Width2;
		rectTab.top = 0;
		rectTab.bottom = rectTab.top + const_Tab_Height-1;
		pTabInfo->Create(_T("电子名片"),rectTab,this,RIGHC_TAB_ID_ECARD,false);
		m_pTabList.add(pTabInfo);
	}else
	{
		const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(m_pCallInfo.m_sGroupCode);
		const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(m_pCallInfo.m_sGroupCode);
		CString sWindowText;
		if (nOnlineSize>=0)
			sWindowText.Format(_T("群成员(%d/%d)"),nOnlineSize,nMemberSize);
		else
			sWindowText.Format(_T("群成员(%d)"),nMemberSize);
		//m_labelUserList.SetWindowText(sWindowText);
		//m_labelUserList.SetDrawPanelRgn(false);
		//m_labelUserList.SetNorTextColor(RGB(128,128,128));

		CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
		CRect rectTab;
		rectTab.left = 1;
		rectTab.right = rectTab.left + const_Tab_Width1;
		rectTab.top = 0;
		rectTab.bottom = rectTab.top + const_Tab_Height-1;
		pTabInfo->Create(sWindowText,rectTab,this,RIGHC_TAB_ID_USERLIST,false);
		m_pTabList.add(pTabInfo);
	}

	//m_labelECard.SetAutoSize(false);
	//m_labelECard.SetWindowText(_T("电子名片"));
	////m_labelECard.Load(IDB_PNG_TAB_75X30);
	//m_labelECard.SetDrawPanelRgn(false);
	//m_labelECard.SetNorTextColor(RGB(128,128,128));
	//m_labelVideos.SetAutoSize(false);
	//m_labelVideos.SetWindowText(_T("视频面板"));
	//m_labelVideos.ShowWindow(theApp.GetDisableVideo()?SW_HIDE:SW_SHOW);
	////m_labelVideos.Load(IDB_PNG_TAB_75X30);
	//m_labelVideos.SetDrawPanelRgn(false);
	//m_labelVideos.SetNorTextColor(RGB(128,128,128));
	//m_labelRemoteDesktop.SetAutoSize(false);
	//m_labelRemoteDesktop.SetWindowText(_T("远程桌面"));
	//m_labelRemoteDesktop.SetDrawPanelRgn(false);
	//m_labelRemoteDesktop.ShowWindow(theApp.GetDisableRD()?SW_HIDE:SW_SHOW);
	//m_labelRemoteDesktop.SetNorTextColor(RGB(128,128,128));
	////m_labelRemoteDesktop.ShowWindow(SW_HIDE);
	//m_labelFiles.SetAutoSize(false);
	//m_labelFiles.SetWindowText(_T("文件传输"));
	////m_labelFiles.Load(IDB_PNG_TAB_75X30);
	//m_labelFiles.SetDrawPanelRgn(false);
	//m_labelFiles.ShowWindow(SW_HIDE);
	//m_labelFiles.SetNorTextColor(RGB(128,128,128));

	SetCtrlColor();
	//m_labelUserList.SetDrawPanel(true,theDefaultFlatBgColor);
	//m_labelECard.SetDrawPanel(true,theDefaultFlatBgColor);
	//m_labelVideos.SetDrawPanel(true,theDefaultFlatBgColor);
	//m_labelRemoteDesktop.SetDrawPanel(true,theDefaultFlatBgColor);
	//m_labelFiles.SetDrawPanel(true,theDefaultFlatBgColor);

	m_pDlgUserList = new CDlgUserList(this);
	m_pDlgUserList->SetCircle(false);
	m_pDlgUserList->m_pCallInfo = m_pCallInfo;
	m_pDlgUserList->Create(CDlgUserList::IDD, this);
	m_pDlgUserList->ShowWindow(SW_HIDE);

	m_pDlgUserInfo = new CDlgUserInfo(this);
	m_pDlgUserInfo->SetCircle(false);
	m_pDlgUserInfo->m_pCallInfo = this->m_pCallInfo;
	m_pDlgUserInfo->m_pFromAccountInfo = this->m_pFromAccountInfo;
	m_pDlgUserInfo->Create(CDlgUserInfo::IDD, this);
	m_pDlgUserInfo->ShowWindow(SW_HIDE);

	ShowTabID(0);
	//if (m_pCallInfo.m_sGroupCode==0)
	//{
	//	m_labelUserList.ShowWindow(SW_HIDE);
	//	m_labelUserList.SetChecked(false);
	//	m_labelECard.ShowWindow(SW_SHOW);
	//	m_labelECard.SetChecked(true);
	//	OnBnClickedButtonEcard();
	//}else
	//{
	//	m_labelUserList.ShowWindow(SW_SHOW);
	//	m_labelUserList.SetChecked(true);
	//	m_labelECard.ShowWindow(SW_HIDE);
	//	m_labelECard.SetChecked(false);
	//	OnBnClickedButtonUserlist();
	//}
	//m_labelECard.ShowWindow(SW_HIDE);
	//m_labelUserList.ShowWindow(SW_HIDE);
	//m_labelVideos.ShowWindow(SW_HIDE);
	//m_labelRemoteDesktop.ShowWindow(SW_HIDE);
	//m_labelFiles.ShowWindow(SW_HIDE);

	m_pPanFiles = new CPanelFiles(this);
	m_pPanFiles->SetCircle(false);
	//m_pPanFiles->m_pChatRoom = m_pChatRoom;
	m_pPanFiles->Create(CPanelFiles::IDD, this);
	m_pPanFiles->ShowWindow(SW_HIDE);
	m_pPanVideos = new CPanelVideos(this);
	m_pPanVideos->SetCircle(false);
	m_pPanVideos->m_pCallInfo = m_pCallInfo;
	m_pPanVideos->Create(CPanelVideos::IDD, this);
	m_pPanVideos->ShowWindow(SW_HIDE);
	m_pPanRemoteDesktop = new CPanelRemoteDesktop(this);
	m_pPanRemoteDesktop->SetCircle(false);
	m_pPanRemoteDesktop->m_pCallInfo = m_pCallInfo;
	m_pPanRemoteDesktop->Create(CPanelRemoteDesktop::IDD, this);
	m_pPanRemoteDesktop->ShowWindow(SW_HIDE);

	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChatRight::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	int x=1;
	//if (m_pCallInfo.m_sGroupCode==0)
	//{
	//	m_labelECard.MovePoint(x, 0,const_Tab_Width2,const_Tab_Height-1);
	//	x += const_Tab_Width2;
	//}else
	//{
	//	m_labelUserList.MovePoint(x,0,const_Tab_Width1,const_Tab_Height-1);
	//	x += const_Tab_Width1;
	//}
	//if (!theApp.GetDisableVideo())
	//{
	//	m_labelVideos.MovePoint(x, 0,const_Tab_Width2,const_Tab_Height-1);
	//	x += const_Tab_Width2;
	//}
	//if (!theApp.GetDisableRD())
	//{
	//	m_labelRemoteDesktop.MovePoint(x, 0,const_Tab_Width2,const_Tab_Height-1);
	//	x += const_Tab_Width2;
	//}
	//m_labelFiles.MovePoint(x,0,const_Tab_Width2,const_Tab_Height-1);
	x = 1;
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	{
		m_pDlgUserList->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pPanVideos != NULL && m_pPanVideos->GetSafeHwnd() != NULL)
	{
		m_pPanVideos->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pPanRemoteDesktop != NULL && m_pPanRemoteDesktop->GetSafeHwnd() != NULL)
	{
		m_pPanRemoteDesktop->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pMsgRecord != NULL && m_pMsgRecord->GetSafeHwnd() != NULL)
	{
		m_pMsgRecord->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}
	if (m_pResourceMgr != NULL && m_pResourceMgr->GetSafeHwnd() != NULL)
	{
		m_pResourceMgr->MoveWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
	}

	{
		AUTO_CONST_RLOCK(m_pTabList);
		CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			CRightTabInfo::pointer pFrameWndInfo = *pIter;
			pFrameWndInfo->MoveAppWindow(x, const_Tab_Height, cx-x, cy-const_Tab_Height);
		}
	}

}

void CDlgChatRight::OnDestroy()
{
	CEbDialogBase::OnDestroy();
	//m_pChatRoom.reset();
	//m_pCallInfo.reset();
	//m_pFromAccountInfo.reset();
	if (m_pMsgRecord!=NULL)
	{
		m_pMsgRecord->DestroyWindow();
		m_pMsgRecord = NULL;
	}
	if (m_pResourceMgr!=NULL)
	{
		m_pResourceMgr->DestroyWindow();
		m_pResourceMgr = NULL;
	}
	if (m_pPanFiles)
	{
		m_pPanFiles->DestroyWindow();
		delete m_pPanFiles;
		m_pPanFiles = NULL;
	}
	if (m_pPanVideos)
	{
		m_pPanVideos->DestroyWindow();
		delete m_pPanVideos;
		m_pPanVideos = NULL;
	}
	if (m_pPanRemoteDesktop)
	{
		m_pPanRemoteDesktop->DestroyWindow();
		delete m_pPanRemoteDesktop;
		m_pPanRemoteDesktop = NULL;
	}
	if (m_pDlgUserInfo)
	{
		m_pDlgUserInfo->DestroyWindow();
		delete m_pDlgUserInfo;
		m_pDlgUserInfo = NULL;
	}
	if (m_pDlgUserList)
	{
		m_pDlgUserList->DestroyWindow();
		delete m_pDlgUserList;
		m_pDlgUserList = NULL;
	}
	m_pTabList.clear();
}

void CDlgChatRight::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgChatRight::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}
void CDlgChatRight::OnExitUser(eb::bigint nUserId, bool bExitDep)
{
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	{
		m_pDlgUserList->OnExitUser(nUserId,bExitDep);
		//int nUserSize = m_pChatRoom->GetUserSize();
		//if (nUserSize < 2)
		//	m_pDlgUserList->ShowWindow(SW_HIDE);
	}
}
void CDlgChatRight::OnEnterUser(eb::bigint nUserId, const char* sFromInfo, bool bSort)
{
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	{
		m_pDlgUserList->OnEnterUser(nUserId,bSort);
		//int nUserSize = m_pChatRoom->GetUserSize();
		//if (nUserSize > 2)
		//	m_pDlgUserList->ShowWindow(SW_SHOW);
	}
}

void CDlgChatRight::RefreshAppWnd(void)
{
	BoostReadLock rdLock(m_pTabList.mutex());
	CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsCheckedRefresh())
		{
			return;
		}
	}

}
void CDlgChatRight::LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	if (m_pDlgUserList!=NULL)
		m_pDlgUserList->LineStateChange(nUserId, bLineState);
	if (m_pCallInfo.m_sGroupCode>0)
	{
		const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(m_pCallInfo.m_sGroupCode);
		const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(m_pCallInfo.m_sGroupCode);
		CString sWindowText;
		if (nOnlineSize>=0)
			sWindowText.Format(_T("群成员(%d/%d)"),nOnlineSize,nMemberSize);
		else
			sWindowText.Format(_T("群成员(%d)"),nMemberSize);
		ChangeTabText(RIGHC_TAB_ID_USERLIST,sWindowText);
		//m_labelUserList.SetWindowText(sWindowText);
	}
}

void CDlgChatRight::ExitRD(void)
{
	if (m_pPanRemoteDesktop!=NULL)
		m_pPanRemoteDesktop->ExitRD();
}
void CDlgChatRight::RefreshWeb(void)
{
	if (m_pMsgRecord!=NULL && m_pMsgRecord->GetSafeHwnd()!=NULL && m_pMsgRecord->IsWindowVisible())
	{
		m_pMsgRecord->RedrawCefBrowser();
	}else
	{
		AUTO_CONST_RLOCK(m_pTabList);
		CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			CRightTabInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCtrlID()>=RIGHC_TAB_ID_APP_FIRST)
			{
				if (pFrameWndInfo->IsCheckedRefresh())
				{
					break;
				}
			}
		}
	}
}

void CDlgChatRight::OnMove(void)
{
	if (m_pPanVideos!=NULL)
	{
		m_pPanVideos->OnMove();
	}
	if (m_pMsgRecord!=NULL && m_pMsgRecord->GetSafeHwnd()!=NULL && m_pMsgRecord->IsWindowVisible())
	{
		m_pMsgRecord->OnMove();
	}

	{
		AUTO_CONST_RLOCK(m_pTabList);
		CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			CRightTabInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCtrlID()>=RIGHC_TAB_ID_APP_FIRST)
			{
				if (pFrameWndInfo->IsChecked())
				{
					pFrameWndInfo->OnMove();
					break;
				}
			}
		}
	}

	//if (m_pPanRemoteDesktop!=NULL)
	//	m_pPanRemoteDesktop->OnMove();
}

void CDlgChatRight::GetProcessing(bool& pVideoProcessing, bool& pFileProcessing, bool& pDesktopProcessing) const
{
	if (m_pPanVideos!=NULL)
	{
		pVideoProcessing = m_pPanVideos->GetVideoCount()>0;
	}else
	{
		pVideoProcessing = false;
	}
	if (m_pPanFiles!=NULL)
	{
		pFileProcessing = !m_pPanFiles->IsEmpty();
	}else
	{
		pFileProcessing = false;
	}
	 if (m_pPanRemoteDesktop!=NULL)
		 pDesktopProcessing = m_pPanRemoteDesktop->GetInDesktop();
	 else
		 pDesktopProcessing = false;
}
int CDlgChatRight::GetNeedWidth(void) const
{
	int nTabWidth = 0;
	int nResult = 220;
	AUTO_CONST_RLOCK(m_pTabList);
	CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		switch (pFrameWndInfo->GetCtrlID())
		{
		case RIGHC_TAB_ID_USERLIST:
			nTabWidth += const_Tab_Width1;
			continue;
			break;
		case RIGHC_TAB_ID_ECARD:
		case RIGHC_TAB_ID_RD:
		case RIGHC_TAB_ID_FILE:
			break;
		case RIGHC_TAB_ID_VIDEO:
			nResult = max(nResult,((m_pPanVideos->GetVideoCount()+2)/3)*220);
			break;
		case RIGHC_TAB_ID_CHAT_RECORD:
		case RIGHC_TAB_ID_GROUP_SHARE:
			nResult = max(nResult,380);
			break;
		default:
			{
				const int nAppWidth = min(380,pFrameWndInfo->GetAppWidth());
				if (nAppWidth==0)
					nResult = max(nResult,280);
				else if (nAppWidth>0)
					nResult = max(nResult,nAppWidth);
			}break;
		}
		nTabWidth += const_Tab_Width2;
	}
	return max(nResult,nTabWidth);
}

void CDlgChatRight::ExitChat(bool bHangup)
{
	if (m_pPanVideos!=NULL)
	{
		m_pPanVideos->ExitChat(bHangup);
	}
	if (m_pPanFiles!=NULL)
	{
		m_pPanFiles->ExitChat(bHangup);
	}
	if (m_pPanRemoteDesktop!=NULL)
		m_pPanRemoteDesktop->ExitChat(bHangup);
}

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo)
{
	if (m_pDlgUserList)
	{
		m_pDlgUserList->OnUserEmpInfo(pMemberInfo);
	}
}
#else
void CDlgChatRight::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort)
{
	if (m_pDlgUserList)
	{
		m_pDlgUserList->OnUserEmpInfo(pMemberInfo,bSort);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::RDRequestResponse(EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequestResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequestResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VAckResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDAckResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoRequest(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDRequest(const EB_RemoteDesktopInfo* pVideoInfo)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequest(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDAccept(const EB_RemoteDesktopInfo* pVideoInfo)
{
	this->OpenRdPan();
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDAccept(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	CloseTabID(RIGHC_TAB_ID_RD);
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoCancel(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDCancel(const EB_RemoteDesktopInfo* pVideoInfo)
{
	CloseTabID(RIGHC_TAB_ID_RD);
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDCancel(pVideoInfo);
}
#endif
#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	CloseTabID(RIGHC_TAB_ID_RD);
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoEnd(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDEnd(const EB_RemoteDesktopInfo* pVideoInfo)
{
	CloseTabID(RIGHC_TAB_ID_RD);
	//OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDEnd(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VRequestResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VRequestResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VAckResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VAckResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoRequest(pVideoInfo,pUserVideoInfo);
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,500,NULL);
}
#else
void CDlgChatRight::VideoRequest(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoRequest(pVideoInfo,pUserVideoInfo);
	
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,500,NULL);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::VideoAccept(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	this->OpenVideoPan();
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
		CloseTabID(RIGHC_TAB_ID_VIDEO);
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoCancel(pVideoInfo,pUserVideoInfo);
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}
#else
void CDlgChatRight::VideoCancel(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
		CloseTabID(RIGHC_TAB_ID_VIDEO);
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoCancel(pVideoInfo,pUserVideoInfo);
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}
#endif
#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
		CloseTabID(RIGHC_TAB_ID_VIDEO);
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoEnd(pVideoInfo,pUserVideoInfo);
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}
#else
void CDlgChatRight::VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
		CloseTabID(RIGHC_TAB_ID_VIDEO);
	//OnBnClickedButtonVideos();
	m_pPanVideos->VideoEnd(pVideoInfo,pUserVideoInfo);
	if (m_pCallInfo.m_sGroupCode>0)
		SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnSendingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	this->OpenFilePan();
	//OnBnClickedButtonFiles();
	m_pPanFiles->OnSendingFile(pCrFileInfo);
}
#else
void CDlgChatRight::OnSendingFile(const CCrFileInfo * pCrFileInfo)
{
	this->OpenFilePan();
	//OnBnClickedButtonFiles();
	m_pPanFiles->OnSendingFile(pCrFileInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	this->OpenFilePan();
	//OnBnClickedButtonFiles();
	m_pPanFiles->OnReceivingFile(pCrFileInfo);
}
#else
void CDlgChatRight::OnReceivingFile(const CCrFileInfo * pCrFileInfo)
{
	this->OpenFilePan();
	//OnBnClickedButtonFiles();
	m_pPanFiles->OnReceivingFile(pCrFileInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo)
{
	DeleteDlgTranFile(pCrFileInfo->MsgId);
}
#else
void CDlgChatRight::OnReceivedFile(const CCrFileInfo * pCrFileInfo)
{
	DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
}
#endif

void CDlgChatRight::DeleteDlgTranFile(eb::bigint nMsgId)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->DeleteDlgTranFile(nMsgId);
		if (m_pPanFiles->IsEmpty())
		{
			CloseTabID(RIGHC_TAB_ID_FILE);
			//m_labelFiles.SetCheck(false);
			//m_labelFiles.ShowWindow(SW_HIDE);
			//if (this->m_pCallInfo.m_sGroupCode==0)
			//	OnBnClickedButtonEcard();
			//else
			//	OnBnClickedButtonUserlist();
		}
	}
}

#ifdef USES_EBCOM_TEST
void CDlgChatRight::SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->SetFilePercent(pChatRoomFilePercent);
	}
}
#else
void CDlgChatRight::SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->SetFilePercent(pChatRoomFilePercent);
	}
}
#endif
void CDlgChatRight::OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId)
{
	if (m_pResourceMgr!=NULL)
		m_pResourceMgr->OnResourceMove(pResourceInfo,nOldParentResId);
}
void CDlgChatRight::OnResourceInfo(const EB_ResourceInfo& pResourceInfo)
{
	if (m_pResourceMgr!=NULL)
		m_pResourceMgr->OnResourceInfo(pResourceInfo);
}
void CDlgChatRight::OnResourceDelete(const EB_ResourceInfo& pResourceInfo)
{
	if (m_pResourceMgr!=NULL)
		m_pResourceMgr->OnResourceDelete(pResourceInfo);
}

void CDlgChatRight::OpenVideoPan(void)
{
	if (theApp.GetDisableVideo()) return;
	if (!ShowTabID(RIGHC_TAB_ID_VIDEO))
	{
		CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
		CRect rectTab;
		rectTab.left = 1+GetTabSizeWidth();
		rectTab.right = rectTab.left + const_Tab_Width2;
		rectTab.top = 0;
		rectTab.bottom = rectTab.top + const_Tab_Height-1;
		pTabInfo->Create(_T("视频面板"),rectTab,this,RIGHC_TAB_ID_VIDEO);
		m_pTabList.add(pTabInfo);
		ShowTabID(RIGHC_TAB_ID_VIDEO);
		CheckAndSendAdjustWidth();
	}
	//if (m_pPanVideos==NULL)
	//{
	//	m_pPanVideos = new CPanelVideos(this);
	//	m_pPanVideos->SetCircle(false);
	//	m_pPanVideos->m_pCallInfo = m_pCallInfo;
	//	m_pPanVideos->Create(CPanelVideos::IDD, this);
	//}

	if (!m_pPanVideos->IsWindowVisible())
	{
		CRect rect;
		if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
		{
			m_pDlgUserInfo->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
		}
		m_pPanVideos->MoveWindow(&rect);
		m_pPanVideos->ShowWindow(SW_SHOW);
	}
}
void CDlgChatRight::OpenRdPan(void)
{
	if (theApp.GetDisableRD()) return;
	if (!ShowTabID(RIGHC_TAB_ID_RD))
	{
		CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
		CRect rectTab;
		rectTab.left = 1+GetTabSizeWidth();
		rectTab.right = rectTab.left + const_Tab_Width2;
		rectTab.top = 0;
		rectTab.bottom = rectTab.top + const_Tab_Height-1;
		pTabInfo->Create(_T("远程桌面"),rectTab,this,RIGHC_TAB_ID_RD);
		m_pTabList.add(pTabInfo);
		ShowTabID(RIGHC_TAB_ID_RD);
		CheckAndSendAdjustWidth();
	}
	//if (m_pPanRemoteDesktop==NULL)
	//{
	//	m_pPanRemoteDesktop = new CPanelRemoteDesktop(this);
	//	m_pPanRemoteDesktop->SetCircle(false);
	//	m_pPanRemoteDesktop->m_pCallInfo = m_pCallInfo;
	//	m_pPanRemoteDesktop->Create(CPanelRemoteDesktop::IDD, this);
	//}
	if (!m_pPanRemoteDesktop->IsWindowVisible())
	{
		CRect rect;
		if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
		{
			m_pDlgUserInfo->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
		}
		m_pPanRemoteDesktop->MoveWindow(&rect);
		m_pPanRemoteDesktop->ShowWindow(SW_SHOW);
	}
}
void CDlgChatRight::OpenFilePan(void)
{
	if (!ShowTabID(RIGHC_TAB_ID_FILE))
	{
		CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
		CRect rectTab;
		rectTab.left = 1+GetTabSizeWidth();
		rectTab.right = rectTab.left + const_Tab_Width2;
		rectTab.top = 0;
		rectTab.bottom = rectTab.top + const_Tab_Height-1;
		pTabInfo->Create(_T("文件传输"),rectTab,this,RIGHC_TAB_ID_FILE);
		m_pTabList.add(pTabInfo);
		ShowTabID(RIGHC_TAB_ID_FILE);
		CheckAndSendAdjustWidth();
	}
	//if (m_pPanFiles==NULL)
	//{
	//	m_pPanFiles = new CPanelFiles(this);
	//	m_pPanFiles->SetCircle(false);
	//	//m_pPanFiles->m_pChatRoom = m_pChatRoom;
	//	m_pPanFiles->Create(CPanelFiles::IDD, this);
	//	m_pPanFiles->ShowWindow(SW_SHOW);
	//}
	if (!m_pPanFiles->IsWindowVisible())
	{
		CRect rect;
		if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
		{
			m_pDlgUserInfo->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
		}
		m_pPanFiles->MoveWindow(&rect);
		m_pPanFiles->ShowWindow(SW_SHOW);
	}
}

//void CDlgChatRight::Update(void)
//{
//	m_labelUserList.SetChecked(m_hCurrentSelect==m_labelUserList.GetSafeHwnd());
//	theApp.InvalidateParentRect(&m_labelUserList);
//	m_labelECard.SetChecked(m_hCurrentSelect==m_labelECard.GetSafeHwnd());
//	theApp.InvalidateParentRect(&m_labelECard);
//	if (!theApp.GetDisableVideo())
//	{
//		m_labelVideos.SetChecked(m_hCurrentSelect==m_labelVideos.GetSafeHwnd());
//		theApp.InvalidateParentRect(&m_labelVideos);
//	}
//	if (!theApp.GetDisableRD())
//	{
//		m_labelRemoteDesktop.SetChecked(m_hCurrentSelect==m_labelRemoteDesktop.GetSafeHwnd());
//		theApp.InvalidateParentRect(&m_labelRemoteDesktop);
//	}
//	m_labelFiles.SetChecked(m_hCurrentSelect==m_labelFiles.GetSafeHwnd());
//	theApp.InvalidateParentRect(&m_labelFiles);
//	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd())
//	{
//		m_pDlgUserList->ShowWindow(m_labelUserList.GetChecked()?SW_SHOW:SW_HIDE);
//	}
//	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd())
//	{
//		m_pDlgUserInfo->ShowWindow(m_labelECard.GetChecked()?SW_SHOW:SW_HIDE);
//	}
//	if (!theApp.GetDisableVideo())
//	{
//		if (m_pPanVideos != NULL && m_pPanVideos->GetSafeHwnd())
//			m_pPanVideos->ShowWindow(m_labelVideos.GetChecked()?SW_SHOW:SW_HIDE);
//	}
//	if (!theApp.GetDisableRD())
//	{
//		if (m_pPanRemoteDesktop != NULL && m_pPanRemoteDesktop->GetSafeHwnd())
//			m_pPanRemoteDesktop->ShowWindow(m_labelRemoteDesktop.GetChecked()?SW_SHOW:SW_HIDE);
//	}
//	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd())
//		m_pPanFiles->ShowWindow(m_labelFiles.GetChecked()?SW_SHOW:SW_HIDE);
//	// 刷新整个区域
//	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd())
//		theApp.InvalidateParentRect(m_pDlgUserInfo);
//	theApp.InvalidateParentRect(this);
//}

void CDlgChatRight::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CDlgChatRight::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

//void CDlgChatRight::OnBnClickedButtonUserlist()
//{
//	m_hCurrentSelect = m_labelUserList.GetSafeHwnd();
//	Update();
//}
//
//void CDlgChatRight::OnBnClickedButtonEcard()
//{
//	m_hCurrentSelect = m_labelECard.GetSafeHwnd();
//	Update();
//}
//
//void CDlgChatRight::OnBnClickedButtonVideos()
//{
//	this->OpenVideoPan();
//	m_hCurrentSelect = m_labelVideos.GetSafeHwnd();
//	Update();
//}
//void CDlgChatRight::OnBnClickedButtonFiles()
//{
//	this->OpenFilePan();
//	//m_labelFiles.ShowWindow(SW_SHOW);
//	m_hCurrentSelect = m_labelFiles.GetSafeHwnd();
//	Update();
//}
//void CDlgChatRight::OnBnClickedButtonRd()
//{
//	this->OpenRdPan();
//	m_hCurrentSelect = m_labelRemoteDesktop.GetSafeHwnd();
//	Update();
//}

void CDlgChatRight::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(const_Tab_Height-1,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, RGB(255,255,255),0);

		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false,true,1));
		//this->AddBgDrawInfo(CEbBackDrawInfo(23,theDefaultBgColorBgLight1,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultBgColorBgLight3,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
		//this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	}

}

void CDlgChatRight::SetCallInfo(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccountInfo)
{
	m_pCallInfo = pCallInfo;
	m_pFromAccountInfo = pFromAccountInfo;

	if (m_pDlgUserList!=NULL)
		m_pDlgUserList->m_pCallInfo = m_pCallInfo;
	if (m_pDlgUserInfo!=NULL)
		m_pDlgUserInfo->m_pCallInfo = this->m_pCallInfo;
	if (m_pPanVideos!=NULL)
		m_pPanVideos->m_pCallInfo = m_pCallInfo;
	if (m_pPanRemoteDesktop!=NULL)
		m_pPanRemoteDesktop->m_pCallInfo = m_pCallInfo;
}

//BOOL CDlgChatRight::PreTranslateMessage(MSG* pMsg)
//{
//	if (pMsg->message == WM_KEYDOWN)
//	{
//		const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
//		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
//		////const bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;
//		if (bShift && bControl)
//		{
//			if (pMsg->wParam=='S')
//			{
//				theApp.GetMainWnd()->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS, 1, 1);
//				return TRUE;
//			}
//			//if (pMsg->wParam=='M')
//			//{
//			//	OnBnClickedButtonMsgRecord();
//			//	return TRUE;
//			////}else if (pMsg->wParam=='A')
//			////{
//			////	OnBnClickedButtonEbsc();
//			////	return TRUE;
//			//}
//		}
///*
//		if (GetKeyState(VK_CONTROL)&0x80)
//		{
//			if (pMsg->wParam==VK_UP)
//			{
//				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
//				return TRUE;
//			}else if (pMsg->wParam==VK_DOWN)
//			{
//				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
//				return TRUE;
//			}
//		}*/
//	}
//	return CEbDialogBase::PreTranslateMessage(pMsg);
//}

void CDlgChatRight::OnRightTabClicked(UINT nID)
{
	ShowTabID(nID,true);
}
void CDlgChatRight::ShowHideTab(const CRightTabInfo::pointer& pTabInfo, bool bShow)
{
	if (pTabInfo.get()==NULL) return;
	pTabInfo->ShowHide(bShow);
	switch (pTabInfo->GetCtrlID())
	{
	case RIGHC_TAB_ID_ECARD:
		m_pDlgUserInfo->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	case RIGHC_TAB_ID_USERLIST:
		m_pDlgUserList->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	case RIGHC_TAB_ID_VIDEO:
		m_pPanVideos->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	case RIGHC_TAB_ID_RD:
		m_pPanRemoteDesktop->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	case RIGHC_TAB_ID_FILE:
		m_pPanFiles->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	case RIGHC_TAB_ID_CHAT_RECORD:
		if (m_pMsgRecord!=NULL)
		{
			m_pMsgRecord->ShowWindow(bShow?SW_SHOW:SW_HIDE);
			if (bShow)
				m_pMsgRecord->RedrawCefBrowser();
		}
		break;
	case RIGHC_TAB_ID_GROUP_SHARE:
		if (m_pResourceMgr!=NULL)
			m_pResourceMgr->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		break;
	default:
		break;
	}
}
UINT CDlgChatRight::GetTabIDBySubscribeId(eb::bigint nSubscribeId) const
{
	AUTO_CONST_RLOCK(m_pTabList);
	CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetSubscribeId()==nSubscribeId)
			return pFrameWndInfo->GetCtrlID();
	}
	return 0;
}
int CDlgChatRight::GetOffsetIndexByHwnd(HWND hWnd) const
{
	if (hWnd==NULL) return -1;
	int nCurrentIndex = 0;
	AUTO_CONST_RLOCK(m_pTabList);
	CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsAppWindow(hWnd))
		{
			return nCurrentIndex;
		}
		nCurrentIndex++;
	}
	return -1;
}

bool CDlgChatRight::ShowTabID(UINT nID, bool bCheckClose)
{
	CRightTabInfo::pointer pShowFrameWndInfo;
	CRightTabInfo::pointer pHideFrameWndInfo;
	bool bClose = false;
	{
		CPoint pos;
		if (bCheckClose)
		{
			GetCursorPos(&pos);
			this->ScreenToClient(&pos);
		}

		BoostReadLock rdLock(m_pTabList.mutex());
		CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			CRightTabInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCtrlID()==nID || (nID==0 && pShowFrameWndInfo.get()==NULL))
			{
				pShowFrameWndInfo = pFrameWndInfo;
				pFrameWndInfo->ShowHide(true);
				if (bCheckClose && !bClose)
				{
					if (pFrameWndInfo->IsClickClose(pos))
					{
						bClose = true;
					}
				}
				
				//if (!bClose)
				//	pFrameWndInfo->ShowHide(true);
				if (pHideFrameWndInfo.get()!=NULL) break;
			}else if (pFrameWndInfo->IsChecked())
			{
				pHideFrameWndInfo = pFrameWndInfo;
				if (pShowFrameWndInfo.get()!=NULL) break;
			}
		}
	}
	if (pShowFrameWndInfo.get()==NULL) return false;
	bool bOldHideTabCheck = false;
	if (pHideFrameWndInfo.get()!=NULL)// && pShowFrameWndInfo->IsChecked())
	{
		bOldHideTabCheck = pHideFrameWndInfo->IsChecked();
		ShowHideTab(pHideFrameWndInfo,false);
	}
	const UINT nToShowCtrlID = pShowFrameWndInfo->GetCtrlID();
	switch (nToShowCtrlID)
	{
	case RIGHC_TAB_ID_ECARD:
		m_pDlgUserInfo->ShowWindow(SW_SHOW);
		break;
	case RIGHC_TAB_ID_USERLIST:
		m_pDlgUserList->ShowWindow(SW_SHOW);
		break;
	case RIGHC_TAB_ID_VIDEO:
		{
			if (!theApp.GetDisableVideo())
			{
				m_pPanVideos->ShowWindow(SW_SHOW);
				if (bClose)
				{
					if (m_pPanVideos->GetVideoCount()<=0 ||
						CDlgMessageBox::EbDoModal(this,"视频聊天",_T("正在视频通话：\t\n确定关闭视频通话吗？"),CDlgMessageBox::IMAGE_QUESTION)==IDOK)
					{
						m_pPanVideos->ExitChat(false);
						CloseTabID(nToShowCtrlID);
						m_pPanVideos->ShowWindow(SW_HIDE);
					}
				}
			}
		}break;
	case RIGHC_TAB_ID_RD:
		{
			if (!theApp.GetDisableRD())
			{
				m_pPanRemoteDesktop->ShowWindow(SW_SHOW);				
				if (bClose)
				{
					if (!m_pPanRemoteDesktop->GetInDesktop() ||
						CDlgMessageBox::EbDoModal(this,"远程桌面",_T("正在远程桌面：\t\n确定关闭远程桌面吗？"),CDlgMessageBox::IMAGE_QUESTION)==IDOK)
					{
						m_pPanRemoteDesktop->ExitRD();
						CloseTabID(nToShowCtrlID);
						m_pPanRemoteDesktop->ShowWindow(SW_HIDE);
					}
				}
			}
		}break;
	case RIGHC_TAB_ID_FILE:
		{
			m_pPanFiles->ShowWindow(SW_SHOW);
			if (bClose)
			{
				if (m_pPanFiles->IsEmpty() ||
					CDlgMessageBox::EbDoModal(this,"文件传输",_T("正在传输文件：\t\n确定停止所有传输文件吗？"),CDlgMessageBox::IMAGE_QUESTION)==IDOK)
				{
					m_pPanFiles->ExitChat(false);
					CloseTabID(nToShowCtrlID);
					m_pPanFiles->ShowWindow(SW_HIDE);
				}
			}
		}break;
	case RIGHC_TAB_ID_CHAT_RECORD:
		{
			if (m_pMsgRecord==NULL) break;
			if (bClose)
			{
				CloseTabID(nToShowCtrlID);
				m_pMsgRecord->ShowWindow(SW_HIDE);
			}else
			{
				m_pMsgRecord->ShowWindow(SW_SHOW);
				m_pMsgRecord->RedrawCefBrowser();
			}
		}break;
	case RIGHC_TAB_ID_GROUP_SHARE:
		{
			if (m_pResourceMgr==NULL) break;
			if (bClose)
			{
				CloseTabID(nToShowCtrlID);
				m_pResourceMgr->ShowWindow(SW_HIDE);
			}else
			{
				m_pResourceMgr->ShowWindow(SW_SHOW);
			}
		}break;
	default:
		{
			if (bClose)
			{
				pShowFrameWndInfo->SendClose();
			}
			return true;
		}break;
	}
	if (bClose && bOldHideTabCheck)
		ShowTabID(pHideFrameWndInfo->GetCtrlID());
	return true;
}
bool CDlgChatRight::CloseTabID(UINT nID)
{
	CRightTabInfo::pointer pFindFrameWndInfo;
	CRightTabInfo::pointer pPrevFrameWndInfo;
	{
		BoostWriteLock wtlock(m_pTabList.mutex());
		CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			CRightTabInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetCtrlID()==nID)
			{
				pFindFrameWndInfo = pFrameWndInfo;
				m_pTabList.erase(pIter);
				break;
			}
			pPrevFrameWndInfo = pFrameWndInfo;
		}
	}
	if (pFindFrameWndInfo.get()!=NULL)
	{
		//if (pFindFrameWndInfo->IsChecked() || m_pTabList.size()==1)
		{
			if (pPrevFrameWndInfo.get()!=NULL)
			{
				ShowTabID(pPrevFrameWndInfo->GetCtrlID());
			}else
			{
				ShowTabID(0);
			}
		}
		ChangeTabPos();

		ShowHideTab(pFindFrameWndInfo,false);
		if (pFindFrameWndInfo->GetCtrlID()>=RIGHC_TAB_ID_APP_FIRST)
		{
			m_pAppCtrlIDList.add(pFindFrameWndInfo->GetCtrlID());
		}
		//switch (nID)
		//{
		////case RIGHC_TAB_ID_ECARD:
		////	m_pDlgUserInfo->ShowWindow(SW_HIDE);
		////	break;
		////case RIGHC_TAB_ID_USERLIST:
		////	m_pDlgUserList->ShowWindow(SW_HIDE);
		////	break;
		//case RIGHC_TAB_ID_VIDEO:
		//	m_pPanVideos->ShowWindow(SW_HIDE);
		//	break;
		//case RIGHC_TAB_ID_RD:
		//	m_pPanRemoteDesktop->ShowWindow(SW_HIDE);
		//	break;
		//case RIGHC_TAB_ID_FILE:
		//	m_pPanFiles->ShowWindow(SW_HIDE);
		//	break;
		//case RIGHC_TAB_ID_CHAT_RECORD:
		//	m_pMsgRecord->ShowWindow(SW_HIDE);
		//	break;
		//case RIGHC_TAB_ID_GROUP_SHARE:
		//	m_pResourceMgr->ShowWindow(SW_HIDE);
		//	break;
		//default:
		//	break;
		//}
		CheckAndSendAdjustWidth();
	}
	return pFindFrameWndInfo.get()==NULL?false:true;
}
bool CDlgChatRight::ChangeTabText(UINT nID, LPCTSTR lpszCaption)
{
	BoostReadLock rdLock(m_pTabList.mutex());
	CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetCtrlID()==nID)
		{
			pFrameWndInfo->SetTabText(lpszCaption);
			//this->SetToolTipText(nID,lpszCaption);
			return true;
		}
	}
	return false;
}
bool CDlgChatRight::ChangeTabText(const CWnd* pWnd, LPCTSTR lpszCaption)
{
	BoostReadLock rdLock(m_pTabList.mutex());
	CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->ChangeTabText(pWnd,lpszCaption))
		{
			//this->SetToolTipText(pFrameWndInfo->GetCtrlID(),lpszCaption);
			return true;
		}
	}
	return false;
}

int CDlgChatRight::GetTabSizeWidth(void) const
{
	int nResult = 0;
	AUTO_CONST_RLOCK(m_pTabList);
	CLockList<CRightTabInfo::pointer>::const_iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		nResult += pFrameWndInfo->GetTabWidth();
	}
	return nResult;
}

void CDlgChatRight::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMERID_CHECK_MOUSE_POS:
		{
			CheckMousePos();
		}break;
	case TIMERID_CHECK_ADJUST_WIDTH:
		{
			KillTimer(nIDEvent);
			CheckAndSendAdjustWidth();
		}break;
	default:
		break;
	}
	CEbDialogBase::OnTimer(nIDEvent);
}
void CDlgChatRight::CheckMousePos(void)
{
	CPoint pos;
	GetCursorPos(&pos);
	this->ScreenToClient(&pos);
	BoostReadLock rdLock(m_pTabList.mutex());
	CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		pFrameWndInfo->CheckMousePos(pos);
	}
}
//LRESULT CDlgChatRight::OnMessageWndClose(WPARAM wParam, LPARAM lParam)
//{
//	const HWND hCloseWnd = (HWND)wParam;
//	if (hCloseWnd==m_pPanVideos->GetSafeHwnd())
//	{
//		this->CloseTabID(RIGHC_TAB_ID_VIDEO);
//	}else if (hCloseWnd==m_pPanRemoteDesktop->GetSafeHwnd())
//	{
//		this->CloseTabID(RIGHC_TAB_ID_RD);
//	}
//	return 0;
//}
void CDlgChatRight::CheckAndSendAdjustWidth(void)
{
	const int nNeedWidth = GetNeedWidth();
	CRect rect;
	this->GetWindowRect(&rect);
	const int nWidthOffset = nNeedWidth-rect.Width();
	if (nWidthOffset!=0 && this->GetSafeHwnd()!=NULL)
	{
		if (rect.left>0 && rect.right>0)	// 上个窗口最小化为负数，不处理
		{
			rect.right += nWidthOffset;
			this->MoveWindow(&rect);
		}
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_ADJUST_WIDTH,(WPARAM)this->GetSafeHwnd(),(LPARAM)nWidthOffset);
	}
}
void CDlgChatRight::ChangeTabPos(void)
{
	CRect rectTab;
	rectTab.left = 1;
	rectTab.right = 1;
	rectTab.top = 0;
	rectTab.bottom = const_Tab_Height-1;
	BoostReadLock rdLock(m_pTabList.mutex());
	CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
	for (; pIter!=m_pTabList.end(); pIter++)
	{
		CRightTabInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetCtrlID()==RIGHC_TAB_ID_USERLIST)
		{
			rectTab.right = rectTab.left + const_Tab_Width1;
		}else
		{
			rectTab.left = rectTab.right;
			rectTab.right = rectTab.left+const_Tab_Width2;
		}
		pFrameWndInfo->MoveBtnWindow(&rectTab);
	}

}

LRESULT CDlgChatRight::OnMessageGroupShare(WPARAM wParam, LPARAM lParam)
{
	if (m_pCallInfo.m_sGroupCode==0) return 0;
	if (CloseTabID(RIGHC_TAB_ID_GROUP_SHARE))
	{
		return 0;
	}
	CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
	CRect rectTab;
	rectTab.left = 1+GetTabSizeWidth();
	rectTab.right = rectTab.left + const_Tab_Width2;
	rectTab.top = 0;
	rectTab.bottom = rectTab.top + const_Tab_Height-1;
	pTabInfo->Create(_T("群共享"),rectTab,this,RIGHC_TAB_ID_GROUP_SHARE);
	m_pTabList.add(pTabInfo);
	ShowTabID(RIGHC_TAB_ID_GROUP_SHARE);
	if (m_pResourceMgr==NULL)
	{
		m_pResourceMgr = new CDlgResourceMgr(this);
		m_pResourceMgr->SetChildMode(true);
		m_pResourceMgr->SetCircle(false);
		m_pResourceMgr->Create(CDlgResourceMgr::IDD_CHILD, this);
		//m_pResourceMgr->Create(CDlgResourceMgr::IDD, this);	// ** 界面会卡
		m_pResourceMgr->ShowWindow(SW_HIDE);
		//m_pResourceMgr->SetParent(this);
		//m_pResourceMgr->ModifyStyle(WS_POPUP, WS_CHILD);
	}
	m_pResourceMgr->LoadGroupResource(m_pCallInfo.m_sGroupCode);
	if (!m_pResourceMgr->IsWindowVisible())
	{
		CRect rect;
		if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
		{
			m_pDlgUserInfo->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
		}
		m_pResourceMgr->MoveWindow(&rect);
		m_pResourceMgr->ShowWindow(SW_SHOW);
	}
	CheckAndSendAdjustWidth();
	return 0;
}
LRESULT CDlgChatRight::OnMessageMsgRecord(WPARAM wParam, LPARAM lParam)
{
	if (CloseTabID(RIGHC_TAB_ID_CHAT_RECORD))
	{
		return 0;
	}
	CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
	CRect rectTab;
	rectTab.left = 1+GetTabSizeWidth();
	rectTab.right = rectTab.left + const_Tab_Width2;
	rectTab.top = 0;
	rectTab.bottom = rectTab.top + const_Tab_Height-1;
	pTabInfo->Create(_T("聊天记录"),rectTab,this,RIGHC_TAB_ID_CHAT_RECORD);
	m_pTabList.add(pTabInfo);
	ShowTabID(RIGHC_TAB_ID_CHAT_RECORD);
	if (m_pMsgRecord==NULL)
	{
		m_pMsgRecord = new CDlgMsgRecord(this);
		m_pMsgRecord->SetChildMode(true);
		m_pMsgRecord->SetCircle(false);
		//m_pMsgRecord->Create(CDlgMsgRecord::IDD, this);	// **界面会卡
		m_pMsgRecord->Create(CDlgMsgRecord::IDD_CHILD, this);
		m_pMsgRecord->ShowWindow(SW_HIDE);
		//m_pMsgRecord->SetParent(this);
		//m_pMsgRecord->ModifyStyle(WS_POPUP, WS_CHILD);
	}
	if (m_pCallInfo.m_sGroupCode==0)
		m_pMsgRecord->LoadAccountMsgRecord(m_pFromAccountInfo.GetUserId(),"");
	else
		m_pMsgRecord->LoadDepartmentMsgRecord(m_pCallInfo.m_sGroupCode,"");
	if (!m_pMsgRecord->IsWindowVisible())
	{
		CRect rect;
		if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
		{
			m_pDlgUserInfo->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
		}
		m_pMsgRecord->MoveWindow(&rect);
		m_pMsgRecord->ShowWindow(SW_SHOW);
		m_pMsgRecord->RedrawCefBrowser();
	}
	CheckAndSendAdjustWidth();
	return 0;
}
LRESULT CDlgChatRight::OnFrameWndMax(WPARAM wParam, LPARAM lParam)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	return 0;
}
LRESULT CDlgChatRight::OnFrameWndClose(WPARAM wParam, LPARAM lParam)
{
	const HWND hCloseWnd = (HWND)wParam;
	if (hCloseWnd==m_pPanVideos->GetSafeHwnd())
	{
		this->CloseTabID(RIGHC_TAB_ID_VIDEO);
	}else if (hCloseWnd==m_pPanRemoteDesktop->GetSafeHwnd())
	{
		this->CloseTabID(RIGHC_TAB_ID_RD);
	}else if (m_pMsgRecord!=NULL && hCloseWnd==m_pMsgRecord->GetSafeHwnd())
	{
		this->CloseTabID(RIGHC_TAB_ID_CHAT_RECORD);
	}else
	{
		const UINT nToCloseCtrlID = (UINT)lParam;
		this->CloseTabID(nToCloseCtrlID);
	}
	return 0;
}
LRESULT CDlgChatRight::OnFrameWndTitle(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	char * sTitle = (char*)lParam;
	if (sTitle==0 || strlen(sTitle)==0) return 0;
	ChangeTabText(pWnd, sTitle);
	delete[] sTitle;
	return 0;
}
LRESULT CDlgChatRight::OnFrameWndOpen(WPARAM wParam, LPARAM lParam)
{
	const size_t nIndex = (size_t)wParam;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		const eb::bigint m_nSelectCallId = m_pCallInfo.GetCallId();
		const eb::bigint m_nSelectUserId = 0;
		const tstring m_sSelectAccount;
		const eb::bigint m_nSelectGroupId = m_pCallInfo.m_sGroupCode;
		if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_WINDOW ||	// 窗口模式；
			pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME)
		{
#ifdef USES_EBCOM_TEST
			const CEBString sFuncUrl = theEBClientCore->EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount.c_str(),m_nSelectGroupId).GetBSTR();
#else
			const CEBString sFuncUrl = theEBAppClient.EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId);
#endif
			if (!sFuncUrl.empty())
			{
				CString sFullFuncurl;
				sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theDefaultFlatBgColor),GetGValue(theDefaultFlatBgColor),GetBValue(theDefaultFlatBgColor));

				UINT nTabID = GetTabIDBySubscribeId(pSubscribeFuncInfo.m_nSubscribeId);
				if (nTabID==0)
				{
					if (!m_pAppCtrlIDList.front(nTabID))
						return 0;
				}
				if (!ShowTabID(nTabID))
				{
					CRect rectApp;
					if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
					{
						m_pDlgUserInfo->GetWindowRect(&rectApp);
						this->ScreenToClient(&rectApp);
					}

					CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
					CRect rectTab;
					rectTab.left = 1+GetTabSizeWidth();
					rectTab.right = rectTab.left + const_Tab_Width2;
					rectTab.top = 0;
					rectTab.bottom = rectTab.top + const_Tab_Height-1;
					pTabInfo->Create(pSubscribeFuncInfo.m_sFunctionName.c_str(),rectTab,this,nTabID);
					pTabInfo->SetSubscribeFuncInfo(rectApp,pSubscribeFuncInfo,(LPCTSTR)sFullFuncurl,"",false);
					m_pTabList.add(pTabInfo);
					ShowTabID(nTabID);
					this->SetToolTipText(nTabID,pSubscribeFuncInfo.m_sFunctionName.c_str());
					CheckAndSendAdjustWidth();
				}
			}
		}else
		{
			theApp.m_nSelectCallId = m_nSelectCallId;
			theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
		}
	}
	return 0;
}
LRESULT CDlgChatRight::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
	if (pOpenAppUrlInfo==0) return 0;
	UINT nTabID = 0;
	if (!m_pAppCtrlIDList.front(nTabID))
	{
		ShellExecute(NULL, "open", pOpenAppUrlInfo->m_sUrl.c_str(), NULL, pOpenAppUrlInfo->m_sUrl.c_str(),SW_SHOW);
		delete pOpenAppUrlInfo;
		return 0;
	}

	CRect rectApp;
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->GetWindowRect(&rectApp);
		this->ScreenToClient(&rectApp);
	}

	const int nFromOffsetIndex = GetOffsetIndexByHwnd(pOpenAppUrlInfo->m_hwndFrom);
	const int nInsertOffset = nFromOffsetIndex!=-1?(nFromOffsetIndex+1):-1;

	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = pOpenAppUrlInfo->m_sUrl;
	pSubscribeFuncInfo.m_bClosePrompt = false;
	pSubscribeFuncInfo.m_bDisableContextMenu = false;
	pSubscribeFuncInfo.m_bDisableScrollBar = false;
	pSubscribeFuncInfo.m_bWinResizable = true;
	CRightTabInfo::pointer pTabInfo = CRightTabInfo::create();
	CRect rectTab;
	rectTab.left = 1+GetTabSizeWidth();
	rectTab.right = rectTab.left + const_Tab_Width2;
	rectTab.top = 0;
	rectTab.bottom = rectTab.top + const_Tab_Height-1;
	pTabInfo->Create(pSubscribeFuncInfo.m_sFunctionName.c_str(),rectTab,this,nTabID);
	pTabInfo->SetSubscribeFuncInfo(rectApp,pSubscribeFuncInfo,pOpenAppUrlInfo->m_sUrl,pOpenAppUrlInfo->m_sPostData,pOpenAppUrlInfo->m_bSaveBrowseTitle);
	bool bInsertOk = false;
	if (nInsertOffset>=0 && nInsertOffset<(int)m_pTabList.size())
	{
		int nCurrentIndex = 0;
		BoostWriteLock wtLock(m_pTabList.mutex());
		CLockList<CRightTabInfo::pointer>::iterator pIter = m_pTabList.begin();
		for (; pIter!=m_pTabList.end(); pIter++)
		{
			if ((nCurrentIndex++)==nInsertOffset)
			{
				m_pTabList.insert(pIter,pTabInfo);
				bInsertOk = true;
				break;
			}
		}
	}
	if (bInsertOk)
	{
		ChangeTabPos();
	}else
	{
		m_pTabList.add(pTabInfo);
	}		
	//m_pTabList.add(pTabInfo);
	ShowTabID(nTabID);
	//this->SetToolTipText(nTabID,pSubscribeFuncInfo.m_sFunctionName.c_str());
	CheckAndSendAdjustWidth();

	delete pOpenAppUrlInfo;
	return 0;
}
LRESULT CDlgChatRight::OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam)
{
	char * sUrl = (char*)wParam;
	if (sUrl==0 || strlen(sUrl)==0) return 0;
	delete[] sUrl;
	return 0;
}
