// DlgEditAccSetting.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgEditAccSetting.h"


// CDlgEditAccSetting dialog

IMPLEMENT_DYNAMIC(CDlgEditAccSetting, CEbDialogBase)

CDlgEditAccSetting::CDlgEditAccSetting(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEditAccSetting::IDD, pParent)
{

}

CDlgEditAccSetting::~CDlgEditAccSetting()
{
}

void CDlgEditAccSetting::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLE_OUTENT_CALL, m_bEnableOutEntCall);
	DDX_Control(pDX, IDC_CHECK_ENABLE_OUTENT_ACCEPT, m_bEnableOutEntAccept);
	DDX_Control(pDX, IDC_CHECK_ENABLE_USER_CALL, m_bEnableUserCall);
	DDX_Control(pDX, IDC_CHECK_ENABLE_USER_ACCEPT, m_bEnableUserAccept);
	DDX_Control(pDX, IDC_CHECK_ENABLE_VISITOR_CALL, m_bEnableVisitorCall);
	DDX_Control(pDX, IDC_CHECK_ENABLE_VISITOR_ACCEPT, m_bEnableVisitorAccept);
	DDX_Control(pDX, IDC_CHECK_ENABLE_OFF_CALL, m_bEnableOffCall);
	DDX_Control(pDX, IDC_CHECK_ENABLE_OFF_FILE, m_bEnableOffFile);
	DDX_Control(pDX, IDC_CHECK_ENABLE_CONTACT_ACCEPT, m_bEnableContactAccept);
}


BEGIN_MESSAGE_MAP(CDlgEditAccSetting, CEbDialogBase)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CDlgEditAccSetting::SetCtrlColor(void)
{
	m_bEnableOutEntCall.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableOutEntCall.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableOutEntAccept.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableOutEntAccept.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableUserCall.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableUserCall.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableUserAccept.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableUserAccept.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableVisitorCall.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableVisitorCall.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableVisitorAccept.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableVisitorAccept.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableOffCall.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableOffCall.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableOffFile.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableOffFile.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_bEnableContactAccept.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_bEnableContactAccept.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
}

// CDlgEditAccSetting message handlers

BOOL CDlgEditAccSetting::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

	m_bEnableOutEntCall.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableOutEntCall.SetTextTop(-1);
	m_bEnableOutEntCall.SetTextLeft(16);
	m_bEnableOutEntCall.SetAlignText(CLabelEx::Align_Left);
	m_bEnableOutEntAccept.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableOutEntAccept.SetTextTop(-1);
	m_bEnableOutEntAccept.SetTextLeft(16);
	m_bEnableOutEntAccept.SetAlignText(CLabelEx::Align_Left);
	m_bEnableUserCall.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableUserCall.SetTextTop(-1);
	m_bEnableUserCall.SetTextLeft(16);
	m_bEnableUserCall.SetAlignText(CLabelEx::Align_Left);
	m_bEnableUserAccept.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableUserAccept.SetTextTop(-1);
	m_bEnableUserAccept.SetTextLeft(16);
	m_bEnableUserAccept.SetAlignText(CLabelEx::Align_Left);
	m_bEnableVisitorCall.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableVisitorCall.SetTextTop(-1);
	m_bEnableVisitorCall.SetTextLeft(16);
	m_bEnableVisitorCall.SetAlignText(CLabelEx::Align_Left);
	m_bEnableVisitorAccept.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableVisitorAccept.SetTextTop(-1);
	m_bEnableVisitorAccept.SetTextLeft(16);
	m_bEnableVisitorAccept.SetAlignText(CLabelEx::Align_Left);
	m_bEnableOffCall.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableOffCall.SetTextTop(-1);
	m_bEnableOffCall.SetTextLeft(16);
	m_bEnableOffCall.SetAlignText(CLabelEx::Align_Left);
	m_bEnableOffFile.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableOffFile.SetTextTop(-1);
	m_bEnableOffFile.SetTextLeft(16);
	m_bEnableOffFile.SetAlignText(CLabelEx::Align_Left);
	m_bEnableContactAccept.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_bEnableContactAccept.SetTextTop(-1);
	m_bEnableContactAccept.SetTextLeft(16);
	m_bEnableContactAccept.SetAlignText(CLabelEx::Align_Left);
	//m_bEnableContactAccept.EnableWindow(FALSE);	// ??
	SetCtrlColor();

	// 
	const int const_y	= 18;
	const int const_x1	= 45-28;	// 28是父窗口左边间隔
	//const int const_x2	= 133-28;
	const int const_static_width	= 80;
	const int const_edit_width		= 315;
	const int const_edit_height		= 21;
	const int const_height_interval1	= 33;
	const int const_height_interval2	= 23;
	int nX = const_x1;
	int nY = const_y;
	if (theApp.GetAuthContact())
	{
		this->GetDlgItem(IDC_STATIC_CONTACT)->ShowWindow(SW_HIDE);
		this->GetDlgItem(IDC_CHECK_ENABLE_CONTACT_ACCEPT)->ShowWindow(SW_HIDE);
	}else
	{
		this->GetDlgItem(IDC_STATIC_CONTACT)->MoveWindow(nX,nY,const_static_width,const_edit_height);
		nX += const_static_width;
		this->GetDlgItem(IDC_CHECK_ENABLE_CONTACT_ACCEPT)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
		nX = const_x1;
		nY += const_height_interval1;
	}
	// 外部企业用户
	this->GetDlgItem(IDC_STATIC_OUTENT)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_CHECK_ENABLE_OUTENT_CALL)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nY += const_height_interval2;
	this->GetDlgItem(IDC_CHECK_ENABLE_OUTENT_ACCEPT)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	// 注册用户
	nX = const_x1;
	nY += const_height_interval1;
	this->GetDlgItem(IDC_STATIC_REG)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_CHECK_ENABLE_USER_CALL)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nY += const_height_interval2;
	this->GetDlgItem(IDC_CHECK_ENABLE_USER_ACCEPT)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	// 游客
	nX = const_x1;
	nY += const_height_interval1;
	this->GetDlgItem(IDC_STATIC_VISITOR)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_CHECK_ENABLE_VISITOR_CALL)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nY += const_height_interval2;
	this->GetDlgItem(IDC_CHECK_ENABLE_VISITOR_ACCEPT)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	// 离线状态
	nX = const_x1;
	nY += const_height_interval1;
	this->GetDlgItem(IDC_STATIC_OFFLINE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_CHECK_ENABLE_OFF_FILE)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nY += const_height_interval2;
	this->GetDlgItem(IDC_CHECK_ENABLE_OFF_CALL)->MoveWindow(nX,nY,const_edit_width,const_edit_height);

	Load();
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditAccSetting::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgEditAccSetting::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}
void CDlgEditAccSetting::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgEditAccSetting::Load(void)
{
#ifdef USES_EBCOM_TEST
	long nSettingValue = theEBClientCore->EB_MyAccountSetting;
#else
	int nSettingValue = theEBAppClient.EB_GetMyAccountSetting();
#endif
	m_bEnableOutEntCall.SetCheck((nSettingValue&EB_SETTING_ENABLE_OUTENT_CALL)==EB_SETTING_ENABLE_OUTENT_CALL);
	m_bEnableOutEntAccept.SetCheck((nSettingValue&EB_SETTING_AUTO_OUTENT_ACCEPT)==EB_SETTING_AUTO_OUTENT_ACCEPT);
	m_bEnableUserCall.SetCheck((nSettingValue&EB_SETTING_ENABLE_USER_CALL)==EB_SETTING_ENABLE_USER_CALL);
	m_bEnableUserAccept.SetCheck((nSettingValue&EB_SETTING_AUTO_USER_ACCEPT)==EB_SETTING_AUTO_USER_ACCEPT);
	m_bEnableVisitorCall.SetCheck((nSettingValue&EB_SETTING_ENABLE_VISITOR_CALL)==EB_SETTING_ENABLE_VISITOR_CALL);
	m_bEnableVisitorAccept.SetCheck((nSettingValue&EB_SETTING_AUTO_VISITOR_ACCEPT)==EB_SETTING_AUTO_VISITOR_ACCEPT);
	m_bEnableOffCall.SetCheck((nSettingValue&EB_SETTING_ENABLE_OFF_CALL)==EB_SETTING_ENABLE_OFF_CALL);
	m_bEnableOffFile.SetCheck((nSettingValue&EB_SETTING_ENABLE_OFF_FILE)==EB_SETTING_ENABLE_OFF_FILE);
	m_bEnableContactAccept.SetCheck((nSettingValue&EB_SETTING_AUTO_CONTACT_ACCEPT)==EB_SETTING_AUTO_CONTACT_ACCEPT);
}

void CDlgEditAccSetting::Save(void)
{
	UpdateData();
	int nSettingValue = 0;
	nSettingValue |= m_bEnableOutEntCall.GetCheck()?EB_SETTING_ENABLE_OUTENT_CALL:0;
	nSettingValue |= m_bEnableOutEntAccept.GetCheck()?EB_SETTING_AUTO_OUTENT_ACCEPT:0;
	nSettingValue |= m_bEnableUserCall.GetCheck()?EB_SETTING_ENABLE_USER_CALL:0;
	nSettingValue |= m_bEnableUserAccept.GetCheck()?EB_SETTING_AUTO_USER_ACCEPT:0;
	nSettingValue |= m_bEnableVisitorCall.GetCheck()?EB_SETTING_ENABLE_VISITOR_CALL:0;
	nSettingValue |= m_bEnableVisitorAccept.GetCheck()?EB_SETTING_AUTO_VISITOR_ACCEPT:0;
	nSettingValue |= m_bEnableOffCall.GetCheck()?EB_SETTING_ENABLE_OFF_CALL:0;
	nSettingValue |= m_bEnableOffFile.GetCheck()?EB_SETTING_ENABLE_OFF_FILE:0;
	nSettingValue |= m_bEnableContactAccept.GetCheck()?EB_SETTING_AUTO_CONTACT_ACCEPT:0;
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_MyAccountSetting = nSettingValue;
#else
	theEBAppClient.EB_SetMyAccountSetting(nSettingValue);
#endif
}

