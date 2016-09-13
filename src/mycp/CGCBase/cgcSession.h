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

// cgcSession.h file here
#ifndef __cgcsession_head__
#define __cgcsession_head__

#include <boost/enable_shared_from_this.hpp>
#include "cgcdef.h"
#include "cgcAttributes.h"
#include "cgcResponse.h"

namespace mycp {

const int DEFAULT_MAX_INACTIVE_INTERVAL = 10;	//10*60;

class cgcSession
	//: public cgcAttributes
	: public boost::enable_shared_from_this<cgcSession>
{
public:
	typedef boost::shared_ptr<cgcSession> pointer;

	//////////////////////////////////////////
	//	session info
	//////////////////////////////////////////
	virtual int getProtocol(void) const = 0;

	virtual bool isNewSession(void) const = 0;
	virtual time_t getCreationTime(void) const = 0;
	virtual time_t getLastAccessedtime(void) const = 0;
	virtual void invalidate(void) = 0;
	virtual bool isInvalidate(void) const = 0;
	virtual const tstring & getId(void) const = 0;
	virtual unsigned long getRemoteId(void) const = 0;

	virtual void setMaxInactiveInterval(int interval) = 0;

	// Default DEFAULT_MAX_INACTIVE_INTERVAL Seconds
	virtual int getMaxInactiveInterval(void) const = 0;

	//// Return the open session account and password.
	//virtual const tstring & getAccount(void) const =0;
	//virtual const tstring & getSecure(void) const =0;

	virtual cgcAttributes::pointer getAttributes(bool create) = 0;
	virtual cgcAttributes::pointer getAttributes(void) const = 0;

	// Get the cgcResponse::pointer to send msg.
	virtual cgcResponse::pointer getLastResponse(const tstring& moduleName) const = 0;
	virtual cgcResponse::pointer getHoldResponse(unsigned long rsemoteId) = 0;
};
const cgcSession::pointer cgcNullSession;

} // namespace mycp

#endif // __cgcsession_head__
