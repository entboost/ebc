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

// dlldefine.h file here

#define libSOTPCLIENT_STATIC 1	// 0,1

#ifndef LIBSOTPCLIENT_CLASS
#if libSOTPCLIENT_STATIC
#define LIBSOTPCLIENT_CLASS 
#else
#ifdef WIN32
#ifdef libSOTPCLIENT_EXPORTS
#define LIBSOTPCLIENT_CLASS __declspec(dllexport)
#else
#define LIBSOTPCLIENT_CLASS __declspec(dllimport)
#endif // libDS_EXPORTS
#else
#define LIBSOTPCLIENT_CLASS 
#endif // WIN32
#endif // libSOTPCLIENT_STATIC
#endif // LIBSOTPCLIENT_CLASS

//#ifdef WIN32
//#ifndef CGCLIB_CLASS
//#ifdef CGCLIB_EXPORTS
//#define CGCLIB_CLASS __declspec(dllexport)
//#define CGCLIB_CLASS 
//#else
//#define CGCLIB_CLASS __declspec(dllimport)
//#define CGCLIB_CLASS 
//#endif
//#endif
//#else
//#define CGCLIB_CLASS 
//#endif

#define CGCLIB_CLASS 
