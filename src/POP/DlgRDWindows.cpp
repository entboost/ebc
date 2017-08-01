// DlgRDWindows.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgRDWindows.h"


// CDlgRDWindows dialog

IMPLEMENT_DYNAMIC(CDlgRDWindows, CDialog)

CDlgRDWindows::CDlgRDWindows(CWnd* pParent /*=NULL*/, bool bDeleteThis)
	: CDialog(CDlgRDWindows::IDD, pParent)
	, m_bDeleteThis(bDeleteThis)
	, m_nCallId(0), m_nUserId(0)

{

}

CDlgRDWindows::~CDlgRDWindows()
{
}

void CDlgRDWindows::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CDlgRDWindows, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//ON_WM_TIMER()
	//ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgRDWindows::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgRDWindows::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgRDWindows::OnBnClickedButtonClose)

	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CDlgRDWindows message handlers

BOOL CDlgRDWindows::OnInitDialog()
{
	CDialog::OnInitDialog();
	//ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	//this->SetSplitterBorder();

	m_btnMin.SetAutoSize(false);
	m_btnMin.SetDrawToolButtonPic(1,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMin.SetDrawPanelRgn(false);
	m_btnMin.SetToolTipText(_T("最小化"));
	m_btnMax.SetAutoSize(false);
	m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMax.SetDrawPanelRgn(false);
	m_btnMax.SetToolTipText(_T("最大化"));
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));

	this->MoveWindow(0, 0, 720, 580);
	this->SetWindowText(m_sTitle.c_str());
	//SetCircle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRDWindows::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_bDeleteThis)
		delete this;
}

void CDlgRDWindows::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMax.GetSafeHwnd()!=NULL)
		m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	//const int offset = 5;
	//theEBAppClient.EB_MoveRemoteDesktopWindow(m_nCallId,m_nUserId,offset,36,cx-offset*2,cy-36-offset,true);
	theEBAppClient.EB_MoveRemoteDesktopWindow(m_nCallId,m_nUserId,0,0,cx,cy,true);
}

void CDlgRDWindows::OnPaint()
{
	//if (IsIconic())
	//{
	//	//CPaintDC dc(this); // 用于绘制的设备上下文

	//	//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	//	//// 使图标在工作区矩形中居中
	//	//int cxIcon = GetSystemMetrics(SM_CXICON);
	//	//int cyIcon = GetSystemMetrics(SM_CYICON);
	//	//CRect rect;
	//	//GetClientRect(&rect);
	//	//int x = (rect.Width() - cxIcon + 1) / 2;
	//	//int y = (rect.Height() - cyIcon + 1) / 2;

	//	//// 绘制图标
	//	//dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CPaintDC dc(this); // 用于绘制的设备上下文
	//	CRect rectClient;
	//	this->GetClientRect(&rectClient);
	//	CSkinMemDC memDC(&dc, rectClient);
	//	this->ClearBgDrawInfo();
	//	if (theApp.GetColorGradient())
	//		this->AddBgDrawInfo(CEbBackDrawInfo(36,theDefaultBgColorTitleLight1,false));
	//	else
	//		this->AddBgDrawInfo(CEbBackDrawInfo(36,1.0,false));
	//	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	//	this->DrawPopBg(&memDC, theApp.GetMainColor());
	//	Gdiplus::Graphics graphics(memDC.m_hDC);

	//	const FontFamily fontFamily(theFontFamily.c_str());
	//	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);
	//	//const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	//	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	//	Gdiplus::PointF pointTitle(7,9);
	//	USES_CONVERSION;
	//	graphics.DrawString(A2W_ACP(m_sTitle.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	//}
}

void CDlgRDWindows::OnClose()
{
	if (CDlgMessageBox::EbDoModal(this,"远程桌面",_T("正在远程桌面中：\r\n确定退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}

#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_RDEnd(m_nCallId);
#else
	theEBAppClient.EB_RDEnd(m_nCallId);
#endif
	//CDialog::OnClose();
	this->EndDialog(IDOK);
}

void CDlgRDWindows::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgRDWindows::OnBnClickedButtonMax()
{
	//int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	////int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	//int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	//static CRect theRestoreRect;
	//CRect rect;
	//GetWindowRect(&rect);
	//if (rect.Width() == m_nScreenWidth)
	//{
	//	this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
	//	m_btnMax.Load(IDB_PNG_MAX);
	//	MoveWindow(&theRestoreRect);
	//}else
	//{
	//	this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
	//	m_btnMax.Load(IDB_PNG_RESTORE);
	//	theRestoreRect = rect;
	//	rect.left = 0;
	//	rect.top = 0;
	//	rect.right = rect.left + m_nScreenWidth;
	//	rect.bottom = rect.top + m_nScreenHeight;
	//	MoveWindow(&rect);
	//}
	////PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CDlgRDWindows::OnBnClickedButtonClose()
{
	//PostMessage(EB_MSG_EXIT_CHAT, 0, 0);
	this->PostMessage(WM_CLOSE, 0, 0);
}

void CDlgRDWindows::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	//OnBnClickedButtonMax();
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDlgRDWindows::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CDlgRDWindows::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	this->PostMessage(WM_CLOSE, 0, 0);
	//CDialog::OnCancel();
}
