// DlgEnterpriseInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgEnterpriseInfo.h"


// CDlgEnterpriseInfo dialog

IMPLEMENT_DYNAMIC(CDlgEnterpriseInfo, CEbDialogBase)

CDlgEnterpriseInfo::CDlgEnterpriseInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEnterpriseInfo::IDD, pParent)
	, m_sEnterpriseName(_T(""))
	, m_sPhone(_T(""))
	, m_sFax(_T(""))
	, m_sEmail(_T(""))
	, m_sUrl(_T(""))
	, m_sAddress(_T(""))
	, m_sDescription(_T(""))
{

}

CDlgEnterpriseInfo::~CDlgEnterpriseInfo()
{
}

void CDlgEnterpriseInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_ENT_NAME, m_sEnterpriseName);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_sPhone);
	DDX_Text(pDX, IDC_EDIT_FAX, m_sFax);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_URL, m_sUrl);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
	DDX_Control(pDX, IDC_EDIT_ENT_NAME, m_editEnterpriseName);
	DDX_Control(pDX, IDC_EDIT_PHONE, m_editPhone);
	DDX_Control(pDX, IDC_EDIT_FAX, m_editFax);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgEnterpriseInfo, CEbDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgEnterpriseInfo::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgEnterpriseInfo::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgEnterpriseInfo message handlers

BOOL CDlgEnterpriseInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

	this->GetDlgItem(IDC_EDIT_ENT_NAME)->EnableWindow(FALSE);

	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());

	m_editEnterpriseName.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editPhone.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editFax.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editEmail.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editUrl.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editAddress.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editDescription.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());

	const int const_y	= 60;
	const int const_x1	= 34;
	const int const_x2	= 314;
	const int const_x_interval	= 70;
	const int const_static_width	= 70;
	const int const_edit_width1		= 175;
	const int const_edit_width2		= 455;
	const int const_edit_height		= 21;
	const int const_y_interval	= 35;
	int nX = const_x1;
	int nY = const_y;
	this->GetDlgItem(IDC_STATIC_ENT_NAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_ENT_NAME)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_FAX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_FAX)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_EMAIL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_EMAIL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_URL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_URL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_DESCRIPTION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(nX,nY,const_edit_width2,70);

	const int POS_DLG_WIDTH = 612;
	const int POS_DLG_HEIGHT = 340;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEnterpriseInfo::OnBnClickedOk()
{
	UpdateData();
	if (m_sEnterpriseName.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_ENT_NAME)->SetFocus();
		CDlgMessageBox::EbDoModal(this,_T("请输入公司名称！"),_T(""),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
	OnOK();
}

void CDlgEnterpriseInfo::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作矩形中居中
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
		CPaintDC dc2(this);
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc2, rectClient);
		this->ClearBgDrawInfo();
		if (theApp.GetColorGradient())
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		else
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(245,theDefaultBgColorBgLight1,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);
		Gdiplus::Image * pImage;
		if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_ORG,_T("png"),pImage))
		{
			graphics.DrawImage(pImage,PointF(10,10));
			delete pImage;
		}
		// 写标题
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
		const Gdiplus::PointF pointTitle(40,13);
		graphics.DrawString(L"企业资料",-1,&fontEbTitle,pointTitle,&brushEbTitle);

		//CPaintDC dc2(this);
		//this->ClearBgDrawInfo();
		//this->AddBgDrawInfo(CEbBackDrawInfo(42,0.7,true));
		//this->AddBgDrawInfo(CEbBackDrawInfo(245,0.95,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,0.85,false));
		//this->DrawPopBg(&dc2, theApp.GetMainColor());
		//CRect rectClient;
		//this->GetClientRect(&rectClient);
		//HDC m_hdcMemory = dc2.m_hDC;

		//// 写标题
		//CFont pNewFontTitle;
		//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
		//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
		//::SetBkMode(m_hdcMemory, TRANSPARENT);
		//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
		//CString sOutText = _T("企业资料");
		//TextOut(m_hdcMemory, 15, 15, sOutText, sOutText.GetLength());
		//SelectObject(m_hdcMemory, pOldFond);

	}
}

void CDlgEnterpriseInfo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);

	const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 80;
	const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 28;
	const int const_btn_intever = 10;
	int x = cx-const_btn_width-const_btn_intever;	// 最右边间隔
	const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	x -= (const_btn_width+const_btn_intever);
	m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
}

void CDlgEnterpriseInfo::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
	//CEbDialogBase::OnCancel();
}
