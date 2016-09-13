// cefbrowser.h file here
#ifndef __cefbrowser_h__
#define __cefbrowser_h__

#include "cef/client_handler.h"

#define CEF_DEFAULT_MAX_ZOOM_IN			7.0
#define CEF_DEFAULT_MAX_ZOOM_OUT		-4.5
#define CEF_DEFAULT_ZOOM_OFFSET			0.5

class CCefBrowser
{
public:
	bool Create(const std::string& sUrl, const RECT& rect, HWND pParentWnd, UINT nID)
	{
		//std::wstring username_key = L"username";
		//std::wstring username_value = L"xidada";
		//std::wstring domain = L"blog.csdn.net";
		//CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
		//CefCookie cookie;
		//CefString(&cookie.name).FromWString(username_key.c_str());
		//CefString(&cookie.value).FromWString(username_value.c_str());
		//CefString(&cookie.domain).FromWString(domain.c_str());
		//CefString(&cookie.path).FromASCII("/");
		//cookie.has_expires = false;

		//domain = L"https://" + domain;
		//CefPostTask(TID_IO, NewCefRunnableMethod(manager.get(), &CefCookieManager::SetCookie,CefString(domain.c_str()), cookie));

		if (g_handler.get()!=NULL)
		{
			if (g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetMainFrame()!=NULL)
				g_handler->GetBrowser()->GetMainFrame()->LoadURL(sUrl);
			return true;
		}
		HWND hWnd = pParentWnd;//->GetSafeHwnd();
		CefWindowInfo info;
		CefBrowserSettings settings;
		cef_string_utf8_to_utf16("UTF-8",5,&settings.default_encoding);
		//settings.javascript_open_windows = STATE_ENABLED;
		//settings.text_area_resize = STATE_ENABLED;
		info.SetAsChild(hWnd, rect);
		//info.ex_style 
		// ** 不能添加这二行代码（添加会导致本地不能保存缓存，某些页面不能正常使用） 2016-07-08
		//settings.local_storage = STATE_DISABLED;
		//settings.application_cache = STATE_DISABLED;
		settings.local_storage = STATE_ENABLED;
		settings.application_cache = STATE_ENABLED;
		//settings.web_security = STATE_DISABLED;
		//settings.plugins = STATE_ENABLED;

		g_handler = new ClientHandler(m_pHandler);
		g_handler->SetMainWindowHandle(hWnd);
		const bool ret = CefBrowserHost::CreateBrowser(info, g_handler.get(), sUrl, settings, NULL);
		if (!ret)
			g_handler = NULL;
		else
		{
			//g_handler->SetOwner(g_handler->GetBrowser());
			//g_handler->GetBrowser()->GetHost()->SetMouseCursorChangeDisabled(false);
			this->SetFocus();
		}
		return ret;
	}
	bool IsCreated(void) const {return g_handler.get()!=NULL?true:false;}
	void CloseAllBrowsers(bool force_close) { if (g_handler.get()!=NULL) { g_handler->CloseAllBrowsers(force_close);}}
	void Destroy(void) { if (g_handler.get()!=NULL) { g_handler->CloseAllBrowsers(true);g_handler->SetHandler(NULL); g_handler=NULL;}}
	CefWindowHandle GetSafeHwnd(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser() ? g_handler->GetBrowser()->GetHost()->GetWindowHandle():NULL;}
	void LoadURL(const std::string& sUrl) {if (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetMainFrame()!=NULL) g_handler->GetBrowser()->GetMainFrame()->LoadURL(sUrl);}
	void LoadString(const std::string& sString, const std::string& sUrl) {if (g_handler.get()!=NULL && g_handler->GetBrowser() && g_handler->GetBrowser()->GetMainFrame()) g_handler->GetBrowser()->GetMainFrame()->LoadString(sString,sUrl);}
	tstring GetLocationURL(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->GetMainFrame()->GetURL():"";}
	void ExecuteJavaScript(const std::string& code, const std::string& script_url, int start_line) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetMainFrame()->ExecuteJavaScript(code, script_url, start_line);}

	bool CanGoBack(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->CanGoBack():false;}
	void GoBack(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GoBack();}
	bool CanGoForward(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->CanGoForward():false;}
	void GoForward(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GoForward();}
	bool IsLoading(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->IsLoading():false;}
	void Reload(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->Reload();}
	void StopLoad(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->StopLoad();}
	bool IsPopup(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->IsPopup():false;}
	bool HasDocument(void) const {return g_handler.get()!=NULL && g_handler->GetBrowser()?g_handler->GetBrowser()->HasDocument():false;}
	void SetFocus(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->SetFocus(true);}
	void NotifyMoveOrResizeStarted(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->NotifyMoveOrResizeStarted();}
	void SendMouseMoveEvent(const CefMouseEvent& event, bool mouseLeave) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->SendMouseMoveEvent(event,mouseLeave);}
	void SendMouseWheelEvent(const CefMouseEvent& event, int deltaX, int deltaY) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->SendMouseWheelEvent(event,deltaX, deltaY);}
	void StopFinding(bool clearSelection) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->StopFinding(clearSelection);}
	void Find(int identifier, const CefString& searchText,bool forward, bool matchCase, bool findNext) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->Find(identifier,searchText,forward,matchCase,findNext);}
	void Print(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()) g_handler->GetBrowser()->GetHost()->Print();}
	void ZoomIn(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetHost()!=NULL) g_handler->GetBrowser()->GetHost()->SetZoomLevel(g_handler->GetBrowser()->GetHost()->GetZoomLevel()+0.5);}
	void ZoomOut(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetHost()!=NULL) g_handler->GetBrowser()->GetHost()->SetZoomLevel(g_handler->GetBrowser()->GetHost()->GetZoomLevel()-0.5);}
	void ZoomReset(void) {if (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetHost()!=NULL) g_handler->GetBrowser()->GetHost()->SetZoomLevel(0.0);}
	void SetZoomLevel(double fValue) {if (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetHost()!=NULL) g_handler->GetBrowser()->GetHost()->SetZoomLevel(fValue);}
	double GetZoomLevel(void) const {return (g_handler.get()!=NULL && g_handler->GetBrowser()!=NULL && g_handler->GetBrowser()->GetHost()!=NULL)?g_handler->GetBrowser()->GetHost()->GetZoomLevel():0.0;}

	bool CanExecuteJavaScript(const std::string& sFunctionName) const {return (g_handler.get()!=NULL)?g_handler->CanExecuteJavaScript(sFunctionName):false;}

	bool PostData(const std::string& sUrl, const std::string& sData)
	{
		if (g_handler.get()!=NULL && g_handler->GetBrowser())
		{
			// Create a new request
			CefRefPtr<CefRequest> request(CefRequest::Create());

			// Set the request URL
			request->SetURL(sUrl);

			// Add post data to the request.  The correct method and content-
			// type headers will be set by CEF.
			CefRefPtr<CefPostDataElement> postDataElement(CefPostDataElement::Create());
			postDataElement->SetToBytes(sData.length(), sData.c_str());
			CefRefPtr<CefPostData> postData(CefPostData::Create());
			postData->AddElement(postDataElement);
			request->SetPostData(postData);

			// Add a custom header
			CefRequest::HeaderMap headerMap;
			headerMap.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));	// ?
			request->SetHeaderMap(headerMap);

			// Load the request
			g_handler->GetBrowser()->GetMainFrame()->LoadRequest(request);
			return true;
		}
		return false;
	}

	BOOL Invalidate(BOOL bErase = TRUE)
	{
		CefWindowHandle hwnd = GetSafeHwnd();
		if (hwnd!=NULL)
		{
			RECT rect;
			::GetWindowRect(hwnd,&rect);
			return ::InvalidateRect(hwnd,&rect,bErase);
		}
		return FALSE;
	}
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE)
	{
		CefWindowHandle hwnd = GetSafeHwnd();
		if (hwnd!=NULL)
		{
			return ::InvalidateRect(hwnd,lpRect,bErase);
		}
		return FALSE;
	}
	BOOL ShowWindow(int nCmdShow)
	{
		CefWindowHandle hwnd = GetSafeHwnd();
		if (hwnd!=NULL)
		{
			return ::ShowWindow(hwnd, nCmdShow);
		}
		return FALSE;
	}
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE)
	{
		CefWindowHandle hwnd = GetSafeHwnd();
		if (hwnd!=NULL)
		{
			if (nWidth==0 || nHeight==0)
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			else
				::MoveWindow(hwnd,x,y,nWidth,nHeight,bRepaint);

			//CRect rect(x,y,x+nWidth,y+nHeight);
			//HDWP hdwp = BeginDeferWindowPos(1);
			////hdwp = DeferWindowPos(hdwp, editWnd, NULL, urloffset,
			////    0, rect.right - urloffset, URLBAR_HEIGHT, SWP_NOZORDER);
			//hdwp = DeferWindowPos(hdwp, hwnd, NULL,
			//	rect.left, rect.top, rect.right - rect.left,
			//	rect.bottom - rect.top, SWP_NOZORDER);
			//EndDeferWindowPos(hdwp);
		}
	}

	CCefBrowser(CCefBrowserHandler* pHandler)
		: m_pHandler(pHandler)
	{}
	~CCefBrowser(void)
	{
		if (g_handler.get()!=NULL)
			g_handler->CloseAllBrowsers(false);
	}
private:
	CefRefPtr<ClientHandler> g_handler;
	CCefBrowserHandler* m_pHandler;
};

#endif // __cefbrowser_h__
