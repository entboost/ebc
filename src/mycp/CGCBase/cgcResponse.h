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

// cgcResponse.h file here
#ifndef __cgcresponse_head__
#define __cgcresponse_head__

#include <boost/shared_ptr.hpp>
#include "cgcSession.h"

namespace mycp {

// cgcResponse
class cgcResponse
{
public:
	typedef boost::shared_ptr<cgcResponse> pointer;

	virtual unsigned long getRemoteId(void) const = 0;
	virtual void invalidate(void) = 0;
	virtual bool isInvalidate(void) const = 0;

	virtual int sendResponse(const char * pResponseData, size_t nResponseSize) = 0;
	virtual unsigned long setNotResponse(int nHoldSecond = 0) = 0;
	//virtual unsigned long holdResponse(int nHoldSecond = 60) = 0;
        virtual bool setConfigValue(int /*nConfigItem*/, unsigned int /*nConfigValue*/) {return false;}
        virtual bool getConfigValue(int /*nConfigItem*/, unsigned int* /*pOutConfigValue*/) const {return false;}
};

const cgcResponse::pointer cgcNullResponse;

} // namespace mycp

#endif // __cgcresponse_head__
