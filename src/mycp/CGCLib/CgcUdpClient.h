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

// CgcUdpClient.h file here
#ifndef __CgcUdpClient_h__
#define __CgcUdpClient_h__

//
// include
#include "CgcBaseClient.h"

namespace mycp {

class CGCLIB_CLASS CgcUdpClient
	: public CgcBaseClient
	, public mycp::asio::UdpSocket_Handler
	//, public boost::enable_shared_from_this<CgcUdpClient>
{
public:
	typedef boost::shared_ptr<CgcUdpClient> pointer;

	static CgcUdpClient::pointer create(void)
	{
		return CgcUdpClient::pointer(new CgcUdpClient());
	}

	CgcUdpClient(void);
	virtual ~CgcUdpClient(void);

private:
	virtual int startClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize);
	virtual void stopClient(void);
	virtual size_t recvData(unsigned char * buffer, size_t size) {return 0;}
	virtual size_t sendData(const unsigned char * data, size_t size);
	virtual bool isInvalidate(void) const;
	virtual bool setRemoteAddr(const tstring & sRemoteAddr);

	virtual bool doSetConfig(int nConfig, unsigned int nInValue);
	virtual void doGetConfig(int nConfig, unsigned int* nOutValue) const;

	// UdpSocket_Handler
	virtual void OnReceiveData(const mycp::asio::UdpSocket & pUdpSocket, const mycp::asio::UdpEndPoint::pointer& endpoint);

private:
	//IoService::pointer m_ipService;
	mycp::asio::UdpSocket::pointer m_udpClient;
	udp::endpoint m_endpointLocal;
	udp::endpoint m_endpointRemote;
	int m_nIoSendBufferSize;
	int m_nIoReceiveBufferSize;

};

} // namespace cgc
#endif // __CgcUdpClient_h__
