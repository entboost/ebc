// DlgVideo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgVideo.h"
#include "Core/Fourier.h"

#define TIMER_SHOW_WINDOW	101
#define TIMER_SHOWHIDE_VIDEOCONTROL 300

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CDlgVideo dialog

IMPLEMENT_DYNAMIC(CDlgVideo, CDialog)

CDlgVideo::CDlgVideo(CWnd* pParent /*=NULL*/,bool bDeleteThis)
	: CDialog(CDlgVideo::IDD, pParent)
	, m_bDeleteThis(bDeleteThis)
{
//#ifndef USES_EBCOM_TEST
//	m_pUserVideo = NULL;
//#endif
	m_nCallId = 0;
	m_nVideoUserId = 0;
	m_nVoicePos = 0;
	m_pVoiceBar = NULL;
	m_pVideoControl = NULL;
	m_bShow = false;
}

CDlgVideo::~CDlgVideo()
{
}

void CDlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVideo, CDialog)
	//ON_MESSAGE (VIDEO_CONTRON_CLICKED, OnMsgVideoControlClicked)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDlgVideo message handlers

BOOL CDlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pVideoControl = new CDlgVideoControl(this,(CVideoControlCallback*)this);
	m_pVideoControl->Create(CDlgVideoControl::IDD, this);
	m_pVoiceBar = new CDlgVoiceBar(this);
	m_pVoiceBar->Create(CDlgVoiceBar::IDD, this);
	OnMove();

	SetTimer( 50 , 50 , NULL );
	SetTimer(TIMER_SHOWHIDE_VIDEOCONTROL, 300, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVideo::OnDestroy()
{
	if (m_pVideoControl)
	{
		m_pVideoControl->DestroyWindow();
		delete m_pVideoControl;
		m_pVideoControl = NULL;
	}
	if (m_pVoiceBar)
	{
		m_pVoiceBar->DestroyWindow();
		delete m_pVoiceBar;
		m_pVoiceBar = NULL;
	}

	CDialog::OnDestroy();

	if (m_bDeleteThis)
		delete this;

}

void CDlgVideo::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDlgVideo::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CDlgVideo::ShowVideo(void)
{
	if (!m_bShow || !this->IsWindowVisible())
	{
		// ** 直接使用ShowWindow(SW_SHOW);会导致窗口问题；
		m_bShow = true;
		SetTimer(TIMER_SHOW_WINDOW,10,NULL);
		//this->ShowWindow(SW_SHOW);
	}
}


void CDlgVideo::OnMove(void)
{
	if (m_pUserVideo.GetSafeHwnd())
	{
		CRect rect;
		this->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		m_pUserVideo.MoveWindow(rect.left,rect.top,rect.Width(),rect.Height());
	}

	if (m_pVideoControl != NULL && m_pVideoControl->GetSafeHwnd())
	{
		CRect rectWindow;
		this->GetWindowRect(&rectWindow);

		CRect rect;
		rect.left = rectWindow.left;// + 2;
		rect.right = rectWindow.right;// - 2;
		rect.top = rectWindow.top;// + 2;
		rect.bottom = rect.top + 20;
		m_pVideoControl->MoveWindow(&rect);
	}
	if (m_pVoiceBar != NULL && m_pVoiceBar->GetSafeHwnd())
	{
		CRect rectWindow;
		//this->ClientToScreen(&rectWindow);
		this->GetWindowRect(&rectWindow);

		const long const_VoiceBar_Width = 50;
		const long const_VoiceBar_Height = 40;
		CRect rect;
		rect.left = rectWindow.left + (5+1);
		rect.right = rect.left + const_VoiceBar_Width;
		rect.bottom = rectWindow.bottom - (5+1);
		rect.top = rect.bottom - const_VoiceBar_Height;
		m_pVoiceBar->MoveWindow(&rect);
	}
}

void CDlgVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	OnMove();
}

void CDlgVideo::SetVideoInfo(eb::bigint nCallId, eb::bigint nUserId)
{
	m_nCallId = nCallId;
	m_nVideoUserId = nUserId;
}

//void CDlgVideo::FFTdataProcess(char *pdata, int length, void* udata)
//{
//	CDlgVideo *pDlg = reinterpret_cast<CDlgVideo *>(udata);
//	if(pDlg) {
//		pDlg->AddFFTSampleData( pdata, length);
//	}
//}

void CDlgVideo::SetVoicePos(int nVoicePos)
{
	m_nVoicePos = nVoicePos;
}

void CDlgVideo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (50 == nIDEvent)
	{
		if (m_pVoiceBar != NULL && m_pVoiceBar->GetSafeHwnd())
		{
			if (m_nVoicePos > 0 && !m_pVoiceBar->IsWindowVisible() && this->IsWindowVisible())
			{
				m_pVoiceBar->ShowWindow(SW_SHOW);
			}
			if (!m_pVoiceBar->SetVoiceValue(m_nVoicePos))
			{
				m_nVoicePos = 0;
			}
		}
	}else if (TIMER_SHOWHIDE_VIDEOCONTROL==nIDEvent)
	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_pVideoControl != NULL && m_pVideoControl->GetSafeHwnd())
		{
			CRect rect;
			m_pVideoControl->GetWindowRect(&rect);
			this->ScreenToClient(&rect);
			//this->GetWindowRect(&rect);
			//this->ScreenToClient(&rect);
			//rect.bottom -= 22;
			if (/*this->m_nUserId > 0 && */!m_pVideoControl->IsWindowVisible() && rect.PtInRect(pos) && this->IsWindowVisible())
			{
				m_pVideoControl->ShowWindow(SW_SHOW);
			}else if (!rect.PtInRect(pos) && m_pVideoControl->IsWindowVisible())
			{
				m_pVideoControl->ShowWindow(SW_HIDE);
			}
		}
		//if (m_StaNetInfo.GetSafeHwnd())
		//{
		//	CRect rect;
		//	m_StaNetInfo.GetWindowRect(&rect);
		//	this->ScreenToClient(&rect);
		//	if (rect.PtInRect(pos))
		//	{
		//		m_StaNetInfo.SetFocus();					// 用于实现鼠标提示
		//	}
		//}
		//if (m_StaUserIcon.GetSafeHwnd())
		//{
		//	CRect rect;
		//	m_StaUserIcon.GetWindowRect(&rect);
		//	this->ScreenToClient(&rect);
		//	if (rect.PtInRect(pos))
		//	{
		//		m_StaUserIcon.SetFocus();					// 用于实现鼠标提示
		//	}
		//}
		//if (m_StaVote.GetSafeHwnd())
		//{
		//	CRect rect;
		//	m_StaVote.GetWindowRect(&rect);
		//	this->ScreenToClient(&rect);
		//	if (rect.PtInRect(pos))
		//	{
		//		m_StaVote.SetFocus();						// 用于实现鼠标提示
		//	}
		//}
	}else if (TIMER_SHOW_WINDOW==nIDEvent)
	{
		KillTimer(nIDEvent);
		this->ShowWindow(SW_SHOW);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgVideo::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	this->OnMove();
}

void CDlgVideo::OnControlClick(int nControlId,BOOL bChecked)
{
	switch (nControlId)
	{
	case 1:
		{
			// 屏蔽或打开视频
			if (bChecked)
			{
				// 屏蔽视频
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_SetVideoOpenClose(m_nCallId,m_nVideoUserId,VARIANT_FALSE);
#else
				theEBAppClient.EB_SetVideoOpenClose(m_nCallId,m_nVideoUserId,false);
#endif
			}else
			{
				// 打开视频；
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_SetVideoOpenClose(m_nCallId,m_nVideoUserId,VARIANT_TRUE);
#else
				theEBAppClient.EB_SetVideoOpenClose(m_nCallId,m_nVideoUserId,true);
#endif
			}
		}break;
	case 2:
		{
			// 屏蔽或打开声音
			if (bChecked)
			{
				// 屏蔽声音
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_SetAudioOpenClose(m_nCallId,m_nVideoUserId,VARIANT_FALSE);
#else
				theEBAppClient.EB_SetAudioOpenClose(m_nCallId,m_nVideoUserId,false);
#endif
			}else
			{
				// 打开声音；
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_SetAudioOpenClose(m_nCallId,m_nVideoUserId,VARIANT_TRUE);
#else
				theEBAppClient.EB_SetAudioOpenClose(m_nCallId,m_nVideoUserId,true);
#endif
			}
		}break;
	default:
		break;
	}
}
//LRESULT CDlgVideo::OnMsgVideoControlClicked(WPARAM wParam, LPARAM lParam)
//{
//
//
//	return 0;
//}
