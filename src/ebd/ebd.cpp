
// ebd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ebd.h"
#include "ebdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef USES_EBCOM_TEST
#ifdef _DEBUG
#pragma comment(lib, "libebrdd.lib")
#else
#pragma comment(lib, "libebrd.lib")
#endif
#endif

// CebdApp

BEGIN_MESSAGE_MAP(CebdApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CebdApp construction

CebdApp::CebdApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CebdApp object

CebdApp theApp;
Cebrd::pointer theEBRD;


// CebdApp initialization
BOOL CebdApp::InitInstance()
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

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//int na = sizeof(mycp::bigint);
	//int nb = sizeof(mycp::uint8);
	//int n1 = sizeof(tagSotpRtpCommand0);
	//int n2 = sizeof(tagSotpRtpCommand);
	//CString stext;
	//stext.Format(_T("%d,%d"),n1,n2);
	//AfxMessageBox(stext);
	theEBRD = Cebrd::create();
	//theEBRD.Start("127.0.0.1:18012",0);
	theEBRD->Start("114.215.181.249:17012",0);
	//AfxMessageBox(_T("222"));

	CebdDlg dlg;
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

	theEBRD.reset();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
