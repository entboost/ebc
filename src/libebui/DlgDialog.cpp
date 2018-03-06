// DlgDialog.cpp : implementation file
//

#include "stdafx.h"
#ifdef EBUI_EXPORTS
#include "EBUIApp.h"
#else
#include "POP.h"
#endif
#include "DlgDialog.h"
#include "DlgFrameList.h"
#include "DlgChatInput.h"
#include "DlgDiaRecord.h"
//#include "DlgSelectUser.h"
#include "DlgUserList.h"
//#include "DlgVideoConference.h"
#include "Core/SkinMemDC.h"

#define IDC_STA_SPLITTER 15001
//#define TIMER_UPDATEWINDOW		100

#define DEFAULT_P2P_WIDTH	710
//#define DEFAULT_P2P_WIDTH	560
#define DEFAULT_DEP_WIDTH	760
#define DEFAULT_DLG_HEIGHT	588

#define USES_REMOTE_DESKTOP

// CDlgDialog dialog

IMPLEMENT_DYNAMIC(CDlgDialog, CEbDialogBase)

CDlgDialog::CDlgDialog(CWnd* pParent /*=NULL*/, bool bDeleteThis)
	: CEbDialogBase(CDlgDialog::IDD, pParent)
	//, m_bReceiveOffLineMsg(false)
	, m_bDeleteThis(bDeleteThis)
{
	m_nGroupType = EB_GROUP_TYPE_DEPARTMENT;
	m_nChatFuncSize = 0;
	m_pFromImage = NULL;
	m_nFromLineState = EB_LINE_STATE_ONLINE_NEW;
	m_bOwnerCall = false;
}

CDlgDialog::~CDlgDialog()
{
}

void CDlgDialog::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_SEND_FILE, m_btnSendFile);
	DDX_Control(pDX, IDC_BUTTON_VIDEO, m_btnVideo);
	DDX_Control(pDX, IDC_BUTTON_RD, m_btnRemoteDesktop);
	DDX_Control(pDX, IDC_BUTTON_GROUP_SHARE, m_btnGroupShare);
	DDX_Control(pDX, IDC_BUTTON_ADD_USER, m_btnAddUser);
	DDX_Control(pDX, IDC_BUTTON_EXIT_CHAT, m_btnExitChat);
	DDX_Control(pDX, IDC_BUTTON_CHAT_FUNC, m_btnChatFunc);
}


BEGIN_MESSAGE_MAP(CDlgDialog, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(EB_MSG_SELECTED_EMP, OnMessageSelectedEmp)
	ON_MESSAGE(EB_MSG_EXIT_CHAT, OnMessageExitChat)
	ON_MESSAGE(EB_MSG_EBSC, OnMessageEBSC)
	ON_MESSAGE(EB_MSG_EBSC_OK, OnMessageEBSCOK)
	ON_MESSAGE(EB_MSG_VIEW_MSG_RECORD, OnMessageMsgRecord)
	//ON_MESSAGE(WM_HOTKEY, OnMessageEBSC)
	
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CDlgDialog::OnBnClickedButtonAddUser)
	ON_BN_CLICKED(IDC_BUTTON_SEND_FILE, &CDlgDialog::OnBnClickedButtonSendFile)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgDialog::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgDialog::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgDialog::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_EXIT_CHAT, &CDlgDialog::OnBnClickedButtonExitChat)
	ON_BN_CLICKED(IDC_BUTTON_CHAT_FUNC, &CDlgDialog::OnBnClickedButtonChatFunc)
	
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_VIDEO, &CDlgDialog::OnBnClickedButtonVideo)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON_GROUP_SHARE, &CDlgDialog::OnBnClickedButtonGroupShare)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_COMMAND(EB_COMMAND_RD_CONTROL_ME, OnRdControlMe)
	ON_COMMAND(EB_COMMAND_RD_CONTROL_DEST, OnRdControlDest)
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
	ON_BN_CLICKED(IDC_BUTTON_RD, &CDlgDialog::OnBnClickedButtonRd)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CDlgDialog::SetCtrlColor(void)
{
	if (m_pDlgChatInput.get()!=NULL && m_pDlgChatInput->GetSafeHwnd()!=NULL)
		m_pDlgChatInput->SetCtrlColor();
	if (m_pDlgChatRight!=NULL && m_pDlgChatRight->GetSafeHwnd()!=NULL)
		m_pDlgChatRight->SetCtrlColor();
}

void CDlgDialog::SetOwnerCall(bool bOwnerCall)
{
	m_bOwnerCall = bOwnerCall;
}

// CDlgDialog message handlers

BOOL CDlgDialog::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
//#ifdef USES_FRAME_LIST
//	//this->ModifyStyle(
//#else
//	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
//#endif

	//this->SetTransparentType(CEbDialogBase::TT_EDIT);
	//RegisterHotKey(this->m_hWnd, 0x2013, MOD_CONTROL|MOD_ALT, 'P'); 

	this->SetMouseMove(FALSE);

	m_btnMin.ShowWindow(SW_HIDE);
	m_btnMin.SetAutoSize(false);
	m_btnMin.Load(IDB_PNG_MIN);
	m_btnMin.SetToolTipText(_T("最小化"));
	m_btnMax.SetAutoSize(false);
	m_btnMax.Load(IDB_PNG_MAX);
	m_btnMax.SetToolTipText(_T("最大化"));
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("关闭"));
	const bool bIsDepDialog = m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0;

	this->GetDlgItem(IDC_BUTTON_NULL)->MoveWindow(0,0,0,0);
	//m_btnAddUser.SetWndFocus(this->GetDlgItem(IDC_BUTTON_NULL));
	m_btnAddUser.SetAutoSize(false);
	m_btnAddUser.Load(IDB_PNG_BTN_ADDUSER);
	m_btnAddUser.SetWindowText(_T(""));
	m_btnAddUser.SetToolTipText(_T("邀请好友加入讨论组"));
	m_btnSendFile.SetAutoSize(false);
	m_btnSendFile.Load(IDB_PNG_BTN_SENDFILE);
	m_btnSendFile.SetWindowText(_T(""));
	m_btnSendFile.SetToolTipText(_T("发送文件"));
	m_btnVideo.SetAutoSize(false);
	m_btnVideo.Load(IDB_PNG_BTN_VIDEOCHAT);
	m_btnVideo.SetWindowText(_T(""));
	m_btnVideo.ShowWindow(theApp.GetDisableVideo()?SW_HIDE:SW_SHOW);

	// IDB_PNG_BTN_AUDIOCHAT
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideo.SetToolTipText(_T("开始视频通话"));
	}else
	{
		m_btnVideo.SetToolTipText(_T("申请加入视频会议"));
		m_btnSendFile.ShowWindow(theApp.GetDisableGroupSharedCloud()?SW_HIDE:SW_SHOW);

	}
#ifdef USES_REMOTE_DESKTOP
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		m_btnRemoteDesktop.SetAutoSize(false);
		m_btnRemoteDesktop.Load(IDB_PNG_BTN_REMOTEDESKTOP);
		m_btnRemoteDesktop.SetWindowText(_T(""));
		m_btnRemoteDesktop.SetToolTipText(_T("远程桌面"));
		m_btnRemoteDesktop.ShowWindow(theApp.GetDisableRD()?SW_HIDE:SW_SHOW);
	}else
	{
		m_btnRemoteDesktop.ShowWindow(SW_HIDE);
	}
#else
	m_btnRemoteDesktop.ShowWindow(SW_HIDE);
#endif

	m_btnGroupShare.SetAutoSize(false);
	m_btnGroupShare.Load(IDB_PNG_BTN_GROUPSHARE);
	m_btnGroupShare.SetWindowText(_T(""));
	m_btnGroupShare.SetToolTipText(_T("群共享"));
	if (!bIsDepDialog || theApp.GetDisableGroupSharedCloud())
		m_btnGroupShare.ShowWindow(SW_HIDE);
	m_btnChatFunc.SetAutoSize(false);
	m_btnChatFunc.Load(IDB_PNG_BTN_CHAT_FUNC);
	m_btnChatFunc.SetWindowText(_T(""));
	m_btnChatFunc.SetToolTipText(_T("应用功能"));
	//if (theApp.IsLogonVisitor())
	//	m_btnChatFunc.ShowWindow(SW_HIDE);
	m_btnExitChat.SetAutoSize(false);
	m_btnExitChat.Load(IDB_PNG_BTN_EXITCHAT);
	m_btnExitChat.SetWindowText(_T(""));
	m_btnExitChat.SetToolTipText(_T("退出会话"));

	// ??
	//if (/*m_pEbCallInfo->m_pCallInfo->m_bOffLineCall && */!m_pEbCallInfo->m_pFromAccountInfo.m_sFromInfo.empty())
	////if (this->m_bReceiveOffLineMsg && !m_pFromAccountInfo->m_sFromInfo.empty())
	//{
	//	theEBAppClient.GetAccountInfoByFromInfo(m_pEbCallInfo->m_pFromAccountInfo->m_sFromInfo.c_str(), m_pEbCallInfo->m_pFromAccountInfo);
	//}

	m_pDlgChatInput = CDlgChatInput::create(this);
	m_pDlgChatInput->SetOwnerCall(m_bOwnerCall);
	m_pDlgChatInput->SetCircle(false);
	m_pDlgChatInput->SetCallInfo(m_pEbCallInfo->m_pCallInfo,m_pEbCallInfo->m_pFromAccountInfo);
	//m_pDlgChatInput->m_pCallInfo = m_pEbCallInfo->m_pCallInfo;
	//m_pDlgChatInput->m_pFromAccountInfo = m_pEbCallInfo->m_pFromAccountInfo;
	//m_pDlgChatInput->m_bReceiveOffLineMsg = m_bReceiveOffLineMsg;
	m_pDlgChatInput->Create(CDlgChatInput::IDD, this);
	m_pDlgChatInput->ShowWindow(SW_SHOW);
	m_pDlgChatInput->SetFocus();

	CRect rc(200, 0, 200+1, 500);
	m_wndSplitter.SetStyle(SPS_VERTICAL);
	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_STA_SPLITTER);
	m_wndSplitter.SetTransparent(true);
	//m_wndSplitter.SetTransparent(false);
	//m_wndSplitter.SetColor(theApp.GetBgColor3(),theApp.GetBgColor1());
	//m_wndSplitter.SetColor(RGB(188, 234, 246), RGB(188, 234, 246));
	//m_wndSplitter.SetRange(50, 50, -1);

	m_pDlgChatRight = CDlgChatRight::create(this);
	m_pDlgChatRight->SetCircle(false);
	m_pDlgChatRight->SetCallInfo(m_pEbCallInfo->m_pCallInfo,m_pEbCallInfo->m_pFromAccountInfo);
	//m_pDlgChatRight->m_pCallInfo = m_pEbCallInfo->m_pCallInfo;
	//m_pDlgChatRight->m_pFromAccountInfo = m_pEbCallInfo->m_pFromAccountInfo;
	m_pDlgChatRight->Create(CDlgChatRight::IDD, this);
	m_pDlgChatRight->MoveWindow(300, 0, 227, 500);	// 227:WIDTH
	m_pDlgChatRight->ShowWindow(SW_SHOW);

	// **必须放在MoveWindow前面；
	const EB_FUNC_LOCATION nFuncLocation = m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0?EB_FUNC_LOCATION_USER_CHAT_BTN1:EB_FUNC_LOCATION_GROUP_CHAT_BTN1;
#ifdef USES_EBCOM_TEST
	m_nChatFuncSize = theEBClientCore->EB_GetSubscribeFuncSize(nFuncLocation);
#else
	m_nChatFuncSize = theEBAppClient.EB_GetSubscribeFuncSize(nFuncLocation);
#endif
	m_btnChatFunc.ShowWindow(m_nChatFuncSize>0?SW_SHOW:SW_HIDE);

	CEBString sDescription;
	m_nFromLineState = EB_LINE_STATE_ONLINE_NEW;
	USES_CONVERSION;
	if (bIsDepDialog)
	{
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		if (pGroupInfo != NULL)
		{
			m_sFromName = pGroupInfo->GroupName.GetBSTR();
			m_sFromDescription = pGroupInfo->Description.GetBSTR();
			m_nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
		}
#else
		EB_GroupInfo pGroupInfo;
		if (theEBAppClient.EB_GetGroupInfo(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,&pGroupInfo))
		{
			m_sFromName = pGroupInfo.m_sGroupName;
			m_sFromDescription = pGroupInfo.m_sDescription;
			m_nGroupType = pGroupInfo.m_nGroupType;
		}
#endif
		if (m_sFromName.empty())
		{
			char lpszBuffer[24];
			sprintf(lpszBuffer,"%lld",m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
			m_sFromName = lpszBuffer;
		}

		switch (m_nGroupType)
		{
		case EB_GROUP_TYPE_DEPARTMENT:
			{
				// 部门及项目组，不能随意添加成员，和不能随意退出
				m_btnAddUser.ShowWindow(SW_HIDE);
				m_btnExitChat.ShowWindow(SW_HIDE);
				m_pFromImage = theApp.m_imageDefaultDepartment->Clone();
			}break;
		case EB_GROUP_TYPE_PROJECT:
			{
				// 部门及项目组，不能随意添加成员，和不能随意退出
				m_btnAddUser.ShowWindow(SW_HIDE);
				m_btnExitChat.ShowWindow(SW_HIDE);
				m_pFromImage = theApp.m_imageDefaultProject->Clone();
			}break;
		case EB_GROUP_TYPE_GROUP:
			{
				m_btnExitChat.SetToolTipText(_T("退出该群组"));
				m_btnAddUser.SetToolTipText(_T("邀请好友加入群组"));
				m_pFromImage = theApp.m_imageDefaultGroup->Clone();
			}break;
		case EB_GROUP_TYPE_TEMP:
			{
				m_btnExitChat.SetToolTipText(_T("退出该讨论组"));
				m_btnAddUser.SetToolTipText(_T("邀请好友加入讨论组"));
				m_pFromImage = theApp.m_imageDefaultTempGroup->Clone();
			}break;
		default:
			break;
		}

#ifdef USES_EBCOM_TEST
		_variant_t pMemberList = theEBClientCore->EB_GetGroupMemberInfoList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		if (pMemberList.vt!=VT_EMPTY && pMemberList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pMemberList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_DISPATCH)
					continue;
				CComPtr<IEB_MemberInfo> pInterface;
				var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
				OnUserEmpInfo(pInterface);
			}
		}
#else
		std::vector<EB_MemberInfo> pMemberList;
		theEBAppClient.EB_GetGroupMemberInfoList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pMemberList);
		for (size_t i=0;i<pMemberList.size(); i++)
		{
			const EB_MemberInfo& pMemberInfo = pMemberList[i];
			OnUserEmpInfo(&pMemberInfo);
		}
#endif
	}else
	{
		if (m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR)
		{
			m_pFromImage = theApp.m_imageDefaultVisitor->Clone();
			m_sFromName = m_pEbCallInfo->m_pFromAccountInfo.GetAccount();
		}else
		{
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode);
			if (pMemberInfo != NULL)
			{
				m_sFromName = pMemberInfo.UserName.GetBSTR();
				m_nFromLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
				m_sFromDescription = pMemberInfo->Description.GetBSTR();
				const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
				if (PathFileExists(sMemberHeadFile.c_str()))
				{
					m_pFromImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sMemberHeadFile.c_str()));
					//sImagePath = sMemberHeadFile;
				}
			}else if (m_pEbCallInfo->m_bOffLineUser)
			{
				m_nFromLineState = EB_LINE_STATE_OFFLINE;
			}
#else
			EB_MemberInfo pMemberInfo;
			if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode))
			{
				m_sFromName = pMemberInfo.m_sUserName;
				m_nFromLineState = pMemberInfo.m_nLineState;
				m_sFromDescription = pMemberInfo.m_sDescription;
				if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
				{
					//sImagePath = pMemberInfo.m_sHeadResourceFile;
					m_pFromImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(pMemberInfo.m_sHeadResourceFile.c_str()));
				}
			}else if (m_pEbCallInfo->m_bOffLineUser)
			{
				m_nFromLineState = EB_LINE_STATE_OFFLINE;
			}
#endif
			if (m_sFromName.empty())
			{
				if (!m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName.empty())
				{
					m_sFromName = m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName;
				}else
				{
					m_sFromName = m_pEbCallInfo->m_pFromAccountInfo.GetUserName();
				}
			}
		}
	}
	if (m_pFromImage == NULL)
	{
		m_pFromImage = theApp.m_imageDefaultMember->Clone();
	}

//#ifdef USES_EBCOM_TEST
//	short nGroupType = theEBClientCore->EB_GetGroupType(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
//	if (nGroupType >= 0)
//#else
//	EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
//	if (theEBAppClient.EB_GetGroupType(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,nGroupType))
//#endif
//	{
//		m_nGroupType = (EB_GROUP_TYPE)nGroupType;
//		switch (m_nGroupType)
//		{
//		case EB_GROUP_TYPE_DEPARTMENT:
//		case EB_GROUP_TYPE_PROJECT:
//			{
//				// 部门及项目组，不能随意添加成员，和不能随意退出
//				m_btnAddUser.ShowWindow(SW_HIDE);
//				m_btnExitChat.ShowWindow(SW_HIDE);
//			}break;
//		case EB_GROUP_TYPE_GROUP:
//			{
//				m_btnExitChat.SetToolTipText(_T("退出该群组"));
//				m_btnAddUser.SetToolTipText(_T("邀请好友加入个人群组"));
//			}break;
//		case EB_GROUP_TYPE_TEMP:
//			{
//				m_btnExitChat.SetToolTipText(_T("退出该讨论组"));
//				m_btnAddUser.SetToolTipText(_T("邀请好友加入讨论组"));
//			}break;
//		default:
//			break;
//		}
//
//#ifdef USES_EBCOM_TEST
//		_variant_t pMemberList = theEBClientCore->EB_GetGroupMemberInfoList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
//		if (pMemberList.vt!=VT_EMPTY && pMemberList.parray != NULL)
//		{
//			CComSafeArray<VARIANT> m_sa(pMemberList.parray);
//			for (ULONG i=0;i<m_sa.GetCount();i++)
//			{
//				CComVariant var = m_sa.GetAt(i);
//				if (var.vt != VT_DISPATCH)
//					continue;
//				CComPtr<IEB_MemberInfo> pInterface;
//				var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
//				OnUserEmpInfo(pInterface);
//			}
//		}
//#else
//		std::vector<EB_MemberInfo> pMemberList;
//		theEBAppClient.EB_GetGroupMemberInfoList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pMemberList);
//		for (size_t i=0;i<pMemberList.size(); i++)
//		{
//			const EB_MemberInfo& pMemberInfo = pMemberList[i];
//			OnUserEmpInfo(&pMemberInfo);
//		}
//#endif
//	}

	SetCircle(false);

	//m_pDlgChatInput->SetWindowPos(m_pDlgChatRight.get(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnAddUser.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnSendFile.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnVideo.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnGroupShare.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnChatFunc.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnExitChat.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnMin.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnMax.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_btnClose.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	//SetTimer(TIMER_UPDATEWINDOW, 5, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
//#ifdef USES_FRAME_LIST
//#else
//	CEbDialogBase::OnOK();
//#endif
}

void CDlgDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
//#ifdef USES_FRAME_LIST
//#else
//	CEbDialogBase::OnCancel();
//#endif
}

void CDlgDialog::OnDestroy()
{
	//ExitRoom(0);
	//if (m_pChatRoom.get())
	//{
	//	m_pChatRoom->SetMsgHwnd(NULL);
	//	m_pChatRoom->Stop();
	//	m_pChatRoom.reset();
	//}
	//UnregisterHotKey(this->m_hWnd,0x2013); 
	CEbDialogBase::OnDestroy();
	m_wndSplitter.DestroyWindow();
	if (m_pDlgChatInput.get() != NULL)
	{
		m_pDlgChatInput->DestroyWindow();
		m_pDlgChatInput.reset();
	}
	//m_pChatInfo.reset();
	if (m_pDlgChatRight.get() != NULL)
	{
		m_pDlgChatRight->DestroyWindow();
		m_pDlgChatRight.reset();
	}
	m_pEbCallInfo.reset();
	//m_pCallInfo.reset();
	//m_pFromAccountInfo.reset();
	m_menuRemoteDesktop.DestroyMenu();
	m_menuFunc.DestroyMenu();
	m_menuState.DestroyMenu();

	if (m_pDlgSelectUser.GetSafeHwnd()!=NULL)
	{
		m_pDlgSelectUser.DestroyWindow();
	}
	//theEBRemoteDesktop.Release();
	if (m_pFromImage!=NULL)
	{
		delete m_pFromImage;
		m_pFromImage = NULL;
	}

	if (m_bDeleteThis)
		delete this;
}

void CDlgDialog::DoResize(int delta)
{
	//CSplitterControl::ChangePos(&m_BtnSelfClear, 0, delta);
	//CSplitterControl::ChangePos(&m_chkSelfAutoRoll, 0, delta);
	CSplitterControl::ChangeWidth(m_pDlgChatInput.get(), delta, CW_LEFTALIGN);
	CSplitterControl::ChangeWidth(m_pDlgChatRight.get(), -delta, CW_RIGHTALIGN);
	Invalidate();
	//SetTimer(TIMER_UPDATEWINDOW, 5, NULL);
}

LRESULT CDlgDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STA_SPLITTER)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
				DoResize(pHdr->delta);
		}
	}
	return CEbDialogBase::DefWindowProc(message, wParam, lParam);
}

void CDlgDialog::MoveSize(int cx, int cy)
{
	CRect rectChatRight;
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->GetWindowRect(&rectChatRight);
		this->ScreenToClient(&rectChatRight);
	}
	CRect rectSplitter;
	if (m_wndSplitter.GetSafeHwnd())
	{
		m_wndSplitter.GetWindowRect(&rectSplitter);
		rectSplitter.right = rectSplitter.left+2;
		this->ScreenToClient(&rectSplitter);
	}

	const bool bIsDepDialog = m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0;
	const int const_toolbar_width	= m_btnSendFile.GetImgWidth();	// 60
	const int const_toolbar_height	= m_btnSendFile.GetImgHeight();	// 30
	const int const_left_intever = 1;
	int x = const_left_intever;
	int y = 50;	// 72
	if (!bIsDepDialog || (m_nGroupType != EB_GROUP_TYPE_DEPARTMENT && m_nGroupType != EB_GROUP_TYPE_PROJECT))
	{
		m_btnAddUser.MovePoint(x, y);
		x += const_toolbar_width;
	}
	
	if (!bIsDepDialog || !theApp.GetDisableGroupSharedCloud())	// 单聊会话，或者没有禁用群共享
	{
		m_btnSendFile.MovePoint(x, y);
		x += const_toolbar_width;
	}
	if (!theApp.GetDisableVideo())
	{
		m_btnVideo.MovePoint(x, y);
		x += const_toolbar_width;
	}
#ifdef USES_REMOTE_DESKTOP
	if (!bIsDepDialog && !theApp.GetDisableRD())
	{
		m_btnRemoteDesktop.MovePoint(x, y);
		x += const_toolbar_width;
	}
#endif
	if (bIsDepDialog && !theApp.GetDisableGroupSharedCloud())
	{
		m_btnGroupShare.MovePoint(x, y);
		x += const_toolbar_width;
	}
	// 应用功能
	//if (!theApp.IsLogonVisitor())
	if (m_nChatFuncSize>0)
	{
		m_btnChatFunc.MovePoint(x, y);
		x += const_toolbar_width;
	}
	if (!bIsDepDialog || (m_nGroupType != EB_GROUP_TYPE_DEPARTMENT && m_nGroupType != EB_GROUP_TYPE_PROJECT))
	{
		m_btnExitChat.MovePoint(x, y);
		x += const_toolbar_width;
	}
	y += (const_toolbar_height);
	const int const_dlg_height = cy-y-const_left_intever;
	int userlist_width = 0;
	//if (bIsDepDialog && m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd())
	//{
	//	userlist_width = 150;
	//	m_pDlgUserList->MoveWindow(const_left_intever,y,userlist_width,const_dlg_height);
	//	m_pDlgUserList->ShowWindow(SW_SHOW);
	//	userlist_width += 5;	// 5为间隔
	//}
	const int const_chatright_left = cx-rectChatRight.Width();
	const int const_splitter_left = const_chatright_left-rectSplitter.Width();
	const int const_chatinput_width = const_splitter_left-const_left_intever-userlist_width;
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->MoveWindow(const_chatright_left, y, rectChatRight.Width(), const_dlg_height);
	}
	if (m_wndSplitter.GetSafeHwnd())
	{
		m_wndSplitter.MoveWindow(const_splitter_left, y, rectSplitter.Width(), const_dlg_height);
		m_wndSplitter.SetRange(100,cx-100);
	}
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->MoveWindow(const_left_intever+userlist_width, y, const_chatinput_width, const_dlg_height-2);	// -2解决左下角空白问题
	}
	//SetTimer(TIMER_UPDATEWINDOW, 5, NULL);
}

void CDlgDialog::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_offset = 0;
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-const_offset;
	m_btnClose.MovePoint(btnx, const_offset);
	btnx -= m_btnMin.GetImgWidth();
	m_btnMax.MovePoint(btnx, const_offset);
	btnx -= m_btnMin.GetImgWidth();
	m_btnMin.MovePoint(btnx, const_offset);

	MoveSize(cx, cy);
}

//void CDlgDialog::OnEnterRoom(void)
//{
//	const std::string sLogonAccount = theApp.GetLogonAccount();
//	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
//	{
//		m_pDlgUserList->OnEnterUser(sLogonAccount.c_str());
//	}
//}
#ifdef USES_EBCOM_TEST
void CDlgDialog::OnUserEnterRoom(IEB_ChatAccountInfo* pAccountInfo)
{
	const eb::bigint nFromUserId = pAccountInfo->UserId;
	const CEBString sFromInfo = pAccountInfo->FromInfo.GetBSTR();
	this->m_pDlgChatRight->OnEnterUser(nFromUserId, sFromInfo.c_str());
	//if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	//{
	//	m_pDlgUserList->OnEnterUser(sFromAccount.c_str());
	//}
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnUserEnter(nFromUserId, sFromInfo);
	}
}
#else
void CDlgDialog::OnUserEnterRoom(const CCrAccountInfo* pAccountInfo)
{
	this->m_pDlgChatRight->OnEnterUser(pAccountInfo->m_sAccount, pAccountInfo->m_sFromInfo.c_str());
	//if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	//{
	//	m_pDlgUserList->OnEnterUser(sFromAccount);
	//}
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnUserEnter(pAccountInfo->m_sAccount, pAccountInfo->m_sFromInfo);
	}
	// ??
	//size_t nUserSize = m_pEbCallInfo->m_pChatRoom->GetUserSize();
	//if (nUserSize > 1)
	//{
	//	m_btnSendFile.EnableWindow(TRUE);
	//}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnUserExitRoom(IEB_ChatAccountInfo* pAccountInfo)
{
	UserExitRoom(pAccountInfo->UserId,false);
}
#else
void CDlgDialog::OnUserExitRoom(const CCrAccountInfo* pAccountInfo)
{
	UserExitRoom(pAccountInfo->m_sAccount,false);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnReceiveRich(IEB_ChatRichInfo* pCrMsgInfo,CString* sOutFirstMsg)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnReceiveRich(pCrMsgInfo,sOutFirstMsg);
		if (this->IsWindowVisible())
			::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
	}
}
#else
void CDlgDialog::OnReceiveRich(const CCrRichInfo * pCrMsgInfo,CString* sOutFirstMsg)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnReceiveRich(pCrMsgInfo,sOutFirstMsg);
		if (this->IsWindowVisible())
			::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnSendRich(IEB_ChatRichInfo* pCrMsgInfo,EB_STATE_CODE nState)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnSendRich(pCrMsgInfo,nState,NULL	);
	}
}
#else
void CDlgDialog::OnSendRich(const CCrRichInfo * pCrMsgInfo,EB_STATE_CODE nState)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnSendRich(pCrMsgInfo,nState,NULL);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnSendingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		const tstring sFileName = libEbc::GetFileName(CEBString(pCrFileInfo->FileName.GetBSTR()));
		CString sWindowText;
		sWindowText.Format(_T("发送文件：%s"),sFileName.c_str());
		m_pDlgChatInput->AddLineString(pCrFileInfo->MsgId,sWindowText);
		//sWindowText.Format(_T("发送文件：<a href=\"%s\" />%s</a>..."), pChatRoomFileInfo->m_sFileName.c_str(), sFileName.c_str());
		//m_pDlgChatInput->AddMessage(CDlgChatInput::OT_SLEF,sWindowText,TRUE);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->OnSendingFile(pCrFileInfo);
	}
}
#else
void CDlgDialog::OnSendingFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
		CString sWindowText;
		sWindowText.Format(_T("发送文件：%s"),sFileName.c_str());
		m_pDlgChatInput->AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
		//sWindowText.Format(_T("发送文件：<a href=\"%s\" />%s</a>..."), pChatRoomFileInfo->m_sFileName.c_str(), sFileName.c_str());
		//m_pDlgChatInput->AddMessage(CDlgChatInput::OT_SLEF,sWindowText,TRUE);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->OnSendingFile(pCrFileInfo);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnSentFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnSentFile(pCrFileInfo);

		if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
		{
			const eb::bigint sReceiveAccount = pCrFileInfo->ReceiveAccount;
			if (sReceiveAccount==theApp.GetLogonUserId())
			{
				const eb::bigint nMsgId = pCrFileInfo->MsgId;
				const eb::bigint sSendFrom = pCrFileInfo->SendFrom;
				const CEBString sFileName = pCrFileInfo->FileName.GetBSTR();
				CEBString sSaveDbToAccount;// = pCrFileInfo->SendTo.GetBSTR();
				if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
					sSaveDbToAccount = m_pEbCallInfo->m_pFromAccountInfo.GetAccount();
				CString sSql;
				sSql.Format(_T("INSERT INTO msg_record_t(msg_id,dep_code,from_uid,to_uid,private,msg_type,msg_text) ")\
					_T("VALUES(%lld,%lld,%lld,%lld,%d,%d,'%s')"),
					nMsgId,m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom,
					sSaveDbToAccount,pCrFileInfo->Private?1:0,MRT_FILE,libEbc::ACP2UTF8(sFileName.c_str()).c_str());
				theApp.m_pBoUsers->execute(sSql);
			}
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->MsgId);
	}
	if (this->GetParent()->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::OnSentFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnSentFile(pCrFileInfo);

		if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
		{
			if (pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
			{
				eb::bigint sSaveDbToAccount = pCrFileInfo->m_sSendTo;
				if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
					sSaveDbToAccount = m_pEbCallInfo->m_pFromAccountInfo.GetUserId();
				CString sSql;
				sSql.Format(_T("INSERT INTO msg_record_t(msg_id,dep_code,from_uid,to_uid,private,msg_type,msg_text) ")\
					_T("VALUES(%lld,%lld,%lld,%lld,%d,%d,'%s')"),
					pCrFileInfo->m_nMsgId,m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sSendFrom,
					sSaveDbToAccount,pCrFileInfo->m_bPrivate?1:0,MRT_FILE,libEbc::ACP2UTF8(pCrFileInfo->m_sFileName.c_str()).c_str());
				theApp.m_pBoUsers->execute(sSql);
			}
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
	}
	if (this->GetParent()->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnCancelFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->MsgId);
	}
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		const eb::bigint sReceiveAccount(pCrFileInfo->ReceiveAccount);
		const tstring sFileName = libEbc::GetFileName(CEBString(pCrFileInfo->FileName.GetBSTR()));
		CString sWindowText;
		if (pCrFileInfo->SendFrom!=theApp.GetLogonUserId() || sReceiveAccount==0 || sReceiveAccount==theApp.GetLogonUserId())
			sWindowText.Format(_T("取消文件：%s"),sFileName.c_str());
		else if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方拒绝接收文件：%s"),sFileName.c_str());
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sReceiveAccount).GetBSTR();
			sWindowText.Format(_T("%s 拒绝接收文件：%s"),sMemberName.c_str(),sFileName.c_str());
		}
		m_pDlgChatInput->AddLineString(pCrFileInfo->MsgId,sWindowText);

		//sWindowText.Format(_T("用户取消文件：<a href=\"%s\" />%s</a>"), pCrFileInfo->m_sFileName.c_str(), sFileName.c_str());
		//m_pDlgChatInput->AddMessage(CDlgChatInput::OT_SLEF,sWindowText,TRUE);
	}
}
#else
void CDlgDialog::OnCancelFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
	}
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
		CString sWindowText;
		if (pCrFileInfo->m_sSendFrom!=theApp.GetLogonUserId() || pCrFileInfo->m_sReceiveAccount==0 || pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
			sWindowText.Format(_T("取消文件：%s"),sFileName.c_str());
		else if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方拒绝接收文件：%s"),sFileName.c_str());
		else
		{
			CEBString sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sReceiveAccount,sMemberName);
			sWindowText.Format(_T("%s 拒绝接收文件：%s"),sMemberName.c_str(),sFileName.c_str());
		}
		m_pDlgChatInput->AddLineString(pCrFileInfo->m_nMsgId,sWindowText);

		//sWindowText.Format(_T("用户取消文件：<a href=\"%s\" />%s</a>"), pCrFileInfo->m_sFileName.c_str(), sFileName.c_str());
		//m_pDlgChatInput->AddMessage(CDlgChatInput::OT_SLEF,sWindowText,TRUE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo,CString* sOutFirstMsg)
{
	const eb::bigint sSendFrom = pCrFileInfo->SendFrom;
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		const tstring sFileName = libEbc::GetFileName(CEBString(pCrFileInfo->FileName.GetBSTR()));
		CString sWindowText;
		if (m_pEbCallInfo->m_bOffLineUser || pCrFileInfo->OffFile==VARIANT_TRUE)
		//if (m_pEbCallInfo->m_pCallInfo->m_bOffLineCall)
		//if (m_bReceiveOffLineMsg)
		{
			if (this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom).GetBSTR();
				sWindowText.Format(_T("%s 发送离线文件：%s"),sMemberName.c_str(),sFileName.c_str());
			}else
				sWindowText.Format(_T("对方发送离线文件：%s"),sFileName.c_str());
			m_pDlgChatInput->AddLineString(pCrFileInfo->MsgId,sWindowText);
		}else
		{
			if (this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom).GetBSTR();
				sWindowText.Format(_T("%s 发送文件：%s"),sMemberName.c_str(),sFileName.c_str());
			}else
				sWindowText.Format(_T("对方发送文件：%s"),sFileName.c_str());
			m_pDlgChatInput->AddLineString(pCrFileInfo->MsgId,sWindowText);
		}
		if (sOutFirstMsg!=NULL)
		{
			const bool bIsDepDialog = m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0;
			if (bIsDepDialog)
				sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pEbCallInfo->m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("接收文件").c_str());
			else
				sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pEbCallInfo->m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("接收文件").c_str());
		}
		m_pDlgChatRight->OnReceivingFile(pCrFileInfo);
		if (this->IsWindowVisible())
			::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
	}
}
#else
void CDlgDialog::OnReceivingFile(const CCrFileInfo * pCrFileInfo,CString* sOutFirstMsg)
{
	const eb::bigint sSendFrom = pCrFileInfo->m_sSendFrom;
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
		CString sWindowText;
		if (m_pEbCallInfo->m_bOffLineUser || pCrFileInfo->m_bOffFile)
		//if (m_pEbCallInfo->m_pCallInfo->m_bOffLineCall)
		//if (m_bReceiveOffLineMsg)
		{
			if (this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				CEBString sMemberName;
				theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom,sMemberName);
				sWindowText.Format(_T("%s 发送离线文件：%s"),sMemberName.c_str(),sFileName.c_str());
			}else
				sWindowText.Format(_T("对方发送离线文件：%s"),sFileName.c_str());
			m_pDlgChatInput->AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
		}else
		{
			if (this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				CEBString sMemberName;
				theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom,sMemberName);
				sWindowText.Format(_T("%s 发送文件：%s"),sMemberName.c_str(),sFileName.c_str());
			}else
				sWindowText.Format(_T("对方发送文件：%s"),sFileName.c_str());
			m_pDlgChatInput->AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
		}
		if (sOutFirstMsg!=NULL)
		{
			const bool bIsDepDialog = m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0;
			if (bIsDepDialog)
				sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pEbCallInfo->m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("接收文件").c_str());
			else
				sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pEbCallInfo->m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("接收文件").c_str());
		}
		m_pDlgChatRight->OnReceivingFile(pCrFileInfo);
		if (this->IsWindowVisible())
			::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnReceivedFile(pCrFileInfo);

		if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
		{
			const eb::bigint sSendFrom = pCrFileInfo->SendFrom;
			const eb::bigint nMsgId = pCrFileInfo->MsgId;
			const CEBString sFileName = pCrFileInfo->FileName.GetBSTR();
			CEBString sFromName;
			eb::bigint sSaveDbToAccount = pCrFileInfo->SendTo;
			if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			{
				//sSaveDbToAccount = m_pEbCallInfo->m_pFromAccountInfo.GetAccount();
				sFromName = m_sFromName;
			}else
			{
				sFromName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom).GetBSTR();
			}
			CString sSql;
			sSql.Format(_T("INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) ")\
				_T("VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')"),
				nMsgId,m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
				sSaveDbToAccount,pCrFileInfo->Private?1:0,MRT_FILE,libEbc::ACP2UTF8(sFileName.c_str()).c_str());
			theApp.m_pBoUsers->execute(sSql);
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->OnReceivedFile(pCrFileInfo);
	}
	::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::OnReceivedFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->OnReceivedFile(pCrFileInfo);

		if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
		{
			tstring sFromName;
			eb::bigint sSaveDbToAccount = pCrFileInfo->m_sSendTo;
			if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			{
				//sSaveDbToAccount = m_pEbCallInfo->m_pFromAccountInfo.GetUserId();	// ??
				sFromName = m_sFromName;
			}else
			{
				theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sSendFrom,sFromName);
			}
			CString sSql;
			sSql.Format(_T("INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) ")\
				_T("VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')"),
				pCrFileInfo->m_nMsgId,m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
				sSaveDbToAccount,pCrFileInfo->m_bPrivate?1:0,MRT_FILE,libEbc::ACP2UTF8(pCrFileInfo->m_sFileName.c_str()).c_str());
			theApp.m_pBoUsers->execute(sSql);
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->OnReceivedFile(pCrFileInfo);
	}
	::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->SetFilePercent(pChatRoomFilePercent);
	}
}
#else
void CDlgDialog::OnFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->SetFilePercent(pChatRoomFilePercent);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnSave2CloutDrive(IEB_ChatFileInfo* pCrFileInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		const CEBString sFileName = pCrFileInfo->FileName.GetBSTR();
		CString sWindowText;
		if (nStateValue==0)
		{
			sWindowText.Format(_T("《%s》已经成功存入个人云盘！"),sFileName.c_str());
		}else
		{
			sWindowText.Format(_T("《%s》存入个人云盘失败（%d）！"),sFileName.c_str(),nStateValue);
		}
		m_pDlgChatInput->AddLineString(pCrFileInfo->MsgId,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		if (nStateValue==0)
			m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->MsgId);
	}
}
#else
void CDlgDialog::OnSave2CloutDrive(const CCrFileInfo * pCrFileInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
		CString sWindowText;
		if (nStateValue==0)
		{
			sWindowText.Format(_T("《%s》已经成功存入个人云盘！"),sFileName.c_str());
		}else
		{
			sWindowText.Format(_T("《%s》存入个人云盘失败（%d）！"),sFileName.c_str(),nStateValue);
		}
		m_pDlgChatInput->AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		if (nStateValue==0)
			m_pDlgChatRight->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::RDRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("邀请对方视频通话..."));
		else
			sWindowText.Format(_T("打开本地视频加入视频会议..."));
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VRequestResponse(pVideoInfo,nStateValue);
	}
}
#else
void CDlgDialog::RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			if (pVideoInfo->m_nRDType == EB_RD_DESKTOP_SRC)
				sWindowText.Format(_T("邀请对方远程协助..."));
			else
				sWindowText.Format(_T("请求控制对方电脑..."));
		}else
			sWindowText.Format(_T("共享本端远程桌面..."));
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDRequestResponse(pVideoInfo,nStateValue);
	}
}
#endif


#ifdef USES_EBCOM_TEST
void CDlgDialog::RDAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			CString sWindowText;
			sWindowText.Format(_T("接受对方视频通话，视频通话中..."));
			m_pDlgChatInput->AddLineString(0,sWindowText);
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VAckResponse(pVideoInfo,nStateValue);
	}
}
#else
void CDlgDialog::RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			CString sWindowText;
			if (pVideoInfo->m_nRDType == EB_RD_DESKTOP_SRC)
				sWindowText.Format(_T("接受对方远程协助，正在连接..."));
			else
				sWindowText.Format(_T("接受对方控制我的电脑，正在连接..."));
			m_pDlgChatInput->AddLineString(0,sWindowText);
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDAckResponse(pVideoInfo,nStateValue);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::RDRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方邀请视频通话..."));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 打开视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoRequest(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::RDRequest(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			if (pVideoInfo->m_nRDType == EB_RD_DESKTOP_SRC)
				sWindowText.Format(_T("对方邀请远程协助..."));
			else
				sWindowText.Format(_T("对方请求控制你的电脑..."));
		}else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pVideoInfo->m_nFromUserId,sMemberName);
			sWindowText.Format(_T("%s 打开远程共享桌面..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDRequest(pVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::RDAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方接受视频通话，视频通话中..."));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 打开我的视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoAccept(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::RDAccept(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			if (pVideoInfo->m_nRDType == EB_RD_DESKTOP_SRC)
				sWindowText.Format(_T("对方接受远程协助，正在连接..."));
			else
				sWindowText.Format(_T("对方接受你控制他的电脑，正在连接..."));
		}else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pVideoInfo->m_nFromUserId,sMemberName);
			sWindowText.Format(_T("%s 打开共享桌面..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDAccept(pVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方拒绝视频通话"));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 退出视频会议"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoCancel(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::RDCancel(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			sWindowText.Format(_T("对方拒绝远程桌面请求"));
		}else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pVideoInfo->m_nFromUserId,sMemberName);
			sWindowText.Format(_T("%s 退出共享桌面"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDCancel(pVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("视频通话结束"));
		else if (theApp.GetLogonUserId()==pUserVideoInfo->UserId)
			sWindowText.Format(_T("关闭本地视频会议视频"));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 关闭视频会议视频"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoEnd(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::RDEnd(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("远程桌面结束"));
		else if (theApp.GetLogonUserId()==pVideoInfo->m_nFromUserId)
			sWindowText.Format(_T("关闭本地共享桌面"));
		else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pVideoInfo->m_nFromUserId,sMemberName);
			sWindowText.Format(_T("%s 关闭远程桌面"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->RDEnd(pVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("邀请对方视频通话..."));
		else
			sWindowText.Format(_T("打开本地视频加入视频会议..."));
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VRequestResponse(pVideoInfo,nStateValue);
	}
}
#else
void CDlgDialog::VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("邀请对方视频通话..."));
		else
			sWindowText.Format(_T("打开本地视频加入视频会议..."));
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VRequestResponse(pVideoInfo,nStateValue);
	}
}
#endif


#ifdef USES_EBCOM_TEST
void CDlgDialog::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			CString sWindowText;
			sWindowText.Format(_T("接受对方视频通话，视频通话中..."));
			m_pDlgChatInput->AddLineString(0,sWindowText);
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VAckResponse(pVideoInfo,nStateValue);
	}
}
#else
void CDlgDialog::VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			CString sWindowText;
			sWindowText.Format(_T("接受对方视频通话，视频通话中..."));
			m_pDlgChatInput->AddLineString(0,sWindowText);
		}else if (nStateValue==EB_STATE_OK)
		{
			bool pVideoProcessing = false;
			bool pFileProcessing = false;
			bool pDesktopProcessing = false;
			m_pDlgChatRight->GetProcessing(pVideoProcessing,pFileProcessing,pDesktopProcessing);
			if (!pVideoProcessing)
			{
				CString sWindowText;
				sWindowText.Format(_T("成功申请视频会议，可以“打开视频”加入视频会议，然后“退出会议”！"));
				m_pDlgChatInput->AddLineString(0,sWindowText);
			}
		}
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VAckResponse(pVideoInfo,nStateValue);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方邀请视频通话..."));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 打开视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoRequest(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::VideoRequest(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方邀请视频通话..."));
		else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->m_sUserAccount,sMemberName);
			sWindowText.Format(_T("%s 打开视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoRequest(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方接受视频通话，视频通话中..."));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 打开我的视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoAccept(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::VideoAccept(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方接受视频通话，视频通话中..."));
		else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->m_sUserAccount,sMemberName);
			sWindowText.Format(_T("%s 打开我的视频会议视频..."),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoAccept(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方拒绝视频通话"));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 退出视频会议"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoCancel(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::VideoCancel(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方拒绝视频通话"));
		else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->m_sUserAccount,sMemberName);
			sWindowText.Format(_T("%s 退出视频会议"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoCancel(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoTimeout(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("请求视频通话超时"));
		else
		{
			//const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			//sWindowText.Format(_T("%s 退出视频会议"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoCancel(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::VideoTimeout(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("请求视频通话超时"));
		else
		{
			//std::string sMemberName;
			//theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->m_sUserAccount,sMemberName);
			//sWindowText.Format(_T("%s 退出视频会议"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoCancel(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("视频通话结束"));
		else if (theApp.GetLogonUserId()==pUserVideoInfo->UserId)
			sWindowText.Format(_T("关闭本地视频会议视频"));
		else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->UserId).GetBSTR();
			sWindowText.Format(_T("%s 关闭视频会议视频"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoEnd(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#else
void CDlgDialog::VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		CString sWindowText;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("视频通话结束"));
		else if (theApp.GetLogonUserId()==pUserVideoInfo->m_sUserAccount)
			sWindowText.Format(_T("关闭本地视频会议视频"));
		else
		{
			std::string sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pUserVideoInfo->m_sUserAccount,sMemberName);
			sWindowText.Format(_T("%s 关闭视频会议视频"),sMemberName.c_str());
		}
		m_pDlgChatInput->AddLineString(0,sWindowText);
	}
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->VideoEnd(pVideoInfo,pUserVideoInfo);
	}
	if (this->IsWindowVisible())
		::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}
#endif


//void CDlgDialog::ShowDialog(void)
//{
//	if (!this->IsWindowVisible())
//	{
//		this->ShowWindow(SW_SHOW);
//		SetTimer(TIMER_UPDATEWINDOW, 5, NULL);
//	}
//}
#ifdef USES_EBCOM_TEST
void CDlgDialog::ChangeDepartmentInfo(IEB_GroupInfo* pGroupInfo)
{
	this->Invalidate();
	//if (m_pDlgUserList)
	//{
	//	// ??
	//	//m_pDlgUserList->ChangeDepartmentInfo(pGroupInfo);
	//}
}
#else
void CDlgDialog::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	this->Invalidate();
	//if (m_pDlgUserList)
	//{
	//	// ??
	//	//m_pDlgUserList->ChangeDepartmentInfo(pGroupInfo);
	//}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgDialog::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo)
{
	if (m_pDlgSelectUser.GetSafeHwnd()!=NULL)
		m_pDlgSelectUser.onMemberInfo(pMemberInfo);

	if (m_pDlgChatRight.get()!=NULL && pMemberInfo->GroupCode==this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode)
	{
		m_pDlgChatRight->OnUserEmpInfo(pMemberInfo);
	}
}
#else
void CDlgDialog::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo)
{
	if (m_pDlgSelectUser.GetSafeHwnd()!=NULL)
		m_pDlgSelectUser.onMemberInfo(pMemberInfo);

	if (m_pDlgChatRight.get()!=NULL && pMemberInfo->m_sGroupCode==this->m_pEbCallInfo->m_pCallInfo.m_sGroupCode)
	{
		m_pDlgChatRight->OnUserEmpInfo(pMemberInfo);
	}
}
#endif
void CDlgDialog::OnRemoveGroup(eb::bigint nGroupId)
{
	if (m_pDlgSelectUser.GetSafeHwnd()!=NULL)
		m_pDlgSelectUser.OnRemoveGroup(nGroupId);
}
void CDlgDialog::OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId)
{
	if (m_pDlgSelectUser.GetSafeHwnd()!=NULL)
		m_pDlgSelectUser.OnRemoveMember(nGroupId, nMemberId);
}

void CDlgDialog::OnAlertingCall(void)
{
	if (m_pDlgChatInput.get()!=NULL)
	{
		m_pDlgChatInput->OnAlertingCall();
	}
}
//void CDlgDialog::OnCall2Group(void)
//{
//	// 调整界面
//	CRect rect;
//	this->GetWindowRect(rect);
//	if (DEFAULT_P2P_WIDTH==rect.Width())
//	{
//		rect.right = rect.left+DEFAULT_DEP_WIDTH;
//		this->MoveWindow(&rect);
//		this->SetCircle();
//	}
//	MoveSize(rect.Width(),rect.Height());
//}

void CDlgDialog::UserExitRoom(eb::bigint nUserId, bool bExitDep)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd() != NULL)
	{
		m_pDlgChatRight->OnExitUser(nUserId,bExitDep);
	}
	bool bIsDepCall = m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0;
	if (bIsDepCall)
	{

	}else
	{
		if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
		{
			m_pDlgChatInput->OnUserExit(nUserId);
		}
		// ??
		//size_t nUserSize = m_pEbCallInfo->m_pChatRoom->GetUserSize();
		//if (nUserSize <= 1)
		//{
		//	m_btnSendFile.EnableWindow(FALSE);
		//}
	}
}

//void CDlgDialog::ReceiveMsg(CChatMsgInfo::pointer pChatMsgInfo)
//{
//	if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
//	{
//		m_pDlgChatInput->ReceiveMsg(pChatMsgInfo);
//		::FlashWindow(this->GetSafeHwnd(), TRUE);
//	}
//}


void CDlgDialog::OnClose()
{
	if (m_pDlgChatRight.get()!=NULL)
	{
		bool pVideoProcessing = false;
		bool pFileProcessing = false;
		bool pDesktopProcessing = false;
		m_pDlgChatRight->GetProcessing(pVideoProcessing,pFileProcessing,pDesktopProcessing);
		if (pVideoProcessing && pFileProcessing)
		{
			if (CDlgMessageBox::EbDoModal(this,"正在视频通话，和传输文件：",_T("确定现在退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
			{
				return;
			}
		}else if (pVideoProcessing)
		{
			if (CDlgMessageBox::EbDoModal(this,"正在视频通话：",_T("确定现在退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
			{
				return;
			}
		}else if (pFileProcessing)
		{
			if (CDlgMessageBox::EbDoModal(this,"正在传输文件：",_T("确定现在退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
			{
				return;
			}
		}
		if (pDesktopProcessing)
		{
			if (CDlgMessageBox::EbDoModal(this,"正在远程桌面：",_T("确定现在退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
			{
				return;
			}
		}
		m_pDlgChatRight->ExitChat(false);
	}
	this->GetParent()->PostMessage(EBWM_FRAME_WND_CLOSE,(WPARAM)this,0);

#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallExit(this->m_pEbCallInfo->m_pCallInfo.GetCallId());
#else
	theEBAppClient.EB_CallExit(this->m_pEbCallInfo->m_pCallInfo.GetCallId());
#endif
	theApp.m_pCallList.remove(this->m_pEbCallInfo->m_pCallInfo.GetCallId());
	//CEbDialogBase::OnClose();
	this->EndDialog(IDOK);
}

void CDlgDialog::OnBnClickedButtonAddUser()
{
	std::vector<std::string> pExistUserList;

	bool bExistGroup = false;
#ifdef USES_EBCOM_TEST
	short nGroupType = -1;
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
	{
		nGroupType = theEBClientCore->EB_GetGroupType(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		if (nGroupType >= 0)
			bExistGroup = true;
	}
#else
	EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		bExistGroup = theEBAppClient.EB_GetGroupType(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,nGroupType);
#endif
	// ????加离线用户
	if (bExistGroup)
	{
		switch (nGroupType)
		{
		case EB_GROUP_TYPE_DEPARTMENT:
		case EB_GROUP_TYPE_PROJECT:
			{
				// 部门及项目组，不能随意添加成员
				return;
			}break;
		default:
			{
#ifdef USES_EBCOM_TEST
				_variant_t pMemberList = theEBClientCore->EB_GetGroupMemberAccountList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
				if (pMemberList.vt!=VT_EMPTY && pMemberList.parray != NULL)
				{
					CComSafeArray<VARIANT> m_sa(pMemberList.parray);
					for (ULONG i=0;i<m_sa.GetCount();i++)
					{
						CComVariant var = m_sa.GetAt(i);
						if (var.vt != VT_BSTR)
							continue;
						const CEBString sMemberAccount(var.bstrVal);
						pExistUserList.push_back(sMemberAccount);
					}
				}
#else
				theEBAppClient.EB_GetGroupMemberAccountList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pExistUserList);
#endif
			}break;
		}
	}else
	{

#ifdef USES_EBCOM_TEST
		_variant_t pMemberList = theEBClientCore->EB_GetCallAccountList(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		if (pMemberList.vt!=VT_EMPTY && pMemberList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pMemberList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_BSTR)
					continue;
				const CEBString sMemberAccount(var.bstrVal);
				pExistUserList.push_back(sMemberAccount);
			}
		}
#else
		theEBAppClient.EB_GetCallAccountList(m_pEbCallInfo->m_pCallInfo.GetCallId(),pExistUserList);
#endif
	}

	AFX_SWITCH_INSTANCED();
	//CDlgSelectUser pDlg(this);
	if (m_pDlgSelectUser.GetSafeHwnd()==NULL)
	{
		m_pDlgSelectUser.Create(CDlgSelectUser::IDD,this);
	}else if (m_pDlgSelectUser.IsWindowVisible())
	{
		return;
	}
	m_pDlgSelectUser.ShowWindow(SW_SHOW);
	for (size_t i=0; i<pExistUserList.size(); i++)
	{
		m_pDlgSelectUser.m_pSelectedTreeItem.insert(pExistUserList[i],0,false);
	}
	const INT_PTR nResponse = m_pDlgSelectUser.RunModalLoop();
	m_pDlgSelectUser.ShowWindow(SW_HIDE);
	if (nResponse == IDOK)
	//if (pDlg.DoModal() == IDOK)
	{
		//if (m_pEbCallInfo->m_pCallInfo->m_sGroupCode.empty())
		//	m_pEbCallInfo->m_pCallInfo->m_sGroupCode = m_pEbCallInfo->m_pCallInfo->GetCallId();	// 自动生成临时群组
		// ??
		BoostReadLock rdlock(m_pDlgSelectUser.m_pSelectedTreeItem.mutex());
		//boost::mutex::scoped_lock lock(pDlg.m_pSelectedTreeItem.mutex());
		CLockMap<tstring,eb::bigint>::const_iterator pIter = m_pDlgSelectUser.m_pSelectedTreeItem.begin();
		for (; pIter!=m_pDlgSelectUser.m_pSelectedTreeItem.end(); pIter++)
		{
			const tstring sSelAccount = pIter->first;
			const eb::bigint sEmpCode = pIter->second;
			// 判断是否已经存在该会话
			bool bExistAccount = false;
			for (size_t i=0; i<pExistUserList.size(); i++)
			{
				if (pExistUserList[i] == sSelAccount)
				{
					bExistAccount = true;
					break;
				}
			}
			if (!bExistAccount)
			{
				if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
				{
#ifdef USES_EBCOM_TEST
					theEBClientCore->EB_Call2Group(m_pEbCallInfo->m_pCallInfo.GetCallId(),sSelAccount.c_str());
#else
					theEBAppClient.EB_Call2Group(m_pEbCallInfo->m_pCallInfo.GetCallId(),sSelAccount.c_str());
#endif
				}else if (sEmpCode>0)
				{
#ifdef USES_EBCOM_TEST
					theEBClientCore->EB_CallMember(sEmpCode,m_pEbCallInfo->m_pCallInfo.GetCallId());
#else
					theEBAppClient.EB_CallMember(sEmpCode,m_pEbCallInfo->m_pCallInfo.GetCallId());
#endif
				}else
				{
#ifdef USES_EBCOM_TEST
					theEBClientCore->EB_CallAccount(sSelAccount.c_str(),m_pEbCallInfo->m_pCallInfo.GetCallId());
#else
					theEBAppClient.EB_CallAccount(sSelAccount.c_str(),m_pEbCallInfo->m_pCallInfo.GetCallId());
#endif
				}
				//theEBAppClient.CallDep(m_pEbCallInfo->m_pCallInfo->m_sGroupCode,sSelAccount.c_str(),sEmpCode);
			}
		}
	}
	m_pDlgSelectUser.ResetSelected();

/*
	CDlgDiaRecord pDlgDiaRecord(this);
	if (pDlgDiaRecord.DoModal() == IDOK)
	{
		CString sCallee = pDlgDiaRecord.m_sCallNumber;
		theEBAppClient.CallUser(sCallee, m_pCallInfo->GetCallId().c_str());
	}*/

	//CDlgSelectUser* pDlgTemp = m_pDlgSelectUser;
	//m_pDlgSelectUser = NULL;
	//pDlgTemp->DestroyWindow();
	//delete pDlgTemp;

	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
	theApp.InvalidateParentRect(&m_btnAddUser);
}

void CDlgDialog::OnBnClickedButtonSendFile()
{
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0 && theApp.GetDisableGroupSharedCloud())
	{
		CDlgMessageBox::EbMessageBox(this,_T("禁用群共享："),_T("请使用单聊会话窗口发送文件！"),CDlgMessageBox::IMAGE_WARNING,3);
		return;
	}

	CFileDialog dlg(TRUE, 0, 0, OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, this);
	const DWORD MAXFILE = 30 * 256;
	dlg.m_ofn.nMaxFile = MAXFILE;
	char* pc = new char[MAXFILE];
	dlg.m_ofn.lpstrFile = pc;
	dlg.m_ofn.lpstrFile[0] = NULL;
	if (dlg.DoModal() == IDOK)
	{
		POSITION pos = dlg.GetStartPosition(); 
		while (pos)
		{
			const CString sPathName = dlg.GetNextPathName(pos);
//			if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
//			{
//#ifdef USES_EBCOM_TEST
//				theEBClientCore->EB_AddTempGroupFileRes((LPCTSTR)sPathName,"",m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
//#else
//				theEBAppClient.EB_AddTempGroupFileRes(sPathName,"",m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
//#endif
//			}else if (this->m_pDlgChatInput != NULL)
			{
				m_pDlgChatInput->SendFile(sPathName,false);
			}
		}
	}
	delete []pc;
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
	theApp.InvalidateParentRect(&m_btnSendFile);
}
void CDlgDialog::OnBnClickedButtonVideo()
{
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_VideoAck(m_pEbCallInfo->m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
		theEBAppClient.EB_VideoAck(m_pEbCallInfo->m_pCallInfo.GetCallId(),true,0);
#endif
		//CDlgVideoConference::pointer pDlgDialog = theApp.GetDlgVideoConference(this->m_pEbCallInfo);
		//pDlgDialog->SetForegroundWindow();
		//CDlgMessageBox::EbDoModal(this,_T("当前版本不支持视频会议："),_T("请联系恩布公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_VideoRequest(m_pEbCallInfo->m_pCallInfo.GetCallId(), EB_VIDEO_BOTH);
#else
	theEBAppClient.EB_VideoRequest(m_pEbCallInfo->m_pCallInfo.GetCallId(), EB_VIDEO_BOTH);
#endif
}

void CDlgDialog::OnTimer(UINT_PTR nIDEvent)
{
	//switch (nIDEvent)
	//{
	//case TIMER_UPDATEWINDOW:
	//	{
	//		KillTimer(nIDEvent);
	//		if (m_pDlgChatInput != NULL	&& m_pDlgChatInput->GetSafeHwnd())
	//		{
	//			m_pDlgChatInput->ShowWindow(SW_HIDE);				// 可以解决刷新问题
	//			m_pDlgChatInput->ShowWindow(SW_SHOW);
	//		}
	//	}break;
	//default:
	//	break;
	//}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgDialog::OnPaint()
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
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}
}


void CDlgDialog::OnDraw(void)
{
	DrawInfo();
}
//const Gdiplus::ColorMatrix theColorMatrix =
//{0.3, 0.3, 0.3, 0.0, 0.0,
//    0.59, 0.59, 0.59, 0.0, 0.0,
//    0.11, 0.11, 0.11, 0.0, 0.0,
//    0.0, 0.0, 0.0, 1.0, 0.0,
//	0.0, 0.0, 0.0, 0.0, 1.0};
void CDlgDialog::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	// 50+35=85
	this->AddBgDrawInfo(CEbBackDrawInfo(85,theDefaultBgColorBgLight3,false));	// 0.90
	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	Gdiplus::Graphics graphics(memDC.m_hDC);

	//CEBString sImagePath;// = theApp.GetAppPath()+_T("\\img\\myimg.png");
	USES_CONVERSION;
//	CEBString sTitle;
//	Image * pImage = NULL;
//	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_ONLINE_NEW;
//	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
//	{
//		if (m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR)
//		{
//			pImage = theApp.m_imageDefaultVisitor->Clone();
//			sTitle =  _T("游客") + m_pEbCallInfo->m_pFromAccountInfo.GetAccount();
//		}else
//		{
//			sTitle = m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName;
//#ifdef USES_EBCOM_TEST
//			CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode);
//			if (pMemberInfo != NULL)
//			{
//				pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
//				sDescription = pMemberInfo->Description.GetBSTR();
//				const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
//				if (PathFileExists(sMemberHeadFile.c_str()))
//				{
//					pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sMemberHeadFile.c_str()));
//					//sImagePath = sMemberHeadFile;
//				}
//			}
//#else
//			EB_MemberInfo pMemberInfo;
//			if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode))
//			{
//				pOutLineState = pMemberInfo.m_nLineState;
//				sDescription = pMemberInfo.m_sDescription;
//				if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
//				{
//					//sImagePath = pMemberInfo.m_sHeadResourceFile;
//					pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(pMemberInfo.m_sHeadResourceFile.c_str()));
//				}
//			}
//#endif
//		}
//	}else
//	{
//		EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
//		if (pGroupInfo != NULL)
//		{
//			sTitle = pGroupInfo->GroupName.GetBSTR();
//			sDescription = pGroupInfo->Description.GetBSTR();
//			nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
//		}
//#else
//		EB_GroupInfo pGroupInfo;
//		if (theEBAppClient.EB_GetGroupInfo(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,&pGroupInfo))
//		{
//			sTitle = pGroupInfo.m_sGroupName;
//			sDescription = pGroupInfo.m_sDescription;
//			nGroupType = pGroupInfo.m_nGroupType;
//		}
//#endif
//		if (nGroupType==EB_GROUP_TYPE_DEPARTMENT)
//		{
//			pImage = theApp.m_imageDefaultDepartment->Clone();
//		}else if (nGroupType==EB_GROUP_TYPE_PROJECT)
//		{
//			pImage = theApp.m_imageDefaultProject->Clone();
//		}else if (nGroupType==EB_GROUP_TYPE_GROUP)
//		{
//			pImage = theApp.m_imageDefaultGroup->Clone();
//		}else// if (nGroupType==EB_GROUP_TYPE_TEMP)
//		{
//			pImage = theApp.m_imageDefaultTempGroup->Clone();
//		}
//
////#ifdef USES_EBCOM_TEST
////		const CEBString sGroupName = theEBClientCore->EB_GetGroupName(m_pEbCallInfo->m_pCallInfo.m_sGroupCode).GetBSTR();
////		if (!sGroupName.empty())
////		{
////			sTitle = sGroupName.c_str();
////		}
////#else
////		CEBString sGroupName;
////		if (theEBAppClient.EB_GetGroupName(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sGroupName))
////		{
////			sTitle = sGroupName.c_str();
////		}
////#endif
//	}
//	if (pImage == NULL)
//	{
//		pImage = theApp.m_imageDefaultMember->Clone();
//	}
	//const int const_head_top = 18;
	//const int const_head_size = 42;
	//Gdiplus::Rect destRect(15, 18, const_head_size, const_head_size);
	//if (pOutLineState == EB_LINE_STATE_OFFLINE)
	//{
	//	// 离线
	//	Gdiplus::ImageAttributes pImageAttributes;
	//	pImageAttributes.SetColorMatrix(&theColorMatrix,ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
	//	graphics.DrawImage(pImage,Gdiplus::Rect(15, 18, const_head_size, const_head_size),0,0, pImage->GetWidth(), pImage->GetHeight(),Gdiplus::UnitPixel,&pImageAttributes);
	//}else
	//{
	//	graphics.DrawImage(pImage, destRect);
	//}
	//delete pImage;
	//Gdiplus::Image * pImage2=NULL;
	//if (pOutLineState == EB_LINE_STATE_BUSY)
	//{
	//	pImage2 = theApp.m_imageStateBusy->Clone();
	//}else if (pOutLineState == EB_LINE_STATE_AWAY)
	//{
	//	pImage2 = theApp.m_imageStateAway->Clone();
	//}
	//if (pImage2!=NULL)
	//{
	//	destRect.X += (const_head_size-pImage2->GetWidth());
	//	destRect.Y += (const_head_size-pImage2->GetHeight());
	//	destRect.Width = pImage2->GetWidth();
	//	destRect.Height = pImage2->GetHeight();
	//	graphics.DrawImage(pImage2, destRect);
	//	delete pImage2;
	//}

	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	const Gdiplus::Font fontEbDescription(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
	//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	Gdiplus::PointF pointTitle(7,5);
	graphics.DrawString(A2W_ACP(m_sFromName.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	if (!m_sFromDescription.empty())
	{
		pointTitle.Y += 22;
		graphics.DrawString(A2W_ACP(m_sFromDescription.c_str()),-1,&fontEbDescription,pointTitle,&brushEbTitle);
	}

	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//CRect rectClient;
	//this->GetClientRect(&rectClient);
	//HDC m_hdcMemory = dc.m_hDC;

	//// 写标题
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//CString sOutText = _T("苏东坡－软件研发部");
	//TextOut(m_hdcMemory, 6, 5, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

	//// MY IMG
	//Gdiplus::Graphics graphics(m_hdcMemory);    
	//CString sImagePath = theApp.GetAppPath()+_T("\\img\\myimg.png");
	//USES_CONVERSION;
	//Gdiplus::Image m_img(A2W_ACP(sImagePath));    
	//graphics.DrawImage(&m_img, POS_ADIMG_LEFT, POS_ADIMG_TOP, POS_ADIMG_SIGE, POS_ADIMG_SIGE);

	//// 写字
	//CFont pNewFontAd;
	//pNewFontAd.CreatePointFont(150, _T("宋体"));//创建显示文本的字体
	//pOldFond = SelectObject(m_hdcMemory, pNewFontAd.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));
	//sOutText = _T("李白");
	//int texty = POS_ADIMG_TOP+15;
	//TextOut(m_hdcMemory, POS_ADIMG_LEFT+POS_ADIMG_SIGE, texty, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

	//texty += 28;
	//CFont * pFontSta = this->GetFont();
	//pOldFond = SelectObject(m_hdcMemory, pFontSta->m_hObject);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//sOutText = _T("打破常规 挑战自我");
	//TextOut(m_hdcMemory, POS_ADIMG_LEFT+POS_ADIMG_SIGE, texty, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

}

void CDlgDialog::OnBnClickedButtonMin()
{
	this->GetParent()->PostMessage(EBWM_FRAME_WND_MIN,(WPARAM)this,0);
}

void CDlgDialog::OnBnClickedButtonMax()
{
	this->GetParent()->PostMessage(EBWM_FRAME_WND_MAX,(WPARAM)this,0);
}

void CDlgDialog::OnBnClickedButtonClose()
{
	//PostMessage(EB_MSG_EXIT_CHAT, 0, 0);
	this->PostMessage(WM_CLOSE, 0, 0);
}

void CDlgDialog::OnBnClickedButtonExitChat()
{
	PostMessage(EB_MSG_EXIT_CHAT, 1, 0);
}

void CDlgDialog::OnBnClickedButtonChatFunc()
{
	//if (theApp.IsLogonVisitor())
	//	return;
	if (m_menuFunc.GetSafeHmenu()==NULL)
	{
		m_menuFunc.CreatePopupMenu();
	}else
	{
		while (m_menuFunc.GetMenuItemCount()>0)
			m_menuFunc.RemoveMenu(0,MF_BYPOSITION);
	}

	// 应用功能菜单
	const EB_FUNC_LOCATION nFuncLocation = m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0?EB_FUNC_LOCATION_USER_CHAT_BTN1:EB_FUNC_LOCATION_GROUP_CHAT_BTN1;
	theApp.ClearSubscribeSelectInfo();
	theApp.m_pSubscribeFuncList.clear();
#ifdef USES_EBCOM_TEST
	_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(nFuncLocation);
	if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
			var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
			if (pSubscribeFuncInfo == NULL) continue;
			theApp.m_pSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
		}
	}
#else
	theEBAppClient.EB_GetSubscribeFuncList(nFuncLocation,theApp.m_pSubscribeFuncList);
#endif
	if (!theApp.m_pSubscribeFuncList.empty())
	{
		for (size_t i=0;i<theApp.m_pSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[i];
			m_menuFunc.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
		}
	}else
	{
		m_menuFunc.AppendMenu(MF_BYCOMMAND,0,_T("没有集成应用"));
		m_menuFunc.EnableMenuItem(0,MF_GRAYED);
	}

	CPoint point;
	GetCursorPos(&point);
	m_menuFunc.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}

LRESULT CDlgDialog::OnMessageSelectedEmp(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)wParam;
	if (this->m_pDlgChatInput != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->SelectedEmp(pMemberInfo);
	}
#else
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wParam;
	if (this->m_pDlgChatInput != NULL && m_pDlgChatInput->GetSafeHwnd())
	{
		m_pDlgChatInput->SelectedEmp(pMemberInfo);
	}
#endif
	return 0;
}

LRESULT CDlgDialog::OnMessageEBSC(WPARAM wParam, LPARAM lParam)
{
	CString sParameter;
	sParameter.Format(_T("\"\" %d %d\""),(int)EB_MSG_EBSC_OK,(int)this->GetSafeHwnd());
	theApp.RunEBSC(sParameter);
	return 0;
}
LRESULT CDlgDialog::OnMessageEBSCOK(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox(_T("OnMessageEBSCOK"));
	if (this->m_pDlgChatInput != NULL && m_pDlgChatInput->GetSafeHwnd())
		m_pDlgChatInput->SetScreenCopyFinished();
	return 0;
}

LRESULT CDlgDialog::OnMessageMsgRecord(WPARAM wParam, LPARAM lParam)
{
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		const EB_AccountInfo & pFromAccountInfo = m_pEbCallInfo->m_pFromAccountInfo;
		//if (pFromAccountInfo.get() != NULL)
		{
			bIsAccount = true;
			sId = pFromAccountInfo.GetUserId();
			if (!m_pEbCallInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName.empty())
				sName = pFromAccountInfo.m_pFromCardInfo.m_sName;
			else
				sName = pFromAccountInfo.GetUserName();
		//}else
		//{
		//	return 1;
		}
	}else
	{
		sId = m_pEbCallInfo->m_pCallInfo.m_sGroupCode;
		char lpszId[24];
		sprintf(lpszId,"%lld",sId);
		sName = lpszId;
#ifdef USES_EBCOM_TEST
		const CEBString sGroupName = theEBClientCore->EB_GetGroupName(m_pEbCallInfo->m_pCallInfo.m_sGroupCode).GetBSTR();
		if (!sGroupName.empty())
		{
			sName = sGroupName;
		}
#else
		CEBString sGroupName;
		if (theEBAppClient.EB_GetGroupName(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,sGroupName))
			sName = sGroupName;
#endif
	}
	CDlgMsgRecord * pDlgMsgRecord = NULL;
	if (!theApp.m_pMsgRecord.find(sId,pDlgMsgRecord))
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = this;
		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgMsgRecord = new CDlgMsgRecord(pParent);
		pDlgMsgRecord->Create(CDlgMsgRecord::IDD,pParent);
		theApp.m_pMsgRecord.insert(sId,pDlgMsgRecord);
	}
	CRect rect;
	this->GetWindowRect(&rect);
	const int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度 
	rect.left = (m_nScreenWidth-520)/2;	// rect.right;
	rect.right = rect.left + 520;
	//rect.left = rect.right-100;
	//rect.right = rect.left + 520;
	pDlgMsgRecord->MoveWindow(&rect);
	pDlgMsgRecord->SetCircle();
	if (bIsAccount)
		pDlgMsgRecord->LoadAccountMsgRecord(sId,sName);
	else
		pDlgMsgRecord->LoadDepartmentMsgRecord(sId,sName);
	//pDlgMsgRecord->SetForegroundWindow();

	return 0;
	//if (m_pDlgChatRight.get()!=NULL)
	//{
	//	m_pDlgChatRight->OnMsgRecord();
	//}
	//return 0;
}

LRESULT CDlgDialog::OnMessageExitChat(WPARAM wParam, LPARAM lParam)
{
	const bool bHangup = wParam==1?true:false;
	if (bHangup)
	{
		// 需要挂断会话
		const eb::bigint sSelDepCode = m_pEbCallInfo->m_pCallInfo.m_sGroupCode;
		if (sSelDepCode==0)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallHangup(m_pEbCallInfo->m_pCallInfo.GetCallId());
#else
			theEBAppClient.EB_CallHangup(m_pEbCallInfo->m_pCallInfo.GetCallId());
#endif
			this->PostMessage(WM_CLOSE, 0, 0);
		}else
		{
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(sSelDepCode);
			if (pGroupInfo == NULL)
			{
				this->PostMessage(WM_CLOSE, 0, 0);
			}else
			{
				const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
				const eb::bigint nCreateUserId = pGroupInfo->CreateUserId;
				if (pGroupInfo->GroupType == EB_GROUP_TYPE_DEPARTMENT ||
					pGroupInfo->GroupType == EB_GROUP_TYPE_PROJECT)
				{
					// 部门，项目组
					this->PostMessage(WM_CLOSE, 0, 0);
				}else if (nCreateUserId == theApp.GetLogonUserId())
				{
					// 部门创建者，不能退出
					CString sText;
					sText.Format(_T("你确定解散[%s]吗？"), sGroupName.c_str());
					CString sTitle;
					if (pGroupInfo->GroupType == EB_GROUP_TYPE_GROUP)
						sTitle = _T("你是群组创建者，退出将会解散：");
					else
						sTitle = _T("你是讨论组创建者，退出将会解散：");
					if (CDlgMessageBox::EbDoModal(this,sTitle,sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
					{
						theEBClientCore->EB_DeleteGroup(sSelDepCode);
						this->PostMessage(WM_CLOSE, 0, 0);
					}
				}else
				{
					CString sText;
					sText.Format(_T("[%s]吗？"), sGroupName.c_str());
					if (CDlgMessageBox::EbDoModal(this,"你确定退出：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
					//if (MessageBox(sText, _T("退出"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
					{
						theEBClientCore->EB_ExitGroup(sSelDepCode);
						this->PostMessage(WM_CLOSE, 0, 0);
					}
				}
			}
		}
#else
			EB_GroupInfo pGroupInfo;
			if (!theEBAppClient.EB_GetGroupInfo(sSelDepCode,&pGroupInfo))
			{
				this->PostMessage(WM_CLOSE, 0, 0);
			}else if (pGroupInfo.m_nGroupType == EB_GROUP_TYPE_DEPARTMENT ||
				pGroupInfo.m_nGroupType == EB_GROUP_TYPE_PROJECT)
			{
				// 部门，项目组
				this->PostMessage(WM_CLOSE, 0, 0);
			}else if (pGroupInfo.m_nCreateUserId == theEBAppClient.EB_GetLogonUserId())
			{
				// 部门创建者，不能退出
				CString sText;
				sText.Format(_T("你确定解散[%s]吗？"), pGroupInfo.m_sGroupName.c_str());
				CString sTitle;
				if (pGroupInfo.m_nGroupType == EB_GROUP_TYPE_GROUP)
					sTitle = _T("你是群组创建者，退出将会解散：");
				else
					sTitle = _T("你是讨论组创建者，退出将会解散：");
				if (CDlgMessageBox::EbDoModal(this,sTitle,sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
				{
					theEBAppClient.EB_DeleteGroup(sSelDepCode);
					this->PostMessage(WM_CLOSE, 0, 0);
				}
			}else
			{
				CString sText;
				sText.Format(_T("[%s]吗？"), pGroupInfo.m_sGroupName.c_str());
				if (CDlgMessageBox::EbDoModal(this,"你确定退出：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
				//if (MessageBox(sText, _T("退出"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
				{
					theEBAppClient.EB_ExitGroup(sSelDepCode);
					this->PostMessage(WM_CLOSE, 0, 0);
				}
			}
		}
#endif
	}else
	{
		// 不需要挂断，退出即可
		//int nOnlineSize = theEBAppClient.EB_GetCallOnlineSize(m_pEbCallInfo->m_pCallInfo.GetCallId());
		//const std::string sSelDepCode = m_pEbCallInfo->m_pCallInfo.m_sGroupCode;
		//if (sSelDepCode.empty() && (nOnlineSize <= 1))	// ?? || m_pEbCallInfo->m_pCallInfo->m_nCallState!=EB_CALL_STATE_CONNECTED))
		//{
		//	theEBAppClient.EB_CallHangup(m_pEbCallInfo->m_pCallInfo.GetCallId());
		//}
		this->PostMessage(WM_CLOSE, 0, 0);
	}
	return 0;
}

void CDlgDialog::OnDropFiles(HDROP hDropInfo)
{
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0 && theApp.GetDisableGroupSharedCloud())
	{
		CDlgMessageBox::EbMessageBox(this,_T("禁用群共享："),_T("请使用单聊会话窗口发送文件！"),CDlgMessageBox::IMAGE_WARNING,3);
		return;
	}

	const int nFileSum = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);   //得到文件个数
	for (int i=0; i<nFileSum; i++)
	{
		char lpszFile[MAX_PATH];
		memset(lpszFile,0,MAX_PATH);
		DragQueryFile(hDropInfo,i,lpszFile,MAX_PATH);         //得到文件名
		//			if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		//			{
		//#ifdef USES_EBCOM_TEST
		//				theEBClientCore->EB_AddTempGroupFileRes(lpszFile,"",m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		//#else
		//				theEBAppClient.EB_AddTempGroupFileRes(lpszFile,"",m_pEbCallInfo->m_pCallInfo.m_sGroupCode);
		//#endif
		//			}else if (this->m_pDlgChatInput != NULL)
		{
			m_pDlgChatInput->SendFile(lpszFile,true);
		}
	}
	CEbDialogBase::OnDropFiles(hDropInfo);
}

void CDlgDialog::ScrollToEnd(void)
{
	if (m_pDlgChatInput.get()!=NULL)
		m_pDlgChatInput->ScrollToEnd();
}

void CDlgDialog::SetBtnMax(bool bMax)
{
	if (m_btnMax.GetSafeHwnd()==NULL || this->GetSafeHwnd()==NULL)
		return;
	if (bMax)
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
		m_btnMax.Load(IDB_PNG_MAX);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.Load(IDB_PNG_RESTORE);
	}
}

void CDlgDialog::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
	OnMove();
}

void CDlgDialog::OnBnClickedButtonGroupShare()
{
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0 || theApp.GetDisableGroupSharedCloud()) return;
	theApp.OpenGroupShareWindow(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,this);
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
	theApp.InvalidateParentRect(&m_btnGroupShare);
}

BOOL CDlgDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//if (pMsg->message == WM_KEYDOWN)// && pMsg->wParam == VK_SNAPSHOT)// && ((pMsg->lParam&(1<<31))==0))
	//{
	//	//if (GetKeyState(VK_CONTROL)&0x80)
	//	//{
	//	//	if(  pMsg->wParam  == 'P')  
	//	//		this->PostMessage(EB_MSG_EBSC);
	//	//}
	//}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

BOOL CDlgDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return TRUE;
	return CEbDialogBase::OnEraseBkgnd(pDC);
}

void CDlgDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (theApp.IsLogonVisitor())
	{
		CEbDialogBase::OnRButtonUp(nFlags, point);
		return;
	}

	const EB_FUNC_LOCATION nFuncLocation = m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0?EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER_CHAT:EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP_CHAT;
	CMenu pPopupMenu;
	pPopupMenu.CreatePopupMenu();
	theApp.ClearSubscribeSelectInfo();
#ifdef USES_EBCOM_TEST
	_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(nFuncLocation);
	if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
			var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
			if (pSubscribeFuncInfo == NULL) continue;
			theApp.m_pSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
		}
	}
#else
	theEBAppClient.EB_GetSubscribeFuncList(nFuncLocation,theApp.m_pSubscribeFuncList);
#endif
	if (!theApp.m_pSubscribeFuncList.empty())
	{
		for (size_t i=0;i<theApp.m_pSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[i];
			pPopupMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
		}

		CPoint point;
		GetCursorPos(&point);
		pPopupMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
	}

	CEbDialogBase::OnRButtonUp(nFlags, point);
}
void CDlgDialog::OnSubscribeFunc(UINT nID)
{
	size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		theApp.m_nSelectCallId = m_pEbCallInfo->m_pCallInfo.GetCallId();
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo);
	}
}
void CDlgDialog::OnMove(void)
{
	if (m_pDlgChatRight.get() != NULL && m_pDlgChatRight->GetSafeHwnd())
	{
		m_pDlgChatRight->OnMove();
	}
}

void CDlgDialog::LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	if (m_pDlgChatRight.get()!=NULL)
	{
		m_pDlgChatRight->LineStateChange(nUserId, bLineState);
	}
}

void CDlgDialog::ExitRD(void)
{
	if (m_pDlgChatRight.get()!=NULL)
	{
		m_pDlgChatRight->ExitRD();
	}
}
void CDlgDialog::SetCallInfo(const CEBCCallInfo::pointer& pCallInfo)
{
	m_pEbCallInfo = pCallInfo;
	if (m_pDlgChatInput.get()!=NULL)
	{
		m_pDlgChatInput->SetCallInfo(m_pEbCallInfo->m_pCallInfo,m_pEbCallInfo->m_pFromAccountInfo);
		//m_pDlgChatInput->m_pCallInfo = m_pEbCallInfo->m_pCallInfo;
		//m_pDlgChatInput->m_pFromAccountInfo = m_pEbCallInfo->m_pFromAccountInfo;
	}
	if (m_pDlgChatRight.get()!=NULL)
	{
		m_pDlgChatRight->SetCallInfo(m_pEbCallInfo->m_pCallInfo,m_pEbCallInfo->m_pFromAccountInfo);
	}
}


void CDlgDialog::OnBnClickedButtonRd()
{
	if (m_menuRemoteDesktop.GetSafeHmenu()==NULL)
	{
		m_menuRemoteDesktop.CreatePopupMenu();
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
		{
			m_menuRemoteDesktop.AppendMenu(MF_BYCOMMAND,EB_COMMAND_RD_CONTROL_DEST,_T("请求控制对方电脑"));
			m_menuRemoteDesktop.AppendMenu(MF_BYCOMMAND,EB_COMMAND_RD_CONTROL_ME,_T("邀请对方远程协助"));
		}else
		{
			m_menuRemoteDesktop.AppendMenu(MF_BYCOMMAND,EB_COMMAND_RD_CONTROL_ME,_T("共享本端远程桌面"));
		}
	}
	CPoint point;
	GetCursorPos(&point);
	m_menuRemoteDesktop.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}
void CDlgDialog::OnRdControlMe(void)
{
#ifdef USES_EBCOM_TEST

#else
	theEBAppClient.EB_RDRequest(m_pEbCallInfo->m_pCallInfo.GetCallId(), EB_RD_DESKTOP_SRC);
#endif
}
void CDlgDialog::OnRdControlDest(void)
{
	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
	{
		CDlgMessageBox::EbDoModal(this,_T("当前版本不支持多人共享远程桌面："),_T("请联系恩布公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
#ifdef USES_EBCOM_TEST

#else
	theEBAppClient.EB_RDRequest(m_pEbCallInfo->m_pCallInfo.GetCallId(), EB_RD_DESKTOP_DEST);
#endif
}
void CDlgDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}
