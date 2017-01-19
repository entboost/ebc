
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
//#define _BIND_TO_CURRENT_CRT_VERSION 1
//#define _BIND_TO_CURRENT_MFC_VERSION 1
//#define _BIND_TO_CURRENT_ATL_VERSION 1

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <boost/shared_ptr.hpp>

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

#include "bodb2.h"
#include "Tools/SplitterControl.h"
#include "Tools/NewMenu.h"
//#include "Tools/MenuXP.h"
#include "Tools/TraButton.h"
#include "Tools/SkinMemDC.h"
#include "Tools/LabelEx.h"
#include "Tools/HoverEdit.h"
#include "Tools/HoverStatic.h"
#include "Tools/HoverComboBox.h"
#include "Tools/BitComboBox.h"
#include "Tools/ImageEx.h"
#include "../include/ebc_public.h"
#include "../include/EbDialogBase.h"
#include "../include/colorconver.h"
#include "../include/EbcLanguages.h"
#ifdef USES_EBCOM_TEST
#include <atlcom.h>
#import "..\build\ebcore.dll"  no_namespace
#include "../include/eb_object.h"
#include "../include/chatroomhandle.h"
#include "../include/IEB_MemberInfo.h"
#include "../include/IEB_ChatRichInfo.h"
#include "../include/IEB_ChatFileInfo.h"
#include "../include/IEB_ContactInfo.h"
#include "../include/IEB_EnterpriseInfo.h"
#include "../include/IEB_GroupInfo.h"
#include "../include/EBCoreEventsSink.h"
//#include "EBSearchEventsSink.h"
#include "atlsafe.h"
#else
#include "../include/EBParseSetting.h"
#include <CGCBase/cgcSmartString.h>
#include "EBAppClient.h"
//#include "videoroom.h"
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


