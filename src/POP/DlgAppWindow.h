#pragma once
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
//#define EB_COMMAND_RAME_WND_CLOSE	WM_USER+3
//#define EB_COMMAND_RAME_WND_TITLE	WM_USER+4
//#define EB_COMMAND_RAME_WND_OPEN		WM_USER+5
//#define EB_COMMAND_RAME_WND_VIEW_ICO		WM_USER+6
#ifdef USES_LIBCEF
#include "cef/cefbrowser.h"
#endif

#include "PanelFind.h"

// CDlgAppWindow dialog
#define USES_EBDIALOGBASE	// ** 灰色跟这里没关系，是上一层窗口问题
#ifdef USES_EBDIALOGBASE
#define CPARENT_DIALOG CEbDialogBase
#else
#define CPARENT_DIALOG CDialog
#endif

class CDlgAppWindow : public CPARENT_DIALOG
	, public CWebUICallback
#ifdef USES_LIBCEF
	, public CCefBrowserHandler
	, public CFindHandler
#endif
{
	DECLARE_DYNAMIC(CDlgAppWindow)

public:
	CDlgAppWindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAppWindow();

	void SetCtrlColor(bool bInvalidate);

	bool m_bOpenNewClose;
	EB_SubscribeFuncInfo m_pFuncInfo;
	void Navigate(const std::string& sUrl, const std::string& sPostData);
	void SetUserData(UINT nUserData) {m_nUserData = nUserData;}
	UINT GetUserData(void) const {return m_nUserData;}
	void SetSaveBrowseTitle(bool b) {m_bSaveBrowseTitle = b;}
	CString GetLocationURL(void) const;
	void doRefresh(void);
	void goBack(void);
	void goForward(void);
	void SetWebFocus(bool bRefresh=false);
	void NotifyMoveOrResizeStarted(void);
	void OnLogonSuccess(void);
	void OnOffline(int nServerState);
	void InvalidateRect2(LPRECT lpRect, BOOL bErase = TRUE);
	void ChangeBrowserType(EB_BROWSER_TYPE nNewBrowserType);
	EB_BROWSER_TYPE GetBrowserType(void) const {return m_nBrowserType;}
	void changeBrowserType(void);
	bool canSaveHistory(void) const;
	void saveHistory(void);
	void OnMove(void);
	//void OnShowHide(bool bShow);

	const CString& GetCurrentTitle(void) const {return m_sCurrentTitle;}
	const CString& GetFaviconFile(void) const;
	void DoDownloadFavicon(void);

// Dialog Data
	enum { IDD = IDD_DLG_APPWINDOW };

protected:
	tstring m_sToNavigateUrl;
	tstring m_sPostData;
	UINT m_nUserData;
	bool m_bSaveBrowseTitle;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	EB_BROWSER_TYPE m_nBrowserType;
#ifdef USES_LIBCEF
	CCefBrowser m_pCefBrowser;
	double m_fZoomLevel;
#endif
	CExplorer m_pExplorer;
	CLabelEx m_labelStatusText;
	CWebUIController m_webUICtrl;
	bool m_bDocumentComplete;
	time_t m_tLoadOrIcoFinishedTime;
	//DWORD m_dwLoadingTime;
	int m_nShowImageType;	// 0:loading,1:default_page,2:downloading_icon,3:url_icon
	CString m_sCurrentTitle;
	CString m_sFaviconFile;
	bool m_bQuit;
	bool m_bSaveBrowseInfo;
	CPanelFind* m_pPanelFind;
	CPanelText * m_pPanelStatus;
	std::wstring m_sStatusMessage;

	CString m_sDownloadFaviconDomainName;
	CString m_sDownloadFaviconFaviconUrl;
	boost::shared_ptr<boost::thread> m_pDownloadFaviconThread;

	LRESULT OnMsgProcessFavicon(WPARAM wParam, LPARAM lParam);
	//static bool DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified);
	// CWebUICallback
	virtual void OnStatusTextChange(BSTR sStatusText);
	virtual void OnTitleChange(BSTR sTitle);
	virtual void OnNewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
	void ShowZoomLevel(void);
#ifdef USES_LIBCEF
	void MovePanelFind(int cx);
	virtual bool OnJSDialog(const wchar_t* sOriginUrl, const wchar_t* sAcceptLang, cef_jsdialog_type_t dialog_type, const wchar_t* sMessageText,const wchar_t* sDefaultPromptText,bool& pOutSuccess,std::string& pOutUserInput);
	virtual void OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate);
	virtual bool OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl);
	virtual bool OnDisableContextMenu(void) const {return m_pFuncInfo.m_bDisableContextMenu;}
	virtual bool OnCefClose(void);
	virtual bool OnBeforePopup(const wchar_t* sTargetUrl);
	virtual bool OnBeforeBrowse(const wchar_t* sUrl);
	virtual void OnAddressChange(const wchar_t* sUrl);
	virtual void OnTitleChange(const wchar_t* sTitle, const wchar_t* sUrl);
	virtual void OnFaviconURLChange(const wchar_t* sFaviconURL, const wchar_t* sUrl);
	virtual void OnStatusMessage(const wchar_t* sValue);
	virtual void OnLoadError(int nErrorCode,const wchar_t* sErrorText, const wchar_t* sFailedUrl);
	virtual void OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward);
	virtual bool OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event);
	virtual void OnFileReceiving(const CefDownloadInfo::pointer& pDownloadInfo);
	virtual bool OnFilePercent(const CefDownloadInfo::pointer& pDownloadInfo, double fPercent, int nCurrentSpeed, int nTranSeconds);
	virtual void OnFileCencel(const CefDownloadInfo::pointer& pDownloadInfo);
	virtual void OnFileReceived(const CefDownloadInfo::pointer& pDownloadInfo);
	virtual void OnFindClose(void);
	virtual void OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext);
#endif

	void OnBeforeNavigate(const wchar_t* szURL, bool* pOutCancel);
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//afx_msg void OnPaint();
	//afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	//afx_msg void OnMove(int x, int y);
};
