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

#ifdef WIN32
#pragma warning(disable:4267 4819 4996)
#endif // WIN32

#include "CgcTcpClient.h"
//#include <boost/format.hpp>

namespace mycp {

CgcTcpClient::CgcTcpClient(void)
: CgcBaseClient(_T("TCP"))
, m_connectReturned(false)
, m_bDisconnect(true)
{
}

CgcTcpClient::~CgcTcpClient(void)
{
	StopClient();
}

int CgcTcpClient::startClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize)
{
	//printf("******** CgcTcpClient::startClient (%s)\n",sCgcServerAddr.c_str());
	if (m_tcpClient.get() != 0) return 0;

	std::vector<std::string> pList;
	if (CgcBaseClient::ParseString(sCgcServerAddr.c_str(),":",pList)!=2)
	{
		return -1;
	}
	std::string sIp;
	for (int i=0;i<5;i++)
	{
		sIp = CgcBaseClient::GetHostIp(pList[0].c_str(),"");
		if (!sIp.empty())
			break;
#ifdef WIN32
		Sleep(200);
#else
		usleep(200000);
#endif
	}
	if (sIp.empty())
	{
		//return -1;
		sIp = pList[0];
	}
	//const std::string sIp = CgcBaseClient::GetHostIp(pList[0].c_str());
	const unsigned short nPort = atoi(pList[1].c_str());
	if (nPort==0) return -1;

	//CCgcAddress cgcAddress = CCgcAddress(sCgcServerAddr, CCgcAddress::ST_TCP);
	//tstring sIp = cgcAddress.getip();
	//unsigned short nPort = (unsigned short)cgcAddress.getport();

	if (m_ipService.get() == 0)
		m_ipService = mycp::asio::IoService::create();

	//TcpClient_Handler::pointer clientHandler = boost::enable_shared_from_this<CgcTcpClient>::shared_from_this();
	CgcTcpClient::pointer clientHandler = boost::static_pointer_cast<CgcTcpClient, CgcBaseClient>(boost::enable_shared_from_this<CgcBaseClient>::shared_from_this());
	m_tcpClient = mycp::asio::TcpClient::create(clientHandler);
	//m_tcpClient->socket()->set_option(boost::asio::socket_base::send_buffer_size(64*1024));		// 8192

	m_connectReturned = false;
	// ?? bindPort
	boost::system::error_code ec;
	tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(sIp.c_str(),ec), nPort);
	m_tcpClient->connect(m_ipService->ioservice(), endpoint);
	m_ipService->start();
	
//	for (int i=0; i<150; i++)	// * Max 15S
//	{
//		if (m_connectReturned)
//			return 0;
//#ifdef WIN32
//		Sleep(100);
//#else
//		usleep(100000);
//#endif
//	}
//	return -1;

	while (!m_connectReturned)
#ifdef WIN32
		Sleep(100);
#else
		usleep(100000);
#endif
	return 0;
}

void CgcTcpClient::stopClient(void)
{
	if (m_bExitStopIoService && m_ipService.get() != 0)
	{
		m_ipService->stop();
	}
	if (m_tcpClient.get() != 0)
	{
		m_tcpClient->disconnect();
	}
	m_tcpClient.reset();
	m_ipService.reset();
}

size_t CgcTcpClient::sendData(const unsigned char * data, size_t size)
{
	if (data == 0 || isInvalidate()) return 0;
	if (m_bDisconnect) return 0;

	m_tSendRecv = time(0);
	m_tcpClient->write(data, size);
	return 0;
}

bool CgcTcpClient::isInvalidate(void) const
{
	return m_tcpClient.get() == 0 || !m_tcpClient->is_open();
}

void CgcTcpClient::OnConnected(const mycp::asio::TcpClientPointer& tcpClient)
{
	BOOST_ASSERT (tcpClient.get() != 0);
	m_connectReturned = true;
	m_bDisconnect = false;
	if (tcpClient->is_open())
	{
#ifdef USES_OPENSSL
		// ??
		const tcp::endpoint local_endpoint = tcpClient->socket()->get_socket()->local_endpoint();
		const tcp::endpoint remote_endpoint = tcpClient->socket()->get_socket()->remote_endpoint();
#else
		const tcp::endpoint local_endpoint = tcpClient->socket()->local_endpoint();
		const tcp::endpoint remote_endpoint = tcpClient->socket()->remote_endpoint();
#endif
		m_ipLocal = CCgcAddress(local_endpoint.address().to_string(), local_endpoint.port(), CCgcAddress::ST_TCP);
		m_ipRemote = CCgcAddress(remote_endpoint.address().to_string(), remote_endpoint.port(), CCgcAddress::ST_TCP);
	}
}

void CgcTcpClient::OnReceiveData(const mycp::asio::TcpClientPointer& tcpClient, const mycp::asio::ReceiveBuffer::pointer& data)
{
	BOOST_ASSERT (tcpClient.get() != 0);
	BOOST_ASSERT (data.get() != 0);

	if (data->size() <= 0) return;
	m_tSendRecv = time(0);
	this->parseData(data->data(), data->size(),0);
	//this->parseData(CCgcData::create(data->data(), data->size()),0);
	////this->parseData(CCgcData::create(data->data(), data->size()),(unsigned long)tcpClient.get());
}

} // namespace cgc
