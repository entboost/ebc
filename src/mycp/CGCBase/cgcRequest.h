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

// cgcRequest.h file here
#ifndef __cgcrequest_head__
#define __cgcrequest_head__

#include <boost/shared_ptr.hpp>
#include "cgcSession.h"

namespace mycp {

// cgcRequest
class cgcRequest
{
public:
	typedef boost::shared_ptr<cgcRequest> pointer;

	virtual void setProperty(const tstring & propertyName, const tstring & propertyValue, bool clear = true) = 0;
	virtual void setProperty(const tstring & propertyName, int propertyValue, bool clear = true) = 0;
	virtual void setProperty(const tstring & propertyName, double propertyValue, bool clear = true) = 0;
	virtual cgcValueInfo::pointer getProperty(const tstring & propertyName) const = 0;
	virtual cgcAttributes::pointer getAttributes(bool create) = 0;
	virtual cgcAttributes::pointer getAttributes(void) const = 0;
	virtual cgcSession::pointer getSession(void) const = 0;

	virtual const char * getContentData(void) const =0;
	virtual size_t getContentLength(void) const =0;
	virtual bool lockSessionApi(int nLockApi, int nTimeoutSeconds=0, bool bUnlockForce=false) =0;
	virtual void unlockSessionApi(void) =0;
	
	// TCP,UDP,RTP,...
	virtual tstring getScheme(void) const = 0;

	virtual int getProtocol(void) const = 0;
	virtual int getServerPort(void) const = 0;
	virtual const tstring & getServerAddr(void) const = 0;	// IP format.

	virtual unsigned long getRemoteId(void) const = 0;
	virtual unsigned long getIpAddress(void) const = 0;		// host byte order
	virtual const tstring & getRemoteAddr(void) const = 0;	// IP:PORT format.
	virtual const tstring & getRemoteHost(void) const {return getRemoteAddr();}
	virtual tstring getRemoteAccount(void) const {return "";}
	virtual tstring getRemoteSecure(void) const {return "";}
};

} // namespace mycp

#endif // __cgcrequest_head__
