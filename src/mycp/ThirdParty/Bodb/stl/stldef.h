// stldef.h file here
#ifndef __stldef_h__
#define __stldef_h__

#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <fstream>
#ifndef TSTRING_DEFINED
#define TSTRING_DEFINED
#ifdef _UNICODE
typedef std::wstring tstring;
typedef std::wfstream tfstream;
//#ifndef TCHAR
//#define TCHAR wchar_t
//#endif // TCHAR
#else
typedef std::string tstring;
typedef std::fstream tfstream;
//#ifndef TCHAR
//#define TCHAR char
//#endif  // TCHAR
#endif // _UNICODE
#endif // TSTRING_DEFINED

//#ifdef WIN32
//#include <tchar.h>
//#else
//#include "tchar.h"
//#endif // WIN32
//#include <string>
////#include <list>
////#include <map>
//////#include <algorithm>
////

// define
#ifndef FAR
#define FAR
#endif

#define __T(x)      x
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)
//
//// typedef
//typedef unsigned int UINT;
//typedef unsigned long ULONG;
//

//
//
////
//// typedef
//typedef std::list<void*> PtrList;
//typedef	std::list<tstring> TStringList;
//typedef TStringList::iterator TStringListIter;
//typedef TStringList::const_iterator TStringListConstIter;
//
//typedef std::map<ULONG, void*> ULongPtrMap;
//typedef std::pair<ULONG, void*> ULongPtrPair;


// **************** 
// DeletePtr, for for_each()
struct DeletePtr
{
	template<typename T>
	void operator() (const T* ptr) const
	{
		if (ptr)
			delete ptr;
	}
};

// DeletePair, for for_each()
struct DeletePair
{
	template<typename Ty1, typename Ty2>
	void operator() (const std::pair<Ty1, Ty2> &ptr) const
	{
		if (ptr.second)
			delete ptr.second;
	}
};

#endif // __stldef_h__
