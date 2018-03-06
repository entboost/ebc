
// ebd.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CebdApp:
// See ebd.cpp for the implementation of this class
//

class CebdApp : public CWinAppEx
{
public:
	CebdApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CebdApp theApp;
extern Cebrd::pointer theEBRD;