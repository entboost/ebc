// PanelRemoteDesktop.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelRemoteDesktop.h"
//#include "Core/MediaDevice.h"


#define TIMER_ID_DO_VIDEO_CONNECTE		111
#define TIMER_ID_DO_VIDEO_DISCONNECTE	112
#define TIMERID_CHECK_CLOSE_VIDEO		113

//#define WM_BUILD_USER_VIEDO (WM_USER+10)

// CPanelRemoteDesktop dialog

IMPLEMENT_DYNAMIC(CPanelRemoteDesktop, CEbDialogBase)

CPanelRemoteDesktop::CPanelRemoteDesktop(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelRemoteDesktop::IDD, pParent)
	, m_nRequestUserId(0)
{
	m_bInVideoConference = false;
	m_bLocalOnDesktop = false;
	m_nFromUserId = 0;
	for (int i=0; i<MAX_RD_COUNT; i++)
	{
		m_pUserVideo[i] = NULL;
		m_nVideoUserId[i] = 0;
	}
	m_bInFRDRequest = false;
	m_nInDesktop = false;
}

CPanelRemoteDesktop::~CPanelRemoteDesktop()
{
}

void CPanelRemoteDesktop::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ON, m_btnVideoOn);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_END, m_btnVideoEnd);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_ACCEPT, m_btnVideoAccept);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_CANCEL, m_btnVideoCancel);
	DDX_Control(pDX, IDC_CHECK_ENABLE_CONTROL, m_bEnableControl);
}


BEGIN_MESSAGE_MAP(CPanelRemoteDesktop, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//ON_WM_CTLCOLOR()
	//ON_WM_PAINT()
//#ifndef USES_EBCOM_TEST
//	ON_MESSAGE(EB_WM_V_REQUEST_RESPONSE, OnMessageVRequestResponse)
//	ON_MESSAGE(EB_WM_V_ACK_RESPONSE, OnMessageVAckResponse)
//	ON_MESSAGE(EB_WM_VIDEO_REQUEST, OnMessageVideoRequest)
//	ON_MESSAGE(EB_WM_VIDEO_ACCEPT, OnMessageVideoAccept)
//	ON_MESSAGE(EB_WM_VIDEO_REJECT, OnMessageVideoCancel)
//	ON_MESSAGE(EB_WM_VIDEO_CLOSE, OnMessageVideoEnd)
//#endif
	//ON_MESSAGE(WM_BUILD_USER_VIEDO, OnMsgBuildUserVideo)

	ON_BN_CLICKED(IDC_BUTTON_VIDEO_END, &CPanelRemoteDesktop::OnBnClickedButtonVideoEnd)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_ACCEPT, &CPanelRemoteDesktop::OnBnClickedButtonVideoAccept)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_CANCEL, &CPanelRemoteDesktop::OnBnClickedButtonVideoCancel)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ON, &CPanelRemoteDesktop::OnBnClickedButtonOn)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_ENABLE_CONTROL, &CPanelRemoteDesktop::OnBnClickedCheckEnableControl)
END_MESSAGE_MAP()

void CPanelRemoteDesktop::SetCtrlColor(bool bInvalidate)
{
	m_bEnableControl.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableControl.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());

	//m_btnVideoOn.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnVideoOn.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	m_btnVideoAccept.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	m_btnVideoEnd.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	m_btnVideoCancel.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	if (bInvalidate)
		this->Invalidate();
}

BOOL CPanelRemoteDesktop::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	this->SetMouseMove(FALSE);
	//this->SetTransparentType(CEbDialogBase::TT_STATIC);

	m_btnVideoCancel.EnableWindow(TRUE);
	m_btnVideoEnd.EnableWindow(FALSE);
	m_btnVideoAccept.EnableWindow(FALSE);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoOn.ShowWindow(SW_HIDE);
		m_btnVideoCancel.EnableWindow(FALSE);
		m_btnVideoAccept.SetToolTipText(_T("接受远程桌面"));
		m_btnVideoEnd.SetToolTipText(_T("结束远程桌面 Shift + Alt + T"));
		m_btnVideoCancel.SetToolTipText(_T("拒绝远程桌面"));
	}else
	{
		m_btnVideoOn.SetWindowText(_T("打开桌面"));
		m_btnVideoOn.SetToolTipText(_T("打开本机桌面"));
		m_btnVideoEnd.SetWindowText(_T("关闭桌面"));
		m_btnVideoEnd.SetToolTipText(_T("关闭本机桌面"));
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.SetWindowText(_T("加入会议"));
		m_btnVideoCancel.SetToolTipText(_T("加入共享桌面"));

		// 暂时不支持群组共享桌面
		m_btnVideoOn.ShowWindow(SW_HIDE);
		m_btnVideoEnd.ShowWindow(SW_HIDE);
		m_btnVideoCancel.ShowWindow(SW_HIDE);
		m_bEnableControl.ShowWindow(SW_HIDE);
	}

	m_btnVideoOn.SetTextHotMove(false);
	//m_btnVideoOn.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoOn.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoAccept.SetTextHotMove(false);
	//m_btnVideoAccept.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoAccept.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoEnd.SetTextHotMove(false);
	//m_btnVideoEnd.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoEnd.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoCancel.SetTextHotMove(false);
	//m_btnVideoCancel.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoCancel.SetHotTextColor(theDefaultBtnWhiteColor);

	m_bEnableControl.SetTextTop(-1);
	m_bEnableControl.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableControl.SetTextLeft(16);
	m_bEnableControl.SetAlignText(CLabelEx::Align_Left);
	m_bEnableControl.SetBkColor(theDefaultFlatBgColor);
	SetCtrlColor(false);

	m_bEnableControl.SetCheck(TRUE);

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelRemoteDesktop::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CPanelRemoteDesktop::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CPanelRemoteDesktop::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_btn_width = 58;//m_btnVideoEnd.GetImgWidth();
	const int const_btn_height = 26;
	int x = 2;
	int y = 5;
	m_btnVideoOn.MovePoint(x,y,const_btn_width,const_btn_height);
	m_btnVideoAccept.MovePoint(x,y,const_btn_width,const_btn_height);
	x += (const_btn_width+5);
	m_btnVideoEnd.MovePoint(x,y,const_btn_width,const_btn_height);
	x += (const_btn_width+5);
	m_btnVideoCancel.MovePoint(x,y,const_btn_width,const_btn_height);

	//m_bEnableControl.Movep
	//OnMove();
}

void CPanelRemoteDesktop::OnDestroy()
{
//#ifdef USES_EBCOM1
//#else
//	theEBAppClient.EB_SetVideoHwnd(m_pCallInfo.GetCallId(),NULL);
//#endif
	DoVideoDisonnecte();
	CloseAllVideo();
	//m_deviceManager.Deactivate();
	__super::OnDestroy();
}

void CPanelRemoteDesktop::ExitRD(void)
{
	if (m_bInFRDRequest)
		OnBnClickedButtonVideoCancel();
	OnBnClickedButtonVideoEnd();
}

//void CPanelRemoteDesktop::OnMove(void)
//{
//	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
//	for (int i=0; i<MAX_RD_COUNT; i++)
//	{
//		if (m_pUserVideo[i]!=NULL)
//		{
//			m_pUserVideo[i]->OnMove();
//		}
//	}
//}

void CPanelRemoteDesktop::ExitChat(bool bHangup)
{
	OnBnClickedButtonVideoEnd();
	//DoVideoDisonnecte();
}

bool CPanelRemoteDesktop::IsMyOnVideo(void) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	for (int i=0; i<MAX_RD_COUNT; i++)
	{
		if (m_nVideoUserId[i]==theApp.GetLogonUserId())
		{
			return true;
		}
	}
	return false;
}
int CPanelRemoteDesktop::GetVideoIndex(INT64 nVideoUserId) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	for (int i=0; i<MAX_RD_COUNT; i++)
	{
		if (m_nVideoUserId[i]==nVideoUserId)
		{
			return i;
		}
	}
	return -1;
}
//int CPanelRemoteDesktop::GetVideoCount(void) const
//{
//	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
//	int result = 0;
//	for (int i=0; i<MAX_RD_COUNT; i++)
//	{
//		if (m_nVideoUserId[i]>0)
//		{
//			result++;
//		}
//	}
//	return result;
//}
int CPanelRemoteDesktop::GetNullVideoIndex(void)
{
	BoostWriteLock wtlock(m_mutex);
	for (int i=0; i<MAX_RD_COUNT; i++)
	{
		if (m_nVideoUserId[i]==0)
		{
			m_nVideoUserId[i] = 1;	// 外面会重设为UI
			return i;
		}
	}
	return -1;
}
//const int const_view_width = 216;
//const int const_view_height = 168;
//void CPanelRemoteDesktop::OpenUserVideo(INT64 nVideoUserId)
//{
//	if (GetVideoIndex(nVideoUserId)>=0)
//		return;
//	const int index = GetNullVideoIndex();
//	if (index < 0)
//		return;
//	BoostWriteLock wtlock(m_mutex);
//	m_nVideoUserId[index] = nVideoUserId;
//
//	CRect m_rectUser1;
//	m_rectUser1.left = 2+((const_view_width+6)*(index/3));		// 6=左右间隔
//	m_rectUser1.right = m_rectUser1.left + const_view_width;
//	m_rectUser1.top = 38+(const_view_height+6)*(index%3);		// 6=上下间隔
//	m_rectUser1.bottom = m_rectUser1.top + const_view_height;
//	m_rectUser[index] = m_rectUser1;
//
//	CDlgRemoteDesktop * pUserVideo = new CDlgRemoteDesktop(this,false);
//	pUserVideo->Create(CDlgRemoteDesktop::IDD,this);
//	pUserVideo->SetVideoInfo(this->m_pCallInfo.GetCallId(),nVideoUserId);
//
//#ifdef USES_EBCOM_TEST
//	theEBClientCore->EB_OpenRemoteDesktop(m_pCallInfo.GetCallId(),nVideoUserId, (DWORD)pUserVideo,(DWORD)pUserVideo);
//#else
//	theEBAppClient.EB_OpenRemoteDesktop(m_pCallInfo.GetCallId(), nVideoUserId, this->GetSafeHwnd());
//	//theEBAppClient.EB_OpenRemoteDesktop(m_pCallInfo.GetCallId(), nVideoUserId, pUserVideo->GetSafeHwnd());
//	theEBAppClient.EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), nVideoUserId, 3);
//#endif
//	pUserVideo->MoveWindow(&m_rectUser1);
//	pUserVideo->ShowWindow(SW_HIDE);
//	//pUserVideo->ShowWindow(SW_SHOW);
//	m_pUserVideo[index] = pUserVideo;
//}

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	bool bOk = nStateValue==EB_STATE_OK;
	if (bOk)
	{
		theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
		if (m_pCallInfo.m_sGroupCode==0)
		{
			//m_staDesc.SetWindowText(_T("正在请求视频通话，等待对方连接..."));
			//theApp.InvalidateParentRect(&m_staDesc);
		}else
		{
			// 群组视频，打开视频
			if (!m_bInVideoConference)
			{
				m_bInVideoConference = true;
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
				theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),true,0);
#endif
				m_btnVideoCancel.SetWindowText(_T("退出会议"));
				m_btnVideoCancel.SetToolTipText(_T("退出视频会议"));
			}
			SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
		}
	}else
	{
		//m_staDesc.SetWindowText(_T("请求失败！"));
		//theApp.InvalidateParentRect(&m_staDesc);
	}

	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}else
	{
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#else
void CPanelRemoteDesktop::RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	bool bOk = nStateValue==0;
	if (bOk)
	{
		//m_bEnableControl.ShowWindow(pVideoInfo->m_nRDType==EB_RD_DESKTOP_SRC?SW_SHOW:SW_HIDE);
		if (pVideoInfo->m_nRDType==EB_RD_DESKTOP_SRC)
		{
			m_bEnableControl.SetWindowText(_T("允许对方控制本机"));
		}else
		{
			m_bEnableControl.SetWindowText(_T("允许控制对方电脑"));
		}

		if (m_pCallInfo.m_sGroupCode==0)
		{
			//m_staDesc.SetWindowText(_T("正在请求视频通话，等待对方连接..."));
			//theApp.InvalidateParentRect(&m_staDesc);
		}else
		{
			// 群组视频，打开视频
			if (!m_bInVideoConference)
			{
				m_bInVideoConference = true;
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
				theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),true,0);
#endif
				m_btnVideoCancel.SetWindowText(_T("退出会议"));
				m_btnVideoCancel.SetToolTipText(_T("退出视频会议"));
			}
			SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
		}
	}else
	{
		//m_staDesc.SetWindowText(_T("请求失败！"));
		//theApp.InvalidateParentRect(&m_staDesc);
	}

	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(SW_HIDE);
	}else
	{
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#endif

void CPanelRemoteDesktop::DoVideoConnected(void)
{
	std::vector<EB_UserVideoInfo> pUserVideoInfo;
#ifdef USES_EBCOM_TEST
	_variant_t pMemberUserVideoId = theEBClientCore->EB_GetUserRDInfo(m_pCallInfo.GetCallId());
	if (pMemberUserVideoId.vt!=VT_EMPTY && pMemberUserVideoId.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pMemberUserVideoId.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			IEB_UserVideoInfoPtr pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface);
			pUserVideoInfo.push_back(EB_UserVideoInfo(pInterface));
		}
	}
#else
	theEBAppClient.EB_GetUserRDInfo(m_pCallInfo.GetCallId(),pUserVideoInfo);
#endif

	// 生成上麦视频
	m_bLocalOnDesktop = false;
	for (size_t i=0; i<pUserVideoInfo.size(); i++)
	{
		const INT64 nVideoUserId = pUserVideoInfo[i].m_sUserAccount;
		const bool bLocalDesktop = (bool)(nVideoUserId==theApp.GetLogonUserId());
		if (!pUserVideoInfo[i].m_bOnVideo)
		{
			if (!bLocalDesktop && m_pCallInfo.m_sGroupCode==0)
				m_nFromUserId = nVideoUserId;
			continue;
		}
		if (bLocalDesktop)
		{
			m_bLocalOnDesktop = true;
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_OpenLocalDesktop(m_pCallInfo.GetCallId(),9,2);
#else
			theEBAppClient.EB_OpenLocalDesktop(m_pCallInfo.GetCallId(),9,2);
#endif
		}else
		{
			// 群接收时，不打开，没有视频
			if (m_pRDWindows.get()==NULL)
			{
				m_pRDWindows = CDlgRDWindows::create(GetDesktopWindow());
				m_pRDWindows->Create(CDlgRDWindows::IDD, GetDesktopWindow());
			}
			m_pRDWindows->ShowWindow(SW_SHOW);
			m_pRDWindows->SetForegroundWindow();
			m_pRDWindows->SetCallId(m_pCallInfo.GetCallId());
			m_pRDWindows->SetUserId(nVideoUserId);

#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_OpenRemoteDesktop(m_pCallInfo.GetCallId(),nVideoUserId, (DWORD)pUserVideo,(DWORD)pUserVideo);
			theEBClientCore->EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(),nVideoUserId, 3);
#else
			theEBAppClient.EB_OpenRemoteDesktop(m_pCallInfo.GetCallId(), nVideoUserId, m_pRDWindows->GetSafeHwnd());
			theEBAppClient.EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), nVideoUserId, 3);
#endif
			if (m_pCallInfo.m_sGroupCode==0)
				m_nFromUserId = nVideoUserId;
			//OpenUserVideo(nVideoUserId);
		}
	}
	
	if (m_pRDWindows.get()!=NULL && m_nFromUserId>0)
	{
		CString sText;
		sText.Format(_T("用户[%lld]的远程桌面"),m_nFromUserId);
		m_pRDWindows->SetWindowText(sText);
	}

	if (m_bLocalOnDesktop && m_nFromUserId>0)
	{
#ifdef USES_EBCOM_TEST
		if (m_bEnableControl.GetCheck())
			theEBClientCore->EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),m_nFromUserId);
		else
			theEBClientCore->EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),0);
#else
		if (m_bEnableControl.GetCheck())
			theEBAppClient.EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),m_nFromUserId);
		else
			theEBAppClient.EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),0);
#endif
	}
	m_nInDesktop = pUserVideoInfo.empty()?false:true;
}

void CPanelRemoteDesktop::DoVideoDisonnecte(void)
{
	m_bInFRDRequest = false;
	m_nInDesktop = false;
	{
		BoostWriteLock wtlock(m_mutex);
		for (int i=0; i<MAX_RD_COUNT; i++)
		{
			m_nVideoUserId[i] = 0;
			if (m_pUserVideo[i]!=NULL)
			{
				m_pCloseWnd.add(m_pUserVideo[i]);
				m_pUserVideo[i] = NULL;
				SetTimer(TIMERID_CHECK_CLOSE_VIDEO,100,NULL);
			}
		}
		//theApp.InvalidateParentRect(this);
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CloseAllDesktop(m_pCallInfo.GetCallId());
#else
	theEBAppClient.EB_CloseAllDesktop(m_pCallInfo.GetCallId());
#endif

	if (m_pRDWindows.get()!=NULL)
	{
		m_pRDWindows->DestroyWindow();
		m_pRDWindows.reset();
	}
}

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	// 接受才发送窗口消息
	SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}else
	{
		if (!m_bInVideoConference)
		{
			m_bInVideoConference = true;
			m_btnVideoCancel.SetWindowText(_T("退出会议"));
			m_btnVideoCancel.SetToolTipText(_T("退出远程桌面"));
		}
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
		//m_btnVideoEnd.EnableWindow(FALSE);
	}
}
#else
void CPanelRemoteDesktop::RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue)
{
	// 接受才发送窗口消息
	//if (pVideoInfo->m_nRDType==EB_RD_DESKTOP_DEST)
	SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}else
	{
		// 只有接收才有VAckResponse消息；
		// 这里有可能是第一次进入视频会议；
		if (!m_bInVideoConference)
		{
			m_bInVideoConference = true;
			m_btnVideoCancel.SetWindowText(_T("退出会议"));
			m_btnVideoCancel.SetToolTipText(_T("退出远程桌面"));
		}
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(TRUE);
		if (IsMyOnVideo())
		{
			m_btnVideoEnd.EnableWindow(TRUE);
			m_btnVideoCancel.EnableWindow(FALSE);
		}else
		{
			m_bInFRDRequest = true;
			m_btnVideoCancel.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
		}
		//m_btnVideoCancel.EnableWindow(TRUE);
		//if (IsMyOnVideo())
		//	m_btnVideoEnd.EnableWindow(TRUE);
		//else
		//	m_btnVideoEnd.EnableWindow(FALSE);
	}else
	{
		//if (this->GetVideoCount()>0)
		if (m_bInVideoConference)
		{
			// 自动接收视频
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,pUserVideoInfo->UserId);
#else
			theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),VideoDataCallBack,(DWORD)this);
			theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),true,pUserVideoInfo->m_sUserAccount);
#endif
		}
	}
}
#else
void CPanelRemoteDesktop::RDRequest(const EB_RemoteDesktopInfo* pVideoInfo)
{
	m_nRequestUserId = pVideoInfo->m_nFromUserId;
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(TRUE);
		if (IsMyOnVideo())
		{
			m_btnVideoEnd.EnableWindow(TRUE);
			m_btnVideoCancel.EnableWindow(FALSE);
		}else
		{
			m_bInFRDRequest = true;
			if (pVideoInfo->m_nRDType==EB_RD_DESKTOP_DEST)
				m_bEnableControl.SetWindowText(_T("允许对方控制本机"));
			else
				m_bEnableControl.SetWindowText(_T("允许控制对方电脑"));

			m_btnVideoCancel.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
		}
		//m_btnVideoCancel.EnableWindow(TRUE);
		//if (IsMyOnVideo())
		//	m_btnVideoEnd.EnableWindow(TRUE);
		//else
		//	m_btnVideoEnd.EnableWindow(FALSE);
	}else
	{
		//if (this->GetVideoCount()>0)
		if (m_bInVideoConference)
		{
			// 自动接收视频
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,pUserVideoInfo->m_sUserAccount);
#else
			//theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),VideoDataCallBack,AudioDataCallBack);
			theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),true,pVideoInfo->m_nFromUserId);
#endif
		}
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
		SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
	}
}
#else
void CPanelRemoteDesktop::RDAccept(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
		SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,10,NULL);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}
}
#else
void CPanelRemoteDesktop::RDCancel(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelRemoteDesktop::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
	}else
	{
		// 有人关闭我的视频；
		const int nVideoUserId = pUserVideoInfo->UserId;
		if (nVideoUserId == 0) return;

		int index = GetVideoIndex(nVideoUserId);
		if (index >= 0)
		{
			BoostWriteLock wtlock(m_mutex);
			if (m_pUserVideo[index]!=NULL)
			{
				m_pCloseWnd.add(m_pUserVideo[index]);
				m_pUserVideo[index] = NULL;
				SetTimer(TIMERID_CHECK_CLOSE_VIDEO,100,NULL);
			}
			m_nVideoUserId[index]=0;
		}

		//if (GetVideoCount() == 0)
		//{
		//	//SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
		//	DoVideoDisonnecte();
		//}
	}
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		//m_btnVideoCancel.EnableWindow(FALSE);
		//if (!IsMyOnVideo())
		//{
		//	m_btnVideoEnd.EnableWindow(FALSE);
		//}
	}else
	{
		//// 群组可以继续请求查看视频
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#else
void CPanelRemoteDesktop::RDEnd(const EB_RemoteDesktopInfo* pVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
	}else
	{
		// 有人退出视频；
		const INT64 nVideoUserId = pVideoInfo->m_nFromUserId;
		if (nVideoUserId == 0) return;
		const int index = GetVideoIndex(nVideoUserId);
		if (index >= 0)
		{
			BoostWriteLock wtlock(m_mutex);
			if (m_pUserVideo[index]!=NULL)
			{
				m_pCloseWnd.add(m_pUserVideo[index]);
				m_pUserVideo[index] = NULL;
				SetTimer(TIMERID_CHECK_CLOSE_VIDEO,100,NULL);
			}
			m_nVideoUserId[index]=0;
		}

		//if (GetVideoCount() == 0)
		//{
		//	//SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
		//	DoVideoDisonnecte();
		//}
	}
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		//m_btnVideoCancel.EnableWindow(FALSE);
		//if (!IsMyOnVideo())
		//{
		//	m_btnVideoEnd.EnableWindow(FALSE);
		//}
	}else
	{
		//// 群组可以继续请求查看视频
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#endif

void CPanelRemoteDesktop::OnBnClickedButtonVideoEnd()
{
	//DoVideoDisonnecte();	// 自己会收到video_close事件
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
	}else
	{
		m_btnVideoOn.EnableWindow(TRUE);
		m_btnVideoEnd.EnableWindow(FALSE);
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_RDEnd(m_pCallInfo.GetCallId());
#else
	theEBAppClient.EB_RDEnd(m_pCallInfo.GetCallId());
#endif
}

void CPanelRemoteDesktop::OnBnClickedButtonVideoAccept()
{
#ifdef USES_EBCOM_TEST
	//theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
	theEBClientCore->EB_RDAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,m_nRequestUserId);
#else
	theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),true,m_nRequestUserId);
#endif
}

void CPanelRemoteDesktop::OnBnClickedButtonVideoCancel()
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		DoVideoDisonnecte();
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_RDAck(m_pCallInfo.GetCallId(),VARIANT_FALSE,0);
		//theEBClientCore->EB_VideoEnd(m_pCallInfo.GetCallId());
#else
		theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),false,0);
		//theEBAppClient.EB_VideoEnd(m_pCallInfo.GetCallId());
#endif
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
	}else
	{
		if (m_bInVideoConference)
		{
			// 退出会议
			m_bInVideoConference = false;
			DoVideoDisonnecte();
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_FALSE,0);
			theEBClientCore->EB_VideoEnd(m_pCallInfo.GetCallId());
#else
			theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),false,0);
			theEBAppClient.EB_RDEnd(m_pCallInfo.GetCallId());
#endif
			m_btnVideoOn.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
			m_btnVideoCancel.SetWindowText(_T("加入会议"));
			m_btnVideoCancel.SetToolTipText(_T("加入共享桌面"));
		}else
		{
			m_bInVideoConference = true;
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
			theEBAppClient.EB_RDAck(m_pCallInfo.GetCallId(),true,0);
#endif
			m_btnVideoOn.EnableWindow(TRUE);
			m_btnVideoCancel.SetWindowText(_T("退出会议"));
			m_btnVideoCancel.SetToolTipText(_T("退出共享桌面"));
		}
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
		//m_btnVideoEnd.EnableWindow(FALSE);
	}

}

void CPanelRemoteDesktop::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CPanelRemoteDesktop::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CPanelRemoteDesktop::OnBnClickedButtonOn()
{
//	m_btnVideoOn.EnableWindow(FALSE);
//	m_btnVideoEnd.EnableWindow(TRUE);
//
//#ifdef USES_EBCOM_TEST
//	theEBClientCore->EB_VideoRequest(m_pCallInfo.GetCallId(), EB_VIDEO_BOTH);
//#else
//	theEBAppClient.EB_RDRequest(m_pCallInfo.GetCallId(), EB_RD_DESKTOP_SRC);
//#endif
}

void CPanelRemoteDesktop::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_ID_DO_VIDEO_CONNECTE==nIDEvent)
	{
		KillTimer(nIDEvent);
		m_bInFRDRequest = false;
		this->DoVideoConnected();
	}else if (TIMER_ID_DO_VIDEO_DISCONNECTE==nIDEvent)
	{
		KillTimer(nIDEvent);
		this->DoVideoDisonnecte();
	}else if (TIMERID_CHECK_CLOSE_VIDEO==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_CLOSE_VIDEO);
		CloseAllVideo();
		theApp.InvalidateParentRect(this);
	}

	CEbDialogBase::OnTimer(nIDEvent);
}
void CPanelRemoteDesktop::CloseAllVideo(void)
{
	do
	{
		CDlgRemoteDesktop * pWnd = NULL;
		if (m_pCloseWnd.front(pWnd))
		{
			pWnd->DestroyWindow();
			delete pWnd;
		}
	}while (!m_pCloseWnd.empty());
}

void CPanelRemoteDesktop::OnBnClickedCheckEnableControl()
{
	if (m_bEnableControl.GetCheck())
	{
		if (m_bLocalOnDesktop && m_nFromUserId>0)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),m_nFromUserId);
#else
			theEBAppClient.EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),m_nFromUserId);
#endif		
		}else if (!m_bLocalOnDesktop && m_nFromUserId>0)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), m_nFromUserId, 2);
#else
			theEBAppClient.EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), m_nFromUserId, 2);
#endif		
		}
	}else
	{
		if (m_bLocalOnDesktop)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),0);
#else
			theEBAppClient.EB_EnableControlLocalDesktop(m_pCallInfo.GetCallId(),0);
#endif		
		}else if (m_nFromUserId>0)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), m_nFromUserId, 0);
#else
			theEBAppClient.EB_ControlRemoteDesktop(m_pCallInfo.GetCallId(), m_nFromUserId, 0);
#endif	
		}
	}
	m_bEnableControl.Invalidate();
}
BOOL CPanelRemoteDesktop::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (pMsg->wParam==VK_UP)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				return TRUE;
			}
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}
//HBRUSH CPanelRemoteDesktop::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hbr = CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
//	pDC->SetBkMode(TRANSPARENT);  
//	return hbr;
//}
//
//void CPanelRemoteDesktop::OnPaint()
//{
//	if (!IsIconic())
//	{
//		CPaintDC dc2(this);
//		CRect rectClient;
//		this->GetClientRect(&rectClient);
//		CSkinMemDC memDC(&dc2, rectClient);
//		this->ClearBgDrawInfo();
//		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
//		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
//	}
//}
