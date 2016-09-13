// DlgPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgPrompt.h"

#define WINDOW_TITLE_HEIGHT			30

// CDlgPrompt dialog

IMPLEMENT_DYNAMIC(CDlgPrompt, CEbDialogBase)

CDlgPrompt::CDlgPrompt(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgPrompt::IDD, pParent)
	, m_sUserInput(_T(""))
{

}

CDlgPrompt::~CDlgPrompt()
{
}

void CDlgPrompt::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_EDIT_USER_INPUT, m_sUserInput);
	DDX_Control(pDX, IDC_EDIT_USER_INPUT, m_editUserInput);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

}


BEGIN_MESSAGE_MAP(CDlgPrompt, CEbDialogBase)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDlgPrompt::OnBnClickedOk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgPrompt::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgPrompt message handlers

BOOL CDlgPrompt::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

	this->SetDlgItemText(IDC_STATIC_PROMPT,m_sPrompt);
	if (!m_sUserInput.IsEmpty())
	{
		this->SetDlgItemText(IDC_EDIT_USER_INPUT,m_sUserInput);
	}

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	//m_btnClose.SetToolTipText(_T("关闭"));
	//m_btnOk.SetWindowText(_T("保存"));
	//m_btnCancel.SetWindowText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(theApp.GetNormalColor()); 
	m_btnCancel.SetPreTextColor(theApp.GetNormalColor()); 

	const int POS_DLG_WIDTH = 400;
	const int POS_DLG_HEIGHT = 160;

	const int const_y	= WINDOW_TITLE_HEIGHT+20;
	const int const_x	= 25;
	const int const_static_width	= POS_DLG_WIDTH-const_x;
	const int const_edit_width		= POS_DLG_WIDTH-const_x*2;
	const int const_edit_height		= 22;
	int nX = const_x;
	int nY = const_y;
	this->GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nY += 28;
	this->GetDlgItem(IDC_EDIT_USER_INPUT)->MoveWindow(nX,nY,const_edit_width,const_edit_height);

	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);

	this->SetCircle(false);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	this->SetWindowText(m_sTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPrompt::OnPaint()
{
	if (IsIconic())
	{

	}else
	{
		CPaintDC dc2(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc2, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);

		Gdiplus::Graphics graphics(memDC.m_hDC);
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());

		// 写标题
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(6,5);
		graphics.DrawString(A2W(m_sTitle),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}
}

void CDlgPrompt::OnBnClickedOk()
{
	this->GetDlgItemText(IDC_EDIT_USER_INPUT,m_sUserInput);
	if (m_sUserInput.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_USER_INPUT)->SetFocus();
		return;
	}
	this->OnOK();
}

void CDlgPrompt::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	int x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;					// 最右边间隔
	const int const_btn_y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	m_btnOk.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
}

void CDlgPrompt::OnBnClickedButtonClose()
{
	this->OnCancel();
}
