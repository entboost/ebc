// DlgUserInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgUserInfo.h"


// CDlgUserInfo dialog

IMPLEMENT_DYNAMIC(CDlgUserInfo, CEbDialogBase)

CDlgUserInfo::CDlgUserInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgUserInfo::IDD, pParent)
{

}

CDlgUserInfo::~CDlgUserInfo()
{
}

void CDlgUserInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_editAccount);
	DDX_Control(pDX, IDC_EDIT_TEL, m_editTel);
	DDX_Control(pDX, IDC_EDIT_PHONE, m_editPhone);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_JOBTITLE, m_editJobTitle);
	DDX_Control(pDX, IDC_EDIT_DEPARTMENT, m_editGroup);
	DDX_Control(pDX, IDC_EDIT_ENTERPRISE, m_editEnterprise);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
}


BEGIN_MESSAGE_MAP(CDlgUserInfo, CEbDialogBase)
	//ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgUserInfo message handlers

BOOL CDlgUserInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);
	this->SetBgColor(RGB(255,255,255));	// 白色背景

	// test
	//m_editName.Create(WS_CHILD|WS_VISIBLE|ES_LEFT|ES_READONLY, CRect(0,0,300, 12), this, 0xffff);
	//m_editName.SetWindowText(_T("陆游 13512345678"));
	//m_editName.ModifyStyle(0, ES_READONLY);
	//m_editName.SetReadOnly();
	//CEBAccountInfo::pointer pFromAccount = theEBAppClient.GetCallInfo(this->m_sCallId, this->m_sFromAccount); 

	//m_staName.SetBkColor(theDefaultFlatBgColor);	
	//m_staName.SetTextColor(theDefaultTextBlackColor);

	this->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str());
	this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_pFromAccountInfo.GetAccount().c_str());
	this->GetDlgItem(IDC_EDIT_TEL)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sTel.c_str());
	this->GetDlgItem(IDC_EDIT_PHONE)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
	this->GetDlgItem(IDC_EDIT_EMAIL)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sEmail.c_str());
	this->GetDlgItem(IDC_EDIT_JOBTITLE)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
	this->GetDlgItem(IDC_EDIT_DEPARTMENT)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str());
	this->GetDlgItem(IDC_EDIT_ENTERPRISE)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sEnterprise.c_str());
	this->GetDlgItem(IDC_EDIT_ADDRESS)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sAddress.c_str());

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUserInfo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_sta_left = 6;
	const int const_sta_top = 18;
	const int const_sta_width = 40;
	const int const_sta_height = 16;
	const int const_text_width = cx-const_sta_width-const_sta_left;
	const int const_line_intever = 6;					// 行间隔
	// 姓名
	int x = const_sta_left;
	int y = const_sta_top;
	if (this->GetDlgItem(IDC_STATIC_NAME)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_NAME)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (this->GetDlgItem(IDC_EDIT_NAME)->GetSafeHwnd())
	{
		x += (const_sta_width);
		this->GetDlgItem(IDC_EDIT_NAME)->MoveWindow(x, y, const_text_width, const_sta_height);
	}
	// 帐号
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_ACCOUNT)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_ACCOUNT)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editAccount.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editAccount.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	y += 8;	// 上下分隔
	// 电话
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_TEL)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_TEL)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editTel.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editTel.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	// 手机
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_PHONE)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_PHONE)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editPhone.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editPhone.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	// 邮箱
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_EMAIL)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_EMAIL)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editEmail.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editEmail.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	y += 8;	// 上下分隔
	// 职务
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_JOBTITLE)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_JOBTITLE)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editJobTitle.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editJobTitle.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	// 部门
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_DEPARTMENT)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_DEPARTMENT)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editGroup.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editGroup.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	// 公司
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_ENTERPRISE)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_ENTERPRISE)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editEnterprise.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editEnterprise.MoveWindow(x, y, const_text_width, const_sta_height);
	}
	y += 8;	// 上下分隔
	// 地址
	x = const_sta_left;
	y += (const_sta_height+const_line_intever);
	if (this->GetDlgItem(IDC_STATIC_ADDRESS)->GetSafeHwnd())
	{
		this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(x, y, const_sta_width, const_sta_height);
	}
	if (m_editAddress.GetSafeHwnd())
	{
		x += (const_sta_width);
		m_editAddress.MoveWindow(x, y, const_text_width, const_sta_height*3);
	}

}

void CDlgUserInfo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CDlgUserInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (this->GetParent()->GetParent()!=NULL && this->GetParent()->GetParent()->GetParent()!=NULL)
		this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgUserInfo::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgUserInfo::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgUserInfo::OnDestroy()
{
	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
}
HBRUSH CDlgUserInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(40,81,81));
	//pDC->SetBkColor(theDefaultFlatBgColor);
	return hbr;
}

//
//void CDlgUserInfo::InvalidateParentRect(UINT nID)
//{
//	CRect rect;
//	this->GetDlgItem(nID)->GetWindowRect(&rect);
//	this->GetParent()->ScreenToClient(&rect);
//	this->GetParent()->InvalidateRect(&rect);
//}

//void CDlgUserInfo::OnEnKillfocusEditName()
//{
//	InvalidateParentRect(IDC_EDIT_NAME);
//}

//void CDlgUserInfo::OnPaint()
//{
//	if (!IsIconic())
//	{
//		DrawInfo();
//	}
//}
//
//void CDlgUserInfo::DrawInfo(void)
//{
//	CPaintDC dc2(this); // 用于绘制的设备上下文
//	CRect rectClient;
//	this->GetClientRect(&rectClient);
//	CSkinMemDC memDC(&dc2, rectClient);
//	this->ClearBgDrawInfo();
//	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
//	this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
//
//	//Gdiplus::Graphics graphics(memDC.m_hDC);
//
//	//USES_CONVERSION;
//	//const FontFamily fontFamily(theFontFamily.c_str());
//	//const Gdiplus::Font fontTitle(&fontFamily, 16, FontStyleBold, UnitPixel);
//	//const Gdiplus::Font fontText(&fontFamily, 12, FontStyleRegular, UnitPixel);
//	//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(0,0,128));
//	//Gdiplus::SolidBrush brushEbText(Gdiplus::Color(38,38,38));
//	////graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
//	////const Gdiplus::PointF pointTitle(25,7);
//
//	////this->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str());
//	////this->GetDlgItem(IDC_EDIT_TEL)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
//	////this->GetDlgItem(IDC_EDIT_EMAIL)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sEmail.c_str());
//	////this->GetDlgItem(IDC_EDIT_JOBTITLE)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
//	////this->GetDlgItem(IDC_EDIT_DEPARTMENT)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str());
//	////this->GetDlgItem(IDC_EDIT_ENTERPRISE)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sEnterprise.c_str());
//	////this->GetDlgItem(IDC_EDIT_ADDRESS)->SetWindowText(m_pFromAccountInfo.m_pFromCardInfo.m_sAddress.c_str());
//
//	//int x = 5;
//	//int y = 18;
//	//CString sOutText;
//	//Gdiplus::StringFormat format;
//	//format.SetAlignment(Gdiplus::StringAlignmentNear);
//	//format.SetLineAlignment(Gdiplus::StringAlignmentNear);
//	////Gdiplus::StringFormat formatCenter;
//	////formatCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
//	////formatCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
//	//sOutText.Format(_T("%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,RectF(x,y,rectClient.Width()-2*x,20),&format,&brushEbTitle);
//	//y += 20;
//	//sOutText.Format(_T("%s"), m_pFromAccountInfo.GetAccount().c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,RectF(x,y,rectClient.Width()-2*x,20),&format,&brushEbText);
//	//y += 28;
//	//sOutText.Format(_T("电话：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sTel.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 20;
//	//sOutText.Format(_T("手机：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 20;
//	//sOutText.Format(_T("邮箱：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sEmail.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 28;
//	//sOutText.Format(_T("职务：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 20;
//	//sOutText.Format(_T("部门：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 20;
//	//sOutText.Format(_T("公司：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sEnterprise.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
//	//y += 28;
//	//sOutText.Format(_T("地址：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sAddress.c_str());
//	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,RectF(x,y,rectClient.Width()-x,rectClient.Height()-y),&format,&brushEbText);
//
//}

BOOL CDlgUserInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (pMsg->wParam==VK_UP)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				return TRUE;
			}
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}
