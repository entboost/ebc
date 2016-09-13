// DlgVideoControl.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgVideoControl.h"
#include "Core/ImageEx.h"


// CDlgVideoControl dialog

IMPLEMENT_DYNAMIC(CDlgVideoControl, CDialogSK)

CDlgVideoControl::CDlgVideoControl(CWnd* pParent /*=NULL*/,CVideoControlCallback* pCallback)
	: CDialogSK(CDlgVideoControl::IDD, pParent)
	, m_pCallback(pCallback)
{

}

CDlgVideoControl::~CDlgVideoControl()
{
}

void CDlgVideoControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideo);
	DDX_Control(pDX, IDC_STATIC_SPEEK, m_staSpeek);
}


BEGIN_MESSAGE_MAP(CDlgVideoControl, CDialogSK)
	ON_STN_CLICKED(IDC_STATIC_VIDEO, OnStnClickedStaticVideo)
	ON_STN_CLICKED(IDC_STATIC_SPEEK, OnStnClickedStaticSpeek)
	//ON_BN_CLICKED(IDC_BTN_VIDEO, OnStnClickedStaZoom)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgVideoControl message handlers

BOOL CDlgVideoControl::OnInitDialog()
{
	CDialogSK::OnInitDialog();
	this->SetBitmap(IDB_VIDEOCONTROL_BG);
	SetStyle(LO_STRETCH);
	EnableEasyMove(FALSE);
	SetTransparent(200);

	//m_staVideo.SetTextTop(-1);
	m_staVideo.LoadLabel(IDB_PNG_VC_VIDEO,-1,0,2);
	m_staVideo.SetCheckBoxHoverBorder(TRUE,RGB(0,128,255));
	m_staSpeek.LoadLabel(IDB_PNG_VC_SPEEK,-1,0,2);
	m_staSpeek.SetCheckBoxHoverBorder(TRUE,RGB(0,128,255));
	m_staVideo.SetToolTipText(_T("ÆÁ±ÎÊÓÆµ"));
	m_staSpeek.SetToolTipText(_T("ÆÁ±ÎÉùÒô"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//void CDlgVideoControl::OnStnClickedStaZoom()
//{
//	this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 1);
//}
//void CDlgVideoControl::OnStnClickedStaMin()
//{
//	this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 2);
//}
void CDlgVideoControl::OnStnClickedStaticVideo()
{
	if (m_staVideo.GetCheck())
		m_staVideo.SetToolTipText(_T("´ò¿ªÊÓÆµ"));
	else
		m_staVideo.SetToolTipText(_T("ÆÁ±ÎÊÓÆµ"));
	if (m_pCallback)
		m_pCallback->OnControlClick(1,m_staVideo.GetCheck());
	//this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 1);
}
void CDlgVideoControl::OnStnClickedStaticSpeek()
{
	if (m_staSpeek.GetCheck())
		m_staSpeek.SetToolTipText(_T("´ò¿ªÉùÒô"));
	else
		m_staSpeek.SetToolTipText(_T("ÆÁ±ÎÉùÒô"));
	if (m_pCallback)
		m_pCallback->OnControlClick(2,m_staSpeek.GetCheck());
	//this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 2);
}
//void CDlgVideoControl::OnStnClickedStaPicture()
//{
//	this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 5);
//}
//void CDlgVideoControl::OnStnClickedStaClose()
//{
//	this->GetParent()->PostMessage(VIDEO_CONTRON_CLICKED, 6);
//}


void CDlgVideoControl::OnSize(UINT nType, int cx, int cy)
{
	CDialogSK::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	int x = 2;
	m_staVideo.MoveWindowPoint(x, 2);
	const long const_Btn_Width = m_staVideo.GetWidth();
	x += (const_Btn_Width+3);
	m_staSpeek.MoveWindowPoint(x, 2);
}

void CDlgVideoControl::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnCancel();
}

void CDlgVideoControl::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnOK();
}
