
// ebsc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "DesktopWindow.h"


// CebscApp:
// See ebsc.cpp for the implementation of this class
//
const CString theWindowName = _T("entboost_sc");

class CebscApp : public CWinAppEx
{
public:
	CebscApp();

	Gdiplus::Image * m_imageScreen;
	CDesktopWindow m_pDesktopWindow;
	const CString & GetAppPath(void) const {return m_sAppPath;}

// Overrides
	public:
	virtual BOOL InitInstance();

private:
	CString m_sAppPath;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CebscApp theApp;