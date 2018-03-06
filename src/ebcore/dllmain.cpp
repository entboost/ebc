// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ebcore_i.h"
#include "dllmain.h"

CebcoreModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
//
//class CebcoreApp : public CWinApp
//{
//public:
//	virtual BOOL InitInstance(void);
//	virtual int ExitInstance(void);
//
//	DECLARE_MESSAGE_MAP()
//};
//
//
//BEGIN_MESSAGE_MAP(CebcoreApp, CWinApp)
//
//END_MESSAGE_MAP()
//
//CebcoreApp theApp;
//BOOL CebcoreApp::InitInstance(void)
//{
//	_AtlModule.DllMain(0, 0); 
//	return CWinApp::InitInstance();
//}
//
//int CebcoreApp::ExitInstance()
//{
//	_AtlModule.Term();
//	return CWinApp::ExitInstance();
//}
