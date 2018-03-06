#include "StdAfx.h"
#include "EBUIApp.h"
#include "DlgMainFrame.h"
#include "DlgOAuth.h"
#include "DlgGroupInfo.h"
#include "DlgMemberInfo.h"
#include "DlgFuncWindow.h"

#ifdef _USRDLL
#ifdef _DEBUG
#pragma comment(lib, "ustrmbas9d.lib")
#pragma comment(lib, "libebumsd.lib")
#else
#pragma comment(lib, "USTRMBASE9.lib")
#pragma comment(lib, "libebums.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "ustrmbas9d.lib")
#pragma comment(lib, "libebumd.lib")
#else
#pragma comment(lib, "USTRMBASE9.lib")
#pragma comment(lib, "libebum.lib")
#endif
#endif

mycp::bigint EbGetFileSize(const char* sFilePath)
{
	FILE * f = fopen(sFilePath, "rb");
	if (f == NULL)
	{
		return -1;
	}
#ifdef WIN32
	_fseeki64(f, 0, SEEK_END);
	const mycp::bigint nFileSize = _ftelli64(f);
#else
	fseeko(f, 0, SEEK_END);
	const mycp::bigint nFileSize = ftello(f);
#endif
	fclose(f);
	return nFileSize;
}
CString GetLineStateText(EB_USER_LINE_STATE nLineState)
{
	switch (nLineState)
	{
	//case EB_LINE_STATE_ONLINE_OLD:	// ???
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
CString GetGroupTypeText(EB_GROUP_TYPE nGroupType, bool bShortName)
{
	switch (nGroupType)
	{
	case EB_GROUP_TYPE_DEPARTMENT:
		return _T("部门");
	case EB_GROUP_TYPE_PROJECT:
		return bShortName?_T("项目"):_T("项目组");
	case EB_GROUP_TYPE_GROUP:
		return bShortName?_T("群组"):_T("个人群组");
	default:
		return _T("讨论组");
	}
}
// ** 实现win7 home环境下，能实现文件拖拉事件；
#define WM_COPYGLOBALDATA 0x0049
typedef WINUSERAPI BOOL WINAPI CHANGEWINDOWMESSAGEFILTER(UINT message, DWORD dwFlag);
void ChangeMessageProi()
{
	HINSTANCE hDllInst = LoadLibrary(_T("user32.dll"));
	if (hDllInst)
	{
		CHANGEWINDOWMESSAGEFILTER *pAddMessageFilterFunc = (CHANGEWINDOWMESSAGEFILTER *)GetProcAddress(hDllInst, "ChangeWindowMessageFilter");
		if (pAddMessageFilterFunc)
		{
			pAddMessageFilterFunc(WM_DROPFILES, MSGFLT_ADD);
			pAddMessageFilterFunc(WM_COPYDATA, MSGFLT_ADD);
			pAddMessageFilterFunc(0x0049, MSGFLT_ADD);
		}
		FreeLibrary(hDllInst);
	}
}

CEBUIApp theApp;
CEBParseSetting theSetting;
CEBAppClient theEBAppClient;

CEBUIApp::CEBUIApp(void)
: m_pMainWnd(NULL)
, m_pHandler(NULL)
#ifndef _USRDLL
, m_hInstance(NULL)
#endif

{
	CString str;
	GetModuleFileName( NULL, str.GetBuffer(MAX_PATH), MAX_PATH);
	str.ReleaseBuffer( -1);
	const int nFind = str.ReverseFind('\\');
	m_sAppPath = str.Mid(0, nFind);
	m_sAppDataPath = m_sAppPath + _T("\\datas");
	m_sAppName = str.Mid(nFind+1);

	m_nLogonUserId = 0;
	m_bLicenseUser = false;
	m_nOpenRegister = 0;
	m_bOpenVisitor = false;
	m_bSendRegMail = false;
	m_bSaveConversationLocal = true;
	m_bSaveConversationServer = true;
	m_bAuthContact = false;
	m_nDeployId = 0;
	m_nGroupMsgSubId = 0;
	m_nFindAppSubId = 0;
	m_nAutoOpenSubId = 0;
	m_pAutoOpenFuncWindow = NULL;
	m_bAutoHideMainFrame = false;
	m_bHideMainFrame = false;
	m_bDisableUserCloudDrive = false;
	m_bDisableGroupSharedCloud = false;
	m_bDisableModifyPasswd = false;
	m_bDisableVideo = false;
	m_bDisableRemoteDesktop = false;

	//m_imageEbIcon = NULL;
	//m_imageEntLogo = NULL;
	m_image1Group = NULL;
	m_image1People = NULL;
	m_image1Cloud = NULL;
	m_image2Cloud = NULL;
	m_imageDefaultVisitor = NULL;
	m_imageDefaultOrg = NULL;
	m_imageDefaultContact = NULL;
	m_imageDefaultMember = NULL;
	m_imageDefaultDepartment = NULL;
	m_imageDefaultProject = NULL;
	m_imageDefaultGroup = NULL;
	m_imageDefaultTempGroup = NULL;
	m_imageStateAway = NULL;
	m_imageStateBusy = NULL;

	m_pDlgEmotionSelect = NULL;
	//m_iconAway = NULL;
	//m_iconBusy = NULL;
	//m_iconEnterprise = NULL;
	//m_iconDepartment = NULL;
	//m_iconContact = NULL;

	//m_nTreeListSelColor = RGB();
	//m_nTreeListHotColor = RGB();
	//m_nTreeListBorderColor = RGB();

	m_nMainColor = 0;
	m_bColorGradient = false;
	m_nFuncColor = 0;
	m_nHotColor1 = 0;
	m_nHotColor2 = 0;
	m_nHotColor = 0;
	m_nNormalColor = 0;
	m_nBgTitle0 = 0;
	m_nBgColor1 = 0;
	m_nBgColor2 = 0;
	m_nBgColor3 = 0;
	m_nOkColor0 = 0;
	m_nOkColor1 = 0;

	m_nSelectCallId = 0;
	m_nSelectUserId = 0;
	m_nSelectGroupId = 0;

	m_bAdaptIeVersion = false;

	m_nSendType = -1;
	m_bTopMost = false;
}

CEBUIApp::~CEBUIApp(void)
{
	IM_Logout();
}

BOOL IsWow64(void)
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle("kernel32"),"IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
	}
	return bIsWow64;
} 
void CEBUIApp::AdaptIeVersion(void)
{
	//32 bit:
	//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	//64 bit:
	//HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION

	// HKEY_LOCAL_MACHINE\Software\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	// HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	//
	//10001 (0x2711) 	Internet Explorer 10。网页以IE 10的标准模式展现，页面!DOCTYPE无效
	//10000 (0x02710) 	Internet Explorer 10。在IE 10标准模式中按照网页上!DOCTYPE指令来显示网页。Internet Explorer 10 默认值。
	//9999 (0x270F) 	Windows Internet Explorer 9. 强制IE9显示，忽略!DOCTYPE指令
	//9000 (0x2328) 	Internet Explorer 9. Internet Explorer 9默认值，在IE9标准模式中按照网页上!DOCTYPE指令来显示网页。
	//8888 (0x22B8) 	Internet Explorer 8，强制IE8标准模式显示，忽略!DOCTYPE指令
	//8000 (0x1F40) 	Internet Explorer 8默认设置，在IE8标准模式中按照网页上!DOCTYPE指令展示网页
	//7000 (0x1B58) 	使用WebBrowser Control控件的应用程序所使用的默认值，在IE7标准模式中按照网页上!DOCTYPE指令来展示网页。

	if (m_bAdaptIeVersion)
		return;
	const BOOL bIsWow64 = IsWow64();

	CString sSubKey = _T("SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	if (bIsWow64)
		sSubKey = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");

	TCHAR szVersion[64]	= {0};   
	DWORD dwSize = 64;   
	HKEY hKey = NULL;  

	// 先获取IE版本
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer"), 0, KEY_READ, &hKey))
	{
		if (ERROR_SUCCESS != RegQueryValueEx(hKey, TEXT("svcUpdateVersion"), 0, NULL, (LPBYTE)szVersion, &dwSize))
		{
			RegQueryValueEx(hKey, TEXT("Version"), 0, NULL, (LPBYTE)szVersion, &dwSize);
		}
		RegCloseKey(hKey);
	}

	TCHAR *pos = _tcschr(szVersion, TEXT('.'));
	if (NULL == pos)
	{
		return;
		//if (!bIsWow64)
		//{
		//	return;
		//}
		//if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer"), 0, KEY_READ, &hKey))
		//{
		//	RegQueryValueEx(hKey, TEXT("Version"), 0, NULL, (LPBYTE)szVersion, &dwSize);
		//	RegCloseKey(hKey);
		//}
		//pos = _tcschr(szVersion, TEXT('.'));
		//if (NULL == pos)
		//{
		//	return;
		//}
	}
	*pos = 0;

	const DWORD dwIeKenel = _ttoi(szVersion) * 1000;

	// 设置应用程序使用的IE版本
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,sSubKey,0,KEY_WRITE|KEY_READ,&hKey))
	{
		dwSize = sizeof(dwIeKenel);
		const CString sAppName = m_sAppName;
		DWORD nOldIeKenel = 0;
		DWORD nType = REG_DWORD;
		RegQueryValueEx(hKey,sAppName,0, &nType, (LPBYTE)&nOldIeKenel,&dwSize);

		if (nOldIeKenel!=dwIeKenel)
		{
			dwSize = sizeof(dwIeKenel);
			if (ERROR_SUCCESS==RegSetValueEx(hKey, sAppName, 0, REG_DWORD, (LPBYTE)&dwIeKenel, dwSize))
				m_bAdaptIeVersion = true;
		}else
		{
			m_bAdaptIeVersion = true;
		}
		RegCloseKey(hKey);
	}
}

#ifdef _USRDLL
BEGIN_MESSAGE_MAP(CEBUIApp, CWinApp)
END_MESSAGE_MAP()
#endif

CEBUISwitchInstance::CEBUISwitchInstance(void)
{
//#ifndef _USRDLL
	if (theApp.m_hInstance==NULL)
	{
		CString strModulePath;
#ifdef _USRDLL
#ifdef _DEBUG
		strModulePath.Format(_T("%s\\libebuisd.dll"),theApp.GetAppPath());
#else
		strModulePath.Format(_T("%s\\libebuis.dll"),theApp.GetAppPath());
#endif
#else
#ifdef _DEBUG
		strModulePath.Format(_T("%s\\libebuid.dll"),theApp.GetAppPath());
#else
		strModulePath.Format(_T("%s\\libebui.dll"),theApp.GetAppPath());
#endif
#endif
		if (!::PathFileExists(strModulePath))
		{
#ifdef _USRDLL
#ifdef _DEBUG
			strModulePath = _T("libebuids.dll");
#else
			strModulePath = _T("libebuis.dll");
#endif
#else
#ifdef _DEBUG
			strModulePath = _T("libebuid.dll");
#else
			strModulePath = _T("libebui.dll");
#endif
#endif
		}
		theApp.m_hInstance = GetModuleHandle(strModulePath);
	}

	m_hOldResourceHandle = ::AfxGetResourceHandle();
	::AfxSetResourceHandle(theApp.m_hInstance);
	//m_hOldInstanceHandle = AfxGetModuleState()->m_hCurrentInstanceHandle;
	//m_hOldResourceHandle = AfxGetModuleState()->m_hCurrentResourceHandle;
	//AfxGetModuleState()->m_hCurrentInstanceHandle = theApp.m_hInstance;
	//AfxGetModuleState()->m_hCurrentResourceHandle = theApp.m_hInstance;
//#endif
}
CEBUISwitchInstance::~CEBUISwitchInstance(void)
{
//#ifndef _USRDLL
	::AfxSetResourceHandle(m_hOldResourceHandle);
	//AfxGetModuleState()->m_hCurrentInstanceHandle = m_hOldInstanceHandle;
	//AfxGetModuleState()->m_hCurrentResourceHandle = m_hOldResourceHandle;
//#endif
}

ULONG_PTR theGdiplusToken=0;
BOOL CEBUIApp::InitInstance()
{
#ifdef _USRDLL
	if (!CWinApp::InitInstance())
		return FALSE;
#endif

	setlocale(LC_ALL, "");	// 使用默认环境 .936中文目录有问题。

//#ifdef USES_EBUI_STATIC
//	INITCOMMONCONTROLSEX InitCtrls;
//	InitCtrls.dwSize = sizeof(InitCtrls);
//	// Set this to include all the common control classes you want to use
//	// in your application.
//	InitCtrls.dwICC = ICC_WIN95_CLASSES;
//	InitCommonControlsEx(&InitCtrls);
//	//!!!!MFC初始化
//	if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(),0))
//	{
//		//??_Module.LogEvent(_T("MFC初始化错误！"), EVENTLOG_ERROR_TYPE );
//		return FALSE;
//	}
//#endif

	::CoInitialize(NULL);
	//::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	ChangeMessageProi();
	AfxEnableControlContainer();
    CMenuXP::InitializeHook();
	::AfxInitRichEdit2();
	if (theGdiplusToken==0)
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);
	}

	// 执行DLL注册命令
#ifndef _DEBUG
	CString sParameter;
	sParameter.Format(_T("\"%s\\eboleimage.dll\" /s"), this->GetAppPath());
	ShellExecute(NULL,  "open", "regsvr32", sParameter, GetAppPath(), SW_HIDE);
	sParameter.Format(_T("\"%s\\ebcontrol.dll\" /s"), this->GetAppPath());
	ShellExecute(NULL,  "open", "regsvr32", sParameter, GetAppPath(), SW_HIDE);
	sParameter.Format(_T("\"%s\\ebvideoroom.dll\" /s"), this->GetAppPath());
	ShellExecute(NULL,  "open", "regsvr32", sParameter, GetAppPath(), SW_HIDE);

	//// for ebclient&ebrd
	////CComPtr<IEBRemoteDesktop> pEBRemoteDesktop;
	////HRESULT hr = CoCreateInstance(__uuidof(EBRemoteDesktop),NULL,CLSCTX_INPROC_SERVER,__uuidof(IEBRemoteDesktop),(void**)&pEBRemoteDesktop);
	////if (FAILED(hr))
	//{
	//	sParameter.Format(_T("\"%s\\ebcore.dll\" /s"), this->GetAppPath());
	//	ShellExecute(NULL,  "open", "regsvr32", sParameter, GetAppPath(), SW_HIDE);
	//}
	//pEBRemoteDesktop.Release();
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
	const CEBString sVersion = theEBSetting->Version.GetBSTR();
#else
	// 读取配置文件
	theSetting.load(GetSettingFile());
	const CEBString sVersion = theSetting.GetVersion();
#endif
	//// 检查自动更新文件；
	//const CString sNewVersion = EBC_GetProfileString(_T("new_version"),_T("version"));
	//const CString sNewVersionFile = EBC_GetProfileString(_T("new_version"),_T("file"));
	////const CString sNewVersionFile = GetAppPath()+_T("\\1.0.0.1.rar");
	//if (!sNewVersion.IsEmpty())
	//{
	//	if (::PathFileExists(sNewVersionFile) && sNewVersion != sVersion.c_str())
	//	{
	//		// 有新版本需要自动更新
	//		//const CString sDescription = EBC_GetProfileString(_T("new_version"),_T("desc"));
	//		//const CString sUpdateTime = EBC_GetProfileString(_T("new_version"),_T("time"));
	//		//const UINT nUpdateType = EBC_GetProfileInt(_T("new_version"),_T("type"),0);

	//		// 执行自动更新过程
	//		TCHAR lpszModuleFileName[MAX_PATH];
	//		GetModuleFileName( NULL, lpszModuleFileName, MAX_PATH);
	//		CString sUpdateExe = GetAppPath()+_T("\\update\\ebupdate2.exe");
	//		CString sParameter;
	//		sParameter.Format(_T("\"%s\" \"%s\""), sNewVersionFile, lpszModuleFileName);
	//		ShellExecute(NULL,  "open", sUpdateExe, sParameter, GetAppPath(), SW_SHOW);
	//		return FALSE;
	//	}else
	//	{
	//		// 自动更新完成，或其他错误
	//		CString sUpdateExe2 = GetAppPath()+_T("\\update\\ebupdate2.exe");
	//		CString sNewUpdateExe = GetAppPath()+_T("\\update\\ebupdate2_new.exe");
	//		if (PathFileExists(sNewUpdateExe))
	//		{
	//			// 更新ebupdate2.exe文件
	//			for (int i=0; i<10; i++)	// 等级ebupdate2.exe程序退出
	//			{
	//				if (CopyFile(sNewUpdateExe,sUpdateExe2,FALSE))
	//				{
	//					break;
	//				}
	//				Sleep(200);
	//			}
	//			DeleteFile(sNewUpdateExe);
	//		}
	//		// 更新
	//		CString sRarExe2 = GetAppPath()+_T("\\update\\Rar.exe");
	//		CString sNewRarExe = GetAppPath()+_T("\\update\\Rar_new.exe");
	//		if (PathFileExists(sNewRarExe))
	//		{
	//			// 更新Rar.exe文件
	//			for (int i=0; i<10; i++)	// 等级Rar.exe程序退出
	//			{
	//				if (CopyFile(sNewRarExe,sRarExe2,FALSE))
	//				{
	//					break;
	//				}
	//				Sleep(200);
	//			}
	//			DeleteFile(sNewRarExe);
	//		}

	//		//DeleteFile(GetSettingIniFile());
	//		EBC_SetProfileString(_T("new_version"),_T("version"),_T(""));
	//		EBC_SetProfileString(_T("new_version"),_T("desc"),_T(""));
	//		EBC_SetProfileString(_T("new_version"),_T("time"),_T(""));
	//		EBC_SetProfileString(_T("new_version"),_T("file"),_T(""));
	//		EBC_SetProfileInt(_T("new_version"),_T("type"),0);
	//	}
	//}

	m_sAppImgTempPath = m_sAppPath + _T("\\img\\temp");
	if (!::PathFileExists(m_sAppImgTempPath))
		CreateDirectory(m_sAppImgTempPath,NULL);	

	USES_CONVERSION;
	//CString sImagePath = GetAppPath()+_T("\\img\\entboost.ico");
	//m_imageEbIcon = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath));   
	//sImagePath = GetAppPath()+_T("\\img\\entlogo.png");
	//m_imageEntLogo = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath));
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(),0))
	//{
	//	return FALSE;
	//}

	//TCHAR fullPath[MAX_PATH];
	//HINSTANCE hrs = AfxGetResourceHandle(); // 获得当前资源句柄（一般是exe的资源句柄）
	//AfxSetResourceHandle(theApp.m_hInstance); // 设置为dll的资源句柄
	//HINSTANCE h_CurDll = AfxGetResourceHandle(); //取得dll的句柄
	//AfxSetResourceHandle(hrs); // 调用结束后，重新设置为exe的资源句柄
	//GetModuleFileName( h_CurDll , fullPath, MAX_PATH );

	// OK
	//static HINSTANCE theCurrentInst = NULL;
	//if (theCurrentInst==NULL)
	//{
	//	CString strModulePath;
	//	GetModuleFileName( m_hInstance, strModulePath.GetBuffer(MAX_PATH), MAX_PATH);
	//	theCurrentInst = GetModuleHandle(strModulePath);
	//}
	//AfxGetModuleState()->m_hCurrentInstanceHandle = theCurrentInst;
	//AfxGetModuleState()->m_hCurrentResourceHandle = theCurrentInst;
	if (m_image1Group==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_IMG1_GROUP,_T("png"),m_image1Group);
	if (m_image1People==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),m_image1People);
	if (m_image1Cloud==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_IMG1_CLOUD,_T("png"),m_image1Cloud);
	if (m_image2Cloud==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_IMG2_CLOUD,_T("png"),m_image2Cloud);
	if (m_imageDefaultVisitor==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_VISITOR,_T("png"),m_imageDefaultVisitor);
	if (m_imageDefaultOrg==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_ORG,_T("png"),m_imageDefaultOrg);
	CString sFilePath;
	if (m_imageDefaultContact==NULL)
	{
		sFilePath = this->GetAppPath() + _T("/img/defaultcontact.png");
		if (::PathFileExists(sFilePath))
			m_imageDefaultContact = new Gdiplus::Image(A2W_ACP(sFilePath));
		else
			m_imageDefaultContact = m_imageDefaultVisitor->Clone();
	}
	if (m_imageDefaultMember==NULL)
	{
		sFilePath = this->GetAppPath() + _T("/img/defaultmember.png");
		if (::PathFileExists(sFilePath))
			m_imageDefaultMember = new Gdiplus::Image(A2W_ACP(sFilePath));
		else
			m_imageDefaultMember = m_imageDefaultContact->Clone();
	}
	sFilePath = this->GetAppPath() + _T("/img/defaultapp.png");
	if (::PathFileExists(sFilePath))
		m_sDefaultAppImage = sFilePath;
	//else
	//	m_imageDefaultApp = m_imageDefaultContact->Clone();
	//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_CONTACT,_T("png"),m_imageDefaultContact);
	//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("png"),m_imageDefaultMember);
	if (m_imageDefaultDepartment==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_DEPARTMENT,_T("png"),m_imageDefaultDepartment);
	if (m_imageDefaultProject==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_PROJECT,_T("png"),m_imageDefaultProject);
	if (m_imageDefaultGroup==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_GROUP,_T("png"),m_imageDefaultGroup);
	if (m_imageDefaultTempGroup==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_TEMPGROUP,_T("png"),m_imageDefaultTempGroup);
	if (m_imageStateAway==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_STATE_AWAY,_T("png"),m_imageStateAway);
	if (m_imageStateBusy==NULL)
		libEbc::ImageFromIDResource(IDB_PNG_STATE_BUSY,_T("png"),m_imageStateBusy);

	return TRUE;
}

int CEBUIApp::ExitInstance()
{
	m_pChatMenuSubscribeFuncList.clear();
	if (m_pDlgEmotionSelect)
	{
		m_pDlgEmotionSelect->DestroyWindow();
		delete m_pDlgEmotionSelect;
		m_pDlgEmotionSelect = NULL;
	}
	if (m_pAutoOpenFuncWindow!=NULL)
	{
		m_pAutoOpenFuncWindow->DestroyWindow();
		delete m_pAutoOpenFuncWindow;
		m_pAutoOpenFuncWindow = NULL;
	}

	//if (m_imageEbIcon)
	//	delete m_imageEbIcon;
	//if (m_imageEntLogo)
	//	delete m_imageEntLogo;
	if (m_image1Group)
		delete m_image1Group;
	m_image1Group = 0;
	if (m_image1People)
		delete m_image1People;
	m_image1People = 0;
	if (m_image1Cloud)
		delete m_image1Cloud;
	m_image1Cloud = 0;
	if (m_image2Cloud)
		delete m_image2Cloud;
	m_image2Cloud = 0;
	if (m_imageDefaultVisitor)
		delete m_imageDefaultVisitor;
	m_imageDefaultVisitor = 0;
	if (m_imageDefaultOrg)
		delete m_imageDefaultOrg;
	m_imageDefaultOrg = 0;
	if (m_imageDefaultContact)
		delete m_imageDefaultContact;
	m_imageDefaultContact = 0;
	if (m_imageDefaultMember)
		delete m_imageDefaultMember;
	m_imageDefaultMember = 0;
	if (m_imageDefaultDepartment)
		delete m_imageDefaultDepartment;
	m_imageDefaultDepartment = 0;
	if (m_imageDefaultProject)
		delete m_imageDefaultProject;
	m_imageDefaultProject = 0;
	if (m_imageDefaultGroup)
		delete m_imageDefaultGroup;
	m_imageDefaultGroup = 0;
	if (m_imageDefaultTempGroup)
		delete m_imageDefaultTempGroup;
	m_imageDefaultTempGroup = 0;
	if (m_imageStateAway)
		delete m_imageStateAway;
	m_imageStateAway = 0;
	if (m_imageStateBusy)
		delete m_imageStateBusy;
	m_imageStateBusy = 0;

	m_pCallList.clear();
	{
		BoostWriteLock wtlock(m_pMsgRecord.mutex());
		//boost::mutex::scoped_lock lock(m_pMsgRecord.mutex());
		CLockMap<eb::bigint, CDlgMsgRecord*>::const_iterator pIterMsgRecord = m_pMsgRecord.begin();
		for (; pIterMsgRecord!=m_pMsgRecord.end(); pIterMsgRecord++)
		{
			CDlgMsgRecord * pDlgDialog = pIterMsgRecord->second;
			pDlgDialog->DestroyWindow();
			//delete pDlgDialog;
		}
		wtlock.unlock();
		m_pMsgRecord.clear();
	}
	{
		BoostWriteLock wtlock(m_pResourceMgr.mutex());
		//boost::mutex::scoped_lock lock(m_pResourceMgr.mutex());
		CLockMap<eb::bigint, CDlgResourceMgr*>::const_iterator pIterMsgRecord = m_pResourceMgr.begin();
		for (; pIterMsgRecord!=m_pResourceMgr.end(); pIterMsgRecord++)
		{
			CDlgResourceMgr * pDlgDialog = pIterMsgRecord->second;
			pDlgDialog->DestroyWindow();
			//delete pDlgDialog;
		}
		wtlock.unlock();
		m_pResourceMgr.clear();
	}

	{
		CWnd * pWnd = NULL;
		while (m_pCloseWnd.front(pWnd))
		{
			pWnd->DestroyWindow();
		}
	}

	//{
	//	// 
	//	BoostWriteLock wtlock(m_pVideoConferenceList.mutex());
	//	CLockMap<eb::bigint, CDlgVideoConference::pointer>::const_iterator pIter = m_pVideoConferenceList.begin();
	//	for (; pIter!=m_pVideoConferenceList.end(); pIter++)
	//	{
	//		CDlgVideoConference::pointer pDlgDialog = pIter->second;
	//		pDlgDialog->DestroyWindow();
	//	}
	//	wtlock.unlock();
	//	m_pVideoConferenceList.clear();
	//}

//#ifndef USES_FRAME_LIST
//	{
//		BoostWriteLock wtlock(m_pDialogList.mutex());
//		//boost::mutex::scoped_lock lockDialog(m_pDialogList.mutex());
//		CLockMap<eb::bigint, CDlgDialog::pointer>::const_iterator pIter = m_pDialogList.begin();
//		for (; pIter!=m_pDialogList.end(); pIter++)
//		{
//			CDlgDialog::pointer pDlgDialog = pIter->second;
//			//pDlgDialog->ExitRoom();
//			pDlgDialog->DestroyWindow();
//		}
//		wtlock.unlock();
//		m_pDialogList.clear();
//	}
//#endif
	SaveUserSetting();
	if (theGdiplusToken>0)
	{
		Gdiplus::GdiplusShutdown(theGdiplusToken);
		theGdiplusToken = 0;
	}

#ifdef USES_EBCOM_TEST
	theEBSetting.Release();
	if (theEBClientCore != NULL)
	{
		theEBClientCore->EB_Logout();
		theEBClientCore->EB_UnInit();
		theEBClientCore.Release();
	}
#else
	theEBAppClient.EB_Logout();
	theEBAppClient.EB_UnInit();
#endif

	if (m_pBoEB.get()!=NULL)
	{
		bo::bodb_exit(m_pBoEB);
		m_pBoEB.reset();
	}
	m_pBoUsers.reset();
    CMenuXP::UninitializeHook();
	::CoUninitialize();
#ifdef _USRDLL
	CWinApp::ExitInstance();
#endif
	return 0;
}

int CEBUIApp::GetSendType(void)
{
	if (m_nSendType==-1)
		m_nSendType = (int)GetPrivateProfileInt(_T("default"),_T("send-type"),0,GetUserSettingIniFile());
	return m_nSendType;
}
void CEBUIApp::SetSendType(int nSendType)
{
	if (m_nSendType!=nSendType)
	{
		m_nSendType = nSendType;
		char lpszBuf[12];
		sprintf(lpszBuf,"%d",m_nSendType);
		::WritePrivateProfileString(_T("default"),_T("send-type"),lpszBuf,GetUserSettingIniFile());
	}
}


CString CEBUIApp::GetSettingFile(void) const
{
	return GetAppDataPath() + _T("\\setting");
}
CString CEBUIApp::GetSettingIniFile(void) const
{
	return GetAppDataPath() + _T("\\setting.ini");
}
CString CEBUIApp::GetUserSettingIniFile(void) const
{
	return m_sUserMainPath + _T("\\setting.ini");
}

void CEBUIApp::EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	::WritePrivateProfileString(lpszAppName,lpszKeyName,lpszValue,sIniSettingFile);
}
CString CEBUIApp::EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	TCHAR lpszBuffer[1024];
	memset(lpszBuffer,0,sizeof(lpszBuffer));
	::GetPrivateProfileString(lpszAppName,lpszKeyName,lpszDefault,lpszBuffer,sizeof(lpszBuffer),sIniSettingFile);
	return lpszBuffer;
}
void CEBUIApp::EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	CString sValue;
	sValue.Format(_T("%d"),nValue);
	::WritePrivateProfileString(lpszAppName,lpszKeyName,sValue,sIniSettingFile);
}
void CEBUIApp::WriteProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue)
{
	EBC_SetProfileInt(lpszAppName,lpszKeyName,nValue);
}
UINT CEBUIApp::EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	return ::GetPrivateProfileInt(lpszAppName,lpszKeyName,nDefault,sIniSettingFile);
}
UINT CEBUIApp::GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue)
{
	return EBC_GetProfileInt(lpszAppName,lpszKeyName,nValue);
}

HICON CEBUIApp::GetFileHIcon(const std::string& sFilePath)
{
	HICON result = NULL;
	std::string sFileName;
	std::string sFileExt;
	libEbc::GetFileExt(sFilePath,sFileName,sFileExt);
	if (!sFileExt.empty())
	{
		if (m_pFileHIcon.find(sFileExt,result))
		{
			return result;
		}
	}
	SHFILEINFO sfi; 
	ZeroMemory(&sfi,sizeof(sfi)); 
	DWORD ret = SHGetFileInfo(sFilePath.c_str(),
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
	if (ret == 1)
	{
		result = sfi.hIcon;
		if (!sFileExt.empty())
		{
			m_pFileHIcon.insert(sFileExt,result);
		}
	}
	return result;
}
void CEBUIApp::ClearSubscribeSelectInfo(void)
{
	m_pSubscribeFuncList.clear();
	m_nSelectCallId = 0;
	m_nSelectUserId = 0;
	m_sSelectAccount = "";
	m_nSelectGroupId = 0;
}

size_t CEBUIApp::BuildChatMenuSubscribeFuncList(void)
{
	if (m_pChatMenuSubscribeFuncList.empty())
	{
#ifdef USES_EBCOM_TEST
		_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_RIGHT_CLICK_MENU_CHAT_RECORD);
		if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_DISPATCH)
					continue;
				CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
				var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
				if (pSubscribeFuncInfo == NULL) continue;
				m_pChatMenuSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
			}
		}
#else
		theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_RIGHT_CLICK_MENU_CHAT_RECORD,m_pChatMenuSubscribeFuncList);
		//theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_RIGHT_CLICK_MENU_MAINFRAME,m_pChatMenuSubscribeFuncList);
#endif
	}
	return m_pChatMenuSubscribeFuncList.size();
}

//std::string CEBUIApp::GetFullAccount(const char* lpszAccount) const
//{
//	std::string sAccount(lpszAccount);
//	std::string::size_type find = sAccount.find("@");
//	if (find == std::string::npos)
//	{
//		sAccount.append("@");
//		sAccount.append(theSetting.GetServerAddress());
//	}
//	return sAccount;
//}
//int CEBUIApp::GetManagerLevel(const char* sGroupCode) const
//{
//	EB_MemberInfo pMyEmployeeInfo;
//	if (!theEBAppClient.EB_GetMyMemberInfo(&pMyEmployeeInfo,sGroupCode))
//		return 0;
//	return pMyEmployeeInfo.m_nManagerLevel;
//}

void CEBUIApp::GetScreenSize(int & pWidth, int & pHeight)
{
	pWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	pHeight += GetSystemMetrics(SM_CYCAPTION);
}

void CEBUIApp::ReadUserSetting(void)
{
	TCHAR szAppFullPath[MAX_PATH];
	sprintf(szAppFullPath, _T("%s\\setting.ini"), m_sUserMainPath);
	m_cfDefault.bCharSet = GetPrivateProfileInt( _T("FONT"),_T("CharSet"),134,szAppFullPath);
	m_cfDefault.dwMask  = GetPrivateProfileInt(_T("FONT"),_T("Mask"),3892314127,szAppFullPath);
	m_cfDefault.dwEffects = GetPrivateProfileInt(_T("FONT"),_T("Effects"),0,szAppFullPath);
	m_cfDefault.yHeight = GetPrivateProfileInt(_T("FONT"),_T("Height"),240,szAppFullPath);
	m_cfDefault.yOffset = GetPrivateProfileInt(_T("FONT"),_T("Offset"),0,szAppFullPath);
	m_cfDefault.crTextColor = GetPrivateProfileInt(_T("FONT"),_T("Color"),0,szAppFullPath);
	m_cfDefault.bPitchAndFamily = GetPrivateProfileInt(_T("FONT"),_T("PitchAndFamily"),2,szAppFullPath);
	m_cfDefault.cbSize = GetPrivateProfileInt(_T("FONT"),_T("Size"),0  ,szAppFullPath);
	GetPrivateProfileString(_T("FONT"),_T("FaceName"),_T("宋体"), m_cfDefault.szFaceName ,LF_FACESIZE,szAppFullPath);	
}

inline BOOL WritePrivateProfileInt(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt int nValue,
    __in_opt LPCSTR lpFileName
    )
{
	TCHAR lpString[20];
	sprintf(lpString, _T("%d"), nValue);
	return ::WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
}

void CEBUIApp::SaveUserSetting(void)
{
	TCHAR szAppFullPath[MAX_PATH];
	sprintf(szAppFullPath, _T("%s\\setting.ini"), m_sUserMainPath);
	WritePrivateProfileInt( _T("FONT"),_T("CharSet"),m_cfDefault.bCharSet,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Mask"),m_cfDefault.dwMask,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Effects"),m_cfDefault.dwEffects,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Height"),m_cfDefault.yHeight,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Offset"),m_cfDefault.yOffset,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Color"),m_cfDefault.crTextColor,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("PitchAndFamily"),m_cfDefault.bPitchAndFamily,szAppFullPath);
	WritePrivateProfileInt(_T("FONT"),_T("Size"),m_cfDefault.cbSize,szAppFullPath);
	WritePrivateProfileString(_T("FONT"),_T("FaceName"),m_cfDefault.szFaceName,szAppFullPath);	
}

void CEBUIApp::ShowImageWindow(bool bShow,CImageSelectCallback* pCallback,LPRECT lpRect)
{
	if (m_pDlgEmotionSelect==NULL)
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		//CWnd * pParent = this;
		m_pDlgEmotionSelect = new CDlgEmotionSelect(pParent);
		m_pDlgEmotionSelect->Create(CDlgEmotionSelect::IDD,pParent);
	}else
	{
		m_pDlgEmotionSelect->SetCtrlColor();
		m_pDlgEmotionSelect->ShowImage();
	}
	m_pDlgEmotionSelect->SetCallback(pCallback);
	if (lpRect != NULL)
		m_pDlgEmotionSelect->MoveWindow(lpRect);
	if (bShow)
	{
		m_pDlgEmotionSelect->ShowWindow(SW_SHOW);
		m_pDlgEmotionSelect->SetForegroundWindow();
		m_pDlgEmotionSelect->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	}else
	{
		m_pDlgEmotionSelect->ShowWindow(SW_HIDE);
	}
	//m_pDlgEmotionSelect->SetCircle();
}

void CEBUIApp::RunEBSC(const CString& sParameter)
{
	CString sEBDCFile = theApp.GetAppPath()+_T("\\ebsc.exe");
	ShellExecute(NULL, "open", sEBDCFile, sParameter, theApp.GetAppPath(), SW_SHOW);
}

void CEBUIApp::InvalidateParentRect(const CWnd* pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetParent()->ScreenToClient(&rect);
	pWnd->GetParent()->InvalidateRect(&rect);
}

void CEBUIApp::NewEmployeeInfo(CWnd* pParent,const CTreeItemInfo * pTreeItemInfo)
{
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP) return;
#ifdef USES_EBCOM_TEST
	CEBString sGroupName = theEBClientCore->EB_GetGroupName(pTreeItemInfo->m_sGroupCode).GetBSTR();
	if (sGroupName.empty()) return;
	CDlgMemberInfo pDlgMemberInfo(pParent);
	pDlgMemberInfo.m_nGroupType = (EB_GROUP_TYPE)pTreeItemInfo->m_nSubType;
	pDlgMemberInfo.m_sGroupCode = pTreeItemInfo->m_sGroupCode;
	pDlgMemberInfo.m_sGroupName = sGroupName.c_str();
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		CIEB_MemberInfo* pNewMemberInfo = new CIEB_MemberInfo();
		pNewMemberInfo->GroupCode = pTreeItemInfo->m_sGroupCode;
		pNewMemberInfo->MemberAccount = (LPCTSTR)pDlgMemberInfo.m_sMemberAccount;
		pNewMemberInfo->UserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pNewMemberInfo->JobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pNewMemberInfo->JobPosition = pDlgMemberInfo.m_nJobPosition;
		pNewMemberInfo->CellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pNewMemberInfo->WorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pNewMemberInfo->Email = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pNewMemberInfo->Fax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pNewMemberInfo->Address = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pNewMemberInfo->Description = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pNewMemberInfo->Gender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pNewMemberInfo->Birthday = pDlgMemberInfo.m_nBirthday;
		CComPtr<IEB_MemberInfo> pInterface;
		pNewMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		theEBClientCore->EB_EditMember(pInterface);
	}
#else
	CEBString sGroupName;
	if (!theEBAppClient.EB_GetGroupName(pTreeItemInfo->m_sGroupCode,sGroupName)) return;

	////////////////// test start
	//for (int i=0;i<500;i++)
	//{
	//	EB_MemberInfo pEditPopMemberInfo(pTreeItemInfo->m_sGroupCode, 0);
	//	char sMemberAccount[64];
	//	sprintf(sMemberAccount,"tk%d@entboost.com",i);
	//	pEditPopMemberInfo.m_sMemberAccount = sMemberAccount;
	//	pEditPopMemberInfo.m_sUserName = sMemberAccount;
	//	pEditPopMemberInfo.m_sJobTitle = "C++开发工程师";
	//	pEditPopMemberInfo.m_sEmail = "yhz@entboost.com";
	//	pEditPopMemberInfo.m_sFax = "0755-27362216";
	//	pEditPopMemberInfo.m_sCellPhone = "0755-27362216";
	//	pEditPopMemberInfo.m_sAddress = "深圳宝安翻身路47区银丰商务大厦501室";
	//	pEditPopMemberInfo.m_sDescription = "这是测试员工描述。";
	//	theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	//}
	//return;
	//////////////// test end
	AFX_SWITCH_INSTANCED();
	CDlgMemberInfo pDlgMemberInfo(pParent);
	pDlgMemberInfo.m_nGroupType = (EB_GROUP_TYPE)pTreeItemInfo->m_nSubType;
	pDlgMemberInfo.m_sGroupCode = pTreeItemInfo->m_sGroupCode;
	pDlgMemberInfo.m_sGroupName = sGroupName.c_str();
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		EB_MemberInfo pEditPopMemberInfo(pTreeItemInfo->m_sGroupCode, 0);
		pEditPopMemberInfo.m_sMemberAccount = (LPCTSTR)pDlgMemberInfo.m_sMemberAccount;
		pEditPopMemberInfo.m_sUserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pEditPopMemberInfo.m_sJobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pEditPopMemberInfo.m_nJobPosition = pDlgMemberInfo.m_nJobPosition;
		pEditPopMemberInfo.m_sCellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pEditPopMemberInfo.m_sWorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pEditPopMemberInfo.m_sEmail = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pEditPopMemberInfo.m_sFax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pEditPopMemberInfo.m_sAddress = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pEditPopMemberInfo.m_sDescription = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pEditPopMemberInfo.m_nGender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pEditPopMemberInfo.m_nBirthday = pDlgMemberInfo.m_nBirthday;
		theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	}
#endif
}

#ifdef USES_EBCOM_TEST
void CEBUIApp::EditEmployeeInfo(CWnd* pParent,IEB_MemberInfo* pMemberInfo)
{
	if (pMemberInfo==NULL) return;
	const CEBString sGroupName = theEBClientCore->EB_GetGroupName(pMemberInfo->GroupCode).GetBSTR();
	if (sGroupName.empty()) return;
	CDlgMemberInfo pDlgMemberInfo(pParent);
	pDlgMemberInfo.m_sGroupCode = pMemberInfo->GroupCode;
	pDlgMemberInfo.m_sMemberCode = pMemberInfo->MemberCode;
	pDlgMemberInfo.m_sMemberAccount = pMemberInfo->MemberAccount.GetBSTR();
	pDlgMemberInfo.m_sUserName = pMemberInfo->UserName.GetBSTR();
	pDlgMemberInfo.m_sGroupName = sGroupName.c_str();
	pDlgMemberInfo.m_sJobTitle = pMemberInfo->JobTitle.GetBSTR();
	pDlgMemberInfo.m_nJobPosition = pMemberInfo->JobPosition;
	pDlgMemberInfo.m_sCellPhone = pMemberInfo->CellPhone.GetBSTR();
	pDlgMemberInfo.m_sWorkPhone = pMemberInfo->WorkPhone.GetBSTR();
	pDlgMemberInfo.m_sEmail = pMemberInfo->Email.GetBSTR();
	pDlgMemberInfo.m_sFax = pMemberInfo->Fax.GetBSTR();
	pDlgMemberInfo.m_sAddress = pMemberInfo->Address.GetBSTR();
	pDlgMemberInfo.m_sDescription = pMemberInfo->Description.GetBSTR();
	pDlgMemberInfo.m_nGender = pMemberInfo->Gender;
	pDlgMemberInfo.m_nBirthday = pMemberInfo->Birthday;
	pDlgMemberInfo.m_pMemberInfo = pMemberInfo;
	//pDlgMemberInfo.m_sHeadResourceFile = pMemberInfo->HeadResourceFile.c_str();
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		EB_MemberInfo pEditPopMemberInfo(pMemberInfo);
		pEditPopMemberInfo.m_sMemberAccount = (LPCTSTR)pDlgMemberInfo.m_sMemberAccount;
		pEditPopMemberInfo.m_sUserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pEditPopMemberInfo.m_sJobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pEditPopMemberInfo.m_nJobPosition = pDlgMemberInfo.m_nJobPosition;
		pEditPopMemberInfo.m_sCellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pEditPopMemberInfo.m_sWorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pEditPopMemberInfo.m_sEmail = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pEditPopMemberInfo.m_sFax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pEditPopMemberInfo.m_sAddress = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pEditPopMemberInfo.m_sDescription = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pEditPopMemberInfo.m_nGender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pEditPopMemberInfo.m_nBirthday = pDlgMemberInfo.m_nBirthday;
		CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo(pEditPopMemberInfo);
		CComPtr<IEB_MemberInfo> pOutInterface;
		pIEBMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pOutInterface);
		theEBClientCore->EB_EditMember(pOutInterface);
	}
}
#else
void CEBUIApp::EditEmployeeInfo(CWnd* pParent,const EB_MemberInfo* pMemberInfo)
{
	if (pMemberInfo==NULL) return;
	std::string sDepartmentName;
	if (!theEBAppClient.EB_GetGroupName(pMemberInfo->m_sGroupCode,sDepartmentName)) return;
	CDlgMemberInfo pDlgMemberInfo(pParent);
	pDlgMemberInfo.m_sGroupCode = pMemberInfo->m_sGroupCode;
	pDlgMemberInfo.m_sMemberCode = pMemberInfo->m_sMemberCode;
	pDlgMemberInfo.m_sMemberAccount = pMemberInfo->m_sMemberAccount.c_str();
	pDlgMemberInfo.m_sUserName = pMemberInfo->m_sUserName.c_str();
	pDlgMemberInfo.m_sGroupName = sDepartmentName.c_str();
	pDlgMemberInfo.m_sJobTitle = pMemberInfo->m_sJobTitle.c_str();
	pDlgMemberInfo.m_nJobPosition = pMemberInfo->m_nJobPosition;
	pDlgMemberInfo.m_sCellPhone = pMemberInfo->m_sCellPhone.c_str();
	pDlgMemberInfo.m_sWorkPhone = pMemberInfo->m_sWorkPhone.c_str();
	pDlgMemberInfo.m_sEmail = pMemberInfo->m_sEmail.c_str();
	pDlgMemberInfo.m_sFax = pMemberInfo->m_sFax.c_str();
	pDlgMemberInfo.m_sAddress = pMemberInfo->m_sAddress.c_str();
	pDlgMemberInfo.m_sDescription = pMemberInfo->m_sDescription.c_str();
	pDlgMemberInfo.m_nGender = pMemberInfo->m_nGender;
	pDlgMemberInfo.m_nBirthday = pMemberInfo->m_nBirthday;
	pDlgMemberInfo.m_pMemberInfo = pMemberInfo;
	//pDlgMemberInfo.m_sHeadResourceFile = pMemberInfo->m_sHeadResourceFile.c_str();
	AFX_SWITCH_INSTANCED();
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		EB_MemberInfo pEditPopMemberInfo(pMemberInfo);
		pEditPopMemberInfo.m_sUserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pEditPopMemberInfo.m_sJobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pEditPopMemberInfo.m_nJobPosition = pDlgMemberInfo.m_nJobPosition;
		pEditPopMemberInfo.m_sCellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pEditPopMemberInfo.m_sWorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pEditPopMemberInfo.m_sEmail = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pEditPopMemberInfo.m_sFax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pEditPopMemberInfo.m_sAddress = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pEditPopMemberInfo.m_sDescription = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pEditPopMemberInfo.m_nGender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pEditPopMemberInfo.m_nBirthday = pDlgMemberInfo.m_nBirthday;
		theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	}
}
#endif
void CEBUIApp::EditGroupInfo(CWnd* pParent, eb::bigint sGroupCode)
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(sGroupCode);
	if (pGroupInfo == NULL) return;
	const eb::bigint nGroupCreateUserId = pGroupInfo->CreateUserId;
	if (nGroupCreateUserId != theApp.GetLogonUserId())
	{
		// 不是部门创建者，不能删除
		return;
	}

	CDlgGroupInfo pDlgGroupInfo(pParent);
	pDlgGroupInfo.m_nGroupId = pGroupInfo->GroupCode;
	pDlgGroupInfo.m_nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
	pDlgGroupInfo.m_sGroupName = pGroupInfo->GroupName.GetBSTR();
	pDlgGroupInfo.m_sPhone = pGroupInfo->Phone.GetBSTR();
	pDlgGroupInfo.m_sFax = pGroupInfo->Fax.GetBSTR();
	pDlgGroupInfo.m_sEmail = pGroupInfo->Email.GetBSTR();
	pDlgGroupInfo.m_sUrl = pGroupInfo->Url.GetBSTR();
	pDlgGroupInfo.m_sAddress = pGroupInfo->Address.GetBSTR();
	pDlgGroupInfo.m_sDescription = pGroupInfo->Description.GetBSTR();
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		pGroupInfo->GroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pGroupInfo->Phone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pGroupInfo->Fax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pGroupInfo->Email = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pGroupInfo->Url = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pGroupInfo->Address = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pGroupInfo->Description = (LPCTSTR)pDlgGroupInfo.m_sDescription;
		theEBClientCore->EB_EditGroup(pGroupInfo);
	}
#else
	EB_GroupInfo pGroupInfo;
	if (!theEBAppClient.EB_GetGroupInfo(sGroupCode,&pGroupInfo))
	{
		return;
	}else if (pGroupInfo.m_nCreateUserId != theEBAppClient.EB_GetLogonUserId())
	{
		// 不是部门创建者，不能编辑
		return;
	}

	CDlgGroupInfo pDlgGroupInfo(pParent);
	pDlgGroupInfo.m_nGroupId = pGroupInfo.m_sGroupCode;
	pDlgGroupInfo.m_nGroupType = pGroupInfo.m_nGroupType;
	pDlgGroupInfo.m_sGroupName = pGroupInfo.m_sGroupName.c_str();
	pDlgGroupInfo.m_sPhone = pGroupInfo.m_sPhone.c_str();
	pDlgGroupInfo.m_sFax = pGroupInfo.m_sFax.c_str();
	pDlgGroupInfo.m_sEmail = pGroupInfo.m_sEmail.c_str();
	pDlgGroupInfo.m_sUrl = pGroupInfo.m_sUrl.c_str();
	pDlgGroupInfo.m_sAddress = pGroupInfo.m_sAddress.c_str();
	pDlgGroupInfo.m_sDescription = pGroupInfo.m_sDescription.c_str();
	AFX_SWITCH_INSTANCED();
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		EB_GroupInfo pEditPopDepartment(pGroupInfo.m_sEnterpriseCode, pGroupInfo.m_sGroupCode);
		pEditPopDepartment.operator =(pGroupInfo);
		pEditPopDepartment.m_sParentCode = pGroupInfo.m_sParentCode;
		pEditPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pEditPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pEditPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pEditPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pEditPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pEditPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pEditPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
		theEBAppClient.EB_EditGroup(&pEditPopDepartment);
	}
#endif
}

bool CEBUIApp::IsLogonVisitor(void) const
{
#ifdef USES_EBCOM_TEST
	return theEBClientCore != NULL && theEBClientCore->EB_IsLogonVisitor==VARIANT_TRUE;
#else
	return theEBAppClient.EB_IsLogonVisitor();
#endif

}

bool CEBUIApp::GetAppFrameShowed(void) const
{
	// ???
	//if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
	//	return m_pMainWnd->GetAppFrameShowed();
	return false;
}
int CALLBACK CEBUIApp::TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CTreeItemInfo * pItemInfo1 = (const CTreeItemInfo*)lParam1;
	const CTreeItemInfo * pItemInfo2 = (const CTreeItemInfo*)lParam2;
	if (pItemInfo1==NULL || pItemInfo2==NULL) return 1;
	if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		const bool bAppFrameShowed = theApp.GetAppFrameShowed();
		if (bAppFrameShowed)
			return pItemInfo1->m_dwItemData<pItemInfo2->m_dwItemData?1:-1;
		else
			return pItemInfo1->m_dwItemData>pItemInfo2->m_dwItemData?1:-1;
	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// 群（部门）比较，用名称排序
		if (pItemInfo1->m_nSubType!=pItemInfo2->m_nSubType)
		{
			return pItemInfo1->m_nSubType>pItemInfo2->m_nSubType?1:-1;
		}else if (pItemInfo1->m_nIndex==pItemInfo2->m_nIndex)
			return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
		else
			return pItemInfo1->m_nIndex>pItemInfo2->m_nIndex?1:-1;
	}else if ((pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) ||
		(pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT))
	{
		// 员工比较，先比较在线状态
		if (pItemInfo1->m_dwItemData==pItemInfo2->m_dwItemData)
		{
			// 相同在线状态，比较名称
			return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
		}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_ONLINE_NEW)
		{
			// 第一个在线，不用换位置
			return -1;
		}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_ONLINE_NEW)
		{
			// 第二个在线，对调位置
			return 1;
		}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_OFFLINE)
		{
			// 第一个离线，对调位置
			return 1;
		}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_OFFLINE)
		{
			// 第二个离线，不用换位置
			return -1;
		}else if (pItemInfo1->m_dwItemData!=EB_LINE_STATE_OFFLINE)
		{
			// 第一个不是离线，不用换位置
			return -1;
		}else if (pItemInfo2->m_dwItemData!=EB_LINE_STATE_OFFLINE)
		{
			// 第二个不是离线，对调位置
			return 1;
		}
		return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// 第一个是部门，排前面（不用调位置）
		return -1;
	}else if (pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// 第二个是部门，对调位置
		return 1;
	}
	return 0;
}

void CEBUIApp::SetLineState2Db(EB_USER_LINE_STATE nLineState)
{
	if (m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("update user_login_record_t set linestate=%d where account='%s'"),nLineState,theApp.GetLogonAccount());
		m_pBoEB->execsql(sSql);
		m_pBoEB->close();
	}
}

void CEBUIApp::DeleteDbRecord(eb::bigint sId, bool bIsAccount)
{
	// 先删除图片，语音，文件...
	CString sSql;
	if (bIsAccount)
	{
		sSql.Format(_T("select msg_name FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) AND msg_type>=%d"),sId,sId,MRT_JPG);
	}else
	{
		sSql.Format(_T("select msg_name FROM msg_record_t WHERE dep_code=%lld AND msg_type>=%d"),sId,MRT_JPG);
	}
	CString sUserImagePath(GetUserImagePath());
	sUserImagePath.MakeLower();
	USES_CONVERSION;
	int nCookie = 0;
	theApp.m_pBoUsers->select(sSql, nCookie);
	cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
	while (pRecord.get()!=NULL)
	{
		CString sMsgName(libEbc::UTF82ACP(pRecord->getVector()[0]->getStrValue().c_str()).c_str());
		sMsgName.MakeLower();
		// 判断是临时图片，语音，文件目录
		if (sMsgName.Find(sUserImagePath)==0)
		{
			// 删除临时目录图片
			DeleteFile(sMsgName);
		}
		pRecord = theApp.m_pBoUsers->next(nCookie);
	}
	theApp.m_pBoUsers->reset(nCookie);
	// 删除数据
	if (bIsAccount)
	{
		sSql.Format(_T("DELETE FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld)"),sId,sId);
	}else
	{
		sSql.Format(_T("DELETE FROM msg_record_t WHERE dep_code=%lld"),sId);
	}
	m_pBoUsers->execute(sSql);
}
void CEBUIApp::DeleteDbRecord(eb::bigint sMsgId)
{
	// 先删除图片，语音，文件...
	CString sSql;
	sSql.Format(_T("select msg_name FROM msg_record_t WHERE msg_id=%lld AND msg_type>=%d"),sMsgId,MRT_JPG);
	CString sUserImagePath(GetUserImagePath());
	sUserImagePath.MakeLower();
	USES_CONVERSION;
	int nCookie = 0;
	theApp.m_pBoUsers->select(sSql, nCookie);
	cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
	while (pRecord.get()!=NULL)
	{
		CString sMsgName(libEbc::UTF82ACP(pRecord->getVector()[0]->getStrValue().c_str()).c_str());
		sMsgName.MakeLower();
		// 判断是临时图片，语音，文件目录
		if (sMsgName.Find(sUserImagePath)==0)
		{
			// 删除临时目录图片
			DeleteFile(sMsgName);
		}
		pRecord = theApp.m_pBoUsers->next(nCookie);
	}
	theApp.m_pBoUsers->reset(nCookie);
	// 删除数据
	sSql.Format(_T("DELETE FROM msg_record_t WHERE msg_id=%lld"),sMsgId);
	m_pBoUsers->execute(sSql);
}

void CEBUIApp::OpenGroupShareWindow(eb::bigint sGroupCode,const CWnd* pWndLayout)
{
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (!m_pResourceMgr.find(sGroupCode,pDlgResourceMgr))
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgResourceMgr = new CDlgResourceMgr(pParent);
		pDlgResourceMgr->Create(CDlgResourceMgr::IDD,pParent);
		m_pResourceMgr.insert(sGroupCode,pDlgResourceMgr);
	}
	CRect rect;
	pWndLayout->GetWindowRect(&rect);
	rect.left = rect.right-100;
	rect.right = rect.left + 520;
	pDlgResourceMgr->MoveWindow(&rect);
	pDlgResourceMgr->SetCircle();
	pDlgResourceMgr->LoadGroupResource(sGroupCode);
}

void CEBUIApp::OpenMyShareWindow(const CWnd* pWndLayout)
{
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_IsLogoned == VARIANT_FALSE || theApp.IsLogonVisitor())
	{
		return;
	}
#else
	if (!theEBAppClient.EB_IsLogoned() || theApp.IsLogonVisitor()) return;
#endif
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	const eb::bigint sLogonAccount = theApp.GetLogonUserId();
	if (!m_pResourceMgr.find(sLogonAccount,pDlgResourceMgr))
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgResourceMgr = new CDlgResourceMgr(pParent);
		pDlgResourceMgr->Create(CDlgResourceMgr::IDD,pParent);
		m_pResourceMgr.insert(sLogonAccount,pDlgResourceMgr);
	}else
	{
		pDlgResourceMgr->SetCtrlColor();
	}
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	GetScreenSize(nScreenWidth, nScreenHeight);
	const int const_default_width = 520;
	CRect rect;
	pWndLayout->GetWindowRect(&rect);
	rect.left = (nScreenWidth-const_default_width)/2;
	//rect.left = rect.right-100;
	rect.right = rect.left + const_default_width;
	pDlgResourceMgr->MoveWindow(&rect);
	pDlgResourceMgr->SetCircle();
	pDlgResourceMgr->LoadMyResource();
	pDlgResourceMgr->SetForegroundWindow();
}
bool CEBUIApp::OpenSubscribeFuncWindow(const EB_SubscribeFuncInfo& pSubscribeFuncInfo,const std::string& sPostData,const std::string& sParameters)
{
	//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	//CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	//pFuncWindow->m_bBroadcastMsg = true;
	////pFuncWindow->m_sTitle = "aaaa";
	//pFuncWindow->m_sFuncUrl = "<title>你好TITLE</title><a href=\"eb-open2://720,680,0,0,1,1,1,http://www.entboost.com/private\">测试open2</>";
	//pFuncWindow->m_nWidth = 250;
	//pFuncWindow->m_nHeight = 180;
	//pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
	//pFuncWindow->ShowWindow(SW_SHOW);
	//return false;
#ifdef USES_EBCOM_TEST
	const CEBString sFuncUrl = theEBClientCore->EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount.c_str(),m_nSelectGroupId).GetBSTR();
#else
	const CEBString sFuncUrl = theEBAppClient.EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId);
#endif
	if (!sFuncUrl.empty())
	{
		CString sFullFuncurl;
		if (this->GetColorGradient())
			sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetFuncColor()),GetGValue(theApp.GetFuncColor()),GetBValue(theApp.GetFuncColor()));
		else
			sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetMainColor()),GetGValue(theApp.GetMainColor()),GetBValue(theApp.GetMainColor()));
		if (!sParameters.empty())
		{
			sFullFuncurl.Append(_T("&"));
			sFullFuncurl.Append(sParameters.c_str());
		}
		//ShellExecute(NULL, "open", sFullFuncurl, NULL, NULL, SW_SHOW);
		//return true;
		if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MODAL)	// 对话框模式
		{
			AFX_SWITCH_INSTANCED();
			CDlgFuncWindow pFuncWindow;
			pFuncWindow.m_sTitle = pSubscribeFuncInfo.m_sFunctionName;
			pFuncWindow.m_sFuncUrl = sFullFuncurl;
			pFuncWindow.m_sPostData = sPostData;
			pFuncWindow.m_nWidth = pSubscribeFuncInfo.m_nWindowWidth;
			pFuncWindow.m_nHeight = pSubscribeFuncInfo.m_nWindowHeight;
			pFuncWindow.m_bDisableContextMenu = pSubscribeFuncInfo.m_bDisableContextMenu;
			pFuncWindow.m_bDisableScrollBar = pSubscribeFuncInfo.m_bDisableScrollBar;
			pFuncWindow.m_bClosePrompt = pSubscribeFuncInfo.m_bClosePrompt;
			pFuncWindow.m_bWinResizable = pSubscribeFuncInfo.m_bWinResizable;
			pFuncWindow.DoModal();
		}else if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_WINDOW ||	// 窗口模式；
			(pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME && m_bHideMainFrame))
		{
			if (GetAutoOpenSubId()==pSubscribeFuncInfo.m_nSubscribeId)
			{
				if (m_pAutoOpenFuncWindow!=NULL)
				{
					if (!m_pAutoOpenFuncWindow->IsWindowVisible())
						m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
				}else
				{
					AFX_SWITCH_INSTANCED();
					CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
					m_pAutoOpenFuncWindow = new CDlgFuncWindow(pParent,false);
					m_pAutoOpenFuncWindow->m_sTitle = pSubscribeFuncInfo.m_sFunctionName;
					m_pAutoOpenFuncWindow->m_sFuncUrl = sFullFuncurl;
					//m_pAutoOpenFuncWindow->m_sPostData = sPostData;
					m_pAutoOpenFuncWindow->m_nWidth = pSubscribeFuncInfo.m_nWindowWidth;
					m_pAutoOpenFuncWindow->m_nHeight = pSubscribeFuncInfo.m_nWindowHeight;
					m_pAutoOpenFuncWindow->m_bDisableContextMenu = pSubscribeFuncInfo.m_bDisableContextMenu;
					m_pAutoOpenFuncWindow->m_bDisableScrollBar = pSubscribeFuncInfo.m_bDisableScrollBar;
					m_pAutoOpenFuncWindow->m_bClosePrompt = pSubscribeFuncInfo.m_bClosePrompt;
					m_pAutoOpenFuncWindow->m_bWinResizable = pSubscribeFuncInfo.m_bWinResizable;
					m_pAutoOpenFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
					//m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
				}
			}else
			{
				AFX_SWITCH_INSTANCED();
				CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
				CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
				pFuncWindow->m_sTitle = pSubscribeFuncInfo.m_sFunctionName;
				pFuncWindow->m_sFuncUrl = sFullFuncurl;
				pFuncWindow->m_sPostData = sPostData;
				pFuncWindow->m_nWidth = pSubscribeFuncInfo.m_nWindowWidth;
				pFuncWindow->m_nHeight = pSubscribeFuncInfo.m_nWindowHeight;
				pFuncWindow->m_bDisableContextMenu = pSubscribeFuncInfo.m_bDisableContextMenu;
				pFuncWindow->m_bDisableScrollBar = pSubscribeFuncInfo.m_bDisableScrollBar;
				pFuncWindow->m_bClosePrompt = pSubscribeFuncInfo.m_bClosePrompt;
				pFuncWindow->m_bWinResizable = pSubscribeFuncInfo.m_bWinResizable;
				pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
			}
		}else if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME)	// 主面板应用；
		{
			if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
				m_pMainWnd->AddAppUrl((LPCTSTR)sFullFuncurl,pSubscribeFuncInfo,false);
		}else if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_PROGRAM)
		{
			ShellExecute(NULL, "open", sFuncUrl.c_str(), NULL, NULL, SW_SHOW);
		}else// if (pSubscribeFuncInfo.m_nFunctionMode != EB_FUNC_MODE_MAINFRAME)	// 除了主面板，其他都自动打开
		{
			// program & post data(??)
			ShellExecute(NULL, "open", sFullFuncurl, NULL, NULL, SW_SHOW);
		}
		return true;
	}
	return false;
}

bool CEBUIApp::OpenAutoSubscribeFunc(const EB_SubscribeFuncInfo& pSubscribeFuncInfo)
{
	if (m_pAutoOpenFuncWindow!=NULL)
	{
		if (!m_pAutoOpenFuncWindow->IsWindowVisible())
			m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
		return true;
	}
	return OpenSubscribeFuncWindow(pSubscribeFuncInfo);
//#ifdef USES_EBCOM_TEST
//	const CEBString sFuncUrl = theEBClientCore->EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount.c_str(),m_nSelectGroupId).GetBSTR();
//#else
//	const CEBString sFuncUrl = theEBAppClient.EB_GetSubscribeFuncUrl(pSubscribeFuncInfo.m_nSubscribeId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId);
//#endif
//	if (!sFuncUrl.empty())
//	{
//		CString sFullFuncurl;
//		if (this->GetColorGradient())
//			sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetFuncColor()),GetGValue(theApp.GetFuncColor()),GetBValue(theApp.GetFuncColor()));
//		else
//			sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetMainColor()),GetGValue(theApp.GetMainColor()),GetBValue(theApp.GetMainColor()));
//
//		if (pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME)
//		{
//			CPOPDlg* pMainDlg = (CPOPDlg*)m_pMainWnd;
//			pMainDlg->AddAppUrl((LPCTSTR)sFullFuncurl,pSubscribeFuncInfo,false);
//		}else
//		{
//			CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
//			m_pAutoOpenFuncWindow = new CDlgFuncWindow(pParent,false);
//			m_pAutoOpenFuncWindow->m_sTitle = pSubscribeFuncInfo.m_sFunctionName;
//			m_pAutoOpenFuncWindow->m_sFuncUrl = sFullFuncurl;
//			//m_pAutoOpenFuncWindow->m_sPostData = sPostData;
//			m_pAutoOpenFuncWindow->m_nWidth = pSubscribeFuncInfo.m_nWindowWidth;
//			m_pAutoOpenFuncWindow->m_nHeight = pSubscribeFuncInfo.m_nWindowHeight;
//			m_pAutoOpenFuncWindow->m_bDisableContextMenu = pSubscribeFuncInfo.m_bDisableContextMenu;
//			m_pAutoOpenFuncWindow->m_bDisableScrollBar = pSubscribeFuncInfo.m_bDisableScrollBar;
//			m_pAutoOpenFuncWindow->m_bClosePrompt = pSubscribeFuncInfo.m_bClosePrompt;
//			m_pAutoOpenFuncWindow->m_bWinResizable = pSubscribeFuncInfo.m_bWinResizable;
//			m_pAutoOpenFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
//			m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
//		}
//		return true;
//	}
//	return false;
}
void CEBUIApp::ShowAutoOpenSubscribeFunc(int nCmdShow)
{
	if (m_pAutoOpenFuncWindow!=NULL)
	{
		m_pAutoOpenFuncWindow->ShowWindow(nCmdShow);
	}
}
void CEBUIApp::ShowHideAutoOpenSubscribeFunc(bool bShow)
{
	if (m_pAutoOpenFuncWindow!=NULL)
	{
		m_pAutoOpenFuncWindow->ShowWindow(bShow?SW_SHOW:SW_HIDE);

		//if (m_pAutoOpenFuncWindow->IsWindowVisible())
		//	m_pAutoOpenFuncWindow->ShowWindow(SW_HIDE);
		//else
		//	m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
	}
}

bool CEBUIApp::IsEnterpriseuserUser(void)
{
#ifdef USES_EBCOM_TEST
	return theEBClientCore->EB_IsEnterpriseUser(0)?true:false;
#else
	return theEBAppClient.EB_IsEnterpriseUser();
#endif
}

bool CEBUIApp::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
{
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
	{
		CString sDomainName(pTreeItemInfo->m_sAccount.c_str());
		CString sHttp;
		int nFind = sDomainName.Find("://");
		if (nFind>0)
		{
			sHttp = sDomainName.Left(nFind+3);	// http:// or https://
			sDomainName = sDomainName.Mid(nFind+3);
			nFind = sDomainName.Find("/");
			if (nFind>0)
			{
				sDomainName = sDomainName.Mid(0,nFind);
			}
			CString sFaviconFile;
			sFaviconFile.Format(_T("%s\\favicon-%s.ico"),theApp.GetAppImgTempPath(),sDomainName);
			if (PathFileExists(sFaviconFile))
			{
				USES_CONVERSION;
				pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sFaviconFile));
				return true;
			}
		}
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
	{
		pImage1 = m_imageDefaultOrg->Clone();
		return true;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		return false;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
	{
		CEBString sImagePath;
		bool bIsMemberAccount = false;
		EB_USER_LINE_STATE pOutLineState = GetAuthContact()?(EB_USER_LINE_STATE)pTreeItemInfo->m_dwItemData:EB_LINE_STATE_ONLINE_NEW;
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_MemberInfo> pMemberInfo;
		if (pTreeItemInfo->m_nUserId>0)
			pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(pTreeItemInfo->m_nUserId);
		if (pMemberInfo == NULL)
			pMemberInfo = theEBClientCore->EB_GetMemberInfoByAccount2(pTreeItemInfo->m_sAccount.c_str());
		if (pMemberInfo != NULL)
		{
			bIsMemberAccount = true;
			if (GetAuthContact())
			//if (!GetAuthContact())
				pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
			const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
			if (PathFileExists(sMemberHeadFile.c_str()))
			{
				sImagePath = sMemberHeadFile;
			}
		}
#else
		EB_MemberInfo pMemberInfo;
		bool bFind = false;
		if (pTreeItemInfo->m_nUserId>0)
			bFind = theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,pTreeItemInfo->m_nUserId);
		if (!bFind)
			bFind = theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,pTreeItemInfo->m_sAccount.c_str());
		if (bFind)
		{
			bIsMemberAccount = true;
			if (GetAuthContact())
			//if (!GetAuthContact())
				pOutLineState = pMemberInfo.m_nLineState;
			if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
			{
				sImagePath = pMemberInfo.m_sHeadResourceFile;
			}
		}
#endif
		if (bIsMemberAccount)
		{
			switch (pOutLineState)
			{
			case EB_LINE_STATE_UNKNOWN:
			case EB_LINE_STATE_OFFLINE:
				pState = 0;
				break;
			case EB_LINE_STATE_ONLINE_NEW:
				break;
			case EB_LINE_STATE_BUSY:
				{
					pImage2 = m_imageStateBusy->Clone();
				}break;
			case EB_LINE_STATE_AWAY:
				{
					pImage2 = m_imageStateAway->Clone();
				}break;
			default:
				break;
			}
			if (!sImagePath.empty())
			{
				USES_CONVERSION;
				pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
			}else
			{
				pImage1 = m_imageDefaultContact->Clone();
				//sImagePath = (LPCTSTR)this->GetAppPath();
				//sImagePath.append("\\img\\h02.png");
				//USES_CONVERSION;
				//pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
				////pImage1 = m_imageDefaultMember->Clone();
			}
		}else
		{
			switch (pOutLineState)
			{
			case EB_LINE_STATE_UNKNOWN:
			case EB_LINE_STATE_OFFLINE:
				pState = 0;	// 用灰色
				break;
			default:
				break;
			}
			//pState = 0;	// 非成员，全部用灰色
			pImage1 = m_imageDefaultContact->Clone();
		}
		return true;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		//return false;
		CEBString sImagePath;
		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
		if (pMemberInfo != NULL)
		{
			pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
			//sDescription = pMemberInfo->Description.GetBSTR();
			const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
			if (PathFileExists(sMemberHeadFile.c_str()))
			{
				sImagePath = sMemberHeadFile;
			}
		}
#else
		EB_MemberInfo pMemberInfo;
		if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pTreeItemInfo->m_sMemberCode))
		{
			//sDescription = pMemberInfo.m_sDescription;
			pOutLineState = pMemberInfo.m_nLineState;
			if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
			{
				sImagePath = pMemberInfo.m_sHeadResourceFile;
			}
		}
#endif
		switch (pOutLineState)
		{
		case EB_LINE_STATE_UNKNOWN:
		case EB_LINE_STATE_OFFLINE:
			pState = 0;
			break;
		case EB_LINE_STATE_ONLINE_NEW:
			break;
		case EB_LINE_STATE_BUSY:
			{
				pImage2 = m_imageStateBusy->Clone();
			}break;
		case EB_LINE_STATE_AWAY:
			{
				pImage2 = m_imageStateAway->Clone();
			}break;
		default:
			break;
		}
		if (!sImagePath.empty())
		{
			USES_CONVERSION;
			pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
		}else
		{
			//sImagePath = (LPCTSTR)this->GetAppPath();
			//sImagePath.append("\\img\\entlogo.png");
			//USES_CONVERSION;
			//pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
			//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("PNG"),pImage1);
			pImage1 = m_imageDefaultMember->Clone();
			//return libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("png"),pImage1)?true:false;
		}
		return true;
	}
	return false;
}

bool CEBUIApp::GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
{
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	return pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR;
}

//CDlgVideoConference::pointer CEBUIApp::GetDlgVideoConference(const CEBCCallInfo::pointer& pEbCallInfo,bool bBuildNew)
//{
//	const eb::bigint sCallId = pEbCallInfo->m_pCallInfo.GetCallId();
//	CDlgVideoConference::pointer pDlgDialog;
//	if (!m_pVideoConferenceList.find(sCallId, pDlgDialog))
//	{
//		if (bBuildNew)
//		{
//			CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
//			pDlgDialog = CDlgVideoConference::create(pParent);
//			pDlgDialog->m_pCallInfo = pEbCallInfo->m_pCallInfo;
//			pDlgDialog->Create(CDlgVideoConference::IDD, pParent);
//			m_pVideoConferenceList.insert(sCallId, pDlgDialog);
//		}else
//		{
//			return pDlgDialog;	// NULL
//		}
//	}
//	pDlgDialog->ShowWindow(SW_SHOW);
//	return pDlgDialog;
//}

void CEBUIApp::SetTreeListColor(COLORREF nSelColor, COLORREF nHotColor, COLORREF nBoderColor)
{
	m_nTreeListSelColor = nSelColor;
	m_nTreeListHotColor = nHotColor;
	m_nTreeListBorderColor = nBoderColor;
	if (this->m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
	{
		m_pMainWnd->Invalidate();
	}
}
void CEBUIApp::SetMainColor(COLORREF v)
{
	if (m_nMainColor==v) return;
	m_nMainColor=v;
	float h,s,l;
	::RGBtoHSL(GetRValue(m_nMainColor), GetGValue(m_nMainColor), GetBValue(m_nMainColor),&h,&s,&l);
	m_nFuncColor = HSLtoRGB(h, s, theDefaultBgColorTitleLight1);

	m_nHotColor1 = HSLtoRGB(h, s, theDefaultBtnHotColorLight1);
	m_nHotColor2 = HSLtoRGB(h, s, theDefaultBtnHotColorLight2);
	m_nHotColor = theApp.GetColorGradient()?m_nHotColor1:m_nHotColor2;
	m_nNormalColor = ::HSLtoRGB(h, s, theDefaultColorNormalLight);
	m_nBgTitle0 = ::HSLtoRGB(h, s, theDefaultBgColorTitleLight0);
	m_nBgColor1 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight1);
	m_nBgColor2 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight2);
	m_nBgColor3 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight3);
	m_nOkColor0 = ::HSLtoRGB(h, s, theDefaultBtnOkColorLight0);
	m_nOkColor1 = ::HSLtoRGB(h, s, theDefaultBtnOkColorLight1);
	if (this->m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
	{
		m_pMainWnd->Invalidate();
	}
}

eb::bigint CEBUIApp::GetGroupMsgSugId(void)
{
	if (m_nGroupMsgSubId==0)
	{
#ifdef USES_EBCOM_TEST
		unsigned long pGroupMsgSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID);
		if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
		{
			m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
		}
#else
		unsigned long pGroupMsgSubId = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
		if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
		{
			m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
		}
#endif
	}
	return m_nGroupMsgSubId;
}
eb::bigint CEBUIApp::GetFindAppSugId(void)
{
	if (m_nFindAppSubId==0)
	{
#ifdef USES_EBCOM_TEST
		unsigned long pFindAppSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID);
		if (pFindAppSubId!=NULL && strlen((const char*)pFindAppSubId)>0)
		{
			m_nFindAppSubId = eb_atoi64((const char*)pFindAppSubId);
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID,pFindAppSubId);
		}
#else
		unsigned long pFindAppSubId = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID,&pFindAppSubId);
		if (pFindAppSubId!=NULL && strlen((const char*)pFindAppSubId)>0)
		{
			m_nFindAppSubId = eb_atoi64((const char*)pFindAppSubId);
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID,pFindAppSubId);
		}
#endif
	}
	return m_nFindAppSubId;
}

BOOL CEBUIApp::IM_OAuth(const char* lpszParameter)
{
	if (theEBAppClient.EB_IsLogoned())
		return TRUE;

#ifndef _USRDLL
	if (!InitInstance())
		return FALSE;
#endif

//
//	std::string sAppParameter0;
//	//if (__argc >= 2)
//	//{
//	//	sAppParameter0 = __argv[1];
//	//}
//	// **需要先设置一次
//#ifdef USES_EBCOM_TEST
//	this->SetMainColor(theEBSetting->DefaultColor);
//#else
//	this->SetMainColor(theSetting.GetDefaultColor());
//#endif
//
	CDlgOAuth pDlgLogin;
	if (lpszParameter!=NULL)
		pDlgLogin.m_sAppParameter0 = lpszParameter;
	pDlgLogin.DoModal();

	this->m_sAccountPrefix = pDlgLogin.GetAccountPrefix();
	this->m_sProductName = pDlgLogin.GetProductName();
	this->m_bLicenseUser = pDlgLogin.GetLicenseUser();
	this->m_bSendRegMail = pDlgLogin.GetSendRegMail();
	this->m_bSaveConversationLocal = pDlgLogin.GetSaveConversationLocal();
	this->m_bSaveConversationServer = pDlgLogin.GetSaveConversationServer();
	this->m_bAuthContact = pDlgLogin.GetAuthContact();
	this->m_nDeployId = pDlgLogin.GetDeployId();
	//this->m_nGroupMsgSubId = pDlgLogin.GetGroupMsgSugId();
	this->m_nAutoOpenSubId = 0;//pDlgLogin.GetAutoOpenSubId();
	this->m_bAutoHideMainFrame = false;//pDlgLogin.GetAutoHideMainFrame();
	this->m_bHideMainFrame = false;//pDlgLogin.GetHideMainFrame();
	this->m_bDisableUserCloudDrive = pDlgLogin.GetDisableUserCloudDrive();
	this->m_bDisableGroupSharedCloud = pDlgLogin.GetDisableGroupSharedCloud();
	this->m_bDisableModifyPasswd = pDlgLogin.GetDisableModifyPasswd();
	this->m_sEntManagerUrl = pDlgLogin.GetEntManagerurl();
	this->m_bDisableVideo = pDlgLogin.GetDisableVideo();
	this->m_bDisableRemoteDesktop = pDlgLogin.GetDisableRD();

	// ??
	//m_nAutoOpenSubId = 1002300102;	// 找群找人
	//m_bAutoHideMainFrame = true;
	//m_bHideMainFrame = true;
	return IMInitData();
}
#include "DlgLogin.h"
BOOL CEBUIApp::IM_Login(const char* lpszAccount, const char* lpszPasswd)
{
	if (theEBAppClient.EB_IsLogoned())
	{
		if (theApp.m_pHandler!=NULL)
			theApp.m_pHandler->OnLogonSuccess(theApp.GetLogonUserId(),theApp.GetLogonAccount());
		return TRUE;
	}

	if (lpszAccount==NULL || strlen(lpszAccount)==0 || lpszPasswd==NULL || strlen(lpszPasswd)==0)
	{
		if (theApp.m_pHandler!=NULL)
			theApp.m_pHandler->OnLogonError(EB_STATE_PARAMETER_ERROR);
		return FALSE;
	}
#ifndef _USRDLL
	if (!InitInstance())
		return FALSE;
#endif

//
//	std::string sAppParameter0;
//	//if (__argc >= 2)
//	//{
//	//	sAppParameter0 = __argv[1];
//	//}
//	// **需要先设置一次
//#ifdef USES_EBCOM_TEST
//	this->SetMainColor(theEBSetting->DefaultColor);
//#else
//	this->SetMainColor(theSetting.GetDefaultColor());
//#endif
//
//	CDlgOAuth pDlgLogin;
//	pDlgLogin.m_sAppParameter0 = sAppParameter0;
//	pDlgLogin.DoModal();
	CString sBoPath = theApp.GetAppDataPath() + _T("\\bodb");
	m_pBoEB = bo::bodb_init(sBoPath);
	if (m_pBoEB.get() == NULL)
	{
		if (theApp.m_pHandler!=NULL)
			theApp.m_pHandler->OnLogonError(EB_STATE_ERROR,"安装目录损坏，请重新安装！");
		else
			CDlgMessageBox::EbDoModal(NULL,_T("安装目录损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
		return FALSE;
	}
	m_pBoEB->setaccount("system","");
	if (!m_pBoEB->use("eb"))
	{
		if (theApp.m_pHandler!=NULL)
			theApp.m_pHandler->OnLogonError(EB_STATE_ERROR,"配置文件损坏，请重新安装！");
		else
			CDlgMessageBox::EbDoModal(NULL,_T("配置文件损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
		return FALSE;
	}

	CDlgLogin pDlgLogin;
	pDlgLogin.SetAutoLogon(lpszAccount,lpszPasswd);
	pDlgLogin.Create(CDlgLogin::IDD);
	pDlgLogin.ShowWindow(SW_HIDE);
	pDlgLogin.RunModalLoop();
	pDlgLogin.DestroyWindow();

	//this->m_sAccountPrefix = pDlgLogin.GetAccountPrefix();
	//this->m_sProductName = pDlgLogin.GetProductName();
	//this->m_bLicenseUser = pDlgLogin.GetLicenseUser();
	//this->m_bSendRegMail = pDlgLogin.GetSendRegMail();
	//this->m_bSaveConversationLocal = pDlgLogin.GetSaveConversationLocal();
	//this->m_bSaveConversationServer = pDlgLogin.GetSaveConversationServer();
	//this->m_bAuthContact = pDlgLogin.GetAuthContact();
	//this->m_nDeployId = pDlgLogin.GetDeployId();
	//this->m_nGroupMsgSubId = pDlgLogin.GetGroupMsgSugId();
	this->m_nAutoOpenSubId = 0;//pDlgLogin.GetAutoOpenSubId();
	this->m_bAutoHideMainFrame = false;//pDlgLogin.GetAutoHideMainFrame();
	this->m_bHideMainFrame = false;//pDlgLogin.GetHideMainFrame();
	//this->m_bDisableUserCloudDrive = pDlgLogin.GetDisableUserCloudDrive();
	//this->m_bDisableGroupSharedCloud = pDlgLogin.GetDisableGroupSharedCloud();
	//this->m_bDisableModifyPasswd = pDlgLogin.GetDisableModifyPasswd();
	//this->m_sEntManagerUrl = pDlgLogin.GetEntManagerurl();
	//this->m_bDisableVideo = pDlgLogin.GetDisableVideo();
	//this->m_bDisableRemoteDesktop = pDlgLogin.GetDisableRD();

	// ??
	//m_nAutoOpenSubId = 1002300102;	// 找群找人
	//m_bAutoHideMainFrame = true;
	//m_bHideMainFrame = true;
	return IMInitData();
}

BOOL CEBUIApp::IMInitData(void)
{
	const bool bIsVisitor = theApp.IsLogonVisitor();
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_IsLogoned == VARIANT_FALSE)
	{
		return FALSE;
	}
	AdaptIeVersion();
	m_sLogonAccount = CEBString(theEBClientCore->EB_LogonAccount.GetBSTR()).c_str();
	m_nLogonUserId = theEBClientCore->EB_LogonUserId;
	m_sLogonPassword = CEBString(theEBClientCore->EB_Password.GetBSTR()).c_str();
#else
	if (!theEBAppClient.EB_IsLogoned())
	{
		return FALSE;
	}
	AdaptIeVersion();

	m_sLogonAccount = theEBAppClient.EB_GetLogonAccount().c_str();
	m_nLogonUserId = theEBAppClient.EB_GetLogonUserId();
	m_sLogonPassword = theEBAppClient.EB_GetPassword().c_str();
	//bIsVisitor = theEBAppClient.EB_IsLogonVisitor();
#endif

	// ??? 1.16 删除无用目录数据文件，2.0以后删除下面代码；
	CString sDelPath = theApp.GetAppDataPath() + _T("\\bodb\\ebdatas");
	if (::PathFileExists(sDelPath))
	{
		DeleteFile(sDelPath+_T("\\ebdatas.bdf"));
		RemoveDirectory(sDelPath);
	}
	sDelPath = theApp.GetAppDataPath() + _T("\\bodb\\ebuser");
	if (::PathFileExists(sDelPath))
	{
		DeleteFile(sDelPath+_T("\\ebuser.bdf"));
		RemoveDirectory(sDelPath);
	}

	if (m_pBoEB.get()==NULL)
	{
		CString sBoPath = theApp.GetAppDataPath() + _T("\\bodb");
		m_pBoEB = bo::bodb_init(sBoPath);
		if (m_pBoEB.get() == NULL)
		{
			CDlgMessageBox::EbDoModal(NULL,_T("安装目录损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
			return FALSE;
		}
		m_pBoEB->setaccount("system","");
		if (!m_pBoEB->use("eb"))
		{
			CDlgMessageBox::EbDoModal(NULL,_T("配置文件损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
			return FALSE;
		}
	}
	// 检查默认主色调
	bo::PRESULTSET pResltSet = NULL;
	m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='main-color'", &pResltSet);
	if (pResltSet != NULL && pResltSet->rscount>0)
	{
		this->SetMainColor(pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal);
	}else
	{
#ifdef USES_EBCOM_TEST
		this->SetMainColor(theEBSetting->DefaultColor);
#else
		this->SetMainColor(theSetting.GetDefaultColor());
#endif
		CString sSql;
		sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('main-color',%d)"),(DWORD)m_nMainColor);
		m_pBoEB->execsql(sSql);
	}
	bo::bodb_free(pResltSet);
	// 检查渐变效果
	pResltSet = NULL;
	m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='color-gradient'", &pResltSet);
	if (pResltSet != NULL && pResltSet->rscount>0)
	{
		this->SetColorGradient(pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal==1?true:false);
	}else
	{
		this->SetColorGradient(false);
		CString sSql;
		sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('color-gradient',0)"));
		m_pBoEB->execsql(sSql);
	}
	bo::bodb_free(pResltSet);
	m_pBoEB->close();
	m_sUserMainPath = m_sAppPath + _T("\\users");

	// ****************
	//m_iconAway = LoadIcon(IDI_ICON_AWAY);
	//m_iconBusy = LoadIcon(IDI_ICON_BUSY);
	//m_iconEnterprise = LoadIcon(IDI_ENTERPRISE);
	//m_iconDepartment = LoadIcon(IDI_DEPARTMENT);
	//m_iconContact = LoadIcon(IDI_CONTACT);

	//if (pDlgLogin.m_bAutoLogin && !bIsVisitor)
	//{
	//	// 普通用户，要求设置自动登录
	//	theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),m_sLogonAccount);
	//}else if (!pDlgLogin.m_bAutoLogin)
	//{
	//	theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),_T(""));
	//}

	if (bIsVisitor)
	{
		m_sUserMainPath += _T("\\visitor");
	}else
	{
		m_sUserMainPath = m_sUserMainPath + _T("\\") + m_sLogonAccount;
	}
	if (!::PathFileExists(m_sUserMainPath))
	{
		::CreateDirectory(m_sUserMainPath, NULL);
	}
	m_sUserImagePath = m_sUserMainPath + _T("\\image");
	if (!::PathFileExists(m_sUserImagePath))
	{
		::CreateDirectory(m_sUserImagePath, NULL);
	}
	m_sUserFilePath = m_sUserMainPath + _T("\\file");
	if (!::PathFileExists(m_sUserFilePath))
	{
		::CreateDirectory(m_sUserFilePath, NULL);
	}
	ReadUserSetting();

	CString sBoPath;
	sBoPath.Format(_T("%s\\%lld"),m_sUserMainPath,this->GetDeployId());
	if (!::PathFileExists(sBoPath))
		::CreateDirectory(sBoPath, NULL);
	sBoPath = sBoPath + _T("\\bodb");
	//sBoPath = m_sUserMainPath + _T("\\bodb");
	const std::string sDefaultUserBoFile = GetAppDataPath()+_T("\\bodb\\userdatas");
	const std::string sUserBoFile = sBoPath+_T("\\userdatas");
	if (!::PathFileExists(sBoPath))
	{
		::CreateDirectory(sBoPath, NULL);
		CopyFile(sDefaultUserBoFile.c_str(), sUserBoFile.c_str(), TRUE);
	}else if (EbGetFileSize(sUserBoFile.c_str())<=0)
	//}else if (!::PathFileExists(sUserBoFile.c_str()))
	{
		CopyFile(sDefaultUserBoFile.c_str(), sUserBoFile.c_str(), FALSE);	// TRUE
	}
	m_pBoUsers = CSqliteCdbc::create();
	if (!m_pBoUsers->open(libEbc::ACP2UTF8(sUserBoFile.c_str()).c_str()))
	{
		m_pBoUsers.reset();
		CDlgMessageBox::EbDoModal(NULL,_T("安装目录损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
		return FALSE;
	}
	if (m_pBoUsers->select("SELECT create_time FROM url_record_t limit 1")==-1)
	{
		const std::string sCreateSql = "CREATE TABLE url_record_t"\
			"("\
			"url VARCHAR(1024) DEFAULT '',"\
			"title VARCHAR(260) DEFAULT '',"\
			"create_time		TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"\
			"last_time		TIMESTAMP DEFAULT CURRENT_TIMESTAMP"\
			");"\
			"CREATE INDEX url_record_t_idx_url ON url_record_t(url);"\
			"CREATE INDEX url_record_t_last_time ON url_record_t(last_time);";
		m_pBoUsers->execute(sCreateSql.c_str());
	}

	// ??? 1.16 同步旧数据，1.20以后版本删除；
	// ** 判断是否存在旧数据，如果有，一次性同步数据，然后删除旧数据文件；
	CString sOldUserBoPath = m_sUserMainPath+_T("\\bodb\\ebuser");
	if (::PathFileExists(sOldUserBoPath))
	{
		// **存在旧数据，同步旧数据
		const CString sOldBoPath = m_sUserMainPath+_T("\\bodb");
		bo::CBodbHandler::pointer pBoUsers = bo::bodb_init(sOldBoPath);
		if (pBoUsers.get() != NULL)
		{
			try
			{
				pBoUsers->setaccount("system","");
				if (pBoUsers->use("ebuser"))
				{
					// ** 同步数据
					CString sSql = _T("SELECT call_id,call_time,dep_code,dep_name,emp_code,from_uid,from_phone,from_account,from_name,from_type,company,title,tel,email FROM call_record_t LIMIT 50");
					bo::PRESULTSET pResltSet = NULL;
					pBoUsers->execsql(sSql, &pResltSet);
					if (pResltSet != NULL)
					{
						for (int i=0; i<pResltSet->rscount; i++)
						{
							mycp::bigint sCallId = pResltSet->rsvalues[i]->fieldvalues[0]->v.bigintVal;
							time_t m_tTime = pResltSet->rsvalues[i]->fieldvalues[1]->v.timestampVal.time;
							m_tTime += (pResltSet->rsvalues[i]->fieldvalues[1]->v.timestampVal.timezone*60);

							CTime pTime(m_tTime);
							CString sTime = pTime.Format(_T("%Y-%m-%d %H:%M:%S"));

							mycp::bigint sGroupCode = pResltSet->rsvalues[i]->fieldvalues[2]->v.bigintVal;
							//pCallRecordInfo->m_sGroupCode = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[2]->v.varcharVal);
							tstring sDepName = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[3]->v.varcharVal);
							mycp::bigint sEmpCode = pResltSet->rsvalues[i]->fieldvalues[4]->v.bigintVal;
							//pCallRecordInfo->m_sMemberCode = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[4]->v.varcharVal);
							mycp::bigint m_nFromUserId = pResltSet->rsvalues[i]->fieldvalues[5]->v.bigintVal;
							mycp::bigint sPhone = pResltSet->rsvalues[i]->fieldvalues[6]->v.bigintVal;
							tstring m_sFromAccount = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[7]->v.varcharVal);
							tstring sFromName = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[8]->v.varcharVal);
							int nFromType = pResltSet->rsvalues[i]->fieldvalues[9]->v.tinyintVal;
							tstring sCompany = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[10]->v.varcharVal);
							tstring sTitle = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[11]->v.varcharVal);
							//pCallRecordInfo->m_sPhone = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[12]->v.varcharVal);
							tstring sTel = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[12]->v.varcharVal);
							tstring sEmail = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[13]->v.varcharVal);

							sSql.Format(_T("INSERT INTO call_record_t(call_id,call_time,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,from_type,company,title,tel,email) ")\
								_T("VALUES(%lld,'%s',%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')"),
								sCallId,sTime,sGroupCode,libEbc::ACP2UTF8(sDepName.c_str()).c_str(),sEmpCode,m_nFromUserId,sPhone,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),nFromType,
								libEbc::ACP2UTF8(sCompany.c_str()).c_str(),libEbc::ACP2UTF8(sTitle.c_str()).c_str(),sTel.c_str(),sEmail.c_str());
							theApp.m_pBoUsers->execute(sSql);
						}
						bo::bodb_free(pResltSet);
						pResltSet = NULL;
					}
					sSql = _T("select msg_time,msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,dep_code FROM msg_record_t");
					pBoUsers->execsql(sSql, &pResltSet);
					if (pResltSet != NULL)
					{
						for (int i=0; i<pResltSet->rscount; i++)
						{
							time_t nMsgTime = pResltSet->rsvalues[i]->fieldvalues[0]->v.timestampVal.time;
							const short nTimezone = pResltSet->rsvalues[i]->fieldvalues[0]->v.timestampVal.timezone;
							nMsgTime += (nTimezone*60);
							CTime pTime(nMsgTime);
							CString sTime = pTime.Format(_T("%Y-%m-%d %H:%M:%S"));

							const eb::bigint sMsgId = pResltSet->rsvalues[i]->fieldvalues[1]->v.bigintVal;
							const tstring soffTime = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[2]->v.varcharVal);
							const eb::bigint sFromAccount = pResltSet->rsvalues[i]->fieldvalues[3]->v.bigintVal;
							const tstring sFromName = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[4]->v.varcharVal);
							const eb::bigint sToAccount = pResltSet->rsvalues[i]->fieldvalues[5]->v.bigintVal;
							const tstring sToName = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[6]->v.varcharVal);
							const int nPrivate = pResltSet->rsvalues[i]->fieldvalues[7]->v.tinyintVal;
							const int nMsgType = pResltSet->rsvalues[i]->fieldvalues[8]->v.tinyintVal;
							const tstring sMsgName = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[9]->v.varcharVal);
							tstring sMsgText = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[10]->v.varcharVal);
							if (MRT_TEXT==nMsgType)
							{
								bo::bodb_escape_string_out(sMsgText);
								CSqliteCdbc::escape_string_in(sMsgText);
							}
							const eb::bigint sGroupCode = pResltSet->rsvalues[i]->fieldvalues[11]->v.bigintVal;

							sSql.Format(_T("INSERT INTO msg_record_t(msg_time,msg_id,off_time,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text) ")\
								_T("VALUES('%s',%lld,'%s',%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s')"),
								sTime,sMsgId,soffTime.c_str(),sGroupCode,sFromAccount,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),sToAccount,
								libEbc::ACP2UTF8(sToName.c_str()).c_str(),nPrivate,nMsgType,libEbc::ACP2UTF8(sMsgName.c_str()).c_str(),libEbc::ACP2UTF8(sMsgText.c_str()).c_str());
							theApp.m_pBoUsers->execute(sSql);
						}

						bo::bodb_free(pResltSet);
						pResltSet = NULL;
					}
				}
			}catch(std::exception&)
			{
			}catch(...)
			{
			}
			pBoUsers->close();
			bo::bodb_exit(pBoUsers);
			pBoUsers.reset();
		}
		DeleteFile(sOldUserBoPath+_T("\\ebuser.bdf"));
		DeleteFile(sOldUserBoPath+_T("\\ebuser.bdf.bk"));
		RemoveDirectory(sOldUserBoPath);
		sOldUserBoPath = m_sUserMainPath+_T("\\bodb");
		RemoveDirectory(sOldUserBoPath);
	}else
	{
		// ???同步 1.15旧用户数据；1.17以后版本删除；
		sOldUserBoPath = m_sUserMainPath+_T("\\bodb\\userdatas");
		if (::PathFileExists(sOldUserBoPath))
		{
			// 
			CSqliteCdbc pOldSqlite;
			if (pOldSqlite.open(libEbc::ACP2UTF8(sOldUserBoPath).c_str()))
			{
				CString sSql = _T("SELECT call_id,Datetime(call_time,'localtime'),dep_code,dep_name,emp_code,from_uid,from_phone,from_account,from_name,from_type,company,title,tel,email FROM call_record_t LIMIT 50");
				int nCookie = 0;
				pOldSqlite.select(sSql, nCookie);
				cgcValueInfo::pointer pRecord = pOldSqlite.first(nCookie);
				while (pRecord.get()!=NULL)
				{
					const mycp::bigint sCallId = pRecord->getVector()[0]->getBigIntValue();
					const tstring sCallTime = pRecord->getVector()[1]->getStrValue();
					const mycp::bigint sGroupCode = pRecord->getVector()[2]->getBigIntValue();
					const tstring sGroupName = pRecord->getVector()[3]->getStrValue();
					const mycp::bigint sEmpCode = pRecord->getVector()[4]->getBigIntValue();
					const mycp::bigint m_nFromUserId = pRecord->getVector()[5]->getBigIntValue();
					const mycp::bigint sPhone = pRecord->getVector()[6]->getBigIntValue();
					//pCallRecordInfo->m_sFromAccount = pRecord->getVector()[7]->getStrValue();
					const tstring sFromName = pRecord->getVector()[8]->getStrValue();
					const int nFromType = pRecord->getVector()[9]->getIntValue();
					const tstring sCompany = pRecord->getVector()[10]->getStrValue();
					const tstring sTitle = pRecord->getVector()[11]->getStrValue();
					const tstring sTel = pRecord->getVector()[12]->getStrValue();
					const tstring sEmail = pRecord->getVector()[13]->getStrValue();

					sSql.Format(_T("INSERT INTO call_record_t(call_id,call_time,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,from_type,company,title,tel,email) ")\
						_T("VALUES(%lld,'%s',%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')"),
						sCallId,sCallTime.c_str(),sGroupCode,sGroupName.c_str(),sEmpCode,m_nFromUserId,sPhone,sFromName.c_str(),nFromType,
						sCompany.c_str(),sTitle.c_str(),sTel.c_str(),sEmail.c_str());
					theApp.m_pBoUsers->execute(sSql);
					pRecord = pOldSqlite.next(nCookie);
				}
				pOldSqlite.reset(nCookie);

				sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,dep_code FROM msg_record_t"));
				pOldSqlite.select(sSql, nCookie);
				pRecord = pOldSqlite.first(nCookie);
				while (pRecord.get()!=NULL)
				{
					const tstring sTime = pRecord->getVector()[0]->getStrValue();
					const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
					const tstring soffTime = pRecord->getVector()[2]->getStrValue();
					const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
					const tstring sFromName(pRecord->getVector()[4]->getStrValue());
					const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
					const tstring sToName(pRecord->getVector()[6]->getStrValue());
					const int nPrivate = pRecord->getVector()[7]->getIntValue();
					const int nMsgType = pRecord->getVector()[8]->getIntValue();
					const tstring sMsgName(pRecord->getVector()[9]->getStrValue());
					const tstring sMsgText(pRecord->getVector()[10]->getStrValue());
					const eb::bigint sGroupCode = pRecord->getVector()[11]->getBigIntValue();

					sSql.Format(_T("INSERT INTO msg_record_t(msg_time,msg_id,off_time,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text) ")\
						_T("VALUES('%s',%lld,'%s',%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s')"),
						sTime.c_str(),sMsgId,soffTime.c_str(),sGroupCode,sFromAccount,sFromName.c_str(),sToAccount,
						sToName.c_str(),nPrivate,nMsgType,sMsgName.c_str(),sMsgText.c_str());
					theApp.m_pBoUsers->execute(sSql);

					pRecord = pOldSqlite.next(nCookie);
				}
				pOldSqlite.reset(nCookie);

				pOldSqlite.close();
			}
			DeleteFile(sOldUserBoPath);
			sOldUserBoPath = m_sUserMainPath+_T("\\bodb");
			RemoveDirectory(sOldUserBoPath);
		}
	}
	if (theApp.m_pHandler!=NULL)
		theApp.m_pHandler->OnLogonSuccess(theApp.GetLogonUserId(),theApp.GetLogonAccount());
	return TRUE;
}
//void CEBUIApp::PostEbuiMessage(WPARAM wParam, LPARAM lParam)
//{
//	//if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
//	//{
//	//	m_pMainWnd->PostMessage(m_nMsgId, wParam, lParam);
//	//}
//}

BOOL CEBUIApp::IM_IsLogined(void) const
{
	return theEBAppClient.EB_IsLogoned()?TRUE:FALSE;
}

void CEBUIApp::IM_Logout(void)
{
	MainFrame_Destroy();
#ifndef _USRDLL
	ExitInstance();
#endif
}

BOOL CEBUIApp::MainFrame_Create(HWND hParentHwnd, UINT nMsgId)
{
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_IsLogoned == VARIANT_FALSE)
	{
		return FALSE;
	}
#else
	if (!theEBAppClient.EB_IsLogoned())
	{
		return FALSE;
	}
#endif
	if (m_pMainWnd==NULL)
	{
		//AFX_SWITCH_INSTANCED();
		CWnd* hParentWnd = CWnd::FromHandle(hParentHwnd);
		m_pMainWnd = new CDlgMainFrame(hParentWnd);
		if (!m_pMainWnd->Create(CDlgMainFrame::IDD, hParentWnd))
		{
			delete m_pMainWnd;
			m_pMainWnd = NULL;
			return FALSE;
		}
	}
	this->m_pMainWnd->SetMsgId(nMsgId);
	return TRUE;
}
HWND CEBUIApp::MainFrame_GetSafeHwnd(void) const
{
	return (m_pMainWnd==NULL)?NULL:m_pMainWnd->GetSafeHwnd();
}
void CEBUIApp::MainFrame_Destroy(void)
{
	if (m_pMainWnd!=NULL)
	{
		if (m_pMainWnd->GetSafeHwnd()!=NULL)
			m_pMainWnd->DestroyWindow();
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}
}
void CEBUIApp::MainFrame_ShowWindow(int nShow)
{
	if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
		m_pMainWnd->ShowWindow(nShow);
}
void CEBUIApp::MainFrame_MoveWindow(int x, int y, int w, int h, BOOL bRepaint)
{
	if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
		m_pMainWnd->MoveWindow(x, y, w, h, bRepaint);
}
void CEBUIApp::MainFrame_MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
	if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
		m_pMainWnd->MoveWindow(lpRect, bRepaint);
}
void CEBUIApp::MainFrame_Invalidate(BOOL bErase)
{
	if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
		m_pMainWnd->Invalidate(bErase);
}
void CEBUIApp::ChatWindow_SetTopMost(bool bTopMost)
{
	m_bTopMost = bTopMost;
	if (m_pMainWnd!=NULL && m_pMainWnd->GetSafeHwnd()!=NULL)
		m_pMainWnd->SetChatWindowTopMost(m_bTopMost);
}
