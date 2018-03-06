// DlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgLogin.h"
//#include "DlgSetting.h"
//#include "DlgRegister.h"
#include "DlgMessageBox.h"
#include <afxinet.h>
#include "tlhelp32.h"
#include "Psapi.h"

#pragma comment(lib, "Psapi.lib")


#define POS_STA_LEFT 33
#define POS_ADIMG_LEFT 15
#define POS_ADIMG_TOP 50
#define POS_ADIMG_SIGE 120
#define TIMERID_REFRESH_WIN 1100

// CDlgLogin dialog

IMPLEMENT_DYNAMIC(CDlgLogin, CEbDialogBase)

CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgLogin::IDD, pParent)
	//, m_sUserAccount(_T("aa@192.168.0.208"))
	, m_bAuthLogLogonMode(false)
	, m_sUserAccount(_T(""))
	, m_sUserPassword(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nOutLineState = EB_LINE_STATE_ONLINE_NEW;
	m_bAutoLogSuccess = false;
	m_bReload = false;

	//m_nDeployId = 0;
	m_bCanSearch = true;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SKIN, m_btnSkin);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_LINESTATE, m_btnLineState);
	DDX_Control(pDX, IDC_BUTTON_FORGET_PWD, m_btnForgetPwd);
#ifdef USES_NEW_USER_CTRL
	DDX_Text(pDX, IDC_EDIT_USERS, m_sUserAccount);
#else
	DDX_CBString(pDX, IDC_COMBO_USERS, m_sUserAccount);
#endif
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sUserPassword);
	//DDX_Control(pDX, IDC_BUTTON_SETTING, m_btnSetting);
	DDX_Control(pDX, IDC_BUTTON_REG, m_btnReg);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_btnLogin);
	DDX_Control(pDX, IDC_BUTTON_VISITOR, m_btnVisitor);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_STATIC_SAVEPWD, m_labelSavePwd);
	DDX_Control(pDX, IDC_STATIC_AUTOLOGIN, m_labelAutoLogin);
	//DDX_Check(pDX, IDC_CHECK_SAVEPWD, m_bSavePwd);
	//DDX_Control(pDX, IDC_CHECK_SAVEPWD, m_staSavePwd);
#ifdef USES_NEW_USER_CTRL
	DDX_Control(pDX, IDC_EDIT_USERS, m_editUser);
	DDX_Control(pDX, IDC_TREE_USERS, m_treeUsers);
#endif
	//DDX_Control(pDX, IDC_COMBO_USERS, m_comboUsers);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_labelError);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CEbDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDlgLogin::OnBnClickedButtonLogin)

	ON_MESSAGE(EB_WM_OAUTH_FORWARD, OnMessageOAuthForward)
	ON_MESSAGE(EB_WM_LOGON_SUCCESS, OnMessageLogonSuccess)
	ON_MESSAGE(EB_WM_LOGON_TIMEOUT, OnMessageLogonTimeout)
	ON_MESSAGE(EB_WM_LOGON_ERROR, OnMessageLogonError)
	ON_MESSAGE(EB_WM_APPID_ERROR, OnMessageAppIdError)
	ON_MESSAGE(EB_WM_APPID_SUCCESS, OnMessageAppIdSuccess)

	ON_MESSAGE(EB_WM_SERVER_CHENGE, OnMessageServerChange)
	ON_BN_CLICKED(IDC_BUTTON_VISITOR, &CDlgLogin::OnBnClickedButtonVisitor)
	//ON_BN_CLICKED(IDC_BUTTON_SETTING, &CDlgLogin::OnBnClickedButtonSetting)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//ON_CBN_SELCHANGE(IDC_COMBO_USERS, &CDlgLogin::OnCbnSelchangeComboUsers)
	//ON_CBN_EDITCHANGE(IDC_COMBO_USERS, &CDlgLogin::OnCbnEditchangeComboUsers)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgLogin::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgLogin::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_REG, &CDlgLogin::OnBnClickedButtonReg)
	ON_BN_CLICKED(IDC_BUTTON_LINESTATE, &CDlgLogin::OnBnClickedButtonLinestate)
	ON_COMMAND(EB_COMMAND_STATE_ONLINE, &CDlgLogin::OnStateOnline)
	ON_COMMAND(EB_COMMAND_STATE_AWAY, &CDlgLogin::OnStateAway)
	ON_COMMAND(EB_COMMAND_STATE_BUSY, &CDlgLogin::OnStateBusy)
	//ON_COMMAND(EB_COMMAND_DELETE_ACCOUNT, &CDlgLogin::OnDeleteAccount)
	ON_COMMAND_RANGE(EB_COMMAND_SKIN_SETTING,EB_COMMAND_SKIN_2,&CDlgLogin::OnSkinSelect)
	ON_COMMAND(EB_COMMAND_SKIN_GRADIENT, &CDlgLogin::OnSkinGradient)
	ON_COMMAND(EB_COMMAND_SERVER_SETTING, &CDlgLogin::OnServerSetting)
	ON_BN_CLICKED(IDC_BUTTON_SKIN, &CDlgLogin::OnBnClickedButtonSkin)
	ON_STN_CLICKED(IDC_STATIC_AUTOLOGIN, &CDlgLogin::OnStnClickedStaticAutologin)
	ON_BN_CLICKED(IDC_BUTTON_FORGET_PWD, &CDlgLogin::OnBnClickedButtonForgetPwd)
	//ON_NOTIFY(NM_KILLFOCUS, IDC_TREE_USERS, &CDlgLogin::OnNMKillfocusTreeUsers)
	ON_NOTIFY(NM_CLICK, IDC_TREE_USERS, &CDlgLogin::OnNMClickTreeUsers)
	ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChange)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)

	ON_EN_CHANGE(IDC_EDIT_USERS, &CDlgLogin::OnEnChangeEditUsers)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CDlgLogin message handlers
bool CDlgLogin::StartClient(void)
{
#ifdef USES_EBCOM_TEST
	const CEBString sAddress = theEBSetting->ServerAddress.GetBSTR();
	//const CEBString sAddress = theEBClientCore->GetAccountAddress(lpszAccount).GetBSTR();
	theEBClientCore->EB_Init(sAddress.c_str(),"");
	return theEBClientCore->EB_IsInited==VARIANT_TRUE?true:false;
#else
	const CEBString sAddress = theSetting.GetServerAddress();
	//entboost::GetAccountAddress(lpszAccount, sAddress);
	//theEBAppClient.Stop();
	theEBAppClient.EB_Init(sAddress.c_str());
	//theEBAppClient.EB_SetMsgHwnd(this->GetSafeHwnd());
	return theEBAppClient.EB_IsInited();
#endif
}

void CDlgLogin::OnBnClickedButtonLogin()
{
	// TODO: Add your control notification handler code here
	if (!UpdateData())
		return;
	//if (m_sUserAccount.IsEmpty())
	//{
	//	this->GetDlgItem(IDC_COMBO_USERS)->SetFocus();
	//	CDlgMessageBox::EbDoModal(this,"请输入用户帐号！","",CDlgMessageBox::IMAGE_WARNING);
	//	return;
	//}
	if (m_sUserPassword.IsEmpty())
	{
		m_editPassword.SetFocus();
		//CDlgMessageBox::EbDoModal(this,"请输入帐号密码！","",CDlgMessageBox::IMAGE_WARNING);
		return;
	}
	if (!StartClient())
	{
		CDlgMessageBox::EbDoModal(this,"本地服务启动失败！","",CDlgMessageBox::IMAGE_WARNING);
		return;
	}
	//theEBAppClient.EB_LogonOAuth();
	//return;

	m_labelError.ShowWindow(SW_HIDE);
	m_bAutoLogSuccess = false;
	if (!m_bAuthLogLogonMode && theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("delete from user_login_record_t where account='%s'"), m_sUserAccount);
		int ret = theApp.m_pBoEB->execsql(sSql);
		if (m_labelSavePwd.GetCheck())
			sSql.Format(_T("insert into user_login_record_t(account,password,safepwd,linestate) VALUES('%s','%s',true,%d)"),
			m_sUserAccount,m_sUserPassword,m_nOutLineState);
		else
			sSql.Format(_T("insert into user_login_record_t(account,password,linestate) VALUES('%s','',%d)"),m_sUserAccount,m_nOutLineState);
		ret = theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LogonByAccount((LPCTSTR)m_sUserAccount,(LPCTSTR)m_sUserPassword,m_sReqCode.c_str(),m_nOutLineState);
#else
	theEBAppClient.EB_LogonByAccount((LPCTSTR)m_sUserAccount,m_sUserPassword,m_sReqCode.c_str(),m_nOutLineState);
#endif
}
BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
{
	const wchar_t * lpszFine = (const wchar_t*)lparam;
	const std::wstring tempFontName(lplf->lfFaceName);
	if(tempFontName.find(lpszFine)!=std::wstring::npos)
	{
		theFontFamily = lpszFine;
		return false;
	}
	return true;
}

void CDlgLogin::EbSetWindowText(void)
{
	CString stext;
	if (!m_sProductName.IsEmpty())
		stext.Format(_T("%s-登录窗口"),m_sProductName);
	else
	{
#ifdef USES_EBCOM_TEST
		const CEBString sEnterprise = theEBSetting->Enterprise.GetBSTR();
		stext.Format(_T("%s-登录窗口"),sEnterprise.c_str());
#else
		stext.Format(_T("%s-登录窗口"),theSetting.GetEnterprise().c_str());
#endif
	}
	this->SetWindowText(stext);
}

void CDlgLogin::SetCtrlColor(void)
{
	float h,s,l;
	::RGBtoHSL(GetRValue(theApp.GetMainColor()), GetGValue(theApp.GetMainColor()), GetBValue(theApp.GetMainColor()),&h,&s,&l);
	const COLORREF crHot1 = HSLtoRGB(h, s, theDefaultBtnHotColorLight1);
	const COLORREF crHot2 = HSLtoRGB(h, s, theDefaultBtnHotColorLight2);
	const COLORREF crHot = theApp.GetColorGradient()?crHot1:crHot2;
	const COLORREF crPre = ::HSLtoRGB(h, s, theDefaultColorNormalLight);
	m_btnReg.SetNorTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	m_btnReg.SetHotTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	m_btnForgetPwd.SetNorTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	m_btnForgetPwd.SetHotTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	//m_btnSetting.SetNorTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	//m_btnSetting.SetHotTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	//m_btnVisitor.SetNorTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);
	//m_btnVisitor.SetHotTextColor(theApp.GetColorGradient()?crHot2:theDefaultTextWhiteColor);

	//m_btnReg.SetDrawPanel(true,theDefaultBtnWhiteColor,crHot,crPre);
	m_btnLogin.SetDrawPanel(true,theDefaultBtnWhiteColor,crHot,crPre);
	m_btnVisitor.SetDrawPanel(true,crPre,crHot,theDefaultBtnWhiteColor);

#ifdef USES_NEW_USER_CTRL
	m_editUser.SetRectangleColor(crHot2,crHot1);
#endif
	m_editPassword.SetRectangleColor(crHot2,crHot1);
	m_labelSavePwd.SetCheckBoxHoverBorder(TRUE,crHot2);
	m_labelAutoLogin.SetCheckBoxHoverBorder(TRUE,crHot2);
}

void CDlgLogin::SetAutoLogon(const CString& sAccount, const CString& sPasswd)
{
	m_bAuthLogLogonMode = true;
	m_sUserAccount = sAccount;
	m_sUserPassword = sPasswd;
}

//CImageList theImageList;
BOOL CDlgLogin::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_SYSMENU);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
    CMenuXP::SetXPLookNFeel (this,true);
    CMenuXP::UpdateMenuBar (this);
	//theImageList.Create(IDB_COMBO_RIGHT,16,1,ILC_COLOR32);
	//CMenuXP::SetMenuItemImage(EB_COMMAND_STATE_ONLINE,theImageList.m_hImageList,0);

	CPaintDC dc((CWnd*)this);
	::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
	::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());

	//const CEBString sEnterprise = theSetting.GetEnterprise();
	//USES_CONVERSION;
	//std::wstring sText = A2W_ACP(sEnterprise.c_str());
	//MessageBoxW(this->GetSafeHwnd(),sText.c_str(),L"222",MB_OK);

	const CString sEBCSetting = theApp.GetAppDataPath()+_T("\\ebc.ini");
	if (::PathFileExists(sEBCSetting))
	{
		TCHAR lpszBuffer[1024];
		memset(lpszBuffer,0,sizeof(lpszBuffer));
		::GetPrivateProfileString("system","server","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting);
		m_sOrgServer = lpszBuffer;
	}

	if (!StartClient())
	{
		CDlgMessageBox::EbDoModal(this,"本地服务启动失败！","",CDlgMessageBox::IMAGE_WARNING);
		return FALSE;
	}
#ifdef USES_EBCOM_TEST
	(((CEBDispatch*)(CEBCoreEventsSink*)this))->AddRef();	// 增加计数，避免删除自己
	((CEBCoreEventsSink*)this)->m_hFireWnd = this->GetSafeHwnd();
	this->DispEventAdvise(theEBClientCore);
	theEBClientCore->EB_SetDevAppId(278573612908LL,"ec1b9c69094db40d9ada80d657e08cc6",VARIANT_TRUE);
#else
	theEBAppClient.EB_SetMsgHwnd(this->GetSafeHwnd());
	theEBAppClient.EB_SetDevAppId(278573612908LL,"ec1b9c69094db40d9ada80d657e08cc6",true);
#endif

	//this->SetTransparentType(CEbDialogBase::TT_STATIC);
	this->EnableToolTips();
	this->SetToolTipText(IDC_BUTTON_SKIN,_T("设置"));
	this->SetToolTipText(IDC_BUTTON_MIN,_T("最小化"));
	this->SetToolTipText(IDC_BUTTON_CLOSE,_T("关闭"));
	this->SetToolTipText(IDC_BUTTON_LINESTATE,_T("在线"));
#ifdef USES_NEW_USER_CTRL
	this->SetToolTipText(IDC_EDIT_USERS,_T("请点击选择或输入你的登录帐号"));
#else
	this->SetToolTipText(IDC_COMBO_USERS,_T("请选择或输入你的登录帐号"));
#endif
	this->SetToolTipText(IDC_EDIT_PASSWORD,_T("请输入你的密码"));
	this->SetToolTipText(IDC_BUTTON_FORGET_PWD,_T("忘记密码（重置密码）"));
	this->SetToolTipText(IDC_STATIC_SAVEPWD,_T("点击选择是否记住当前密码"));
	this->SetToolTipText(IDC_STATIC_AUTOLOGIN,_T("点击选择下次是否开机自动登录"));
	this->SetToolTipText(IDC_BUTTON_REG,_T("免费注册个人帐号（或公司帐号）"));
	//this->SetToolTipText(IDC_BUTTON_SETTING,_T("设置服务器地址"));
	this->SetToolTipText(IDC_BUTTON_LOGIN,_T("点击登录"));
	this->SetToolTipText(IDC_BUTTON_VISITOR,_T("点击游客进入"));

#ifdef USES_NEW_USER_CTRL
	m_editUser.ShowWindow(SW_SHOW);
	m_btnLineState.ShowWindow(SW_HIDE);
	//m_comboUsers.ShowWindow(SW_HIDE);
#else
	m_comboUsers.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
#endif
	//m_comboUsers.SetComboListBitmap(IDB_COMBO_LIST_LEFT, IDB_COMBO_LIST_RIGHT, IDB_COMBO_LIST_TOP, IDB_COMBO_LIST_BOT);
	//m_comboUsers.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	//m_comboUsers.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	m_btnSkin.Load(IDB_PNG_BTN_SKIN);
	m_btnMin.SetAutoSize(false);
	m_btnMin.Load(IDB_PNG_MIN);
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnLineState.SetAutoSize(false);
	m_btnLineState.Load(IDB_PNG_BTN_STATE_ONLINE);
	m_btnLineState.SetWindowText(_T(""));

#ifdef USES_NEW_USER_CTRL
	//m_editUser.SetPromptText(_T("登录帐号"));
	m_btnDel.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeUsers, 0xffff);
	m_btnDel.SetAutoSize(false);
	m_btnDel.Load(IDB_PNG_HOT_DELETE_S);
	m_btnDel.SetToolTipText(_T("删除帐号信息"));
	m_btnDel.ShowWindow(SW_HIDE);

	m_treeUsers.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeUsers.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeUsers.SetItemHeight(28);
	//m_treeUsers.SetIconSize(32,32);
	//m_treeUsers.SetCallback(&theApp);
	m_treeUsers.ShowWindow(SW_HIDE);
#endif

	//m_editPassword.SetRectangleColor(RGB(0,128,255),RGB(120,176,228));
	//m_editPassword.SetPromptText(_T("请输入帐号密码"));	// ***设置也没用
	m_labelSavePwd.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_labelSavePwd.SetTextColor(theDefaultTextWhiteColor);
	//m_labelSavePwd.SetCheckBoxHoverBorder(TRUE,RGB(0,128,255));
	m_labelAutoLogin.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_labelAutoLogin.SetTextColor(theDefaultTextWhiteColor);
	//m_labelAutoLogin.SetCheckBoxHoverBorder(TRUE,RGB(0,128,255));

	m_btnLogin.SetTextHotMove(false);
	m_btnLogin.SetWindowText(_T("登录"));
	m_btnReg.SetAutoSize(false);
	m_btnReg.SetWindowText(_T("我要注册"));
	m_btnReg.ShowWindow(SW_HIDE);
	m_btnForgetPwd.SetAutoSize(false);
	m_btnForgetPwd.SetWindowText(_T("忘记密码？"));
	//m_btnForgetPwd.SetNorTextColor(RGB(0,128,255));
	//m_btnForgetPwd.SetHotTextColor(RGB(0,128,255));
	m_btnVisitor.SetAutoSize(false);
	//m_btnVisitor.Load(IDB_PNG_BTN_VISITOR);
	m_btnVisitor.SetTextHotMove(false);
	m_btnVisitor.SetWindowText(_T("游客登录通道"));
	m_btnVisitor.ShowWindow(SW_HIDE);
	m_labelError.SetWindowText(_T(""));
	m_labelError.ShowWindow(SW_HIDE);
	m_labelError.SetTextColor(RGB(255,0,0));
	m_labelError.SetAlignText(CLabelEx::Align_Left);

	// hide
	//m_btnSetting.ShowWindow(SW_HIDE);
	//m_btnSetting.SetAutoSize(false);
	//m_btnSetting.SetNorTextColor(RGB(0,128,255));
	//m_btnSetting.SetHotTextColor(RGB(0,128,255));

	SetCtrlColor();

	//theBtnImage.LoadResource(IDB_BITMAP_BTN_7024);
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnLogin.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);
	//m_btnLogin.SetToolTipText(_T("用户登录"));
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnVisitor.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);
	//m_btnVisitor.SetToolTipText(_T("游客登录"));

	if (!m_bAuthLogLogonMode)
	//if (m_sUserAccount.IsEmpty() && m_sUserPassword.IsEmpty())
	{
		if (theApp.m_pBoEB->use("eb"))
		{
			bo::PRESULTSET pResltSet = NULL;
			theApp.m_pBoEB->execsql("SELECT account,password,safepwd,linestate FROM user_login_record_t ORDER BY recordtime DESC", &pResltSet);
			if (pResltSet != NULL)
			{
				for (int i=0; i<pResltSet->rscount; i++)
				{
					const CString sAccount(pResltSet->rsvalues[i]->fieldvalues[0]->v.varcharVal.buffer);
					if (sAccount.IsEmpty() || m_pLoginInfoList.exist(sAccount))
						continue;
					const CString sPassword(pResltSet->rsvalues[i]->fieldvalues[1]->v.varcharVal.buffer);
					bool bSafePwd = pResltSet->rsvalues[i]->fieldvalues[2]->v.booleanVal;
					EB_USER_LINE_STATE nLineState = (EB_USER_LINE_STATE)pResltSet->rsvalues[i]->fieldvalues[3]->v.tinyintVal;
					CLoginInfo::pointer pLoginInfo = CLoginInfo::create(sAccount, sPassword, bSafePwd);
					pLoginInfo->m_nLineState = nLineState;
					m_pLoginInfoList[sAccount] = pLoginInfo;
#ifdef USES_NEW_USER_CTRL
					HTREEITEM hItem = m_treeUsers.InsertItem(sAccount);
					pLoginInfo->m_hItem = hItem;
					m_treeUsers.SetItemData(hItem,(DWORD)pLoginInfo.get());
					if (i==0)
					{
						m_treeUsers.Select(hItem,TVGN_CARET);
						m_sUserAccount = pLoginInfo->m_sAccount;
						m_sUserPassword = pLoginInfo->m_sPassword;
						UpdateData(FALSE);
						m_labelSavePwd.SetCheck(pLoginInfo->m_bSafePwd?TRUE:FALSE);
						m_labelSavePwd.Invalidate();
					}
#else
					m_comboUsers.AddString(sAccount);
#endif
				}
				bo::bodb_free(pResltSet);
			}
			theApp.m_pBoEB->close();
		}
	}
#ifndef USES_NEW_USER_CTRL
	if (m_comboUsers.GetCount() > 0)
	{
		m_comboUsers.SetCurSel(0);
		OnCbnSelchangeComboUsers();
	}else
	{
		//m_sUserAccount = _T("test@entboost.com");
		UpdateData(FALSE);
	}
#endif
	EbSetWindowText();

	//m_sMainExe="ebc.exe";
	//GetExistAppCount();

	if (m_sAppParameter1 == "server_error")
	{

	}else if (!m_sUserAccount.IsEmpty() && !m_sUserPassword.IsEmpty())
	{
		UpdateData(FALSE);
		for (int i=0; i<100; i++)
		{
#ifdef USES_EBCOM_TEST
			const CEBString sAppOnlineKey = theEBClientCore->EB_GetDevAppOnlineKey().GetBSTR();
#else
			const std::string sAppOnlineKey(theEBAppClient.EB_GetDevAppOnlineKey());
#endif
			if (!sAppOnlineKey.empty())
			{
				break;
			}
			Sleep(100);
		}
		// 自动登录
		OnBnClickedButtonLogin();
		m_bAutoLogSuccess = true;	// 必须放后面
	}else
	{
		const CString sAutoRunAccount = theApp.EBC_GetProfileString(_T("system"),_T("auto-run-account"));
		if (!sAutoRunAccount.IsEmpty())
		{
			OnStnClickedStaticAutologin();
			m_labelAutoLogin.SetCheck(TRUE);
			m_labelAutoLogin.Invalidate();
			if (GetExistAppCount()<=1)
			{
				CLoginInfo::pointer pLoginInfo = m_pLoginInfoList[sAutoRunAccount];
				if (pLoginInfo.get()!=0 && !pLoginInfo->m_sAccount.IsEmpty() && !pLoginInfo->m_sPassword.IsEmpty())
				{
					m_sUserAccount = pLoginInfo->m_sAccount;
					m_sUserPassword = pLoginInfo->m_sPassword;
					UpdateData(FALSE);
					m_labelSavePwd.SetCheck(pLoginInfo->m_bSafePwd?TRUE:FALSE);
					m_labelSavePwd.Invalidate();
					if (m_sAppParameter1 != "logout")
					{
						// 等级app online key成功，六秒超时
						PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
						for (int i=0; i<100; i++)
						{
#ifdef USES_EBCOM_TEST
							const CEBString sAppOnlineKey = theEBClientCore->EB_GetDevAppOnlineKey().GetBSTR();
#else
							std::string sAppOnlineKey = theEBAppClient.EB_GetDevAppOnlineKey();
#endif
							if (!sAppOnlineKey.empty())
							{
								break;
							}
							Sleep(100);
						}
						// 自动登录
						OnBnClickedButtonLogin();
						m_bAutoLogSuccess = true;	// 必须放后面
					}
				}
			}
		}
	}

	const int POS_DLG_LOGIN_WIDTH = 274;
	const int POS_DLG_LOGIN_HEIGHT = 588;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_LOGIN_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_LOGIN_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
int CDlgLogin::GetExistAppCount(void) const
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
	if (hProcessSnap==NULL) return 0;
	PROCESSENTRY32 pe32;  
	memset(&pe32,0,sizeof(pe32));
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	int result = 0;
	if(::Process32First(hProcessSnap,&pe32))  
	{  
		const std::string sAppPath(theApp.GetAppPath());
		TCHAR szPath[MAX_PATH];
		do  
		{
			if (m_sMainExe.find(pe32.szExeFile) != std::string::npos)
			{
				std::string sExePath((LPCTSTR)theApp.GetAppPath());
				std::string::size_type find = sExePath.find(":\\");
				if (find!=std::string::npos)
					sExePath = sExePath.substr(find+2);

				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
				if(NULL != hProcess)
				{
					DWORD nSize = MAX_PATH;
					memset(szPath,0,nSize);
					GetProcessImageFileName(hProcess, szPath, MAX_PATH);
					std::string sFindPath(szPath);
					if (sFindPath.empty() || sFindPath.find(sExePath)!=std::string::npos)
					{
						result++;
					}
					CloseHandle(hProcess);
				}else
				{
					result++;
				}
			}
		}  
		while(::Process32Next(hProcessSnap,&pe32));   
	}
	CloseHandle(hProcessSnap);
	//CString stext;
	//stext.Format(_T("%d"),result);
	//AfxMessageBox(stext);
	return result;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onOAuthForward(BSTR sOAuthUrl)
//{
//	// 这里用浏览器实现
//	ShellExecuteW(NULL, L"open", sOAuthUrl, NULL, NULL,SW_SHOW);
//}
//#else
//#endif
LRESULT CDlgLogin::OnMessageOAuthForward(WPARAM wParam, LPARAM lParam)
{
	// 这里用浏览器实现
#ifdef USES_EBCOM_TEST
	const BSTR sOAuthUrl = (const BSTR)wParam;
	ShellExecuteW(NULL, L"open", sOAuthUrl, NULL, NULL,SW_SHOW);
#else
	const char* sOAuthUrl = (const char*)wParam;
	ShellExecute(NULL, "open", sOAuthUrl, NULL, NULL,SW_SHOW);
#endif
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onLogonSuccess(IEB_AccountInfo* pAccountInfo)
//{
//	this->PostMessage(WM_CLOSE, 0, 0);
//}
//#else
//#endif
LRESULT CDlgLogin::OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	unsigned long pEntManagerUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL);
#else
	unsigned long pEntManagerUrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL, &pEntManagerUrl);
#endif
	if (pEntManagerUrl != NULL && strlen((const char*)pEntManagerUrl)>0)
	{
		theApp.m_sEntManagerUrl = (const char*)pEntManagerUrl;
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#else
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#endif
	}
	CEbDialogBase::OnOK();
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onLogonTimeout(IEB_AccountInfo * pAccountInfo)
//{
//	this->ShowWindow(SW_SHOW);
//	CDlgMessageBox::EbMessageBox(this,"登录超时！","请重新登录。",CDlgMessageBox::IMAGE_WARNING,6);
//}
//#else
//#endif
LRESULT CDlgLogin::OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam)
{
	CString sErrorString;
	sErrorString.Format(_T("登录超时，请重新登录！错误代码:%d"),(int)EB_STATE_TIMEOUT_ERROR);
	if (theApp.m_pHandler!=NULL)
		theApp.m_pHandler->OnLogonError(EB_STATE_TIMEOUT_ERROR,sErrorString);
	if (m_bAuthLogLogonMode)
	{
		this->OnOK();
		return 0;
	}

	this->ShowWindow(SW_SHOW);
	m_labelError.ShowWindow(SW_HIDE);
	m_labelError.SetWindowText(sErrorString);
	m_labelError.SetToolTipText(sErrorString);
	m_labelError.ShowWindow(SW_SHOW);
	//CDlgMessageBox::EbMessageBox(this,"登录超时！","请重新登录。",CDlgMessageBox::IMAGE_WARNING,6);
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onLogonError(IEB_AccountInfo * pAccountInfo, EB_STATE_CODE nErrorCode)
//{
//	this->ShowWindow(SW_SHOW);
//	CString sTitle;
//	CString sText;
//	if (nErrorCode == EB_STATE_UNAUTH_ERROR)
//	{
//		sTitle = "该帐号邮箱未通过验证：";
//		sText = "请查收邮件，点击验证链接，完成注册！";
//	}else if (nErrorCode == EB_STATE_ACCOUNT_FREEZE)
//	{
//		sTitle = "帐号已经被临时冻结：";
//		sText = "请联系公司客服！";
//	}else if (nErrorCode == EB_STATE_MAX_RETRY_ERROR)
//	{
//		sTitle = "错误次数太频繁，帐号被临时锁住：";
//		sText = "请稍候再试！";
//	}else if (nErrorCode == EB_STATE_NOT_AUTH_ERROR)
//	{
//		sTitle = "当前版本没有权限：";
//		sText = "请联系公司客服！";
//	}else
//	{
//		sTitle = "登录失败：";
//		sText = "请重新登录！";
//	}
//	CDlgMessageBox::EbMessageBox(this,sTitle,sText,CDlgMessageBox::IMAGE_WARNING,6);
//}
//#else
//#endif

LRESULT CDlgLogin::OnMessageLogonError(WPARAM wParam, LPARAM lParam)
{
	const int nErrorCode = lParam;
	CString sErrorString;
	if (nErrorCode == EB_STATE_UNAUTH_ERROR)
	{
		sErrorString.Format(_T("帐号未激活，请查收邮件，点击验证链接，完成注册！错误代码:%d"),nErrorCode);
	}else if (nErrorCode == EB_STATE_ACCOUNT_FREEZE)
	{
		sErrorString.Format(_T("帐号已经被临时冻结，请联系公司客服！错误代码:%d"),nErrorCode);
	}else if (nErrorCode == EB_STATE_MAX_RETRY_ERROR)
	{
		sErrorString.Format(_T("错误次数太多，帐号被临时锁住，请稍候再试！错误代码:%d"),nErrorCode);
	}else if (nErrorCode == EB_STATE_NOT_AUTH_ERROR)
	{
		sErrorString.Format(_T("没有权限错误！错误代码:%d"),nErrorCode);
	}else if (nErrorCode == EB_STATE_ACC_PWD_ERROR)
	{
		sErrorString.Format(_T("帐号或密码错误，请重新输入！错误代码:%d"),nErrorCode);
	}else if (nErrorCode == EB_STATE_ACCOUNT_NOT_EXIST)
	{
		sErrorString.Format(_T("帐号不存在，请重新输入！错误代码:%d"),nErrorCode);
	}else
	{
		sErrorString.Format(_T("登录失败，请重试！错误代码:%d"),nErrorCode);
	}
	if (theApp.m_pHandler!=NULL)
		theApp.m_pHandler->OnLogonError(nErrorCode,sErrorString);
	if (m_bAuthLogLogonMode)
	{
		this->OnOK();
		return 0;
	}

	this->ShowWindow(SW_SHOW);

	m_labelError.ShowWindow(SW_HIDE);
	m_labelError.SetWindowText(sErrorString);
	m_labelError.SetToolTipText(sErrorString);
	m_labelError.ShowWindow(SW_SHOW);
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onAppIdError(EB_STATE_CODE nState)
//{
//	this->ShowWindow(SW_SHOW);
//	if (nState == EB_STATE_NOT_SUPPORT_VERSION_ERROR)
//		CDlgMessageBox::EbMessageBox(this,"系统不支持当前版本：","请更新客户端后再试！",CDlgMessageBox::IMAGE_WARNING);
//	else
//	{
//		CDlgMessageBox::EbMessageBox(this,"系统验证失败！","请退出程序后重新启动。",CDlgMessageBox::IMAGE_WARNING);
//		m_bReload = true;
//		this->OnOK();
//	}
//}
//#else
//#endif
LRESULT CDlgLogin::OnMessageAppIdError(WPARAM wParam, LPARAM lParam)
{
	const int nErrorCode = wParam;
	if (theApp.m_pHandler!=NULL)
		theApp.m_pHandler->OnLogonError(nErrorCode);
	if (m_bAuthLogLogonMode)
	{
		this->OnOK();
		return 0;
	}

	this->ShowWindow(SW_SHOW);
	if (nErrorCode == EB_STATE_NOT_SUPPORT_VERSION_ERROR)
	{
		CString sErrorString;
		sErrorString.Format(_T("系统不支持当前版本，请更新客户端后再试！错误代码:%d"),nErrorCode);
		m_labelError.ShowWindow(SW_HIDE);
		m_labelError.SetWindowText(sErrorString);
		m_labelError.SetToolTipText(sErrorString);
		m_labelError.ShowWindow(SW_SHOW);
		//CDlgMessageBox::EbMessageBox(this,"系统不支持当前版本：","请更新客户端后再试！",CDlgMessageBox::IMAGE_WARNING);
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"系统验证失败！","请退出程序后重新启动。",CDlgMessageBox::IMAGE_WARNING);
		m_bReload = true;
		this->OnOK();
	}
	return 0;
}

bool CDlgLogin::DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified) const
{
	char     *pBuf   = NULL ;
	int      nBufLen = 0 ;
	TRY
	{
		// connection
		CInternetSession   sess ;
		sess.SetOption (INTERNET_OPTION_CONNECT_TIMEOUT, 30 * 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_BACKOFF, 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_RETRIES, 1) ;

		DWORD       dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;
		CHttpFile   * pF = (CHttpFile*)sess.OpenURL(strUrl, 1, dwFlag); ASSERT(pF);
		if (!pF)
		{
			return false;
		}
		//{AfxThrowInternetException(1);}

		// connection status
		CString      str ;
		//pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;

		pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;
		if (str != _T("200"))
		{
			pF->Close() ;
			delete pF ;
			return false;
			//AfxThrowInternetException(1);
		}

		// confirm update
		pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str);
		pOutNewLastModified = str;
		if (strOldLastModified==pOutNewLastModified)
		{
			pF->Close() ;
			delete pF ;
			return false;
		}

		/* pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str) ;
		if (!pPara->pThis->df_Notify (GetMsg_CheckTime(), &WM_DLF_CHECKTIME(&pPara->strFileURL, &str)))
		{
		pF->Close() ;
		delete pF ;
		AfxThrowInternetException(1);
		}*/

		// start download
		pF->QueryInfo (HTTP_QUERY_CONTENT_LENGTH, str) ; // file's length
		//UINT msg = pUpdateDlg->GetMsg_Progress();
		//::SendMessage(pUpdateDlg->m_hWnd, msg, (WPARAM)pTask, 0);
		//pPara->pThis->df_Notify (GetMsg_StartDownload(), &pPara->strFileURL) ;

		nBufLen = _ttoi(str);
		if (nBufLen > 0)
		{
			// know file's size
			//pUpdateDlg->m_nFileSize = nBufLen;
			//pTask->nTotal = nBufLen;
			char    * p = (pBuf = new char[nBufLen+8]) ;
			ZeroMemory (p, nBufLen+8) ;

			int     nLen = nBufLen;
			while (IsWindow(this->GetSafeHwnd()))
			{
				// download 8K every
				DWORD tickNow = GetTickCount();
				int   n = pF->Read (p, (nLen < 2*1024) ? nLen : 2*1024) ;
				if (n <= 0)
					break ; // success exit
				p += n ; nLen -= n ;

				//pTask->nDownload += n;

				//static int gCounter = 0;
				//if ((++gCounter) % 10 == 1)
				//{
				//	pTask->nDownDelta = n;
				//	pTask->tickEnd = GetTickCount();
				//	pTask->tickDelta = pTask->tickEnd - tickNow;

				//	UINT msg = pUpdateDlg->GetMsg_Progress();
				//	pUpdateDlg->SendMessage(msg, (WPARAM)pTask, 0);
				//	//这里可以显示进度条信息
				//	//pPara->pThis->df_Notify (GetMsg_Progress(), &WM_DLF_PROGRESS(&pPara->strFileURL, nBufLen-nLen, nBufLen)) ;
				//}
			}

			// interrupted
			if (nLen != 0)
			{
				delete[] pBuf; pBuf=NULL;
				nBufLen = 0 ;
			}
		}
		/*else
		{
		// don't know file's size, save context to a temp file.
		bstr_t   strFile = QueryTempFilePath() ;
		CFile    outFile (strFile, CFile::modeCreate|CFile::modeReadWrite) ;
		int      n = (int)pF->GetLength() ;
		while (IsWindow(pUpdateDlg->GetSafeHwnd()) && n)
		{
		char   * pa = new char[n] ;
		n = pF->Read (pa, n) ;
		outFile.Write (pa, n) ;
		//pPara->pThis->df_Notify (GetMsg_Progress(), &WM_DLF_PROGRESS(&pPara->strFileURL, (int)outFile.GetLength(), 0)) ;
		n = (int)pF->GetLength() ;
		delete[] pa ;
		}
		outFile.Close() ;

		// success
		if (n == 0)
		{
		DWORD   dw ;
		if (::InternetQueryDataAvailable ((HINTERNET)(*pF), &dw, 0, 0) && (dw == 0))
		{
		//LoadFileToBuffer (strFile, pBuf, nBufLen) ;
		}
		}
		::DeleteFile(strFile) ;
		}*/

		pF->Close() ;
		delete pF ;
	}
	CATCH_ALL(e) {}
	END_CATCH_ALL

    if (pBuf)
    {
		FILE * f = fopen(strFile,"wb");
		if (f!=NULL)
		{
			fwrite(pBuf,1,nBufLen,f);
			fclose(f);
			delete[] pBuf;
			return true;
		}
        delete[] pBuf;
	}
	return false;
}

//#ifdef USES_EBCOM_TEST
//void CDlgLogin::Fire_onAppIdSuccess(eb::bigint sAppId, const CEBString& sAppOnlineKey)
//{
//	// 帐号前缀
//	const unsigned long pAccountPrefix = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX);
//	if (pAccountPrefix != NULL && strlen((const char*)pAccountPrefix)>0)
//	{
//		theApp.m_sAccountPrefix = (const char*)pAccountPrefix;
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
//	}
//
//	const unsigned long nOpenRegisiter = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_REGISTER);
//	m_btnReg.ShowWindow(nOpenRegisiter==0?SW_HIDE:SW_SHOW);
//	theApp.m_nOpenRegister = nOpenRegisiter;
//	const unsigned long nOpenVisitor = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_VISITOR);
//	m_btnVisitor.ShowWindow(nOpenVisitor==0?SW_HIDE:SW_SHOW);
//	theApp.m_bOpenVisitor = nOpenVisitor==0?false:true;
//	const unsigned long pForgetPwdUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL);
//	m_btnForgetPwd.ShowWindow(pForgetPwdUrl==NULL?SW_HIDE:SW_SHOW);
//	//if (theApp.m_sAccountPrefix.empty())
//	//{
//	//	theApp.m_bSendRegMail = false;
//	//}else
//	//{
//	//	unsigned long nSendRegMail = 0;
//	//	theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
//	//	theApp.m_bSendRegMail = nSendRegMail==0?false:true;
//	//}
//	const unsigned long nSendRegMail = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL);
//	theApp.m_bSendRegMail = nSendRegMail==0?false:true;
//
//	const unsigned long pDeployId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID);
//	if (pDeployId!=NULL)
//	{
//		m_nDeployId = eb_atoi64((const char*)pDeployId);
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
//	}
//
//	// 企业LOGO
//	const unsigned long pEntLogoUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL);
//	const CString sImageTempIniPath = theApp.GetAppPath()+_T("\\img\\temp.ini");
//	const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
//	if (pEntLogoUrl != NULL && strlen((const char*)pEntLogoUrl)>0)
//	{
//		// http://test-um.entboost.com/images/entlogo.png
//		const CString sEntLogoUrl = (const char*)pEntLogoUrl;
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,pEntLogoUrl);
//		char lpszEntLogoLastModified[64];
//		memset(lpszEntLogoLastModified,0,64);
//		::GetPrivateProfileString(_T("entlogo"),_T("last_modified"),_T(""),lpszEntLogoLastModified,64,sImageTempIniPath);
//		CString sNewLastModified;
//		const CString sEntLogoImagePathTemp = theApp.GetAppPath()+_T("\\img\\entlogotemp");	// 先保存到临时中间文件
//		if (DownloadHttpFile(sEntLogoUrl,sEntLogoImagePathTemp,lpszEntLogoLastModified,sNewLastModified))
//		{
//			const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
//			theApp.FreeEntLogo();
//			CopyFile(sEntLogoImagePathTemp,sEntImagePath,FALSE);
//			theApp.SetEntLogo(sEntImagePath);
//			this->Invalidate();
//			DeleteFile(sEntLogoImagePathTemp);
//			::WritePrivateProfileString(_T("entlogo"),_T("last_modified"),sNewLastModified,sImageTempIniPath);
//		}
//	}else if (!theApp.IsEbDefaultEntLogo())
//	{
//		theApp.SetEntLogo(NULL);
//		this->Invalidate();
//		if (::PathFileExists(sEntImagePath))
//		{
//			// 删除企业LOGO，避免下次显示。
//			DeleteFile(sEntImagePath);
//			DeleteFile(sImageTempIniPath);
//		}
//	}
//
//	const unsigned long pProductName = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
//	if (pProductName!=NULL && strlen((const char*)pProductName)>0)
//	{
//		m_sProductName = (const char*)pProductName;
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
//		this->Invalidate();
//		EbSetWindowText();
//	}
//}
//#else
//#endif
LRESULT CDlgLogin::OnMessageAppIdSuccess(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	// 帐号前缀
	const unsigned long pAccountPrefix = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX);
	if (pAccountPrefix != NULL && strlen((const char*)pAccountPrefix)>0)
	{
		theApp.m_sAccountPrefix = (const char*)pAccountPrefix;
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
	}

	const unsigned long nOpenRegisiter = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_REGISTER);
	m_btnReg.ShowWindow(nOpenRegisiter==0?SW_HIDE:SW_SHOW);
	theApp.m_nOpenRegister = nOpenRegisiter;
	const unsigned long nOpenVisitor = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_VISITOR);
	m_btnVisitor.ShowWindow(nOpenVisitor==0?SW_HIDE:SW_SHOW);
	theApp.m_bOpenVisitor = nOpenVisitor==0?false:true;
	const unsigned long nLicenseUser = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER);
	theApp.m_bLicenseUser = nLicenseUser==0?false:true;
	const unsigned long pForgetPwdUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL);
	m_btnForgetPwd.ShowWindow(pForgetPwdUrl==NULL?SW_HIDE:SW_SHOW);
	//if (theApp.m_sAccountPrefix.empty())
	//{
	//	theApp.m_bSendRegMail = false;
	//}else
	//{
	//	unsigned long nSendRegMail = 0;
	//	theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
	//	theApp.m_bSendRegMail = nSendRegMail==0?false:true;
	//}
	const unsigned long nSendRegMail = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL);
	theApp.m_bSendRegMail = nSendRegMail==0?false:true;

	const unsigned long pDeployId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID);
	if (pDeployId!=NULL)
	{
		m_nDeployId = eb_atoi64((const char*)pDeployId);
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
	}

	// 企业LOGO
	if (theApp.m_bLicenseUser)
	{
		const unsigned long pEntLogoUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL);
		const CString sImageTempIniPath = theApp.GetAppPath()+_T("\\img\\temp.ini");
		const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
		if (pEntLogoUrl != NULL && strlen((const char*)pEntLogoUrl)>0)
		{
			// http://test-um.entboost.com/images/entlogo.png
			const CString sEntLogoUrl = (const char*)pEntLogoUrl;
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,pEntLogoUrl);
			char lpszEntLogoLastModified[64];
			memset(lpszEntLogoLastModified,0,64);
			::GetPrivateProfileString(_T("entlogo"),_T("last_modified"),_T(""),lpszEntLogoLastModified,64,sImageTempIniPath);
			CString sNewLastModified;
			const CString sEntLogoImagePathTemp = theApp.GetAppPath()+_T("\\img\\entlogotemp");	// 先保存到临时中间文件
			if (DownloadHttpFile(sEntLogoUrl,sEntLogoImagePathTemp,lpszEntLogoLastModified,sNewLastModified))
			{
				const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
				theApp.FreeEntLogo();
				CopyFile(sEntLogoImagePathTemp,sEntImagePath,FALSE);
				theApp.SetEntLogo(sEntImagePath);
				this->Invalidate();
				DeleteFile(sEntLogoImagePathTemp);
				::WritePrivateProfileString(_T("entlogo"),_T("last_modified"),sNewLastModified,sImageTempIniPath);
			}
		}else if (!theApp.IsEbDefaultEntLogo())
		{
			theApp.SetEntLogo(NULL);
			this->Invalidate();
			if (::PathFileExists(sEntImagePath))
			{
				// 删除企业LOGO，避免下次显示。
				DeleteFile(sEntImagePath);
				DeleteFile(sImageTempIniPath);
			}
		}
		const unsigned long pProductName = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
		if (pProductName!=NULL && strlen((const char*)pProductName)>0)
		{
			m_sProductName = (const char*)pProductName;
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
			this->Invalidate();
			EbSetWindowText();
		}
	}
#else
	// 帐号前缀
	unsigned long pAccountPrefix = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,&pAccountPrefix);
	if (pAccountPrefix != NULL && strlen((const char*)pAccountPrefix)>0)
	{
		theApp.m_sAccountPrefix = (const char*)pAccountPrefix;
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
	}

	unsigned long nOpenRegisiter = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_REGISTER,&nOpenRegisiter);
	m_btnReg.ShowWindow(nOpenRegisiter==0?SW_HIDE:SW_SHOW);
	theApp.m_nOpenRegister = nOpenRegisiter;
	unsigned long nOpenVisitor = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_VISITOR,&nOpenVisitor);
	m_btnVisitor.ShowWindow(nOpenVisitor==0?SW_HIDE:SW_SHOW);
	theApp.m_bOpenVisitor = nOpenVisitor==0?false:true;
	unsigned long nLicenseUser = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER,&nLicenseUser);
	theApp.m_bLicenseUser = nLicenseUser==0?false:true;
	unsigned long pForgetPwdUrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,&pForgetPwdUrl);
	m_btnForgetPwd.ShowWindow(pForgetPwdUrl==NULL?SW_HIDE:SW_SHOW);
	//if (theApp.m_sAccountPrefix.empty())
	//{
	//	theApp.m_bSendRegMail = false;
	//}else
	//{
	//	unsigned long nSendRegMail = 0;
	//	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
	//	theApp.m_bSendRegMail = nSendRegMail==0?false:true;
	//}
	unsigned long nSendRegMail = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
	theApp.m_bSendRegMail = nSendRegMail==0?false:true;
	unsigned long nSaveConversatios = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS,&nSaveConversatios);
	theApp.m_bSaveConversationLocal = (nSaveConversatios&1)==1?true:false;
	theApp.m_bSaveConversationServer = (nSaveConversatios&2)==2?true:false;
	unsigned long nAuthContact = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT,&nAuthContact);
	theApp.m_bAuthContact = nAuthContact==1?true:false;
	unsigned long nDisableUserCloudDrive = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE,&nDisableUserCloudDrive);
	theApp.m_bDisableUserCloudDrive = nDisableUserCloudDrive==1?true:false;
	unsigned long nDisableGroupSharedCloud = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD,&nDisableGroupSharedCloud);
	theApp.m_bDisableGroupSharedCloud = nDisableGroupSharedCloud==1?true:false;
	unsigned long nDisableModifyPasswd = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD,&nDisableModifyPasswd);
	theApp.m_bDisableModifyPasswd = nDisableModifyPasswd==1?true:false;
	unsigned long nDisableVideo = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_VIDEO,&nDisableVideo);
	theApp.m_bDisableVideo = nDisableVideo==1?true:false;
	unsigned long nDisableRD = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP,&nDisableRD);
	theApp.m_bDisableRemoteDesktop = nDisableRD==1?true:false;

	unsigned long pDeployId = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,&pDeployId);
	if (pDeployId!=NULL)
	{
		theApp.m_nDeployId = eb_atoi64((const char*)pDeployId);
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
	}

	if (theApp.m_bLicenseUser)
	{
		//// 企业LOGO
		//unsigned long pEntLogoUrl = 0;
		//theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,&pEntLogoUrl);
		//const CString sImageTempIniPath = theApp.GetAppPath()+_T("\\img\\temp.ini");
		//const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
		//if (pEntLogoUrl != NULL && strlen((const char*)pEntLogoUrl)>0)
		//{
		//	// http://test-um.entboost.com/images/entlogo.png
		//	const CString sEntLogoUrl = (const char*)pEntLogoUrl;
		//	theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,pEntLogoUrl);
		//	char lpszEntLogoLastModified[64];
		//	memset(lpszEntLogoLastModified,0,64);
		//	::GetPrivateProfileString(_T("entlogo"),_T("last_modified"),_T(""),lpszEntLogoLastModified,64,sImageTempIniPath);
		//	CString sNewLastModified;
		//	const CString sEntLogoImagePathTemp = theApp.GetAppPath()+_T("\\img\\entlogotemp");	// 先保存到临时中间文件
		//	if (DownloadHttpFile(sEntLogoUrl,sEntLogoImagePathTemp,lpszEntLogoLastModified,sNewLastModified))
		//	{
		//		const CString sEntImagePath = theApp.GetAppPath()+_T("\\img\\entlogo");			// 企业定制LOGO
		//		theApp.FreeEntLogo();
		//		CopyFile(sEntLogoImagePathTemp,sEntImagePath,FALSE);
		//		theApp.SetEntLogo(sEntImagePath);
		//		this->Invalidate();
		//		DeleteFile(sEntLogoImagePathTemp);
		//		::WritePrivateProfileString(_T("entlogo"),_T("last_modified"),sNewLastModified,sImageTempIniPath);
		//	}
		//}else if (!theApp.IsEbDefaultEntLogo())
		//{
		//	theApp.SetEntLogo(NULL);
		//	this->Invalidate();
		//	if (::PathFileExists(sEntImagePath))
		//	{
		//		// 删除企业LOGO，避免下次显示。
		//		DeleteFile(sEntImagePath);
		//		DeleteFile(sImageTempIniPath);
		//	}
		//}

		unsigned long pProductName = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,&pProductName);
		if (pProductName!=NULL && strlen((const char*)pProductName)>0)
		{
			m_sProductName = (const char*)pProductName;
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
			this->Invalidate();
			EbSetWindowText();
			theApp.m_sProductName = m_sProductName;
		}
	}
#endif

	if (!theApp.m_bLicenseUser)
	{
#ifdef USES_EBCOM_TEST
		const CEBString sEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
		const CEBString sEnterprise = theSetting.GetEnterprise();
#endif
		if (sEnterprise.find("恩布")==std::string::npos)
		{
			m_sProductName = "恩布互联";
			theApp.m_sProductName = m_sProductName;
			this->Invalidate();
		}
	}

	return 0;
}
//#endif

void CDlgLogin::OnBnClickedButtonVisitor()
{
	if (!theApp.m_bOpenVisitor) return;
	if (!UpdateData())
		return;
	
	//if (m_sUserAccount.IsEmpty())
	//{
	//	this->GetDlgItem(IDC_COMBO_USERS)->SetFocus();
	//	AfxMessageBox(_T("请输入服务器地址！"));
	//	return;
	//}
	if (!m_sUserAccount.IsEmpty())
	{
		if (!StartClient())
		{
			CDlgMessageBox::EbDoModal(this,"本地服务启动失败！","",CDlgMessageBox::IMAGE_WARNING);
			return;
		}
	}
	m_bAutoLogSuccess = false;
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LogonByVisitor(m_sReqCode.c_str());
#else
	theEBAppClient.EB_LogonByVisitor(m_sReqCode.c_str());
#endif
}

//void CDlgLogin::OnBnClickedButtonSetting()
//{
//	CDlgSetting dlgSetting;
//	dlgSetting.DoModal();
//	// 需要重新登录；
//	if (m_sOrgServer!= dlgSetting.GetServer())
//	{
//		CString sText;
//		sText.Format(_T("修改服务器地址，需要重新加载验证流程才能生效，确定立即加载吗？"), m_sUserAccount);
//		if (CDlgMessageBox::EbDoModal(this,"重新加载验证流程",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
//		{
//			m_bReload = true;
//			this->OnOK();
//		}
//	}
//}

void CDlgLogin::OnDestroy()
{
#ifdef USES_EBCOM_TEST
	this->DispEventUnadvise(theEBClientCore);
#else
	theEBAppClient.EB_SetMsgHwnd(NULL);
#endif
	CEbDialogBase::OnDestroy();
	m_menuState.DestroyMenu();
	m_menuSkin.DestroyMenu();
}

void CDlgLogin::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = m_btnMin.GetImgWidth();
	const int const_minbtn_height = m_btnMin.GetImgHeight();
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMin.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	if (m_btnSkin.GetSafeHwnd())
		m_btnSkin.MoveWindow(btnx,2,const_minbtn_width,const_minbtn_height);

	const int nLineTop = POS_ADIMG_TOP+POS_ADIMG_SIGE;
	const int nLineLeft = cx/2-m_btnLineState.GetImgWidth()/2;
	m_btnLineState.MovePoint(nLineLeft,nLineTop);
	const int const_textleft = POS_STA_LEFT+2;
	int y = 218;
#ifdef USES_NEW_USER_CTRL
	if (m_editUser.GetSafeHwnd()!=0)
		m_editUser.MoveWindow(const_textleft, y, cx-const_textleft*2, 22);
#else
	if (m_comboUsers.GetSafeHwnd())
	{
		m_comboUsers.MoveWindow(const_textleft, y, cx-const_textleft*2, 22);
	}
#endif
	if (m_editPassword.GetSafeHwnd()!=0)
	{
		y += 55;
		m_editPassword.MoveWindow(const_textleft, y, cx-const_textleft*2, 22);
	}
	const int const_linestate_width = m_btnLineState.GetImgWidth();
	const int const_linestate_height = m_btnLineState.GetImgHeight();
	y += 34;
	const int const_check_width = 100;
	const int const_check_height = 14;
	if (m_labelSavePwd.GetSafeHwnd())
	{
		m_labelSavePwd.MoveWindow(const_textleft, y, const_check_width, const_check_height);
	}
	if (m_labelAutoLogin.GetSafeHwnd())
	{
		m_labelAutoLogin.MoveWindow(cx-const_textleft-80, y, const_check_width, const_check_height);
	}

	//m_btnLineState.MovePoint(70,y);
	//m_btnForgetPwd.MovePoint(cx-const_textleft-60,y,60,18);
	//const int const_check_width = 100;
	//const int const_check_height = 14;
	//if (m_labelSavePwd.GetSafeHwnd())
	//{
	//	y += 30;
	//	m_labelSavePwd.MoveWindow(const_textleft, y, const_check_width, const_check_height);
	//}
	//if (m_labelAutoLogin.GetSafeHwnd())
	//{
	//	y += 25;
	//	m_labelAutoLogin.MoveWindow(const_textleft, y, const_check_width, const_check_height);
	//}
	y += 50;
	const int const_btn_width = 207;//m_btnLogin.GetImgWidth();// 80;
	const int const_btn_height = 32;//m_btnLogin.GetImgHeight();// 30;
	int x = (cx-const_btn_width)/2;
	m_btnLogin.MovePoint(x,y,const_btn_width,const_btn_height);
	y += (const_btn_height+16);
	m_btnReg.MovePoint(x,y,60,18);
	m_btnForgetPwd.MovePoint(cx-const_textleft-60,y,60,18);
	//m_btnReg.MovePoint(x,y,207,32);
	//m_btnReg.MovePoint(x,y);
	y += 68;
	m_btnVisitor.MovePoint(x,y,const_btn_width,const_btn_height);
	//y = cy-40;
	//m_btnSetting.MovePoint(cx-const_textleft-60,y+5,60,18);
	if (m_labelError.GetSafeHwnd()!=0)
		m_labelError.MoveWindow(5,cy-35,cx-10,30);
}

void CDlgLogin::OnCbnSelchangeComboUsers()
{
	int ncursel = m_comboUsers.GetCurSel();
	if (ncursel >= 0)
	{
		CString sSelAccount;
		m_comboUsers.GetLBText(ncursel, sSelAccount);
		CLoginInfo::pointer pLoginInfo = m_pLoginInfoList[sSelAccount];
		if (pLoginInfo.get()==0)
			return;
		m_sUserAccount = pLoginInfo->m_sAccount;
		m_sUserPassword = pLoginInfo->m_sPassword;
		UpdateData(FALSE);
		m_labelSavePwd.SetCheck(pLoginInfo->m_bSafePwd?TRUE:FALSE);
		m_labelSavePwd.Invalidate();

		EB_USER_LINE_STATE pOldLineState = m_nOutLineState;
		m_nOutLineState = pLoginInfo->m_nLineState;
		if (m_nOutLineState != pOldLineState)
		{
			if (m_nOutLineState == EB_LINE_STATE_AWAY)
				m_btnLineState.Load(IDB_PNG_BTN_STATE_AWAY);
			else if (m_nOutLineState == EB_LINE_STATE_BUSY)
				m_btnLineState.Load(IDB_PNG_BTN_STATE_BUSY);
			else// if (m_nOutLineState == EB_LINE_STATE_ONLINE_NEW)
				m_btnLineState.Load(IDB_PNG_BTN_STATE_ONLINE);
			m_btnLineState.Invalidate();
			this->SetToolTipText(IDC_BUTTON_LINESTATE,GetLineStateText(m_nOutLineState));
		}

	}
}

void CDlgLogin::OnCbnEditchangeComboUsers()
{
	// TODO: Add your control notification handler code here
	CString sAccount;
	m_comboUsers.GetWindowText(sAccount);
	m_sUserPassword = _T("");
	m_labelSavePwd.SetCheck(FALSE);
	m_labelSavePwd.Invalidate();
	for (int i=0; i<m_comboUsers.GetCount(); i++)
	{
		CString sText;
		m_comboUsers.GetLBText(i,sText);
		if (sText == sAccount)
		{
			CLoginInfo::pointer pLoginInfo = m_pLoginInfoList[sAccount];
			if (pLoginInfo.get()==0)
				break;
			m_sUserPassword = pLoginInfo->m_sPassword;
			m_labelSavePwd.SetCheck(pLoginInfo->m_bSafePwd?TRUE:FALSE);
			m_labelSavePwd.Invalidate();
			break;
		}
	}
	m_editPassword.SetWindowText(m_sUserPassword);
	//((CButton*)this->GetDlgItem(IDC_CHECK_SAVEPWD))->SetCheck(m_bSavePwd);
}

void CDlgLogin::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		DrawInfo("");
		//CString m_sOutText1 = theSetting.GetLoginText().c_str();
		//DrawInfo(m_sOutText1);
		//CEbDialogBase::OnPaint();
	}
	return;
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		COLORREF m_clrSelectBorder = RGB(0,0,0);

		CRect rcItem;
		this->GetClientRect(&rcItem);
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_clrSelectBorder);	//边框颜色
		CBrush BKBrush,*pOldBrush;
		BKBrush.CreateSolidBrush( theApp.GetMainColor() );	//选中后的颜色
		pOldPen = dc.SelectObject(&BorderPen);
		pOldBrush = dc.SelectObject(&BKBrush);

		// 先画边框
		dc.Rectangle(&rcItem);
		// 再画四个圆狐
		const int const_arc_offset = 5;
		CRect rect1(0,0,const_arc_offset+1,const_arc_offset+1);
		CRect rect2(rcItem.Width()-(const_arc_offset+1),0,rcItem.Width(),const_arc_offset+1);
		CRect rect3(rcItem.Width()-(const_arc_offset+1),rcItem.Height()-(const_arc_offset+1),rcItem.Width(),rcItem.Height());
		CRect rect4(0,rcItem.Height()-(const_arc_offset+1),const_arc_offset+1,rcItem.Height());
		dc.Arc(&rect1, CPoint(rect1.right-1,0), CPoint(0,rect1.bottom-1));
		dc.Arc(&rect2, CPoint(rect2.right,rect2.bottom-1), CPoint(rect2.left+1, 0));
		dc.Arc(&rect3, CPoint(rect3.left+1,rect3.bottom), CPoint(rect3.right, rect3.top+1));
		dc.Arc(&rect4, CPoint(rect4.left, rect4.top+1), CPoint(rect4.right-1,rect4.bottom));

		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		//COLORREF m_clrSelectBorder = RGB(0,0,0);
		//CPen BorderPen,*pOldPen;
		//BorderPen.CreatePen(PS_SOLID,1, m_clrSelectBorder);	//边框颜色
		//BKBrush.CreateSolidBrush( m_colorDefault );	//选中后的颜色
		//pOldBrush = dc.SelectObject(&BKBrush);

		//CRect rect;
		//CPaintDC dc(this);
		//GetClientRect(rect);
		//dc.FillSolidRect(rect, m_colorDefault);
		//pOldPen = dc.SelectObject(&BorderPen);
		//dc.Rectangle(&rect);
		//dc.SelectObject(pOldPen);
		//pDC->SelectObject(pOldBrush);
		//CDialog::OnPaint();
   }
}

void CDlgLogin::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgLogin::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgLogin::DrawInfo(const CString & sAdText)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	if (theApp.GetColorGradient())
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(195,theDefaultBgColorBgLight3,theApp.GetColorGradient()));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	}else
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,theApp.GetColorGradient()));
	}
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());

	// 写标题
	const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);

#ifdef USES_EBCOM_TEST
	const CEBString sEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
	const CEBString sEnterprise = theSetting.GetEnterprise();
#endif
	//MessageBoxW(this->GetSafeHwnd(),theFontFamily.c_str(),L"",MB_OK);
	//MessageBoxA(m_sProductName,sEnterprise.c_str(),MB_OK);
	const Gdiplus::PointF pointTitle(10,10);
	if (!m_sProductName.IsEmpty())
	{
		graphics.DrawString(A2W_ACP(m_sProductName),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}else if (sEnterprise.empty())
		graphics.DrawString(L"恩布互联",-1,&fontEbTitle,pointTitle,&brushEbTitle);
	else
		graphics.DrawString(A2W_ACP(sEnterprise.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);

	//// beta
	//const Gdiplus::Font fontEbBeta(&fontFamily, 12, FontStyleBold, UnitPixel);
	//const Gdiplus::PointF pointBeta(rectClient.Width()-70,rectClient.Height()-20);
	//graphics.DrawString(L"V2014.182",-1,&fontEbBeta,pointBeta,&brushEbTitle);

	// AD LOGO
	const Gdiplus::Font fontAdText(&fontFamily, 16, FontStyleRegular, UnitPixel);
	const int constEntImagleft = (rectClient.Width()-POS_ADIMG_SIGE)/2;
	//graphics.DrawImage(theApp.m_imageEntLogo, constEntImagleft, POS_ADIMG_TOP, POS_ADIMG_SIGE, POS_ADIMG_SIGE);
	//// AD TEXT
	//Gdiplus::SolidBrush brushAdString(Gdiplus::Color(34,177,76));
	//const int nAdLeft = POS_ADIMG_LEFT+POS_ADIMG_SIGE;
	//const Gdiplus::RectF rectAdText(nAdLeft,POS_ADIMG_TOP+15,rectClient.Width()-nAdLeft-10,28*4);
	//Gdiplus::StringFormat formatAdText;
	//formatAdText.SetAlignment(Gdiplus::StringAlignmentNear);
	//formatAdText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	//graphics.DrawString(A2W(sAdText),-1,&fontAdText,rectAdText,&formatAdText,&brushAdString);

	// 帐号，密码
	const Gdiplus::Font fontStatic(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushStatic(Gdiplus::Color(38,38,38));
	int y = 198;
	graphics.DrawString(L"邮箱/帐号：",-1,&fontStatic,Gdiplus::PointF(POS_STA_LEFT,y),&brushStatic);
	graphics.DrawString(L"密码：",-1,&fontStatic,Gdiplus::PointF(POS_STA_LEFT,y+55),&brushStatic);
	//graphics.DrawString(L"状态：",-1,&fontStatic,Gdiplus::PointF(POS_STA_LEFT,y+55*2),&brushStatic);

/*
	// 写字
	CFont pNewFontAd;
	pNewFontAd.CreatePointFont(150, _T("宋体"));//创建显示文本的字体
	pOldFond = SelectObject(m_hdcMemory, pNewFontAd.m_hObject);
	::SetBkMode(m_hdcMemory, TRANSPARENT);
	SetTextColor(m_hdcMemory, RGB(34, 177, 76));	// 绿色
	CString m_sOutText1 = sAdText;
	int texty = POS_ADIMG_TOP+15;
	for (int i=0; i<4; i++)
	{
		sOutText = _T("");
		const int const_text_len = min(16, m_sOutText1.GetLength());
		int textlen = 0;
		for (textlen=0; textlen<const_text_len; textlen++)
		{
			sOutText += m_sOutText1.GetAt(textlen);
			if (sOutText.GetAt(textlen) < 0)
			//if (sOutText.GetAt(textlen) >= 0xa1)
			{
				textlen += 1;
				sOutText += m_sOutText1.GetAt(textlen);
			}
		}
		TextOut(m_hdcMemory, POS_ADIMG_LEFT+POS_ADIMG_SIGE, texty, sOutText, sOutText.GetLength());
		texty += 28;
		m_sOutText1 = m_sOutText1.Right(m_sOutText1.GetLength()-textlen);
		if (m_sOutText1.IsEmpty())
			break;
	}
	SelectObject(m_hdcMemory, pOldFond);

	CFont * pFontSta = this->GetFont();
	pOldFond = SelectObject(m_hdcMemory, pFontSta->m_hObject);
	SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	sOutText = _T("帐号：");
	TextOut(m_hdcMemory, POS_STA_LEFT, POS_STA_FIRST_Y, sOutText, sOutText.GetLength());
	sOutText = _T("密码：");
	TextOut(m_hdcMemory, POS_STA_LEFT, POS_STA_FIRST_Y+50, sOutText, sOutText.GetLength());
	SelectObject(m_hdcMemory, pOldFond);
*/
}


void CDlgLogin::OnBnClickedButtonReg()
{
	if (theApp.m_nOpenRegister==0)
		return;

#ifdef USES_EBCOM_TEST
	const unsigned long pRegisterUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_REGISTER_URL);
#else
	unsigned long pRegisterUrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_REGISTER_URL,&pRegisterUrl);
#endif
	if (pRegisterUrl!=0)
	{
		const std::string sRegisterUrl((const char*)pRegisterUrl);
		ShellExecuteA(NULL, "open", sRegisterUrl.c_str(), NULL, NULL,SW_SHOW);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_REGISTER_URL,pRegisterUrl);
#else
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_REGISTER_URL,pRegisterUrl);
#endif
		return;
	}

	//CDlgRegister dlg;
	//if (dlg.DoModal() == IDOK)
	//{
	//	m_sUserAccount = dlg.m_sAccount;
	//	m_sUserPassword = dlg.m_sPassword;
	//	UpdateData(FALSE);
	//}
}

void CDlgLogin::OnBnClickedButtonLinestate()
{
	if (m_menuState.GetSafeHmenu()==NULL)
	{
		m_menuState.CreatePopupMenu();
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_ONLINE,_T("在线"));
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_AWAY,_T("离开"));
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_BUSY,_T("忙碌"));
		if (m_comboUsers.GetCount() > 0)
		{
			m_menuState.AppendMenu(MF_SEPARATOR);
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_ACCOUNT,_T("删除当前帐号"));
		}
	}
	m_menuState.CheckMenuItem(EB_COMMAND_STATE_ONLINE,m_nOutLineState==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
	m_menuState.CheckMenuItem(EB_COMMAND_STATE_AWAY,m_nOutLineState==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
	m_menuState.CheckMenuItem(EB_COMMAND_STATE_BUSY,m_nOutLineState==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);
	CPoint point;
	GetCursorPos(&point);
	m_menuState.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);

	//CMenu m_menu2;
	//m_menu2.LoadMenu( IDR_MENU_LINESTATE );
	//CMenu *pMenu = m_menu2.GetSubMenu(0);
	//pMenu->RemoveMenu(5, MF_BYPOSITION);
	//pMenu->RemoveMenu(4, MF_BYPOSITION);
	//pMenu->RemoveMenu(3, MF_BYPOSITION);
	//pMenu->CheckMenuItem(ID_STATE_ONLINE,m_nOutLineState==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
	//pMenu->CheckMenuItem(ID_STATE_AWAY,m_nOutLineState==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
	//pMenu->CheckMenuItem(ID_STATE_BUSY,m_nOutLineState==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);

	//CPoint point;
	//GetCursorPos(&point);
	//pMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}
void CDlgLogin::OnStateOnline()
{
	if (m_nOutLineState == EB_LINE_STATE_ONLINE_NEW)
	{
		return;
	}
	m_nOutLineState = EB_LINE_STATE_ONLINE_NEW;
	m_btnLineState.Load(IDB_PNG_BTN_STATE_ONLINE);
	this->SetToolTipText(IDC_BUTTON_LINESTATE,GetLineStateText(m_nOutLineState));
	m_btnLineState.Invalidate();
}

void CDlgLogin::OnStateAway()
{
	if (m_nOutLineState == EB_LINE_STATE_AWAY)
	{
		return;
	}
	m_nOutLineState = EB_LINE_STATE_AWAY;
	m_btnLineState.Load(IDB_PNG_BTN_STATE_AWAY);
	this->SetToolTipText(IDC_BUTTON_LINESTATE,GetLineStateText(m_nOutLineState));
	m_btnLineState.Invalidate();
}

void CDlgLogin::OnStateBusy()
{
	if (m_nOutLineState == EB_LINE_STATE_BUSY)
	{
		return;
	}
	m_nOutLineState = EB_LINE_STATE_BUSY;
	m_btnLineState.Load(IDB_PNG_BTN_STATE_BUSY);
	this->SetToolTipText(IDC_BUTTON_LINESTATE,GetLineStateText(m_nOutLineState));
	m_btnLineState.Invalidate();
}

bool DeleteDirectory(const char* strDirName)
{
	CFileFind tempFind;
	char strTempFileFind[MAX_PATH];
	sprintf(strTempFileFind,"%s\\*.*", strDirName);
	BOOL IsFinded = tempFind.FindFile(strTempFileFind);
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if (!tempFind.IsDots()) 
		{
			char strFoundFileName[MAX_PATH];
			strcpy(strFoundFileName, tempFind.GetFileName().GetBuffer(MAX_PATH));
			if (tempFind.IsDirectory())
			{
				if (!DeleteDirectory(tempFind.GetFilePath()))
					return false;
			}
			else
			{
				if (!DeleteFile(tempFind.GetFilePath()))
					return false;
			}
		}
	}
	tempFind.Close();
	if(!RemoveDirectory(strDirName))
	{
		return false;
	}
	return true;
}

//void CDlgLogin::OnDeleteAccount()
//{
//	UpdateData();
//	if (m_sUserAccount.IsEmpty())
//		return;
//
//	CString sUserDirectory;
//	sUserDirectory.Format(_T("%s\\users\\%s"),theApp.GetAppPath(),m_sUserAccount);
//	if (::PathFileExists(sUserDirectory))
//	{
//		CString sText;
//		sText.Format(_T("[%s]帐号及所有聊天信息吗？"), m_sUserAccount);
//		if (CDlgMessageBox::EbDoModal(this,"你确定删除本地！",sText,CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
//			return;
//		if (!DeleteDirectory(sUserDirectory))
//		{
//			CDlgMessageBox::EbDoModal(this,"操作失败，请检查后再试！","",CDlgMessageBox::IMAGE_ERROR);
//			return;
//		}
//	}
//	if (theApp.m_pBoEB->use("eb"))
//	{
//		if (!m_sUserAccount.IsEmpty())
//		{
//			CString sSql;
//			sSql.Format(_T("delete from user_login_record_t where account='%s'"), m_sUserAccount);
//			theApp.m_pBoEB->execsql(sSql);
//		}
//		theApp.m_pBoEB->close();
//	}
//	const int nSel = m_comboUsers.GetCurSel()>=0?m_comboUsers.GetCurSel():m_comboUsers.FindString(-1,m_sUserAccount);
//	if (nSel >= 0)
//	{
//		m_comboUsers.DeleteString(nSel);
//		if (m_comboUsers.GetCount()>nSel)
//			m_comboUsers.SetCurSel(nSel);			// 前移
//		else if (m_comboUsers.GetCount()>nSel-1)
//			m_comboUsers.SetCurSel(nSel-1);			// 后移
//		else if (m_comboUsers.GetCount() > 0)
//			m_comboUsers.SetCurSel(0);
//		OnCbnSelchangeComboUsers();
//	}
//}

void CDlgLogin::OnSkinSelect(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_SKIN_SETTING;
	if (nIndex == 0)
	{
		AFX_SWITCH_INSTANCED();
		CColorDialog dlg2(theApp.GetMainColor(), CC_FULLOPEN);
		if (dlg2.DoModal() != IDOK)
		{
			return;
		}
		theApp.SetMainColor(dlg2.GetColor());
	}else if (nIndex<=theColorSkinSize)
	{
		theApp.SetMainColor(theColorSkinsValue[nIndex-1]);
	}else
	{
		return;
	}
	SetCtrlColor();
	if (!m_bAuthLogLogonMode && theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='main-color'"),(DWORD)theApp.GetMainColor());
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->Invalidate();
}
void CDlgLogin::OnSkinGradient(void)
{
	const bool bColorGradient = theApp.GetColorGradient()?false:true;
	theApp.SetColorGradient(bColorGradient);
	SetCtrlColor();
	if (!m_bAuthLogLogonMode && theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='color-gradient'"),(int)(bColorGradient?1:0));
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->Invalidate();
}
void CDlgLogin::OnServerSetting(void)
{
	//CDlgSetting dlgSetting;
	//dlgSetting.DoModal();
	//// 需要重新登录；
	//if (m_sOrgServer!= dlgSetting.GetServer())
	//{
	//	CString sText;
	//	sText.Format(_T("修改服务器地址，需要重新加载验证流程才能生效，确定立即加载吗？"), m_sUserAccount);
	//	if (CDlgMessageBox::EbDoModal(this,"重新加载验证流程",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//	{
	//		m_bReload = true;
	//		this->OnOK();
	//	}
	//}
}
void CDlgLogin::OnBnClickedButtonSkin()
{
	if (m_menuSkin.GetSafeHmenu()==NULL)
	{
		m_menuSkin.CreatePopupMenu();
		for (int i=0; i<theColorSkinSize; i++)
		{
			m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_1+i,theColorSkinsString[i]);
		}
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_SETTING,_T("更多色调"));
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_GRADIENT,_T("渐变效果"));
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SERVER_SETTING,_T("连接设置"));
	}
	bool bFindedChecked = false;
	for (int i=0; i<theColorSkinSize; i++)
	{
		if (theApp.GetMainColor()==theColorSkinsValue[i])
		{
			m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_1+i,MF_CHECKED);
			bFindedChecked = true; // **这里不能break退出
		}else
		{
			m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_1+i,MF_UNCHECKED);
		}
	}
	m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_SETTING,bFindedChecked?MF_UNCHECKED:MF_CHECKED);
	m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_GRADIENT,theApp.GetColorGradient()?MF_CHECKED:MF_UNCHECKED);
	CPoint point;
	GetCursorPos(&point);
	m_menuSkin.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}

void CDlgLogin::OnStnClickedStaticAutologin()
{
	// 设置自启动
	const CString sAutoRunExeName = _T("entboost");
	HKEY hKey = NULL;
	LPCTSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE|KEY_READ, &hKey);
	if(lRet == ERROR_SUCCESS)
	{
		char pFileName[MAX_PATH] = {0};
		//GetModuleFileName(NULL, pFileName, MAX_PATH);
		//const std::string sModuleFileName(pFileName);
		const std::string sModuleFileName(m_sMainExe);
		DWORD dwDataSize = sModuleFileName.size();
		lRet = ::RegQueryValueEx(hKey,sAutoRunExeName,0,0,(BYTE*)pFileName,&dwDataSize);
		if(lRet != ERROR_SUCCESS || sModuleFileName != pFileName)
		{
			lRet = RegSetValueEx(hKey,sAutoRunExeName,0,REG_SZ,(BYTE *)sModuleFileName.c_str(),sModuleFileName.size());
		}
		RegCloseKey(hKey);
		if(lRet != ERROR_SUCCESS)
		{
			// ??
			CDlgMessageBox::EbDoModal(this,"设置失败，请重试！","",CDlgMessageBox::IMAGE_WARNING);
			return;
		}
	}
	UpdateData();
	if (m_labelAutoLogin.GetCheck() && !m_labelSavePwd.GetCheck())
	{
		m_labelSavePwd.SetCheck(TRUE);
		m_labelSavePwd.Invalidate();
	}
}

void CDlgLogin::OnBnClickedButtonForgetPwd()
{
#ifdef USES_EBCOM_TEST
	const unsigned long pForgetPwdUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL);
#else
	unsigned long pForgetPwdUrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,&pForgetPwdUrl);
#endif
	if (pForgetPwdUrl!=0)
	{
		const std::string sForgetPwdUrl((const char*)pForgetPwdUrl);
		ShellExecuteA(NULL, "open", sForgetPwdUrl.c_str(), NULL, NULL,SW_SHOW);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,pForgetPwdUrl);
#else
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,pForgetPwdUrl);
#endif
	}
}

void CDlgLogin::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CEbDialogBase::OnCancel();
}

LRESULT CDlgLogin::OnMessageServerChange(WPARAM wParam, LPARAM lParam)
{
	// 会话无效，在这里重新登录
#ifdef USES_EBCOM_TEST
	int nServerState = (int)wParam;
#else
	CEBCallbackInterface::SERVER_STATE nServerState = (CEBCallbackInterface::SERVER_STATE)wParam;
#endif
	if (nServerState == 3)	// CEBCallbackInterface::SERVER_RESTART)
	{
		m_bReload = true;
		this->OnOK();
		// **SDK内部会自动重新登录；
		//CDlgMessageBox::EbMessageBox(this,"更新成功：",_T("正在重新登录..."),CDlgMessageBox::IMAGE_WARNING,10);
		//SetTimer(TIMERID_RELOGIN,5000,NULL);
	}
	return 0;
}

BOOL CDlgLogin::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef USES_NEW_USER_CTRL
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_editUser.GetSafeHwnd())
	{
		if (!m_treeUsers.IsWindowVisible())
		{
			CRect rect;
			m_editUser.GetWindowRect(&rect);
			this->ScreenToClient(&rect);
			rect.top = rect.bottom;
			rect.bottom = rect.top + 112;
			m_treeUsers.MoveWindow(&rect);
			ShowTreeCtrl(true);
			m_editUser.SetSel(0,-1);
		}else
		{
			ShowTreeCtrl(false);
		}
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnDel.GetSafeHwnd())
	{
		m_editUser.SetFocus();
		DeleteItem(m_treeUsers.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONDOWN &&
		pMsg->hwnd != m_editUser.GetSafeHwnd() && pMsg->hwnd != m_treeUsers.GetSafeHwnd() && pMsg->hwnd!=m_btnDel.GetSafeHwnd())
	{
		ShowTreeCtrl(false);
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeUsers.GetSafeHwnd())
	{
		SelectItem(m_treeUsers.GetSelectedItem());
		return FALSE;
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_UP && pMsg->hwnd == m_treeUsers.GetSafeHwnd())
	{
		// 已经移到最上面
		if (m_treeUsers.GetCount()>0 && m_treeUsers.GetSelectedItem()!=NULL && m_treeUsers.GetPrevSiblingItem(m_treeUsers.GetSelectedItem())==NULL)
		//if (m_treeUsers.GetCount()>0 && m_treeUsers.GetSelectedItem()!=NULL && m_treeUsers.GetNextItem(m_treeUsers.GetSelectedItem(),TVGN_PREVIOUS))
		{
			m_editUser.SetFocus();
		}
	}else if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_editUser.GetSafeHwnd())
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			if (m_treeUsers.IsWindowVisible())
			{
				SelectItem(m_treeUsers.GetTrackItem());
				return FALSE;
			}
		}else if (pMsg->wParam == VK_TAB)
		{
			if (m_treeUsers.IsWindowVisible())
			{
				ShowTreeCtrl(false);
			}
		}else if (pMsg->wParam == VK_DOWN)
		{
			if (m_treeUsers.IsWindowVisible() && m_treeUsers.GetCount()>0)
			{
				m_treeUsers.SetFocus();
			}else if (m_treeUsers.GetCount()>0)
			{
				HTREEITEM hNextItem = m_treeUsers.GetNextSiblingItem(m_treeUsers.GetSelectedItem());
				if (hNextItem!=0)
				{
					m_treeUsers.Select(hNextItem, TVGN_CARET);
					return FALSE;
				}
			}
		}else if (pMsg->wParam == VK_UP)
		{
			if (m_treeUsers.GetCount()>0)
			{
				HTREEITEM hPrevItem = m_treeUsers.GetPrevSiblingItem(m_treeUsers.GetSelectedItem());
				if (hPrevItem!=0)
				{
					m_treeUsers.Select(hPrevItem, TVGN_CARET);
					return FALSE;
				}
			}
		}else if (pMsg->wParam==VK_DELETE || pMsg->wParam==VK_BACK)
		{
			m_editPassword.SetWindowText(_T(""));
		}else
		{
			m_bCanSearch = true;
		}
	}
#endif
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

void CDlgLogin::OnNMKillfocusTreeUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
//#ifdef USES_NEW_USER_CTRL
//	if (m_treeUsers.IsWindowVisible())
//		m_treeUsers.ShowWindow(SW_HIDE);
//#endif
}

void CDlgLogin::ShowTreeCtrl(bool bShow)
{
	if (bShow && !m_treeUsers.IsWindowVisible())
	{
		m_treeUsers.ShowWindow(SW_SHOW);

		m_editPassword.ShowWindow(SW_HIDE);
		m_labelSavePwd.ShowWindow(SW_HIDE);
		m_labelAutoLogin.ShowWindow(SW_HIDE);
	}else if (!bShow && m_treeUsers.IsWindowVisible())
	{
		m_editPassword.ShowWindow(SW_SHOW);
		m_labelSavePwd.ShowWindow(SW_SHOW);
		m_labelSavePwd.Invalidate();
		m_labelAutoLogin.ShowWindow(SW_SHOW);
		m_labelAutoLogin.Invalidate();

		m_treeUsers.ShowWindow(SW_HIDE);
	}
}

void CDlgLogin::DeleteItem(HTREEITEM hItem)
{
	if (hItem!=0)
	{
		CLoginInfo* pLoginInfo = (CLoginInfo*)m_treeUsers.GetItemData(hItem);
		if (pLoginInfo!=0)
		{
			const CString sUserAccount(pLoginInfo->m_sAccount);

			CString sUserDirectory;
			sUserDirectory.Format(_T("%s\\users\\%s"),theApp.GetAppPath(),sUserAccount);
			if (::PathFileExists(sUserDirectory))
			{
				CString sText;
				sText.Format(_T("[%s]帐号及所有聊天信息吗？"), sUserAccount);
				if (CDlgMessageBox::EbDoModal(this,"你确定删除本地！",sText,CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
					return;
				if (!DeleteDirectory(sUserDirectory))
				{
					CDlgMessageBox::EbDoModal(this,"操作失败，请检查后再试！","",CDlgMessageBox::IMAGE_ERROR);
					return;
				}
			}
			if (theApp.m_pBoEB->use("eb"))
			{
				if (!sUserAccount.IsEmpty())
				{
					CString sSql;
					sSql.Format(_T("delete from user_login_record_t where account='%s'"), sUserAccount);
					theApp.m_pBoEB->execsql(sSql);
				}
				theApp.m_pBoEB->close();
			}
			m_treeUsers.DeleteItem(hItem);
			m_pLoginInfoList.remove(sUserAccount);

			//const int nSel = m_comboUsers.GetCurSel()>=0?m_comboUsers.GetCurSel():m_comboUsers.FindString(-1,m_sUserAccount);
			//if (nSel >= 0)
			//{
			//	m_comboUsers.DeleteString(nSel);
			//	if (m_comboUsers.GetCount()>nSel)
			//		m_comboUsers.SetCurSel(nSel);			// 前移
			//	else if (m_comboUsers.GetCount()>nSel-1)
			//		m_comboUsers.SetCurSel(nSel-1);			// 后移
			//	else if (m_comboUsers.GetCount() > 0)
			//		m_comboUsers.SetCurSel(0);
			//	OnCbnSelchangeComboUsers();
			//}
		}
	}
}
void CDlgLogin::SelectItem(HTREEITEM hItem, bool bHideUserCtrl)
{
	if (bHideUserCtrl && m_treeUsers.IsWindowVisible())
	{
		ShowTreeCtrl(false);
	}
	if (hItem!=0)
	{
		CLoginInfo* pLoginInfo = (CLoginInfo*)m_treeUsers.GetItemData(hItem);
		if (pLoginInfo!=0)
		{
			if (!m_treeUsers.IsWindowVisible())
				m_editUser.SetFocus();
			int nSel = 0;
			if (m_bCanSearch)
			{
				CString sOldAccount;
				m_editUser.GetWindowText(sOldAccount);
				if (sOldAccount.GetLength()==1 && pLoginInfo->m_sAccount.Find(sOldAccount)==0)
					nSel = 1;
				else
				{
					int nStartChar = 0;
					int nEndChar = 0;
					m_editUser.GetSel(nStartChar,nEndChar);
					if (nStartChar>0 && sOldAccount.Left(nStartChar)==pLoginInfo->m_sAccount.Left(nStartChar))
					{
						nSel = nStartChar;
					}
				}
			}
			m_bCanSearch = false;
			m_editUser.SetWindowText(pLoginInfo->m_sAccount);
			m_editPassword.SetWindowText(pLoginInfo->m_sPassword);
			m_editUser.SetSel(nSel,-1);
		}
	}
}
void CDlgLogin::OnNMClickTreeUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
#ifdef USES_NEW_USER_CTRL
	ShowTreeCtrl(false);
	m_editUser.SetFocus();
	if (m_treeUsers.GetSelectedItem()==m_treeUsers.GetTrackItem())
		SelectItem(m_treeUsers.GetTrackItem());
	SetTimer(TIMERID_REFRESH_WIN,1,NULL);
#endif

}

LRESULT CDlgLogin::OnTreeItemSelChange(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree* pOwner = (VividTree*)lp;
	if (item!=NULL && pOwner == &m_treeUsers)
	{
		SelectItem(item,false);
	}
	return 0;
}

LRESULT CDlgLogin::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM hItem = (HTREEITEM)wp;
	VividTree * pOwner = (VividTree*)lp;
	if (pOwner == &m_treeUsers)
	{
		if (hItem == NULL)
		{
			m_btnDel.ShowWindow(SW_HIDE);
		}else
		{
			CRect rect;
			const int const_btn_width = m_btnDel.GetImgWidth();
			m_treeUsers.GetItemRect(hItem, &rect, TRUE);
			const int nTop = rect.top;
			m_treeUsers.GetClientRect(&rect);
			const int nRight = rect.right;
			m_btnDel.MovePoint(nRight-const_btn_width, nTop);
			m_btnDel.ShowWindow(SW_SHOW);
			m_btnDel.Invalidate();
			return 0;
		}
	}
	return 0;
}

void CDlgLogin::OnEnChangeEditUsers()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bCanSearch) return;
	CString sAccount;
	m_editUser.GetWindowText(sAccount);
	bool bFindAccount = false;
	if (!sAccount.IsEmpty())
	{
		CLockMap<CString, CLoginInfo::pointer>::iterator pIter = m_pLoginInfoList.begin();
		for (; pIter!=m_pLoginInfoList.end(); pIter++)
		{
			CLoginInfo::pointer pLoginInfo = pIter->second;
			if (pLoginInfo->m_sAccount.Find(sAccount)==0)
			{
				bFindAccount = true;
				if (m_treeUsers.GetSelectedItem()==pLoginInfo->m_hItem)
					SelectItem(pLoginInfo->m_hItem,false);
				else
					m_treeUsers.Select(pLoginInfo->m_hItem,TVGN_CARET);
				break;
			}
		}
	}
	if (!bFindAccount)
	{
		m_editPassword.SetWindowText(_T(""));
	}

}

void CDlgLogin::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_REFRESH_WIN==nIDEvent)
	{
		KillTimer(nIDEvent);
		theApp.InvalidateParentRect(&m_labelSavePwd);
		theApp.InvalidateParentRect(&m_labelAutoLogin);
	}

	CEbDialogBase::OnTimer(nIDEvent);
}
