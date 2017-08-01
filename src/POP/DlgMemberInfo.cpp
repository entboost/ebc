// DlgMemberInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMemberInfo.h"

#define TIMERID_CHECK_RESOURCEFILE 100

// CDlgMemberInfo dialog

IMPLEMENT_DYNAMIC(CDlgMemberInfo, CEbDialogBase)

CDlgMemberInfo::CDlgMemberInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMemberInfo::IDD, pParent)
	, m_nGroupType(EB_GROUP_TYPE_DEPARTMENT), m_sGroupCode(0)
	, m_sMemberCode(0)
	, m_sMemberUserId(0)
	, m_sMemberAccount(_T(""))
	, m_sUserName(_T(""))
	, m_sJobTitle(_T(""))
	, m_nJobPosition(0)
	, m_sCellPhone(_T(""))
	, m_sWorkPhone(_T(""))
	, m_sEmail(_T(""))
	, m_sFax(_T(""))
	, m_sAddress(_T(""))
	, m_sDescription(_T(""))
	, m_bNewEemployee(false)
	, m_nGender(0)
	, m_nBirthday(0)
	, m_nDisplayIndex(0)
{
	m_imageHead = NULL;
	m_pDlgChangeHead = NULL;
	//m_nOldFileSize = 0;
	m_bSetTimer = false;
}

CDlgMemberInfo::~CDlgMemberInfo()
{
}

void CDlgMemberInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);

	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_sMemberAccount);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_sUserName);
	DDX_Text(pDX, IDC_EDIT_DEPNAME, m_sGroupName);
	DDX_Text(pDX, IDC_EDIT_JOB_TITLE, m_sJobTitle);
	DDX_Text(pDX, IDC_EDIT_JOB_POSITION, m_nJobPosition);
	DDX_Text(pDX, IDC_EDIT_CELL_PHONE, m_sCellPhone);
	DDX_Text(pDX, IDC_EDIT_WORK_PHONE, m_sWorkPhone);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_FAX, m_sFax);
	DDX_Text(pDX, IDC_EDIT_DISPLAY_INDEX2, m_nDisplayIndex);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
	DDX_Control(pDX, IDC_COMBO_GENDER, m_comboGender);
	DDX_CBIndex(pDX, IDC_COMBO_GENDER, m_nGender);
	DDX_Control(pDX, IDC_COMBO_BIR_YEAR, m_comboBirYear);
	//DDX_CBString(pDX, IDC_COMBO_BIR_YEAR, m_nBirYear);
	DDX_Control(pDX, IDC_COMBO_BIR_MONTH, m_comboBirMonth);
	//DDX_CBString(pDX, IDC_COMBO_BIR_MONTH, m_nBirMonth);
	DDX_Control(pDX, IDC_COMBO_BIR_DAY, m_comboBirDay);
	//DDX_CBString(pDX, IDC_COMBO_BIR_DAY, m_nBirDay);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_CHANGEHEAD, m_btnChangeHead);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT_EMP, m_btnDefaultEmp);

	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_editMemberAccount);
	DDX_Control(pDX, IDC_EDIT_DEPNAME, m_editGroupName);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
	DDX_Control(pDX, IDC_EDIT_JOB_TITLE, m_editJobTitle);
	DDX_Control(pDX, IDC_EDIT_JOB_POSITION, m_editJobPosition);
	DDX_Control(pDX, IDC_EDIT_CELL_PHONE, m_editCellPhone);
	DDX_Control(pDX, IDC_EDIT_WORK_PHONE, m_editWorkPhone);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_FAX, m_editFax);
	DDX_Control(pDX, IDC_EDIT_DISPLAY_INDEX2, m_editDisplayIndex);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgMemberInfo, CEbDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgMemberInfo::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMemberInfo::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_EMP, &CDlgMemberInfo::OnBnClickedButtonDefaultEmp)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CHANGEHEAD, &CDlgMemberInfo::OnBnClickedButtonChangehead)
	ON_MESSAGE(EB_COMMAND_CHANGE_HEAD, OnMsgChangeHead)
	
END_MESSAGE_MAP()


// CDlgMemberInfo message handlers

void CDlgMemberInfo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_sMemberAccount.IsEmpty()) {
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("登录帐号不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}
	//else if (m_bNewEemployee && theApp.GetAccountPrefix().IsEmpty()) {
	//	const tstring sAccount(m_sMemberAccount);
	//	//eb::bigint nPhone = 0;
	//	if (sAccount.find("@") != std::string::npos || !libEbc::IsFullNumber(sAccount.c_str(),sAccount.size()))
	//	{
	//		// xx@xx邮箱格式
	//	}else if (sAccount.size()==11 && sAccount.substr(0,1)=="1")
	//	{
	//		//nPhone = eb_atoi64(sAccount.c_str());
	//	}else
	//	{
	//		//// 错误格式；
	//		//this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
	//		//CDlgMessageBox::EbMessageBox(this,"","帐号格式错误：\r\n请重新输入正确邮箱地址或手机号码！",CDlgMessageBox::IMAGE_WARNING,5);
	//		//return;
	//	}
	//}
	if (m_sUserName.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("姓名不能为空：\r\n请输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return;
	}
	if (m_bNewEemployee)
	{
#ifdef USES_EBCOM_TEST
		if (theEBClientCore->EB_IsExistMemberByAccount(m_sGroupCode, (LPCTSTR)m_sMemberAccount))
#else
		if (theEBAppClient.EB_IsExistMemberByAccount(m_sGroupCode, m_sMemberAccount))
#endif
		{
			this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
			CString sText;
			sText.Format(_T("%s\r\n帐号已经存在，请重新输入！"), m_sMemberAccount);
			CDlgMessageBox::EbMessageBox(this,_T(""),sText,CDlgMessageBox::IMAGE_ERROR,5);
			return;
		}
	}
	// 计算生日整数
	CString stext;
	m_comboBirYear.GetWindowText(stext);
	int nBirYear = atoi(stext);
	m_comboBirMonth.GetWindowText(stext);
	int nBirMonth = atoi(stext);
	m_comboBirDay.GetWindowText(stext);
	int nBirDay = atoi(stext);
	m_nBirthday = nBirYear*10000+nBirMonth*100+nBirDay;

	OnOK();
}

BOOL CDlgMemberInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	SetTransparentType(CEbDialogBase::TT_STATIC);

	//CRect rect;
	//m_editMemberAccount.GetRect(&rect);
	//rect.top += 4;
	//m_editMemberAccount.SetRectNP(&rect);
	m_editMemberAccount.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	//this->GetDlgItem(IDC_EDIT_ACCOUNT)->ModifyStyleEx(0,WS_EX_CLIENTEDGE);

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	//m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetWindowText(_T("保存"));
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

	m_editMemberAccount.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editGroupName.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editUserName.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editJobTitle.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editJobPosition.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editCellPhone.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editWorkPhone.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editEmail.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editFax.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editDisplayIndex.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editAddress.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editDescription.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);

	m_btnChangeHead.SetTextHotMove(false);
	m_btnChangeHead.SetToolTipText(_T("更换显示头像"));
	m_btnChangeHead.SetNorTextColor(RGB(0,128,255));
	m_btnChangeHead.SetHotTextColor(RGB(255,255,255));
	m_btnChangeHead.SetPreTextColor(RGB(255,255,255));
	m_btnChangeHead.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnDefaultEmp.SetToolTipText(_T("默认名片用于聊天窗口显示电子名片"));
	m_btnDefaultEmp.SetTextHotMove(false);
	m_btnDefaultEmp.SetNorTextColor(RGB(0,128,255));
	m_btnDefaultEmp.SetHotTextColor(RGB(255,255,255));
	m_btnDefaultEmp.SetPreTextColor(RGB(255,255,255));
	m_btnDefaultEmp.SetWindowText(_T("设为默认名片"));
	m_btnDefaultEmp.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	if (m_sMemberCode>0)
	{
#ifdef USES_EBCOM_TEST
		const eb::bigint sDefaultMemberCode = theEBClientCore->EB_MyDefaultMemberCode;
#else
		eb::bigint sDefaultMemberCode;
		theEBAppClient.EB_GetMyDefaultMemberCode(sDefaultMemberCode);
#endif
		if (m_sMemberCode==sDefaultMemberCode)
		{
			m_btnDefaultEmp.SetWindowText(_T("当前默认名片"));
			//m_btnDefaultEmp.SetWindowText(_T("取消默认名片"));
		}
	}

	m_comboGender.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboGender.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboGender.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	m_comboBirYear.SetDoubleDrawHover(true);
	m_comboBirYear.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirYear.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirYear.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	m_comboBirMonth.SetDoubleDrawHover(true);
	m_comboBirMonth.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirMonth.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirMonth.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	m_comboBirDay.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboBirDay.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirDay.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	m_comboBirDay.SetDoubleDrawHover(true);

	m_editMemberAccount.SetPromptText(_T("**登录帐号**"));
	m_editUserName.SetPromptText(_T("**用户名称**"));
	this->GetDlgItem(IDC_EDIT_DEPNAME)->EnableWindow(FALSE);
	//m_editGroupName.SetReadOnly(TRUE);
	if (!m_sMemberAccount.IsEmpty())
	{
		m_bNewEemployee = false;
		//m_editMemberAccount.SetReadOnly(TRUE);
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(FALSE);

#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetGroupType(m_sGroupCode,m_nGroupType);
		if (!theEBClientCore->GetEB_CanEditMyBaseMemberInfo(m_sGroupCode))
#else
		theEBAppClient.EB_GetGroupType(m_sGroupCode,m_nGroupType);
		if (!theEBAppClient.EB_CanEditMyBaseMemberInfo(m_sGroupCode))
#endif
		{
			//m_editUserName.SetReadOnly(TRUE);
			//m_editCellPhone.SetReadOnly(TRUE);
			//m_editEmail.SetReadOnly(TRUE);
			//m_editJobTitle.SetReadOnly(TRUE);
			//m_editJobPosition.SetReadOnly(TRUE);
			//m_editWorkPhone.SetReadOnly(TRUE);
			//m_editFax.SetReadOnly(TRUE);
			//m_editDisplayIndex.SetReadOnly(TRUE);
			//m_editAddress.SetReadOnly(TRUE);

			this->GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_GENDER)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_YEAR)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_MONTH)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_DAY)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_CELL_PHONE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_EMAIL)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_JOB_TITLE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_JOB_POSITION)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_WORK_PHONE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_FAX)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_DISPLAY_INDEX2)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_ADDRESS)->EnableWindow(FALSE);
		}else if (!theEBAppClient.EB_IsGroupAdminLevel(m_sGroupCode))
		{
			// 非管理员权限，不能修改显示排序
			this->GetDlgItem(IDC_EDIT_DISPLAY_INDEX2)->EnableWindow(FALSE);
		}
		if (m_sMemberUserId==theApp.GetLogonUserId())	// 自己才能修改
		{
			CString sText;
			sText.Format(_T("更换当前%s[%s]电子名片显示头像"),GetGroupTypeText(m_nGroupType,false),m_sGroupName);
			m_btnChangeHead.SetToolTipText(sText);
		}else
		{
			this->GetDlgItem(IDC_BUTTON_CHANGEHEAD)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_DEFAULT_EMP)->ShowWindow(SW_HIDE);
		}
	}else
	{
		m_bNewEemployee = true;
		this->GetDlgItem(IDC_BUTTON_CHANGEHEAD)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON_DEFAULT_EMP)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(TRUE);
		m_editMemberAccount.SetReadOnly(FALSE);
		this->EnableToolTips();
		this->SetToolTipText(IDC_EDIT_ACCOUNT,_T("请输入用户登录帐号！"));
		//this->SetToolTipText(IDC_EDIT_ACCOUNT,_T("请输入邮箱地址（或手机号码），用于恩布登录帐号！"));
	}
	// 性别
	m_comboGender.AddString(_T(""));
	m_comboGender.AddString(_T("男性"));
	m_comboGender.AddString(_T("女性"));
	CString stext;
	int nBirYear = m_nBirthday/10000;
	const int nBirMonth = (m_nBirthday%10000)/100;
	const int nBirDay = m_nBirthday%100;
	// 年
	for (int year=1960; year<=2012; year++)
	{
		stext.Format(_T("%04d年"), year);
		const int nIndex = m_comboBirYear.AddString(stext);
		if (nBirYear == year)
		{
			m_comboBirYear.SetCurSel(nIndex);
			nBirYear = 0;
		}
	}
	if (nBirYear > 0)
	{
		stext.Format(_T("%04d年"), nBirYear);
		//m_comboBirYear.SetWindowText(stext);
		const int nIndex = m_comboBirYear.AddString(stext);
		m_comboBirYear.SetCurSel(nIndex);
	}
	// 月
	for (int month=1; month<=12; month++)
	{
		stext.Format(_T("%02d月"), month);
		const int nIndex = m_comboBirMonth.AddString(stext);
		if (nBirMonth == month)
			m_comboBirMonth.SetCurSel(nIndex);
	}
	//if (nBirMonth > 0)
	//{
	//	stext.Format(_T("%02d月"), nBirMonth);
	//	m_comboBirMonth.SetWindowText(stext);
	//}
	// 日
	for (int day=1; day<=31; day++)
	{
		stext.Format(_T("%02d日"), day);
		const int nIndex = m_comboBirDay.AddString(stext);
		if (nBirDay == day)
			m_comboBirDay.SetCurSel(nIndex);
	}
	//if (nBirDay > 0)
	//{
	//	stext.Format(_T("%02d日"), nBirDay);
	//	m_comboBirDay.SetWindowText(stext);
	//}

	const int const_y1	= 60;
	const int const_y2	= 148;
	const int const_x1	= 34;
	const int const_x2	= 314;
	const int const_x_interval	= 70;
	const int const_static_width	= 70;
	const int const_edit_width1		= 175;
	const int const_edit_width2		= 455;
	const int const_edit_height		= 21;
	const int const_y_interval	= 35;
	this->GetDlgItem(IDC_EDIT_ACCOUNT)->MoveWindow(123,const_y1,const_edit_width1,const_edit_height);
	this->GetDlgItem(IDC_EDIT_DEPNAME)->MoveWindow(123,const_y1+const_edit_height+2,const_edit_width1,const_edit_height);
	m_btnChangeHead.MoveWindow(123,const_y1+(const_edit_height+2)*2+5,60,22);

	int nX = const_x1;
	int nY = const_y2;
	this->GetDlgItem(IDC_STATIC_USERNAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_USERNAME)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_JOB_TITLE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_JOB_TITLE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_GENDER)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_COMBO_GENDER)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_JOB_POSITION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_JOB_POSITION)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_BIRTHDAY)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_COMBO_BIR_YEAR)->MoveWindow(nX,nY,71,const_edit_height+1);
	nX += 70;
	this->GetDlgItem(IDC_COMBO_BIR_MONTH)->MoveWindow(nX,nY,54,const_edit_height+1);
	nX += 53;
	this->GetDlgItem(IDC_COMBO_BIR_DAY)->MoveWindow(nX,nY,53,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_WORK_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_WORK_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_CELL_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CELL_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_FAX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_FAX)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_EMAIL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_EMAIL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_DISPLAY_INDEX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DISPLAY_INDEX2)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_edit_width2,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_DESCRIPTION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(nX,nY,const_edit_width2,43);	// 53


	const int POS_DLG_WIDTH = 612;
	const int POS_DLG_HEIGHT = 455;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	m_rectHead.left = 25;
	m_rectHead.right = m_rectHead.left + 80;
	m_rectHead.top = 55;
	m_rectHead.bottom = m_rectHead.top + 80;
	//m_sHeadResourceFile = theApp.GetAppPath()+_T("\\img\\myimg.png");
	//USES_CONVERSION;
	//if (!m_sHeadResourceFile.IsEmpty() && PathFileExists(m_sHeadResourceFile))
	//{
	//	m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sHeadResourceFile));   
	//}

	//m_editMemberAccount.SetFocus();
	if (!m_bNewEemployee)
	{
		UpdateData(FALSE);
	}
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMemberInfo::OnPaint()
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

void CDlgMemberInfo::DrawInfo(void)
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
	if (m_nGroupType==EB_GROUP_TYPE_DEPARTMENT)
		graphics.DrawString(L"员工资料",-1,&fontEbTitle,pointTitle,&brushEbTitle);
	else
		graphics.DrawString(L"成员资料",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	const tstring sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
	if (!sHeadResourceFile.empty() && PathFileExists(sHeadResourceFile.c_str()))
	{
		if (m_sOldFileMd5.empty())
		//if (m_nOldFileSize==0)
		{
			mycp::bigint m_nOldFileSize = 0;
			libEbc::GetFileMd5(sHeadResourceFile.c_str(),m_nOldFileSize,m_sOldFileMd5);
			//m_nOldFileSize = libEbc::GetFileSize(sHeadResourceFile.c_str());
		}
		Gdiplus::Image * pImage = libEbc::LoadImageFromFile(sHeadResourceFile.c_str());
		if (pImage==NULL)
			pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sHeadResourceFile.c_str()));
		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		delete pImage;
		//Gdiplus::Image imageHead((const WCHAR*)A2W_ACP(sHeadResourceFile.c_str()));   
		//graphics.DrawImage(&imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	}else
	{
		Image * pImage = theApp.m_imageDefaultMember->Clone();;
		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		delete pImage;
	}
	if (theApp.GetSendRegMail() && m_bNewEemployee)
	{
		Gdiplus::SolidBrush brushTip(Gdiplus::Color(255,0,128));
		graphics.DrawString(L"添加成功，请接收验证邮件，验证恩布帐号！",-1,&fontEbTitle,PointF(310,63),&brushTip);
	}
	//if (m_imageHead!=NULL)
	//{
	//	graphics.DrawImage(m_imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	//}

	//CPaintDC dc(this); // 用于绘制的设备上下文
	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//CRect rectClient;
	//this->GetClientRect(&rectClient);
	//HDC m_hdcMemory = dc.m_hDC;

	//// 写标题
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//CString sOutText = _T("群名片");
	//TextOut(m_hdcMemory, 6, 5, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

}

void CDlgMemberInfo::OnSize(UINT nType, int cx, int cy)
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
	//const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 60;
	//const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 24;
	//int x = cx-const_btn_width-10;					// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+10);						// 按钮间隔
	//m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	x = 20;
	m_btnDefaultEmp.MovePoint(x,const_btn_y,80,22);
}

void CDlgMemberInfo::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgMemberInfo::OnBnClickedButtonDefaultEmp()
{
	bool bAlreadyMyDefaultEmp = false;
	if (m_sMemberCode>0)
	{
#ifdef USES_EBCOM_TEST
		const eb::bigint sDefaultMemberCode = theEBClientCore->EB_MyDefaultMemberCode;
#else
		eb::bigint sDefaultMemberCode;
		theEBAppClient.EB_GetMyDefaultMemberCode(sDefaultMemberCode);
#endif
		if (m_sMemberCode==sDefaultMemberCode)
		{
			bAlreadyMyDefaultEmp = true;
		}
	}

	if (bAlreadyMyDefaultEmp)
	{
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("当前是默认名片！"),CDlgMessageBox::IMAGE_INFORMATION,5);
//		// 取消默认名片
//#ifdef USES_EBCOM_TEST
//		theEBClientCore->EB_MyDefaultMemberCode = 0;
//#else
//		theEBAppClient.EB_SetMyDefaultMemberCode(0);
//#endif
//		m_btnDefaultEmp.SetWindowText(_T("设为默认名片"));
	}else
	{

#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_MyDefaultMemberCode = m_sMemberCode;
#else
		theEBAppClient.EB_SetMyDefaultMemberCode(m_sMemberCode);
#endif
		//m_btnDefaultEmp.SetWindowText(_T("取消默认名片"));
	}
}

void CDlgMemberInfo::OnDestroy()
{
	if (m_pDlgChangeHead)
	{
		m_pDlgChangeHead->DestroyWindow();
		delete m_pDlgChangeHead;
		m_pDlgChangeHead = NULL;
	}
	CEbDialogBase::OnDestroy();
	if (m_imageHead!=NULL)
	{
		delete m_imageHead;
		m_imageHead = NULL;
	}
}

void CDlgMemberInfo::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//if (!m_pMemberInfo.m_sHeadResourceFile.empty())
	if (!m_bNewEemployee && m_sMemberUserId==theApp.GetLogonUserId())	// 自己才能修改
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			return;
		}
	}
	CEbDialogBase::OnMouseMove(nFlags, point);
}

void CDlgMemberInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bNewEemployee && m_sMemberUserId==theApp.GetLogonUserId())	// 自己才能修改
	{
		ReleaseCapture();
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			OnBnClickedButtonChangehead();
		}
	}
	CEbDialogBase::OnLButtonUp(nFlags, point);
}

void CDlgMemberInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bNewEemployee && m_sMemberUserId==theApp.GetLogonUserId())	// 自己才能修改
	//if (!m_bNewEemployee)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			this->SetCapture();
		}
	}
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgMemberInfo::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_CHECK_RESOURCEFILE == nIDEvent)
	{
		mycp::bigint nHeadResourceId = 0;
		tstring sHeadResourceFilePath;
		tstring sHeadResourceFileMd5;
		//int nFileSize = 0;
		theEBAppClient.EB_GetMemberHeadFile(m_pMemberInfo.m_sMemberCode,nHeadResourceId,sHeadResourceFilePath,sHeadResourceFileMd5);
		// ** resourceid会先更新，不能判断resourceid
		if (!sHeadResourceFilePath.empty() && PathFileExists(sHeadResourceFilePath.c_str()) && !sHeadResourceFileMd5.empty() && (sHeadResourceFilePath!=m_pMemberInfo.m_sHeadResourceFile || sHeadResourceFileMd5!=m_sOldFileMd5))
		{
			m_pMemberInfo.m_sHeadResourceId = nHeadResourceId;
			m_pMemberInfo.m_sHeadResourceFile = sHeadResourceFilePath;
			m_sOldFileMd5 = sHeadResourceFileMd5;
			//m_nOldFileSize = nFileSize;
			KillTimer(TIMERID_CHECK_RESOURCEFILE);
			this->Invalidate();
		}

		////if (m_pMemberInfo.get()!=NULL)
		//{
		//	const tstring sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
		//	if (!sHeadResourceFile.empty() && PathFileExists(sHeadResourceFile.c_str()))
		//	{
		//		unsigned int nFileSize = libEbc::GetFileSize(sHeadResourceFile.c_str());
		//		if (nFileSize>0 && nFileSize!=m_nOldFileSize)
		//		{
		//			m_nOldFileSize = nFileSize;
		//			this->Invalidate();
		//		}
		//	}
		//}
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMemberInfo::OnBnClickedButtonChangehead()
{
	//if (m_pMemberInfo.m_sHeadResourceFile..get()!=NULL)
	if (!m_bNewEemployee)
	{
		//if (!m_bSetTimer)
		//{
		//	m_bSetTimer = true;
		//	SetTimer(TIMERID_CHECK_RESOURCEFILE,6*1000,NULL);
		//}
		if (m_pDlgChangeHead==NULL)
		{
			m_pDlgChangeHead = new CDlgChangeHead(this);
			m_pDlgChangeHead->m_sGroupCode = this->m_sGroupCode;
			m_pDlgChangeHead->SetHeadResorceFile(m_pMemberInfo.m_sHeadResourceFile);
			m_pDlgChangeHead->Create(CDlgChangeHead::IDD,this);
			m_pDlgChangeHead->SetCallback(this);	// **必须放在这后面
		}
		m_pDlgChangeHead->ShowWindow(SW_SHOW);
	}
}

void CDlgMemberInfo::OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo)
{
#ifdef USES_EBCOM_TEST
	const int ret = theEBClientCore->EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
#else
	const int ret = theEBAppClient.EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
#endif
	if (ret == 0)
	{
		m_pDlgChangeHead->SetHeadResorceFile(pSelectedImageInfo.m_sResFile);
		m_pDlgChangeHead->ShowWindow(SW_HIDE);
		//m_pMemberInfo.m_sHeadResourceId = pSelectedImageInfo.m_sResId;
		m_pMemberInfo.m_sHeadResourceFile = pSelectedImageInfo.m_sResFile;
		mycp::bigint m_nOldFileSize = 0;
		m_sOldFileMd5.clear();
		libEbc::GetFileMd5(m_pMemberInfo.m_sHeadResourceFile.c_str(),m_nOldFileSize,m_sOldFileMd5);
		//m_nOldFileSize = libEbc::GetFileSize(m_pMemberInfo.m_sHeadResourceFile.c_str());
		this->Invalidate();
	}
}

BOOL CDlgMemberInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==' ')
	{
		// 控制不能输入空格
		if (pMsg->hwnd == m_editMemberAccount.GetSafeHwnd())
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

LRESULT CDlgMemberInfo::OnMsgChangeHead(WPARAM wp, LPARAM lp)
{
	KillTimer(TIMERID_CHECK_RESOURCEFILE);
	SetTimer(TIMERID_CHECK_RESOURCEFILE,1000,NULL);
	return 0;
}
