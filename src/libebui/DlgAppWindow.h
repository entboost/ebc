#pragma once
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
#define EBWM_FRAME_WND_CLOSE	WM_USER+3
#define EBWM_FRAME_WND_TITLE	WM_USER+4
#define EBWM_FRAME_WND_OPEN		WM_USER+5
#define EBWM_FRAME_VIEW_ICO		WM_USER+6


// CDlgAppWindow dialog

class CDlgAppWindow : public CEbDialogBase
	, public CWebUICallback
{
	DECLARE_DYNAMIC(CDlgAppWindow)

public:
	CDlgAppWindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAppWindow();

	bool m_bOpenNewClose;
	EB_SubscribeFuncInfo m_pFuncInfo;
	void Navigate(const std::string& sUrl);
	CString GetLocationURL(void);
	void doRefresh(void);
	void goBack(void);
	void goForward(void);
	void SetWebFocus(void);
	void InvalidateRect2(LPRECT lpRect, BOOL bErase = TRUE);

	const CString& GetCurrentTitle(void) const {return m_sCurrentTitle;}
	const CString& GetFaviconFile(void) const {return m_sFaviconFile;}
	void OnLogonSuccess(void);

// Dialog Data
	enum { IDD = IDD_DLG_APPWINDOW };

protected:
	std::string m_sToNavigateUrl;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CExplorer m_pExplorer;
	CLabelEx m_labelStatusText;
	CWebUIController m_webUICtrl;
	bool m_bDocumentComplete;
	CString m_sCurrentTitle;
	CString m_sFaviconFile;
	bool m_bQuit;
	bool m_bSaveBrowseInfo;

	LRESULT OnMsgProcessFavicon(WPARAM wParam, LPARAM lParam);
	static bool DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified);
	// CWebUICallback
	virtual void OnStatusTextChange(BSTR sStatusText);
	virtual void OnTitleChange(BSTR sTitle);
	virtual void OnNewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);

	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL);
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel);
	////afx_msg void NewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel); 
	//afx_msg void NewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl); 
	//afx_msg void OnTitleChange(LPDISPATCH pDisp, VARIANT* URL);
	afx_msg void OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
