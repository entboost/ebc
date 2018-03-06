// DlgGroupInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgGroupInfo.h"


// CDlgGroupInfo dialog

IMPLEMENT_DYNAMIC(CDlgGroupInfo, CEbDialogBase)

CDlgGroupInfo::CDlgGroupInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgGroupInfo::IDD, pParent)
	, m_nGroupId(0),m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
	, m_sGroupName(_T(""))
	, m_sPhone(_T(""))
	, m_sFax(_T(""))
	, m_sEmail(_T(""))
	, m_sUrl(_T(""))
	, m_sAddress(_T(""))
	, m_nDisplayIndex(0)
	, m_sDescription(_T(""))
{

}

CDlgGroupInfo::~CDlgGroupInfo()
{
}

void CDlgGroupInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_DEP_NAME, m_sGroupName);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_sPhone);
	DDX_Text(pDX, IDC_EDIT_FAX, m_sFax);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_URL, m_sUrl);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_EDIT_DISPLAY_INDEX, m_nDisplayIndex);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
	DDX_Control(pDX, IDC_EDIT_DEP_NAME, m_editGroupName);
	DDX_Control(pDX, IDC_EDIT_PHONE, m_editPhone);
	DDX_Control(pDX, IDC_EDIT_FAX, m_editFax);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_DISPLAY_INDEX, m_editDisplayIndex);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgGroupInfo, CEbDialogBase)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgGroupInfo::OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgGroupInfo::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgGroupInfo message handlers

BOOL CDlgGroupInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());

	m_editGroupName.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editPhone.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editFax.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editEmail.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editUrl.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editAddress.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editDisplayIndex.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
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
	this->GetDlgItem(IDC_STATIC_GROUP_NAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DEP_NAME)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);

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
	if (m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || m_nGroupType == EB_GROUP_TYPE_PROJECT)
	{
		this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
		nX = const_x2;
		this->GetDlgItem(IDC_STATIC_DISPLAY_INDEX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
		nX += const_x_interval;
		this->GetDlgItem(IDC_EDIT_DISPLAY_INDEX)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	}else
	{
		this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);
		this->GetDlgItem(IDC_STATIC_DISPLAY_INDEX)->ShowWindow(SW_HIDE);
		this->GetDlgItem(IDC_EDIT_DISPLAY_INDEX)->ShowWindow(SW_HIDE);
	}

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

	switch (m_nGroupType)
	{
	case EB_GROUP_TYPE_DEPARTMENT:
		m_editGroupName.SetPromptText(_T("**部门名称**"));
		break;
	case EB_GROUP_TYPE_PROJECT:
		m_editGroupName.SetPromptText(_T("**项目名称**"));
		break;
	case EB_GROUP_TYPE_GROUP:
		m_editGroupName.SetPromptText(_T("**群组名称**"));
		break;
	default:
		m_editGroupName.SetPromptText(_T("**讨论组名称**"));
		break;
	}
	CString sText;
	sText.Format(_T("%s名称："),GetGroupTypeText(m_nGroupType,true));
	this->GetDlgItem(IDC_STATIC_GROUP_NAME)->SetWindowText(sText);

	SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGroupInfo::OnDestroy()
{
	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
}

void CDlgGroupInfo::OnBnClickedOk()
{
	UpdateData();
	if (m_sGroupName.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DEP_NAME)->SetFocus();
		CString sText;
		sText.Format(_T("请输入%s名称！"),GetGroupTypeText(m_nGroupType));
		CDlgMessageBox::EbDoModal(this,sText,_T(""),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
	OnOK();
}

void CDlgGroupInfo::OnSize(UINT nType, int cx, int cy)
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

void CDlgGroupInfo::OnPaint()
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
		//Gdiplus::Image * pImage = theApp.m_image1Group->Clone();
		//graphics.DrawImage(pImage,PointF(10,10));
		//delete pImage;
		graphics.DrawImage(theApp.m_image1Group,PointF(10,10));
		// 写标题
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
		const Gdiplus::PointF pointTitle(40,13);
		CString sText;
		if (m_nGroupId==0)
			sText.Format(_T("新建%s"),GetGroupTypeText(m_nGroupType));
		else
			sText.Format(_T("修改%s ID:%lld"),GetGroupTypeText(m_nGroupType),m_nGroupId);
		USES_CONVERSION;
		graphics.DrawString(A2W_ACP(sText),-1,&fontEbTitle,pointTitle,&brushEbTitle);

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
		//CString sOutText = _T("群（部门）资料");
		//TextOut(m_hdcMemory, 15, 15, sOutText, sOutText.GetLength());
		//SelectObject(m_hdcMemory, pOldFond);

	}
}

void CDlgGroupInfo::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}
