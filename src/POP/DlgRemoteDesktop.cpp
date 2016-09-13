// DlgRemoteDesktop.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgRemoteDesktop.h"
#include "Core/Fourier.h"

#define TIMER_SHOW_WINDOW	101
#define TIMER_SHOWHIDE_VIDEOCONTROL 300

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CDlgRemoteDesktop dialog

IMPLEMENT_DYNAMIC(CDlgRemoteDesktop, CDialog)

CDlgRemoteDesktop::CDlgRemoteDesktop(CWnd* pParent /*=NULL*/,bool bDeleteThis)
	: CDialog(CDlgRemoteDesktop::IDD, pParent)
	, m_bDeleteThis(bDeleteThis)
{
//#ifndef USES_EBCOM_TEST
//	m_pUserVideo = NULL;
//#endif
	m_nCallId = 0;
	m_nVideoUserId = 0;
	m_pVideoControl = NULL;
	m_bShow = false;
}

CDlgRemoteDesktop::~CDlgRemoteDesktop()
{
}

void CDlgRemoteDesktop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRemoteDesktop, CDialog)
	//ON_MESSAGE (VIDEO_CONTRON_CLICKED, OnMsgVideoControlClicked)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDlgRemoteDesktop message handlers

BOOL CDlgRemoteDesktop::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pVideoControl = new CDlgVideoControl(this,(CVideoControlCallback*)this);
	m_pVideoControl->Create(CDlgVideoControl::IDD, this);
	OnMove();

	SetTimer(TIMER_SHOWHIDE_VIDEOCONTROL, 300, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRemoteDesktop::OnDestroy()
{
	if (m_pVideoControl)
	{
		m_pVideoControl->DestroyWindow();
		delete m_pVideoControl;
		m_pVideoControl = NULL;
	}

	CDialog::OnDestroy();

	if (m_bDeleteThis)
		delete this;

}

void CDlgRemoteDesktop::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDlgRemoteDesktop::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

bool CDlgRemoteDesktop::Photo(const char* lspzSaveTo)
{
	//if (m_pUserVideoInterface != NULL)
	//{
	//	return SUCCEEDED(m_pUserVideoInterface->VW_SaveBitmapFile(lspzSaveTo))?true:false;
	//}
	return false;
}

void CDlgRemoteDesktop::OnMove(void)
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
}

void CDlgRemoteDesktop::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	OnMove();
}

void CDlgRemoteDesktop::SetVideoInfo(eb::bigint nCallId, eb::bigint nUserId)
{
	m_nCallId = nCallId;
	m_nVideoUserId = nUserId;
}

void CDlgRemoteDesktop::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMER_SHOWHIDE_VIDEOCONTROL==nIDEvent)
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
			if (/*this->m_nUserId > 0 && */!m_pVideoControl->IsWindowVisible() && rect.PtInRect(pos))
			{
				m_pVideoControl->ShowWindow(SW_SHOW);
			}else if (!rect.PtInRect(pos) && m_pVideoControl->IsWindowVisible())
			{
				m_pVideoControl->ShowWindow(SW_HIDE);
			}
		}
	}else if (TIMER_SHOW_WINDOW==nIDEvent)
	{
		KillTimer(nIDEvent);
		this->ShowWindow(SW_SHOW);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgRemoteDesktop::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	this->OnMove();
}

void CDlgRemoteDesktop::OnControlClick(int nControlId,BOOL bChecked)
{
	// ???
	return;
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
//LRESULT CDlgRemoteDesktop::OnMsgVideoControlClicked(WPARAM wParam, LPARAM lParam)
//{
//
//
//	return 0;
//}
