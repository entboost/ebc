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

// cgcString.h file here
#ifndef __cgcstring_head__
#define __cgcstring_head__

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcServices.h>

namespace cgc{

const int MAX_STRING_FORMAT_SIZE = 5*1024;

class cgcString
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcString> pointer;

	virtual std::string W2Char(const wchar_t * strSource) const = 0;
	virtual std::wstring Char2W(const char * strSource) const = 0;

#ifdef WIN32
	// CP_ACP CP_UTF8 ...
	virtual std::string convert(const char * strSource, int sourceCodepage, int targetCodepage) const = 0;
#endif

	virtual std::string format(const char * format,...) const = 0; // MAX_FORMAT_SIZE
	virtual const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace) const = 0;
	virtual const tstring & toUpper(tstring & strSource) const = 0;
	virtual const tstring & toLower(tstring & strSource) const = 0;

	virtual int toInt(const char * strValue, int defaultValue) const = 0;
	virtual long toLong(const char * strValue, long defaultValue) const = 0;
	virtual unsigned long toULong(const char * strValue, unsigned long defaultValue) const = 0;
	virtual double toDouble(const char * strValue, double defaultValue) const = 0;

};

#define CGC_STRINGSERVICE_DEF(s) boost::static_pointer_cast<cgcString, cgcServiceInterface>(s)

} // cgc namespace

#endif // __cgcstring_head__
