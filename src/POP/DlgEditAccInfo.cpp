// DlgEditAccInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgEditAccInfo.h"


// CDlgEditAccInfo dialog

IMPLEMENT_DYNAMIC(CDlgEditAccInfo, CEbDialogBase)

CDlgEditAccInfo::CDlgEditAccInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEditAccInfo::IDD, pParent)
{

}

CDlgEditAccInfo::~CDlgEditAccInfo()
{
}

void CDlgEditAccInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_editAccount);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
	DDX_Control(pDX, IDC_COMBO_GENDER, m_comboGender);
	DDX_Control(pDX, IDC_COMBO_BIR_YEAR, m_comboBirYear);
	DDX_Control(pDX, IDC_COMBO_BIR_MONTH, m_comboBirMonth);
	DDX_Control(pDX, IDC_COMBO_BIR_DAY, m_comboBirDay);
	DDX_Control(pDX, IDC_EDIT_CELL_PHONE, m_editCellPhone);
	DDX_Control(pDX, IDC_EDIT_WORK_PHONE, m_editWorkPhone);
	DDX_Control(pDX, IDC_EDIT_EMAIL2, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_MAINPAGE, m_editUrl);
	DDX_Control(pDX, IDC_COMBO_AREA1, m_comboArea1);
	DDX_Control(pDX, IDC_COMBO_AREA2, m_comboArea2);
	DDX_Control(pDX, IDC_COMBO_AREA3, m_comboArea3);
	DDX_Control(pDX, IDC_COMBO_AREA4, m_comboArea4);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_ZIPCODE, m_editZipCode);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgEditAccInfo, CEbDialogBase)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO_AREA1, &CDlgEditAccInfo::OnCbnSelchangeComboArea1)
	ON_CBN_SELCHANGE(IDC_COMBO_AREA2, &CDlgEditAccInfo::OnCbnSelchangeComboArea2)
	ON_CBN_SELCHANGE(IDC_COMBO_AREA3, &CDlgEditAccInfo::OnCbnSelchangeComboArea3)
	//ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CDlgEditAccInfo::CheckData(void)
{
	if (m_comboArea1.GetCount()<=1)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetAreaInfo(0,1);
#else
		theEBAppClient.EB_GetAreaInfo(0,1);
#endif
	}
	// 更新用户帐号
#ifdef USES_EBCOM_TEST
	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
#else
	const CEBString sUserName = theEBAppClient.EB_GetUserName();;
#endif
	m_pMyAccountInfo.SetUserName(sUserName);
	this->GetDlgItem(IDC_EDIT_USERNAME)->SetWindowText(m_pMyAccountInfo.GetUserName().c_str());
}

void CDlgEditAccInfo::SetCtrlColor(void)
{
	m_comboGender.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirYear.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirMonth.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirDay.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboArea1.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboArea2.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboArea3.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboArea4.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);

	m_editAccount.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editUserName.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editCellPhone.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editWorkPhone.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editEmail.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editUrl.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editAddress.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editZipCode.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editDescription.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
}

// CDlgEditAccInfo message handlers

BOOL CDlgEditAccInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

	m_brush.CreateSolidBrush(theDefaultFlatBgColor);

#ifdef USES_EBCOM_TEST
	if (theApp.GetDisableAccountEdit() || !theEBClientCore->EB_CanEditMyBaseAccountInfo())
#else
	if (theApp.GetDisableAccountEdit() || !theEBAppClient.EB_CanEditMyBaseAccountInfo())
#endif
	{
		//m_comboBirYear.ModifyStyle(CBS_DROPDOWN,0);
		//m_comboBirYear.ModifyStyle(0,CBS_DROPDOWNLIST);
		this->GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_GENDER)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_BIR_YEAR)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_BIR_MONTH)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_BIR_DAY)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_CELL_PHONE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_WORK_PHONE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_EMAIL2)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_MAINPAGE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_AREA1)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_AREA2)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_AREA3)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_COMBO_AREA4)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_ADDRESS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_ZIPCODE)->EnableWindow(FALSE);
	}

	m_editAccount.SetReadOnly(TRUE);
	//m_editAccount.EnableWindow(FALSE);

	//m_comboGender.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboGender.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboGender.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	//m_comboBirYear.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirYear.SetDoubleDrawHover(true);
	m_comboBirYear.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirYear.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	//m_comboBirMonth.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirMonth.SetDoubleDrawHover(true);
	m_comboBirMonth.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirMonth.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	//m_comboBirDay.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirDay.SetDoubleDrawHover(true);
	m_comboBirDay.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirDay.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	//m_comboArea1.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboArea1.SetDoubleDrawHover(true);
	m_comboArea1.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboArea1.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	//m_comboArea2.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboArea2.SetDoubleDrawHover(true);
	m_comboArea2.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboArea2.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	//m_comboArea3.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboArea3.SetDoubleDrawHover(true);
	m_comboArea3.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboArea3.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	//m_comboArea4.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboArea4.SetDoubleDrawHover(true);
	m_comboArea4.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboArea4.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	SetCtrlColor();

	// 性别
	m_comboGender.AddString(_T(""));
	m_comboGender.AddString(_T("男性"));
	m_comboGender.AddString(_T("女性"));
	CString stext;
	// 年
	for (int year=1960; year<=2012; year++)
	{
		stext.Format(_T("%04d年"), year);
		m_comboBirYear.AddString(stext);
	}
	// 月
	for (int month=1; month<=12; month++)
	{
		stext.Format(_T("%02d月"), month);
		m_comboBirMonth.AddString(stext);
	}
	// 日
	for (int day=1; day<=31; day++)
	{
		stext.Format(_T("%02d日"), day);
		m_comboBirDay.AddString(stext);
	}

	const int const_y	= 18;
	const int const_x1	= 45-28;	// 28是父窗口左边间隔
	//const int const_x2	= 133-28;
	const int const_combo_width		= 165;
	const int const_static_width	= 80;
	const int const_edit_width		= const_combo_width*2+const_static_width+10;
	const int const_edit_height		= 21;
	const int const_height_interval	= 33;
	// 帐号
	int nX = const_x1;
	int nY = const_y;
	this->GetDlgItem(IDC_STATIC_ACCOUNT)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_ACCOUNT)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 名称
	nX += (const_combo_width+10);
	this->GetDlgItem(IDC_STATIC_NAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_USERNAME)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 性别
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_GENDER)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_COMBO_GENDER)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 生日
	nX += (const_combo_width+10);
	//nX = const_x1;
	//nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_BIRTHDAY)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += 80;
	this->GetDlgItem(IDC_COMBO_BIR_YEAR)->MoveWindow(nX,nY,66,const_edit_height);
	nX += 65;
	this->GetDlgItem(IDC_COMBO_BIR_MONTH)->MoveWindow(nX,nY,51,const_edit_height);
	nX += 50;
	this->GetDlgItem(IDC_COMBO_BIR_DAY)->MoveWindow(nX,nY,50,const_edit_height);
	// 手机
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_CELL_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_CELL_PHONE)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 电话
	nX += (const_combo_width+10);
	this->GetDlgItem(IDC_STATIC_WORK_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_WORK_PHONE)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 邮箱
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_EMAIL2)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_EMAIL2)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 主页
	nX += (const_combo_width+10);
	this->GetDlgItem(IDC_STATIC_URL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_MAINPAGE)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 地区
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_AREA)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_COMBO_AREA1)->MoveWindow(nX,nY,83,const_edit_height);	// 83+83-1=165=const_combo_width
	nX += 82;
	this->GetDlgItem(IDC_COMBO_AREA2)->MoveWindow(nX,nY,83,const_edit_height);
	nX += 82;
	this->GetDlgItem(IDC_COMBO_AREA3)->MoveWindow(nX,nY,91,const_edit_height);
	nX += 90;
	this->GetDlgItem(IDC_COMBO_AREA4)->MoveWindow(nX,nY,80,const_edit_height);
	// 地址
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_combo_width,const_edit_height);
	// 邮编
	nX += (const_combo_width+10);
	this->GetDlgItem(IDC_STATIC_ZIPCODE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_ZIPCODE)->MoveWindow(nX,nY,const_combo_width,const_edit_height);

	// 备注
	nX = const_x1;
	nY += const_height_interval;
	this->GetDlgItem(IDC_STATIC_DESCRIPTION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_static_width;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(nX,nY,const_edit_width,60);

	Load();
	CheckData();
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditAccInfo::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgEditAccInfo::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgEditAccInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgEditAccInfo::OnAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter)
{
	CBitComboBox * pComboArea = NULL;
	switch(nParameter)
	{
	case 1:
		{
			// 国家或地区
			pComboArea = &m_comboArea1;
		}break;
	case 2:
		{
			// 省或地区
			pComboArea = &m_comboArea2;
		}break;
	case 3:
		{
			// 市或地区
			pComboArea = &m_comboArea3;
		}break;
	case 4:
		{
			// 地区
			pComboArea = &m_comboArea4;
		}break;
	default:
		return;
		break;
	}
	const int nFind = pComboArea->FindString(0,pAreaInfo->m_sAreaName.c_str());
	if (nFind<0)
	{
		const int nIndex = pComboArea->AddString(pAreaInfo->m_sAreaName.c_str());
		pComboArea->SetItemData(nIndex,pAreaInfo->m_nAreaCode);
	}

}

void CDlgEditAccInfo::Load(void)
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_AccountInfo> pOutInterface = theEBClientCore->EB_GetMyAccountInfo();
	m_pMyAccountInfo = pOutInterface;
#else
	theEBAppClient.EB_GetMyAccountInfo(&m_pMyAccountInfo);
#endif

	this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_pMyAccountInfo.GetAccount().c_str());
	this->GetDlgItem(IDC_EDIT_USERNAME)->SetWindowText(m_pMyAccountInfo.GetUserName().c_str());
	m_comboGender.SetCurSel((int)m_pMyAccountInfo.GetGender());
	
	CString stext;
	const int m_nBirthday = m_pMyAccountInfo.GetBirthday();
	const int nBirYear = m_nBirthday/10000;
	if (nBirYear > 0)
	{
		stext.Format(_T("%04d年"), nBirYear);
		//m_comboBirYear.SetWindowText(stext);
		int nIndex = m_comboBirYear.FindString(0,stext);
		if (nIndex<0)
		{
			nIndex = m_comboBirYear.AddString(stext);
		}
		m_comboBirYear.SetCurSel(nIndex);
	}
	const int nBirMonth = (m_nBirthday%10000)/100;
	if (nBirMonth > 0)
	{
		stext.Format(_T("%02d月"), nBirMonth);
		//m_comboBirMonth.SetWindowText(stext);
		int nIndex = m_comboBirMonth.FindString(0,stext);
		if (nIndex<0)
		{
			nIndex = m_comboBirMonth.AddString(stext);
		}
		m_comboBirMonth.SetCurSel(nIndex);
	}
	const int nBirDay = m_nBirthday%100;
	if (nBirDay > 0)
	{
		stext.Format(_T("%02d日"), nBirDay);
		//m_comboBirDay.SetWindowText(stext);
		int nIndex = m_comboBirDay.FindString(0,stext);
		if (nIndex<0)
		{
			nIndex = m_comboBirDay.AddString(stext);
		}
		m_comboBirDay.SetCurSel(nIndex);
	}
	
	this->GetDlgItem(IDC_EDIT_CELL_PHONE)->SetWindowText(m_pMyAccountInfo.GetMobile().c_str());
	this->GetDlgItem(IDC_EDIT_WORK_PHONE)->SetWindowText(m_pMyAccountInfo.GetTel().c_str());
	this->GetDlgItem(IDC_EDIT_EMAIL2)->SetWindowText(m_pMyAccountInfo.GetEmail().c_str());
	if (!m_pMyAccountInfo.GetArea1String().empty())
	{
		int nIndex = m_comboArea1.FindString(0,m_pMyAccountInfo.GetArea1String().c_str());
		if (nIndex<0)
		{
			nIndex = m_comboArea1.AddString(m_pMyAccountInfo.GetArea1String().c_str());
			m_comboArea1.SetItemData(nIndex,(DWORD)m_pMyAccountInfo.GetArea1());
		}
		m_comboArea1.SetCurSel(nIndex);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetAreaInfo(m_pMyAccountInfo.GetArea1(),2);
#else
		theEBAppClient.EB_GetAreaInfo(m_pMyAccountInfo.GetArea1(),2);
#endif
	}
	if (!m_pMyAccountInfo.GetArea2String().empty())
	{
		int nIndex = m_comboArea2.FindString(0,m_pMyAccountInfo.GetArea2String().c_str());
		if (nIndex<0)
		{
			nIndex = m_comboArea2.AddString(m_pMyAccountInfo.GetArea2String().c_str());
			m_comboArea2.SetItemData(nIndex,(DWORD)m_pMyAccountInfo.GetArea2());
		}
		m_comboArea2.SetCurSel(nIndex);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetAreaInfo(m_pMyAccountInfo.GetArea2(),3);
#else
		theEBAppClient.EB_GetAreaInfo(m_pMyAccountInfo.GetArea2(),3);
#endif
	}
	if (!m_pMyAccountInfo.GetArea3String().empty())
	{
		int nIndex = m_comboArea3.FindString(0,m_pMyAccountInfo.GetArea3String().c_str());
		if (nIndex<0)
		{
			nIndex = m_comboArea3.AddString(m_pMyAccountInfo.GetArea3String().c_str());
			m_comboArea3.SetItemData(nIndex,(DWORD)m_pMyAccountInfo.GetArea3());
		}
		m_comboArea3.SetCurSel(nIndex);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetAreaInfo(m_pMyAccountInfo.GetArea3(),4);
#else
		theEBAppClient.EB_GetAreaInfo(m_pMyAccountInfo.GetArea3(),4);
#endif
	}
	if (!m_pMyAccountInfo.GetArea4String().empty())
	{
		int nIndex = m_comboArea1.FindString(0,m_pMyAccountInfo.GetArea4String().c_str());
		if (nIndex<0)
		{
			nIndex = m_comboArea4.AddString(m_pMyAccountInfo.GetArea4String().c_str());
			m_comboArea4.SetItemData(nIndex,(DWORD)m_pMyAccountInfo.GetArea4());
		}
		m_comboArea4.SetCurSel(nIndex);
	}
	this->GetDlgItem(IDC_EDIT_MAINPAGE)->SetWindowText(m_pMyAccountInfo.GetUrl().c_str());
	this->GetDlgItem(IDC_EDIT_ADDRESS)->SetWindowText(m_pMyAccountInfo.GetAddress().c_str());
	this->GetDlgItem(IDC_EDIT_ZIPCODE)->SetWindowText(m_pMyAccountInfo.GetZipCode().c_str());
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(m_pMyAccountInfo.GetDescription().c_str());

	//this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(theEBAppClient.EB_GetLogonAccount().c_str());
	//this->GetDlgItem(IDC_EDIT_USERNAME)->SetWindowText(theEBAppClient.EB_GetUserName().c_str());
	//this->GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(theEBAppClient.EB_GetDescription().c_str());
}

void CDlgEditAccInfo::Save(void)
{
	CString sUserName;
	this->GetDlgItem(IDC_EDIT_USERNAME)->GetWindowText(sUserName);
	if (sUserName.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("个人名称不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}
	CString sDescription;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->GetWindowText(sDescription);

	// 计算生日整数
	CString stext;
	m_comboBirYear.GetWindowText(stext);
	const int nBirYear = atoi(stext);
	m_comboBirMonth.GetWindowText(stext);
	const int nBirMonth = atoi(stext);
	m_comboBirDay.GetWindowText(stext);
	const int nBirDay = atoi(stext);
	const int m_nBirthday = nBirYear*10000+nBirMonth*100+nBirDay;

	CString sMobile;
	this->GetDlgItem(IDC_EDIT_CELL_PHONE)->GetWindowText(sMobile);
	CString sTel;
	this->GetDlgItem(IDC_EDIT_WORK_PHONE)->GetWindowText(sTel);
	CString sEmail;
	this->GetDlgItem(IDC_EDIT_EMAIL2)->GetWindowText(sEmail);

	const int nArea1 = m_comboArea1.GetCurSel()<0?0:m_comboArea1.GetItemData(m_comboArea1.GetCurSel());
	const int nArea2 = m_comboArea2.GetCurSel()<0?0:m_comboArea2.GetItemData(m_comboArea2.GetCurSel());
	const int nArea3 = m_comboArea3.GetCurSel()<0?0:m_comboArea3.GetItemData(m_comboArea3.GetCurSel());
	const int nArea4 = m_comboArea4.GetCurSel()<0?0:m_comboArea4.GetItemData(m_comboArea4.GetCurSel());

	CString sAddress;
	this->GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(sAddress);
	CString sUrl;
	this->GetDlgItem(IDC_EDIT_MAINPAGE)->GetWindowText(sUrl);
	CString sZipCode;
	this->GetDlgItem(IDC_EDIT_ZIPCODE)->GetWindowText(sZipCode);

#ifdef USES_EBCOM_TEST

	//const CEBString sCurrentUserName = theEBClientCore->EB_UserName.GetBSTR();
	//const CEBString sCurrentDescription = theEBClientCore->EB_Description.GetBSTR();
	//if (sCurrentUserName != (LPCTSTR)sUserName)
	//{
	//	theEBClientCore->EB_UserName = (LPCTSTR)sUserName;
	//}
	//if (sCurrentDescription != (LPCTSTR)sDescription)
	//{
	//	theEBClientCore->EB_Description = (LPCTSTR)sDescription;
	//}
#else
	theEBAppClient.EB_GetMyAccountInfo(&m_pMyAccountInfo);
	if (theApp.GetDisableAccountEdit() || !theEBAppClient.EB_CanEditMyBaseAccountInfo())
	{
	}else
	{
		m_pMyAccountInfo.SetUserName((LPCTSTR)sUserName);
		m_pMyAccountInfo.SetGender((EB_GENDER_TYPE)m_comboGender.GetCurSel());
		m_pMyAccountInfo.SetBirthday(m_nBirthday);
		m_pMyAccountInfo.SetMobile((LPCTSTR)sMobile);
		m_pMyAccountInfo.SetTel((LPCTSTR)sTel);
		m_pMyAccountInfo.SetEmail((LPCTSTR)sEmail);
		m_pMyAccountInfo.SetArea1(nArea1);
		m_pMyAccountInfo.SetArea2(nArea2);
		m_pMyAccountInfo.SetArea3(nArea3);
		m_pMyAccountInfo.SetArea4(nArea4);
		m_pMyAccountInfo.SetUrl((LPCTSTR)sUrl);
		m_pMyAccountInfo.SetAddress((LPCTSTR)sAddress);
		m_pMyAccountInfo.SetZipCode((LPCTSTR)sZipCode);
	}
	m_pMyAccountInfo.SetDescription((LPCTSTR)sDescription);
	theEBAppClient.EB_SetMyAccountInfo(&m_pMyAccountInfo);

	//tstring sCurrentUserName = theEBAppClient.EB_GetUserName();
	//tstring sCurrentDescription = theEBAppClient.EB_GetDescription();
	//if (sCurrentUserName != (LPCTSTR)sUserName)
	//{
	//	theEBAppClient.EB_SetUserName(sUserName);
	//}
	//if (sCurrentDescription != (LPCTSTR)sDescription)
	//{
	//	theEBAppClient.EB_SetDescription(sDescription);
	//}
#endif
}

void CDlgEditAccInfo::OnCbnSelchangeComboArea1()
{
	const int nIndex = m_comboArea1.GetCurSel();
	if (nIndex<0) return;
	const int nAreaCode = m_comboArea1.GetItemData(nIndex);
	m_comboArea2.ResetContent();
	m_comboArea3.ResetContent();
	m_comboArea4.ResetContent();
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_GetAreaInfo(nAreaCode,2);
#else
	theEBAppClient.EB_GetAreaInfo(nAreaCode,2);
#endif
}

void CDlgEditAccInfo::OnCbnSelchangeComboArea2()
{
	const int nIndex = m_comboArea2.GetCurSel();
	if (nIndex<0) return;
	const int nAreaCode = m_comboArea2.GetItemData(nIndex);
	m_comboArea3.ResetContent();
	m_comboArea4.ResetContent();
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_GetAreaInfo(nAreaCode,3);
#else
	theEBAppClient.EB_GetAreaInfo(nAreaCode,3);
#endif
}

void CDlgEditAccInfo::OnCbnSelchangeComboArea3()
{
	const int nIndex = m_comboArea3.GetCurSel();
	if (nIndex<0) return;
	const int nAreaCode = m_comboArea3.GetItemData(nIndex);
	m_comboArea4.ResetContent();
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_GetAreaInfo(nAreaCode,4);
#else
	theEBAppClient.EB_GetAreaInfo(nAreaCode,4);
#endif
}

HBRUSH CDlgEditAccInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//if (pWnd!=NULL && pWnd->GetSafeHwnd()==m_comboBirYear.GetSafeHwnd())
	//{
	//	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//}
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//if (pWnd->GetDlgCtrlID()==m_comboBirYear.GetDlgCtrlID())
	//{
	//	// 
	//	int i=0;
	//}
	//if (nCtlColor == CTLCOLOR_STATIC)
	//{
	//	pDC->SetBkMode(TRANSPARENT);
	//	return (HBRUSH)m_brush.m_hObject;
	//	pDC->SetBkMode(TRANSPARENT);
	//	return (HBRUSH)::GetStockObject(NULL_BRUSH);
	//}

	return CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
}
