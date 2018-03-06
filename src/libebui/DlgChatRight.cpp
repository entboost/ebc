// DlgChatRight.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgChatRight.h"
#include "DlgUserInfo.h"
#include "PanelVideos.h"
#include "PanelRemoteDesktop.h"
#include "PanelFiles.h"
#include "DlgUserList.h"

// CDlgChatRight dialog

IMPLEMENT_DYNAMIC(CDlgChatRight, CEbDialogBase)

CDlgChatRight::CDlgChatRight(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgChatRight::IDD, pParent)
	, m_pDlgUserList(NULL)
	, m_pDlgUserInfo(NULL)
	, m_pPanVideos(NULL)
	, m_pPanRemoteDesktop(NULL)
	, m_pPanFiles(NULL)
	, m_hCurrentSelect(NULL)

{

}

CDlgChatRight::~CDlgChatRight()
{
}

void CDlgChatRight::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_USERLIST, m_labelUserList);
	DDX_Control(pDX, IDC_BUTTON_ECARD, m_labelECard);
	DDX_Control(pDX, IDC_BUTTON_VIDEOS, m_labelVideos);
	DDX_Control(pDX, IDC_BUTTON_RD, m_labelRemoteDesktop);
	DDX_Control(pDX, IDC_BUTTON_FILES, m_labelFiles);
}


BEGIN_MESSAGE_MAP(CDlgChatRight, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ECARD, &CDlgChatRight::OnBnClickedButtonEcard)
	ON_BN_CLICKED(IDC_BUTTON_VIDEOS, &CDlgChatRight::OnBnClickedButtonVideos)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_USERLIST, &CDlgChatRight::OnBnClickedButtonUserlist)
	ON_BN_CLICKED(IDC_BUTTON_FILES, &CDlgChatRight::OnBnClickedButtonFiles)
	ON_BN_CLICKED(IDC_BUTTON_RD, &CDlgChatRight::OnBnClickedButtonRd)
END_MESSAGE_MAP()

void CDlgChatRight::SetCtrlColor(void)
{
	m_labelUserList.SetDrawPanel(true,theApp.GetBgColor3(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_labelECard.SetDrawPanel(true,theApp.GetBgColor3(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_labelVideos.SetDrawPanel(true,theApp.GetBgColor3(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_labelRemoteDesktop.SetDrawPanel(true,theApp.GetBgColor3(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_labelFiles.SetDrawPanel(true,theApp.GetBgColor3(),theApp.GetBgColor2(),theApp.GetBgColor2());

	if (m_pDlgUserList!=NULL && m_pDlgUserList->GetSafeHwnd()!=NULL)
		m_pDlgUserList->SetCtrlColor();
	if (m_pPanRemoteDesktop!=NULL && m_pPanRemoteDesktop->GetSafeHwnd()!=NULL)
		m_pPanRemoteDesktop->SetCtrlColor();
	if (m_pPanVideos!=NULL && m_pPanVideos->GetSafeHwnd()!=NULL)
		m_pPanVideos->SetCtrlColor();
}

// CDlgChatRight message handlers

BOOL CDlgChatRight::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//this->SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetMouseMove(FALSE);

	m_labelUserList.SetAutoSize(false);
	m_labelUserList.SetWindowText(_T("群成员"));
	//m_labelUserList.Load(IDB_PNG_TAB_75X30);

	m_labelUserList.SetDrawPanelRgn(false);

	m_labelECard.SetAutoSize(false);
	m_labelECard.SetWindowText(_T("电子名片"));
	//m_labelECard.Load(IDB_PNG_TAB_75X30);
	m_labelECard.SetDrawPanelRgn(false);
	m_labelVideos.SetAutoSize(false);
	m_labelVideos.SetWindowText(_T("视频面板"));
	m_labelVideos.ShowWindow(theApp.GetDisableVideo()?SW_HIDE:SW_SHOW);
	//m_labelVideos.Load(IDB_PNG_TAB_75X30);
	m_labelVideos.SetDrawPanelRgn(false);
	m_labelRemoteDesktop.SetAutoSize(false);
	m_labelRemoteDesktop.SetWindowText(_T("远程桌面"));
	m_labelRemoteDesktop.SetDrawPanelRgn(false);
	m_labelRemoteDesktop.ShowWindow(theApp.GetDisableRD()?SW_HIDE:SW_SHOW);
	//m_labelRemoteDesktop.ShowWindow(SW_HIDE);
	m_labelFiles.SetAutoSize(false);
	m_labelFiles.SetWindowText(_T("文件传输"));
	//m_labelFiles.Load(IDB_PNG_TAB_75X30);
	m_labelFiles.SetDrawPanelRgn(false);
	m_labelFiles.ShowWindow(SW_HIDE);

	SetCtrlColor();

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

	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_labelUserList.ShowWindow(SW_HIDE);
		m_labelUserList.SetChecked(false);
		m_labelECard.ShowWindow(SW_SHOW);
		m_labelECard.SetChecked(true);
		OnBnClickedButtonEcard();
	}else
	{
		m_labelUserList.ShowWindow(SW_SHOW);
		m_labelUserList.SetChecked(true);
		m_labelECard.ShowWindow(SW_HIDE);
		m_labelECard.SetChecked(false);
		OnBnClickedButtonUserlist();
	}

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChatRight::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_Tab_Width	= 75;//m_labelECard.GetImgWidth();
	const int const_Tab_Height	= 24;//m_labelECard.GetImgHeight();
	int x=0;
	m_labelUserList.MovePoint(x,0,const_Tab_Width,const_Tab_Height);
	m_labelECard.MovePoint(x, 0,const_Tab_Width,const_Tab_Height);
	x += const_Tab_Width;
	if (!theApp.GetDisableVideo())
	{
		m_labelVideos.MovePoint(x, 0,const_Tab_Width,const_Tab_Height);
		x += const_Tab_Width;
	}
	if (!theApp.GetDisableRD())
	{
		m_labelRemoteDesktop.MovePoint(x, 0,const_Tab_Width,const_Tab_Height);
		x += const_Tab_Width;
	}
	m_labelFiles.MovePoint(x,0,const_Tab_Width,const_Tab_Height);
	
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	{
		m_pDlgUserList->MoveWindow(0, const_Tab_Height, cx, cy-const_Tab_Height);
	}
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->MoveWindow(0, const_Tab_Height, cx, cy-const_Tab_Height);
	}
	if (m_pPanVideos != NULL && m_pPanVideos->GetSafeHwnd() != NULL)
	{
		m_pPanVideos->MoveWindow(0, const_Tab_Height, cx, cy-const_Tab_Height);
	}
	if (m_pPanRemoteDesktop != NULL && m_pPanRemoteDesktop->GetSafeHwnd() != NULL)
	{
		m_pPanRemoteDesktop->MoveWindow(0, const_Tab_Height, cx, cy-const_Tab_Height);
	}
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->MoveWindow(0, const_Tab_Height, cx, cy-const_Tab_Height);
	}

}

void CDlgChatRight::OnDestroy()
{
	CEbDialogBase::OnDestroy();
	//m_pChatRoom.reset();
	//m_pCallInfo.reset();
	//m_pFromAccountInfo.reset();
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
void CDlgChatRight::OnEnterUser(eb::bigint nUserId, const char* sFromInfo)
{
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd() != NULL)
	{
		m_pDlgUserList->OnEnterUser(nUserId);
		//int nUserSize = m_pChatRoom->GetUserSize();
		//if (nUserSize > 2)
		//	m_pDlgUserList->ShowWindow(SW_SHOW);
	}
}

void CDlgChatRight::LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	if (m_pDlgUserList!=NULL)
		m_pDlgUserList->LineStateChange(nUserId, bLineState);
}

void CDlgChatRight::ExitRD(void)
{
	if (m_pPanRemoteDesktop!=NULL)
		m_pPanRemoteDesktop->ExitRD();
}
void CDlgChatRight::OnMove(void)
{
	if (m_pPanVideos!=NULL)
	{
		m_pPanVideos->OnMove();
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
void CDlgChatRight::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo)
{
	if (m_pDlgUserList)
	{
		m_pDlgUserList->OnUserEmpInfo(pMemberInfo);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::RDRequestResponse(EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequestResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequestResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VAckResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDAckResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoRequest(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDRequest(const EB_RemoteDesktopInfo* pVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDRequest(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDAccept(const EB_RemoteDesktopInfo* pVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDAccept(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoCancel(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDCancel(const EB_RemoteDesktopInfo* pVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDCancel(pVideoInfo);
}
#endif
#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->VideoEnd(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::RDEnd(const EB_RemoteDesktopInfo* pVideoInfo)
{
	OnBnClickedButtonRd();
	m_pPanRemoteDesktop->RDEnd(pVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VRequestResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VRequestResponse(pVideoInfo,nStateValue);
	//m_pPanVideos->SendMessage(EB_WM_V_REQUEST_RESPONSE,(WPARAM)pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VAckResponse(pVideoInfo,nStateValue);
}
#else
void CDlgChatRight::VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VAckResponse(pVideoInfo,nStateValue);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoRequest(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::VideoRequest(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoRequest(pVideoInfo,pUserVideoInfo);
	//m_pPanVideos->SendMessage(EB_WM_VIDEO_REQUEST,(WPARAM)pVideoInfo,(LPARAM)pUserVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::VideoAccept(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoAccept(pVideoInfo,pUserVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoCancel(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::VideoCancel(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoCancel(pVideoInfo,pUserVideoInfo);
}
#endif
#ifdef USES_EBCOM_TEST
void CDlgChatRight::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoEnd(pVideoInfo,pUserVideoInfo);
}
#else
void CDlgChatRight::VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo)
{
	OnBnClickedButtonVideos();
	m_pPanVideos->VideoEnd(pVideoInfo,pUserVideoInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnSendingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	OnBnClickedButtonFiles();
	m_pPanFiles->OnSendingFile(pCrFileInfo);
}
#else
void CDlgChatRight::OnSendingFile(const CCrFileInfo * pCrFileInfo)
{
	OnBnClickedButtonFiles();
	m_pPanFiles->OnSendingFile(pCrFileInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatRight::OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	OnBnClickedButtonFiles();
	m_pPanFiles->OnReceivingFile(pCrFileInfo);
}
#else
void CDlgChatRight::OnReceivingFile(const CCrFileInfo * pCrFileInfo)
{
	OnBnClickedButtonFiles();
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
			m_labelFiles.SetCheck(false);
			m_labelFiles.ShowWindow(SW_HIDE);
			if (this->m_pCallInfo.m_sGroupCode==0)
				OnBnClickedButtonEcard();
			else
				OnBnClickedButtonUserlist();
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


void CDlgChatRight::OpenVideoPan(void)
{
	if (m_pPanVideos==NULL)
	{
		AFX_SWITCH_INSTANCED();
		m_pPanVideos = new CPanelVideos(this);
		m_pPanVideos->SetCircle(false);
		m_pPanVideos->m_pCallInfo = m_pCallInfo;
		m_pPanVideos->Create(CPanelVideos::IDD, this);
	}
	CRect rect;
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
	}
	m_pPanVideos->MoveWindow(&rect);
	m_pPanVideos->ShowWindow(SW_SHOW);
}
void CDlgChatRight::OpenRdPan(void)
{
	if (m_pPanRemoteDesktop==NULL)
	{
		AFX_SWITCH_INSTANCED();
		m_pPanRemoteDesktop = new CPanelRemoteDesktop(this);
		m_pPanRemoteDesktop->SetCircle(false);
		m_pPanRemoteDesktop->m_pCallInfo = m_pCallInfo;
		m_pPanRemoteDesktop->Create(CPanelRemoteDesktop::IDD, this);
	}
	CRect rect;
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
	}
	m_pPanRemoteDesktop->MoveWindow(&rect);
	m_pPanRemoteDesktop->ShowWindow(SW_SHOW);
}
void CDlgChatRight::OpenFilePan(void)
{
	if (m_pPanFiles==NULL)
	{
		AFX_SWITCH_INSTANCED();
		m_pPanFiles = new CPanelFiles(this);
		m_pPanFiles->SetCircle(false);
		//m_pPanFiles->m_pChatRoom = m_pChatRoom;
		m_pPanFiles->Create(CPanelFiles::IDD, this);
		m_pPanFiles->ShowWindow(SW_SHOW);
	}
	CRect rect;
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd() != NULL)
	{
		m_pDlgUserInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
	}
	m_pPanFiles->MoveWindow(&rect);
	m_pPanFiles->ShowWindow(SW_SHOW);
}

void CDlgChatRight::Update(void)
{
	m_labelUserList.SetChecked(m_hCurrentSelect==m_labelUserList.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelUserList);
	m_labelECard.SetChecked(m_hCurrentSelect==m_labelECard.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelECard);
	if (!theApp.GetDisableVideo())
	{
		m_labelVideos.SetChecked(m_hCurrentSelect==m_labelVideos.GetSafeHwnd());
		theApp.InvalidateParentRect(&m_labelVideos);
	}
	if (!theApp.GetDisableRD())
	{
		m_labelRemoteDesktop.SetChecked(m_hCurrentSelect==m_labelRemoteDesktop.GetSafeHwnd());
		theApp.InvalidateParentRect(&m_labelRemoteDesktop);
	}
	m_labelFiles.SetChecked(m_hCurrentSelect==m_labelFiles.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelFiles);
	if (m_pDlgUserList != NULL && m_pDlgUserList->GetSafeHwnd())
	{
		m_pDlgUserList->ShowWindow(m_labelUserList.GetChecked()?SW_SHOW:SW_HIDE);
	}
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd())
	{
		m_pDlgUserInfo->ShowWindow(m_labelECard.GetChecked()?SW_SHOW:SW_HIDE);
	}
	if (!theApp.GetDisableVideo())
	{
		if (m_pPanVideos != NULL && m_pPanVideos->GetSafeHwnd())
			m_pPanVideos->ShowWindow(m_labelVideos.GetChecked()?SW_SHOW:SW_HIDE);
	}
	if (!theApp.GetDisableRD())
	{
		if (m_pPanRemoteDesktop != NULL && m_pPanRemoteDesktop->GetSafeHwnd())
			m_pPanRemoteDesktop->ShowWindow(m_labelRemoteDesktop.GetChecked()?SW_SHOW:SW_HIDE);
	}
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd())
		m_pPanFiles->ShowWindow(m_labelFiles.GetChecked()?SW_SHOW:SW_HIDE);
	// 刷新整个区域
	if (m_pDlgUserInfo != NULL && m_pDlgUserInfo->GetSafeHwnd())
		theApp.InvalidateParentRect(m_pDlgUserInfo);
	theApp.InvalidateParentRect(this);
}


void CDlgChatRight::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgChatRight::OnBnClickedButtonUserlist()
{
	m_hCurrentSelect = m_labelUserList.GetSafeHwnd();
	Update();
}

void CDlgChatRight::OnBnClickedButtonEcard()
{
	m_hCurrentSelect = m_labelECard.GetSafeHwnd();
	Update();
}

void CDlgChatRight::OnBnClickedButtonVideos()
{
	this->OpenVideoPan();
	m_hCurrentSelect = m_labelVideos.GetSafeHwnd();
	Update();
}
void CDlgChatRight::OnBnClickedButtonFiles()
{
	this->OpenFilePan();
	m_labelFiles.ShowWindow(SW_SHOW);
	m_hCurrentSelect = m_labelFiles.GetSafeHwnd();
	Update();
}

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
		this->AddBgDrawInfo(CEbBackDrawInfo(24,theDefaultBgColorBgLight3,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
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

void CDlgChatRight::OnBnClickedButtonRd()
{
	this->OpenRdPan();
	m_hCurrentSelect = m_labelRemoteDesktop.GetSafeHwnd();
	Update();
}
