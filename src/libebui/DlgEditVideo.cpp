// DlgEditVideo.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgEditVideo.h"

// CDlgEditVideo dialog

IMPLEMENT_DYNAMIC(CDlgEditVideo, CEbDialogBase)

CDlgEditVideo::CDlgEditVideo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEditVideo::IDD, pParent)
{
	m_pMyVideo = NULL;
}

CDlgEditVideo::~CDlgEditVideo()
{
}

void CDlgEditVideo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VIDEOS, m_comboVideos);
	DDX_Control(pDX, IDC_BUTTON_LOCAL_VIDEO, m_btnLocalVideo);
}


BEGIN_MESSAGE_MAP(CDlgEditVideo, CEbDialogBase)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEOS, &CDlgEditVideo::OnCbnSelchangeComboVideos)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_VIDEO, &CDlgEditVideo::OnBnClickedButtonLocalVideo)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDlgEditVideo message handlers

void CDlgEditVideo::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgEditVideo::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}
void CDlgEditVideo::SetCtrlColor(void)
{
	m_btnLocalVideo.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
}

BOOL CDlgEditVideo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

	this->EnableToolTips();
	this->SetToolTipText(IDC_COMBO_VIDEOS,_T("选择视频设备，打开预览视频，保存默认视频通话设备。"));

	m_btnLocalVideo.SetTextHotMove(false);
	m_btnLocalVideo.SetWindowText(_T("打开"));
	m_btnLocalVideo.SetToolTipText(_T("打开本地预览视频"));
	m_btnLocalVideo.SetTextTop(-1);
	SetCtrlColor();

	m_comboVideos.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboVideos.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboVideos.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	const int nLocalVideoIndex = (int)theApp.GetProfileInt(_T("param"),_T("local-video-index"),0);
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
			m_comboVideos.AddString(sVideoDevice.c_str());
		}
	}
#else
	std::vector<std::string> pVideoDevices;
	CEBAppClient::EB_GetVideoDeviceList(pVideoDevices);
	for (size_t i=0; i<pVideoDevices.size(); i++)
	{
		const std::string sVideoDevice = pVideoDevices[i];
		m_comboVideos.AddString(sVideoDevice.c_str());
	}
#endif

	if (m_comboVideos.GetCount()>nLocalVideoIndex)
		m_comboVideos.SetCurSel(nLocalVideoIndex);
	else if (m_comboVideos.GetCount()>0)
		m_comboVideos.SetCurSel(0);

	const int const_y	= 18;
	const int const_x1	= 45-28;	// 28是父窗口左边间隔
	const int const_x2	= 272-28;
	int nX = const_x1;
	int nY = const_y;
	this->GetDlgItem(IDC_COMBO_VIDEOS)->MoveWindow(nX,nY,210,22);
	nX = const_x2;
	this->GetDlgItem(IDC_BUTTON_LOCAL_VIDEO)->MoveWindow(nX,nY-2,60,26);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditVideo::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgEditVideo::OnCbnSelchangeComboVideos()
{
	const int nLocalVideoIndex = m_comboVideos.GetCurSel();
	theApp.WriteProfileInt(_T("param"),_T("local-video-index"),nLocalVideoIndex);
}

void CDlgEditVideo::Fire_onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData)
{
	if (nUserData == 0) return;
	if (nCallbackType==EBMM_ROOM_CALLBACK_FIRST_VIDEO_FPS)
		((CDlgVideo*)nUserData)->ShowVideo();
	else if (nCallbackType==EBMM_ROOM_CALLBACK_VOICE_VOLUME)
		((CDlgVideo*)nUserData)->SetVoicePos(lParam);
}
//
//#else
//void CDlgEditVideo::ProcessVideoStream(vr::bigint nId, vr::VIDEOFRAME *pFrame, void *lParam, DWORD udata)
//{
//	if (udata == 0) return;
//	((CDlgVideo*)udata)->DrawVideo(pFrame->data, pFrame->usedlen);
//}
//#endif

void CDlgEditVideo::OnMove(void)
{
	if (m_pMyVideo!=NULL && m_pMyVideo->GetSafeHwnd())
	{
		m_pMyVideo->OnMove();
	}

}

void CDlgEditVideo::CloseVideo(void)
{
	DispEventUnadvise(m_pVideoRoom);
	if (m_pVideoRoom != NULL)
	{
		// 关闭本地视频
		if (m_pMyVideo!=NULL)	// ** 避免背景灰色
			m_pMyVideo->ShowWindow(SW_HIDE);
		m_pVideoRoom->StopVideoCapture();
		m_pVideoRoom->StopAudioCapture();
		m_pVideoRoom->UnRegister();
		m_pVideoRoom->Stop();
		m_pVideoRoom.Release();
	}
	if (m_pMyVideo != NULL)
	{
		m_pMyVideo->DestroyWindow();
		delete m_pMyVideo;
		m_pMyVideo = NULL;
	}
	m_btnLocalVideo.SetWindowText(_T("打开"));
	m_btnLocalVideo.SetToolTipText(_T("打开本地视频"));
	m_comboVideos.EnableWindow(TRUE);
}

void CDlgEditVideo::CreateVideoRoom(void)
{
	if (m_pVideoRoom == NULL)
	{
		CoCreateInstance(__uuidof(EBMMRoom),NULL,CLSCTX_INPROC,__uuidof(IEBMMRoom),(void**)&m_pVideoRoom);
		m_pVideoRoom->Start("","",0);
		m_pVideoRoom->Register(0, theApp.GetLogonUserId(),0, VARIANT_FALSE);
		m_pVideoRoom->LocalAudioSend = VARIANT_FALSE;
		m_pVideoRoom->LocalVideoSend = VARIANT_FALSE;

		// OK
		DispEventAdvise(m_pVideoRoom);
	}
}

void CDlgEditVideo::OnBnClickedButtonLocalVideo()
{
	if (m_comboVideos.GetCount() == 0)
	{
		CDlgMessageBox::EbMessageBox(this,_T("本地找不到视频设备！"),_T(""),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
	CreateVideoRoom();
	if (m_pMyVideo == NULL)
	{
		// 打开本地视频
		AFX_SWITCH_INSTANCED();
		const int nvideoindex = m_comboVideos.GetCurSel();
		m_pVideoRoom->InitVideoCapture(nvideoindex, 10);
		m_pVideoRoom->InitAudioCapture();

		CRect m_rectMy;
		m_comboVideos.GetWindowRect(&m_rectMy);
		this->ScreenToClient(&m_rectMy);
		m_rectMy.right = m_rectMy.left+const_video_width;
		m_rectMy.top += 28;
		m_rectMy.bottom = m_rectMy.top + const_video_height;
		m_pMyVideo = new CDlgVideo(this);
		m_pMyVideo->Create(CDlgVideo::IDD,this);
		m_pMyVideo->MoveWindow(&m_rectMy);
		m_pMyVideo->ShowWindow(SW_SHOW);
		m_pVideoRoom->StartVideoCapture((DWORD)m_pMyVideo->GetSafeHwnd());
		m_pVideoRoom->StartAudioCapture();
		m_pVideoRoom->StartVideoGraph(theApp.GetLogonUserId());
		m_pVideoRoom->SetUserData(theApp.GetLogonUserId(),(ULONG)m_pMyVideo);
		m_btnLocalVideo.SetWindowText(_T("关闭"));
		m_btnLocalVideo.SetToolTipText(_T("关闭本地视频"));
		m_comboVideos.EnableWindow(FALSE);
	}else
	{
		// 关闭本地视频
		CloseVideo();
	}
	theApp.InvalidateParentRect(this);
}

void CDlgEditVideo::OnDestroy()
{
	CloseVideo();
	CEbDialogBase::OnDestroy();
}

void CDlgEditVideo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	//m_btnLocalVideo.MovePoint(220, 5;
}

void CDlgEditVideo::OnTimer(UINT_PTR nIDEvent)
{
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgEditVideo::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
}
