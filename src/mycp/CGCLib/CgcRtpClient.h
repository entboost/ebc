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

// CgcRtpClient.h file here
#ifndef __CgcRtpClient_h__
#define __CgcRtpClient_h__

#include "CgcBaseClient.h"

#if (USES_RTP)
namespace cgc
{

// 
class CGCLIB_CLASS CgcRtpClient
	: public CgcBaseClient
	, public OnRtpHandler
{
public:
	static CgcRtpClient::pointer create(void)
	{
		return CgcRtpClient::pointer(new CgcRtpClient());
	}

	CgcRtpClient(void);
	virtual ~CgcRtpClient(void);

public:
	//void setRemoteAddr(unsigned long destIp, unsigned int destPort);
	//size_t SendData(const unsigned char * data, size_t size) {return sendData(data, size);}

private:
	virtual int startClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize);
	virtual void stopClient(void);
	virtual bool isInvalidate(void) const;
	virtual size_t sendData(const unsigned char * data, size_t size);
	virtual size_t sendData(const unsigned char * data, size_t size, unsigned int timestamp);
	virtual bool setRemoteAddr(const tstring & sRemoteAddr);
	virtual void setMediaType(unsigned short mediatype);

protected:
	// OnRtpHandler handler
	virtual void onReceiveEvent(CRTPData::pointer receiveData, DoRtpHandler::pointer pDoRtpHandler, void * param);
	virtual void onRtpKilledEvent(DoRtpHandler::pointer pDoRtpHandler, void * rtpParam) {}

protected:
	//CRtpProxy m_rtpClient;
	CRtp		m_rtp;
	DoRtpHandler::pointer m_pDoRtpHandler;
};

} // namespace cgc
#endif // USES_RTP
#endif // __CgcRtpClient_h__
