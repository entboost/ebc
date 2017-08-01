// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifdef WIN32
#pragma warning(disable:4819)
#endif

#include <stdafx.h>
#include "client_handler.h"

#include <string>
#include <windows.h>
#include <shlobj.h> 

#include "include/cef_browser.h"
#include "include/cef_frame.h"
//#include "cefclient/resource.h"

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefString& url) {
  CEF_REQUIRE_UI_THREAD();
  if (GetBrowserId() == browser->GetIdentifier()) {
	  if (m_pHandler)
		  m_pHandler->OnAddressChange(url.c_str());
  }
  //if (GetBrowserId() == browser->GetIdentifier() && frame->IsMain()) {
  //  // Set the edit window text
  //  SetWindowText(edit_handle_, std::string(url).c_str());
  //}
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
  CEF_REQUIRE_UI_THREAD();

  //const bool bIsValid = browser->GetMainFrame()->IsValid();
  // Set the frame window title bar
  if (browser->GetHost()==NULL) return;
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  if (GetBrowserId() == browser->GetIdentifier()) {
	  // The frame window will be the parent of the browser window
	  hwnd = GetParent(hwnd);
	  if (m_pHandler!=NULL && browser->GetMainFrame()!=NULL)
		  m_pHandler->OnTitleChange(title.c_str(),browser->GetMainFrame()->GetURL().c_str());
  }
  SetWindowTextW(hwnd, title.c_str());
}

void ClientHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                  const std::vector<CefString>& icon_urls)
{
  CEF_REQUIRE_UI_THREAD();
  if (GetBrowserId() == browser->GetIdentifier())
  {
	  if (m_pHandler!=NULL && browser->GetMainFrame()!=NULL)
	  {
		  std::wstring sFaviconURL;
		  for (size_t i=0; i<icon_urls.size(); i++)
		  {
			  sFaviconURL = icon_urls[i];
			  if ((i+1)==icon_urls.size()) break;
			  const size_t nSize = sFaviconURL.size();
			  if (nSize>4)
			  {
				  std::wstring sExt(sFaviconURL.substr(nSize-3));
				  std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
				  if (sExt==L"ico") break;
			  }
		  }
		  m_pHandler->OnFaviconURLChange(sFaviconURL.c_str(),browser->GetMainFrame()->GetURL().c_str());
	  }
  }
}

bool ClientHandler::OnTooltip(CefRefPtr<CefBrowser> browser,
                         CefString& text)
{
  CEF_REQUIRE_UI_THREAD();
	return false;
}

void ClientHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser,
                               const CefString& value)
{
  CEF_REQUIRE_UI_THREAD();

  if (m_pHandler!=NULL)
  {
	  //if (value.empty())
		 // m_sInDrawEnter.clear();

		//if (value.c_str()!=NULL)
		//{
		//	USES_CONVERSION;
		//	// ** 中文做 URL DECODE
		//	tstring sValutTemp(libEbc::URLDecode(W2A_UTF8(value.c_str()),false));
		//	// ** 去掉 http:// 或 https://
		//	//if (sValutTemp.size()>7 && sValutTemp.substr(0,7)=="http://")
		//	//	sValutTemp = sValutTemp.substr(7);
		//	//else if (sValutTemp.size()>8 && sValutTemp.substr(0,8)=="https://")
		//	//	sValutTemp = sValutTemp.substr(8);
		//	m_pHandler->OnStatusMessage(A2W_UTF8(sValutTemp.c_str()));
		//}else
		//{
		//	m_pHandler->OnStatusMessage(NULL);
		//}
	  m_pHandler->OnStatusMessage(value.c_str());
  }
}

void ClientHandler::SendNotification(NotificationType type) {
  //UINT id;
  //switch (type) {
  //case NOTIFY_CONSOLE_MESSAGE:
  //  id = ID_WARN_CONSOLEMESSAGE;
  //  break;
  //case NOTIFY_DOWNLOAD_COMPLETE:
  //  id = ID_WARN_DOWNLOADCOMPLETE;
  //  break;
  //case NOTIFY_DOWNLOAD_ERROR:
  //  id = ID_WARN_DOWNLOADERROR;
  //  break;
  //default:
  //  return;
  //}
  //PostMessage(main_handle_, WM_COMMAND, id, 0);
}

void ClientHandler::SetLoading(bool isLoading) {
  //DCHECK(edit_handle_ != NULL && reload_handle_ != NULL &&
  //       stop_handle_ != NULL);
  //EnableWindow(edit_handle_, TRUE);
  //EnableWindow(reload_handle_, !isLoading);
  //EnableWindow(stop_handle_, isLoading);
}

void ClientHandler::SetNavState(bool canGoBack, bool canGoForward) {
  //DCHECK(back_handle_ != NULL && forward_handle_ != NULL);
  //EnableWindow(back_handle_, canGoBack);
  //EnableWindow(forward_handle_, canGoForward);
}

std::string ClientHandler::GetDownloadPath(const std::string& file_name) {
  TCHAR szFolderPath[MAX_PATH];
  std::string path;

  // Save the file in the user's "My Documents" folder.
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
                                NULL, 0, szFolderPath))) {
    path = CefString(szFolderPath);
    path += "\\" + file_name;
  }

  return path;
}
