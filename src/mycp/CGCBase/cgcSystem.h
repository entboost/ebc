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

// cgcSystem.h file here
#ifndef __cgcsystem_head__
#define __cgcsystem_head__

#include <boost/shared_ptr.hpp>
#include "cgcparameter.h"
#include "cgccdbcs.h"
#include "cgcuses.h"
#include "cgcdlldefine.h"
#include "cgcSession.h"
#include "cgcAttributes.h"
#include "cgcCDBCService.h"

namespace mycp {

// cgcSystem
class cgcSystem
	//: public cgcAttributes
{
public:
	typedef boost::shared_ptr<cgcSystem> pointer;

	// Setting in [CGCP_PATH]/conf/params.xml file.
	virtual cgcParameterMap::pointer getInitParameters(void) const = 0;
	//virtual cgcCDBCInfo::pointer getCDBCInfo(const tstring& name) const = 0;

	// Return [CGCP_PATH] value.
	virtual const tstring & getServerPath(void) const = 0;

	// Setting in [CGCP_PATH]/default.xml file.
	virtual const tstring & getServerName(void) const = 0;
	virtual const tstring & getServerAddr(void) const = 0;
	virtual const tstring & getServerCode(void) const = 0;
	virtual int getServerRank(void) const = 0;

	virtual cgcSession::pointer getSession(const tstring& sessionId) const = 0;
	virtual cgcResponse::pointer getLastResponse(const tstring& sessionId,const tstring& moduleName) const = 0;
	virtual cgcResponse::pointer getHoldResponse(const tstring& sessionId,unsigned long remoteId) = 0;

	virtual cgcAttributes::pointer getAttributes(bool create) = 0;
	virtual cgcAttributes::pointer getAttributes(void) const = 0;
	virtual cgcAttributes::pointer getAppAttributes(const tstring & appName) const = 0;

};
#define CGC_GET_LAST_RESPONSE(sessionid) (theSystem->getLastResponse(sessionid,theApplication->getApplicationName()))
#define CGC_GET_HOLD_RESPONSE(sessionid,remoteid) (theSystem->getHoldResponse(sessionid,remoteid))

} // namespace mycp

#endif // __cgcsystem_head__
