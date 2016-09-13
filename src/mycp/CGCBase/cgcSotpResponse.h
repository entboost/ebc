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

// cgcSotpResponse.h file here
#ifndef __cgcsotpresponse_head__
#define __cgcsotpresponse_head__

#include <vector>
#include "cgcparameter.h"
#include "cgcResponse.h"

namespace mycp {

// cgcSotpResponse
class cgcSotpResponse
	: public cgcResponse
{
public:
	typedef boost::shared_ptr<cgcSotpResponse> pointer;

	virtual void setAttachName(const tstring & name) = 0;
	virtual void setAttachInfo(mycp::bigint total, mycp::bigint index) = 0;
	virtual void setAttachData(const unsigned char * attachData, unsigned int attachSize) = 0;
	virtual void setAttach(const cgcAttachment::pointer& pAttach) = 0;

	virtual void setParameter(const cgcParameter::pointer& parameter, bool bSetForce=false) = 0;
	virtual void addParameter(const cgcParameter::pointer& parameter, bool bAddForce=false) = 0;
	virtual void addParameters(const std::vector<cgcParameter::pointer>& parameters, bool bAddForce=false) = 0;
	virtual void delParameter(const tstring& paramName) = 0;
	virtual void clearParameter(void) = 0;
	virtual size_t getParameterCount(void) const = 0;

	virtual void lockResponse(void) = 0;
	// Return : >= 0,  succeeded
	virtual int sendResponse(long retCode = 0, unsigned long sign = 0, bool bNeedAck = true) = 0;

	// Reserve
	virtual void setEncoding(const tstring & newValue = _T("GBK")) = 0;
};

const cgcSotpResponse::pointer cgcNullSotpResponse;


#define CGC_RES_PARAMETER(P) response->setParameter(P)
#define CGC_RES_LOCK() response->lockResponse()
#define CGC_RES_RESPONSE(C, S) response->sendResponse(C, S)

#define CGC_SOTPRESPONSE_CAST(r) boost::shared_ptr<cgcSotpResponse>(boost::static_pointer_cast<cgcSotpResponse, cgcResponse>(r))

//boost::shared_ptr<cgcSotpResponse> CGC_SOTPRESPONSE_CAST(boost::shared_ptr<cgcResponse> const & r)
//{
//	return boost::static_pointer_cast<cgcSotpResponse, cgcResponse>(r);
//}

} // namespace mycp

#endif // __cgcsotpresponse_head__
