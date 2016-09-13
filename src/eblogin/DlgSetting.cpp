// DlgSetting.cpp : implementation file
//

#include "stdafx.h"
#include "eblogin.h"
#include "DlgSetting.h"


// CDlgSetting dialog

IMPLEMENT_DYNAMIC(CDlgSetting, CEbDialogBase)

CDlgSetting::CDlgSetting(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgSetting::IDD, pParent)
{
}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERVER, m_editServer);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CEbDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgSetting::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgSetting::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgSetting message handlers
 
BOOL CDlgSetting::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

	this->EnableToolTips();
	this->SetToolTipText(IDC_EDIT_SERVER,_T("格式：IP:PORT（域名:PORT）；例如：192.168.10.100:18012"));

	//
	const CString sEBCSetting = theApp.GetAppDataPath()+_T("\\ebc.ini");
	if (::PathFileExists(sEBCSetting))
	{
		TCHAR lpszBuffer[1024];
		memset(lpszBuffer,0,sizeof(lpszBuffer));
		::GetPrivateProfileString("system","server","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting);
		m_sServer = lpszBuffer;
		m_editServer.SetWindowText(lpszBuffer);
	}

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnOk.SetToolTipText(_T("确定"));
	//m_btnOk.SetDrawPanelRgn(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnCancel.SetToolTipText(_T("取消"));
	//m_btnCancel.SetDrawPanelRgn(false);

	float h,s,l;
	::RGBtoHSL(GetRValue(theApp.GetMainColor()), GetGValue(theApp.GetMainColor()), GetBValue(theApp.GetMainColor()),&h,&s,&l);
	const COLORREF crOk = theDefaultBtnNormal==0.0?theApp.GetMainColor():HSLtoRGB(h, s, (l+theDefaultBtnNormal));
	const COLORREF crHot = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
	const COLORREF crPre = ::HSLtoRGB(h, s, min(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
	//const COLORREF crOk = HSLtoRGB(h, s, theDefaultBtnOkColorLight1);
	//const COLORREF crHot1 = HSLtoRGB(h, s, theDefaultBtnHotColorLight1);
	//const COLORREF crHot2 = HSLtoRGB(h, s, theDefaultBtnHotColorLight2);
	//const COLORREF crHot = theApp.GetColorGradient()?crHot1:crHot2;
	//const COLORREF crPre = ::HSLtoRGB(h, s, theDefaultColorNormalLight);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnOk.SetDrawPanel(true,crOk,crHot,crPre);
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,crOk);
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(crOk); 
	m_btnCancel.SetPreTextColor(crOk); 
	m_editServer.SetRectangleColor(crHot,theDefaultFlatLineColor);	// crPre
	//if (theApp.GetColorFlat())
	//	m_editServer.SetRectangleColor(crHot2,crHot1);
	//else
	//	m_editServer.SetRectangleColor(crHot2,crPre);

	const int const_y	= 61;
	const int const_x1	= 35;
	const int const_x2	= 123;
	const int const_static_width	= 80;
	const int const_edit_width		= 175+40;
	const int const_edit_height		= 21;
	//const int const_height_interval	= 44;
	int nX = const_x1;
	int nY = const_y;
	this->GetDlgItem(IDC_STATIC_SERVER)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_EDIT_SERVER)->MoveWindow(nX,nY,const_edit_width,const_edit_height);

	const int POS_DLG_WIDTH = 400;	// 360
	const int POS_DLG_HEIGHT = 160;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	this->SetWindowText(_T("连接设置"));
	SetCircle();
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetting::OnBnClickedOk()
{
	const CString sOldServer = m_sServer;
	m_editServer.GetWindowText(m_sServer);
	if (sOldServer!=m_sServer)
	{
		const CString sEBCSetting = theApp.GetAppDataPath()+_T("\\ebc.ini");
		::WritePrivateProfileString("system","server",m_sServer,sEBCSetting);
	}
	this->OnOK();
}
void CDlgSetting::OnPaint()
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
		DrawInfo();
		//CEbDialogBase::OnPaint();
	}
}

void CDlgSetting::DrawInfo(void)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	//if (theApp.GetColorFlat())
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, theDefaultBgColorTitleHeight, 2);
	//}else
	//{
	//	if (theApp.GetColorGradient())
	//		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
	//	else
	//		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	//	this->AddBgDrawInfo(CEbBackDrawInfo(66,theDefaultBgColorBgLight1,false));
	//	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//	this->DrawPopBg(&memDC, theApp.GetMainColor());
	}
	Gdiplus::Graphics graphics(memDC.m_hDC);
	Gdiplus::Image * pImage;
	if (theApp.ImageFromIDResource(IDB_PNG_IMG1_REG,_T("png"),pImage))
	{
		graphics.DrawImage(pImage,PointF(10,10));
		delete pImage;
	}
	//graphics.DrawImage(theApp.m_image1Group,PointF(10,10));
	// 写标题
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::PointF pointTitle(40,12);
	graphics.DrawString(L"连接设置",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	const Gdiplus::Font fontEbTip(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushTip(Gdiplus::Color(128,128,128));
	const std::string sEnterprise = theSetting.GetEnterprise();
	if (sEnterprise.find("恩布")==std::string::npos || (!theApp.m_sProductName.IsEmpty() && theApp.m_sProductName.Find(_T("恩布"))<0))
		graphics.DrawString(L"留空默认连接公有云环境！",-1,&fontEbTip,PointF(122,83),&brushTip);
	else
		graphics.DrawString(L"留空默认连接恩布公有云环境！",-1,&fontEbTip,PointF(122,83),&brushTip);

	//CPaintDC dc2(this);
	//this->ClearBgDrawInfo();
	//this->AddBgDrawInfo(CEbBackDrawInfo(42,0.7,true));
	//this->AddBgDrawInfo(CEbBackDrawInfo(246,0.98,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,0.9,false));
	//this->DrawPopBg(&dc2, theApp.GetMainColor());
	//// IDB_PNG_IMG1_REG

	//CRect rectClient;
	//this->GetClientRect(&rectClient);
	//HDC m_hdcMemory = dc2.m_hDC;

	//// 写标题
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//CString sOutText = _T("注册新帐号");
	//TextOut(m_hdcMemory, 13, 13, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);
}

void CDlgSetting::OnSize(UINT nType, int cx, int cy)
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


void CDlgSetting::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}
