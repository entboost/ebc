// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once
#define _BIND_TO_CURRENT_MFC_VERSION 1

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

#include "../include/SqliteExt.h"
#include "bodb2.h"

#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <comdefsp.h>
#include <atlimage.h>

#include "stl/lockmap.h"
#include "stl/locklist.h"

#include "Core/ProgressST.h"
#include "Core/SplitterControl.h"
#include "Core/ImageEx.h"
#include "Core/MenuXP.h"
#include "Core/OleRichEditCtrl.h"
#include "Core/TraButton.h"
//#include "Core/MrFrame.h"
#include "Core/EbTranEdit.h"
#include "Core/HoverEdit.h"
#include "Core/VividTree.h"
#include "Core/SkinMemDC.h"
#include "Core/colorconver.h"
#include "Core/BitComboBox.h"
#include "Core/LabelEx.h"
#include "../include/ebc_public.h"
#include "../include/EbDialogBase.h"

#include <atlcom.h>
#import "..\build\ebvideoroom.dll" no_namespace
#include "EBMMRoomEventsSink.h"
#include "EBVideoDataEventsSink.h"
#ifdef USES_EBCOM_TEST
#import "..\build\ebcore.dll"  no_namespace	// for ebclient&ebrd
#include "../include/eb_object.h"
#include "../include/chatroomhandle.h"
#include "../include/IEB_MemberInfo.h"
#include "../include/IEB_ChatRichInfo.h"
#include "../include/IEB_ChatFileInfo.h"
#include "../include/IEB_ContactInfo.h"
#include "../include/IEB_EnterpriseInfo.h"
#include "../include/IEB_GroupInfo.h"
#include "../include/EBCoreEventsSink.h"
#include "EBSearchEventsSink.h"
#include "atlsafe.h"
#else
#include "../include/EBParseSetting.h"
#include "EBAppClient.h"
//#include "videoroom.h"
#endif

#import "..\build\ebcontrol.dll" no_namespace
#import "..\build\eboleimage.dll" no_namespace
#include "EBRichMessageEventsSink.h"
