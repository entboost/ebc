// DlgContactInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgContactInfo.h"
#include "DlgRequestAddContact.h"

//#define TIMERID_LOAD_CONTACT_LIST 200
#define TIMERID_LOAD_UGINFO_LIST 201

// CDlgContactInfo dialog

IMPLEMENT_DYNAMIC(CDlgContactInfo, CEbDialogBase)

CDlgContactInfo::CDlgContactInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgContactInfo::IDD, pParent)
	, m_sContact(_T(""))
	//, m_sGroup(_T(""))
	, m_sName(_T(""))
	, m_sPhone(_T(""))
	, m_sCompany(_T(""))
	, m_sJobTitle(_T(""))
	, m_sUrl(_T(""))
	, m_sTel(_T(""))
	, m_sFax(_T(""))
	, m_sEmail(_T(""))
	, m_sAddress(_T(""))
	, m_sDescription(_T(""))
{
	//m_nContactId = 0;
	m_nContactType = 0;
	m_nContactUserId = 0;
	m_nUGId = 0;
}

CDlgContactInfo::~CDlgContactInfo()
{
}

void CDlgContactInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_CONTACT_ACCOUNT, m_sContact);
	//DDX_CBString(pDX, IDC_COMBO_CONTACT, m_sContact);
	//DDX_Control(pDX, IDC_COMBO_CONTACT, m_comboContact);
	//DDX_CBString(pDX, IDC_COMBO_GROUP, m_sGroup);
	DDX_Control(pDX, IDC_COMBO_GROUP, m_comboGroup);
	DDX_Text(pDX, IDC_EDIT_CONTACT_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_CONTACT_PHONE, m_sPhone);
	DDX_Text(pDX, IDC_EDIT_COMPANY, m_sCompany);
	DDX_Text(pDX, IDC_EDIT_JOB_TITLE2, m_sJobTitle);
	DDX_Text(pDX, IDC_EDIT_URL, m_sUrl);
	DDX_Text(pDX, IDC_EDIT_TEL, m_sTel);
	DDX_Text(pDX, IDC_EDIT_FAX2, m_sFax);
	DDX_Text(pDX, IDC_EDIT_CONTACT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_CONTACT_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
	DDX_Control(pDX, IDC_EDIT_CONTACT_ACCOUNT, m_editContact);
	DDX_Control(pDX, IDC_EDIT_CONTACT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_CONTACT_PHONE, m_editPhone);
	DDX_Control(pDX, IDC_EDIT_COMPANY, m_editCompany);
	DDX_Control(pDX, IDC_EDIT_JOB_TITLE2, m_editJobTitle);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_EDIT_TEL, m_editTel);
	DDX_Control(pDX, IDC_EDIT_FAX2, m_editFax);
	DDX_Control(pDX, IDC_EDIT_CONTACT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_CONTACT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_BUTTON_REQUEST_ADD_CONTACT, m_btnRequestAddContact);
}


BEGIN_MESSAGE_MAP(CDlgContactInfo, CEbDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgContactInfo::OnBnClickedOk)
	//ON_CBN_SELCHANGE(IDC_COMBO_CONTACT, &CDlgContactInfo::OnCbnSelchangeComboContact)
	//ON_CBN_EDITCHANGE(IDC_COMBO_CONTACT, &CDlgContactInfo::OnCbnEditchangeComboContact)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgContactInfo::OnBnClickedButtonClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_REQUEST_ADD_CONTACT, &CDlgContactInfo::OnBnClickedButtonRequestAddContact)
END_MESSAGE_MAP()


// CDlgContactInfo message handlers

BOOL CDlgContactInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_STATIC);

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnOk.SetWindowText(_T("保存"));
	m_btnCancel.SetTextHotMove(false);
	m_btnCancel.SetWindowText(_T("取消"));
	m_btnOk.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(theApp.GetNormalColor()); 
	m_btnCancel.SetPreTextColor(theApp.GetNormalColor()); 

	m_editContact.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editName.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editPhone.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editCompany.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editJobTitle.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editUrl.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editTel.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editFax.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editEmail.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editAddress.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editDescription.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);

	m_btnRequestAddContact.SetTextHotMove(false);
	m_btnRequestAddContact.SetNorTextColor(RGB(0,128,255));
	m_btnRequestAddContact.SetHotTextColor(RGB(255,255,255));
	m_btnRequestAddContact.SetPreTextColor(RGB(255,255,255));
	m_btnRequestAddContact.SetWindowText(_T("加为好友"));
	if (theApp.GetAuthContact() && m_nContactUserId>0 && (m_nContactType&EB_CONTACT_TYPE_AUTH)==0)
	{
		m_btnRequestAddContact.ShowWindow(SW_SHOW);
		m_btnRequestAddContact.SetToolTipText(_T("点击申请添加好友"));
	}else
	{
		m_btnRequestAddContact.ShowWindow(SW_HIDE);
	}
	m_btnRequestAddContact.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());

	//m_comboContact.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	//m_comboGroup.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboGroup.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	//m_comboGroup.ModifyStyle(CBS_SORT,0);	// **不能排序，排序就会有问题；（这段代码不能修改）
	m_comboGroup.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboGroup.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	this->SetWindowText(_T("联系人资料"));
	//if (theApp.GetAuthContact() && !m_sContact.IsEmpty())
	if (theApp.GetAuthContact())// || !m_sContact.IsEmpty())
	{
		m_editContact.SetReadOnly(TRUE);
		m_editContact.EnableWindow(TRUE);
	}else
	{
		//m_editContact.CreateEx(WS_EX_CLIENTEDGE,"EDIT","",ES_AUTOHSCROLL|ES_LEFT|WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(0,0,1,1),this,IDC_EDIT_CONTACT_ACCOUNT);
		//m_editContact.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
		m_editContact.SetPromptText(_T("联系人帐号"));
		m_editContact.EnableWindow(TRUE);
	}

	//const bool bNewCoontact = m_sContact.IsEmpty();
	////m_editContact.EnableWindow(bNewCoontact?TRUE:FALSE);
	//m_editContact.EnableWindow(FALSE);
	//if (bNewCoontact)
	//{
	//	this->SetWindowText(_T("新建联系人"));
	//}else
	//{
	//	this->SetWindowText(_T("修改联系人"));
	//}
	SetTimer(TIMERID_LOAD_UGINFO_LIST,10,NULL);

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
	this->GetDlgItem(IDC_STATIC_CONTACT)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CONTACT_ACCOUNT)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	//this->GetDlgItem(IDC_COMBO_CONTACT)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_GROUP)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_COMBO_GROUP)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_CONTACT_NAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CONTACT_NAME)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_CONTACT_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CONTACT_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_COMPANY)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_COMPANY)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_TITLE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_JOB_TITLE2)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_URL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_URL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_TEL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_TEL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_EMAIL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CONTACT_EMAIL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_FAX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_FAX2)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CONTACT_ADDRESS)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_DESCRIPTION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(nX,nY,const_edit_width2,70);
	//m_editContact.Invalidate();
	//theApp.InvalidateParentRect(&m_editContact);

	const int POS_DLG_WIDTH = 612;
	const int POS_DLG_HEIGHT = 410;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgContactInfo::OnBnClickedOk()
{
	UpdateData();
	//if (this->m_sContact.IsEmpty())
	//{
	//	m_editContact.SetFocus();
	//	CDlgMessageBox::EbDoModal(this,_T("请输入帐号！"),_T(""),CDlgMessageBox::IMAGE_WARNING);
	//	return;
	//}
	if (m_sName.IsEmpty())
	{
		m_editName.SetFocus();
		CDlgMessageBox::EbDoModal(this,_T(""),_T("联系人姓名不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_WARNING,true,5);
		return;
	}
	const int nItem = m_comboGroup.GetCurSel();
	if (nItem>=0)
	{
		m_pUGInfoList.find(nItem,m_nUGId);
	}

	OnOK();
}

//#ifdef USES_EBCOM_TEST
//void CDlgContactInfo::Data2Ctrl(IEB_ContactInfo* pPopContactInfo)
//{
//	if (pPopContactInfo != NULL)
//	{
//		m_sContact = pPopContactInfo->Contact.GetBSTR();
//		m_sGroup = pPopContactInfo->Group.GetBSTR();
//		m_sName = pPopContactInfo->Name.GetBSTR();
//		m_sPhone = pPopContactInfo->Phone.GetBSTR();
//		m_sEmail = pPopContactInfo->Email.GetBSTR();
//		m_sAddress = pPopContactInfo->Address.GetBSTR();
//		m_sDescription = pPopContactInfo->Description.GetBSTR();
//		UpdateData(FALSE);
//	}
//}
//#else
//void CDlgContactInfo::Data2Ctrl(const EB_ContactInfo* pPopContactInfo)
//{
//	if (pPopContactInfo != NULL)
//	{
//		m_sContact = pPopContactInfo->m_sContact.c_str();
//		m_sGroup = pPopContactInfo->m_sGroupName.c_str();
//		m_sName = pPopContactInfo->m_sName.c_str();
//		m_sPhone = pPopContactInfo->m_sPhone.c_str();
//		m_sEmail = pPopContactInfo->m_sEmail.c_str();
//		m_sAddress = pPopContactInfo->m_sAddress.c_str();
//		m_sDescription = pPopContactInfo->m_sDescription.c_str();
//		UpdateData(FALSE);
//	}
//}
//#endif

//void CDlgContactInfo::OnCbnSelchangeComboContact()
//{
//	int nitem = m_comboContact.GetCurSel();
//	if (nitem >= 0)
//	{
//		const eb::bigint nContactId = m_pContactList[nitem];
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_ContactInfo> pEBContactInfo = theEBClientCore->EB_GetContactInfo1(nContactId);
//		Data2Ctrl(pEBContactInfo);
//#else
//		EB_ContactInfo pPopContactInfo;
//		if (theEBAppClient.EB_GetContactInfo1(nContactId, &pPopContactInfo))
//		{
//			Data2Ctrl(&pPopContactInfo);
//		}
//#endif
//	}
//}

//void CDlgContactInfo::OnCbnEditchangeComboContact()
//{
//	UpdateData();
//	if (m_nContactId>0)
//	{
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_ContactInfo> pEBContactInfo = theEBClientCore->EB_GetContactInfo1(m_nContactId);
//		Data2Ctrl(pEBContactInfo);
//#else
//		EB_ContactInfo pPopContactInfo;
//		if (theEBAppClient.EB_GetContactInfo1(m_nContactId,&pPopContactInfo))
//		{
//			Data2Ctrl(&pPopContactInfo);
//		}
//#endif
//	}
//}

void CDlgContactInfo::OnPaint()
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
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

		//if (theApp.GetColorGradient())
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		//else
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(315,theDefaultBgColorBgLight1,false));
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
		// 写标题
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(40,11);

		if (m_nContactUserId>0)
		{
			USES_CONVERSION;
			CString sText;
			sText.Format(_T("联系人资料 帐号ID:%lld"),m_nContactUserId);
			graphics.DrawString(A2W_ACP(sText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
		}else
		{
			graphics.DrawString(L"联系人资料",-1,&fontEbTitle,pointTitle,&brushEbTitle);
		}
		//const bool bNewCoontact = m_sContact.IsEmpty();
		//if (bNewCoontact)
		//	graphics.DrawString(L"联系人资料",-1,&fontEbTitle,pointTitle,&brushEbTitle);
		//else
		//{
		//	USES_CONVERSION;
		//	CString sText;
		//	sText.Format(_T("联系人资料-%s"),m_sContact);
		//	graphics.DrawString(A2W_ACP(sText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
		//}

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
		//CString sOutText = _T("通讯录资料");
		//TextOut(m_hdcMemory, 15, 15, sOutText, sOutText.GetLength());
		//SelectObject(m_hdcMemory, pOldFond);

	}
}

void CDlgContactInfo::OnSize(UINT nType, int cx, int cy)
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

	//const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 80;
	//const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 28;
	//const int const_btn_intever = 10;
	//int x = cx-const_btn_width-const_btn_intever;	// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+const_btn_intever);
	//m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	x = 20;
	m_btnRequestAddContact.MovePoint(x,const_btn_y,60,21);
}

void CDlgContactInfo::OnBnClickedButtonClose()
{
	this->OnCancel();
}

void CDlgContactInfo::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_LOAD_UGINFO_LIST==nIDEvent)
	{
		KillTimer(nIDEvent);
		if (!m_sContact.IsEmpty() && m_sName.IsEmpty())
		{
			m_editName.SetFocus();
		}
#ifdef USES_EBCOM_TEST
		//_variant_t pContactList = theEBClientCore->EB_GetContactList();
		//if (pContactList.vt!=VT_EMPTY && pContactList.parray != NULL)
		//{
		//	CComSafeArray<VARIANT> m_sa(pContactList.parray);
		//	for (ULONG i=0;i<m_sa.GetCount();i++)
		//	{
		//		CComVariant var = m_sa.GetAt(i);
		//		if (var.vt != VT_DISPATCH)
		//			continue;
		//		CComPtr<IEB_ContactInfo> pContactInfo;
		//		var.pdispVal->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pContactInfo);
		//		if (pContactInfo == NULL) continue;

		//		const CEBString sName(pContactInfo->Name.GetBSTR());
		//		const CEBString sContact(pContactInfo->Contact.GetBSTR());
		//		const CEBString sGroup(pContactInfo->Group.GetBSTR());
		//		CString sAddStringText;
		//		sAddStringText.Format(_T("%s(%s)"), sName.c_str(), sContact.c_str());
		//		int nitem = m_comboContact.AddString(sContact.c_str());
		//		m_pContactList[nitem] = sContact;
		//		if (sContact == (LPCTSTR)m_sContact)
		//		{
		//			m_comboContact.SetCurSel(nitem);
		//		}
		//		if (!sGroup.empty())
		//		{
		//			if (m_comboGroup.FindString(0, sGroup.c_str()) < 0)
		//			{
		//				m_comboGroup.AddString(sGroup.c_str());
		//			}
		//		}
		//	}
		//}
#else
		std::vector<EB_UGInfo> pUGInfoList;
		theEBAppClient.EB_GetUGInfoList(pUGInfoList);
		for (size_t i=0; i<pUGInfoList.size(); i++)
		{
			const EB_UGInfo& pUGInfo = pUGInfoList[i];
			const int nItem = m_comboGroup.AddString(pUGInfo.m_sGroupName.c_str());
			m_pUGInfoList.insert(nItem,pUGInfo.m_nUGId);
			if (m_nUGId>0 && m_nUGId==pUGInfo.m_nUGId)
			{
				m_comboGroup.SetCurSel(nItem);
			}
		}

		//std::vector<EB_ContactInfo> pContactList;
		//theEBAppClient.EB_GetContactList(pContactList);
		//for (size_t i=0; i<pContactList.size(); i++)
		//{
		//	const EB_ContactInfo& pPopContactInfo = pContactList[i];
		//	CString sAddStringText;
		//	sAddStringText.Format(_T("%s(%s)"), pPopContactInfo.m_sName.c_str(), pPopContactInfo.m_sContact.c_str());
		//	int nitem = m_comboContact.AddString(pPopContactInfo.m_sContact.c_str());
		//	m_pContactList[nitem] = pPopContactInfo.m_nContactId;
		//	if (pPopContactInfo.m_sContact == (LPCTSTR)m_sContact)
		//	{
		//		m_comboContact.SetCurSel(nitem);
		//	}
		//	if (!pPopContactInfo.m_sGroupName.empty())
		//	{
		//		if (m_comboGroup.FindString(0, pPopContactInfo.m_sGroupName.c_str()) < 0)
		//		{
		//			m_comboGroup.AddString(pPopContactInfo.m_sGroupName.c_str());
		//		}
		//	}
		//}
#endif
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgContactInfo::OnBnClickedButtonRequestAddContact()
{
	if (theApp.GetAuthContact() && m_nContactUserId>0 && (m_nContactType&EB_CONTACT_TYPE_AUTH)==0)
	{
		CDlgRequestAddContact pDlg;
		pDlg.m_sHeadFilePath = theApp.GetUserHeadFilePath(m_nContactUserId,(LPCTSTR)m_sContact);
		CString sText;
		sText.Format(_T("%s\n%lld\n%s"),m_sName,m_nContactUserId,m_sContact);
		pDlg.m_sHeadName = (LPCTSTR)sText;
		if (pDlg.DoModal()==IDOK)
		{
			EB_ContactInfo pContactInfo;
			pContactInfo.m_nContactUserId = m_nContactUserId;
			pContactInfo.m_sContact = m_sContact;
			pContactInfo.m_sDescription = (LPCTSTR)pDlg.m_sDescription;
			theEBAppClient.EB_EditContact(&pContactInfo);
		}
	}
}
