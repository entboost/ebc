
// ebsc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ebsc.h"
#include "ebscDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CebscApp

BEGIN_MESSAGE_MAP(CebscApp, CWinAppEx)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CebscApp construction

CebscApp::CebscApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	CString str;
	GetModuleFileName( NULL, str.GetBuffer(MAX_PATH), MAX_PATH);
	str.ReleaseBuffer( -1);
	str = str.Mid(0, str.ReverseFind('\\'));
	m_sAppPath = str;
	//m_sAppDataPath = m_sAppPath + _T("\\datas");

	m_imageScreen = NULL;
}


// The one and only CebscApp object

CebscApp theApp;


// CebscApp initialization

ULONG_PTR theGdiplusToken=0;
BOOL CebscApp::InitInstance()
{
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

	// 实现在登录口，保持单实例。
	HANDLE g_hMutex = CreateMutex(NULL,TRUE,_T("__entboost_sc_mutex__"));
	if(ERROR_ALREADY_EXISTS==GetLastError())
	{
		//找到另一个实例
		HWND l_hWnd = FindWindow(NULL, theWindowName);
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
		return FALSE;
	}

	std::string sWindowText;
	int nMsgID = 0;
	HWND hWindow = NULL;
	if (__argc >= 3)
	{
		sWindowText = __argv[1];
		nMsgID = atoi(__argv[2]);
		if (__argc >= 4)
			hWindow = (HWND)atoi(__argv[3]);
	}
	bool bMinWindow = false;
	HWND hMinWindow = NULL;
	if (__argc >= 6)
	{
		bMinWindow = atoi(__argv[4])==1?true:false;
		hMinWindow = (HWND)atoi(__argv[5]);
	}
	if (bMinWindow && hMinWindow!=NULL)
	{
		::ShowWindow(hMinWindow,SW_HIDE);
		//::ShowWindow(hMinWindow,SW_MINIMIZE);
	}

	AfxEnableControlContainer();
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);

	if (!m_pDesktopWindow.Create())
	{
		if (bMinWindow && hMinWindow!=NULL)
		{
			::ShowWindow(hMinWindow,SW_SHOW);
			//::ShowWindow(hMinWindow,SW_RESTORE);
		}
		return FALSE;
	}
	m_imageScreen = Gdiplus::Bitmap::FromHBITMAP(m_pDesktopWindow.CopyScreen(),NULL);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("entboost sc"));

	CebscDlg dlg;
	dlg.m_sWindowText = sWindowText;
	dlg.m_nMsgID = nMsgID;
	dlg.m_hWindow = hWindow;
	dlg.m_bMinWindow = bMinWindow;
	dlg.m_hMinWindow = hMinWindow;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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
	if (bMinWindow && hMinWindow!=NULL)
	{
		::ShowWindow(hMinWindow,SW_SHOW);
		//::ShowWindow(hMinWindow,SW_RESTORE);
	}

	if (m_imageScreen!=NULL)
	{
		delete m_imageScreen;
	}
	m_pDesktopWindow.Release();
	Gdiplus::GdiplusShutdown(theGdiplusToken);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
