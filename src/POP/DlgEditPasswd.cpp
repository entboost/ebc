// DlgEditPasswd.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgEditPasswd.h"


// CDlgEditPasswd dialog

IMPLEMENT_DYNAMIC(CDlgEditPasswd, CEbDialogBase)

CDlgEditPasswd::CDlgEditPasswd(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEditPasswd::IDD, pParent)
{

}

CDlgEditPasswd::~CDlgEditPasswd()
{
}

void CDlgEditPasswd::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OLDPWD, m_editOldPwd);
	DDX_Control(pDX, IDC_EDIT_NEWPWD, m_editNewPwd);
	DDX_Control(pDX, IDC_EDIT_CONFIRM, m_editConfirm);
}


BEGIN_MESSAGE_MAP(CDlgEditPasswd, CEbDialogBase)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgEditPasswd message handlers

void CDlgEditPasswd::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgEditPasswd::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	Save();
	//CEbDialogBase::OnOK();
}

void CDlgEditPasswd::SetCtrlColor(void)
{
	m_editOldPwd.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editNewPwd.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editConfirm.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
}

BOOL CDlgEditPasswd::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

	const int const_y	= 18;
	const int const_x1	= 45-28;	// 28是父窗口左边间隔
	//const int const_x2	= 133-28;
	const int const_static_width	= 80;
	const int const_edit_width		= 315;
	const int const_edit_height		= 21;
	const int const_height_interval	= 33;
	int nX = const_x1;
	int nY = const_y;
	this->GetDlgItem(IDC_STATIC_OLDPWD)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_OLDPWD)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_NEWPWD)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_NEWPWD)->MoveWindow(nX,nY,const_edit_width,const_edit_height);
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_CONFIRM)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_CONFIRM)->MoveWindow(nX,nY,const_edit_width,const_edit_height);

	SetCtrlColor();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditPasswd::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgEditPasswd::Save(void)
{
#ifdef USES_EBCOM_TEST
	const CEBString sCurrentUserName = theEBClientCore->EB_UserName.GetBSTR();
	//const CEBString sCurrentPasswd = theEBClientCore->EB_Password.GetBSTR();
#else
	tstring sCurrentUserName = theEBAppClient.EB_GetUserName();
	//tstring sCurrentPasswd = theEBAppClient.EB_GetPassword();
#endif
	CString sOldPwd;
	this->GetDlgItem(IDC_EDIT_OLDPWD)->GetWindowText(sOldPwd);
	if (sOldPwd.IsEmpty())
	//if (sCurrentPasswd != (LPCTSTR)sOldPwd)
	{
		this->GetDlgItem(IDC_EDIT_OLDPWD)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("当前密码不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}
	CString sNewPwd;
	CString sConfirm;
	this->GetDlgItem(IDC_EDIT_NEWPWD)->GetWindowText(sNewPwd);
	this->GetDlgItem(IDC_EDIT_CONFIRM)->GetWindowText(sConfirm);
	if (sNewPwd.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_NEWPWD)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("新密码不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}else if (sNewPwd != sConfirm)
	{
		this->GetDlgItem(IDC_EDIT_NEWPWD)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("新密码不一致：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}else if (sOldPwd == sNewPwd)
	{
		this->GetDlgItem(IDC_EDIT_NEWPWD)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("新密码跟当前密码一样：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_SetPassword((LPCTSTR)sOldPwd,(LPCTSTR)sNewPwd);
	//theEBClientCore->EB_Password = (LPCTSTR)sNewPwd;
#else
	theEBAppClient.EB_SetPassword(sOldPwd,sNewPwd);
#endif
	if (theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("update user_login_record_t set password='%s' where account='%s' AND password='%s'"),sNewPwd,theApp.GetLogonAccount(),sOldPwd);
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->GetDlgItem(IDC_EDIT_OLDPWD)->SetWindowText(_T(""));
	//this->GetDlgItem(IDC_EDIT_NEWPWD)->SetWindowText(_T(""));
	//this->GetDlgItem(IDC_EDIT_CONFIRM)->SetWindowText(_T(""));
}


BOOL CDlgEditPasswd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==' ')
	{
		// 控制不能输入空格
		if (pMsg->hwnd == m_editOldPwd.GetSafeHwnd() ||
			pMsg->hwnd == m_editNewPwd.GetSafeHwnd() ||
			pMsg->hwnd == m_editConfirm.GetSafeHwnd())
		{
			return TRUE;
		}
	}

	return CEbDialogBase::PreTranslateMessage(pMsg);
}
