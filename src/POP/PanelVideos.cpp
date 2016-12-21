// PanelVideos.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelVideos.h"
//#include "Core/MediaDevice.h"

//#define MAKEINT64(a, b)    ((INT64)(((DWORD)(((INT64)(a)) & 0xffffffff)) | ((INT64)((DWORD)(((INT64)(b)) & 0xffffffff))) << 32))
//#define LOINT64(l)         ((DWORD)(((INT64)(l)) & 0xffffffff))
//#define HIINT64(l)         ((DWORD)((((INT64)(l)) >> 32) & 0xffffffff))

#define TIMER_ID_DO_VIDEO_CONNECTE		111
#define TIMER_ID_DO_VIDEO_DISCONNECTE	112
#define TIMERID_CHECK_CLOSE_VIDEO		113

//#define WM_BUILD_USER_VIEDO (WM_USER+10)

// CPanelVideos dialog

IMPLEMENT_DYNAMIC(CPanelVideos, CEbDialogBase)

CPanelVideos::CPanelVideos(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelVideos::IDD, pParent)
{
	m_bInFVideoRequest = false;
	m_bInVideoConference = false;
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		m_pUserVideo[i] = NULL;
		m_nVideoUserId[i] = 0;
	}
}

CPanelVideos::~CPanelVideos()
{
}

void CPanelVideos::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ON, m_btnVideoOn);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_END, m_btnVideoEnd);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_ACCEPT, m_btnVideoAccept);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_CANCEL, m_btnVideoCancel);
}


BEGIN_MESSAGE_MAP(CPanelVideos, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_DESTROY()
//#ifndef USES_EBCOM_TEST
//	ON_MESSAGE(EB_WM_V_REQUEST_RESPONSE, OnMessageVRequestResponse)
//	ON_MESSAGE(EB_WM_V_ACK_RESPONSE, OnMessageVAckResponse)
//	ON_MESSAGE(EB_WM_VIDEO_REQUEST, OnMessageVideoRequest)
//	ON_MESSAGE(EB_WM_VIDEO_ACCEPT, OnMessageVideoAccept)
//	ON_MESSAGE(EB_WM_VIDEO_REJECT, OnMessageVideoCancel)
//	ON_MESSAGE(EB_WM_VIDEO_CLOSE, OnMessageVideoEnd)
//#endif
	//ON_MESSAGE(WM_BUILD_USER_VIEDO, OnMsgBuildUserVideo)

	ON_BN_CLICKED(IDC_BUTTON_VIDEO_END, &CPanelVideos::OnBnClickedButtonVideoEnd)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_ACCEPT, &CPanelVideos::OnBnClickedButtonVideoAccept)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_CANCEL, &CPanelVideos::OnBnClickedButtonVideoCancel)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ON, &CPanelVideos::OnBnClickedButtonOn)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPanelVideos message handlers
#ifdef USES_EBCOM_TEST
//void CPanelVideos::Fire_onVideoStream(ULONG nUserId, BYTE* pVideoData, ULONG nVideoSize, ULONG nUserData)
void CPanelVideos::Fire_onVideoData(LONGLONG nUserVideoId, const BYTE* pData, DWORD dwSize, DWORD dwParam)
{
	if (dwParam == 0) return;
	((CDlgVideo*)dwParam)->DrawVideo((void*)pData,dwSize);
	//((CPanelVideos*)dwParam)->DrawVideoData(nUserVideoId,(void*)pData,nSize);
}
void CPanelVideos::Fire_onAudioData(LONGLONG nUserid, const BYTE* pData, DWORD nSize, DWORD dwParam)
{
	CDlgVideo * pUserVideo = (CDlgVideo*)dwParam;
	if(pUserVideo) {
		pUserVideo->AddFFTSampleData((const char*)pData, nSize);
	}
}

#else
void CPanelVideos::EBVideoDataCallBack(eb::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData)
{
	switch (nCallBackType)
	{
	case EB_CALLBACK_FIRST_VIDEO_FPS:
		((CDlgVideo*)pUserData)->ShowVideo();
		break;
	case EB_CALLBACK_VOICE_VOLUME:
		((CDlgVideo*)pUserData)->SetVoicePos(lParam);
		break;
	default:
		break;
	}
}
#endif
//void CPanelVideos::FFTdataProcess(char *pdata, int length, void* udata)
//{
//	//CDlgVideo *pDlg = reinterpret_cast<CDlgVideo *>(udata);
//	//if(pDlg) {
//	//	pDlg->AddFFTSampleData( pdata, length);
//	//}
//}
//void CPanelVideos::DrawVideoData(INT64 nuserid, void * pdata, int len)
//{
//	int index = GetVideoIndex(nuserid);
//	if (index >= 0 && m_pUserVideo[index]!=NULL)
//	{
//		m_pUserVideo[index]->DrawVideo(pdata, len);
//		if (!m_pUserVideo[index]->IsWindowVisible())
//			m_pUserVideo[index]->ShowWindow(SW_SHOW);
//	}else
//	{
//		PostMessage(WM_BUILD_USER_VIEDO,LOINT64(nuserid),HIINT64(nuserid));
//	}
//}

void CPanelVideos::SetCtrlColor(bool bInvalidate)
{
	m_btnVideoOn.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	m_btnVideoAccept.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	m_btnVideoEnd.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	m_btnVideoCancel.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	if (bInvalidate)
		this->Invalidate();
}

BOOL CPanelVideos::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	this->SetMouseMove(FALSE);
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

#ifdef USES_EBCOM_TEST
	_variant_t pVideoDeviceList = theEBClientCore->EB_GetVideoDeviceList();
	if (pVideoDeviceList.vt!=VT_EMPTY && pVideoDeviceList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pVideoDeviceList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_BSTR)
				continue;
			const CEBString sVideoDevice(var.bstrVal);
			m_pVideoDevices.push_back(sVideoDevice);
		}
	}
#else
	CEBAppClient::EB_GetVideoDeviceList(m_pVideoDevices);
#endif
	m_btnVideoEnd.EnableWindow(FALSE);
	m_btnVideoAccept.EnableWindow(FALSE);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoOn.ShowWindow(SW_HIDE);
		m_btnVideoCancel.EnableWindow(FALSE);
		m_btnVideoAccept.SetToolTipText(_T("接受视频通话"));
		m_btnVideoEnd.SetToolTipText(_T("结束视频通话"));
		m_btnVideoCancel.SetToolTipText(_T("拒绝视频通话"));
	}else
	{
		m_btnVideoAccept.ShowWindow(SW_HIDE);
		m_btnVideoOn.SetWindowText(_T("打开视频"));
		m_btnVideoOn.SetToolTipText(_T("打开本机视频"));
		m_btnVideoEnd.SetWindowText(_T("关闭视频"));
		m_btnVideoEnd.SetToolTipText(_T("关闭本机视频"));
		m_btnVideoCancel.EnableWindow(TRUE);
		m_btnVideoCancel.SetWindowText(_T("加入会议"));
		m_btnVideoCancel.SetToolTipText(_T("加入视频会议"));
	}

	m_btnVideoOn.SetTextHotMove(false);
	m_btnVideoOn.SetDrawPanelRgn(false);
	//m_btnVideoOn.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoOn.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoAccept.SetTextHotMove(false);
	m_btnVideoAccept.SetDrawPanelRgn(false);
	//m_btnVideoAccept.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoAccept.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoEnd.SetTextHotMove(false);
	m_btnVideoEnd.SetDrawPanelRgn(false);
	//m_btnVideoEnd.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoEnd.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnVideoCancel.SetTextHotMove(false);
	m_btnVideoCancel.SetDrawPanelRgn(false);
	//m_btnVideoCancel.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnVideoCancel.SetHotTextColor(theDefaultBtnWhiteColor);
	SetCtrlColor(false);

#ifdef USES_EBCOM_TEST
	// 增加二次计数，避免删除自己
	((CEBVideoDataEventsSink*)this)->AddRef();
	((CEBVideoDataEventsSink*)this)->AddRef();
	theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
	//theEBAppClient.EB_SetVideoHwnd(m_pCallInfo.GetCallId(),this->GetSafeHwnd());
#else
	theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
	//theEBAppClient.EB_SetVideoHwnd(m_pCallInfo.GetCallId(),this->GetSafeHwnd());
#endif
	//InitMediaDevice();
	//SetMicroBoostLevel(0.0);			// 设置麦克风加强为0
	//SetMicrophoneValue(FALSE,-1.0);		// 自动开麦克风

	//m_epVolume.Create(this->GetSafeHwnd());
	//m_epVolume.SetMute(FALSE);
	//m_epVolume.SetVolume(90);
	//m_mixer.SetHwnd(this->GetSafeHwnd());
	//m_mixer.SetMute(CMixer::MICROPHONE, false);
	//m_deviceManager.SetAVDevice(&m_videoDevice, &m_audioDevice);
	//m_deviceManager.Activate();
	//m_deviceManager.SetAudioDefaultInputPin(CAudioInput::Input_MicPhone);	// 麦克风
	////m_deviceManager.SetAudioDefaultInputPin(CAudioInput::Input_Stereo);		// 混音

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelVideos::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CPanelVideos::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CPanelVideos::OnSize(UINT nType, int cx, int cy)
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
	OnMove();
}

void CPanelVideos::OnDestroy()
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

void CPanelVideos::OnMove(void)
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_pUserVideo[i]!=NULL)
		{
			m_pUserVideo[i]->OnMove();
		}
	}
}

void CPanelVideos::ExitChat(bool bHangup)
{
	if (m_bInFVideoRequest)
	{
		OnBnClickedButtonVideoCancel();
	}
	OnBnClickedButtonVideoEnd();
	//DoVideoDisonnecte();
}

bool CPanelVideos::IsMyOnVideo(void) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_nVideoUserId[i]==theApp.GetLogonUserId())
		{
			return true;
		}
	}
	return false;
}
int CPanelVideos::GetVideoIndex(INT64 nVideoUserId) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_nVideoUserId[i]==nVideoUserId)
		{
			return i;
		}
	}
	return -1;
}
int CPanelVideos::CloseUserVideo(INT64 nVideoUserId)
{
	BoostWriteLock wtlock(m_mutex);
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_nVideoUserId[i]==nVideoUserId)
		{
			m_nVideoUserId[i]=0;
			if (m_pUserVideo[i]!=NULL)
			{
				m_pUserVideo[i]->ShowWindow(SW_HIDE);
				m_pCloseWnd.add(m_pUserVideo[i]);
				m_pUserVideo[i] = NULL;
				SetTimer(TIMERID_CHECK_CLOSE_VIDEO,2000,NULL);
			}
			return i;
		}
	}
	return -1;
}

int CPanelVideos::GetVideoCount(void) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
	int result = 0;
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_nVideoUserId[i]>0)
		{
			result++;
		}
	}
	return result;
}
int CPanelVideos::GetNullVideoIndex(void)
{
	BoostWriteLock wtlock(m_mutex);
	for (int i=0; i<MAX_VIDEO_COUNT; i++)
	{
		if (m_nVideoUserId[i]==0)
		{
			m_nVideoUserId[i] = 1;	// 外面会重设为UI
			return i;
		}
	}
	return -1;
}
const int const_view_width = 216;
const int const_view_height = 168;
void CPanelVideos::OpenUserVideo(INT64 nVideoUserId)
{
	if (GetVideoIndex(nVideoUserId)>=0)
		return;
	const int index = GetNullVideoIndex();
	if (index < 0)
		return;
	const bool bLocalUser = nVideoUserId==theApp.GetLogonUserId()?true:false;
	BoostWriteLock wtlock(m_mutex);
	m_nVideoUserId[index] = nVideoUserId;

	CRect m_rectUser1;
	m_rectUser1.left = 2+((const_view_width+6)*(index/3));		// 6=左右间隔
	m_rectUser1.right = m_rectUser1.left + const_view_width;
	m_rectUser1.top = 38+(const_view_height+6)*(index%3);		// 6=上下间隔
	m_rectUser1.bottom = m_rectUser1.top + const_view_height;
	m_rectUser[index] = m_rectUser1;

	CDlgVideo * pUserVideo = new CDlgVideo(this,false);
	pUserVideo->Create(CDlgVideo::IDD,this);
	pUserVideo->SetVideoInfo(this->m_pCallInfo.GetCallId(),nVideoUserId);
	pUserVideo->MoveWindow(&m_rectUser1);
	pUserVideo->ShowWindow(SW_HIDE);
	if (bLocalUser)// && !m_pVideoDevices.empty())
	{
		size_t nLocalVideoIndex = (size_t)theApp.GetProfileInt(_T("param"),_T("local-video-index"),0);
		if (nLocalVideoIndex >= m_pVideoDevices.size())
			nLocalVideoIndex = 0;
#ifdef USES_EBCOM_TEST
		videoType = theEBClientCore->EB_OpenLocalVideo(m_pCallInfo.GetCallId(),nLocalVideoIndex,(DWORD)pUserVideo,(DWORD)pUserVideo);
		//videoType = theEBClientCore->EB_OpenLocalVideo(m_pCallInfo.GetCallId(),nLocalVideoIndex,(IDispatch*)this,(ULONG)pUserVideo);
#else
		const int ret = theEBAppClient.EB_OpenLocalVideo(m_pCallInfo.GetCallId(),nLocalVideoIndex,pUserVideo->GetSafeHwnd(),(void*)pUserVideo);
		if (ret==10)
		{
			CDlgMessageBox::EbMessageBox(this,_T(""),_T("打开视频摄像头失败：\r\n请检查摄像头是否被占用或其他硬件故障！"),CDlgMessageBox::IMAGE_ERROR,5);
		}
#endif
	}else if (!bLocalUser)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_OpenVideo(m_pCallInfo.GetCallId(),nVideoUserId, (DWORD)pUserVideo,(DWORD)pUserVideo);
#else
		theEBAppClient.EB_OpenUserVideo(m_pCallInfo.GetCallId(), nVideoUserId, pUserVideo->GetSafeHwnd(),(void*)pUserVideo);
#endif
	}
	m_pUserVideo[index] = pUserVideo;
}
//LRESULT CPanelVideos::OnMsgBuildUserVideo(WPARAM wParam, LPARAM lParam)
//{
//	const INT64 nVideoUserId = MAKEINT64(wParam,lParam);
//	bool bLocalVideo = (bool)(nVideoUserId==theApp.GetLogonUserId());
//	OpenUserVideo(nVideoUserId,bLocalVideo);
//	return 0;
//}

#ifdef USES_EBCOM_TEST
void CPanelVideos::VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
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
			SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
			//DoVideoConnected();
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
void CPanelVideos::VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	bool bOk = nStateValue==0;
	if (bOk)
	{
		theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
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
			SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
			//DoVideoConnected();
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
#endif

void CPanelVideos::DoVideoConnected(void)
{
	//DoVideoDisonnecte();
	std::vector<EB_UserVideoInfo> pUserVideoInfo;
#ifdef USES_EBCOM_TEST
	_variant_t pMemberUserVideoId = theEBClientCore->EB_GetUserVideoInfo(m_pCallInfo.GetCallId());
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
	//if (pOnVideoUserVideoId.vt!=VT_EMPTY && pOnVideoUserVideoId.parray != NULL)
	//{
	//	CComSafeArray<VARIANT> m_sa(pOnVideoUserVideoId.parray);
	//	for (ULONG i=0;i<m_sa.GetCount();i++)
	//	{
	//		CComVariant var = m_sa.GetAt(i);
	//		if (var.vt != VT_DISPATCH)
	//			continue;
	//		IEB_UserVideoInfoPtr pInterface;
	//		var.pdispVal->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface);
	//		pOnVideoUserId.push_back(EB_UserVideoInfo(pInterface));
	//	}
	//}
#else
	theEBAppClient.EB_GetUserVideoInfo(m_pCallInfo.GetCallId(),pUserVideoInfo);
#endif

	// 生成上麦视频
	for (size_t i=0; i<pUserVideoInfo.size(); i++)
	{
		if (pUserVideoInfo[i].m_bOnVideo)
			OpenUserVideo(pUserVideoInfo[i].m_sUserAccount);
	}
}

void CPanelVideos::DoVideoDisonnecte(bool bHideOnly, INT64 nOnlyHideUid)
{
	m_bInFVideoRequest = false;
	{
		BoostReadLock rdlock(m_mutex);
		for (int i=0; i<MAX_VIDEO_COUNT; i++)
		{
			if (m_pUserVideo[i]==NULL)
			{
				continue;
			}
			if (nOnlyHideUid==0 || nOnlyHideUid==m_nVideoUserId[i])
			{
				m_pUserVideo[i]->ShowWindow(SW_HIDE);
			}
		}
		if (bHideOnly) return;
	}

//#ifdef USES_EBCOM_TEST
//	theEBClientCore->EB_CloseAllVideo(m_pCallInfo.GetCallId());
//#else
//	theEBAppClient.EB_CloseAllVideo(m_pCallInfo.GetCallId());
//#endif

	{
		BoostWriteLock wtlock(m_mutex);
		for (int i=0; i<MAX_VIDEO_COUNT; i++)
		{
			m_nVideoUserId[i] = 0;
			if (m_pUserVideo[i]!=NULL)
			{
				m_pCloseWnd.add(m_pUserVideo[i]);
				m_pUserVideo[i] = NULL;
				SetTimer(TIMERID_CHECK_CLOSE_VIDEO,2000,NULL);
			}
		}
	}
}

#ifdef USES_EBCOM_TEST
void CPanelVideos::VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue)
{
	// 接受才发送窗口消息
	SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
	//DoVideoConnected();
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
			m_btnVideoCancel.SetToolTipText(_T("退出视频会议"));
		}
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
		//m_btnVideoEnd.EnableWindow(FALSE);
	}
}
#else
void CPanelVideos::VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue)
{
	// 接受才发送窗口消息
	SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
	//DoVideoConnected();
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
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
#else
			theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
#endif
			m_bInVideoConference = true;
			m_btnVideoCancel.SetWindowText(_T("退出会议"));
			m_btnVideoCancel.SetToolTipText(_T("退出视频会议"));
		}
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
		//m_btnVideoEnd.EnableWindow(FALSE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelVideos::VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		if (!IsMyOnVideo())
		{
			m_bInFVideoRequest = true;
			m_btnVideoAccept.EnableWindow(TRUE);
			m_btnVideoCancel.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
		}
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
void CPanelVideos::VideoRequest(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		if (!IsMyOnVideo())
		{
			m_bInFVideoRequest = true;
			m_btnVideoAccept.EnableWindow(TRUE);
			m_btnVideoCancel.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
		}
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
			theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
			theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),true,pUserVideoInfo->m_sUserAccount);
#endif
		}
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelVideos::VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
		SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
		//DoVideoConnected();
	}
}
#else
void CPanelVideos::VideoAccept(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(TRUE);
		m_btnVideoCancel.EnableWindow(FALSE);
		SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
		//DoVideoConnected();
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelVideos::VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		m_btnVideoAccept.EnableWindow(FALSE);
	}
}
#else
void CPanelVideos::VideoCancel(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		m_btnVideoAccept.EnableWindow(FALSE);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelVideos::VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
		//DoVideoDisonnecte();
	}else
	{
		// 有人关闭我的视频；
		const int nVideoUserId = pUserVideoInfo->UserId;
		if (nVideoUserId == 0) return;
		CloseUserVideo(nVideoUserId);
		if (GetVideoCount() == 0)
		{
			SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
			//DoVideoDisonnecte();
		}
	}
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}else
	{
		//// 群组可以继续请求查看视频
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#else
void CPanelVideos::VideoEnd(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo)
{
	if (m_pCallInfo.m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
		//DoVideoDisonnecte();
	}else
	{
		// 有人退出视频；
		const INT64 nVideoUserId = pUserVideoInfo->m_sUserAccount;
		if (nVideoUserId == 0) return;
		CloseUserVideo(nVideoUserId);
		if (GetVideoCount() == 0)
		{
			SetTimer(TIMER_ID_DO_VIDEO_DISCONNECTE,10,NULL);
			//DoVideoDisonnecte();
		}
	}
	if (m_pCallInfo.m_sGroupCode==0)
	{
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
	}else
	{
		//// 群组可以继续请求查看视频
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
	}
}
#endif

void CPanelVideos::OnBnClickedButtonVideoEnd()
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		DoVideoDisonnecte(true);
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
	}else
	{
		DoVideoDisonnecte(true, theApp.GetLogonUserId());
		m_btnVideoOn.EnableWindow(TRUE);
		m_btnVideoEnd.EnableWindow(FALSE);
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_VideoEnd(m_pCallInfo.GetCallId());
#else
	theEBAppClient.EB_VideoEnd(m_pCallInfo.GetCallId());
#endif
	// ???
}

void CPanelVideos::OnBnClickedButtonVideoAccept()
{
	//if (m_pCallInfo.m_sGroupCode==0)
	//{
	//	this->OpenUserVideo(theApp.GetLogonUserId());
	//}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
	theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
	theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
	theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),true,0);
#endif
}

void CPanelVideos::OnBnClickedButtonVideoCancel()
{
	if (m_pCallInfo.m_sGroupCode==0)
	{
		DoVideoDisonnecte();
		m_btnVideoAccept.EnableWindow(FALSE);
		m_btnVideoEnd.EnableWindow(FALSE);
		m_btnVideoCancel.EnableWindow(FALSE);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_FALSE,0);
		//theEBClientCore->EB_VideoEnd(m_pCallInfo.GetCallId());
#else
		theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),false,0);
		//theEBAppClient.EB_VideoEnd(m_pCallInfo.GetCallId());
#endif
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
	}else
	{
		if (m_bInVideoConference)
		{
			// 退出会议
			m_bInVideoConference = false;
			DoVideoDisonnecte(true);
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_VideoEnd(m_pCallInfo.GetCallId());
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_FALSE,0);
#else
			theEBAppClient.EB_VideoEnd(m_pCallInfo.GetCallId());
			theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),false,0);
#endif
			DoVideoDisonnecte();
			m_btnVideoOn.EnableWindow(TRUE);
			m_btnVideoEnd.EnableWindow(FALSE);
			m_btnVideoCancel.SetWindowText(_T("加入会议"));
			m_btnVideoCancel.SetToolTipText(_T("加入视频会议"));
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
		}else
		{
			m_bInVideoConference = true;
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_SetVideoCallback(m_pCallInfo.GetCallId(),(IDispatch*)this,(IDispatch*)this);
			theEBClientCore->EB_VideoAck(m_pCallInfo.GetCallId(),VARIANT_TRUE,0);
#else
			theEBAppClient.EB_SetVideoCallback(m_pCallInfo.GetCallId(),EBVideoDataCallBack);
			theEBAppClient.EB_VideoAck(m_pCallInfo.GetCallId(),true,0);
#endif
			m_btnVideoOn.EnableWindow(TRUE);
			m_btnVideoCancel.SetWindowText(_T("退出会议"));
			m_btnVideoCancel.SetToolTipText(_T("退出视频会议"));
		}
		//m_btnVideoAccept.EnableWindow(TRUE);
		//m_btnVideoCancel.EnableWindow(TRUE);
		//m_btnVideoEnd.EnableWindow(FALSE);
	}

}

void CPanelVideos::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CPanelVideos::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CPanelVideos::OnBnClickedButtonOn()
{
	//if (m_pCallInfo.m_sGroupCode>0)
	//	this->OpenUserVideo(theApp.GetLogonUserId());
	m_btnVideoOn.EnableWindow(FALSE);
	m_btnVideoEnd.EnableWindow(TRUE);

#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_VideoRequest(m_pCallInfo.GetCallId(), EB_VIDEO_BOTH);
#else
	theEBAppClient.EB_VideoRequest(m_pCallInfo.GetCallId(), EB_VIDEO_BOTH);
#endif
}

void CPanelVideos::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_ID_DO_VIDEO_CONNECTE==nIDEvent)
	{
		KillTimer(nIDEvent);
		m_bInFVideoRequest = false;
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
void CPanelVideos::CloseAllVideo(void)
{
	do
	{
		CDlgVideo * pWnd = NULL;
		if (m_pCloseWnd.front(pWnd))
		{
			pWnd->DestroyWindow();
			delete pWnd;
		}
	}while (!m_pCloseWnd.empty());
}
BOOL CPanelVideos::PreTranslateMessage(MSG* pMsg)
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
