// DlgEditInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgEditInfo.h"
#include "DlgEditAccInfo.h"
#include "DlgEditAccSetting.h"
#include "DlgEditPasswd.h"
#include "DlgEditVideo.h"


// CDlgEditInfo dialog

IMPLEMENT_DYNAMIC(CDlgEditInfo, CEbDialogBase)

CDlgEditInfo::CDlgEditInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEditInfo::IDD, pParent)
{
	m_pCurrentLabel = NULL;
	m_pDlgEditAccInfo = NULL;
	m_pDlgEditAccSetting = NULL;
	m_pDlgEditPasswd = NULL;
	m_pDlgEditVideo = NULL;
}

CDlgEditInfo::~CDlgEditInfo()
{
}

void CDlgEditInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_ACCINFO, m_btnAccInfo);
	DDX_Control(pDX, IDC_BUTTON_ACCSETTING, m_btnAccSetting);
	DDX_Control(pDX, IDC_BUTTON_PASSWD, m_btnPasswd);
	DDX_Control(pDX, IDC_BUTTON_VIDEO, m_btnVideo);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT_EMP, m_btnDefaultEmp);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

}


BEGIN_MESSAGE_MAP(CDlgEditInfo, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgEditInfo::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_ACCINFO, &CDlgEditInfo::OnBnClickedButtonAccinfo)
	ON_BN_CLICKED(IDC_BUTTON_ACCSETTING, &CDlgEditInfo::OnBnClickedButtonAccSetting)
	ON_BN_CLICKED(IDC_BUTTON_PASSWD, &CDlgEditInfo::OnBnClickedButtonPasswd)
	ON_BN_CLICKED(IDOK, &CDlgEditInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO, &CDlgEditInfo::OnBnClickedButtonVideo)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditInfo::OnBnClickedCancel)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_EMP, &CDlgEditInfo::OnBnClickedButtonDefaultEmp)
END_MESSAGE_MAP()

void CDlgEditInfo::OnAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter)
{
	if (m_pDlgEditAccInfo!=NULL)
		m_pDlgEditAccInfo->OnAreaInfo(pAreaInfo,nParameter);
}

void CDlgEditInfo::CheckData(void)
{
	if (m_pDlgEditAccInfo!=NULL)
		m_pDlgEditAccInfo->CheckData();
}

void CDlgEditInfo::SetCtrlColor(void)
{
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnDefaultEmp.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());

	m_btnOk.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetNormalColor());
	m_btnCancel.SetHotTextColor(theApp.GetNormalColor()); 
	m_btnCancel.SetPreTextColor(theApp.GetNormalColor()); 

	if (m_pDlgEditAccInfo!=NULL)
		m_pDlgEditAccInfo->SetCtrlColor();
	if (m_pDlgEditAccSetting!=NULL)
		m_pDlgEditAccSetting->SetCtrlColor();
	if (m_pDlgEditPasswd!=NULL)
		m_pDlgEditPasswd->SetCtrlColor();
	if (m_pDlgEditVideo!=NULL)
		m_pDlgEditVideo->SetCtrlColor();
}

// CDlgEditInfo message handlers

BOOL CDlgEditInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	SetTransparentType(CEbDialogBase::TT_DLG);

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));

	//m_btnAccInfo.SetAutoSize(true);
	m_btnAccInfo.SetWindowText(_T("帐号信息"));
	m_btnAccInfo.Load(IDB_PNG_TAB_ACCINFO);
	m_btnAccInfo.SetChecked(true);
	m_btnAccInfo.SetTextTop(35);
	m_btnAccSetting.SetWindowText(_T("个人设置"));
	m_btnAccSetting.Load(IDB_PNG_TAB_ACCSETTING);
	m_btnAccSetting.SetTextTop(35);
	m_btnPasswd.SetWindowText(_T("修改密码"));
	m_btnPasswd.Load(IDB_PNG_TAB_SETPWD);
	m_btnPasswd.SetTextTop(35);
	m_btnVideo.SetWindowText(_T("视频设置"));
	m_btnVideo.Load(IDB_PNG_TAB_SETVIDEO);
	m_btnVideo.SetTextTop(35);

	m_btnDefaultEmp.SetTextHotMove(false);
	m_btnDefaultEmp.SetToolTipText(_T("显示默认名片信息"));
	m_btnDefaultEmp.SetNorTextColor(RGB(0,128,255));
	m_btnDefaultEmp.SetHotTextColor(RGB(255,255,255));
	m_btnDefaultEmp.SetPreTextColor(RGB(255,255,255));
	m_btnDefaultEmp.SetWindowText(_T("默认名片"));
	eb::bigint nMyDefaultmemberCode = 0;
	if (!theEBAppClient.EB_GetMyDefaultMemberCode(nMyDefaultmemberCode) || nMyDefaultmemberCode==0)
	{
		m_btnDefaultEmp.ShowWindow(SW_HIDE);
	}

	m_btnOk.SetTextHotMove(false);
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetTextHotMove(false);
	SetCtrlColor();

	if (theApp.GetDisableModifyPasswd())
	{
		m_btnPasswd.ShowWindow(SW_HIDE);
	}
	if (theApp.IsLogonVisitor())
	{
		// 游客
		m_btnAccSetting.ShowWindow(SW_HIDE);
		m_btnPasswd.ShowWindow(SW_HIDE);
		//m_btnAccSetting.EnableWindow(FALSE);
		//m_btnPasswd.EnableWindow(FALSE);
	}

	m_pDlgEditAccInfo = new CDlgEditAccInfo(this);
	m_pDlgEditAccInfo->SetCircle(false);
	m_pDlgEditAccInfo->Create(CDlgEditAccInfo::IDD,this);
	m_pDlgEditAccInfo->ShowWindow(SW_SHOW);

	this->SetWindowText(_T("个人中心"));

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditInfo::OnPaint()
{
	if (!IsIconic())
	{
		DrawInfo();
	}
}
void CDlgEditInfo::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(106,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, 106,2);

	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(106,theDefaultBgColorTitleLight1,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(106,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(296,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	//Gdiplus::Image * pImage;
	//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),pImage))
	//{
	//	graphics.DrawImage(pImage,PointF(10,7));
	//	delete pImage;
	//}
	graphics.DrawImage(theApp.m_image1People,PointF(10,7));
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::PointF pointTitle(40,11);
	graphics.DrawString(L"个人中心",-1,&fontEbTitle,pointTitle,&brushEbTitle);
}

void CDlgEditInfo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	SetCircle();

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	const int const_labelbtn_width  = m_btnAccInfo.GetImgWidth();
	const int const_labelbtn_height  = m_btnAccInfo.GetImgHeight();
	const int const_left  = 28;
	const int const_top  = 37;
	int y = const_top;
	int x = const_left;
	m_btnAccInfo.MovePoint(x,y);
	x += const_labelbtn_width;
	if (!theApp.IsLogonVisitor())
	{
		m_btnAccSetting.MovePoint(x,y);
		x += const_labelbtn_width;
	}
	if (!theApp.IsLogonVisitor() && !theApp.GetDisableModifyPasswd())
	{
		m_btnPasswd.MovePoint(x,y);
		x += const_labelbtn_width;
	}
	m_btnVideo.MovePoint(x,y);
	x = const_left;		// 10间隔
	y += const_labelbtn_height;
	const int const_dlg_width = cx-x-const_left;
	const int const_dlg_height = cy-y-50;	// 50最下面间隔
	if (m_pDlgEditAccInfo != NULL && m_pDlgEditAccInfo->GetSafeHwnd())
	{
		m_pDlgEditAccInfo->MoveWindow(x,y,const_dlg_width,const_dlg_height);
	}

	x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;					// 最右边间隔
	const int const_btn_y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	m_btnOk.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	//const int const_btn_width = 81;//m_btnOk.GetImgWidth();// 60;
	//const int const_btn_height = 32;//m_btnOk.GetImgHeight();// 24;
	//x = cx-const_btn_width-10;						// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+6);						// 按钮间隔
	//m_btnOk.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
	x = 20;
	m_btnDefaultEmp.MovePoint(x,const_btn_y,60,22);
}

void CDlgEditInfo::OnDestroy()
{
	CEbDialogBase::OnDestroy();
	if (m_pDlgEditAccInfo)
	{
		m_pDlgEditAccInfo->DestroyWindow();
		delete m_pDlgEditAccInfo;
		m_pDlgEditAccInfo = NULL;
	}
	if (m_pDlgEditAccSetting)
	{
		m_pDlgEditAccSetting->DestroyWindow();
		delete m_pDlgEditAccSetting;
		m_pDlgEditAccSetting = NULL;
	}
	if (m_pDlgEditPasswd)
	{
		m_pDlgEditPasswd->DestroyWindow();
		delete m_pDlgEditPasswd;
		m_pDlgEditPasswd = NULL;
	}
	if (m_pDlgEditVideo)
	{
		m_pDlgEditVideo->DestroyWindow();
		delete m_pDlgEditVideo;
		m_pDlgEditVideo = NULL;
	}
	
}

void CDlgEditInfo::OnBnClickedButtonClose()
{
	//if (m_pDlgEditAccSetting)
	//	m_pDlgEditAccSetting->Load();

	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgEditInfo::RefreshLabelWindow(void)
{
	if ((m_btnAccInfo.GetChecked()&&m_btnAccInfo.GetSafeHwnd()!=m_pCurrentLabel)
		|| (!m_btnAccInfo.GetChecked()&&m_btnAccInfo.GetSafeHwnd()==m_pCurrentLabel))
	{
		m_btnAccInfo.SetChecked(m_btnAccInfo.GetSafeHwnd()==m_pCurrentLabel);
		m_btnAccInfo.Invalidate();
		theApp.InvalidateParentRect(&m_btnAccInfo);
	}
	if ((m_btnAccSetting.GetChecked()&&m_btnAccSetting.GetSafeHwnd()!=m_pCurrentLabel)
		|| (!m_btnAccSetting.GetChecked()&&m_btnAccSetting.GetSafeHwnd()==m_pCurrentLabel))
	{
		m_btnAccSetting.SetChecked(m_btnAccSetting.GetSafeHwnd()==m_pCurrentLabel);
		m_btnAccSetting.Invalidate();
		theApp.InvalidateParentRect(&m_btnAccSetting);
	}
	if ((m_btnPasswd.GetChecked()&&m_btnPasswd.GetSafeHwnd()!=m_pCurrentLabel)
		|| (!m_btnPasswd.GetChecked()&&m_btnPasswd.GetSafeHwnd()==m_pCurrentLabel))
	{
		m_btnPasswd.SetChecked(m_btnPasswd.GetSafeHwnd()==m_pCurrentLabel);
		m_btnPasswd.Invalidate();
		theApp.InvalidateParentRect(&m_btnPasswd);
	}
	if ((m_btnVideo.GetChecked()&&m_btnVideo.GetSafeHwnd()!=m_pCurrentLabel)
		|| (!m_btnVideo.GetChecked()&&m_btnVideo.GetSafeHwnd()==m_pCurrentLabel))
	{
		m_btnVideo.SetChecked(m_btnVideo.GetSafeHwnd()==m_pCurrentLabel);
		m_btnVideo.Invalidate();
		theApp.InvalidateParentRect(&m_btnVideo);
	}
	m_pDlgEditAccInfo->ShowWindow(m_btnAccInfo.GetChecked()?SW_SHOW:SW_HIDE);
	if (m_pDlgEditAccSetting != NULL)
		m_pDlgEditAccSetting->ShowWindow(m_btnAccSetting.GetChecked()?SW_SHOW:SW_HIDE);
	if (m_pDlgEditPasswd != NULL)
		m_pDlgEditPasswd->ShowWindow(m_btnPasswd.GetChecked()?SW_SHOW:SW_HIDE);
	if (m_pDlgEditVideo != NULL)
		m_pDlgEditVideo->ShowWindow(m_btnVideo.GetChecked()?SW_SHOW:SW_HIDE);
}

void CDlgEditInfo::OnBnClickedButtonAccinfo()
{
	m_pCurrentLabel = m_btnAccInfo.GetSafeHwnd();
	RefreshLabelWindow();
	m_pDlgEditAccInfo->SetFocus();
}
void CDlgEditInfo::OnBnClickedButtonAccSetting()
{
	if (theApp.IsLogonVisitor()) return;

	if (m_pDlgEditAccSetting == NULL)
	{
		m_pDlgEditAccSetting = new CDlgEditAccSetting(this);
		m_pDlgEditAccSetting->Create(CDlgEditAccSetting::IDD,this);
		CRect rect;
		m_pDlgEditAccInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		m_pDlgEditAccSetting->MoveWindow(&rect);
	}
	m_pCurrentLabel = m_btnAccSetting.GetSafeHwnd();
	RefreshLabelWindow();
	m_pDlgEditAccSetting->SetFocus();
}

void CDlgEditInfo::OnBnClickedButtonPasswd()
{
	if (theApp.IsLogonVisitor() || theApp.GetDisableModifyPasswd()) return;

	if (m_pDlgEditPasswd == NULL)
	{
		m_pDlgEditPasswd = new CDlgEditPasswd(this);
		m_pDlgEditPasswd->Create(CDlgEditPasswd::IDD,this);
		CRect rect;
		m_pDlgEditAccInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		m_pDlgEditPasswd->MoveWindow(&rect);
	}
	m_pCurrentLabel = m_btnPasswd.GetSafeHwnd();
	RefreshLabelWindow();
	m_pDlgEditPasswd->SetFocus();
}
void CDlgEditInfo::OnBnClickedButtonVideo()
{
	if (m_pDlgEditVideo == NULL)
	{
		m_pDlgEditVideo = new CDlgEditVideo(this);
		m_pDlgEditVideo->Create(CDlgEditVideo::IDD,this);
		CRect rect;
		m_pDlgEditAccInfo->GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		m_pDlgEditVideo->MoveWindow(&rect);
	}
	m_pCurrentLabel = m_btnVideo.GetSafeHwnd();
	RefreshLabelWindow();
	m_pDlgEditVideo->SetFocus();
}

void CDlgEditInfo::OnBnClickedOk()
{
	if (m_btnAccInfo.GetChecked())
	{
		// 保存帐号信息
		m_pDlgEditAccInfo->Save();
	}else if (m_btnAccSetting.GetChecked() && m_pDlgEditAccSetting != NULL)
	{
		// 修改个人设置
		m_pDlgEditAccSetting->Save();
	}else if (m_btnPasswd.GetChecked() && m_pDlgEditPasswd != NULL)
	{
		// 修改密码
		m_pDlgEditPasswd->Save();
	}
}


void CDlgEditInfo::OnBnClickedCancel()
{
	if (m_pDlgEditVideo != NULL && m_pDlgEditVideo->GetSafeHwnd())
	{
		m_pDlgEditVideo->CloseVideo();
	}
	CEbDialogBase::OnCancel();
}

void CDlgEditInfo::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
	if (m_pDlgEditVideo != NULL && m_pDlgEditVideo->GetSafeHwnd())
	{
		m_pDlgEditVideo->OnMove();
	}
}

void CDlgEditInfo::OnBnClickedButtonDefaultEmp()
{
	EB_MemberInfo pMemberInfo;
	if (theEBAppClient.EB_GetMyDefaultMemberInfo(&pMemberInfo))
	{
		theApp.EditEmployeeInfo(this,&pMemberInfo);
	}
}
