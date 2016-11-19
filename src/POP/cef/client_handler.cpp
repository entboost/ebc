// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "StdAfx.h"
#include "client_handler.h"
#include <stdio.h>
#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_path_util.h"
#include "include/cef_process_util.h"
#include "include/cef_trace.h"
#if (CEF_VERSION==2526)
#include "include/cef_parser.h"
#else
#include "include/cef_url.h"
#endif
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
//#include "include/cef_nplugin.h"



namespace {

// Custom menu command Ids.
enum client_menu_ids {
  CLIENT_ID_SHOW_DEVTOOLS   = MENU_ID_USER_FIRST,
  CLIENT_ID_CLOSE_DEVTOOLS,
  CLIENT_ID_INSPECT_ELEMENT,
  CLIENT_ID_OPEN_LINK_URL0,	// load_url
  CLIENT_ID_OPEN_LINK_URL1,
  CLIENT_ID_OPEN_LINK_URL2,
  CLIENT_ID_SAVEAS_LINK_URL,
  CLIENT_ID_COPY_LINK_URL,
  CLIENT_ID_COPY_IMAGE_URL,
  CLIENT_ID_OPEN_IMAGE_URL,
  //CEF_CLIENT_MENU_COMMAND_ID_FIND,
  //CLIENT_ID_RELOAD,
  //CLIENT_ID_TESTMENU_SUBMENU,
  //CLIENT_ID_TESTMENU_CHECKITEM,
  //CLIENT_ID_TESTMENU_RADIOITEM1,
  //CLIENT_ID_TESTMENU_RADIOITEM2,
  //CLIENT_ID_TESTMENU_RADIOITEM3,
};

const char kTestOrigin[] = "http://tests/";

// Retrieve the file name and mime type based on the specified url.
//bool ParseTestUrl(const std::string& url,
//                  std::string* file_name,
//                  std::string* mime_type) {
//  // Retrieve the path component.
//  CefURLParts parts;
//  CefParseURL(url, parts);
//  std::string file = CefString(&parts.path);
//  if (file.size() < 2)
//    return false;
//
//  // Remove the leading slash.
//  file = file.substr(1);
//
//  // Verify that the file name is valid.
//  for(size_t i = 0; i < file.size(); ++i) {
//    const char c = file[i];
//    if (!isalpha(c) && !isdigit(c) && c != '_' && c != '.')
//      return false;
//  }
//
//  // Determine the mime type based on the file extension, if any.
//  size_t pos = file.rfind(".");
//  if (pos != std::string::npos) {
//    tstring ext = file.substr(pos + 1);
//    if (ext == "html")
//      *mime_type = "text/html";
//    else if (ext == "png")
//      *mime_type = "image/png";
//    else
//      return false;
//  } else {
//    // Default to an html extension if none is specified.
//    *mime_type = "text/html";
//    file += ".html";
//  }
//
//  *file_name = file;
//  return true;
//}

}  // namespace

int ClientHandler::browser_count_ = 0;

ClientHandler::ClientHandler(CCefBrowserHandler* pHandler)
  : browser_id_(0),
    is_closing_(false),
    main_handle_(NULL),
    edit_handle_(NULL),
    back_handle_(NULL),
    forward_handle_(NULL),
    stop_handle_(NULL),
    reload_handle_(NULL),
    focus_on_editable_field_(false)
	, m_pHandler(pHandler)
{
#if defined(OS_LINUX)
  gtk_dialog_ = NULL;
#endif
  m_bCloseAll = false;
  mouse_cursor_change_disabled_ = false;

  // Read command line settings.
  //CefRefPtr<CefCommandLine> command_line =
  //    CefCommandLine::GetGlobalCommandLine();

  //if (command_line->HasSwitch(cefclient::kUrl))
  //  startup_url_ = command_line->GetSwitchValue(cefclient::kUrl);
  //if (startup_url_.empty())
  //  startup_url_ = "http://www.google.com/";

  //mouse_cursor_change_disabled_ =
  //    command_line->HasSwitch(cefclient::kMouseCursorChangeDisabled);
  m_bSendMouseLeaveEvent = true;
}

ClientHandler::~ClientHandler() {
}

bool ClientHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();
#ifdef USES_MESSAGE_ROUTER
	if (message_router_->OnProcessMessageReceived(browser, source_process,
		message)) {
			return true;
	}
#endif

	const std::string& messageName = message->GetName();
	if (messageName == "on_first_keydown") 
	{   
#if (CEF_VERSION<2454)
//#if (CEF_VERSION==2272)
		// extract message
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (args->GetSize()==4)
		{
			const int32 x = args->GetInt(0);
			const int32 y = args->GetInt(1);
			const int32 w = args->GetInt(2);
			const int32 h = args->GetInt(3);

			CefMouseEvent pEvent;
			pEvent.x = x+w/2;
			pEvent.y = y+h/2;

			const HWND hWnd = browser->GetHost()->GetWindowHandle();
			POINT ptNode;
			ptNode.x = x;
			ptNode.y = y;
			::ClientToScreen(hWnd,&ptNode);
			CRect rectNode(ptNode.x,ptNode.y,ptNode.x+w,ptNode.y+h);

			POINT ptCursor;
			::GetCursorPos(&ptCursor);
			if (rectNode.PtInRect(ptCursor))
			{
				browser->GetHost()->SendMouseMoveEvent(pEvent,false);
				browser->GetHost()->SendMouseMoveEvent(pEvent,true);
			}else
			{
				browser->GetHost()->SendMouseMoveEvent(pEvent,true);
				browser->GetHost()->SendMouseMoveEvent(pEvent,false);
			}
		}
#endif
		return true;
	}else if (messageName == "can_execute_javascript")
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (args->GetSize()==2)
		{
			const bool nCanExecuteJavaScript = args->GetBool(0);
			const std::string sFunctionName = args->GetString(1);
			if (nCanExecuteJavaScript)
				m_pCanExecuteJavaScriptList.insert(sFunctionName,true,false);
			else
				m_pCanExecuteJavaScriptList.remove(sFunctionName);
		}
		return true;
	//}else if (messageName.size()>4 && messageName.substr(0,4)=="can_")// if (messageName == "can_entboost_on_user_off_line")
	//{
	//	CefRefPtr<CefListValue> args = message->GetArgumentList();
	//	if (args->GetSize()==1)
	//	{
	//		const bool nCanExecuteJavaScript = args->GetBool(0);
	//		const std::string sFunctionName = messageName.substr(4);
	//		if (nCanExecuteJavaScript)
	//			m_pCanExecuteJavaScriptList.insert(sFunctionName,true,false);
	//		else
	//			m_pCanExecuteJavaScriptList.remove(sFunctionName);
	//	}
	//	return true;
	}
	return false;

	//// Check for messages from the client renderer.
	//tstring message_name = message->GetName();
	//if (message_name == client_renderer::kFocusedNodeChangedMessage) {
	//  // A message is sent from ClientRenderDelegate to tell us whether the
	//  // currently focused DOM node is editable. Use of |focus_on_editable_field_|
	//  // is redundant with CefKeyEvent.focus_on_editable_field in OnPreKeyEvent
	//  // but is useful for demonstration purposes.
	//  focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
	//  return true;
	//}
	return false;
}

void ClientHandler::OnFindResult(CefRefPtr<CefBrowser> browser,
                            int identifier,
                            int count,
                            const CefRect& selectionRect,
                            int activeMatchOrdinal,
                            bool finalUpdate)
{
  CEF_REQUIRE_UI_THREAD();
  if (m_pHandler!=NULL)
  {
	  m_pHandler->OnFindResult(identifier,count,selectionRect,activeMatchOrdinal,finalUpdate);
  }
}
#define USES_INSERT_LINK_FIRST_POS

inline bool CefCheckUrl(const std::wstring& sUrl)
{
	const size_t nSize = sUrl.size();
	std::wstring sUrlPrefix = nSize>=8?sUrl.substr(0,8):L"";
	std::transform(sUrlPrefix.begin(), sUrlPrefix.end(), sUrlPrefix.begin(), tolower);
	if (sUrlPrefix.find(L"https://")==0 || sUrlPrefix.find(L"http://")==0 || sUrlPrefix.find(L"www.")==0)
	{
		for (size_t i=0;i<nSize;i++)
		{
			const wchar_t pChar = sUrl.at(i);
			if (pChar>=128 || pChar=='\t' || pChar=='\r' || pChar=='\n' || pChar==';' || pChar==',' || pChar==' ')
			//if (pChar>255 || pChar=='\t' || pChar=='\r' || pChar=='\n' || pChar==';' || pChar==',' || pChar==160 || pChar==' ')	// 160=' '
			{
				return false;
			}				
		}
		return true;
	}
	return false;
}

#include "StdAfx.h"
#include "../include/ebc_public.h"
void ClientHandler::OnBeforeContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model) {
  CEF_REQUIRE_UI_THREAD();

  if (model->GetCount()>0 && m_pHandler!=NULL)
  {
	  if (m_pHandler->OnDisableContextMenu())
	  {
		  model->Clear();
		  return;
	  }
  }

  //CefResourceBundleHandler::GetLocalizedString()
  if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
	  //model->SetLabel(MENU_ID_BACK,libEbc::ACP2UTF8("返回(&B)"));
	  //model->SetLabel(MENU_ID_FORWARD,libEbc::ACP2UTF8("前进(&F)"));
	  //model->SetLabel(MENU_ID_RELOAD,libEbc::ACP2UTF8("重新加载(&L)"));
	  ////model->SetLabel(MENU_ID_RELOAD_NOCACHE,libEbc::ACP2UTF8("重新加载(&L)"));
	  //model->SetLabel(MENU_ID_STOPLOAD,libEbc::ACP2UTF8("停止加载(&S)"));

	  //model->SetLabel(MENU_ID_UNDO,libEbc::ACP2UTF8("撤销(&U)"));
	  //model->SetLabel(MENU_ID_REDO,libEbc::ACP2UTF8("重做(&R)"));
	  //model->SetLabel(MENU_ID_CUT,libEbc::ACP2UTF8("剪切(&T)"));
	  //model->SetLabel(MENU_ID_COPY,libEbc::ACP2UTF8("复制(&C)"));
	  //model->SetLabel(MENU_ID_PASTE,libEbc::ACP2UTF8("粘贴(&P)"));
	  //model->SetLabel(MENU_ID_DELETE,libEbc::ACP2UTF8("删除(&D)"));
	  //model->SetLabel(MENU_ID_SELECT_ALL,libEbc::ACP2UTF8("全选(&A)"));

	  ////model->SetLabel(MENU_ID_FIND,libEbc::ACP2UTF8("查找(&F)"));
	  //model->SetLabel(MENU_ID_PRINT,libEbc::ACP2UTF8("打印(&R)..."));
	  //model->SetLabel(MENU_ID_VIEW_SOURCE,libEbc::ACP2UTF8("查看网页源代码(&V)"));

		if (m_pHandler!=NULL && m_pHandler->OnDisableBackForward())
		{
			//model->SetEnabled(MENU_ID_BACK,false);
			//model->SetEnabled(MENU_ID_FORWARD,false);
			const int nForwardIndex = model->GetIndexOf(MENU_ID_FORWARD);
			if (nForwardIndex>0)
			{
				if (model->GetTypeAt(nForwardIndex+1)==MENUITEMTYPE_SEPARATOR)
				{
					model->RemoveAt(nForwardIndex+1);
				}
			}
			model->Remove(MENU_ID_BACK);
			model->Remove(MENU_ID_FORWARD);
		}

#ifdef USES_INSERT_LINK_FIRST_POS
	  const bool bHasMenuItem = model->GetCount()>0?true:false;
#endif

	  //if (model->GetIndexOf(MENU_ID_RELOAD)<0)
	  //{
		 // const int nInsertIndexAt = model->GetIndexOf(MENU_ID_FORWARD);
		 // if (nInsertIndexAt>=0)
			//  model->InsertItemAt(nInsertIndexAt+1,MENU_ID_RELOAD,libEbc::ACP2UTF8("刷新(&F)").string());
	  //}
	  if (model->GetIndexOf(MENU_ID_RELOAD_NOCACHE)<0)
	  {
		  const int nInsertIndexAt = model->GetIndexOf(MENU_ID_FORWARD);
		  if (nInsertIndexAt>=0)
			  model->InsertItemAt(nInsertIndexAt+1,MENU_ID_RELOAD_NOCACHE,libEbc::ACP2UTF8("重新加载(&L)").string());
	  }
#ifdef USES_INSERT_LINK_FIRST_POS
	  int nInsertIndexAt = 0;
#else
	  int nInsertIndexAt = model->GetIndexOf(MENU_ID_PRINT)-1;
#endif
	  const CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
	  if (!sUnfilteredLinkUrl.empty())
	  {
#ifndef USES_INSERT_LINK_FIRST_POS
		  if (nInsertIndexAt<0)
			  nInsertIndexAt = 0;
		  else
			  model->InsertSeparatorAt(nInsertIndexAt++);
#endif
		  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_LINK_URL1,libEbc::ACP2UTF8("在新标签页中打开链接(&T)").string());
		  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_LINK_URL2,libEbc::ACP2UTF8("在新浏览器中打开链接(&W)").string());
		  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_SAVEAS_LINK_URL,libEbc::ACP2UTF8("链接另存为(&K)...").string());
		  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_COPY_LINK_URL,libEbc::ACP2UTF8("复制链接地址(&E)").string());
	  }else
	  {
		  const std::wstring sSelectionText = params->GetSelectionText();
		  if (CefCheckUrl(sSelectionText))
		  {
#ifndef USES_INSERT_LINK_FIRST_POS
			  if (nInsertIndexAt<0)
				  nInsertIndexAt = 0;
			  else
				  model->InsertSeparatorAt(nInsertIndexAt++);
#endif
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_LINK_URL0,libEbc::ACP2UTF8("打开链接(&O)").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_LINK_URL1,libEbc::ACP2UTF8("在新标签页中打开链接(&T)").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_LINK_URL2,libEbc::ACP2UTF8("在新浏览器中打开链接(&W)").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_SAVEAS_LINK_URL,libEbc::ACP2UTF8("链接另存为(&K)...").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_COPY_LINK_URL,libEbc::ACP2UTF8("复制链接地址(&E)").string());
		  }
	  }
	  const CefString sSourceUrl = params->GetSourceUrl();	// 图片链接地址
	  if (params->GetMediaType()==CM_MEDIATYPE_IMAGE)
	  {
		  const CefString sSourceUrl = params->GetSourceUrl();	// 图片链接地址
		  if (!sSourceUrl.empty())
		  //const CefString sImageUrl = sSourceUrl.empty()?params->GetLinkUrl():sSourceUrl;
		  //if (!sImageUrl.empty())
		  {
#ifdef USES_INSERT_LINK_FIRST_POS
			  if (nInsertIndexAt>0)
				  model->InsertSeparatorAt(nInsertIndexAt++);
#else
			  if (nInsertIndexAt<0)
				  nInsertIndexAt = 0;
			  else
				  model->InsertSeparatorAt(nInsertIndexAt++);
#endif
			  model->InsertItemAt(nInsertIndexAt++,CEF_CLIENT_MENU_COMMAND_ID_IMAGE_SAVEAS,libEbc::ACP2UTF8("图片另存为(&V)...").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_COPY_IMAGE_URL,libEbc::ACP2UTF8("复制图片网址(&O)").string());
			  model->InsertItemAt(nInsertIndexAt++,CEF_CLIENT_MENU_COMMAND_ID_COPY_IMAGE,libEbc::ACP2UTF8("复制图片(&Y)").string());
			  model->InsertItemAt(nInsertIndexAt++,CLIENT_ID_OPEN_IMAGE_URL,libEbc::ACP2UTF8("在新标签页中打开图片(&I)").string());
		  }
	  }

#ifdef USES_INSERT_LINK_FIRST_POS
	  if (bHasMenuItem && nInsertIndexAt>0)
	  {
		  model->InsertSeparatorAt(nInsertIndexAt++);
	  }
#endif
	  if (model->GetIndexOf(MENU_ID_FIND)<0 && params->GetSelectionText().empty())
	  {
#ifdef USES_INSERT_LINK_FIRST_POS
		  //const int nPrintIndex = model->GetIndexOf(MENU_ID_PRINT);
		  //if (nPrintIndex>=0)
			 // nInsertIndexAt = nPrintIndex-1;
		  model->InsertItemAt(nInsertIndexAt++,CEF_CLIENT_MENU_COMMAND_ID_FIND,libEbc::ACP2UTF8("查找(Ctrl+F)").string());
		  if (bHasMenuItem)
			  model->InsertSeparatorAt(nInsertIndexAt++);
#else
		  if (nInsertIndexAt<0)
			  nInsertIndexAt = model->GetIndexOf(MENU_ID_PRINT)-1;
		  model->InsertSeparatorAt(nInsertIndexAt++);
		  model->InsertItemAt(nInsertIndexAt++,CEF_CLIENT_MENU_COMMAND_ID_FIND,libEbc::ACP2UTF8("查找(Ctrl+F)").string());
#endif
	  }

    // Add a separator if the menu already has items.
    if (model->GetCount() > 0)
	{
      model->AddSeparator();
	}
	//if (params->HasImageContents())
	//	model->AddItem(CLIENT_ID_COPY_LINK_URL,libEbc::ACP2UTF8("复制图片网址(&E)").string());
	//const CefString sLinkUrl = params->GetLinkUrl();
	//const CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
	//const CefString sSelectText = params->GetSelectionText();
	//if (!sLinkUrl.empty())
	//{
	//}

    // Add DevTools items to all context menus.
	model->AddItem(CLIENT_ID_SHOW_DEVTOOLS,libEbc::ACP2UTF8("显示开发工具(&D)").string());
	//model->AddItem(CLIENT_ID_CLOSE_DEVTOOLS,libEbc::ACP2UTF8("关闭开发工具"));
    model->AddSeparator();
	model->AddItem(CLIENT_ID_INSPECT_ELEMENT,libEbc::ACP2UTF8("审查元素(&N)").string());

  //  // Test context menu features.
  //  BuildTestMenu(model);
  }
}

bool CopyToClipboard(HWND hWnd, const CefString& sString)
{
	if (sString.empty()) return false;
	USES_CONVERSION;
	const tstring sSelectString(W2A(sString.c_str()));
	if( OpenClipboard(hWnd) )
	{
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, sSelectString.size()+1);
		char * buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, sSelectString.c_str());
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
	return true;
}

bool ClientHandler::OnContextMenuCommand(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int command_id,
    EventFlags event_flags) {
  CEF_REQUIRE_UI_THREAD();

  switch (command_id) {
	  case MENU_ID_VIEW_SOURCE:
		  {
			  // view-source:about:blank
			  if (browser->GetMainFrame()==NULL) return false;
			  const CefString sUrl = browser->GetMainFrame()->GetURL();
			  if (sUrl.empty()) return false;
			  std::wstring sViewSourceString(L"view-source:");
			  sViewSourceString.append(sUrl.c_str());
			  return m_pHandler->OnBeforePopup(sViewSourceString.c_str());
		  }break;
	  case CLIENT_ID_OPEN_LINK_URL0:
		  {
			  CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
			  if (sUnfilteredLinkUrl.empty())
				  sUnfilteredLinkUrl = params->GetSelectionText();
			  if (!sUnfilteredLinkUrl.empty())
			  {
				  if (browser->GetMainFrame()!=NULL)
				  {
					  browser->GetMainFrame()->LoadURL(sUnfilteredLinkUrl);
				  }else if (m_pHandler!=NULL)
				  {
					  if (!m_pHandler->OnBeforeBrowse(sUnfilteredLinkUrl.c_str()))
						  return m_pHandler->OnBeforePopup(sUnfilteredLinkUrl.c_str());
				  }
				  return true;
			  }
		  }break;
	  case CLIENT_ID_OPEN_LINK_URL1:
		  {
			  if (m_pHandler!=NULL)
			  {
				  CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
				  if (sUnfilteredLinkUrl.empty())
					  sUnfilteredLinkUrl = params->GetSelectionText();
				  if (!sUnfilteredLinkUrl.empty())
				  {
					  if (!m_pHandler->OnBeforeBrowse(sUnfilteredLinkUrl.c_str()))
						  return m_pHandler->OnBeforePopup(sUnfilteredLinkUrl.c_str());
					  else
						  return true;
				  }
			  }
			  return false;
		  }break;
	  case CLIENT_ID_OPEN_LINK_URL2:
		  {
			  CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
			  if (sUnfilteredLinkUrl.empty())
				  sUnfilteredLinkUrl = params->GetSelectionText();
			  if (!sUnfilteredLinkUrl.empty())
			  {
				  if (m_pHandler==NULL)
				  {
					  ShellExecuteW(NULL,  L"open", sUnfilteredLinkUrl.c_str(), NULL, NULL, SW_SHOW);
				  }else
				  {
					  if (!m_pHandler->OnBeforeBrowse(sUnfilteredLinkUrl.c_str()))
						  ShellExecuteW(NULL,  L"open", sUnfilteredLinkUrl.c_str(), NULL, NULL, SW_SHOW);
				  }
				  return true;
			  }
			  return false;
		  }break;
	  case CLIENT_ID_SAVEAS_LINK_URL:
		  {
			  if (browser->GetMainFrame()==NULL) return false;
			  CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
			  if (sUnfilteredLinkUrl.empty())
				  sUnfilteredLinkUrl = params->GetSelectionText();
			  if (!sUnfilteredLinkUrl.empty())
			  {
				  //// 
				  ////std::wstring uriContent = L"data:application/octet-stream,download=abc.html,";
				  ////std::wstring uriContent = L"data:application/octet-stream,attachment;filename=abc.html,";
				  //std::wstring uriContent = L"data:application/octet-stream,";
				  //uriContent.append(sUnfilteredLinkUrl.c_str());
				  //std::wstring sJSCommand = L"window.open(\"";
				  //sJSCommand.append(uriContent.c_str());
				  //sJSCommand.append(L"\", '');");
				  //browser->GetMainFrame()->ExecuteJavaScript(sJSCommand,"",0);
				  //return true;

				  //sJSCommand = L"document.execCommand('SaveAs','1',\"";
				  //sJSCommand.append(uriContent.c_str());
				  //sJSCommand.append(L"\");");

				  ////std::wstring sJSCommand = L"alert('You pressed the space bar!');";
				  //std::wstring sJSCommand = L"document.execCommand('SaveAs');";
				  //browser->GetMainFrame()->ExecuteJavaScript(sJSCommand,"",0);
				  //return true;

				  //myTempWindow = window.open(filepath,'','left=10000,screenX=10000');
				  //myTempWindow.document.execCommand('SaveAs','null',name);
				  //myTempWindow.close();
				  //sJSCommand = L"var myTempWindow = window.open(\"";
				  //sJSCommand.append(uriContent.c_str());
				  //sJSCommand.append(L"\", '');");
				  //sJSCommand.append(L"myTempWindow.document.execCommand('SaveAs','null','abc.html');");
				  //sJSCommand.append(L"myTempWindow.close();");
				  //browser->GetMainFrame()->ExecuteJavaScript(sJSCommand,"",0);

				  //<html>
				  //  <head>
				  //    <meta charset="UTF-8">
				  //    <script type="text/javascript">
				  //      function linkDownload(a, filename, content) {
				  ////        contentType =  'data:application/octet-stream,';
				  ////        uriContent = contentType + encodeURIComponent(content);
				  ////        a.setAttribute('href', uriContent);
				  //        a.setAttribute('href', content);
				  //        a.setAttribute('download', filename);
				  //      }
				  //      function download(filename, content) {
				  //        var a = document.createElement('a');
				  //        linkDownload(a, filename, content);
				  //        document.body.appendChild(a);
				  //        a.click();
				  //        document.body.removeChild(a);
				  //      }
				  //    </script>
				  //   </head>
				  //  <body>
				  //    <a href="#" onclick="linkDownload(this, 'test.txt', 'Hello World!');">download</a>
				  //    <button onclick="download('test.txt', 'Hello World!');">download</button>
				  //  </body>
				  //</html>

				  const std::wstring sUrlTemp = sUnfilteredLinkUrl;
				  std::string::size_type find = sUrlTemp.rfind(L"/");
				  if (find==std::string::npos) return false;
				  const std::wstring sFileName(sUrlTemp.substr(find+1));

				  std::wstring sJSCommand = L"var a = document.createElement('a');";
				  sJSCommand.append(L"a.setAttribute('href', '");					// a.setAttribute('href', uriContent);
				  sJSCommand.append(sUnfilteredLinkUrl.c_str());
				  sJSCommand.append(L"');");
				  sJSCommand.append(L"a.setAttribute('download', '");				// a.setAttribute('download', filename);
				  sJSCommand.append(sFileName);
				  sJSCommand.append(L"');");
				  sJSCommand.append(L"document.body.appendChild(a);");
				  sJSCommand.append(L"a.click();");
				  sJSCommand.append(L"document.body.removeChild(a);");
				  browser->GetMainFrame()->ExecuteJavaScript(sJSCommand,"",0);
				  return true;
			  }
		  }break;
	  case CLIENT_ID_COPY_LINK_URL:
		  {
			  if (browser->GetHost()==NULL) return false;
			  CefString sUnfilteredLinkUrl = params->GetUnfilteredLinkUrl();
			  if (sUnfilteredLinkUrl.empty())
				  sUnfilteredLinkUrl = params->GetSelectionText();
			  return CopyToClipboard(browser->GetHost()->GetWindowHandle(),sUnfilteredLinkUrl);
		  }break;
	  case CLIENT_ID_COPY_IMAGE_URL:
		  {
			  if (params->GetMediaType()==CM_MEDIATYPE_IMAGE)
			  {
				  //"javascript:document.execCommand('SaveAs','true','your_file.txt')", params->GetSourceUrl(), 0);
				  //browser->GetMainFrame()->ExecuteJavaScript(
					 // "document.execCommand('SaveAs','true','your_file.txt')", params->GetSourceUrl(), 0);

				  //// ** 可以下载
				  ////uriContent = "data:application/octet-stream," + encodeURIComponent(content);
				  ////newWindow = window.open(uriContent, 'neuesDokument');

				  //std::wstring uriContent = L"data:application/octet-stream,";
				  //uriContent.append(params->GetSourceUrl().c_str());
				  //std::wstring sJSCommand = L"window.open(\"";
				  //sJSCommand.append(uriContent.c_str());
				  //sJSCommand.append(L"\", 'neuesDokument');");
				  //browser->GetMainFrame()->ExecuteJavaScript(sJSCommand,"",0);
				  //return false;

				  //var url = getContextURL() + 'services/exportExcel?reportDate=20151110';
				  //window.open(url); 
				  //const tstring sUrl = params->GetSourceUrl();
				  //std::stringstream ss;
				  //ss << "<html><script language=\"JavaScript\">"
					 // " var url = " << sUrl << ";"
					 // " window.open(url);"
					 // "</script>"
					 // "<body bgcolor=\"white\">"
					 // "</body></html>";
				  //frame->LoadString(ss.str(), params->GetSourceUrl());
				  //return false;

				  const CefString sSourceUrl = params->GetSourceUrl();	// 图片链接地址
				  if (!sSourceUrl.empty())
					  return CopyToClipboard(browser->GetHost()->GetWindowHandle(),sSourceUrl);
				  //else
					 // return CopyToClipboard(browser->GetHost()->GetWindowHandle(),params->GetLinkUrl());
			  }
			  return false;
		  }break;
	  case CLIENT_ID_OPEN_IMAGE_URL:
		  {
			  if (m_pHandler!=NULL && params->GetMediaType()==CM_MEDIATYPE_IMAGE)
			  {
				  const CefString sSourceUrl = params->GetSourceUrl();	// 图片链接地址
				  if (!sSourceUrl.empty())
				  {
					  //LoadString
					  //params->
					  if (!m_pHandler->OnBeforeBrowse(sSourceUrl.c_str()))
						  return m_pHandler->OnBeforePopup(sSourceUrl.c_str());
					  else
						  return true;
				  }
			  }
			  return false;
		  }break;
	  case CEF_CLIENT_MENU_COMMAND_ID_IMAGE_SAVEAS:
	  case CEF_CLIENT_MENU_COMMAND_ID_COPY_IMAGE:
		  {
			  if (m_pHandler!=NULL && params->GetMediaType()==CM_MEDIATYPE_IMAGE)
			  {
				  const CefString sSourceUrl = params->GetSourceUrl();	// 图片链接地址
				  if (!sSourceUrl.empty())
				  {
					  return m_pHandler->OnExecuteMenu(command_id,sSourceUrl.c_str());
				  }
			  }
			  return false;
		  }break;
    case CLIENT_ID_SHOW_DEVTOOLS:
      ShowDevTools(browser, CefPoint());
      return true;
    case CLIENT_ID_CLOSE_DEVTOOLS:
      CloseDevTools(browser);
      return true;
    case CLIENT_ID_INSPECT_ELEMENT:
      ShowDevTools(browser, CefPoint(params->GetXCoord(), params->GetYCoord()));
      return true;
	case CEF_CLIENT_MENU_COMMAND_ID_FIND:
	case MENU_ID_FIND:
      return ExecuteTestMenu(CEF_CLIENT_MENU_COMMAND_ID_FIND,NULL);
	case MENU_ID_RELOAD:
		browser->Reload();
		return true;
	case MENU_ID_RELOAD_NOCACHE:
		browser->ReloadIgnoreCache();
		return true;
	//case CLIENT_ID_RELOAD:
	//	browser->Reload();
	//	return true;
    default:  // Allow default handling, if any.
      return ExecuteTestMenu(command_id,NULL);
  }
  return false;
}

#if !defined(OS_LINUX)

bool ClientHandler::OnFileDialog(CefRefPtr<CefBrowser> browser,
                                 FileDialogMode mode,
                                 const CefString& title,
                                 const CefString& default_file_name,
                                 const std::vector<CefString>& accept_types,
																 int selected_accept_filter,
                                 CefRefPtr<CefFileDialogCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  return false;
}

#endif  // !defined(OS_LINUX)

bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                     const CefString& message,
                                     const CefString& source,
                                     int line) {
  CEF_REQUIRE_UI_THREAD();

//  bool first_message;
//  tstring logFile;
//
//  {
//    first_message = log_file_.empty();
//    if (first_message) {
//      std::stringstream ss;
//      ss << AppGetWorkingDirectory();
//#if defined(OS_WIN)
//      ss << "\\";
//#else
//      ss << "/";
//#endif
//      ss << "console.log";
//      log_file_ = ss.str();
//    }
//    logFile = log_file_;
//  }
//
//  FILE* file = fopen(logFile.c_str(), "a");
//  if (file) {
//    std::stringstream ss;
//    ss << "Message: " << std::string(message) << "\r\nSource: " <<
//        std::string(source) << "\r\nLine: " << line <<
//        "\r\n-----------------------\r\n";
//    fputs(ss.str().c_str(), file);
//    fclose(file);
//
//    if (first_message)
//      SendNotification(NOTIFY_CONSOLE_MESSAGE);
//  }

  return false;
}

void ClientHandler::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    CefRefPtr<CefBeforeDownloadCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  // Continue the download and show the "Save As" dialog.
  callback->Continue(GetDownloadPath(suggested_name), true);
}

inline int64 GetNextBigFileId(void)
{
	static int theIndex = 0;
	return (time(0)%89999999)*10000000+((++theIndex)%10000)*10000+(rand()%10000);
}

void ClientHandler::OnDownloadUpdated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    CefRefPtr<CefDownloadItemCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  //callback->Cancel();
  //callback->Pause();
  //callback->Resume();
  const uint32 nFileId = download_item->GetId();
  if (m_bCloseAll)
  {
	  CefDownloadInfo::pointer pDownloadInfo;
	  if (m_pFileDownloadList.find(nFileId,pDownloadInfo) && !pDownloadInfo->m_bCancel)
	  {
		  callback->Cancel();
		  pDownloadInfo->m_bCancel = true;
	  }
	  return;
  }

  if (m_pHandler!=NULL)
  {
	  const bool bIsInProgress = download_item->IsInProgress();
	  if (bIsInProgress && download_item->GetReceivedBytes()>0 && !m_pFileDownloadList.exist(nFileId))
	  {
		  const CefString sName = download_item->GetFullPath();
		  if (!sName.empty())
		  {
			  CefDownloadInfo::pointer pDownloadInfo = CefDownloadInfo::create(GetNextBigFileId(),download_item->GetTotalBytes());
			  pDownloadInfo->m_sFileName = sName;
			  m_pFileDownloadList.insert(nFileId,pDownloadInfo);
			  m_pHandler->OnFileReceiving(pDownloadInfo);
		  }
	  }

	  CefDownloadInfo::pointer pDownloadInfo;
	  if (download_item->IsCanceled() && m_pFileDownloadList.find(nFileId,pDownloadInfo,true))
	  {
		  m_pHandler->OnFileCencel(pDownloadInfo);
	  }else if (download_item->IsComplete() && m_pFileDownloadList.find(nFileId,pDownloadInfo,true))
	  {
		  const CefString sName = download_item->GetFullPath();
		  if (!sName.empty() && pDownloadInfo->m_sFileName!=sName.c_str())
			  pDownloadInfo->m_sFileName = sName;
			if (pDownloadInfo->m_sFileName.rfind(L".")==std::wstring::npos && download_item->GetContentDisposition().c_str()!=NULL)
			{
				const std::wstring sTemp = download_item->GetContentDisposition();
				const std::wstring::size_type nFindExt = sTemp.rfind(L".");
				if (nFindExt!=std::wstring::npos)
				{
					const std::wstring sFileExt = sTemp.substr(nFindExt);
					int nToCopyIndex = 0;
					wchar_t lpszCopyToFileName[260];
					swprintf(lpszCopyToFileName,L"%s%s",pDownloadInfo->m_sFileName.c_str(),sFileExt.c_str());
					while (true)
					{
						if (CopyFileW(pDownloadInfo->m_sFileName.c_str(),lpszCopyToFileName,TRUE))
						{
							DeleteFileW(pDownloadInfo->m_sFileName.c_str());
							pDownloadInfo->m_sFileName = lpszCopyToFileName;
							break;
						}
						swprintf(lpszCopyToFileName,L"%s(%d)%s",pDownloadInfo->m_sFileName.c_str(),(++nToCopyIndex),sFileExt.c_str());
					}
				}
			}
		  m_pHandler->OnFileReceived(pDownloadInfo);
	  }else if (bIsInProgress && m_pFileDownloadList.find(nFileId,pDownloadInfo))
	  {
		  const time_t tNow = time(0);
		  if ((tNow-pDownloadInfo->m_tLastTime)>0)
		  {
				//if (pDownloadInfo->m_nFileSize==0 && download_item->GetTotalBytes()>0)
				//	pDownloadInfo->m_nFileSize = download_item->GetTotalBytes();
			  pDownloadInfo->m_tLastTime = tNow;
			  pDownloadInfo->m_nReceivedBytes = download_item->GetReceivedBytes();
			  const double fPercent = max(0.0,pDownloadInfo->m_nFileSize==0?download_item->GetPercentComplete():(((double)download_item->GetReceivedBytes()*100.0)/(double)pDownloadInfo->m_nFileSize));
			  //const double fPercent = download_item->GetPercentComplete();//+(rand()%100)*0.01;
			  if (m_pHandler->OnFilePercent(pDownloadInfo, fPercent,download_item->GetCurrentSpeed(),tNow-pDownloadInfo->m_tCreateTime))
			  {
				  if (!pDownloadInfo->m_bCancel)
				  {
					  pDownloadInfo->m_bCancel = true;
					  callback->Cancel();
				  }
				  return;
			  }
		  }
	  }
  }

  //const int64 nTotalBytes = download_item->GetTotalBytes();
  //const int64 nReceivedBytes = download_item->GetReceivedBytes();
  //const int64 nCurrentSpeed = download_item->GetCurrentSpeed();
  //const int nPercentComplete = download_item->GetPercentComplete();

  if (download_item->IsComplete()) {
	  // OnMessageReceivedFile
    SetLastDownloadFile(download_item->GetFullPath());
    SendNotification(NOTIFY_DOWNLOAD_COMPLETE);
	m_pFileDownloadList.remove(nFileId);
  }else if (download_item->IsCanceled())
  {
	  m_pFileDownloadList.remove(nFileId);
  }
}

bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                CefDragHandler::DragOperationsMask mask) {
  CEF_REQUIRE_UI_THREAD();

  // Forbid dragging of link URLs.
  if ((mask & DRAG_OPERATION_LINK) && browser->GetMainFrame()!=NULL)
  {
	  const CefString sBrowserUrl = browser->GetMainFrame()->GetURL();
	  if (sBrowserUrl.empty())
	  {
		  //browser->GetHost()->SendCaptureLostEvent();
		  return true;	// * 避免重复打开
	  }
	  if (dragData->IsLink())
	  {
		  const CefString sLinkUrl = dragData->GetLinkURL();
		  //if (sLinkUrl==sBrowserUrl) return true;	// * 避免重复打开
		  if (m_pHandler!=NULL && !sLinkUrl.empty())
		  {
			  //m_sInDrawEnter = sLinkUrl;
			  if (!m_pHandler->OnBeforeBrowse(sLinkUrl.c_str()))
			  {
				  return m_pHandler->OnBeforePopup(sLinkUrl.c_str());
			  }
		  }
	  }else if (dragData->IsFile())
	  {
		  if (sBrowserUrl!="about:blank") return false;	// *当前有页面，默认打开，让JS可以处理
		  if (m_pHandler!=NULL)							// *当前没有页面，默认打开，支持打开多个；
		  {
			  std::vector<CefString> pFileNameList;
			  dragData->GetFileNames(pFileNameList);
			  for (size_t i=0; i<pFileNameList.size(); i++)
			  {
				  m_pHandler->OnBeforePopup(pFileNameList[i].c_str());
			  }
		  }
		  return true;
	  }
    return true;
  }

  return false;
}

bool ClientHandler::OnRequestGeolocationPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int request_id,
      CefRefPtr<CefGeolocationCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  // Allow geolocation access from all websites.
  callback->Continue(true);
  return true;
}

#if !defined(OS_LINUX)

bool ClientHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
                               const CefString& origin_url,
                               const CefString& accept_lang,
                               JSDialogType dialog_type,
                               const CefString& message_text,
                               const CefString& default_prompt_text,
                               CefRefPtr<CefJSDialogCallback> callback,
                               bool& suppress_message) {
  CEF_REQUIRE_UI_THREAD();
  if (m_pHandler!=NULL)
  {
	  bool bSuccess = true;
	  std::string sUserInput;
	  if (m_pHandler->OnJSDialog(origin_url.c_str(),accept_lang.c_str(),dialog_type,message_text.c_str(),default_prompt_text.c_str(),bSuccess,sUserInput))
	  {
		  callback->Continue(bSuccess,sUserInput.c_str());
		  return true;
	  }
  }

  return false;
}

bool ClientHandler::OnBeforeUnloadDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& message_text,
    bool is_reload,
    CefRefPtr<CefJSDialogCallback> callback) {
  CEF_REQUIRE_UI_THREAD();
  callback->Continue(true,"");	// ?控制不显示退出提示框
  return true;//false;
}

void ClientHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
}

#endif  // !defined(OS_LINUX)

bool ClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                  const CefKeyEvent& event,
                                  CefEventHandle os_event,
                                  bool* is_keyboard_shortcut) {
  CEF_REQUIRE_UI_THREAD();
  
  //event.type 
  //event.windows_key_code
  if (event.windows_key_code == 0x74)	// 116 F5
  {
	  // os_event. VK_F5
	  //os_event->
  }else if (!event.focus_on_editable_field && event.windows_key_code == 0x20) {
    // Special handling for the space character when an input element does not
    // have focus. Handling the event in OnPreKeyEvent() keeps the event from
    // being processed in the renderer. If we instead handled the event in the
    // OnKeyEvent() method the space key would cause the window to scroll in
    // addition to showing the alert box.
    //if (event.type == KEYEVENT_RAWKEYDOWN) {
    //  browser->GetMainFrame()->ExecuteJavaScript(
    //      "alert('You pressed the space bar!');", "", 0);
    //}
    return true;
  }else if (event.focus_on_editable_field)
  {
	  //if (os_event!=NULL)
	  //{
		 // //CRect rect;
		 // //::GetWindowRect(os_event->hwnd,rect);

		 // CPoint pos;
		 // ::GetCursorPos(&pos);

		 // CefMouseEvent pEvent;
		 // pEvent.x = pos.x;
		 // pEvent.y = pos.y;

		 // //pEvent.x = rect.left+2;
		 // //pEvent.y = rect.top+2;
		 // browser->GetHost()->SendMouseMoveEvent(pEvent,true);
		 // browser->GetHost()->SendMouseMoveEvent(pEvent,false);
		 // //browser->GetHost()->SendCaptureLostEvent();

		 // //CefTextInputContext pInputContext = browser->GetHost()->GetNSTextInputContext();

		 // //CPoint pos;
		 // //pos.x = LOWORD(os_event->lParam);
		 // //pos.y = HIWORD(os_event->lParam);
		 // //int i=0;
	  //}

	  //if (event.type==KEYEVENT_CHAR)
	  //{
		 // //browser->GetMainFrame()->ExecuteJavaScript("alert(window.myval);","",0);

		 // //CefRefPtr<CefBrowser> pBrowser = this->GetBrowser();
		 // //pBrowser->GetMainFrame()->ExecuteJavaScript("window.xxx = 12;","",0);
		 // ////CEF_REQUIRE_RENDERER_THREAD();
		 // //CefRefPtr<CefV8Context> pContext = pBrowser->GetMainFrame()->GetV8Context();
		 // //if (pContext!=NULL)
		 // //{
			// // CefRefPtr<CefV8Value> pXXX = pContext->GetGlobal()->GetValue(CefString("xxx"));
			// // const int32 x = pXXX->GetIntValue();
		 // //}

		 //// //if (m_bSendMouseLeaveEvent && browser->GetHost()!=NULL)
		 //// //{
			//// // m_bSendMouseLeaveEvent = false;
			//// // CPoint pos;
			//// // ::GetCursorPos(&pos);
			//// // CefMouseEvent pEvent;
			//// // pEvent.x = pos.x;
			//// // pEvent.y = pos.y;
			//// // browser->GetHost()->SendMouseMoveEvent(pEvent,true);
		 //// //}
		 //// ////browser->GetHost()->NotifyScreenInfoChanged();
		 //// ////browser->GetHost()->Invalidate(PET_VIEW);

		 //// //if (m_pHandler!=NULL)
			//// // m_pHandler->OnFirstKeyOnEditableField();

		 //// //HWND hwnd = browser->GetHost()->GetWindowHandle();
		 //// //if (hwnd!=NULL && (hwnd=::GetParent(hwnd))!=NULL)
		 //// //{
			//// // RECT rect;
			//// // ::GetWindowRect(hwnd,&rect);
			//// // ::InvalidateRect(hwnd,&rect,TRUE);
		 //// //}
	  //}
	  //const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
	  const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
	  if (!bControl || os_event==NULL || os_event->wParam!=VK_TAB)
		  return false;
  //}else if (!m_bSendMouseLeaveEvent)
  //{
	 // m_bSendMouseLeaveEvent = true;
  }

  if (m_pHandler!=NULL && os_event!=NULL && browser->GetMainFrame()!=NULL)
	  return m_pHandler->OnKeyEvent(browser->GetMainFrame()->GetURL().c_str(),os_event);

  return false;
}
bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser,
						const CefKeyEvent& event,
						CefEventHandle os_event)
{
	if (os_event!=NULL && os_event->message==WM_KEYDOWN && os_event->wParam==VK_F5)
	{
		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		if (bControl)
			browser->Reload();
		else
			browser->ReloadIgnoreCache();
		return true;
	}else if (os_event!=NULL && os_event->message==WM_KEYDOWN && os_event->wParam==VK_F12)
	{
		ShowDevTools(browser, CefPoint());
		return true;
	}
	//if (m_pHandler!=NULL && os_event!=NULL )
	//	return m_pHandler->OnKeyEvent(browser->GetMainFrame()->GetURL().c_str(),os_event);

	return false;
}

#if (CEF_VERSION==2526)
bool ClientHandler::OnBeforePopup(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& target_url,
      const CefString& target_frame_name,
      CefLifeSpanHandler::WindowOpenDisposition target_disposition,
      bool user_gesture,
      const CefPopupFeatures& popupFeatures,
      CefWindowInfo& windowInfo,
      CefRefPtr<CefClient>& client,
      CefBrowserSettings& settings,
	  bool* no_javascript_access) {
#else
bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  const CefString& target_url,
                                  const CefString& target_frame_name,
                                  const CefPopupFeatures& popupFeatures,
                                  CefWindowInfo& windowInfo,
                                  CefRefPtr<CefClient>& client,
                                  CefBrowserSettings& settings,
                                  bool* no_javascript_access) {
#endif
  CEF_REQUIRE_IO_THREAD();

  //CefAddWebPluginDirectory("C:\windows\SysWOW64\Macromed\Flash\NPSWF32_12_0_0_70.dll");
  //CefRefreshWebPlugins();
  if (m_pHandler)
	  return m_pHandler->OnBeforePopup(target_url.c_str());

  //*no_javascript_access = false;
  if (browser->GetHost()->IsWindowRenderingDisabled()) {
    // Cancel popups in off-screen rendering mode.
    return true;
  }
  return false;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

#ifdef USES_MESSAGE_ROUTER
  if (!message_router_) {
    // Create the browser-side router for query handling.
    CefMessageRouterConfig config;
    message_router_ = CefMessageRouterBrowserSide::Create(config);

    // Register handlers with the router.
    CreateMessageHandlers(message_handler_set_);
    MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    for (; it != message_handler_set_.end(); ++it)
      message_router_->AddHandler(*(it), false);
  }
#endif

  browser->GetHost()->AddWordToDictionary("www");
  browser->GetHost()->AddWordToDictionary("entboost");
  browser->GetHost()->AddWordToDictionary("Entboost");
  browser->GetHost()->AddWordToDictionary("ENTBOOST");
  // Disable mouse cursor change if requested via the command-line flag.
  //if (mouse_cursor_change_disabled_)
  //  browser->GetHost()->SetMouseCursorChangeDisabled(true);
  //const bool b = browser->GetHost()->IsWindowRenderingDisabled();

  if (!GetBrowser())   {
    base::AutoLock lock_scope(lock_);
    // We need to keep the main child window, but not popup windows
    browser_ = browser;
    browser_id_ = browser->GetIdentifier();
  } else if (browser->IsPopup()) {
    // Add to the list of popup browsers.
    popup_browsers_.push_back(browser);

    // Give focus to the popup browser. Perform asynchronously because the
    // parent window may attempt to keep focus after launching the popup.
    CefPostTask(TID_UI,
        base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true));
  }

  browser_count_++;
}

bool ClientHandler::RunModal(CefRefPtr<CefBrowser> browser)
{
	return false;
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (GetBrowserId() == browser->GetIdentifier()) {
	  if (m_pHandler!=NULL)
	  {
		  if (m_pHandler->OnCefClose())
			  return true;
	  }
    base::AutoLock lock_scope(lock_);
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

#ifdef USES_MESSAGE_ROUTER
  message_router_->OnBeforeClose(browser);
#endif

  OnClearAll();

  if (GetBrowserId() == browser->GetIdentifier()) {
    {
      base::AutoLock lock_scope(lock_);
      // Free the browser pointer so that the browser can be destroyed
      browser_ = NULL;
    }

    if (osr_handler_.get()) {
      osr_handler_->OnBeforeClose(browser);
      osr_handler_ = NULL;
    }
  } else if (browser->IsPopup()) {
    // Remove from the browser popup list.
    BrowserList::iterator bit = popup_browsers_.begin();
    for (; bit != popup_browsers_.end(); ++bit) {
      if ((*bit)->IsSame(browser)) {
        popup_browsers_.erase(bit);
        break;
      }
    }
  }

  if (--browser_count_ == 0) {
#ifdef USES_MESSAGE_ROUTER
    // All browser windows have closed.
    // Remove and delete message router handlers.
    MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    for (; it != message_handler_set_.end(); ++it) {
      message_router_->RemoveHandler(*(it));
      delete *(it);
    }
    message_handler_set_.clear();
    message_router_ = NULL;
#endif

    // Quit the application message loop.
    //AppQuitMessageLoop();
  }
}

//class CCefDOMVisitor : public CefDOMVisitor
//{
//public:
//	CCefDOMVisitor(void){}
//	//~CCefDOMVisitor(void){}
//
//  virtual void Visit(CefRefPtr<CefDOMDocument> document)
//  {
//	  CefString sTitle = document->GetTitle();
//  }
//  IMPLEMENT_REFCOUNTING(CCefDOMVisitor);
//};

void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool isLoading,
                                         bool canGoBack,
                                         bool canGoForward) {
  CEF_REQUIRE_UI_THREAD();
  if (GetBrowserId() == browser->GetIdentifier() && browser->GetMainFrame()!=NULL)
  {
	  if (m_pHandler)
		  m_pHandler->OnLoadingStateChange(browser->GetMainFrame()->GetURL().c_str(),isLoading,canGoBack,canGoForward);
  }
  //if (!isLoading)
  //{
	 // // ok
	 // //browser->GetMainFrame()->GetSource();

	 // CefRefPtr<CCefDOMVisitor> visitor = new CCefDOMVisitor();
	 // browser->GetMainFrame()->VisitDOM(visitor.get());
	 // //CefRefPtr<CefDOMDocument> document = new CefDOMDocument;
	 // //visitor->Visit(document);
	 // //CefString sTitle = document->GetTitle();
  //}

  //SetLoading(isLoading);
  //SetNavState(canGoBack, canGoForward);
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Don't display an error for external protocols that we allow the OS to
  // handle. See OnProtocolExecution().
  if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
    tstring urlStr = frame->GetURL();
    if (urlStr.find("spotify:") == 0)
      return;
  }

  if (m_pHandler!=NULL)
  {
	  m_pHandler->OnLoadError(errorCode, errorText.c_str(), failedUrl.c_str());
  }

  // Display a load error message.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
	  "<h2>Failed to load URL " << std::string(failedUrl) <<
	  " with error " << std::string(errorText) << " (" << errorCode <<
	  ").</h2></body></html>";
  frame->LoadString(ss.str(), failedUrl);
}

bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   bool is_redirect) {
  CEF_REQUIRE_UI_THREAD();

	//CefRequest::HeaderMap hdrMap;
	//request->GetHeaderMap(hdrMap);
	//hdrMap.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
	//request->SetHeaderMap(hdrMap);

  //const int nFlags = request->GetFlags();
  //const cef_resource_type_t nResourceType = request->GetResourceType();
  //const cef_transition_type_t nTransitionType = request->GetTransitionType();

  //const CefString s0 = browser->GetMainFrame()->GetURL();
  //const CefString s1 = frame->GetURL();
  //const CefString s2 = request->GetURL();
  if (GetBrowserId() == browser->GetIdentifier() && request!=NULL)
  {
	  //if (s2=="https://pbsz.ebank.cmbchina.com/CmbBank_CreditCardV2/UI/CreditCardPC/CreditCardV2_CardManager/cm_QueryTranfserCardRegistration.aspx#")
	  //{
		 // if (m_pHandler)
			//  m_pHandler->OnBeforePopup(s2.c_str());
		 // return true;
	  //}
	  if (m_pHandler)
	  {
		  if (m_pHandler->OnBeforeBrowse(request->GetURL().c_str()))
			  return true;
	  }
  }

  //CefString s1 = frame->GetURL();
  //CefString s2 = request->GetURL();
  //if (std::string(s2).find("http://www.baidu.com")==std::string::npos)
  //{
	 // return true;
  //}

#ifdef USES_MESSAGE_ROUTER
  message_router_->OnBeforeBrowse(browser, frame);
#endif
  return false;
}

CefRefPtr<CefResourceHandler> ClientHandler::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
  CEF_REQUIRE_IO_THREAD();

  //tstring url = request->GetURL();
  //if (url.find(kTestOrigin) == 0) {
  //  // Handle URLs in the test origin.
  //  tstring file_name, mime_type;
  //  if (ParseTestUrl(url, &file_name, &mime_type)) {
  //    if (file_name == "request.html") {
  //      // Show the request contents.
  //      tstring dump;
  //      DumpRequestContents(request, dump);
  //      tstring str = "<html><body bgcolor=\"white\"><pre>" + dump +
  //                        "</pre></body></html>";
  //      CefRefPtr<CefStreamReader> stream =
  //          CefStreamReader::CreateForData(
  //              static_cast<void*>(const_cast<char*>(str.c_str())),
  //              str.size());
  //      DCHECK(stream.get());
  //      return new CefStreamResourceHandler("text/html", stream);
  //    } else {
  //      // Load the resource from file.
  //      CefRefPtr<CefStreamReader> stream =
  //          GetBinaryResourceReader(file_name.c_str());
  //      if (stream.get())
  //        return new CefStreamResourceHandler(mime_type, stream);
  //    }
  //  }
  //}

  return NULL;
}

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                   const CefString& origin_url,
                                   int64 new_size,
                                   CefRefPtr<CefRequestCallback> callback) {
  CEF_REQUIRE_IO_THREAD();

  static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

  // Grant the quota request if the size is reasonable.
  callback->Continue(new_size <= max_size);
  return true;
}

void ClientHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                        const CefString& url,
                                        bool& allow_os_execution) {
  CEF_REQUIRE_UI_THREAD();

  tstring urlStr = url;

  // Allow OS execution of Spotify URIs.
  if (urlStr.find("spotify:") == 0)
    allow_os_execution = true;
}

bool ClientHandler::OnBeforePluginLoad(CefRefPtr<CefBrowser> browser,
                                  const CefString& url,
                                  const CefString& policy_url,
                                  CefRefPtr<CefWebPluginInfo> info)
{
	//const std::wstring sPluginName = info->GetName();
	//if (sPluginName.find(L"360")!=std::wstring::npos)
	//	return true;
	return false;
}

void ClientHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                              TerminationStatus status) {
  CEF_REQUIRE_UI_THREAD();

#ifdef USES_MESSAGE_ROUTER
  message_router_->OnRenderProcessTerminated(browser);
#endif

  // Load the startup URL if that's not the website that we terminated on.
  CefRefPtr<CefFrame> frame = browser->GetMainFrame();
  std::string url(frame->GetURL());
  std::transform(url.begin(), url.end(), url.begin(), tolower);

  std::string startupURL(GetStartupURL());
  if (startupURL != "chrome://crash" && !url.empty() &&
      url.find(startupURL) != 0) {
    frame->LoadURL(startupURL);
  }
}

bool ClientHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
                                      CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return false;
  return osr_handler_->GetRootScreenRect(browser, rect);
}

bool ClientHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return false;
  return osr_handler_->GetViewRect(browser, rect);
}

bool ClientHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
                                   int viewX,
                                   int viewY,
                                   int& screenX,
                                   int& screenY) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return false;
  return osr_handler_->GetScreenPoint(browser, viewX, viewY, screenX, screenY);
}

bool ClientHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                  CefScreenInfo& screen_info) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return false;
  return osr_handler_->GetScreenInfo(browser, screen_info);
}

void ClientHandler::OnPopupShow(CefRefPtr<CefBrowser> browser,
                                bool show) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return;
  return osr_handler_->OnPopupShow(browser, show);
}

void ClientHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
                                const CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return;
  return osr_handler_->OnPopupSize(browser, rect);
}

void ClientHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                            PaintElementType type,
                            const RectList& dirtyRects,
                            const void* buffer,
                            int width,
                            int height) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return;
  osr_handler_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void ClientHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
                                   CefCursorHandle cursor,
                                   CursorType type,
                                   const CefCursorInfo& custom_cursor_info) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return;
  osr_handler_->OnCursorChange(browser, cursor, type, custom_cursor_info);
}

bool ClientHandler::StartDragging(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDragData> drag_data,
    CefRenderHandler::DragOperationsMask allowed_ops,
    int x, int y) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return false;
  return osr_handler_->StartDragging(browser, drag_data, allowed_ops, x, y);
}

void ClientHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
    CefRenderHandler::DragOperation operation) {
  CEF_REQUIRE_UI_THREAD();
  if (!osr_handler_.get())
    return;
  osr_handler_->UpdateDragCursor(browser, operation);
}

void ClientHandler::SetMainWindowHandle(ClientWindowHandle handle) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&ClientHandler::SetMainWindowHandle, this, handle));
    return;
  }

  main_handle_ = handle;
}

ClientWindowHandle ClientHandler::GetMainWindowHandle() const {
  CEF_REQUIRE_UI_THREAD();
  return main_handle_;
}

void ClientHandler::SetEditWindowHandle(ClientWindowHandle handle) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&ClientHandler::SetEditWindowHandle, this, handle));
    return;
  }

  edit_handle_ = handle;
}

void ClientHandler::SetButtonWindowHandles(ClientWindowHandle backHandle,
                                           ClientWindowHandle forwardHandle,
                                           ClientWindowHandle reloadHandle,
                                           ClientWindowHandle stopHandle) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&ClientHandler::SetButtonWindowHandles, this,
                   backHandle, forwardHandle, reloadHandle, stopHandle));
    return;
  }

  back_handle_ = backHandle;
  forward_handle_ = forwardHandle;
  reload_handle_ = reloadHandle;
  stop_handle_ = stopHandle;
}

void ClientHandler::SetOSRHandler(CefRefPtr<RenderHandler> handler) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&ClientHandler::SetOSRHandler, this, handler));
    return;
  }

  osr_handler_ = handler;
}

CefRefPtr<ClientHandler::RenderHandler> ClientHandler::GetOSRHandler() const {
  return osr_handler_; 
}

CefRefPtr<CefBrowser> ClientHandler::GetBrowser() const {
  base::AutoLock lock_scope(lock_);
  return browser_;
}

int ClientHandler::GetBrowserId() const {
  base::AutoLock lock_scope(lock_);
  return browser_id_;
}

void ClientHandler::OnClearAll(void)
{
	//if (m_pHandler!=NULL)
	{
		BoostReadLock rdLock(m_pFileDownloadList.mutex());
		CLockMap<uint32,CefDownloadInfo::pointer>::iterator pIter = m_pFileDownloadList.begin();
		for (; pIter!=m_pFileDownloadList.end(); pIter++)
		{
			CefDownloadInfo::pointer pDownloadInfo = pIter->second;
			for (int i=0;i<20;i++)	// max 2S
			{
				Sleep(100);
				if (pDownloadInfo->m_bCancel)
				{
					if (m_pHandler!=NULL)
						m_pHandler->OnFileCencel(pDownloadInfo);
					Sleep(100);
					break;
				}
			}
		}
	}
	m_pFileDownloadList.clear();
}
void ClientHandler::CloseAllBrowsers(bool force_close) {
	m_bCloseAll = true;
	OnClearAll();
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&ClientHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	try
	{
		if (!popup_browsers_.empty()) {
			// Request that any popup browsers close.
			BrowserList::const_iterator it = popup_browsers_.begin();
			for (; it != popup_browsers_.end(); ++it)
				(*it)->GetHost()->CloseBrowser(force_close);
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}

	try
	{
		if (browser_.get()!=NULL) {
			// Request that the main browser close.
			browser_->GetHost()->CloseBrowser(force_close);
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
}

bool ClientHandler::IsClosing() const {
  base::AutoLock lock_scope(lock_);
  return is_closing_;
}

std::string ClientHandler::GetLogFile() const {
  CEF_REQUIRE_UI_THREAD();
  return log_file_;
}

void ClientHandler::SetLastDownloadFile(const std::string& fileName) {
  CEF_REQUIRE_UI_THREAD();
  last_download_file_ = fileName;
}

std::string ClientHandler::GetLastDownloadFile() const {
  CEF_REQUIRE_UI_THREAD();
  return last_download_file_;
}

void ClientHandler::ShowDevTools(CefRefPtr<CefBrowser> browser,
                                 const CefPoint& inspect_element_at) {
  CefWindowInfo windowInfo;
  CefBrowserSettings settings;

#if defined(OS_WIN)
  windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
#endif

  browser->GetHost()->ShowDevTools(windowInfo, this, settings,
                                   inspect_element_at);
}

void ClientHandler::CloseDevTools(CefRefPtr<CefBrowser> browser) {
  browser->GetHost()->CloseDevTools();
}

std::string ClientHandler::GetStartupURL() const {
  return startup_url_;
}

void ClientHandler::BeginTracing() {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI, base::Bind(&ClientHandler::BeginTracing, this));
    return;
  }

  CefBeginTracing(CefString(), NULL);
}

void ClientHandler::EndTracing() {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI, base::Bind(&ClientHandler::EndTracing, this));
    return;
  }

  class Client : public CefEndTracingCallback,
                  public CefRunFileDialogCallback {
    public:
    explicit Client(CefRefPtr<ClientHandler> handler)
        : handler_(handler) {
      RunDialog();
    }

    void RunDialog() {
      static const char kDefaultFileName[] = "trace.txt";
	  std::string path(handler_->GetDownloadPath(kDefaultFileName));
      if (path.empty())
        path = kDefaultFileName;

#if (CEF_VERSION>=2272)
      // Results in a call to OnFileDialogDismissed.
      handler_->GetBrowser()->GetHost()->RunFileDialog(
          static_cast<cef_file_dialog_mode_t>(
              FILE_DIALOG_SAVE | FILE_DIALOG_OVERWRITEPROMPT_FLAG),
          CefString(),  // title
          path,
          std::vector<CefString>(),  // accept_filters
          0,  // selected_accept_filter
          this);
#else
      // Results in a call to OnFileDialogDismissed.
      handler_->GetBrowser()->GetHost()->RunFileDialog(
          FILE_DIALOG_SAVE, CefString(), path, std::vector<CefString>(),
          this);
#endif
    }

#if (CEF_VERSION>=2272)
    void OnFileDialogDismissed(
        int selected_accept_filter,
        const std::vector<CefString>& file_paths) OVERRIDE {
      if (!file_paths.empty()) {
        // File selected. Results in a call to OnEndTracingComplete.
        CefEndTracing(file_paths.front(), this);
      } else {
        // No file selected. Discard the trace data.
        CefEndTracing(CefString(), NULL);
      }
    }
#else
    virtual void OnFileDialogDismissed(
        CefRefPtr<CefBrowserHost> browser_host,
        const std::vector<CefString>& file_paths) OVERRIDE {
      CEF_REQUIRE_UI_THREAD();
      if (!file_paths.empty()) {
        // File selected. Results in a call to OnEndTracingComplete.
        CefEndTracing(file_paths.front(), this);
      } else {
        // No file selected. Discard the trace data.
        CefEndTracing(CefString(), NULL);
      }
    }
#endif

    virtual void OnEndTracingComplete(
        const CefString& tracing_file) OVERRIDE {
      CEF_REQUIRE_UI_THREAD();
      handler_->SetLastDownloadFile(tracing_file.ToString());
      handler_->SendNotification(NOTIFY_DOWNLOAD_COMPLETE);
    }

    private:
    CefRefPtr<ClientHandler> handler_;

    IMPLEMENT_REFCOUNTING(Client);
  };

  new Client(this);
}

bool ClientHandler::Save(const std::string& path, const std::string& data) {
  FILE* f = fopen(path.c_str(), "w");
  if (!f)
    return false;
  size_t total = 0;
  do {
    size_t write = fwrite(data.c_str() + total, 1, data.size() - total, f);
    if (write == 0)
      break;
    total += write;
  } while (total < data.size());
  fclose(f);
  return true;
}

// static
void ClientHandler::CreateMessageHandlers(MessageHandlerSet& handlers) {
  //// Create the dialog test handlers.
  //dialog_test::CreateMessageHandlers(handlers);

  //// Create the binding test handlers.
  //binding_test::CreateMessageHandlers(handlers);

  //// Create the window test handlers.
  //window_test::CreateMessageHandlers(handlers);
}

//void ClientHandler::BuildTestMenu(CefRefPtr<CefMenuModel> model) {
//  if (model->GetCount() > 0)
//    model->AddSeparator();
//
//  // Build the sub menu.
//  CefRefPtr<CefMenuModel> submenu =
//      model->AddSubMenu(CLIENT_ID_TESTMENU_SUBMENU, "Context Menu Test");
//  submenu->AddCheckItem(CLIENT_ID_TESTMENU_CHECKITEM, "Check Item");
//  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM1, "Radio Item 1", 0);
//  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM2, "Radio Item 2", 0);
//  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM3, "Radio Item 3", 0);
//
//  // Check the check item.
//  if (test_menu_state_.check_item)
//    submenu->SetChecked(CLIENT_ID_TESTMENU_CHECKITEM, true);
//
//  // Check the selected radio item.
//  submenu->SetChecked(
//      CLIENT_ID_TESTMENU_RADIOITEM1 + test_menu_state_.radio_item, true);
//}

bool ClientHandler::ExecuteTestMenu(int command_id, const wchar_t* lpszUrl) {

  if (command_id == CEF_CLIENT_MENU_COMMAND_ID_FIND)
  {
	  if (m_pHandler!=NULL)
		  return m_pHandler->OnExecuteMenu(command_id,lpszUrl);
  }
  return false;



  //if (command_id == CLIENT_ID_TESTMENU_CHECKITEM) {
  //  // Toggle the check item.
  //  test_menu_state_.check_item ^= 1;
  //  return true;
  //} else if (command_id >= CLIENT_ID_TESTMENU_RADIOITEM1 &&
  //           command_id <= CLIENT_ID_TESTMENU_RADIOITEM3) {
  //  // Store the selected radio item.
  //  test_menu_state_.radio_item = (command_id - CLIENT_ID_TESTMENU_RADIOITEM1);
  //  return true;
  //}

  //// Allow default handling to proceed.
  //return false;
}

