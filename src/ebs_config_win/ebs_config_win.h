
// ebs_config_win.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cebs_config_winApp:
// See ebs_config_win.cpp for the implementation of this class
//

class Cebs_config_winApp : public CWinAppEx
{
public:
	Cebs_config_winApp();

	const CString & GetAppPath(void) const {return m_sAppPath;}
	const CString & GetAppUpPath(void) const {return m_sAppUpPath;}	// 上一级目录，不带\

// Overrides
	public:
	virtual BOOL InitInstance();

private:
	CString m_sAppPath;
	CString m_sAppUpPath;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cebs_config_winApp theApp;