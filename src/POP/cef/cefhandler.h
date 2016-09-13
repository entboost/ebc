// cefhandler.h file here
#ifndef __cefhandler_h__
#define __cefhandler_h__
#pragma once

#define  CEF_CLIENT_MENU_COMMAND_ID_FIND			MENU_ID_USER_FIRST+100
#define  CEF_CLIENT_MENU_COMMAND_ID_IMAGE_SAVEAS	MENU_ID_USER_FIRST+101
#define  CEF_CLIENT_MENU_COMMAND_ID_COPY_IMAGE		MENU_ID_USER_FIRST+102

class CefDownloadInfo
{
public:
	typedef boost::shared_ptr<CefDownloadInfo> pointer;
	static CefDownloadInfo::pointer create(int64 nFileId, int64 nFileSize)
	{
		return CefDownloadInfo::pointer(new CefDownloadInfo(nFileId, nFileSize));
	}

	int64 m_nFileId;
	int64 m_nFileSize;
	int64 m_nReceivedBytes;
	time_t m_tCreateTime;
	time_t m_tLastTime;
	std::wstring m_sFileName;
	bool m_bCancel;
	CefDownloadInfo(int64 nFileId, int64 nFileSize)
		: m_nFileId(nFileId), m_nFileSize(nFileSize)
		, m_tLastTime(0)
		, m_bCancel(false)
	{
		m_tCreateTime = time(0);
	}
	CefDownloadInfo(void)
		: m_nFileId(0), m_nFileSize(0)
		, m_tLastTime(0)
		, m_bCancel(false)
	{
		m_tCreateTime = time(0);
	}
};

class CCefBrowserHandler
{
public:
	virtual bool OnDisableContextMenu(void) const {return false;}
	virtual bool OnDisableBackForward(void) const {return false;}

	virtual bool OnCefClose(void) {return false;}
	virtual bool OnBeforePopup(const wchar_t* sTargetUrl) {return false;}	// return true cancel Popup
	virtual bool OnBeforeBrowse(const wchar_t* sUrl) {return false;}		// return true cancel Browse
	virtual void OnAddressChange(const wchar_t* sUrl) {}
	virtual void OnTitleChange(const wchar_t* sTitle, const wchar_t* sUrl) {}
	virtual void OnFaviconURLChange(const wchar_t* sFaviconURL, const wchar_t* sUrl) {}
	virtual void OnStatusMessage(const wchar_t* sValue) {}
	virtual void OnLoadError(int nErrorCode,const wchar_t* sErrorText, const wchar_t* sFailedUrl) {}
	virtual void OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward) {}
	virtual bool OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event) {return false;}
	virtual void OnFileReceiving(const CefDownloadInfo::pointer& pDownloadInfo) {}
	virtual bool OnFilePercent(const CefDownloadInfo::pointer& pDownloadInfo, double fPercent, int nCurrentSpeed, int nTranSeconds) {return false;}	// *return true cancel download
	virtual void OnFileCencel(const CefDownloadInfo::pointer& pDownloadInfo) {}
	virtual void OnFileReceived(const CefDownloadInfo::pointer& pDownloadInfo) {}

	// download
	//virtual void OnBeforeDownload(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward) {}

	virtual void OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate) {}
	virtual bool OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl) {return true;}
	virtual bool OnJSDialog(const wchar_t* sOriginUrl, const wchar_t* sAcceptLang, cef_jsdialog_type_t dialog_type, const wchar_t* sMessageText,const wchar_t* sDefaultPromptText,bool& pOutSuccess,std::string& pOutUserInput) {return false;}
};

#endif  // __cefhandler_h__
