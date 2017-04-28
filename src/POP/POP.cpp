// POP.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "POP.h"
#include "POPDlg.h"
//#include "DlgLogin.h"
#include "DlgOAuth.h"
#include "DlgGroupInfo.h"
#include "DlgMemberInfo.h"
#include "DlgFuncWindow.h"
//#include <excpt.h>
//#include <boost/exception/all.hpp>
#include "ExceptHandler.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

bool theWantExitApp = false;

// /FORCE:MULTIPLE
#ifndef USES_EBCOM_TEST
#ifdef _DEBUG
//#pragma comment(lib, "videoroomd.lib")
//#pragma comment(lib, "ustrmbas9d.lib")
#pragma comment(lib, "libebumd.lib")
#else
//#pragma comment(lib, "videoroom.lib")
//#pragma comment(lib, "USTRMBASE9.lib")
#pragma comment(lib, "libebum.lib")
#endif
#endif

HICON theDefaultDirIcon = NULL;
CLockMap<tstring,HICON> theFileHIcon;				// ext->

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

// CPOPApp

BEGIN_MESSAGE_MAP(CPOPApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_HELP, &CPOPApp::OnAbout)
END_MESSAGE_MAP()

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
CString GetGroupManagerText(EB_GROUP_TYPE nGroupType)
{
	switch (nGroupType)
	{
	case EB_GROUP_TYPE_DEPARTMENT:
		return _T("部门经理");
	case EB_GROUP_TYPE_PROJECT:
		return _T("项目经理");
	case EB_GROUP_TYPE_GROUP:
		return _T("群主");
	default:
		return _T("讨论组长");
	}
}
#include <direct.h>
#include <io.h>
int copyDir(const char * pSrc,const char *pDes)
{
	if (NULL == pSrc || NULL == pDes)	return -1;
	mkdir(pDes);
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};

	char *str = "\\*.*";
	strcpy(dir,pSrc);
	strcat(dir,str);

	//首先查找dir中符合要求的文件
	long hFile = 0;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy(srcFileName,pSrc);
			strcat(srcFileName,"\\");
			strcat(srcFileName,fileinfo.name);

			strcpy(desFileName,pDes);
			strcat(desFileName,"\\");
			strcat(desFileName,fileinfo.name);

			//检查是不是目录
			//如果不是目录,则进行处理文件夹下面的文件
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				if (!CopyFile(srcFileName,desFileName,FALSE))
				{
					_findclose(hFile);
					return -1;
				}
				//copyFile(srcFileName,desFileName);
			}
			else//处理目录，递归调用
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
				{
					//if (!::PathFileExists(desFileName))
					//	::CreateDirectory(desFileName,NULL);

					if (copyDir(srcFileName,desFileName)!=1)
					{
						_findclose(hFile);
						return -2;
					}
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		return 1;
	}
	return -3;
}
int deleteDir(const char * pSrc)
{
	if (NULL == pSrc)	return -1;
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};

	char *str = "\\*.*";
	strcpy(dir,pSrc);
	strcat(dir,str);

	//首先查找dir中符合要求的文件
	long hFile = 0;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy(srcFileName,pSrc);
			strcat(srcFileName,"\\");
			strcat(srcFileName,fileinfo.name);

			//检查是不是目录
			//如果不是目录,则进行处理文件夹下面的文件
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				DeleteFile(srcFileName);
			}
			else//处理目录，递归调用
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
				{
					deleteDir(srcFileName);
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		RemoveDirectory(pSrc);
		return 1;
	}
	return -3;
}
#include <afxinet.h>
bool DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified)
{
	char     *pBuf   = NULL ;
	int      nBufLen = 0 ;
	TRY
	{
		DWORD dwServiceType;
		CString strServer;
		CString strObject;
		INTERNET_PORT nPort;
		BOOL bParsed = AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);
		if (!bParsed) return false;
		tstring sIp;
		for (int i=0;i<3; i++)
		{
			sIp = libEbc::GetHostIp(strServer,"");
			if (!sIp.empty())
				break;
#ifdef WIN32
			Sleep(50);
#else
			usleep(50000);
#endif
		}
		if (sIp.empty()) return false;

		// connection
		CInternetSession   sess ;
		sess.SetOption (INTERNET_OPTION_CONNECT_TIMEOUT, 10 * 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_BACKOFF, 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_RETRIES, 1) ;

		DWORD       dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;
		CHttpFile   * pF = (CHttpFile*)sess.OpenURL(strUrl, 1, dwFlag);
		ASSERT(pF);
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
		if (!strOldLastModified.IsEmpty() && strOldLastModified==pOutNewLastModified)
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
			while (true)
			{
				// download 8K every
				//DWORD tickNow = GetTickCount();
				int   n = pF->Read (p, (nLen < 2*1024) ? nLen : 2*1024) ;
				if (n <= 0)
					break ; // success exit
				p += n ; nLen -= n ;
			}

			// interrupted
			if (nLen != 0)
			{
				delete[] pBuf; pBuf=NULL;
				nBufLen = 0 ;
			}
		}else
		{
			// don't know file's size, save context to a temp file.
			FILE * f = NULL;
			int      n = (int)pF->GetLength() ;
			while (n>0)
				//while (IsWindow(this->GetSafeHwnd()) && n>0)
			{
				char   * pa = new char[n+1];
				n = pF->Read (pa, n) ;
				if (f==NULL)
				{
					f = fopen(strFile,"wb");
					if (f==NULL)
					{
						delete[] pa ;
						break;
					}
				}
				fwrite(pa,1,n,f);
				n = (int)pF->GetLength() ;
				delete[] pa ;
			}
			if (f!=NULL)
			{
				fclose(f);
				pF->Close();
				delete pF ;
				return true;
			}
		}
		/*outFile.Close() ;

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
HRESULT GetPostData(LPCTSTR cszPostData,LPVARIANT pvPostData)
{
	HRESULT hr;
	LPSAFEARRAY psa;
	//LPCTSTR cszPostData = "FName=Matt&Flavor=Mocha+Chip";
	UINT cElems = lstrlen(cszPostData);
	LPSTR pPostData;

	if (!pvPostData)
	{
		return E_POINTER;
	}

	VariantInit(pvPostData);
	psa = SafeArrayCreateVector(VT_UI1, 0, cElems);
	if (!psa)
	{
		return E_OUTOFMEMORY;
	}

	hr = SafeArrayAccessData(psa, (LPVOID*)&pPostData);
	memcpy(pPostData, cszPostData, cElems);
	hr = SafeArrayUnaccessData(psa);

	V_VT(pvPostData) = VT_ARRAY | VT_UI1;

	V_ARRAY(pvPostData) = psa;
	return NOERROR;
}

// CPOPApp 构造

//#define _CRTDBG_MAP_ALLOC
//#include "stdlib.h"
//#include "crtdbg.h"
#ifdef _DEBUG
int Atest(int a) {return a+1;}
#endif

CPOPApp::CPOPApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	CString str;
	GetModuleFileName( NULL, str.GetBuffer(MAX_PATH), MAX_PATH);
	str.ReleaseBuffer( -1);
	const int nFind = str.ReverseFind('\\');
	m_sAppPath = str.Mid(0, nFind);
	m_sAppDataPath = m_sAppPath + _T("\\datas");
	m_sAppName = str.Mid(nFind+1);
	//m_sCefCachePath = m_sAppPath + _T("\\cef_cache_path");
	// app data path
	char lpszBuffer[MAX_PATH];
	SHGetSpecialFolderPath(::GetDesktopWindow(),lpszBuffer,CSIDL_APPDATA,FALSE);
#ifdef USES_LIBCEF_USER_CACHE
	m_sCefCachePath.Format(_T("%s\\entboost\\cef_cache_temp"),lpszBuffer);
	if (!::PathFileExists(m_sCefCachePath))
		CreateDirectory(m_sCefCachePath,NULL);	
#else
	m_sCefCachePath.Format(_T("%s\\entboost\\cef_cache_path"),lpszBuffer);
#endif
	const tstring sSettingIniFile = m_sAppDataPath+_T("\\setting.ini");
	m_bLogDebug = GetPrivateProfileInt("system","debug",0,sSettingIniFile.c_str())==1?true:false;
	m_pLogDebug = NULL;

	m_nLogonUserId = 0;
	m_nSystemAccountFlag = 0;
	m_bLicenseUser = false;
	m_bSendRegMail = false;
	m_bSaveConversationLocal = true;
	m_bSaveConversationServer = true;
	m_bAuthContact = false;
	m_nDeployId = 0;
	m_nLicenstType = 0;
	m_nEBServerVersion = 0;
	m_nGroupMsgSubId = 0;
	m_nFindAppSubId = 0;
	m_nMyCollectionSubId = 0;
	m_nAutoOpenSubId = 0;
	m_bAutoHideMainFrame = false;
	m_bHideMainFrame = false;
	m_bDisableUserCloudDrive = false;
	m_bDisableGroupSharedCloud = false;
	m_bDisableModifyPasswd = false;
	m_bDisableVideo = false;
	m_bDisableRemoteDesktop = false;
	m_bDisableAccountEdit = false;
	m_nEnterpriseCreateUserId = 0;
	m_bDisableMsgReceipt = false;
	m_bStatSubGroupMember = false;

#ifdef USES_LIBCEF
	m_nDefaultBrowserType = EB_BROWSER_TYPE_CEF;
#else
	m_nDefaultBrowserType = EB_BROWSER_TYPE_IE;
#endif
	m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_OFFICE;//EB_UI_STYLE_TYPE_CHAT;
	m_nMinEBSC = false;
	m_bIeException = false;

	m_imageExBtnSendECard = NULL;
	m_imageWorkFrame = NULL;
	m_imageMainFrame = NULL;
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
	m_imageStateForbid = NULL;

	m_pDlgEmotionSelect = NULL;
	//m_iconAway = NULL;
	//m_iconBusy = NULL;
	//m_iconEnterprise = NULL;
	//m_iconDepartment = NULL;
	//m_iconContact = NULL;
	m_nMainColor = 0;
	m_nFlatBgColor = RGB(250,250,250);
	m_bColorGradient = false;
	m_bColorFlat = true;
	//m_nFuncColor = 0;
	m_nTrueColor = 0;
	m_nHotColor = 0;
	m_nHotColor1 = 0;
	m_nHotColor1 = 0;
	m_nHotColor2 = 0;
	m_nPreColor = 0;
	m_nSelColor = 0;
	m_nNormalColor = 0;
	//m_nBgTitle0 = 0;
	//m_nBgColor1 = 0;
	//m_nBgColor2 = 0;
	//m_nBgColor3 = 0;
	//m_nOkColor0 = 0;
	////m_nOkColor1 = 0;

	m_nSelectCallId = 0;
	m_nSelectUserId = 0;
	m_nSelectGroupId = 0;

	m_bAdaptIeVersion = false;

	m_nSendType = -1;
	//m_dlgAbout = NULL;
#ifdef _DEBUG
	//Atest(1);
#endif
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


// 唯一的一个 CPOPApp 对象

CPOPApp theApp;
#ifdef USES_EBCOM_TEST
CComPtr<IEBParseSetting> theEBSetting;
CComPtr<IEBClientCore> theEBClientCore;
#else
CEBParseSetting theSetting;
CEBAppClient theEBAppClient;
#endif

// CPOPApp 初始化
//#include "../include/md5.h"

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
void CPOPApp::AdaptIeVersion(void)
{
	//32 bit:
	//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	//64 bit:
	//HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION

	// HKEY_LOCAL_MACHINE\Software\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	// HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	//
	//11001 (0x2AF9)	//IE11. Webpages containing standards-based !DOCTYPE directives are displayed in IE11 edge mode. Default value for IE11.
						//IE11，包含标准!DOCTYPE指令的页面将会以IE11兼容模式打开，IE11浏览器的默认值。
	//11000 (0x2AF8)	//Internet Explorer 11. Webpages are displayed in IE11 edge mode, regardless of the declared !DOCTYPE directive. Failing to declare a !DOCTYPE directive causes the page to load in Quirks.
						//IE11，无论是否声明!DOCTYPE指令，页面将会以IE11的edge模式打开。对于未正确声明!DOCTYPE指令的页面，将会以怪异模式（quirks mode）加载。
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

	DWORD dwIeKenel = _ttoi(szVersion) * 1000;
	if (dwIeKenel==11000)
		dwIeKenel = 11001;	// IE11
	//CString sText;
	//sText.Format(_T("%d"),dwIeKenel);	// 8000
	//AfxMessageBox(sText);

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
#ifdef USES_LIBCEF
#include "cef/cefbrowser.h"
#include "cef/client_app.h"
#ifdef _DEBUG
#pragma comment(lib, "libcef_dll_wrapperd.lib")
#pragma comment(lib, "libcefd.lib")
#else
#pragma comment(lib, "libcef_dll_wrapper.lib")
#pragma comment(lib, "libcef.lib")
#endif
#endif

#include "tlhelp32.h"
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
int CPOPApp::GetExistAppCount(void) const
{
	char pFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	const std::string m_sMainExe(pFileName);

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
	if (hProcessSnap==NULL) return 0;
	PROCESSENTRY32 pe32;  
	memset(&pe32,0,sizeof(pe32));
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	int result = 0;
	if(::Process32First(hProcessSnap,&pe32))  
	{  
		//const std::string sAppPath(theApp.GetAppPath());
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

CefRefPtr<ClientApp> theCefApp;
ULONG_PTR theGdiplusToken=0;
BOOL CPOPApp::InitInstance()
{
	setlocale(LC_ALL, "");	// 使用默认环境 .936中文目录有问题。
	//setlocale(LC_ALL, ".936");

#ifdef USES_LIBCEF
	CefMainArgs main_args(this->m_hInstance);
	theCefApp = new ClientApp();
	const int exit_code = CefExecuteProcess(main_args, theCefApp.get(), NULL);
	if (exit_code >= 0) return FALSE;
#endif

	//_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(330);

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	//CWinAppEx::InitInstance();
	::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);	// *1.19
	//::CoInitializeEx(NULL,COINIT_MULTITHREADED);	// 没有事件
	//::CoInitialize(NULL);
	ChangeMessageProi();
	AfxEnableControlContainer();
	CNewMenu::SetDisableSelBorder(TRUE);
	CNewMenu::SetDisableShade(TRUE);
	CNewMenu::SetXpBlending(FALSE);
	CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP);
	::AfxInitRichEdit2();
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);

#ifdef USES_LIBCEF
	//CefMainArgs main_args(this->m_hInstance);
	//void* sandbox_info = NULL;
	//theCefApp = new ClientApp();
	//const int exit_code = CefExecuteProcess(main_args, theCefApp.get(), sandbox_info);
	//if (exit_code >= 0)
	//{
	//	return FALSE;
	//	//return exit_code;
	//}
#ifdef USES_LIBCEF_USER_CACHE
//	CefSettings settings;
//#if !defined(CEF_USE_SANDBOX)
//	settings.no_sandbox = true;
//#endif
//	settings.multi_threaded_message_loop = true;
//	cef_string_utf8_to_utf16("zh-CN",5,&settings.locale);
//	//cef_string_utf8_to_utf16(m_sCefCachePath,m_sCefCachePath.GetLength(),&settings.cache_path);
//#ifndef _DEBUG
//	settings.log_severity = LOGSEVERITY_DISABLE;
//#endif
//	CefMainArgs main_args(this->m_hInstance);
//	CefInitialize(main_args, settings, theCefApp.get(), NULL);
#else
	if (GetExistAppCount()<=1)
	{
		const CString sSettingIniFile = GetSettingIniFile();
		const bool bClearChromeCache = GetPrivateProfileInt("system","clear_chrome_cache",0,sSettingIniFile)==1?true:false;
		if (bClearChromeCache)
		{
			deleteDir(m_sCefCachePath);
			WritePrivateProfileString("system","clear_chrome_cache","0",sSettingIniFile);
		}
	}

	CefSettings settings;
#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif
	settings.multi_threaded_message_loop = true;
	cef_string_utf8_to_utf16("zh-CN",5,&settings.locale);
	cef_string_utf8_to_utf16(m_sCefCachePath,m_sCefCachePath.GetLength(),&settings.cache_path);
#ifndef _DEBUG
	settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	// Populate the settings based on command line arguments.
	//AppGetSettings(settings);
	CefInitialize(main_args, settings, theCefApp.get(), sandbox_info);
	//CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
	//manager->SetStoragePath((LPCTSTR)m_sCefCachePath,true);
#endif	// USES_LIBCEF_USER_CACHE

#endif

	SetRegistryKey(_T("entboost"));

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
	// 检查自动更新文件；
	const CString sNewVersion = EBC_GetProfileString(_T("new_version"),_T("version"));
	const CString sNewVersionFile = EBC_GetProfileString(_T("new_version"),_T("file"));
	//const CString sNewVersionFile = GetAppPath()+_T("\\1.0.0.1.rar");
	if (!sNewVersion.IsEmpty())
	{
		if (::PathFileExists(sNewVersionFile) && sNewVersion != sVersion.c_str())
		{
			// 有新版本需要自动更新
			//const CString sDescription = EBC_GetProfileString(_T("new_version"),_T("desc"));
			//const CString sUpdateTime = EBC_GetProfileString(_T("new_version"),_T("time"));
			//const UINT nUpdateType = EBC_GetProfileInt(_T("new_version"),_T("type"),0);

			// 执行自动更新过程
			TCHAR lpszModuleFileName[MAX_PATH];
			GetModuleFileName( NULL, lpszModuleFileName, MAX_PATH);
			CString sUpdateExe = GetAppPath()+_T("\\update\\ebupdate2.exe");
			CString sParameter;
			sParameter.Format(_T("\"%s\" \"%s\""), sNewVersionFile, lpszModuleFileName);
			ShellExecute(NULL,  "open", sUpdateExe, sParameter, GetAppPath(), SW_SHOW);
			return FALSE;
		}else
		{
			// 自动更新完成，或其他错误
			const CString sUpdateExe2 = GetAppPath()+_T("\\update\\ebupdate2.exe");
			const CString sNewUpdateExe = GetAppPath()+_T("\\update\\ebupdate2_new.exe");
			tstring sNewFileMd5;
			mycp::bigint nNewFileSize = 0;
			if (libEbc::GetFileMd5(sNewUpdateExe,nNewFileSize,sNewFileMd5))
			{
				tstring sOldFileMd5;
				mycp::bigint nOldFileSize = 0;
				if (!libEbc::GetFileMd5(sUpdateExe2,nOldFileSize,sOldFileMd5) || sNewFileMd5!=sOldFileMd5)
				{
					// 更新ebupdate2.exe文件
					for (int i=0; i<10; i++)	// 等级ebupdate2.exe程序退出
					{
						if (CopyFile(sNewUpdateExe,sUpdateExe2,FALSE))
						{
							break;
						}
						Sleep(200);
					}
				}
				DeleteFile(sNewUpdateExe);
			}
			//if (PathFileExists(sNewUpdateExe))
			//{
			//	// 更新ebupdate2.exe文件
			//	for (int i=0; i<10; i++)	// 等级ebupdate2.exe程序退出
			//	{
			//		if (CopyFile(sNewUpdateExe,sUpdateExe2,FALSE))
			//		{
			//			break;
			//		}
			//		Sleep(200);
			//	}
			//	DeleteFile(sNewUpdateExe);
			//}
			// 更新
			const CString sRarExe2 = GetAppPath()+_T("\\update\\Rar.exe");
			const CString sNewRarExe = GetAppPath()+_T("\\update\\Rar_new.exe");
			if (libEbc::GetFileMd5(sNewRarExe,nNewFileSize,sNewFileMd5))
			{
				tstring sOldFileMd5;
				mycp::bigint nOldFileSize = 0;
				if (!libEbc::GetFileMd5(sRarExe2,nOldFileSize,sOldFileMd5) || sNewFileMd5!=sOldFileMd5)
				{
					// 更新Rar.exe文件
					for (int i=0; i<10; i++)	// 等级Rar.exe程序退出
					{
						if (CopyFile(sNewRarExe,sRarExe2,FALSE))
						{
							break;
						}
						Sleep(200);
					}
				}
				DeleteFile(sNewRarExe);
			}
			//if (PathFileExists(sNewRarExe))
			//{
			//	// 更新Rar.exe文件
			//	for (int i=0; i<10; i++)	// 等级Rar.exe程序退出
			//	{
			//		if (CopyFile(sNewRarExe,sRarExe2,FALSE))
			//		{
			//			break;
			//		}
			//		Sleep(200);
			//	}
			//	DeleteFile(sNewRarExe);
			//}

			//DeleteFile(GetSettingIniFile());
			EBC_SetProfileString(_T("new_version"),_T("version"),_T(""));
			EBC_SetProfileString(_T("new_version"),_T("desc"),_T(""));
			EBC_SetProfileString(_T("new_version"),_T("time"),_T(""));
			EBC_SetProfileString(_T("new_version"),_T("file"),_T(""));
			EBC_SetProfileInt(_T("new_version"),_T("type"),0);
		}
	}
	const CString sUpdateTempDir = m_sAppPath + _T("\\update_temp");
	if (::PathFileExists(sUpdateTempDir))
		deleteDir(sUpdateTempDir);	// **删除自动更新临时目录文件

	m_sAppImgTempPath = m_sAppPath + _T("\\img\\temp");
	if (!::PathFileExists(m_sAppImgTempPath))
		CreateDirectory(m_sAppImgTempPath,NULL);	

    m_bmpTreeDirOpened.LoadBitmap(IDB_TREE_OPENED);
    m_bmpTreeDirClosed.LoadBitmap(IDB_TREE_CLOSED);
    m_bmpTreeType0Opened.LoadBitmap(IDB_TREE_TYPE0_OPENED);
    m_bmpTreeType0Closed.LoadBitmap(IDB_TREE_TYPE0_CLOSED);
    m_bmpTreeType1Opened.LoadBitmap(IDB_TREE_TYPE1_OPENED);
    m_bmpTreeType1Closed.LoadBitmap(IDB_TREE_TYPE1_CLOSED);
    m_bmpTreeType2Opened.LoadBitmap(IDB_TREE_TYPE2_OPENED);
    m_bmpTreeType2Closed.LoadBitmap(IDB_TREE_TYPE2_CLOSED);
    m_bmpTreeType9Opened.LoadBitmap(IDB_TREE_TYPE9_OPENED);
    m_bmpTreeType9Closed.LoadBitmap(IDB_TREE_TYPE9_CLOSED);

	USES_CONVERSION;
	libEbc::ImageExFromIDResource(IDB_PNG_BTN_SEND_ECARD,_T("png"),m_imageExBtnSendECard);
	libEbc::ImageFromIDResource(IDB_PNG_WORKFRAME,_T("png"),m_imageWorkFrame);
	libEbc::ImageFromIDResource(IDB_PNG_MAINFRAME,_T("png"),m_imageMainFrame);
	//CString sImagePath = GetAppPath()+_T("\\img\\entboost.ico");
	//m_imageEbIcon = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath));   
	//sImagePath = GetAppPath()+_T("\\img\\entlogo.png");
	//m_imageEntLogo = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath));
	libEbc::ImageFromIDResource(IDB_PNG_IMG1_GROUP,_T("png"),m_image1Group);
	libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),m_image1People);
	libEbc::ImageFromIDResource(IDB_PNG_IMG1_CLOUD,_T("png"),m_image1Cloud);
	libEbc::ImageFromIDResource(IDB_PNG_IMG2_CLOUD,_T("png"),m_image2Cloud);
	libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_VISITOR,_T("png"),m_imageDefaultVisitor);
	libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_ORG,_T("png"),m_imageDefaultOrg);
	CString sFilePath = this->GetAppPath() + _T("/img/defaultcontact.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultContact = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		m_imageDefaultContact = m_imageDefaultVisitor->Clone();
	sFilePath = this->GetAppPath() + _T("/img/defaultmember.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultMember = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		m_imageDefaultMember = m_imageDefaultContact->Clone();
	sFilePath = this->GetAppPath() + _T("/img/defaultapp.png");
	if (::PathFileExists(sFilePath))
		m_sDefaultAppImage = sFilePath;
	//else
	//	m_imageDefaultApp = m_imageDefaultContact->Clone();
	m_sDefaultLoadingImageFile = this->GetAppPath() + _T("\\img\\loading.gif");
	m_sDefaultPageImageFile = this->GetAppPath() + _T("\\img\\page.png");
	//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_CONTACT,_T("png"),m_imageDefaultContact);
	//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("png"),m_imageDefaultMember);
	sFilePath = this->GetAppPath() + _T("/img/defaultgroup0.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultDepartment = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_DEPARTMENT,_T("png"),m_imageDefaultDepartment);
	sFilePath = this->GetAppPath() + _T("/img/defaultgroup1.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultProject = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_PROJECT,_T("png"),m_imageDefaultProject);
	sFilePath = this->GetAppPath() + _T("/img/defaultgroup2.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultGroup = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_GROUP,_T("png"),m_imageDefaultGroup);
	sFilePath = this->GetAppPath() + _T("/img/defaultgroup9.png");
	if (::PathFileExists(sFilePath))
		m_imageDefaultTempGroup = new Gdiplus::Image(A2W_ACP(sFilePath));
	else
		libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_TEMPGROUP,_T("png"),m_imageDefaultTempGroup);
	libEbc::ImageFromIDResource(IDB_PNG_STATE_AWAY,_T("png"),m_imageStateAway);
	libEbc::ImageFromIDResource(IDB_PNG_STATE_BUSY,_T("png"),m_imageStateBusy);
	libEbc::ImageFromIDResource(IDB_PNG_STATE_FORBID,_T("png"),m_imageStateForbid);
	

	//// 实现在登录口，保持单实例。
	//HANDLE g_hMutex = CreateMutex(NULL,TRUE,_T("__entboost_mutex__"));
	//if(ERROR_ALREADY_EXISTS==GetLastError())
	//{
	//	//找到另一个实例
	//	CString stext;
	//	stext.Format(_T("%s-登录窗口"),theSetting.GetEnterprise().c_str());
	//	HWND l_hWnd = FindWindow(NULL, stext);
	//	if (l_hWnd!=NULL)
	//	{
	//		if (TRUE == IsIconic(l_hWnd))
	//		{
	//			ShowWindow(l_hWnd,SW_SHOWNORMAL); //还原窗口
	//		}
	//		SetForegroundWindow(l_hWnd); //使窗口在最前方
	//	}else
	//	{
	//		// ?
	//	}
	//	return FALSE;
	//}

	tstring sAppParameter0;
	if (__argc >= 2)
	{
		sAppParameter0 = __argv[1];
	}
	// **需要先设置一次
#ifdef USES_EBCOM_TEST
	this->SetMainColor(theEBSetting->DefaultColor);
#else
	this->SetMainColor(theSetting.GetDefaultColor());
#endif
	CDlgOAuth pDlgLogin;
	pDlgLogin.m_sAppParameter0 = sAppParameter0;
	pDlgLogin.DoModal();
	this->m_sAccountPrefix = pDlgLogin.GetAccountPrefix();
	this->m_sProductName = pDlgLogin.GetProductName();
	this->m_nSystemAccountFlag = pDlgLogin.GetSystemAccountFlag();
	this->m_bLicenseUser = pDlgLogin.GetLicenseUser();
	this->m_bSendRegMail = pDlgLogin.GetSendRegMail();
	this->m_bSaveConversationLocal = pDlgLogin.GetSaveConversationLocal();
	this->m_bSaveConversationServer = pDlgLogin.GetSaveConversationServer();
	this->m_bAuthContact = pDlgLogin.GetAuthContact();
	this->m_nDeployId = pDlgLogin.GetDeployId();
	this->m_nLicenstType = pDlgLogin.GetLicenseType();
	this->m_nEBServerVersion = pDlgLogin.GetEBServerVersion();
	//this->m_nGroupMsgSubId = pDlgLogin.GetGroupMsgSugId();
	this->m_nAutoOpenSubId = pDlgLogin.GetAutoOpenSubId();
	this->m_bAutoHideMainFrame = pDlgLogin.GetAutoHideMainFrame();
	this->m_bHideMainFrame = pDlgLogin.GetHideMainFrame();
	//this->m_nAutoOpenSubId = 0;//pDlgLogin.GetAutoOpenSubId();
	//this->m_bAutoHideMainFrame = false;//pDlgLogin.GetAutoHideMainFrame();
	//this->m_bHideMainFrame = false;//pDlgLogin.GetHideMainFrame();
	this->m_bDisableUserCloudDrive = pDlgLogin.GetDisableUserCloudDrive();
	this->m_bDisableGroupSharedCloud = pDlgLogin.GetDisableGroupSharedCloud();
	this->m_bDisableModifyPasswd = pDlgLogin.GetDisableModifyPasswd();
	this->m_sEntManagerUrl = pDlgLogin.GetEntManagerurl();
	this->m_bDisableVideo = pDlgLogin.GetDisableVideo();
	this->m_bDisableRemoteDesktop = pDlgLogin.GetDisableRD();
	this->m_bDisableAccountEdit = pDlgLogin.GetDisableAccountEdit();
	this->m_nDefaultBrowserType = pDlgLogin.GetDefaultBrowserType();
	this->m_bDisableMsgReceipt = pDlgLogin.GetDisableMsgReceipt();
	this->m_bStatSubGroupMember = pDlgLogin.GetStatSubGroupMember();
	this->m_sDefaultUrl = pDlgLogin.GetDefaultUrl();

	this->m_bIeException = pDlgLogin.GetIeException();
	
	//// for test
	//m_nAutoOpenSubId = 1002300102;	// 找群找人
	//m_bAutoHideMainFrame = true;
	//m_bHideMainFrame = true;

	const bool bIsVisitor = theApp.IsLogonVisitor();
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_IsLogoned == VARIANT_FALSE)
	{
		return FALSE;
	}
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

	m_pMenuImageList.Create(IDB_BITMAP_MENU_TOOLBAR,16,1,RGB(192,192,192));

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
	// 自动清除超过 2M ebc.exe.exception 文件；
#ifdef _DEBUG
	const CString sFileETemp = theApp.GetAppPath()+_T("\\ebcd.exe.exception");
#else
	const CString sFileETemp = theApp.GetAppPath()+_T("\\ebc.exe.exception");
#endif
	if (libEbc::GetFileSize(sFileETemp)>=2*1024*1024)	// 2MB
	{
		DeleteFile(sFileETemp);
	}

	CString sBoPath = theApp.GetAppDataPath() + _T("\\bodb");
	m_pBoEB = bo::bodb_init(sBoPath);
	if (m_pBoEB.get() == NULL)
	{
		CDlgMessageBox::EbDoModal(NULL,_T(""),_T("安装目录损坏：\r\n请重新安装！"),CDlgMessageBox::IMAGE_ERROR,true,8);
		return FALSE;
	}
	m_pBoEB->setaccount("system","");
	if (!m_pBoEB->use("eb"))
	{
		CDlgMessageBox::EbDoModal(NULL,_T(""),_T("配置文件损坏：\r\n请重新安装！"),CDlgMessageBox::IMAGE_ERROR,true,8);
		return FALSE;
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
	pResltSet = NULL;
	{
		// 检查本地默认浏览器
		m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='default-browser-type'", &pResltSet);
		if (pResltSet != NULL && pResltSet->rscount>0)
		{
			this->SetDefaultBrowserType((EB_BROWSER_TYPE)pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal);
			if (m_bIeException && this->GetDefaultBrowserType())
				SetDefaultBrowserType(EB_BROWSER_TYPE_CEF);
		}else
		{
			CString sSql;
			sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('default-browser-type',%d)"),(int)this->GetDefaultBrowserType());
			m_pBoEB->execsql(sSql);
		}
		bo::bodb_free(pResltSet);
		pResltSet = NULL;
	}
	{
		// 检查默认界面类型
		//m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='default-ui-style-type'", &pResltSet);
		//if (pResltSet != NULL && pResltSet->rscount>0)
		//{
		//	this->SetDefaultUIStyleType((EB_UI_STYLE_TYPE)pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal);
		//}else
		//{
		//	const int UISTYLE_CHAT_DEFAULT_DLG_WIDTH = 298;
		//	if (GetPrivateProfileInt(_T("default"),_T("main-w"),UISTYLE_CHAT_DEFAULT_DLG_WIDTH,theApp.GetUserSettingIniFile())!=UISTYLE_CHAT_DEFAULT_DLG_WIDTH)
		//	{
		//		char lpszBuffer[12];
		//		sprintf(lpszBuffer,"%d",UISTYLE_CHAT_DEFAULT_DLG_WIDTH);
		//		WritePrivateProfileString(_T("default"),_T("main-w"),lpszBuffer,theApp.GetUserSettingIniFile());
		//	}
		//	CString sSql;
		//	sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('default-ui-style-type',%d)"),(int)this->GetDefaultUIStyleType());
		//	m_pBoEB->execsql(sSql);
		//}
		//bo::bodb_free(pResltSet);
		//pResltSet = NULL;
	}
	// 检查扁平效果
	this->SetColorFlat(true);
	this->SetColorGradient(false);
	//m_pBoEB->execsql("SELECT value2 FROM sys_value_t WHERE name='color-flat'", &pResltSet);
	//if (pResltSet != NULL && pResltSet->rscount>0)
	//{
	//	this->SetColorFlat(pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal==1?true:false);
	//}else
	//{
	//	this->SetColorFlat(true);
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
	//	this->SetColorGradient(pResltSet->rsvalues[0]->fieldvalues[0]->v.uintegerVal==1?true:false);
	//}else
	//{
	//	this->SetColorGradient(false);
	//	CString sSql;
	//	sSql.Format(_T("INSERT INTO sys_value_t(name,value2) VALUES('color-gradient',0)"));
	//	m_pBoEB->execsql(sSql);
	//}
	//bo::bodb_free(pResltSet);
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

	sBoPath.Format(_T("%s\\%lld"),m_sUserMainPath,this->GetDeployId());
	if (!::PathFileExists(sBoPath))
		::CreateDirectory(sBoPath, NULL);
	sBoPath = sBoPath + _T("\\bodb");
	//sBoPath = m_sUserMainPath + _T("\\bodb");
	const tstring sDefaultUserBoFile = GetAppDataPath()+_T("\\bodb\\userdatas");
	const tstring sUserBoFile = sBoPath+_T("\\userdatas");
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
		CDlgMessageBox::EbDoModal(NULL,_T(""),_T("安装目录损坏：\r\n请重新安装！"),CDlgMessageBox::IMAGE_ERROR,true,8);
		return FALSE;
	}
	if (m_pBoUsers->select("SELECT create_time FROM url_record_t limit 1")==-1)
	{
		const tstring sCreateSql = "CREATE TABLE url_record_t"\
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
	// 1.19 版本，判断 msg_record_t 表增加 read_flag 字段
	if (m_pBoUsers->select("SELECT read_flag FROM msg_record_t limit 1")==-1)
	{
		m_pBoUsers->execute("ALTER TABLE msg_record_t ADD read_flag TINYINT DEFAULT 1");
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
								bo::bodb_escape_string_out(sMsgText.string());
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
	//}else
	//{
	//	//// ???同步 1.15旧用户数据；1.17以后版本删除；
	//	//sOldUserBoPath = m_sUserMainPath+_T("\\bodb\\userdatas");
	//	//if (::PathFileExists(sOldUserBoPath))
	//	//{
	//	//	// 
	//	//	CSqliteCdbc pOldSqlite;
	//	//	if (pOldSqlite.open(libEbc::ACP2UTF8(sOldUserBoPath).c_str()))
	//	//	{
	//	//		CString sSql = _T("SELECT call_id,Datetime(call_time,'localtime'),dep_code,dep_name,emp_code,from_uid,from_phone,from_account,from_name,from_type,company,title,tel,email FROM call_record_t LIMIT 50");
	//	//		int nCookie = 0;
	//	//		pOldSqlite.select(sSql, nCookie);
	//	//		cgcValueInfo::pointer pRecord = pOldSqlite.first(nCookie);
	//	//		while (pRecord.get()!=NULL)
	//	//		{
	//	//			const mycp::bigint sCallId = pRecord->getVector()[0]->getBigIntValue();
	//	//			const tstring sCallTime = pRecord->getVector()[1]->getStrValue();
	//	//			const mycp::bigint sGroupCode = pRecord->getVector()[2]->getBigIntValue();
	//	//			const tstring sGroupName = pRecord->getVector()[3]->getStrValue();
	//	//			const mycp::bigint sEmpCode = pRecord->getVector()[4]->getBigIntValue();
	//	//			const mycp::bigint m_nFromUserId = pRecord->getVector()[5]->getBigIntValue();
	//	//			const mycp::bigint sPhone = pRecord->getVector()[6]->getBigIntValue();
	//	//			//pCallRecordInfo->m_sFromAccount = pRecord->getVector()[7]->getStrValue();
	//	//			const tstring sFromName = pRecord->getVector()[8]->getStrValue();
	//	//			const int nFromType = pRecord->getVector()[9]->getIntValue();
	//	//			const tstring sCompany = pRecord->getVector()[10]->getStrValue();
	//	//			const tstring sTitle = pRecord->getVector()[11]->getStrValue();
	//	//			const tstring sTel = pRecord->getVector()[12]->getStrValue();
	//	//			const tstring sEmail = pRecord->getVector()[13]->getStrValue();

	//	//			sSql.Format(_T("INSERT INTO call_record_t(call_id,call_time,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,from_type,company,title,tel,email) ")\
	//	//				_T("VALUES(%lld,'%s',%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')"),
	//	//				sCallId,sCallTime.c_str(),sGroupCode,sGroupName.c_str(),sEmpCode,m_nFromUserId,sPhone,sFromName.c_str(),nFromType,
	//	//				sCompany.c_str(),sTitle.c_str(),sTel.c_str(),sEmail.c_str());
	//	//			theApp.m_pBoUsers->execute(sSql);
	//	//			pRecord = pOldSqlite.next(nCookie);
	//	//		}
	//	//		pOldSqlite.reset(nCookie);

	//	//		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,dep_code FROM msg_record_t"));
	//	//		pOldSqlite.select(sSql, nCookie);
	//	//		pRecord = pOldSqlite.first(nCookie);
	//	//		while (pRecord.get()!=NULL)
	//	//		{
	//	//			const tstring sTime = pRecord->getVector()[0]->getStrValue();
	//	//			const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
	//	//			const tstring soffTime = pRecord->getVector()[2]->getStrValue();
	//	//			const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
	//	//			const tstring sFromName(pRecord->getVector()[4]->getStrValue());
	//	//			const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
	//	//			const tstring sToName(pRecord->getVector()[6]->getStrValue());
	//	//			const int nPrivate = pRecord->getVector()[7]->getIntValue();
	//	//			const int nMsgType = pRecord->getVector()[8]->getIntValue();
	//	//			const tstring sMsgName(pRecord->getVector()[9]->getStrValue());
	//	//			const tstring sMsgText(pRecord->getVector()[10]->getStrValue());
	//	//			const eb::bigint sGroupCode = pRecord->getVector()[11]->getBigIntValue();

	//	//			sSql.Format(_T("INSERT INTO msg_record_t(msg_time,msg_id,off_time,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text) ")\
	//	//				_T("VALUES('%s',%lld,'%s',%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s')"),
	//	//				sTime.c_str(),sMsgId,soffTime.c_str(),sGroupCode,sFromAccount,sFromName.c_str(),sToAccount,
	//	//				sToName.c_str(),nPrivate,nMsgType,sMsgName.c_str(),sMsgText.c_str());
	//	//			theApp.m_pBoUsers->execute(sSql);

	//	//			pRecord = pOldSqlite.next(nCookie);
	//	//		}
	//	//		pOldSqlite.reset(nCookie);

	//	//		pOldSqlite.close();
	//	//	}
	//	//	DeleteFile(sOldUserBoPath);
	//	//	sOldUserBoPath = m_sUserMainPath+_T("\\bodb");
	//	//	RemoveDirectory(sOldUserBoPath);
	//	//}
	}

#ifdef USES_LIBCEF
#ifdef USES_LIBCEF_USER_CACHE
	if (bIsVisitor)
	{
		m_sCefCachePath += _T("\\visitor");
	}else
	{
		char lpszBuffer[24];
		sprintf(lpszBuffer,"\\%lld",m_nLogonUserId);
		m_sCefCachePath += lpszBuffer;
	}
	const CString sUserSettingIniFile = GetUserSettingIniFile();
	const bool bClearChromeCache = GetPrivateProfileInt("default","clear_chrome_cache",0,sUserSettingIniFile)==1?true:false;
	if (bClearChromeCache)
	{
		deleteDir(m_sCefCachePath);
		WritePrivateProfileString("default","clear_chrome_cache","0",sUserSettingIniFile);
	}
	// ??? 1.21 兼容旧版本，智能移第一个用户本地缓存，1.22以后版本删除该代码
	const CString sSettingIniFile = GetSettingIniFile();
	const bool bMoveFirstChromeCache = GetPrivateProfileInt("system","move_first_chrome_cache",1,sSettingIniFile)==1?true:false;
	if (bMoveFirstChromeCache && !::PathFileExists(m_sCefCachePath))
	{
		WritePrivateProfileString("system","move_first_chrome_cache","0",sSettingIniFile);
		char lpszBuffer[MAX_PATH];
		SHGetSpecialFolderPath(::GetDesktopWindow(),lpszBuffer,CSIDL_APPDATA,FALSE);
		CString sCefCachePathTemp;
		sCefCachePathTemp.Format(_T("%s\\entboost\\cef_cache_path"),lpszBuffer);
		if (::PathFileExists(sCefCachePathTemp))
		{
			//CreateDirectory(m_sCefCachePath,NULL);
			//copyDir(sCefCachePathTemp,m_sCefCachePath);
			deleteDir(sCefCachePathTemp);
		}
	}

	CefSettings settings;
#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif
	//settings.ignore_certificate_errors = 1;
	settings.multi_threaded_message_loop = 1;
	cef_string_utf8_to_utf16("zh-CN",5,&settings.locale);
	cef_string_utf8_to_utf16(m_sCefCachePath,m_sCefCachePath.GetLength(),&settings.cache_path);
	// User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.95 Safari/537.36
	//const std::string sUserAgent("ENTBOOST/1.21 (Windows) CEF/3.2272.32.gbda8dc7 Chromium/41.0.2272.104");
	//cef_string_utf8_to_utf16(sUserAgent.c_str(),sUserAgent.size(),&settings.user_agent);
#ifndef _DEBUG
	settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	CefInitialize(main_args, settings, theCefApp.get(), NULL);
	//CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
	//manager->SetStoragePath((LPCTSTR)m_sCefCachePath,true);
#endif	// USES_LIBCEF_USER_CACHE
#endif	// USES_LIBCEF

	//const CString theUserBoPath(sBoPath);
	//m_pBoUsers = bo::bodb_init(sBoPath);
	//if (m_pBoUsers.get() == NULL)
	//{
	//	CDlgMessageBox::EbDoModal(NULL,_T("安装目录损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
	//	return FALSE;
	//}
	////bool bret = openebuser();
	//bool bError = false;
	//try
	//{
	//	m_pBoUsers->setaccount("system","");
	//	if (!m_pBoUsers->use("ebuser"))
	//	{
	//		// 数据库文件错误
	//		bError = true;
	//	}
	//}
	//catch(std::exception&)
	//{
	//	bError = true;
	//	//AfxMessageBox(_T("catch 111"));
	//}
	//catch(...)
	//{
	//	// 数据库文件错误
	//	bError = true;
	//	//AfxMessageBox(_T("catch 333"));
	//}
	//if (bError)
	//{
	//	m_pBoUsers->close();
	//	//CopyFile(sDefaultUserBoFile.c_str(), sUserBoFile.c_str(), FALSE);
	//	CopyFile(sUserBoFileBk.c_str(), sUserBoFile.c_str(), FALSE);
	//	CDlgMessageBox::EbDoModal(NULL,_T("配置文件损坏，已经修复："),_T("请重新登录！"),CDlgMessageBox::IMAGE_INFORMATION);
	//	TCHAR lpModuleFileName[MAX_PATH] = {'\0'};
	//	::GetModuleFileName( NULL, lpModuleFileName, MAX_PATH);
	//	ShellExecute(NULL,  "open", lpModuleFileName, "", GetAppPath(), SW_SHOW);
	//	return FALSE;
	//}else
	//{
	//	CopyFile(sUserBoFile.c_str(), sUserBoFileBk.c_str(), FALSE);
	//}

	//// **检查更新；
	//int ret = m_pBoUsers->execsql("select from_type FROM call_record_t WHERE from_type=100");
	//if (ret == -1)
	//{
	//	// **该字段不存在，需要更新数据库字段 --2014-01-26
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE call_id VARCHAR(16) NOT NULL");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE dep_code VARCHAR(16) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE emp_code VARCHAR(16) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD from_type TINYINT DEFAULT 1");
	//	m_pBoUsers->execsql("ALTER TABLE msg_record_t CHANGE msg_id VARCHAR(16) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE msg_record_t CHANGE dep_code VARCHAR(16) DEFAULT ''");
	//}
	////m_pBoUsers->execsql("ALTER TABLE call_record_t DROP COLUMN company");
	//ret = m_pBoUsers->execsql("select company FROM call_record_t WHERE company=''");
	//if (ret == -1)
	//{
	//	// **该字段不存在，需要更新数据库字段 --2014-02-08
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD company VARCHAR(128) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD title VARCHAR(64) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD phone VARCHAR(32) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD tel VARCHAR(32) DEFAULT ''");
	//	m_pBoUsers->execsql("ALTER TABLE call_record_t ADD email VARCHAR(64) DEFAULT ''");
	//}
	//ret = m_pBoUsers->execsql("select from_phone FROM call_record_t WHERE from_phone=0");
	//if (ret == -1)
	//{
	//	// 直接更换文件
	//	m_pBoUsers->close();
	//	bo::bodb_exit(m_pBoUsers);
	//	m_pBoUsers.reset();
	//	CopyFile(sDefaultUserBoFile.c_str(), sUserBoFile.c_str(), FALSE);
	//	m_pBoUsers = bo::bodb_init(theUserBoPath);
	//	if (m_pBoUsers.get() == NULL)
	//	{
	//		CDlgMessageBox::EbDoModal(NULL,_T("安装目录损坏："),_T("请重新安装！"),CDlgMessageBox::IMAGE_ERROR);
	//		return FALSE;
	//	}
	//	m_pBoUsers->setaccount("system","");
	//	m_pBoUsers->use("ebuser");

	//	//// **该字段不存在，需要更新数据库字段 --2014-03-07
	//	////m_pBoUsers->execsql("DELETE FROM call_record_t");
	//	////m_pBoUsers->execsql("DELETE FROM msg_record_t");
	//	//m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE call_id BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE dep_code BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE call_record_t CHANGE emp_code BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE call_record_t DROP from_account");
	//	//m_pBoUsers->execsql("ALTER TABLE call_record_t ADD from_uid BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t CHANGE msg_id BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t CHANGE dep_code BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t DROP from_account");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t DROP to_account");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t ADD from_uid BIGINT DEFAULT 0");
	//	//m_pBoUsers->execsql("ALTER TABLE msg_record_t ADD to_uid BIGINT DEFAULT 0");
	//}

	m_nMinEBSC = GetPrivateProfileInt(_T("default"),_T("min-ebsc"),0,theApp.GetUserSettingIniFile())==1?true:false;

	if (m_bHideMainFrame)
	{
		m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_CHAT;	// *
		//m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_OFFICE;	// *
	}else
	{
		// 检查默认界面类型
		const int nDefaultUIStyleType = GetPrivateProfileInt(_T("default"),_T("uistyle-type"),2,theApp.GetUserSettingIniFile());
		if (nDefaultUIStyleType>=2)
		{
			// 第一次
			m_nDefaultUIStyleType = (EB_UI_STYLE_TYPE)theSetting.GetDefaultUIStyleType();
			if (m_nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			{
				if (GetPrivateProfileInt(_T("default"),_T("main-w"),UISTYLE_CHAT_DEFAULT_DLG_WIDTH,theApp.GetUserSettingIniFile())>UISTYLE_CHAT_DEFAULT_DLG_WIDTH)
				{
					char lpszBuffer[12];
					sprintf(lpszBuffer,"%d",UISTYLE_CHAT_DEFAULT_DLG_WIDTH);
					WritePrivateProfileString(_T("default"),_T("main-w"),lpszBuffer,theApp.GetUserSettingIniFile());
				}
				if (GetPrivateProfileInt(_T("default"),_T("main-h"),UISTYLE_CHAT_DEFAULT_DLG_HEIGHT,theApp.GetUserSettingIniFile())>UISTYLE_CHAT_DEFAULT_DLG_HEIGHT)
				{
					char lpszBuffer[12];
					sprintf(lpszBuffer,"%d",UISTYLE_CHAT_DEFAULT_DLG_HEIGHT);
					WritePrivateProfileString(_T("default"),_T("main-h"),lpszBuffer,theApp.GetUserSettingIniFile());
				}
			}
		}else
		{
			m_nDefaultUIStyleType = (EB_UI_STYLE_TYPE)nDefaultUIStyleType;
		}
	}
	//ShowImageWindow(false,NULL,NULL);
	CPOPDlg dlg;
	m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	dlg.Create(CPOPDlg::IDD);
	//INT_PTR nResponse = IDOK;
	//CefRunMessageLoop();
	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0)) {
	//	// Allow processing of dialog messages.
	//	//if (dialog_hwnd_ && IsDialogMessage(dialog_hwnd_, &msg))
	//	//	continue;
	//	//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	//}
	//}
	INT_PTR nResponse = dlg.RunModalLoop();
	dlg.DestroyWindow();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	//if (hRichHandle!=NULL)
	//	FreeLibrary(hRichHandle);
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CPOPApp::ExitInstance()
{
	//if (m_dlgAbout!=NULL)
	//{
	//	m_dlgAbout->DestroyWindow();
	//	delete m_dlgAbout;
	//	m_dlgAbout = NULL;
	//}
	m_pChatMenuSubscribeFuncList.clear();
	m_pChatCopySubscribeFuncList.clear();
	if (m_pDlgEmotionSelect)
	{
		m_pDlgEmotionSelect->DestroyWindow();
		delete m_pDlgEmotionSelect;
		m_pDlgEmotionSelect = NULL;
	}
	if (m_pAutoOpenFuncWindow.get()!=NULL)
	{
		m_pAutoOpenFuncWindow->DestroyWindow();
		m_pAutoOpenFuncWindow.reset();
	}

	if (m_bmpTreeDirOpened.GetSafeHandle()!=NULL)
		m_bmpTreeDirOpened.DeleteObject();
	if (m_bmpTreeDirClosed.GetSafeHandle()!=NULL)
		m_bmpTreeDirClosed.DeleteObject();
	if (m_bmpTreeType0Opened.GetSafeHandle()!=NULL)
		m_bmpTreeType0Opened.DeleteObject();
	if (m_bmpTreeType0Closed.GetSafeHandle()!=NULL)
		m_bmpTreeType0Closed.DeleteObject();
	if (m_bmpTreeType1Opened.GetSafeHandle()!=NULL)
		m_bmpTreeType1Opened.DeleteObject();
	if (m_bmpTreeType1Closed.GetSafeHandle()!=NULL)
		m_bmpTreeType1Closed.DeleteObject();
	if (m_bmpTreeType2Opened.GetSafeHandle()!=NULL)
		m_bmpTreeType2Opened.DeleteObject();
	if (m_bmpTreeType2Closed.GetSafeHandle()!=NULL)
		m_bmpTreeType2Closed.DeleteObject();
	if (m_bmpTreeType9Opened.GetSafeHandle()!=NULL)
		m_bmpTreeType9Opened.DeleteObject();
	if (m_bmpTreeType9Closed.GetSafeHandle()!=NULL)
		m_bmpTreeType9Closed.DeleteObject();

	if (m_imageExBtnSendECard)
		delete m_imageExBtnSendECard;
	if (m_imageWorkFrame)
		delete m_imageWorkFrame;
	if (m_imageMainFrame)
		delete m_imageMainFrame;
	//if (m_imageEbIcon)
	//	delete m_imageEbIcon;
	//if (m_imageEntLogo)
	//	delete m_imageEntLogo;
	if (m_image1Group)
		delete m_image1Group;
	if (m_image1People)
		delete m_image1People;
	if (m_image1Cloud)
		delete m_image1Cloud;
	if (m_image2Cloud)
		delete m_image2Cloud;
	if (m_imageDefaultVisitor)
		delete m_imageDefaultVisitor;
	if (m_imageDefaultOrg)
		delete m_imageDefaultOrg;
	if (m_imageDefaultContact)
		delete m_imageDefaultContact;
	if (m_imageDefaultMember)
		delete m_imageDefaultMember;
	if (m_imageDefaultDepartment)
		delete m_imageDefaultDepartment;
	if (m_imageDefaultProject)
		delete m_imageDefaultProject;
	if (m_imageDefaultGroup)
		delete m_imageDefaultGroup;
	if (m_imageDefaultTempGroup)
		delete m_imageDefaultTempGroup;
	if (m_imageStateAway)
		delete m_imageStateAway;
	if (m_imageStateBusy)
		delete m_imageStateBusy;
	if (m_imageStateForbid!=NULL)
	{
		delete m_imageStateForbid;
		m_imageStateForbid = NULL;
	}

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
	Gdiplus::GdiplusShutdown(theGdiplusToken);

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

	
	bo::bodb_exit(m_pBoEB);
	m_pBoUsers.reset();
	::CoUninitialize();
	if (theDefaultDirIcon!=NULL)
	{
		DestroyIcon(theDefaultDirIcon);
		theDefaultDirIcon = NULL;
	}
	if (!theFileHIcon.empty(false))
	{
		CLockMap<tstring,HICON>::iterator pIter = theFileHIcon.begin();
		for (; pIter!=theFileHIcon.end(); pIter++)
		{
			HICON hIcon = pIter->second;
			DestroyIcon(hIcon);
		}
		theFileHIcon.clear();
	}
#ifdef USES_LIBCEF
	CefShutdown();
	//CefQuitMessageLoop();
	theCefApp = NULL;
#endif
	if (m_pLogDebug!=NULL)
	{
		fclose(m_pLogDebug);
		m_pLogDebug = NULL;
	}
	return CWinApp::ExitInstance();
	//return CWinAppEx::ExitInstance();
}

int CPOPApp::GetSendType(void)
{
	if (m_nSendType==-1)
		m_nSendType = (int)GetPrivateProfileInt(_T("default"),_T("send-type"),0,GetUserSettingIniFile());
	return m_nSendType;
}
void CPOPApp::SetSendType(int nSendType)
{
	if (m_nSendType!=nSendType)
	{
		m_nSendType = nSendType;
		char lpszBuf[12];
		sprintf(lpszBuf,"%d",m_nSendType);
		::WritePrivateProfileString(_T("default"),_T("send-type"),lpszBuf,GetUserSettingIniFile());
	}
}


CString CPOPApp::GetSettingFile(void) const
{
	return GetAppDataPath() + _T("\\setting");
}
CString CPOPApp::GetSettingIniFile(void) const
{
	return GetAppDataPath() + _T("\\setting.ini");
}
CString CPOPApp::GetUserSettingIniFile(void) const
{
	return m_sUserMainPath + _T("\\setting.ini");
}

void CPOPApp::EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	::WritePrivateProfileString(lpszAppName,lpszKeyName,lpszValue,sIniSettingFile);
}
CString CPOPApp::EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	TCHAR lpszBuffer[1024];
	memset(lpszBuffer,0,sizeof(lpszBuffer));
	::GetPrivateProfileString(lpszAppName,lpszKeyName,lpszDefault,lpszBuffer,sizeof(lpszBuffer),sIniSettingFile);
	return lpszBuffer;
}
void CPOPApp::EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue)
{
	const CString sIniSettingFile = GetSettingIniFile();
	CString sValue;
	sValue.Format(_T("%d"),nValue);
	::WritePrivateProfileString(lpszAppName,lpszKeyName,sValue,sIniSettingFile);
}
UINT CPOPApp::EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault)
{
	const CString sIniSettingFile = GetSettingIniFile();
	return ::GetPrivateProfileInt(lpszAppName,lpszKeyName,nDefault,sIniSettingFile);
}

HICON CPOPApp::GetFileHIcon(const std::string& sFilePath)
{
	HICON result = NULL;
	tstring sFileName;
	tstring sFileExt;
	libEbc::GetFileExt(sFilePath,sFileName,sFileExt);
	if (!sFileExt.empty())
	{
		if (theFileHIcon.find(sFileExt,result))
		{
			return result;
		}
	}
	SHFILEINFO sfi; 
	ZeroMemory(&sfi,sizeof(sfi)); 
	const DWORD ret = SHGetFileInfo(sFilePath.c_str(),
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
	if (ret == 1)
	{
		result = sfi.hIcon;
		if (!sFileExt.empty())
		{
			theFileHIcon.insert(sFileExt,result);
		}
	}
	return result;
}
void CPOPApp::SelectFirstSearch(VividTree& treeSearch, CHoverEdit& editSearch, bool& bCanSearch)
{
	HTREEITEM hFristVisibleItem = treeSearch.GetFirstVisibleItem();
	if (hFristVisibleItem==NULL) return;
	HTREEITEM hFristVisibleChildItem = treeSearch.GetChildItem(hFristVisibleItem);
	HTREEITEM hSelectItem = hFristVisibleChildItem==NULL?hFristVisibleItem:hFristVisibleChildItem;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)treeSearch.GetItemData(hSelectItem);
	if (pTreeItemInfo == NULL) return;

	int nSel = 0;
	CString sOldString;
	editSearch.GetWindowText(sOldString);
	int nStartChar = 0;
	int nEndChar = 0;
	editSearch.GetSel(nStartChar,nEndChar);

	CString sFullString;
	switch (pTreeItemInfo->m_nItemType)
	{
	case CTreeItemInfo::ITEM_TYPE_URL:
		{
			sFullString = pTreeItemInfo->m_sAccount.c_str();	// url
			if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				nSel = 1;
			else
			{
				if (nStartChar>0)
				{
					if (sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
						nSel = nStartChar;
					else
					{
						const int nTemp = sFullString.Find(sOldString.Left(nStartChar));
						if (nTemp>nStartChar)
						{
							if (nTemp>3)	// ** 判断是否http://或者www.之类开头
							{
								CString sTemp = sFullString.Mid(nTemp-3,3);
								sTemp.MakeLower();
								if (sTemp!=_T("://") && sTemp!="ww.")
								{
									return;
								}
							}
							nSel = nStartChar;
							sFullString = sFullString.Mid(nTemp);
						}else
						{
							return;
						}
					}
				}
			}
		}break;
	case CTreeItemInfo::ITEM_TYPE_SUBINFO:
		{
			const bool bInputFuncPrefix = (!sOldString.IsEmpty()&&sOldString.Left(1)=="/")?true:false;
			if (bInputFuncPrefix)
			{
				sFullString = "/";
				sFullString += pTreeItemInfo->m_sAccount.c_str();	// function_name
			}else
			{
				sFullString = pTreeItemInfo->m_sAccount.c_str();	// function_name
			}
			if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				nSel = 1;
			else
			{
				if (nStartChar>0)
				{
					if (sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
						nSel = nStartChar;
					else
					{
						const int nTemp = sFullString.Find(sOldString.Left(nStartChar));
						if (nTemp>nStartChar)
						{
							nSel = nStartChar;
							//sFullString = sFullString.Mid(nTemp);
						}
					}
				}
			}
		}break;
	case CTreeItemInfo::ITEM_TYPE_MEMBER:
		{
			bool bFindFirst = false;
			{
				sFullString = pTreeItemInfo->m_sAccount.c_str();	// account
				if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				{
					nSel = 1;
					bFindFirst = true;
				}else
				{
					if (nStartChar>0 && sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
					{
						nSel = nStartChar;
						bFindFirst = true;
					}
				}
			}
			if (!bFindFirst)
			{
				sFullString.Format(_T("%s(%s)"), pTreeItemInfo->m_sName.c_str(),pTreeItemInfo->m_sAccount.c_str());	// UserName(account)
				if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				{
					nSel = 1;
					bFindFirst = true;
				}else
				{
					if (nStartChar>0 && sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
					{
						nSel = nStartChar;
						bFindFirst = true;
					}
				}
			}
			if (!bFindFirst)
			{
				sFullString.Format("%lld",pTreeItemInfo->m_nUserId);	// user_id
				if (sOldString==sFullString)
				{
					nSel = sOldString.GetLength();
					bFindFirst = true;
				}
			}
			if (!bFindFirst)
			{
				return;
			}
		}break;
	case CTreeItemInfo::ITEM_TYPE_CONTACT:
		{
			bool bFindFirst = false;
			{
				sFullString = pTreeItemInfo->m_sAccount.c_str();	// contact
				if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				{
					nSel = 1;
					bFindFirst = true;
				}else
				{
					if (nStartChar>0 && sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
					{
						nSel = nStartChar;
						bFindFirst = true;
					}
				}
			}
			if (!bFindFirst)
			{
				sFullString.Format(_T("%s(%s)"), pTreeItemInfo->m_sName.c_str(),pTreeItemInfo->m_sAccount.c_str());	// UserName(contact)
				if (sOldString.GetLength()==1 && sFullString.Find(sOldString)==0)
				{
					nSel = 1;
					bFindFirst = true;
				}else
				{
					if (nStartChar>0 && sOldString.Left(nStartChar)==sFullString.Left(nStartChar))
					{
						nSel = nStartChar;
						bFindFirst = true;
					}
				}
			}
			if (!bFindFirst && pTreeItemInfo->m_nUserId>0)
			{
				sFullString.Format("%lld",pTreeItemInfo->m_nUserId);	// user_id
				if (sOldString==sFullString)
				{
					nSel = sOldString.GetLength();
					bFindFirst = true;
				}
			}
			if (!bFindFirst)
			{
				return;
			}
		}break;
	default:
		return;
	}
	const bool bCanSearchTemp = bCanSearch;
	bCanSearch = false;
	treeSearch.SelectItem(hSelectItem);
	editSearch.SetWindowText(sFullString);
	editSearch.SetSel(nSel,-1);
	bCanSearch = bCanSearchTemp;
}
void CPOPApp::ClearSubscribeSelectInfo(void)
{
	m_pSubscribeFuncList.clear();
	m_nSelectCallId = 0;
	m_nSelectUserId = 0;
	m_sSelectAccount = "";
	m_nSelectGroupId = 0;
}

size_t CPOPApp::BuildChatMenuSubscribeFuncList(void)
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
size_t CPOPApp::BuildChatCopySubscribeFuncList(void)
{
	if (m_pChatCopySubscribeFuncList.empty())
	{
#ifdef USES_EBCOM_TEST
		_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_CHAT_COPY_TEXT_TOOLBAR);
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
				m_pChatCopySubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
			}
		}
#else
		theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_CHAT_COPY_TEXT_TOOLBAR,m_pChatCopySubscribeFuncList);
#endif
	}
	return m_pChatCopySubscribeFuncList.size();
}

//tstring CPOPApp::GetFullAccount(const char* lpszAccount) const
//{
//	tstring sAccount(lpszAccount);
//	std::string::size_type find = sAccount.find("@");
//	if (find == std::string::npos)
//	{
//		sAccount.append("@");
//		sAccount.append(theSetting.GetServerAddress());
//	}
//	return sAccount;
//}
//int CPOPApp::GetManagerLevel(const char* sGroupCode) const
//{
//	EB_MemberInfo pMyEmployeeInfo;
//	if (!theEBAppClient.EB_GetMyMemberInfo(&pMyEmployeeInfo,sGroupCode))
//		return 0;
//	return pMyEmployeeInfo.m_nManagerLevel;
//}

void CPOPApp::GetScreenSize(int & pWidth, int & pHeight)
{
	pWidth = GetScreenWidth();
	pHeight = GetScreenHeight();
	//pWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	////int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	//pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	//pHeight += GetSystemMetrics(SM_CYCAPTION);
}
int CPOPApp::GetScreenWidth(void)
{
	return GetSystemMetrics(SM_CXSCREEN); //屏幕宽度
}
int CPOPApp::GetScreenHeight(void)
{
	int pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	pHeight += GetSystemMetrics(SM_CYCAPTION);
	return pHeight;
}
const CString & CPOPApp::GetSysPath(void)
{
	if (m_sSystemDir.IsEmpty())
	{
		::GetSystemDirectory(m_sSystemDir.GetBuffer(256), 256);  
		m_sSystemDir.ReleaseBuffer();  
	}
	return m_sSystemDir;
}

void CPOPApp::ReadUserSetting(void)
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

void CPOPApp::SaveUserSetting(void)
{
	if (!m_sUserMainPath.IsEmpty())
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
}

void CPOPApp::ShowImageWindow(bool bShow,CImageSelectCallback* pCallback,LPRECT lpRect)
{
	if (m_pDlgEmotionSelect==NULL)
	{
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		//CWnd * pParent = this;
		m_pDlgEmotionSelect = new CDlgEmotionSelect(pParent);
		m_pDlgEmotionSelect->Create(CDlgEmotionSelect::IDD,pParent);
	}else if (bShow)
	{
		m_pDlgEmotionSelect->SetCtrlColor();
		m_pDlgEmotionSelect->ShowImage(bShow);
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

void CPOPApp::RunEBSC(const CString& sParameter)
{
	CString sEBDCFile = theApp.GetAppPath()+_T("\\ebsc.exe");
	ShellExecute(NULL, "open", sEBDCFile, sParameter, theApp.GetAppPath(), SW_SHOW);
}

void CPOPApp::InvalidateParentRect(const CWnd* pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetParent()->ScreenToClient(&rect);
	pWnd->GetParent()->InvalidateRect(&rect);
}

void CPOPApp::SetMinEBSC(bool newValue)
{
	if (m_nMinEBSC != newValue)
	{
		m_nMinEBSC = newValue;
		char lpszBuffer[12];
		sprintf(lpszBuffer,"%d",(int)(m_nMinEBSC?1:0));
		WritePrivateProfileString(_T("default"),_T("min-ebsc"),lpszBuffer,theApp.GetUserSettingIniFile());
	}
}
bool CPOPApp::SetDefaultUIStyleType(EB_UI_STYLE_TYPE newValue)
{
	if (m_nDefaultUIStyleType != newValue)
	{
		m_nDefaultUIStyleType = newValue;
		char lpszBuffer[12];
		sprintf(lpszBuffer,"%d",(int)m_nDefaultUIStyleType);
		WritePrivateProfileString(_T("default"),_T("uistyle-type"),lpszBuffer,theApp.GetUserSettingIniFile());
		//if (m_pBoEB.get()!=NULL && m_pBoEB->use("eb"))
		//{
		//	CString sSql;
		//	sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='default-ui-style-type'"),(int)GetDefaultUIStyleType());
		//	m_pBoEB->execsql(sSql);
		//	m_pBoEB->close();
		//}
		return true;
	}
	return false;
}

void CPOPApp::NewEmployeeInfo(CWnd* pParent,const CTreeItemInfo * pTreeItemInfo)
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
	//	pEditPopMemberInfo.m_sEmail = "test@entboost.com";
	//	pEditPopMemberInfo.m_sFax = "0755-27362216";
	//	pEditPopMemberInfo.m_sCellPhone = "0755-27362216";
	//	pEditPopMemberInfo.m_sAddress = "深圳宝安翻身路47区银丰商务大厦512室";
	//	pEditPopMemberInfo.m_sDescription = "这是测试员工描述。";
	//	theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	//}
	//return;
	//////////////// test end
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
		pEditPopMemberInfo.m_nDisplayIndex = pDlgMemberInfo.m_nDisplayIndex;
		theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	}
#endif
}

#ifdef USES_EBCOM_TEST
void CPOPApp::EditEmployeeInfo(CWnd* pParent,IEB_MemberInfo* pMemberInfo)
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
		//pEditPopMemberInfo.m_sHeadResourceId = pDlgMemberInfo.m_pMemberInfo.m_sHeadResourceId;
		pEditPopMemberInfo.m_sHeadResourceFile = pDlgMemberInfo.m_pMemberInfo.m_sHeadResourceFile;
		CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo(pEditPopMemberInfo);
		CComPtr<IEB_MemberInfo> pOutInterface;
		pIEBMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pOutInterface);
		theEBClientCore->EB_EditMember(pOutInterface);
	}
}
#else
void CPOPApp::EditEmployeeInfo(CWnd* pParent,const EB_MemberInfo* pMemberInfo)
{
	if (pMemberInfo==NULL) return;
	tstring sDepartmentName;
	if (!theEBAppClient.EB_GetGroupName(pMemberInfo->m_sGroupCode,sDepartmentName)) return;
	//EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
	//theEBAppClient.EB_GetGroupType(pMemberInfo->m_sGroupCode,nGroupType);

	CDlgMemberInfo pDlgMemberInfo(pParent);
	//pDlgMemberInfo.m_nGroupType = nGroupType;
	pDlgMemberInfo.m_sGroupCode = pMemberInfo->m_sGroupCode;
	pDlgMemberInfo.m_sMemberCode = pMemberInfo->m_sMemberCode;
	pDlgMemberInfo.m_sMemberUserId = pMemberInfo->m_nMemberUserId;
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
	pDlgMemberInfo.m_nDisplayIndex = pMemberInfo->m_nDisplayIndex;
	pDlgMemberInfo.m_pMemberInfo = pMemberInfo;
	//pDlgMemberInfo.m_sHeadResourceFile = pMemberInfo->m_sHeadResourceFile.c_str();
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
		pEditPopMemberInfo.m_nDisplayIndex = pDlgMemberInfo.m_nDisplayIndex;
		//pEditPopMemberInfo.m_sHeadResourceId = pDlgMemberInfo.m_pMemberInfo.m_sHeadResourceId;
		pEditPopMemberInfo.m_sHeadResourceFile = pDlgMemberInfo.m_pMemberInfo.m_sHeadResourceFile;
		theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	}
}
#endif
void CPOPApp::EditGroupInfo(CWnd* pParent, eb::bigint sGroupCode)
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
	}else if (!theEBAppClient.EB_CanEditGroupInfo(pGroupInfo.m_sEnterpriseCode,pGroupInfo.m_sGroupCode))
	//}else if (pGroupInfo.m_nCreateUserId != theEBAppClient.EB_GetLogonUserId())
	{
		// 不是部门创建者，不能编辑
		return;
	}

	CDlgGroupInfo pDlgGroupInfo(pParent);
	pDlgGroupInfo.m_nGroupId = pGroupInfo.m_sGroupCode;
	pDlgGroupInfo.m_nManagerUserId = pGroupInfo.m_nManagerUserId;
	pDlgGroupInfo.m_nGroupType = pGroupInfo.m_nGroupType;
	pDlgGroupInfo.m_sGroupName = pGroupInfo.m_sGroupName.c_str();
	pDlgGroupInfo.m_sPhone = pGroupInfo.m_sPhone.c_str();
	pDlgGroupInfo.m_sFax = pGroupInfo.m_sFax.c_str();
	pDlgGroupInfo.m_sEmail = pGroupInfo.m_sEmail.c_str();
	pDlgGroupInfo.m_sUrl = pGroupInfo.m_sUrl.c_str();
	pDlgGroupInfo.m_sAddress = pGroupInfo.m_sAddress.c_str();
	pDlgGroupInfo.m_sDescription = pGroupInfo.m_sDescription.c_str();
	pDlgGroupInfo.m_nDisplayIndex = pGroupInfo.m_nDisplayIndex;
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		EB_GroupInfo pEditPopDepartment(pGroupInfo.m_sEnterpriseCode, pGroupInfo.m_sGroupCode);
		pEditPopDepartment.operator =(pGroupInfo);
		pEditPopDepartment.m_sParentCode = pGroupInfo.m_sParentCode;
		pEditPopDepartment.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
		pEditPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pEditPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pEditPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pEditPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pEditPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pEditPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pEditPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
		pEditPopDepartment.m_nDisplayIndex = pDlgGroupInfo.m_nDisplayIndex;
		theEBAppClient.EB_EditGroup(&pEditPopDepartment);
	}
#endif
}

bool CPOPApp::IsLogonVisitor(void) const
{
#ifdef USES_EBCOM_TEST
	return theEBClientCore != NULL && theEBClientCore->EB_IsLogonVisitor==VARIANT_TRUE;
#else
	return theEBAppClient.EB_IsLogonVisitor();
#endif

}

Image * CPOPApp::GetUserHeadImage(mycp::bigint nUserId,const tstring& sAccount) const
{
	CEBString sImagePath;
	bool bIsMemberAccount = false;
	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_MemberInfo> pMemberInfo;
	if (nUserId>0)
		pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(nUserId);
	if (pMemberInfo == NULL && !sAccount.empty())
		pMemberInfo = theEBClientCore->EB_GetMemberInfoByAccount2(sAccount.c_str());
	if (pMemberInfo != NULL)
	{
		bIsMemberAccount = true;
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
	if (nUserId>0)
		bFind = theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,nUserId);
	if (!bFind && !sAccount.empty())
		bFind = theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,sAccount.c_str());
	if (bFind)
	{
		bIsMemberAccount = true;
		pOutLineState = pMemberInfo.m_nLineState;
		if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
		{
			sImagePath = pMemberInfo.m_sHeadResourceFile;
		}
	}
#endif
	Image * pImage = NULL;
	if (bIsMemberAccount)
	{
		if (!sImagePath.empty())
		{
			pImage = libEbc::LoadImageFromFile(sImagePath.c_str());
			if (pImage==NULL)
			{
				USES_CONVERSION;
				pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
			}
		}else
		{
			pImage = theApp.m_imageDefaultMember->Clone();
		}
	}else
	{
		pImage = theApp.m_imageDefaultContact->Clone();
	}
	return pImage;
}
tstring CPOPApp::GetUserHeadFilePath(mycp::bigint nUserId,const tstring& sAccount) const
{
	CEBString sImagePath;
	bool bIsMemberAccount = false;
	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_MemberInfo> pMemberInfo;
	if (nUserId>0)
		pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(nUserId);
	if (pMemberInfo == NULL && !sAccount.empty())
		pMemberInfo = theEBClientCore->EB_GetMemberInfoByAccount2(sAccount.c_str());
	if (pMemberInfo != NULL)
	{
		bIsMemberAccount = true;
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
	if (nUserId>0)
		bFind = theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,nUserId);
	if (!bFind && !sAccount.empty())
		bFind = theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,sAccount.c_str());
	if (bFind)
	{
		bIsMemberAccount = true;
		pOutLineState = pMemberInfo.m_nLineState;
		if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
		{
			sImagePath = pMemberInfo.m_sHeadResourceFile;
		}
	}
#endif
	if (bIsMemberAccount)
	{
		if (sImagePath.empty())
		{
			sImagePath = this->GetAppPath() + _T("/img/defaultmember.png");
			if (::PathFileExists(sImagePath.c_str()))
				sImagePath = this->GetAppPath() + _T("/img/defaultcontact.png");
		}
	}else
	{
		sImagePath = this->GetAppPath() + _T("/img/defaultcontact.png");
	}
	return sImagePath;
}

bool CPOPApp::GetMainFrameShowed(void) const
{
	CPOPDlg* pMainDlg = (CPOPDlg*)m_pMainWnd;
	return pMainDlg->GetMainFrameShowed();
}
bool CPOPApp::GetWorkFrameShowed(void) const
{
	CPOPDlg* pMainDlg = (CPOPDlg*)m_pMainWnd;
	return pMainDlg->GetWorkFrameShowed();
}
int CALLBACK CPOPApp::TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CTreeItemInfo * pItemInfo1 = (const CTreeItemInfo*)lParam1;
	const CTreeItemInfo * pItemInfo2 = (const CTreeItemInfo*)lParam2;
	if (pItemInfo1==NULL || pItemInfo2==NULL) return 1;
	if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		const bool bAppFrameShowed = theApp.GetWorkFrameShowed();
#ifdef USES_SUPPORT_UI_STYLE
		if (bAppFrameShowed || (pItemInfo1->m_nSubType==1 && pItemInfo2->m_nSubType==1))
#else
		if (bAppFrameShowed)
#endif
			return pItemInfo1->m_dwItemData<pItemInfo2->m_dwItemData?1:-1;
		else
			return pItemInfo1->m_dwItemData>pItemInfo2->m_dwItemData?1:-1;
	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// 群组（部门）排序
		if (pItemInfo1->m_nSubType!=pItemInfo2->m_nSubType)						// a 先按类型排序
			return pItemInfo1->m_nSubType>pItemInfo2->m_nSubType?1:-1;
		else if (pItemInfo1->m_nIndex!=pItemInfo2->m_nIndex)					// b 再按排序比较
			return pItemInfo1->m_nIndex<pItemInfo2->m_nIndex?1:-1;	// 排序值高显示在前面
		else																													// c 最后按名称gbk排序
			return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
	}else if ((pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) ||
		(pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT))
	{
		if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
		{
			// 员工按照权限类型比较排序
			if (pItemInfo1->m_nIndex!=pItemInfo2->m_nIndex)								// a 先按排序比较
			{
				return pItemInfo1->m_nIndex<pItemInfo2->m_nIndex?1:-1;
			}else if (pItemInfo1->m_nSubType!=pItemInfo2->m_nSubType)			// b 再按类型排序
			{
				return pItemInfo1->m_nSubType>pItemInfo2->m_nSubType?-1:1;
			}
		}else
		{
			// 联系人，按照有没有UID排序；
			if (pItemInfo2->m_nUserId>0 && pItemInfo1->m_nUserId==0) return 1; 				// 第2个有UID，对换位置
			else if (pItemInfo1->m_nUserId>0 && pItemInfo2->m_nUserId==0) return -1;	// 第2个0，不用对换位置
		}

		// 员工比较，先比较在线状态
		if ((pItemInfo1->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo2->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
			(pItemInfo1->m_dwItemData>=EB_LINE_STATE_BUSY && pItemInfo2->m_dwItemData<EB_LINE_STATE_BUSY))
		{
			// a 第一个在线，第二个其他 不用换位置
			// b 第一个离开/忙，第二个离线 不用换位置
			return -1;
		}else if ((pItemInfo2->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo1->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
			(pItemInfo2->m_dwItemData>=EB_LINE_STATE_BUSY && pItemInfo1->m_dwItemData<EB_LINE_STATE_BUSY))
		{
			// a 第2个在线，第1个其他 对调位置
			// b 第2个离开/忙，第1个离线 对调位置
			return 1;
		}
		return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
		//if (pItemInfo1->m_dwItemData==pItemInfo2->m_dwItemData)
		//{
		//	// 相同在线状态，比较名称
		//	return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
		//}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo2->m_dwItemData<=EB_LINE_STATE_OFFLINE)
		//{
		//	// 第一个在线，第二个离线，不用换位置
		//	return -1;
		//}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo1->m_dwItemData<=EB_LINE_STATE_OFFLINE)
		//{
		//	// 第二个在线，第一个离线，对调位置
		//	return 1;
		////}else if (pItemInfo1->m_dwItemData<=EB_LINE_STATE_OFFLINE)
		//////}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_OFFLINE)
		////{
		////	// 第一个离线，对调位置
		////	return 1;
		////}else if (pItemInfo2->m_dwItemData<=EB_LINE_STATE_OFFLINE)
		//////}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_OFFLINE)
		////{
		////	// 第二个离线，不用换位置
		////	return -1;
		////}else if (pItemInfo1->m_dwItemData>EB_LINE_STATE_OFFLINE)
		//////}else if (pItemInfo1->m_dwItemData!=EB_LINE_STATE_OFFLINE)
		////{
		////	// 第一个不是离线，不用换位置
		////	return -1;
		////}else if (pItemInfo2->m_dwItemData>EB_LINE_STATE_OFFLINE)
		//////}else if (pItemInfo2->m_dwItemData!=EB_LINE_STATE_OFFLINE)
		////{
		////	// 第二个不是离线，对调位置
		////	return 1;
		//}
		//return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		// 第一个是部门（目录），排前面（不用调位置）
		return -1;
	}else if (pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		// 第二个是部门（目录），对调位置
		return 1;
	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE)
	{
		return pItemInfo1->m_nBigId>=pItemInfo2->m_nBigId?-1:1;		// 时间大，排前面
		//return strcmp(pItemInfo2->m_sAccount.c_str(), pItemInfo1->m_sAccount.c_str());	// 时间最新排前面
	}
	return 0;
}

void CPOPApp::SetLineState2Db(EB_USER_LINE_STATE nLineState)
{
	if (m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("update user_login_record_t set linestate=%d where account='%s'"),nLineState,theApp.GetLogonAccount());
		m_pBoEB->execsql(sSql);
		m_pBoEB->close();
	}
}

void CPOPApp::DeleteDbRecord(eb::bigint sId, bool bIsAccount)
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
void CPOPApp::DeleteDbRecord(eb::bigint sMsgId)
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

void CPOPApp::OpenGroupShareWindow(eb::bigint sGroupCode,const CWnd* pWndLayout)
{
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (!m_pResourceMgr.find(sGroupCode,pDlgResourceMgr))
	{
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgResourceMgr = new CDlgResourceMgr(pParent);
		pDlgResourceMgr->Create(CDlgResourceMgr::IDD,pParent);
		m_pResourceMgr.insert(sGroupCode,pDlgResourceMgr);
	}
	const int nScreenWidth = GetScreenWidth();
	const int const_default_width = 520;
	CRect rect;
	pWndLayout->GetWindowRect(&rect);
	rect.left = (nScreenWidth-const_default_width)/2;
	rect.right = rect.left + const_default_width;
	pDlgResourceMgr->MoveWindow(&rect);
	pDlgResourceMgr->SetCircle();
	pDlgResourceMgr->LoadGroupResource(sGroupCode);
}

void CPOPApp::OpenMyShareWindow(const CWnd* pWndLayout)
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
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgResourceMgr = new CDlgResourceMgr(pParent);
		pDlgResourceMgr->Create(CDlgResourceMgr::IDD,pParent);
		m_pResourceMgr.insert(sLogonAccount,pDlgResourceMgr);
	}else
	{
		pDlgResourceMgr->SetCtrlColor();
	}
	const int nScreenWidth = GetScreenWidth();
	const int const_default_width = 520;
	CRect rect;
	pWndLayout->GetWindowRect(&rect);
	rect.left = (nScreenWidth-const_default_width)/2;
	rect.right = rect.left + const_default_width;
	pDlgResourceMgr->MoveWindow(&rect);
	pDlgResourceMgr->SetCircle();
	pDlgResourceMgr->LoadMyResource();
	if (pDlgResourceMgr->IsIconic())
		pDlgResourceMgr->ShowWindow(SW_RESTORE);
	pDlgResourceMgr->SetForegroundWindow();
}
bool CPOPApp::OpenSubscribeFuncWindow(const EB_SubscribeFuncInfo& pSubscribeFuncInfo,const std::string& sPostData,const std::string& sParameters,HWND hwndFrom)
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
		//if (this->GetColorGradient())
		//	sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetFuncColor()),GetGValue(theApp.GetFuncColor()),GetBValue(theApp.GetFuncColor()));
		//else
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
			CDlgFuncWindow pFuncWindow;
			pFuncWindow.m_nFuncBrowserType = pSubscribeFuncInfo.m_nBrowserType;
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
			(pSubscribeFuncInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME && m_bHideMainFrame && m_nDefaultUIStyleType == EB_UI_STYLE_TYPE_OFFICE))
		{
			if (GetAutoOpenSubId()==pSubscribeFuncInfo.m_nSubscribeId)
			{
				if (m_pAutoOpenFuncWindow.get()!=NULL)
				{
					if (!m_pAutoOpenFuncWindow->IsWindowVisible())
						m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
				}else
				{
					CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
					m_pAutoOpenFuncWindow = CDlgFuncWindow::pointer(new CDlgFuncWindow(pParent,false));
					m_pAutoOpenFuncWindow->m_nFuncBrowserType = pSubscribeFuncInfo.m_nBrowserType;
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
				CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
				CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
				pFuncWindow->m_nFuncBrowserType = pSubscribeFuncInfo.m_nBrowserType;
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
			CPOPDlg* pMainDlg = (CPOPDlg*)m_pMainWnd;
			pMainDlg->AddAppUrl(true,(LPCTSTR)sFullFuncurl,sPostData,pSubscribeFuncInfo,false,hwndFrom);
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
CString CPOPApp::GetSubscribeFuncUrl(const eb::bigint nSubId, const CString& sParameter) const
{
#ifdef USES_EBCOM_TEST
	const CEBString sFuncUrl = theEBClientCore->EB_GetSubscribeFuncUrl(nSubId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount.c_str(),m_nSelectGroupId).GetBSTR();
#else
	const CEBString sFuncUrl = theEBAppClient.EB_GetSubscribeFuncUrl(nSubId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId);
#endif
	if (!sFuncUrl.empty())
	{
		CString sFullFuncurl;
		//if (this->GetColorGradient())
		//	sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetFuncColor()),GetGValue(theApp.GetFuncColor()),GetBValue(theApp.GetFuncColor()));
		//else
			sFullFuncurl.Format(_T("%s&color=%02x%02x%02x"),sFuncUrl.c_str(),GetRValue(theApp.GetMainColor()),GetGValue(theApp.GetMainColor()),GetBValue(theApp.GetMainColor()));
		if (!sParameter.IsEmpty())
		{
			sFullFuncurl.Append(_T("&"));
			sFullFuncurl.Append(sParameter);
		}
		return sFullFuncurl;
	}
	return "";
}

bool CPOPApp::OpenAutoSubscribeFunc(const EB_SubscribeFuncInfo& pSubscribeFuncInfo)
{
	if (m_pAutoOpenFuncWindow.get()!=NULL)
	{
		if (!m_pAutoOpenFuncWindow->IsWindowVisible())
			m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
		return true;
	}
	return OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
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
//void CPOPApp::ShowAutoOpenSubscribeFunc(int nCmdShow)
//{
//	if (m_pAutoOpenFuncWindow!=NULL)
//	{
//		if (nCmdShow==SW_SHOW && m_pAutoOpenFuncWindow->IsIconic())
//			m_pAutoOpenFuncWindow->ShowWindow(SW_RESTORE);
//		else
//			m_pAutoOpenFuncWindow->ShowWindow(nCmdShow);
//	}
//}
void CPOPApp::ShowHideAutoOpenSubscribeFunc(bool bShow)
{
	if (m_pAutoOpenFuncWindow.get()!=NULL)
	{
		if (bShow)
		{
			if (m_pAutoOpenFuncWindow->IsIconic())
				m_pAutoOpenFuncWindow->ShowWindow(SW_RESTORE);
			else
				m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
		}else
		{
			m_pAutoOpenFuncWindow->ShowWindow(SW_HIDE);
		}
		//if (m_pAutoOpenFuncWindow->IsWindowVisible())
		//	m_pAutoOpenFuncWindow->ShowWindow(SW_HIDE);
		//else
		//	m_pAutoOpenFuncWindow->ShowWindow(SW_SHOW);
	}
}

bool CPOPApp::IsEnterpriseuserUser(void)
{
#ifdef USES_EBCOM_TEST
	return theEBClientCore->EB_IsEnterpriseUser(0)?true:false;
#else
	return theEBAppClient.EB_IsEnterpriseUser();
#endif
}

void CPOPApp::UpdateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType)
{
	// nAckType=0 对方收到消息
	// nAckType=4 请求撤回消息
	// nAckType=6 请求“个人收藏”消息
	// nAckType=7 请求“群收藏”消息
	if (m_pBoUsers.get()!=NULL)
	{
		CString sSql;
		if (nAckType==6)
		{
			// 个人收藏
		}
		else if (nAckType==7)
		{
			// 群收藏
		}
		else if (nAckType==4)
			sSql.Format(_T("UPDATE msg_record_t SET msg_name='',msg_text='',read_flag=read_flag|%d WHERE msg_id=%lld AND from_uid=%lld AND (read_flag&%d)=0"),EBC_READ_FLAG_WITHDRAW,nMsgId,nFromUserId,EBC_READ_FLAG_WITHDRAW);
		else if (nAckType==0)	// ?
			sSql.Format(_T("UPDATE msg_record_t SET read_flag=read_flag|%d WHERE msg_id=%lld AND dep_code=0 AND to_uid=%lld AND (read_flag&%d)=0"),EBC_READ_FLAG_RECEIPT,nMsgId,nFromUserId,EBC_READ_FLAG_RECEIPT);
		m_pBoUsers->execute(sSql);
	}
}

bool CPOPApp::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,Gdiplus::Image*& pImage3,int& pState) const
{
	CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_SUBINFO)
	{
		if (pTreeItemInfo->m_pHeadImage!=NULL)
		{
			pImage1 = pTreeItemInfo->m_pHeadImage;
			return true;
		}
		tstring sResFile(pTreeItemInfo->m_sGroupName);
		if (!PathFileExists(sResFile))
		{
			sResFile = m_sDefaultAppImage;
		}
		USES_CONVERSION;
		pTreeItemInfo->m_pHeadImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sResFile.c_str()));
		pImage1 = pTreeItemInfo->m_pHeadImage;
		return true;
		//tstring sResFile(pTreeItemInfo->m_sGroupName);
		//if (!PathFileExists(sResFile))
		//{
		//	sResFile = m_sDefaultAppImage;
		//}
		//USES_CONVERSION;
		//pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sResFile.c_str()));
		//return true;
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
	{
		if (pTreeItemInfo->m_pHeadImage!=NULL)
		{
			pImage1 = pTreeItemInfo->m_pHeadImage;
			return true;
		}

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
				pTreeItemInfo->m_pHeadImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sFaviconFile));
				pImage1 = pTreeItemInfo->m_pHeadImage;
				return true;
			}
		}
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
	{
		if (pTreeItemInfo->m_pHeadImage==NULL)
			pTreeItemInfo->m_pHeadImage = m_imageDefaultOrg->Clone();
		pImage1 = pTreeItemInfo->m_pHeadImage;
		return true;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		if (pTreeItemInfo->m_sId==0)	// ** 我的部门（群组）显示图标不是跑这里，跑 GetItemDrawOpenClose 获取
			return false;
		if (pTreeItemInfo->m_pHeadImage!=NULL)
		{
			pImage1 = pTreeItemInfo->m_pHeadImage;
			return true;
		}
		// MsgTip
		EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
		if (!theEBAppClient.EB_GetGroupType(pTreeItemInfo->m_sGroupCode,nGroupType))
			return false;
		switch(nGroupType)
		{
		case EB_GROUP_TYPE_DEPARTMENT:
			pTreeItemInfo->m_pHeadImage = m_imageDefaultDepartment->Clone();
			break;
		case EB_GROUP_TYPE_PROJECT:
			pTreeItemInfo->m_pHeadImage = m_imageDefaultProject->Clone();
			break;
		case EB_GROUP_TYPE_GROUP:
			pTreeItemInfo->m_pHeadImage = m_imageDefaultGroup->Clone();
			break;
		default:
			pTreeItemInfo->m_pHeadImage = m_imageDefaultTempGroup->Clone();
			break;
		}
		pImage1 = pTreeItemInfo->m_pHeadImage;
		return true;
		//return false;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
	{
		CEBString sImagePath;
		bool bFind = false;
		//bool bIsMemberAccount = false;
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
		//bool bFind = false;
		tstring sHeadMd5;
		EB_USER_LINE_STATE pLineStateTemp = EB_LINE_STATE_UNKNOWN;
		if (theEBAppClient.EB_GetContactHeadInfoByContactId(pTreeItemInfo->m_sId, sImagePath, sHeadMd5, pLineStateTemp))
		{
			if (GetAuthContact())
			{
				pOutLineState = pLineStateTemp;
			}
			if (PathFileExists(sImagePath.c_str()))
				bFind = true;
		}
		//EB_ContactInfo pContactInfo;
		//if (theEBAppClient.EB_GetContactInfo1(pTreeItemInfo->m_sId, &pContactInfo))
		//{
		//	//if (!GetAuthContact())
		//	if (GetAuthContact())
		//	{
		//		pOutLineState = pContactInfo.m_nLineState;
		//	}
		//	if (PathFileExists(pContactInfo.m_sHeadResourceFile.c_str()))
		//	{
		//		bFind = true;
		//		sImagePath = pContactInfo.m_sHeadResourceFile;
		//	}
		//}
		if (!bFind)
		{
			if (pTreeItemInfo->m_nUserId>0)
				bFind = theEBAppClient.EB_GetMemberHeadInfoByUserId(pTreeItemInfo->m_nUserId,sImagePath,sHeadMd5,pLineStateTemp);
			if (!bFind)
				bFind = theEBAppClient.EB_GetMemberHeadInfoByAccount(pTreeItemInfo->m_sAccount.c_str(),sImagePath,sHeadMd5,pLineStateTemp);
			if (bFind)
			{
				if (GetAuthContact())
					pOutLineState = pLineStateTemp;
			}
			//EB_MemberInfo pMemberInfo;
			//if (pTreeItemInfo->m_nUserId>0)
			//	bFind = theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,pTreeItemInfo->m_nUserId);
			//if (!bFind)
			//	bFind = theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,pTreeItemInfo->m_sAccount.c_str());
			//if (bFind)
			//{
			//	//bIsMemberAccount = true;
			//	if (GetAuthContact())
			//		//if (!GetAuthContact())
			//		pOutLineState = pMemberInfo.m_nLineState;
			//	if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
			//	{
			//		sImagePath = pMemberInfo.m_sHeadResourceFile;
			//	}
			//}
		}
#endif
		if (bFind)
		//if (bIsMemberAccount)
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
			if (!sImagePath.empty() && PathFileExists(sImagePath.c_str()))
			{
				if (pTreeItemInfo->m_sHeadMd5!=sHeadMd5 || pTreeItemInfo->m_pHeadImage==NULL)
				{
					Gdiplus::Image * pUserHead = libEbc::LoadImageFromFile(sImagePath.c_str());
					if (pUserHead==NULL)
					{
						USES_CONVERSION;
						pUserHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
					}
					pTreeItemInfo->UpdateHead(pUserHead,sHeadMd5);
					//USES_CONVERSION;
					//pTreeItemInfo->UpdateHead(new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str())),sHeadMd5);
				}
			}else
			{
				if (!pTreeItemInfo->m_sHeadMd5.empty() || pTreeItemInfo->m_pHeadImage==NULL)
					pTreeItemInfo->UpdateHead(m_imageDefaultContact->Clone(),"");
			}
			pImage1 = pTreeItemInfo->m_pHeadImage;

			//if (!sImagePath.empty() && PathFileExists(sImagePath.c_str()))
			//{
			//	USES_CONVERSION;
			//	pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
			//}else
			//{
			//	pImage1 = m_imageDefaultContact->Clone();
			//	//sImagePath = (LPCTSTR)this->GetAppPath();
			//	//sImagePath.append("\\img\\h02.png");
			//	//USES_CONVERSION;
			//	//pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
			//	////pImage1 = m_imageDefaultMember->Clone();
			//}
		}else
		{
			//// contact
			//if (pCallRecordInfo->m_nFromUserId>0)
			//{
			//	EB_ContactInfo pContactInfo;
			//	theEBAppClient.EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId,&pContactInfo);
			//	pOutLineState = pContactInfo.m_nLineState;
			//}else if (!theApp.GetAuthContact())
			//{
			//	pOutLineState = EB_LINE_STATE_ONLINE_NEW;
			//}

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
			if (!pTreeItemInfo->m_sHeadMd5.empty() || pTreeItemInfo->m_pHeadImage==NULL)
				pTreeItemInfo->UpdateHead(m_imageDefaultContact->Clone(),"");
			pImage1 = pTreeItemInfo->m_pHeadImage;
			//pImage1 = m_imageDefaultContact->Clone();
		}
		return true;
	}
	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
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
		tstring sHeadMd5;
		theEBAppClient.EB_GetMemberHeadInfoByMemberCode(pTreeItemInfo->m_sMemberCode,sImagePath,sHeadMd5,pOutLineState);
		//EB_MemberInfo pMemberInfo;
		//if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pTreeItemInfo->m_sMemberCode))
		//{
		//	//sDescription = pMemberInfo.m_sDescription;
		//	pOutLineState = pMemberInfo.m_nLineState;
		//	if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
		//	{
		//		sImagePath = pMemberInfo.m_sHeadResourceFile;
		//	}
		//}
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
		if ((pTreeItemInfo->m_nExtData&CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH)!=0)
			pImage3 = m_imageStateForbid;

		if (!sImagePath.empty() && PathFileExists(sImagePath.c_str()))
		{
			//if (!sHeadMd5.empty())
			//{
			//	mycp::bigint nFileSizeTemp = 0;
			//	tstring sFileMd5Temp;
			//	if (libEbc::GetFileMd5(sImagePath.c_str(),nFileSizeTemp,sFileMd5Temp) && sFileMd5Temp!=sHeadMd5)
			//	{
			//		sHeadMd5 = sFileMd5Temp;
			//	}
			//}

			if (pTreeItemInfo->m_sHeadMd5!=sHeadMd5 || pTreeItemInfo->m_pHeadImage==NULL)
			{
				Gdiplus::Image * pUserHead = libEbc::LoadImageFromFile(sImagePath.c_str());
				if (pUserHead==NULL)
				{
					USES_CONVERSION;
					pUserHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
				}
				pTreeItemInfo->UpdateHead(pUserHead,sHeadMd5);
				//USES_CONVERSION;
				//pTreeItemInfo->UpdateHead(new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str())),sHeadMd5);
			}
		}else
		{
			if (!pTreeItemInfo->m_sHeadMd5.empty() || pTreeItemInfo->m_pHeadImage==NULL)
				pTreeItemInfo->UpdateHead(m_imageDefaultMember->Clone(),"");
		}
		pImage1 = pTreeItemInfo->m_pHeadImage;

		//if (!sImagePath.empty() && PathFileExists(sImagePath.c_str()))
		//{
		//	USES_CONVERSION;
		//	pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
		//}else
		//{
		//	//sImagePath = (LPCTSTR)this->GetAppPath();
		//	//sImagePath.append("\\img\\entlogo.png");
		//	//USES_CONVERSION;
		//	//pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
		//	//libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("PNG"),pImage1);
		//	pImage1 = m_imageDefaultMember->Clone();
		//	//return libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("png"),pImage1)?true:false;
		//}
		return true;
	}
	return false;
}
bool CPOPApp::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pIcon1, HICON& pIcon2,int& pState) const
{
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	//EB_ResourceInfo pResourceInfo;
	//if (!theEBAppClient.EB_GetResourceInfo(pTreeItemInfo->m_sId,&pResourceInfo)) return NULL;

	switch (pTreeItemInfo->m_nItemType)
	{
	case CTreeItemInfo::ITEM_TYPE_DIR:
		{
			if (theDefaultDirIcon == NULL)
			{
				SHFILEINFO sfi; 
				ZeroMemory(&sfi,sizeof(sfi)); 
				const DWORD ret = SHGetFileInfo(theApp.GetAppDataPath(),
					0, 
					&sfi, 
					sizeof(sfi), 
					SHGFI_ICON);
				if (ret == 1)
				{
					theDefaultDirIcon = sfi.hIcon;
				}
			}
			pIcon1 = theDefaultDirIcon;
			return pIcon1==NULL?false:true;
		}break;
	case CTreeItemInfo::ITEM_TYPE_FILE:
		{
			pIcon1 = GetFileHIcon(pTreeItemInfo->m_sName);
			return pIcon1==NULL?false:true;
		}break;
	case CTreeItemInfo::ITEM_TYPE_NOTE:
		{
		}break;
	default:
		break;
	}
	return false;
}

bool CPOPApp::GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,const CBitmap** pBitmapOpened,const CBitmap** pBitmapClosed) const
{
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	//return pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		if (m_bmpTreeDirOpened.GetSafeHandle()!=NULL)
			*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeDirOpened);
		if (m_bmpTreeDirClosed.GetSafeHandle()!=NULL)
			*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeDirClosed);
		return true;
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		switch (pTreeItemInfo->m_nSubType)
		{
		case -2:	// msg tip
			return false;
		case -1:	// contact group
			{
				if (m_bmpTreeDirOpened.GetSafeHandle()!=NULL)
					*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeDirOpened);
				if (m_bmpTreeDirClosed.GetSafeHandle()!=NULL)
					*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeDirClosed);
			}break;
		case EB_GROUP_TYPE_DEPARTMENT:
			{
				if (m_bmpTreeType0Opened.GetSafeHandle()!=NULL)
					*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeType0Opened);
				if (m_bmpTreeType0Closed.GetSafeHandle()!=NULL)
					*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeType0Closed);
			}break;
		case EB_GROUP_TYPE_PROJECT:
			{
				if (m_bmpTreeType1Opened.GetSafeHandle()!=NULL)
					*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeType1Opened);
				if (m_bmpTreeType1Closed.GetSafeHandle()!=NULL)
					*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeType1Closed);
			}break;
		case EB_GROUP_TYPE_GROUP:
			{
				if (m_bmpTreeType2Opened.GetSafeHandle()!=NULL)
					*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeType2Opened);
				if (m_bmpTreeType2Closed.GetSafeHandle()!=NULL)
					*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeType2Closed);
			}break;
		default:	// EB_GROUP_TYPE_TEMP
			{
				if (m_bmpTreeType9Opened.GetSafeHandle()!=NULL)
					*pBitmapOpened = const_cast<CBitmap*>(&m_bmpTreeType9Opened);
				if (m_bmpTreeType9Closed.GetSafeHandle()!=NULL)
					*pBitmapClosed = const_cast<CBitmap*>(&m_bmpTreeType9Closed);
			}break;
		}
		return true;
	}
	return false;
}
COLORREF CPOPApp::GetItemTextColor(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, COLORREF colorDefault) const
{
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return colorDefault;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		if (pTreeItemInfo->m_nSubType>1)
			return RGB(255,0,128);
		//else if (pTreeItemInfo->m_nSubType>1)
		//	return RGB(255,128,128);
		else if (pTreeItemInfo->m_nSubType==1)	// 自己 
			return RGB(0,0,255);
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		return theDefaultTextBlackColor;
		//if (pTreeItemInfo->m_nSubType==(int)EB_GROUP_TYPE_DEPARTMENT)
		//{
		//	if (pTreeItemInfo->m_nBigId>0)	// m_nMyEmpId
		//		return theApp.GetHotColor2();
		//		//return RGB(86,190,245);
		//		//return RGB(0,0,255);
		//	else
		//		return RGB(32,32,32);
		//}else if (pTreeItemInfo->m_nSubType==(int)EB_GROUP_TYPE_PROJECT)
		//	return RGB(255,0,128);
		//else if (pTreeItemInfo->m_nSubType==(int)EB_GROUP_TYPE_GROUP)
		//	return RGB(32,32,32);
		//	//return RGB(0,64,128);
		//else// if (pTreeItemInfo->m_nSubType==(int)EB_GROUP_TYPE_TEMP)
		//	return RGB(64,64,64);
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE)
	{
		return pTreeItemInfo->m_nSubType==-1?RGB(255,0,128):colorDefault;
	}
	return colorDefault;
}

//CDlgVideoConference::pointer CPOPApp::GetDlgVideoConference(const CEBCCallInfo::pointer& pEbCallInfo,bool bBuildNew)
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

void CPOPApp::SetMainColor(COLORREF v)
{
	m_nMainColor=v;
	float h,s,l;
	::RGBtoHSL(GetRValue(m_nMainColor), GetGValue(m_nMainColor), GetBValue(m_nMainColor),&h,&s,&l);
	m_nFlatBgColor = theDefaultFlatBgColor;//RGB(250,250,250);
	//m_nFuncColor = HSLtoRGB(h, s, theDefaultBgColorTitleLight1);

	m_nHotColor = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
	m_nBgColor = HSLtoRGB(h, s, 0.97);
	m_nHotColor1 = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+0.05)));
	m_nHotColor2 = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+0.10)));
	m_nPreColor = HSLtoRGB(h, s, min(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
	m_nSelColor = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+0.3)));
	m_nTrueColor = HSLtoRGB(h, s, min(theDefaultMaxBtnTrue,(l+theDefaultBtnTrue)));
	m_nNormalColor = theDefaultBtnNormal==0.0?theApp.GetMainColor():HSLtoRGB(h, s, (l+theDefaultBtnNormal));
	//m_nBgTitle0 = ::HSLtoRGB(h, s, theDefaultBgColorTitleLight0);
	//m_nBgColor1 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight1);
	//m_nBgColor2 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight2);
	//m_nBgColor3 = ::HSLtoRGB(h, s, theDefaultBgColorBgLight3);
	//m_nOkColor0 = ::HSLtoRGB(h, s, theDefaultBtnOkColorLight0);
	////m_nOkColor1 = ::HSLtoRGB(h, s, theDefaultBtnOkColorLight1);
}

eb::bigint CPOPApp::GetGroupMsgSugId(void)
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
eb::bigint CPOPApp::GetFindAppSugId(void)
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
eb::bigint CPOPApp::GetMyCollectionSugId(void)
{
	//if (theApp.IsLogonVisitor()) return 0;
	if (m_nMyCollectionSubId==0)
	{
#ifdef USES_EBCOM_TEST
		//unsigned long pMyCollectionSubId = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID);
		//if (pMyCollectionSubId!=NULL && strlen((const char*)pMyCollectionSubId)>0)
		//{
		//	m_nMyCollectionSubId = eb_atoi64((const char*)pMyCollectionSubId);
		//	theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID,pMyCollectionSubId);
		//}
#else
		unsigned long pMyCollectionSubId = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID,&pMyCollectionSubId);
		if (pMyCollectionSubId!=NULL && strlen((const char*)pMyCollectionSubId)>0)
		{
			m_nMyCollectionSubId = eb_atoi64((const char*)pMyCollectionSubId);
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID,pMyCollectionSubId);
		}
#endif
	}
	return m_nMyCollectionSubId;
}

#define MAX_LOG_SIZE 1024
void CPOPApp::LogMessage(const char * format,...)
{
	if (!m_bLogDebug) return;
	try
	{
		char debugmsg[MAX_LOG_SIZE];
		memset(debugmsg, 0, MAX_LOG_SIZE);
		va_list   vl;
		va_start(vl, format);
		int len = vsnprintf(debugmsg, MAX_LOG_SIZE-1, format, vl);
		va_end(vl);
		if (len > MAX_LOG_SIZE)
			len = MAX_LOG_SIZE;
		debugmsg[len] = '\0';

		if (m_pLogDebug==NULL)
		{
			char sDebugFile[260];
			if (theApp.IsLogonVisitor())
				sprintf(sDebugFile, "%s\\debug_ebc_visitor.txt", m_sAppPath);
			else
				sprintf(sDebugFile, "%s\\debug_ebc_%lld.txt", m_sAppPath, theApp.GetLogonUserId());
			m_pLogDebug = fopen(sDebugFile,"w");
			if (m_pLogDebug==NULL)
				return;
		}
		const time_t tNow = time(0);
		const struct tm *newtime = localtime(&tNow);
		char lpszDateTime[64];
		sprintf(lpszDateTime, "%02d-%02d %02d:%02d:%02d ",newtime->tm_mon+1,newtime->tm_mday, newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
		fwrite(lpszDateTime,1,strlen(lpszDateTime),m_pLogDebug);
		fwrite(debugmsg,1,strlen(debugmsg),m_pLogDebug);
		fflush(m_pLogDebug);
	}catch(std::exception const &)
	{
	}catch(...)
	{
	}
}

void CPOPApp::OnAbout()
{
	CString sText;
	sText.Format(_T("当前版本：%s\r\nCopyright (C) 2012-2017"),theSetting.GetVersion().c_str());
	CDlgMessageBox::EbMessageBox(AfxGetMainWnd(),"关于恩布互联 Entboost.com",sText,CDlgMessageBox::IMAGE_ENTBOOST,10);
	//if (m_dlgAbout==NULL)
	//{
	//	m_dlgAbout = new CAboutDlg();
	//	m_dlgAbout->Create(CAboutDlg::IDD,::AfxGetMainWnd());
	//}
	//m_dlgAbout->ShowWindow(SW_SHOW);
}
