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

// cgcHttpResponse.h file here
#ifndef __cgchttpresponse_head__
#define __cgchttpresponse_head__

#include "cgcdef.h"
#include "cgcCookieInfo.h"
#include "cgcResponse.h"

namespace mycp {

const int MAX_FORMAT_SIZE = 2*1024;

class cgcHttpResponse
	: public cgcResponse
{
public:
	typedef boost::shared_ptr<cgcHttpResponse> pointer;

	virtual void println(const tstring& text) = 0;
	virtual void println(const char * format,...) = 0;
	virtual void write(const tstring& text) = 0;
	virtual void write(const char * format,...) = 0;
	virtual void newline(void) = 0;
	virtual void reset(void) = 0;
	virtual void writeData(const char * buffer, size_t size) = 0;

	virtual void setCookie(const tstring& name, const tstring& value) = 0;
	virtual void setCookie(const cgcCookieInfo::pointer& pCookieInfo) = 0;
	virtual void setHeader(const tstring& name, const tstring& value) = 0;

	virtual void addDateHeader(void) = 0;
	virtual size_t getBodySize(void) const = 0;

	virtual void setStatusCode(HTTP_STATUSCODE statusCode) = 0;
	virtual HTTP_STATUSCODE getStatusCode(void) const = 0;
	virtual void setKeepAlive(int nKeepAliveInterval) = 0;	// -1:Connection: close

	virtual void setContentType(const tstring& contentType = "text/html") = 0;
	virtual const tstring& getContentType(void) const = 0;

	virtual void forward(const tstring& url) = 0;
	virtual void location(const tstring& url) = 0;

	virtual void lockResponse(void) = 0;
	// Return : >= 0,  succeeded
	virtual int sendResponse(HTTP_STATUSCODE statusCode = STATUS_CODE_200) = 0;
};

const cgcHttpResponse::pointer cgcNullHttpResponse;

} // namespace mycp

#endif // __cgchttpresponse_head__
