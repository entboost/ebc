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

// cgcServices.h file here
#ifndef __cgcservices_head__
#define __cgcservices_head__

#include "cgcService.h"
#include "cgcHttpRequest.h"
#include "cgcHttpResponse.h"
#include "cgccdbcs.h"

namespace mycp {

class cgcCDBCService;
typedef boost::shared_ptr<cgcCDBCService> cgcCDBCServicePointer;

// cgcServiceManager
class cgcServiceManager
{
public:
	typedef boost::shared_ptr<cgcServiceManager> pointer;

	virtual cgcCDBCInfo::pointer getCDBDInfo(const tstring& datasource) const = 0;
	virtual cgcCDBCServicePointer getCDBDService(const tstring& datasource) = 0;
	virtual void retCDBDService(cgcCDBCServicePointer& cdbcservice) = 0;

	virtual cgcServiceInterface::pointer getService(const tstring & serviceName, const cgcValueInfo::pointer& parameter = cgcNullValueInfo) = 0;
	virtual void resetService(const cgcServiceInterface::pointer & service) = 0;

	virtual HTTP_STATUSCODE executeInclude(const tstring & url, const cgcHttpRequest::pointer & request, const cgcHttpResponse::pointer& response) = 0;
	virtual HTTP_STATUSCODE executeService(const tstring & serviceName, const tstring& function, const cgcHttpRequest::pointer & request, const cgcHttpResponse::pointer& response, tstring & outExecuteResult) = 0;

};

} // namespace mycp

#endif // __cgcservices_head__
