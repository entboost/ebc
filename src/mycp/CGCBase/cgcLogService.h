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

// cgcLogService.h file here
#ifndef __cgclogservice_head__
#define __cgclogservice_head__

#include <boost/shared_ptr.hpp>
#include "cgcdef.h"
#include "cgcServices.h"

namespace mycp {

class cgcLogService
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcLogService> pointer;

	virtual void log(LogLevel level, const char * format,...) = 0;
	virtual void log(LogLevel level, const wchar_t * format,...) = 0;
	virtual void log2(LogLevel level, const char * format) = 0;
	virtual void log2(LogLevel level, const wchar_t * format) = 0;
	virtual bool isLogLevel(LogLevel level) const = 0;
};

const cgcLogService::pointer cgcNullLogService;

#define CGC_LOGSERVICE_DEF(s) boost::static_pointer_cast<cgcLogService, cgcServiceInterface>(s)

} // namespace mycp

#endif // __cgclogservice_head__
