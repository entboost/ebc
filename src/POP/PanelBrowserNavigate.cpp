// PanelBrowserNavigate.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelBrowserNavigate.h"


// CPanelBrowserNavigate dialog

IMPLEMENT_DYNAMIC(CPanelBrowserNavigate, CEbDialogBase)

CPanelBrowserNavigate::CPanelBrowserNavigate(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelBrowserNavigate::IDD, pParent)
{

}

CPanelBrowserNavigate::~CPanelBrowserNavigate()
{
}

void CPanelBrowserNavigate::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_GOBACK, m_btnGoBack);
	DDX_Control(pDX, IDC_BUTTON_GOFORWARD, m_btnGoForward);
}


BEGIN_MESSAGE_MAP(CPanelBrowserNavigate, CEbDialogBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_GOBACK, &CPanelBrowserNavigate::OnBnClickedButtonGoback)
	ON_BN_CLICKED(IDC_BUTTON_GOFORWARD, &CPanelBrowserNavigate::OnBnClickedButtonGoforward)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPanelBrowserNavigate message handlers

BOOL CPanelBrowserNavigate::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW,0);

	m_btnGoBack.Load(IDB_PNG_BTN_GOBACK72X18);
	m_btnGoBack.SetWindowText(_T(""));
	m_btnGoBack.SetToolTipText(_T("转到上一页"));
	m_btnGoForward.Load(IDB_PNG_BTN_GOFORWARD72X18);
	m_btnGoForward.SetWindowText(_T(""));
	m_btnGoForward.SetToolTipText(_T("转到下一页"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelBrowserNavigate::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	m_btnGoBack.MovePoint(0,0);
	m_btnGoForward.MovePoint(18,0);
}

void CPanelBrowserNavigate::OnBnClickedButtonGoback()
{
	// TODO: Add your control notification handler code here
}

void CPanelBrowserNavigate::OnBnClickedButtonGoforward()
{
	// TODO: Add your control notification handler code here
}

void CPanelBrowserNavigate::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	this->DrawPopBg(&memDC, RGB(255,255,255),0);
}
