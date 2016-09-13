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

// cgcHttpRequest.h file here
#ifndef __cgchttprequest_head__
#define __cgchttprequest_head__

#include "cgcdef.h"
#include "cgcRequest.h"
#include "cgcuploadfile.h"

namespace mycp {

// cgcHttpRequest
class cgcHttpRequest
	: public cgcRequest
{
public:
	typedef boost::shared_ptr<cgcHttpRequest> pointer;

	virtual const tstring& getHost(void) const = 0;
	virtual const tstring& getModuleName(void) const = 0;
	virtual const tstring& getFunctionName(void) const = 0;
	virtual HTTP_METHOD getHttpMethod(void) const = 0;
	virtual const tstring& getHttpVersion(void) const = 0;
	virtual const tstring& getRestVersion(void) const = 0;

	virtual size_t getContentLength(void) const = 0;
	virtual const tstring& getContentType(void) const = 0;
	virtual const tstring& getRequestURL(void) const = 0;	// "/path/file.csp?p=v&..."
	virtual const tstring& getRequestURI(void) const = 0;	// "/path/file.csp"
	virtual const tstring& getQueryString(void) const = 0;	// "p=v&..."
	virtual const tstring& getFileName(void) const = 0;
	virtual unsigned int getRangeFrom(void) const = 0;				// default:0=start
	virtual unsigned int getRangeTo(void) const = 0;					// default:0=end

	virtual bool isKeepAlive(void) const = 0;
	//virtual size_t getContentLength(void) const = 0;
	//virtual const char* getContentData(void) const = 0;
	virtual bool getUploadFile(std::vector<cgcUploadFile::pointer>& outFils) const = 0;

	virtual bool isFrowardFrom(void) const = 0;
	virtual const tstring& getForwardFromURL(void) const = 0;

	virtual cgcValueInfo::pointer getParameter(const tstring & paramName) const = 0;
	//virtual bool getParameter(const tstring & paramName, std::vector<cgcValueInfo::pointer>& outParameters) const = 0;
	virtual bool getParameters(std::vector<cgcKeyValue::pointer>& outParameters) const = 0;
	virtual tstring getParameterValue(const tstring & sParamName, const char * sDefault = "") const =0;
	virtual tstring getParameterValue(const tstring & sParamName, const tstring & sDefault) const =0;
	virtual int getParameterValue(const tstring & sParamName, int nDefault) const =0;
	virtual bigint getParameterValue(const tstring & sParamName, bigint tDefault) const =0;
	virtual bool getParameterValue(const tstring & sParamName, bool bDefault) const =0;
	virtual double getParameterValue(const tstring & sParamName, double fDefault) const =0;

	virtual tstring getHeader(const tstring & headerName, const tstring & sDefault = _T("")) const = 0;
	virtual bool getHeaders(std::vector<cgcKeyValue::pointer>& outHeaders) const = 0;

	virtual tstring getCookie(const tstring & name, const tstring & sDefault = _T("")) const = 0;
	virtual bool getCookies(std::vector<cgcKeyValue::pointer>& outCookies) const = 0;
	virtual cgcAttributes::pointer getPageAttributes(void) const = 0;
	virtual void setPageAttributes(const cgcAttributes::pointer& pPageAttributes) = 0;

};

} // namespace mycp

#endif // __cgchttprequest_head__
