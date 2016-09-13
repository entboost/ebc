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

// tchar.h file here
//#pragma once
#ifndef __tchar_h__
#define __tchar_h__

#ifndef TCHAR_DEFINED
#define TCHAR_DEFINED
#ifdef _UNICODE
typedef wchar_t TCHAR;

// String funcions
#define _tprintf        wprintf
#define _stprintf       _swprintf
#define _stprintf_s     swprintf_s
#define _tcsrchr        wcsrchr

// Formatted I/O
#define _tcstoul    wcstoul
#define _vsntprintf     _vsnwprintf
#define _vsntprintf_s   _vsnwprintf_s
#define _ttoi	_wtoi

#else
#ifndef WIN32
typedef char TCHAR;
#endif // WIN32

// String funcions
#define _tprintf        printf
#define _stprintf       sprintf
#define _stprintf_s     sprintf_s
#define _tcsrchr        _mbsrchr

// Formatted I/O
#define _tcstoul    strtoul
#define _vsntprintf     _vsnprintf
#define _vsntprintf_s   _vsnprintf_s
#define _ttoi	atoi


#endif // _UNICODE
#endif // TCHAR_DEFINED

#endif // __tchar_h__
