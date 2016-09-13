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

// cgcRtp.h file here
#ifndef __cgcrtp_head__
#define __cgcrtp_head__

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcService.h>
#include <CGCBase/cgcuses.h>

#if (USES_RTP)
#include <ThirdParty/libRTP/librtphandler.h>

namespace cgc{

class cgcRtp
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcRtp> pointer;

	virtual DoRtpHandler::pointer startRtp(unsigned short rtpPort) = 0;
	virtual void stopRtp(DoRtpHandler::pointer handler) = 0;

};

#define CGC_RTPSERVICE_DEF(rtp) boost::static_pointer_cast<cgcRtp, cgcServiceInterface>(rtp)

} // cgc namespace

#endif // USES_RTP

#endif // __cgcrtp_head__
