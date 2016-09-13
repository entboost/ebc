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

// cgcParserBase.h file here
#ifndef _cgcparserbase_head_included__
#define _cgcparserbase_head_included__

#include <boost/shared_ptr.hpp>
#include "cgcServices.h"

namespace mycp {

class cgcParserCallback
{
public:
	virtual tstring onGetSslPrivateKey(void) const = 0;
	virtual tstring onGetSslPrivatePwd(void) const = 0;
	virtual tstring onGetSslPassword(const tstring& sSessionId) const = 0;
};

class cgcParserBase
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcParserBase> pointer;

	virtual void setParseCallback(cgcParserCallback* pCallback) = 0;

	// UTF8: UTF8->ACP
	virtual bool doParse(const unsigned char * parseData, size_t parseSize,const char* sEncoding="") = 0;

	virtual const tstring& getModuleName(void) const = 0;
	virtual const tstring& getFunctionName(void) const = 0;

	virtual tstring getAccount(void) const {return "";}
	virtual tstring getSecure(void) const {return "";}
	virtual void init(void) = 0;
};

const cgcParserBase::pointer cgcNullParserBaseService;

} // namespace mycp

#endif // _cgcparserbase_head_included__
