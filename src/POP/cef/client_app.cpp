// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifdef WIN32
#pragma warning(disable:4819)
#endif

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.
#include "client_app.h"  // NOLINT(build/include)

#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "include/wrapper/cef_helpers.h"

ClientApp::ClientApp()
//: m_v8Handler(new ClientV8ExtensionHandler())
{
}
ClientApp::~ClientApp()
{
}

void ClientApp::OnRegisterCustomSchemes(
    CefRefPtr<CefSchemeRegistrar> registrar) {
  // Default schemes that support cookies.
  cookieable_schemes_.push_back("http");
  cookieable_schemes_.push_back("https");

  //RegisterCustomSchemes(registrar, cookieable_schemes_);
}

void ClientApp::OnContextInitialized() {
  //CreateBrowserDelegates(browser_delegates_);

  // Register cookieable schemes with the global cookie manager.
#if (CEF_VERSION==2526)
  CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_, NULL);
#else
  CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_);
#endif

  //print_handler_ = CreatePrintHandler();

  BrowserDelegateSet::iterator it = browser_delegates_.begin();
  for (; it != browser_delegates_.end(); ++it)
    (*it)->OnContextInitialized(this);
}

void ClientApp::OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) {
  BrowserDelegateSet::iterator it = browser_delegates_.begin();
  for (; it != browser_delegates_.end(); ++it)
    (*it)->OnBeforeChildProcessLaunch(this, command_line);
}

void ClientApp::OnRenderProcessThreadCreated(
    CefRefPtr<CefListValue> extra_info) {
  BrowserDelegateSet::iterator it = browser_delegates_.begin();
  for (; it != browser_delegates_.end(); ++it)
    (*it)->OnRenderProcessThreadCreated(this, extra_info);
}

void ClientApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {
  //CreateRenderDelegates(render_delegates_);

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnRenderThreadCreated(this, extra_info);
}


// Execute with the specified argument list and return value.  Return true if  
// the method was handled.  
//该方法继承CefV8Handler必需实现  
//功能:判断js调用的本地方法名称，接收参数传递和返回值，实现方法具体功能  
bool ClientV8ExtensionHandler::Execute(const CefString& name,  
									   CefRefPtr<CefV8Value> object,  
									   const CefV8ValueList& arguments,  
									   CefRefPtr<CefV8Value>& retval,  
									   CefString& exception)
{  
	//MessageBoxW(NULL,name.c_str(),L"",MB_OK);
	if (name == "SetTestParam") {  
		// Handle the SetTestParam native function by saving the string argument  
		// into the local member.  
		if (arguments.size() != 1 || !arguments[0]->IsString())  
			return false;  

		//test_param_ = arguments[0]->GetStringValue();  
		return true;  
		//} else if (name == "GetTestParam") {  
		//  // Handle the GetTestParam native function by returning the local member  
		//  // value.  
		//  retval = CefV8Value::CreateString(test_param_);  
		//  return true;  
		//} else if (name == "GetTestObject") {  
		//  // Handle the GetTestObject native function by creating and returning a  
		//  // new V8 object.  
		//  retval = CefV8Value::CreateObject(NULL);  
		//  // Add a string parameter to the new V8 object.  
		//  retval->SetValue("param", CefV8Value::CreateString(  
		//      "Retrieving a parameter on a native object succeeded."),  
		//      V8_PROPERTY_ATTRIBUTE_NONE);  
		//  // Add a function to the new V8 object.  
		//  retval->SetValue("GetMessage",  
		//      CefV8Value::CreateFunction("GetMessage", this),  
		//      V8_PROPERTY_ATTRIBUTE_NONE);  
		//  return true;  
		//}  else if (name == "GetMessage") {  
		//  // Handle the GetMessage object function by returning a string.  
		//  retval = CefV8Value::CreateString(  
		//      "Calling a function on a native object succeeded.");  
		//  return true;  
	}  
	//else if (name == "loadurl")  
	//{  
	//    if (arguments.size() != 1)  
	//        return false;  
	//    if (arguments[0]->IsString())  
	//    {  
	//        retval =  CefV8Value::CreateString( arguments[0]->GetStringValue());  
	//    }  
	//    return true;  
	//}  
	//else if (name == "IndicatorsCloud")  
	//{  
	//    if (arguments.size() != 2)  
	//        return false;  
	//    if (arguments[0]->IsInt() && arguments[1]->IsString())  
	//    {  
	//        retval = CefV8Value::CreateInt(arguments[0]->GetIntValue());  
	//    }  
	//    return true;  
	//}  
	else if (name == "on_first_keydown")  
	{  
		//wchar_t lpszBuf[32];
		//swprintf(lpszBuf, L"size=%d",arguments.size());
		//MessageBoxW(NULL,name.c_str(),lpszBuf,MB_OK);
		if (arguments.size() != 4)
			return false;

		if (arguments[0]->IsInt() && arguments[1]->IsInt() && arguments[2]->IsInt() && arguments[3]->IsInt())
		{  
			const int32 x = arguments[0]->GetIntValue();
			const int32 y = arguments[1]->GetIntValue();
			const int32 w = arguments[2]->GetIntValue();
			const int32 h = arguments[3]->GetIntValue();

			//swprintf(lpszBuf, L"x=%d,y=%d",x,y);
			//MessageBoxW(NULL,lpszBuf,L"",MB_OK);
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(name);
			CefRefPtr<CefListValue> args = msg->GetArgumentList();
			args->SetSize(4);
			args->SetInt(0, x);
			args->SetInt(1, y);
			args->SetInt(2, w);
			args->SetInt(3, h);
			m_pBrowser->SendProcessMessage(PID_BROWSER,msg);
			retval = CefV8Value::CreateInt(0);  
		}  
		return true;  
	}else if (name == "can_execute_javascript")  
	{  
		if (arguments.size() != 1) return false;
		const CefString sFunctionName = arguments[0]->GetStringValue();
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(name);
		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		args->SetSize(2);
		args->SetBool(0, true);
		args->SetString(1, sFunctionName);
		m_pBrowser->SendProcessMessage(PID_BROWSER,msg);
		retval = CefV8Value::CreateInt(0);  
		return true;  
	}
	return false;  
}  
  
//void InitExtensionTest(CefRefPtr<CefV8Handler> handler) {  
//  // Register a V8 extension with the below JavaScript code that calls native  
//  // methods implemented in ClientV8ExtensionHandler.  
//  std::string code = "var cef;"  
//    "if (!cef)"  
//    "  cef = {};"  
//    "if (!cef.entboost)"  
//    "  cef.entboost = {};"  
//    "(function() {"  
//    //"  cef.entboost.__defineGetter__('test_param', function() {"  
//    //"    native function GetTestParam();"  
//    //"    return GetTestParam();"  
//    //"  });"  
//    //"  cef.entboost.__defineSetter__('test_param', function(b) {"  
//    //"    native function SetTestParam();"  
//    //"    if (b) SetTestParam(b);"  
//    //"  });"  
//    //"  cef.entboost.test_object = function() {"  
//    //"    native function GetTestObject();"  
//    //"    return GetTestObject();"  
//    //"  };"  
//    //"  cef.entboost.loadurl = function(val) {"  
//    //"    native function loadurl();"  
//    //"    return loadurl(val);"  
//    //"  };"  
//    //" cef.entboost.IndicatorsCloud = function(val1, val2) {"  
//    //"    native function IndicatorsCloud();"  
//    //"    return IndicatorsCloud(val1, val2);"  
//    //"  };"  
//    " cef.entboost.on_mouseleave = function(x, y) {"  
//    "    native function on_mouseleave();"  
//    "    return on_mouseleave(x, y);"  
//    "  };"  
//    "})();";  
//  CefRegisterExtension("v8/entboost", code, handler);//code表示的字符串是任意合法的JS代码  
//}  
//
//void RunExtensionTest(CefRefPtr<CefBrowser> browser) {  
//    std::string html =  
//        "<html><body>ClientV8ExtensionHandler says:<br><pre>"  
//        "<script language=\"JavaScript\">"  
//        "cef.test.test_param ="  
//        "  'Assign and retrieve a value succeeded the first time.';"  
//        "document.writeln(cef.test.test_param);"  
//        "cef.test.test_param ="  
//        "  'Assign and retrieve a value succeeded the second time.';"  
//        "document.writeln(cef.test.test_param);"  
//        "var obj = cef.test.test_object();"  
//        //"var objxy = cef.test.ObjectXY(1,2);"  
//        "document.writeln(obj.param);"  
//        "document.writeln(obj.GetMessage());"  
//        "document.writeln(cef.test.loadurl(\"hello world.\"));"  
//        "document.writeln(cef.test.IndicatorsCloud(123456789, \"leadsec.\"));"  
//    "</script>"  
//    "</pre></body></html>";  
//  browser->GetMainFrame()->LoadString(html, "about:blank");  
//}  


void ClientApp::OnWebKitInitialized() {
	//InitExtensionTest(m_v8Handler);
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnWebKitInitialized(this);
}

void ClientApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserCreated(this, browser);
}

void ClientApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> ClientApp::GetLoadHandler() {
  CefRefPtr<CefLoadHandler> load_handler;
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !load_handler.get(); ++it)
    load_handler = (*it)->GetLoadHandler(this);

  return load_handler;
}

bool ClientApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   NavigationType navigation_type,
                                   bool is_redirect) {

	//CefRequest::HeaderMap hdrMap;
	//request->GetHeaderMap(hdrMap);
	//hdrMap.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
	//request->SetHeaderMap(hdrMap);

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it) {
    if ((*it)->OnBeforeNavigation(this, browser, frame, request,
                                  navigation_type, is_redirect)) {
      return true;
    }
  }

  return false;
}

void ClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context)
{
	CefRefPtr<ClientV8ExtensionHandler> v8Handler(new ClientV8ExtensionHandler(browser));
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	{
		// Create the "on_first_keydown" function.
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("on_first_keydown", v8Handler);
		// Add the "on_first_keydown" function to the "window" object.
		object->SetValue("entboost_on_first_keydown", func, V8_PROPERTY_ATTRIBUTE_NONE);
	}
	{
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("can_execute_javascript", v8Handler);
		object->SetValue("entboost_can_execute_javascript", func, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	// typeof(函数名)=="function"
	browser->GetMainFrame()->ExecuteJavaScript("if (typeof(entboost_on_user_off_line)==\"function\") window.entboost_can_execute_javascript('entboost_on_user_off_line');","",0);
	browser->GetMainFrame()->ExecuteJavaScript("if (typeof(entboost_on_user_logon_success)==\"function\") window.entboost_can_execute_javascript('entboost_on_user_logon_success');","",0);

	//CefRefPtr<CefV8Value> object = context->GetGlobal();
	//// Create a new V8 string value. See the "Basic JS Types" section below.
	//CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");
	//// Add the string to the window object as "window.myval". See the "JS Objects" section below.
	//object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);

	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnContextCreated(this, browser, frame, context);
}

void ClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextReleased(this, browser, frame, context);
}

void ClientApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefV8Context> context,
                                    CefRefPtr<CefV8Exception> exception,
                                    CefRefPtr<CefV8StackTrace> stackTrace) {
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it) {
    (*it)->OnUncaughtException(this, browser, frame, context, exception,
                               stackTrace);
  }
}

void ClientApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefDOMNode> node)
{
	//frame->ExecuteJavaScript("cef.entboost.on_mouseleave(100,200);","",0);
	//POINT pos;
	//::GetCursorPos(&pos);
	//char lpszCommand[64];
	//sprintf(lpszCommand, "window.entboost_on_mouseleave(%d,%d);",pos.x,pos.y);
	//frame->ExecuteJavaScript(lpszCommand,"",0);

	//frame->ExecuteJavaScript("alert('You pressed the space bar!');","",0);
	//MessageBoxW(NULL,node->GetName().c_str(),L"",MB_OK);

	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnFocusedNodeChanged(this, browser, frame, node);

	//if (node->IsEditable())
	//{
	// // TEXTAREA
	// //CefDOMNode::AttributeMap pMap;
	// //node->GetElementAttributes(pMap);
	// //std::map<CefString, CefString>::iterator pIter = pMap.begin();
	// //for (; pIter!=pMap.end(); pIter++)
	// //{
	// // CefString sName = pIter->first;
	// // CefString sValue = pIter->second;
	// // MessageBoxW(NULL,sName.c_str(),sValue.c_str(),MB_OK);
	// //}
	// //const CefString sOffsetLeft = node->GetElementAttribute("offsetLeft");
	// //MessageBoxW(NULL,node->GetName().c_str(),sOffsetLeft.c_str(),MB_OK);
	//}
}

bool ClientApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  DCHECK_EQ(source_process, PID_BROWSER);

  bool handled = false;

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !handled; ++it) {
    handled = (*it)->OnProcessMessageReceived(this, browser, source_process,
                                              message);
  }

  return handled;
}
