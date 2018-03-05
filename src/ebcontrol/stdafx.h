// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atltypes.h>
#include <string>
#include "SkinMemoryDC.h"

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

extern std::wstring theFontFamily;	// "Times New Roman"
const std::wstring theFontFamily0 = L"Î¢ÈíÑÅºÚ";
const std::wstring theFontFamily1 = L"ËÎÌå";
const int const_font_size = 13;	// 13

using namespace ATL;
