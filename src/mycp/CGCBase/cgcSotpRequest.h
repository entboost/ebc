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

// cgcSotpRequest.h file here
#ifndef __cgcSotpRequest_head__
#define __cgcSotpRequest_head__

#include "cgcparameter.h"
#include "cgcattachment.h"
#include "cgcRequest.h"

namespace mycp {

// cgcSotpRequest
class cgcSotpRequest
	: public cgcRequest
{
public:
	typedef boost::shared_ptr<cgcSotpRequest> pointer;

	virtual bool isHasAttachInfo(void) const =0;
	virtual cgcAttachment::pointer getAttachment(void) const =0;

	virtual cgcParameter::pointer getParameter(const tstring & sParamName) const =0;
	virtual tstring getParameterValue(const tstring & sParamName, const char * sDefault = "") const =0;
	virtual tstring getParameterValue(const tstring & sParamName, const tstring & sDefault) const =0;
	virtual int getParameterValue(const tstring & sParamName, int nDefault) const =0;
	virtual bigint getParameterValue(const tstring & sParamName, bigint tDefault) const =0;
	virtual bool getParameterValue(const tstring & sParamName, bool bDefault) const =0;
	virtual double getParameterValue(const tstring & sParamName, double fDefault) const =0;

	virtual size_t getParameterCount(void) const =0;
	virtual const cgcParameterMap & getParameters(void) const =0;
};

#define CGC_REQ_PARAMETER(N) request->getParameter(N)
#define CGC_REQ_VALUE(N, D) request->getParameterValue(N, D)
//#define CGC_REQ_VALUE2(N, D) request->getParameterValue2(N, D)

} // namespace mycp

#endif // __cgcSotpRequest_head__
