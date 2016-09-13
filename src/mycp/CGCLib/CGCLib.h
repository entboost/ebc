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

// CGCLib.h file here
#ifndef __CGCLib_h__
#define __CGCLib_h__

#include "IncludeBase.h"
#include "SotpClient.h"

#ifdef WIN32
#ifdef _DLL

#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCLib60d.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCLib70d.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCLib71d.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCLib80d.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCLib90d.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCLib60.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCLib70.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCLib71.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCLib80.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCLib90.lib")
#endif

#endif // _DEBUG

#else	// _DLL

#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCLib60sd.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCLib70sd.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCLib71sd.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCLib80sd.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCLib90sd.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "CGCLib60s.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "CGCLib70s.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "CGCLib71s.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "CGCLib80s.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "CGCLib90s.lib")
#endif

#endif // _DEBUG

#endif // _DLL

#endif // WIN32


#endif // __CGCLib_h__

