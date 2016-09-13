/*
    CGCP is a C++ General Communication Platform software library.
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

// cgcSip.h file here
#ifndef __cgcsip_head__
#define __cgcsip_head__

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcService.h>

//#define USES_SIP		1		// [0,1]

#if (USES_SIP)
#include <ThirdParty/libSIP/libsiphandler.h>
#include <ThirdParty/libSIP/SipParameter.h>

namespace cgc{

class cgcSip
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcSip> pointer;

	virtual DoSipHandler::pointer initSip(const CSipParameter & sipp, OnSipHandler * handler) = 0;
	virtual void quitSip(DoSipHandler::pointer sipHandler) = 0;

};

#define CGC_SIPSERVICE_DEF(sip) boost::static_pointer_cast<cgcSip, cgcServiceInterface>(sip)

} // cgc namespace

#endif // USES_SIP

#endif // __cgcsip_head__
