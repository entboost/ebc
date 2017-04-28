#ifdef WIN32
#pragma warning(disable:4819 4267)
#endif

// eblogin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "eblogin.h"
#include "ebloginDlg.h"
#include "DlgLogin.h"
#include "DlgSetting.h"
#include "DlgMessageBox.h"
//using namespace entboost;
//#include <CGCLib/CGCLib.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#ifndef USES_EBCOM_TEST
#ifdef _DEBUG
#pragma comment(lib, "libebumd.lib")
#else
#pragma comment(lib, "libebum.lib")
#endif
#endif

// CebloginApp

BEGIN_MESSAGE_MAP(CebloginApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CString GetLineStateText(EB_USER_LINE_STATE nLineState)
{
	switch (nLineState)
	{
	case EB_LINE_STATE_ONLINE_OLD:
	case EB_LINE_STATE_ONLINE_NEW:
		return _T("在线");
	case EB_LINE_STATE_BUSY:
		return _T("忙碌");
	case EB_LINE_STATE_AWAY:
		return _T("离开");
	default:
		return _T("离线");
	}
}
// CebloginApp construction

CebloginApp::CebloginApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	CString str;
	GetModuleFileName( NULL, str.GetBuffer(MAX_PATH), MAX_PATH);
	str.ReleaseBuffer( -1);
	str = str.Mid(0, str.ReverseFind('\\'));
	m_sAppPath = str;
	m_sAppDataPath = m_sAppPath + _T("\\datas");
	m_sUserMainPath = m_sAppPath + _T("\\users");

	//m_imageEbIcon = NULL;
	m_imageEntLogo = NULL;
	m_nMainColor = RGB(0,0,0);
	m_nFlatBgColor = RGB(250,250,250);
	m_bColorFlat = true;
	m_bColorGradient = false;
	m_nEBServerVersion = 0;
	m_nOpenRegister = 0;
	m_bOpenVisitor = false;
	m_bLicenseUser = false;
	m_bSendRegMail = false;
	m_bEbDefaultEntLogo = false;
}


// The one and only CebloginApp object

CEbcLanguages theEbcLanguage;

CebloginApp theApp;
#ifdef USES_EBCOM_TEST
CComPtr<IEBParseSetting> theEBSetting;
CComPtr<IEBClientCore> theEBClientCore;
#else
CEBParseSetting theSetting;
CEBAppClient theEBAppClient;
#endif

// CebloginApp initialization

ULONG_PTR theGdiplusToken=0;
BOOL CebloginApp::InitInstance()
{
	setlocale(LC_ALL, "");	// 使用默认环境 .936中文目录有问题。
	::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	CNewMenu::SetDisableSelBorder(TRUE);
	CNewMenu::SetDisableShade(TRUE);
	CNewMenu::SetXpBlending(FALSE);
	CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);

#ifdef _DEBUG
	const tstring sLanguageFilePath = m_sAppPath + _T("\\languages\\zh-CN");
	theEbcLanguage.LoadLanguage(sLanguageFilePath.c_str());
#endif

	HWND hFromWnd = NULL;
	std::string sAppParameter0;
	std::string sAppParameter1;	// logout：注销
	std::string sMainExe;
	if (__argc >= 5)
	{
		hFromWnd = (HWND)atoi(__argv[1]);
		sAppParameter0 = __argv[2];
		sAppParameter1 = __argv[3];
		sMainExe = __argv[4];
	}
#ifdef _DEBUG
	const std::string sReqCode;
#else
	// 取出req=XXX
	std::string::size_type find = sAppParameter0.find("req=");
	if (find == std::string::npos)
	{
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);

		if (sMainExe.empty())
		{
			const CString sEBCExePath = GetAppPath()+_T("\\ebc.exe");
			if (::PathFileExists(sEBCExePath))
			{
				ShellExecute(NULL,  "open", sEBCExePath, "", GetAppPath(), SW_SHOW);
			}
		}
		return FALSE;
	}
	const std::string sReqCode = sAppParameter0.substr(find+4);
#endif;

	// 执行DLL注册命令
#ifndef _DEBUG
	CString sParameter;
#ifdef USES_EBCOM_TEST
	sParameter.Format(_T("\"%s\\ebcore.dll\" /s"), this->GetAppPath());
	ShellExecute(NULL,  "open", "regsvr32", sParameter, GetAppPath(), SW_HIDE);
#endif
#endif

#ifdef USES_EBCOM_TEST
	CoCreateInstance(__uuidof(EBClientCore),NULL,CLSCTX_INPROC_SERVER,__uuidof(IEBClientCore),(void**)&theEBClientCore);
	//HRESULT hr = CoCreateInstance(__uuidof(EBClientCore),NULL,CLSCTX_INPROC_SERVER,__uuidof(IEBClientCore),(void**)&theEBClientCore);
	//if (theEBClientCore==NULL)
	//{
	//	CString sError;
	//	sError.Format(_T("0x%x"),hr);
	//	AfxMessageBox(sError);
	//	return FALSE;
	//}
	theEBClientCore->QueryInterface(__uuidof(IEBParseSetting),(void**)&theEBSetting);
	if (theEBSetting==NULL) return FALSE;
	// 读取配置文件
	theEBSetting->Load((LPCTSTR)GetSettingFile());
	//const CEBString sVersion = theEBSetting->Version.GetBSTR();
#else
	// 读取配置文件
	theSetting.load(GetSettingFile());
	//const CEBString sVersion = theSetting.GetVersion();
#endif
	//LoadSetting();

	// 实现在登录口，保持单实例。
	HANDLE g_hMutex = CreateMutex(NULL,TRUE,_T("__entboost_login_mutex__"));
	if(ERROR_ALREADY_EXISTS==GetLastError())
	{
		//找到另一个实例
		CString stext;
#ifdef USES_EBCOM_TEST
		const CEBString sEnterprise = theEBSetting->Enterprise.GetBSTR();
		stext.Format(_T("%s-登录窗口"),sEnterprise.c_str());
#else
		stext.Format(_T("%s-登录窗口"),theSetting.GetEnterprise().c_str());
#endif
		HWND l_hWnd = FindWindow(NULL, stext);
		if (l_hWnd!=NULL)
		{
			if (TRUE == IsIconic(l_hWnd))
			{
				ShowWindow(l_hWnd,SW_SHOWNORMAL); //还原窗口
			}
			SetForegroundWindow(l_hWnd); //使窗口在最前方
		}else
		{
			// ?
		}
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		return FALSE;
	}

	//USES_CONVERSION;
	CString sImagePath = GetAppPath()+_T("\\img\\entlogo");	// 企业定制LOGO
	if (::PathFileExists(sImagePath))
	{
		SetEntLogo(sImagePath);
	}else
	{
		SetEntLogo(NULL);
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("entboost login"));

	const CString sBoPath = GetAppDataPath() + _T("\\bodb");
	m_pBoEB = bo::bodb_init(sBoPath);
	if (m_pBoEB.get() == NULL)
	{
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		CloseHandle(g_hMutex);
		CDlgMessageBox::EbDoModal(NULL,"","安装目录损坏：\r\n请重新安装！",CDlgMessageBox::IMAGE_ERROR,true);
		//AfxMessageBox(_T("安装目录损坏，请重新安装！"));
		return FALSE;
	}
	bool bError = false;
	try
	{
		m_pBoEB->setaccount("system","");
		if (!m_pBoEB->use("eb"))
		{
			// 数据库文件错误
			bError = true;
		}
	}
	catch(std::exception&)
	{
		bError = true;
		//AfxMessageBox(_T("catch 111"));
	}
	catch(...)
	{
		// 数据库文件错误
		bError = true;
		//AfxMessageBox(_T("catch 333"));
	}
	if (bError)
	{
		m_pBoEB->close();
		CopyFile(sBoPath+"\\eb\\eb.bdf.bk", sBoPath+"\\eb\\eb.bdf", FALSE);
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		CloseHandle(g_hMutex);
		//CDlgMessageBox::EbDoModal(NULL,_T("配置文件损坏，已经修复："),_T("请重新登录！"),CDlgMessageBox::IMAGE_INFORMATION);
		ShellExecute(NULL,  "open", sMainExe.c_str(), "", GetAppPath(), SW_SHOW);
		return FALSE;
	}else
	{
		CopyFile(sBoPath+"\\eb\\eb.bdf", sBoPath+"\\eb\\eb.bdf.bk", FALSE);
	}
	// ??? 1.20+版本，判断 user_login_record_t 增加 phone,user_id ，1.25以后版本删除；
	if (theApp.m_pBoEB->execsql("SELECT user_ida FROM user_login_record_t WHERE user_id=1")==-1)
	{
		// **该字段不存在，添加二个字段；
		theApp.m_pBoEB->execsql("ALTER TABLE user_login_record_t ADD user_id BIGINT DEFAULT 0");
		theApp.m_pBoEB->execsql("ALTER TABLE user_login_record_t ADD phone BIGINT DEFAULT 0");
	}

	//m_pBoEB->setaccount("system","");
	//if (!m_pBoEB->use("eb"))
	//{
	//	PostMessage(hFromWnd,WM_CLOSE,0,0);
	//	CloseHandle(g_hMutex);
	//	AfxMessageBox(_T("配置文件损坏，请重新安装！"));
	//	return FALSE;
	//}

	// 检查默认主色调
	bo::PRESULTSET pResltSet = NULL;
	m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='main-color'", &pResltSet);
	if (pResltSet != NULL && pResltSet->rscount>0)
	{
		m_nMainColor = pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal;
		if (m_nMainColor==RGB(86,190,245))	// 修改默认色
		{
			m_nMainColor = RGB(0,162,232);	// * 青绿
			CString sSql;
			sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='main-color'"),(DWORD)theApp.GetMainColor());
			theApp.m_pBoEB->execsql(sSql);
		}
	}else
	{
#ifdef USES_EBCOM_TEST
		m_nMainColor = theEBSetting->DefaultColor;
#else
		m_nMainColor = theSetting.GetDefaultColor();
#endif
		CString sSql;
		sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('main-color',%d)"),(DWORD)m_nMainColor);
		m_pBoEB->execsql(sSql);
	}
	bo::bodb_free(pResltSet);
	SetMainColor(m_nMainColor);
	// 检查扁平效果
	m_bColorFlat = true;
	m_bColorGradient = false;
	//pResltSet = NULL;
	//m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='color-flat'", &pResltSet);
	//if (pResltSet != NULL && pResltSet->rscount>0)
	//{
	//	m_bColorFlat = pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal==1?true:false;
	//}else
	//{
	//	m_bColorFlat = true;
	//	CString sSql;
	//	sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('color-flat',1)"));
	//	m_pBoEB->execsql(sSql);
	//}
	//bo::bodb_free(pResltSet);
	//// 检查渐变效果
	//pResltSet = NULL;
	//m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='color-gradient'", &pResltSet);
	//if (pResltSet != NULL && pResltSet->rscount>0)
	//{
	//	m_bColorGradient = pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal==1?true:false;
	//}else
	//{
	//	m_bColorGradient = false;
	//	CString sSql;
	//	sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('color-gradient',0)"));
	//	m_pBoEB->execsql(sSql);
	//}
	//bo::bodb_free(pResltSet);
	m_pBoEB->close();
	if (sAppParameter1 == "server_error")
	{
		// 服务器失败，重新设置后，重新连接；
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		CloseHandle(g_hMutex);
		CDlgSetting dlg2;
		if (dlg2.DoModal()==IDOK)
		{
			ShellExecute(NULL,  "open", sMainExe.c_str(), "", GetAppPath(), SW_SHOW);
		}
		return FALSE;
	}

	//CebloginDlg dlg;
	CDlgLogin pDlgLogin;
	pDlgLogin.m_sReqCode = sReqCode;
	pDlgLogin.m_sAppParameter1 = sAppParameter1;
	pDlgLogin.m_sMainExe = sMainExe;
	m_pMainWnd = &pDlgLogin;
	INT_PTR nResponse = pDlgLogin.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	CloseHandle(g_hMutex);
	if (pDlgLogin.m_bReload)
	{
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		ShellExecute(NULL,  "open", sMainExe.c_str(), "", GetAppPath(), SW_SHOW);
		return FALSE;
	}

	CString m_sLogonAccount;
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_IsLogoned == VARIANT_FALSE)
	{
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		return FALSE;
	}
	m_sLogonAccount = CEBString(theEBClientCore->EB_LogonAccount.GetBSTR()).c_str();
#else
	if (!theEBAppClient.EB_IsLogoned())
	{
		if (hFromWnd)
			PostMessage(hFromWnd,WM_CLOSE,0,0);
		return FALSE;
	}
	//tstring sTemp;
	//theEBAppClient.EB_GetLogonAccount(sTemp);
	//m_sLogonAccount = sTemp.c_str();
	m_sLogonAccount = theEBAppClient.EB_GetLogonAccount().c_str();
#endif
	if (pDlgLogin.m_labelAutoLogin.GetCheck() && !theApp.IsLogonVisitor())
	{
		// 普通用户，要求设置自动登录
		theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),m_sLogonAccount);
	}else if (!pDlgLogin.m_labelAutoLogin.GetCheck())
	{
		theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),_T(""));
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CebloginApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	//delete m_imageEbIcon;
	delete m_imageEntLogo;
	Gdiplus::GdiplusShutdown(theGdiplusToken);
	bo::bodb_exit(m_pBoEB);

	return CWinAppEx::ExitInstance();
}

CString CebloginApp::GetSettingFile(void) const
{
	return GetAppDataPath() + _T("\\setting");
}
CString CebloginApp::GetSettingIniFile(void) const
{
	return GetAppDataPath() + _T("\\setting.ini");
}
void CebloginApp::EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	::WritePrivateProfileString(lpszAppName,lpszKeyName,lpszValue,sIniSettingFile);
}
CString CebloginApp::EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	TCHAR lpszBuffer[1024];
	memset(lpszBuffer,0,sizeof(lpszBuffer));
	::GetPrivateProfileString(lpszAppName,lpszKeyName,lpszDefault,lpszBuffer,sizeof(lpszBuffer),sIniSettingFile);
	return lpszBuffer;
}
void CebloginApp::EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	CString sValue;
	sValue.Format(_T("%d"),nValue);
	::WritePrivateProfileString(lpszAppName,lpszKeyName,sValue,sIniSettingFile);
}
UINT CebloginApp::EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	return ::GetPrivateProfileInt(lpszAppName,lpszKeyName,nDefault,sIniSettingFile);
}

bool CebloginApp::IsLogonVisitor(void) const
{
#ifdef USES_EBCOM_TEST
	return theEBClientCore != NULL && theEBClientCore->EB_IsLogonVisitor==VARIANT_TRUE;
#else
	return theEBAppClient.EB_IsLogonVisitor();
#endif
}
void CebloginApp::FreeEntLogo(void)
{
	if (m_imageEntLogo)
	{
		delete m_imageEntLogo;
		m_imageEntLogo = NULL;
	}
}

void CebloginApp::SetEntLogo(LPCTSTR lpszFilePath)
{
	FreeEntLogo();
	USES_CONVERSION;
	if (lpszFilePath==NULL || strlen(lpszFilePath)==0)
	{
		m_bEbDefaultEntLogo = true;
		const CString sImagePath = GetAppPath()+_T("\\img\\entlogo.png");	// 恩布默认LOGO
		m_imageEntLogo = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath)); 
	}else
	{
		m_bEbDefaultEntLogo = false;
		m_imageEntLogo = new Gdiplus::Image((const WCHAR*)A2W_ACP(lpszFilePath)); 
	}

}
//void CebloginApp::LoadSetting(void)
//{
//	const CString sSettingFile = m_sAppDataPath + _T("\\setting");
//	theSetting.load((LPCTSTR)sSettingFile);
//}

BOOL CebloginApp::ImageFromIDResource(UINT nID, LPCTSTR sTR, Image * & pImg)
{
    HINSTANCE hInst = AfxGetResourceHandle();
    HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
    if (!hRsrc)
        return FALSE;
    // load resource into memory
    DWORD len = SizeofResource(hInst, hRsrc);
    BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
    if (!lpRsrc)
        return FALSE;
    // Allocate global memory on which to create stream
    HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
    BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
    memcpy(pmem,lpRsrc,len);
    IStream* pstm;
    CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
    // load from stream
    pImg=Gdiplus::Image::FromStream(pstm);
    // free/release stuff
    GlobalUnlock(m_hMem);
    pstm->Release();
    FreeResource(lpRsrc);
    return TRUE;
}

void CebloginApp::SetMainColor(COLORREF v)
{
	m_nMainColor = v;
	float h,s,l;
	::RGBtoHSL(GetRValue(m_nMainColor), GetGValue(m_nMainColor), GetBValue(m_nMainColor),&h,&s,&l);
	m_nFlatBgColor = theDefaultFlatBgColor;//HSLtoRGB(h, s, 0.99);	// 0.98
}

void CebloginApp::GetScreenSize(int & pWidth, int & pHeight)
{
	pWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	pHeight += GetSystemMetrics(SM_CYCAPTION);
}
//
//void CebloginApp::InvalidateParentRect(const CWnd* pWnd)
//{
//	CRect rect;
//	pWnd->GetWindowRect(&rect);
//	pWnd->GetParent()->ScreenToClient(&rect);
//	pWnd->GetParent()->InvalidateRect(&rect);
//}
void CebloginApp::InvalidateParentRect(const CWnd* pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetParent()->ScreenToClient(&rect);
	pWnd->GetParent()->InvalidateRect(&rect);
}
