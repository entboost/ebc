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

// cgcCookieInfo.h file here
#ifndef __cgcCookieInfo_head__
#define __cgcCookieInfo_head__

#pragma warning(disable:4996)

#include <vector>
#include <string.h>
#include "cgcobject.h"

namespace mycp {

class cgcCookieInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<cgcCookieInfo> pointer;
	static cgcCookieInfo::pointer create(void) {return cgcCookieInfo::pointer(new cgcCookieInfo());}
	static cgcCookieInfo::pointer create(const tstring& sName,const tstring& sValue,const tstring& sPath="/",time_t tExpiresTime=0)
	{return cgcCookieInfo::pointer(new cgcCookieInfo(sName,sValue,sPath,tExpiresTime));}

	tstring m_sCookieName;
	tstring m_sCookieValue;
	tstring m_sCookiePath;
	time_t m_tExpiresTime;
	cgcCookieInfo(const tstring& sName,const tstring& sValue,const tstring& sPath,time_t tExpiresTime)
		: m_sCookieName(sName)
		, m_sCookieValue(sValue)
		, m_sCookiePath(sPath)
		, m_tExpiresTime(tExpiresTime)
	{
	}
	cgcCookieInfo(void)
		: m_sCookiePath("/")
		, m_tExpiresTime(0)
	{
	}
};

} // namespace mycp

#endif // __cgcCookieInfo_head__
