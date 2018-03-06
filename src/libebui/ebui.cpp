#include "StdAfx.h"
#include "ebui.h"
#include "EBUIApp.h"

//class CEBUIManager
//{
//public:
//
//};

Cebui::Cebui(void)
{
}

Cebui::~Cebui(void)
{
	IM_Logout();
}

//bool Cebui::EBUI_Create(void)
//{
//	CEBUIManager * pEBUIManager = (CEBUIManager*)m_handle;
//	if (pEBUIManager==NULL)
//	{
//		pEBUIManager = new CEBUIManager();
//	}
//	return EBUI_IsCreated();
//}
//bool Cebui::EBUI_IsCreated(void) const
//{
//	return (m_handle==NULL)?false:true;
//}
//void Cebui::EBUI_Destroy(void)
//{
//	CEBUIManager * pEBUIManager = (CEBUIManager*)m_handle;
//	m_handle = NULL;
//	if (pEBUIManager!=NULL)
//	{
//		delete pEBUIManager;
//	}
//}
void Cebui::SetHandler(CEBUIHandler* pHandler)
{
	theApp.m_pHandler = pHandler;
}
BOOL Cebui::IM_OAuth(const char* lpszParameter)
{
	AFX_SWITCH_INSTANCED();
	return theApp.IM_OAuth(lpszParameter);
}
BOOL Cebui::IM_Login(const char* lpszAccount, const char* lpszPasswd)
{
	AFX_SWITCH_INSTANCED();
	return theApp.IM_Login(lpszAccount,lpszPasswd);
}
BOOL Cebui::IM_IsLogined(void) const
{
	return theApp.IM_IsLogined();
}
void Cebui::IM_Logout(void)
{
	AFX_SWITCH_INSTANCED();
	theApp.IM_Logout();
}

BOOL Cebui::MainFrame_Create(HWND hParentHwnd, UINT nMsgId)
{
	AFX_SWITCH_INSTANCED();
	return theApp.MainFrame_Create(hParentHwnd, nMsgId);
}
HWND Cebui::MainFrame_GetSafeHwnd(void) const
{
	AFX_SWITCH_INSTANCED();
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return theApp.MainFrame_GetSafeHwnd();
}
//void Cebui::MainFrame_Destroy(void)
//{
//	AFX_SWITCH_INSTANCED();
//	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	theApp.MainFrame_Destroy();
//}
void Cebui::MainFrame_ShowWindow(int nShow)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	theApp.MainFrame_ShowWindow(nShow);
}
void Cebui::MainFrame_MoveWindow(int x, int y, int w, int h, BOOL bRepaint)
{
	AFX_SWITCH_INSTANCED();
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	theApp.MainFrame_MoveWindow(x, y, w, h, bRepaint);
}
void Cebui::MainFrame_MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
	AFX_SWITCH_INSTANCED();
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	theApp.MainFrame_MoveWindow(lpRect, bRepaint);
}
void Cebui::MainFrame_Invalidate(BOOL bErase)
{
	AFX_SWITCH_INSTANCED();
	theApp.MainFrame_Invalidate(bErase);
}
void Cebui::ChatWindow_SetTopMost(bool bTopMost)
{
	AFX_SWITCH_INSTANCED();
	theApp.ChatWindow_SetTopMost(bTopMost);
}
void Cebui::SetMainColor(COLORREF nMainColor)
{
	AFX_SWITCH_INSTANCED();
	theApp.SetMainColor(nMainColor);
}
COLORREF Cebui::GetMainColor(void) const
{
	AFX_SWITCH_INSTANCED();
	return theApp.GetMainColor();
}

void Cebui::SetTreeListColor(COLORREF nSelectColor, COLORREF nHotColor, COLORREF nBoderColor)
{
	AFX_SWITCH_INSTANCED();
	return theApp.SetTreeListColor(nSelectColor, nHotColor, nBoderColor);
}
