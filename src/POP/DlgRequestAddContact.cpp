// DlgRequestAddContact.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgRequestAddContact.h"


// CDlgRequestAddContact dialog

IMPLEMENT_DYNAMIC(CDlgRequestAddContact, CEbDialogBase)

CDlgRequestAddContact::CDlgRequestAddContact(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgRequestAddContact::IDD, pParent)
	, m_sDescription(_T(""))
{

}

CDlgRequestAddContact::~CDlgRequestAddContact()
{
}

void CDlgRequestAddContact::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
}


BEGIN_MESSAGE_MAP(CDlgRequestAddContact, CEbDialogBase)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDlgRequestAddContact::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgRequestAddContact::OnBnClickedButtonClose)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgRequestAddContact message handlers

BOOL CDlgRequestAddContact::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	//m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetWindowText(_T("申请"));
	m_btnOk.SetToolTipText(_T("点击发送申请好友请求"));
	m_btnCancel.SetWindowText(_T("关闭"));
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

	m_rectHead.left = 25;
	m_rectHead.right = m_rectHead.left + 80;
	m_rectHead.top = 55;
	m_rectHead.bottom = m_rectHead.top + 80;
	m_rectName = RectF(m_rectHead.left,m_rectHead.bottom+6,m_rectHead.Width()+20,100);

	const int const_description_height = m_rectHead.Height();	// 85
	int x = (int)m_rectName.GetRight();//m_rectHead.right+20;
	m_editDescription.SetPromptText(_T("请输入添加好友验证信息"));
	m_editDescription.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editDescription.MoveWindow(x, m_rectHead.top, 230, const_description_height);

	//const int y = m_rectHead.top+const_description_height+12;
	//m_btnOk.MovePoint(x, y,theDefaultBtnWidth,theDefaultBtnHeight);
	//x += (theDefaultBtnWidth+theDefaultBtnIntever);
	//m_btnCancel.MovePoint(x, y,theDefaultBtnWidth,theDefaultBtnHeight);

	const int POS_DLG_WIDTH = 380;
	const int POS_DLG_HEIGHT = 220;
	CRect rectClient;
	rectClient.left = (this->GetScreenWidth()-POS_DLG_WIDTH)/2;
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.top = (this->GetScreenHeight()-POS_DLG_HEIGHT)/2;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRequestAddContact::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
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
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}
}

void CDlgRequestAddContact::DrawInfo(void)
{
	CPaintDC dc2(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(350,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);
	//Gdiplus::Image * pImage = NULL;
	//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),pImage))
	//{
	//	graphics.DrawImage(pImage,PointF(10,7));
	//	delete pImage;
	//}
	graphics.DrawImage(theApp.m_image1People,PointF(10,7));
	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());

	// 写标题
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);
	const Gdiplus::PointF pointTitle(40,11);
	graphics.DrawString(L"申请添加好友",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	if (!m_sHeadFilePath.empty() && PathFileExists(m_sHeadFilePath.c_str()))
	{
		Gdiplus::Image imageHead((const WCHAR*)A2W_ACP(m_sHeadFilePath.c_str()));   
		graphics.DrawImage(&imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	}else
	{
		Image * pImage = theApp.m_imageDefaultContact->Clone();;
		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		delete pImage;
	}
	//m_sHeadName = "系统管理员qewrqj;ewrqew;rkq";	// for test
	if (!m_sHeadName.empty())
	{
		const Gdiplus::Font fontText(&fontFamily, 12, FontStyleRegular, UnitPixel);
		Gdiplus::SolidBrush brushEbText(Gdiplus::Color(32,32,32));
		USES_CONVERSION;
		graphics.DrawString(A2W_ACP(m_sHeadName.c_str()),-1,&fontText,m_rectName,StringFormat::GenericTypographic(),&brushEbText);
	}
}

void CDlgRequestAddContact::OnBnClickedOk()
{
	UpdateData();
	//if (m_sDescription.IsEmpty())
	//{
	//	m_editDescription.SetFocus();
	//	CDlgMessageBox::EbMessageBox(this,_T(""),_T("请输入添加好友验证信息！"),CDlgMessageBox::IMAGE_ERROR,5);
	//	return;
	//}
	this->OnOK();
}

void CDlgRequestAddContact::OnBnClickedButtonClose()
{
	this->OnCancel();
	//PostMessage(WM_CLOSE, 0, 0);
}

void CDlgRequestAddContact::OnSize(UINT nType, int cx, int cy)
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
