/*
    MYCP is a HTTP and C++ Web Application Server.
    Copyright (C) 2009-2010  Akee Yang <akee.yang@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// cgcclassinclude.h file here
#ifndef __cgcclassinclude_h__
#define __cgcclassinclude_h__

#include "SotpCallTable2.h"
#include "PpSotp2.h"
#include "PpHttp.h"


#ifdef WIN32

#ifdef _DLL
#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCClass60d.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCClass70d.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCClass71d.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCClass80d.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCClass90d.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCClass60.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCClass70.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCClass71.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCClass80.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCClass90.lib")
#endif

#endif // _DEBUG

#else

#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCClass60sd.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCClass70sd.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCClass71sd.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCClass80sd.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCClass90sd.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCClass60s.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCClass70s.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCClass71s.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCClass80s.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCClass90s.lib")
#endif

#endif // _DEBUG
#endif

#endif // WIN32

#endif // __cgcclassinclude_h__
