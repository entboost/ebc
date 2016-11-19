// DlgOAuth.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgOAuth.h"

#define TIMER_ID_CHECK_EBLOGIN 1000
#define TIMER_ID_CLOSE			1001
#define OAUTH_WINDOW_TEXT _T("__entboost_oauth_window__")

#include "tlhelp32.h"
#pragma comment(lib, "shlwapi.lib")
bool CheckExistApp(const tstring& strExeName)
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
	if (hProcessSnap==NULL) return false;
	PROCESSENTRY32 pe32;  
	memset(&pe32,0,sizeof(pe32));
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	bool bExistApp = false;
	if(::Process32First(hProcessSnap,&pe32))  
	{  
		do  
		{ 
			if (strExeName.find(pe32.szExeFile) != std::string::npos)
			{
				bExistApp = true;
				break;
			}
		}  
		while(::Process32Next(hProcessSnap,&pe32));   
	}
	CloseHandle(hProcessSnap);
	return bExistApp;
}

// CDlgOAuth dialog

IMPLEMENT_DYNAMIC(CDlgOAuth, CDialog)

CDlgOAuth::CDlgOAuth(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOAuth::IDD, pParent)
	//, m_pCefBrowser(NULL)
{
	m_bLicenseUser = false;
	m_bSendRegMail = false;
	m_nSaveConversations = 3;
	m_bAuthContact = false;
	m_nDeployId = 0;
	//m_nGroupMsgSubId = 0;
	m_nAutoOpenSubId = 0;
	m_bAutoHideMainFrame = false;
	m_bHideMainFrame = false;
	m_bDisableUserCloudDrive = false;
	m_bDisableGroupSharedCloud = false;
	m_bDisableModifyPasswd = false;
	m_bDisableVideo = false;
	m_bDisableRemoteDesktop = false;
	m_bDisableAccountEdit = false;
	m_nDefaultBrowserType = EB_BROWSER_TYPE_CEF;
	m_bDisableMsgReceipt = false;

	m_nWaitCloseIndex = 0;
	m_bDocumentComplete = false;
	m_bIeException = false;
}

CDlgOAuth::~CDlgOAuth()
{
}

void CDlgOAuth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_pExplorer);
}


BEGIN_MESSAGE_MAP(CDlgOAuth, CDialog)

	ON_MESSAGE(EB_WM_OAUTH_FORWARD, OnMessageOAuthForward)
	ON_MESSAGE(EB_WM_LOGON_SUCCESS, OnMessageLogonSuccess)
	ON_MESSAGE(EB_WM_LOGON_TIMEOUT, OnMessageLogonTimeout)
	ON_MESSAGE(EB_WM_LOGON_ERROR, OnMessageLogonError)
	ON_MESSAGE(EB_WM_APPID_SUCCESS, OnMessageAppIdSuccess)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgOAuth, CDialog)
	//{{AFX_EVENTSINK_MAP(CDlgOAuth)
	ON_EVENT(CDlgOAuth,IDC_EXPLORER1,259,OnDocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CDlgOAuth message handlers
void CDlgOAuth::RunEBC(void)
{
	char pFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	ShellExecute(NULL, "open", pFileName, _T(""), theApp.GetAppPath(), SW_SHOW);
}

//#ifdef USES_EBCOM_TEST
//void CDlgOAuth::Fire_onOAuthForward(BSTR sOAuthUrl)
//{
//	// 打开恩布验证程序
//	char pFileName[MAX_PATH] = {0};
//	GetModuleFileName(NULL, pFileName, MAX_PATH);
//	CString sEBLoginExe = theApp.GetAppPath()+_T("\\eblogin.exe");
//	CString sParameter;
//	sParameter.Format(_T("%d \"%s\" \"%s\" \"%s\""), (int)this->GetSafeHwnd(),CEBString(sOAuthUrl).c_str(),m_sAppParameter0.c_str(),pFileName);	// m_sAppParameter0=="logout"，是注销
//	ShellExecute(NULL,  "open", sEBLoginExe, sParameter, theApp.GetAppPath(), SW_SHOW);
//	// 这里用浏览器实现
//	//ShellExecuteW(NULL, L"open", sOAuthUrl, NULL, NULL,SW_SHOW);
//}
//#else
//#endif
LRESULT CDlgOAuth::OnMessageOAuthForward(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	// 打开恩布验证程序
	const BSTR sOAuthUrl = (const BSTR)wParam;
	char pFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	CString sEBLoginExe = theApp.GetAppPath()+_T("\\eblogin.exe");
	CString sParameter;
	sParameter.Format(_T("%d \"%s\" \"%s\" \"%s\""), (int)this->GetSafeHwnd(),CEBString(sOAuthUrl).c_str(),m_sAppParameter0.c_str(),pFileName);	// m_sAppParameter0=="logout"，是注销
	ShellExecute(NULL,  "open", sEBLoginExe, sParameter, theApp.GetAppPath(), SW_SHOW);
#else
	const char* sOAuthUrl = (const char*)wParam;
	// 打开恩布验证程序
	char pFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	CString sEBLoginExe = theApp.GetAppPath()+_T("\\eblogin.exe");
	CString sParameter;
	sParameter.Format(_T("%d \"%s\" \"%s\" \"%s\""), (int)this->GetSafeHwnd(),sOAuthUrl,m_sAppParameter0.c_str(),pFileName);	// m_sAppParameter0=="logout"，是注销
	ShellExecute(NULL,  "open", sEBLoginExe, sParameter, theApp.GetAppPath(), SW_SHOW);
	// 这里用浏览器实现
#endif
	return 0;
}
void CDlgOAuth::OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	m_bDocumentComplete = true;
}
//#ifdef USES_EBCOM_TEST
//void CDlgOAuth::Fire_onLogonSuccess(IEB_AccountInfo* pAccountInfo)
//{
//	this->PostMessage(WM_CLOSE, 0, 0);
//	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	//CEbDialogBase::OnOK();
//}
//#else
//#endif
LRESULT CDlgOAuth::OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam)
{
	tstring sLogonHttpReqUrl;
#ifdef USES_EBCOM_TEST
	unsigned long pLogonHttpRequrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL);
	if (pLogonHttpRequrl!=NULL && strlen((const char*)pLogonHttpRequrl)>0)
	{
		sLogonHttpReqUrl = (const char*)pLogonHttpRequrl;
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL,pLogonHttpRequrl);
	}
#else
	unsigned long pLogonHttpRequrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL,&pLogonHttpRequrl);
	if (pLogonHttpRequrl != NULL && strlen((const char*)pLogonHttpRequrl)>0)
	{
		sLogonHttpReqUrl = (const char*)pLogonHttpRequrl;
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL,pLogonHttpRequrl);
	}
	// 清空后 libebum 会一直等待，直到 CPOPDlg::OnInitDialog() 重新设置 EB_SetMsgHwnd()，避免接收离线消息不能正常显示风险；
	theEBAppClient.EB_SetMsgHwnd(NULL);
#endif
	if (!sLogonHttpReqUrl.empty())
	{
		// ??? V1.22版本，1.23以后使用访问令牌，完美解决网络问题，可以删除该代码；
		// ***解决用户电脑使用各种VPN，浏览器代理工具，导致跳转应用错误问题；
		try
		{
			//CRect rect(0,0,0,0);
			//m_pCefBrowser.Create(sLogonHttpReqUrl.string(),rect,this->GetSafeHwnd(),0);
			m_pExplorer.Navigate(sLogonHttpReqUrl.c_str(),NULL,NULL,NULL,NULL);
			SetTimer(TIMER_ID_CLOSE,100,NULL);
		}catch(...)
		{
			m_bIeException = true;
			//AfxMessageBox(_T("ie exception"));
			SetTimer(TIMER_ID_CLOSE,10,NULL);
		}
	}else
	{
		this->PostMessage(WM_CLOSE, 0, 0);
	}

#ifdef USES_EBCOM_TEST
	unsigned long pEntManagerUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL);
#else
	unsigned long pEntManagerUrl = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL, &pEntManagerUrl);
#endif
	if (pEntManagerUrl != NULL && strlen((const char*)pEntManagerUrl)>0)
	{
		m_sEntManagerUrl = (const char*)pEntManagerUrl;
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#else
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#endif
	}

	//CDialog::OnOK();
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgOAuth::Fire_onLogonTimeout(IEB_AccountInfo * pAccountInfo)
//{
//	//this->ShowWindow(SW_SHOW);
//	CDlgMessageBox::EbDoModal(this,_T("登录超时！"),_T("请重新登录。"),CDlgMessageBox::IMAGE_ERROR);
//	this->PostMessage(WM_CLOSE, 0, 0);
//	RunEBC();
//}
//#else
//#endif
LRESULT CDlgOAuth::OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam)
{
	//this->ShowWindow(SW_SHOW);
	CDlgMessageBox::EbDoModal(this,_T(""),_T("登录超时：\r\n请重新登录！"),CDlgMessageBox::IMAGE_ERROR,true,8);
	this->PostMessage(WM_CLOSE, 0, 0);
	RunEBC();
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgOAuth::Fire_onLogonError(IEB_AccountInfo * pAccountInfo, EB_STATE_CODE nState)
//{
//	//this->ShowWindow(SW_SHOW);
//	if (nState == EB_STATE_UNAUTH_ERROR)
//		CDlgMessageBox::EbDoModal(this,_T("该帐号邮箱未通过验证："),_T("请查收邮件，点击验证链接，完成注册！"),CDlgMessageBox::IMAGE_ERROR);
//	else
//		CDlgMessageBox::EbDoModal(this,_T("登录失败！"),_T("请重新登录。"),CDlgMessageBox::IMAGE_ERROR);
//	this->PostMessage(WM_CLOSE, 0, 0);
//	RunEBC();
//}
//#else
//#endif
LRESULT CDlgOAuth::OnMessageLogonError(WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = lParam;
	//this->ShowWindow(SW_SHOW);
	if (nErrorCode == EB_STATE_UNAUTH_ERROR)
		CDlgMessageBox::EbDoModal(this,_T(""),_T("帐号未通过验证：\r\n请查收邮件，点击验证链接，完成注册！"),CDlgMessageBox::IMAGE_ERROR,true);
	else
	{
		CString sText;
		sText.Format(_T("错误代码：%d\r\n请重新登录！"),nErrorCode);
		CDlgMessageBox::EbDoModal(this,"登录失败",sText,CDlgMessageBox::IMAGE_ERROR,true);
	}
	this->PostMessage(WM_CLOSE, 0, 0);
	RunEBC();
	return 0;
}

//#ifdef USES_EBCOM_TEST
//void CDlgOAuth::Fire_onAppIdSuccess(eb::bigint sAppId, const CEBString& sAppOnlineKey)
//{
//	unsigned long pAccountPrefix = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
//	if (pAccountPrefix!=NULL && strlen((const char*)pAccountPrefix)>0)
//	{
//		m_sAccountPrefix = (const char*)pAccountPrefix;
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pAccountPrefix);
//	}
//	unsigned long pProductName = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
//	if (pProductName!=NULL && strlen((const char*)pProductName)>0)
//	{
//		m_sProductName = (const char*)pProductName;
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
//	}
//	const unsigned long nSendRegMail = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL);
//	m_bSendRegMail = nSendRegMail==1?true:false;
//	const unsigned long nSaveConversatios = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS);
//	m_nSaveConversations = nSaveConversatios;
//	const unsigned long nAuthContact = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT);
//	m_bAuthContact = nAuthContact==1?true:false;
//
//	//unsigned long pGroupMsgSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID);
//	//if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
//	//{
//	//	m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
//	//	theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
//	//}
//	unsigned long pAutoOpenSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID);
//	if (pAutoOpenSubId!=NULL && strlen((const char*)pAutoOpenSubId)>0)
//	{
//		m_nAutoOpenSubId = eb_atoi64((const char*)pAutoOpenSubId);
//		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,pAutoOpenSubId);
//	}
//	const unsigned long nAutoHideMainFrame = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME);
//	m_bAutoHideMainFrame = nAutoHideMainFrame==1?true:false;
//	const unsigned long nHideMainFrame = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_HIDE_MAINFRAME);
//	m_bHideMainFrame = nHideMainFrame==1?true:false;
//
//}
//#else
//#endif
LRESULT CDlgOAuth::OnMessageAppIdSuccess(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	unsigned long pAccountPrefix = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
	if (pAccountPrefix!=NULL && strlen((const char*)pAccountPrefix)>0)
	{
		m_sAccountPrefix = (const char*)pAccountPrefix;
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pAccountPrefix);
	}
	const unsigned long nLicenseUser = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER);
	m_bLicenseUser = nLicenseUser==0?false:true;
	if (m_bLicenseUser)
	{
		unsigned long pProductName = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME);
		if (pProductName!=NULL && strlen((const char*)pProductName)>0)
		{
			m_sProductName = (const char*)pProductName;
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
		}
	}
	const unsigned long nSendRegMail = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL);
	m_bSendRegMail = nSendRegMail==1?true:false;
	const unsigned long nSaveConversatios = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS);
	m_nSaveConversations = nSaveConversatios;
	const unsigned long nAuthContact = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT);
	m_bAuthContact = nAuthContact==1?true:false;

	unsigned long pDeployId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID);
	if (pDeployId!=NULL && strlen((const char*)pDeployId)>0)
	{
		m_nDeployId = eb_atoi64((const char*)pDeployId);
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
	}
	//unsigned long pGroupMsgSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID);
	//if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
	//{
	//	m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
	//	theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
	//}
	unsigned long pAutoOpenSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID);
	if (pAutoOpenSubId!=NULL && strlen((const char*)pAutoOpenSubId)>0)
	{
		m_nAutoOpenSubId = eb_atoi64((const char*)pAutoOpenSubId);
		theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,pAutoOpenSubId);
	}
	const unsigned long nAutoHideMainFrame = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME);
	m_bAutoHideMainFrame = nAutoHideMainFrame==1?true:false;
	const unsigned long nHideMainFrame = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_HIDE_MAINFRAME);
	m_bHideMainFrame = nHideMainFrame==1?true:false;
	const unsigned long nDisableUserCloudDrive = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE);
	m_bDisableUserCloudDrive = nDisableUserCloudDrive==1?true:false;
	const unsigned long nDisableGroupSharedCloud = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD);
	m_bDisableGroupSharedCloud = nDisableGroupSharedCloud==1?true:false;
	const unsigned long nDisableModifyPasswd = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD);
	m_bDisableModifyPasswd = nDisableModifyPasswd==1?true:false;
	const unsigned long nDisableVideo = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_VIDEO);
	m_bDisableVideo = nDisableVideo==1?true:false;
	const unsigned long nDisableRD = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP);
	m_bDisableRemoteDesktop = nDisableRD==1?true:false;
	const unsigned long nDisableAccountEdit = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT);
	m_bDisableAccountEdit = nDisableAccountEdit==1?true:false;
	const unsigned long nDefaultBrowserType = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE);
	m_nDefaultBrowserType = nDefaultBrowserType==1?EB_BROWSER_TYPE_IE:EB_BROWSER_TYPE_CEF;
#else
	unsigned long pAccountPrefix = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,&pAccountPrefix);
	if (pAccountPrefix != NULL && strlen((const char*)pAccountPrefix)>0)
	{
		m_sAccountPrefix = (const char*)pAccountPrefix;
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
	}
	unsigned long nLicenseUser = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER,&nLicenseUser);
	m_bLicenseUser = nLicenseUser==0?false:true;
	if (m_bLicenseUser)
	{
		unsigned long pProductName = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,&pProductName);
		if (pProductName!=NULL && strlen((const char*)pProductName)>0)
		{
			m_sProductName = (const char*)pProductName;
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
		}
	}
	unsigned long nSendRegMail = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
	m_bSendRegMail = nSendRegMail==1?true:false;
	unsigned long nSaveConversatios = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS,&nSaveConversatios);
	m_nSaveConversations = nSaveConversatios;
	unsigned long nAuthContact = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT,&nAuthContact);
	m_bAuthContact = nAuthContact==1?true:false;

	unsigned long pDeployId = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,&pDeployId);
	if (pDeployId!=NULL && strlen((const char*)pDeployId)>0)
	{
		m_nDeployId = eb_atoi64((const char*)pDeployId);
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
	}
	//unsigned long pGroupMsgSubId = 0;
	//theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
	//if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
	//{
	//	m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
	//	theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
	//}
	unsigned long pAutoOpenSubId = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,&pAutoOpenSubId);
	if (pAutoOpenSubId!=NULL && strlen((const char*)pAutoOpenSubId)>0)
	{
		m_nAutoOpenSubId = eb_atoi64((const char*)pAutoOpenSubId);
		theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,pAutoOpenSubId);
	}
	unsigned long nAutoHideMainFrame = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME,&nAutoHideMainFrame);
	m_bAutoHideMainFrame = nAutoHideMainFrame==1?true:false;
	unsigned long nHideMainFrame = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_HIDE_MAINFRAME,&nHideMainFrame);
	m_bHideMainFrame = nHideMainFrame==1?true:false;
	unsigned long nDisableUserCloudDrive = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE,&nDisableUserCloudDrive);
	m_bDisableUserCloudDrive = nDisableUserCloudDrive==1?true:false;
	unsigned long nDisableGroupSharedCloud = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD,&nDisableGroupSharedCloud);
	m_bDisableGroupSharedCloud = nDisableGroupSharedCloud==1?true:false;
	unsigned long nDisableModifyPasswd = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD,&nDisableModifyPasswd);
	m_bDisableModifyPasswd = nDisableModifyPasswd==1?true:false;
	unsigned long nDisableVideo = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_VIDEO,&nDisableVideo);
	m_bDisableVideo = nDisableVideo==1?true:false;
	unsigned long nDisableRD = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP,&nDisableRD);
	m_bDisableRemoteDesktop = nDisableRD==1?true:false;
	unsigned long nDisableAccountEdit = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT,&nDisableAccountEdit);
	m_bDisableAccountEdit = nDisableAccountEdit==1?true:false;
	unsigned long nDefaultBrowserType = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE,&nDefaultBrowserType);
	m_nDefaultBrowserType = nDefaultBrowserType==1?EB_BROWSER_TYPE_IE:EB_BROWSER_TYPE_CEF;
	unsigned long nDisableMsgReceipt = 0;
	theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MSG_RECEIPT,&nDisableMsgReceipt);
	m_bDisableMsgReceipt = nDisableMsgReceipt==1?true:false;

#endif
	return 0;
}

bool CDlgOAuth::StartClient(void)
{
	//AfxMessageBox(sAddress.c_str());
#ifdef USES_EBCOM_TEST
	const CEBString sAddress = theEBSetting->ServerAddress.GetBSTR();
	//const CEBString sAddress = theEBClientCore->GetAccountAddress(lpszAccount).GetBSTR();
	theEBClientCore->EB_Init(sAddress.c_str(),"");
	return theEBClientCore->EB_IsInited==VARIANT_TRUE?true:false;
#else
	//const CEBString sAddress("192.168.1.100:18012;test-lc.entboost.com:18012");
	const CEBString sAddress(theSetting.GetServerAddress());
	//tstring sAddress;
	//entboost::GetAccountAddress(lpszAccount, sAddress);
	//CDlgMessageBox::EbDoModal(this,_T("服务器地址！"),sAddress.c_str(),CDlgMessageBox::IMAGE_INFORMATION);
	//theEBAppClient.Stop();
	theEBAppClient.EB_Init(sAddress.c_str());
	theEBAppClient.EB_SetMsgHwnd(this->GetSafeHwnd());
	return theEBAppClient.EB_IsInited();
#endif
}
void CDlgOAuth::ExecuteEBLogonServerError(void)
{
	char pFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	const CString sEBLoginExe = theApp.GetAppPath()+_T("\\eblogin.exe");
	CString sParameter;
	sParameter.Format(_T("0 \"req=0\" \"server_error\" \"%s\""), pFileName);	// m_sAppParameter0=="server_error"，需要重新设置
	ShellExecute(NULL,  "open", sEBLoginExe, sParameter, theApp.GetAppPath(), SW_SHOW);
}

BOOL CDlgOAuth::OnInitDialog()
{
	CDialog::OnInitDialog();
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW,SWP_FRAMECHANGED);	// 隐藏工具栏
	// 隐藏窗口（无标题窗口有效）
	WINDOWPLACEMENT wp;
	wp.length=sizeof(WINDOWPLACEMENT);
	wp.flags=WPF_RESTORETOMAXIMIZED;
	wp.showCmd=SW_HIDE;
	SetWindowPlacement(&wp);

	if (!StartClient())
	{
		CDlgMessageBox::EbDoModal(this,_T(""),_T("本地程序启动失败：\r\n请检查本地网络后再试。"),CDlgMessageBox::IMAGE_ERROR,true,8);
		this->PostMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}
	this->SetWindowText(OAUTH_WINDOW_TEXT);
	// *** 用户登录自定义数据，企业有需要，可以填写自己数据，格式没有限制，最长256字节
	const tstring sUserData;
	theEBAppClient.EB_SetSystemParameter(EB_SYSTEM_PARAMETER_LOGON_USER_DATA,(unsigned long)sUserData.c_str());

	const eb::bigint theDevAppId = 278573612908LL;
	const CEBString theDevAppKey = "ec1b9c69094db40d9ada80d657e08cc6";
#ifdef USES_EBCOM_TEST
	(((CEBDispatch*)(CEBCoreEventsSink*)this))->AddRef();	// 增加计数，避免删除自己
	((CEBCoreEventsSink*)this)->m_hFireWnd = this->GetSafeHwnd();
	this->DispEventAdvise(theEBClientCore);
	bool ret = theEBClientCore->EB_SetDevAppId(theDevAppId,theDevAppKey.c_str(),VARIANT_TRUE)==S_OK;
#else
	bool ret = theEBAppClient.EB_SetDevAppId(theDevAppId,theDevAppKey.c_str(),true)>=0;
#endif
	const CString theAutoRunAccount = theApp.EBC_GetProfileString(_T("system"),_T("auto-run-account"));
	if (!ret)
	{
		// 本地服务有问题，检查是否自动登录，
		// 自动登录，继续等待
		// 手工登录，提示错误
		if (theAutoRunAccount.IsEmpty())
		{
			CDlgMessageBox::EbDoModal(this,_T(""),_T("连接服务器失败：\r\n请重新登录；如果出现同样错误提示，请检查本地网络后再试。"),CDlgMessageBox::IMAGE_ERROR,true,8);
			ExecuteEBLogonServerError();
			this->PostMessage(WM_CLOSE, 0, 0);
			return FALSE;
		}else
		{
			// 自动登录，一直等待
			while (true)
			{
#ifdef USES_EBCOM_TEST
				bool ret = theEBClientCore->EB_SetDevAppId(theDevAppId,theDevAppKey.c_str(),VARIANT_TRUE)==S_OK;
#else
				bool ret = theEBAppClient.EB_SetDevAppId(theDevAppId,theDevAppKey.c_str(),true)>=0;
#endif
				if (ret)
					break;
				Sleep(3000);
				static int theTimeout = 0;
				if ((theTimeout++) == 60)	// 60*3=180；3分钟
					return FALSE;
			}
		}
	}

	for (int i=0; i<30; i++)
	{
#ifdef USES_EBCOM_TEST
		const CEBString sAppOnlineKey = theEBClientCore->EB_GetDevAppOnlineKey().GetBSTR();
#else
		const CEBString sAppOnlineKey = theEBAppClient.EB_GetDevAppOnlineKey();
#endif
		if (!sAppOnlineKey.empty())
		{
			break;
		}
		Sleep(600);
		if (i+1==20)
		{
			static int theTimeout = 0;
			// 18*10=180；3分钟
			if ((++theTimeout) == 18 || theAutoRunAccount.IsEmpty())
			{
				// 网络有问题，提示失败。
				CDlgMessageBox::EbDoModal(this,_T(""),_T("服务器连接失败：\r\n请重新登录；如果出现同样错误提示，请检查本地网络后再试。"),CDlgMessageBox::IMAGE_ERROR,true,8);
				ExecuteEBLogonServerError();
				this->PostMessage(WM_CLOSE, 0, 0);
				return FALSE;
			}else
			{
				// 自动登录，重设，继续等待；
				i=0;
			}
		}
	}

	SetTimer(TIMER_ID_CHECK_EBLOGIN,30*1000,NULL);
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LogonOAuth("", EB_LINE_STATE_ONLINE_NEW);
#else
	theEBAppClient.EB_LogonOAuth("");
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOAuth::OnDestroy()
{
	//m_pCefBrowser.Destroy();

#ifdef USES_EBCOM_TEST
	this->DispEventUnadvise(theEBClientCore);
#else
	theEBAppClient.EB_SetMsgHwnd(NULL);
#endif

	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CDlgOAuth::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_ID_CHECK_EBLOGIN==nIDEvent)
	{
		if (!CheckExistApp("eblogin.exe"))
		{
			KillTimer(nIDEvent);
			this->PostMessage(WM_CLOSE, 0, 0);
			return;
		}
	}else if (TIMER_ID_CLOSE==nIDEvent)
	{
		if (m_bDocumentComplete || m_bIeException || (m_nWaitCloseIndex++)>15)
		{
			KillTimer(nIDEvent);
			this->PostMessage(WM_CLOSE, 0, 0);
			return;
		}
	}

	CDialog::OnTimer(nIDEvent);
}
