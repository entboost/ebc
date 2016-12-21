
// eblogin.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
using namespace entboost;

//#include "EBAppClient.h"
//#include "../include/EBParseSetting.h"

//const std::wstring theFontFamily = L"微软雅黑";
//const std::wstring theFontFamily = L"Times New Roman";

// CebloginApp:
// See eblogin.cpp for the implementation of this class
//
CString GetLineStateText(EB_USER_LINE_STATE nLineState);

/*
 * eblogin.exe 恩布客户端登录验证程序
 * APP 应用类
 * 
*//////////////////////////////////////////////////////

class CebloginApp : public CWinAppEx
{
public:
	CebloginApp();

	static BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image * & pImg);

	Gdiplus::Image * m_imageEntLogo;
	bo::CBodbHandler::pointer m_pBoEB;

	void SetMainColor(COLORREF v);
	COLORREF GetMainColor(void) const {return m_nMainColor;}
	COLORREF GetFlatBgColor(void) const {return m_nFlatBgColor;}
	void SetColorGradient(bool v) {m_bColorGradient=v;}
	bool GetColorGradient(void) const {return m_bColorGradient;}
	void SetColorFlat(bool v) {m_bColorFlat = v;}
	bool GetColorFlat(void) const {return m_bColorFlat;}

	//static void InvalidateParentRect(const CWnd* pWnd);
	static void GetScreenSize(int & pWidth, int & pHeight);
	static void InvalidateParentRect(const CWnd* pWnd);
	const CString & GetAppPath(void) const {return m_sAppPath;}
	const CString & GetAppDataPath(void) const {return m_sAppDataPath;}

	CString GetSettingFile(void) const;
	CString GetSettingIniFile(void) const;
	void EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue);
	CString EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault=NULL);
	void EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue);
	UINT EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault=0);
	bool IsLogonVisitor(void) const;
	void FreeEntLogo(void);
	void SetEntLogo(LPCTSTR lpszFilePath);	// NULL为默认LOGO
	bool IsEbDefaultEntLogo(void) const {return m_bEbDefaultEntLogo;}
	//void LoadSetting(void);

	CString m_sProductName;

	int m_nOpenRegister;
	bool m_bOpenVisitor;
	bool m_bLicenseUser;
	std::string m_sAccountPrefix;
	bool m_bSendRegMail;
// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_nMainColor;
	COLORREF m_nFlatBgColor;
	bool m_bColorGradient;
	bool m_bColorFlat;
	CString m_sAppPath;
	CString m_sAppDataPath;
	CString m_sUserMainPath;
	bool m_bEbDefaultEntLogo;
public:
	virtual int ExitInstance();
};

extern CEbcLanguages theEbcLanguage;

extern CebloginApp theApp;
#ifdef USES_EBCOM_TEST
extern CComPtr<IEBParseSetting>	theEBSetting;
extern CComPtr<IEBClientCore>	theEBClientCore;
#else
extern CEBParseSetting theSetting;
extern CEBAppClient theEBAppClient;
#endif
