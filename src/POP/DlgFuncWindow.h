#pragma once
#define USES_CEF_BROADCAST
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
#ifndef USES_CEF_BROADCAST
#include "BroadcastView.h"
#endif
#ifdef USES_LIBCEF
#include "cef/cefbrowser.h"
#endif
#include "PanelFind.h"


// CDlgFuncWindow dialog
/*
 * 应用功能窗口类
 * 用于显示广播消息窗口，和弹出应用集成页面等；
 * 
*//////////////////////////////////////////////////////

class CDlgFuncWindow : public CEbDialogBase
#ifndef USES_CEF_BROADCAST
	, public CBroadcastViewCallback
#endif
#ifdef USES_LIBCEF
	, public CCefBrowserHandler
	, public CFindHandler
#endif
{
	DECLARE_DYNAMIC(CDlgFuncWindow)
public:
	typedef boost::shared_ptr<CDlgFuncWindow> pointer;
	CDlgFuncWindow(CWnd* pParent = NULL,bool bDeleteThis=false);   // standard constructor
	virtual ~CDlgFuncWindow();

	CEBCCallInfo::pointer m_pEbCallInfo;
	void UpdateBroadcastMsg(const std::string& sTitle, const std::string& sText, eb::bigint nMsgId);
	void Navigate(const std::string& sUrl);

	//void SetChildMode(bool bChildMode = true) {m_bChildMode = bChildMode;}
	int m_nMsgType;
	bool m_bBroadcastMsg;	// default false
	bool m_bOpenNewClose;	// default false
	tstring m_sTitle;
	tstring m_sFuncUrl;
	tstring m_sPostData;
	size_t m_nWidth;
	size_t m_nHeight;
	bool m_bDisableContextMenu;
	bool m_bDisableScrollBar;
	bool m_bClosePrompt;
	bool m_bWinResizable;
	EB_FUNC_BROWSER_TYPE m_nFuncBrowserType;
	mycp::bigint GetShowMyMsgSubId(void) const {return m_nShowMyMsgSubId;}
	void SetShowMyMsgSubId(mycp::bigint nSubId,const std::string& sParameters);

// Dialog Data
	enum { IDD = IDD_DLG_FUNC_WINDOW };

protected:
	bool m_bDeleteThis;
	//bool m_bChildMode;
	//bool m_nShowFuncUrl;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CTraButton m_btnViewMyMsg;
	EB_BROWSER_TYPE m_nBrowserType;
#ifdef USES_LIBCEF
	CCefBrowser m_pCefBrowser;
	double m_fZoomLevel;
#endif
	CExplorer m_pExplorer;
	CWebUIController m_webUICtrl;
	bool m_bDocumentComplete;
#ifndef USES_CEF_BROADCAST
	CBroadcastView* m_pBroadcastView;
#endif
	CPanelFind* m_pPanelFind;
	CPanelText * m_pPanelStatus;
	std::wstring m_sStatusMessage;
	eb::bigint m_nBroadcastMsgId;

	mycp::bigint m_nShowMyMsgSubId;
	std::string m_sBCMsgParameters;

	void OnBeforeNavigate(const wchar_t* szURL, bool* pOutCancel);
	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL);
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel);
	//afx_msg void NewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel); 
	afx_msg void NewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl); 
	afx_msg void OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	//afx_msg void OnTitleChange(LPDISPATCH pDisp, VARIANT* sTitle);

	void ShowZoomLevel(void);
#ifdef USES_LIBCEF
	void MovePanelFind(int cx);
	virtual bool OnJSDialog(const wchar_t* sOriginUrl, const wchar_t* sAcceptLang, cef_jsdialog_type_t dialog_type, const wchar_t* sMessageText,const wchar_t* sDefaultPromptText,bool& pOutSuccess,std::string& pOutUserInput);
	virtual void OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate);
	virtual bool OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl);
	virtual bool OnDisableContextMenu(void) const {return m_bDisableContextMenu;}
	virtual bool OnBeforePopup(const wchar_t* sTargetUrl);
	virtual bool OnBeforeBrowse(const wchar_t* sUrl);
	virtual void OnAddressChange(const wchar_t* sUrl);
	virtual void OnTitleChange(const wchar_t* sTitle, const wchar_t* sUrl);
	virtual void OnStatusMessage(const wchar_t* sValue);
	virtual void OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward);
	virtual bool OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event);
	virtual void OnFileReceiving(const CefDownloadInfo::pointer& pDownloadInfo);
	virtual bool OnFilePercent(const CefDownloadInfo::pointer& pDownloadInfo, double fPercent, int nCurrentSpeed, int nTranSeconds);
	virtual void OnFileCencel(const CefDownloadInfo::pointer& pDownloadInfo);
	virtual void OnFileReceived(const CefDownloadInfo::pointer& pDownloadInfo);

	virtual void OnFindClose(void);
	virtual void OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext);

#endif

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	void DoClose(bool bTimerToClose);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButtonViewMymsg();
};
