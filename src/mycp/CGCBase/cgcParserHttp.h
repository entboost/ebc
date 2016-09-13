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

// cgcParserHttp.h file here
#ifndef _cgcparserhttp_head_included__
#define _cgcparserhttp_head_included__

#include <vector>
#include "cgcdef.h"
#include "cgcmultipart.h"
#include "cgcParserBase.h"
#include "cgcCookieInfo.h"

namespace mycp {

class cgcParserHttp
	: public cgcParserBase
{
public:
	typedef boost::shared_ptr<cgcParserHttp> pointer;

	//////////////////////////////////////////
	// Request
	virtual const tstring& getHost(void) const = 0;
	virtual const tstring& getHttpVersion(void) const = 0;
	virtual const tstring& getRestVersion(void) const = 0;	// v01,...

	virtual HTTP_METHOD getHttpMethod(void) const = 0;
	virtual bool isServletRequest(void) const = 0;

	//virtual size_t getContentLength(void) const = 0;
	virtual const tstring& getRequestURL(void) const = 0;	// "/path/file.csp?p=v&..."
	virtual const tstring& getRequestURI(void) const = 0;	// "/path/file.csp"
	virtual const tstring& getQueryString(void) const = 0;	// "p=v&..."
	virtual const tstring& getFileName(void) const = 0;
	virtual unsigned int getRangeFrom(void) const = 0;				// default:0=start
	virtual unsigned int getRangeTo(void) const = 0;					// default:0=end

	virtual bool isKeepAlive(void) const = 0;
	virtual int getKeepAlive(void) const = 0;
	virtual void setKeepAlive(int nKeepAliveInterval) = 0;	// -1:Connection: close
	virtual size_t getContentLength(void) const = 0;
	virtual const char* getContentData(void) const = 0;
	virtual cgcMultiPart::pointer getMultiPart(void) const = 0;
	virtual bool getUploadFile(std::vector<cgcUploadFile::pointer>& outFiles) const = 0;

	// Parameter
	virtual cgcValueInfo::pointer getParameter(const tstring & paramName) const = 0;
	//virtual bool getParameter(const tstring & paramName, std::vector<cgcValueInfo::pointer>& outParameters) const = 0;
	virtual bool getParameters(std::vector<cgcKeyValue::pointer>& outParameters) const = 0;

	// Herder
	virtual tstring getHeader(const tstring & header, const tstring& defaultValue) const = 0;
	virtual bool getHeaders(std::vector<cgcKeyValue::pointer>& outHeaders) const = 0;

	// Cookie
	virtual bool isEmptyCookieMySessionId(void) const = 0;
	virtual tstring getCookieMySessionId(void) const = 0;
	virtual tstring getCookie(const tstring & name, const tstring& defaultValue) const = 0;
	virtual bool getCookies(std::vector<cgcKeyValue::pointer>& outCookies) const = 0;

	//////////////////////////////////////////////////
	// Response
	virtual void println(const char * text, size_t size) = 0;
	virtual void println(const tstring& text) = 0;
	virtual void write(const char * text, size_t size) = 0;
	virtual void write(const tstring& text) = 0;
	virtual void newline(void) = 0;
	virtual void reset(void) = 0;

	virtual const char * getHttpResult(size_t& outSize) const = 0;

	///////////////////////////////////////////////////////
	// Other
	virtual void setStatusCode(HTTP_STATUSCODE statusCode = STATUS_CODE_200) = 0;
	virtual HTTP_STATUSCODE getStatusCode(void) const = 0;

	virtual void addDateHeader(bool v=true) = 0;
	virtual void addContentLength(bool v=true) = 0;
	virtual size_t getBodySize(void) const = 0;
	virtual const tstring& getForwardFromURL(void) const = 0;

	virtual void setCookieMySessionId(const tstring& sMySessionId) = 0;
	//virtual void setCookieExpiresMinute(unsigned int nExpiresMinute = 10) = 0;
	virtual void setCookie(const tstring& name, const tstring& value) = 0;
	virtual void setCookie(const cgcCookieInfo::pointer& pCookieInfo) = 0;
	virtual void setHeader(const tstring& name, const tstring& value) = 0;
	virtual void setContentType(const tstring& contentType = "text/html") = 0;
	virtual const tstring& getContentType(void) const = 0;
	virtual const tstring& getReqContentType(void) const = 0;

	virtual void forward(const tstring& url) = 0;
	virtual void location(const tstring& url) = 0;
};

const cgcParserHttp::pointer cgcNullParserHttpService;

#define CGC_PARSERHTTPSERVICE_DEF(s) boost::static_pointer_cast<cgcParserHttp, cgcServiceInterface>(s)

} // namespace mycp

#endif // _cgcparserhttp_head_included__
