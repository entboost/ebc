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

// CgcTcpClient.h file here
#ifndef __entboost_CgcTcpClient_h__
#define __entboost_CgcTcpClient_h__

#ifndef USES_OPENSSL
#define USES_OPENSSL
#endif

#define USES_MYCP_TCPCLIENT
//
// include
#include <CGCBase/cgcaddress.h>
#include <ThirdParty/Boost/asio/IoService.h>
#include <ThirdParty/Boost/asio/boost_socket.h>
#ifdef USES_MYCP_TCPCLIENT
#include <ThirdParty/Boost/asio/TcpClient.h>
using namespace mycp::asio;
#else
#include "TcpClient.h"
#endif

namespace entboost {

class TcpClient_Callback
{
public:
	typedef boost::shared_ptr<TcpClient_Callback> pointer;
	virtual void OnReceiveData(const mycp::asio::ReceiveBuffer::pointer& data) = 0;
};

class CgcTcpClient
	: public TcpClient_Handler
	, public IoService_Handler
	, public boost::enable_shared_from_this<CgcTcpClient>
{
public:
	typedef boost::shared_ptr<CgcTcpClient> pointer;

	static CgcTcpClient::pointer create(TcpClient_Callback* pCallback)
	{
		return CgcTcpClient::pointer(new CgcTcpClient(pCallback));
	}

	CgcTcpClient(void)
		: m_connectReturned(false)
		, m_bDisconnect(true)
		, m_bException(false)
		, m_pCallback(NULL)
		//, m_sReceiveData(_T(""))
	{
	}

	CgcTcpClient(TcpClient_Callback* pCallback)
		: m_connectReturned(false)
		, m_bDisconnect(true)
		, m_bException(false)
		, m_pCallback(pCallback)
		//, m_sReceiveData(_T(""))
	{
	}

	virtual ~CgcTcpClient(void)
	{
		stopClient();
	}
	//static void Test_To_SSL_library_init(void)
	//{
	//	static bool theTestInit = false;
	//	//printf("**** Test2_SSL_library_init = %d\n",(int)(theTestInit?1:0));
	//	if (!theTestInit)
	//	{
	//		theTestInit = true;
	//		SSL_CTX * handle_ = ::SSL_CTX_new(::SSLv2_client_method());	// 返回0
	//		if (handle_==NULL)
	//		{
	//			SSL_library_init();	// *** 初始化SSL环境，解决组件在线更新 context error 异常问题；
	//		}else
	//		{
	//			SSL_CTX_free(handle_);
	//		}
	//	}
	//}

	static mycp::tstring GetHostIp(const char * lpszHostName,const char* lpszDefault)
	{
		try
		{
			struct hostent *host_entry;
			//struct sockaddr_in addr;
			/* 即要解析的域名或主机名 */
			host_entry=gethostbyname(lpszHostName);
			//printf("%s\n", dn_or_ip);
			if(host_entry!=0)
			{
				char lpszIpAddress[50];
				//printf("解析IP地址: ");
				sprintf(lpszIpAddress, "%d.%d.%d.%d",
					(host_entry->h_addr_list[0][0]&0x00ff),
					(host_entry->h_addr_list[0][1]&0x00ff),
					(host_entry->h_addr_list[0][2]&0x00ff),
					(host_entry->h_addr_list[0][3]&0x00ff));
				return lpszIpAddress;
			}
		}catch(std::exception&)
		{
		}catch(...)
		{}
		return lpszDefault;
	}
#ifdef USES_OPENSSL
	virtual int startClient(const mycp::tstring & sCgcServerAddr, unsigned int bindPort,boost::asio::ssl::context* ctx=NULL)
#else
	virtual int startClient(const mycp::tstring & sCgcServerAddr, unsigned int bindPort)
#endif
	{
		if (m_tcpClient.get() != 0) return 0;

		mycp::CCgcAddress cgcAddress = mycp::CCgcAddress(sCgcServerAddr, mycp::CCgcAddress::ST_TCP);
		const unsigned short nPort = (unsigned short)cgcAddress.getport();
		const mycp::tstring sInIp = cgcAddress.getip();
		mycp::tstring sIp;
		for (int i=0;i<5;i++)
		{
			sIp = CgcTcpClient::GetHostIp(sInIp.c_str(),"");
			if (!sIp.empty())
				break;
#ifdef WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
		}
		if (sIp.empty())
			sIp = sInIp;

		if (m_ipService.get() == 0)
			m_ipService = IoService::create();

		TcpClient_Handler::pointer clientHandler = boost::enable_shared_from_this<CgcTcpClient>::shared_from_this();
		//CgcTcpClient::pointer clientHandler = boost::static_pointer_cast<CgcTcpClient, CgcBaseClient>(boost::enable_shared_from_this<CgcBaseClient>::shared_from_this());

		m_tcpClient = TcpClient::create(clientHandler);

		m_connectReturned = false;
		// ?? bindPort
		boost::system::error_code ec;
		tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(sIp.c_str(),ec), nPort);
#ifdef USES_OPENSSL
		m_tcpClient->connect(m_ipService->ioservice(), endpoint,ctx);
#else
		m_tcpClient->connect(m_ipService->ioservice(), endpoint);
#endif
		m_ipService->start(shared_from_this());
		while (!m_connectReturned)
#ifdef WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
		return 0;
	}
	virtual void stopClient(void)
	{
		m_pCallback = NULL;
		if (m_ipService.get() != 0)
		{
			m_ipService->stop();
		}
		if (m_tcpClient.get() != 0)
		{
			//m_tcpClient->resetHandler();
			m_tcpClient->disconnect();
		}
		m_tcpClient.reset();
		m_ipService.reset();
	}
	virtual size_t sendData(const unsigned char * data, size_t size)
	{
		if (IsDisconnection() || IsException() || data == 0 || isInvalidate()) return 0;

		//const size_t s = m_tcpClient->write(data, size);
		//m_tcpClient->async_read_some();
		//return s;
		return m_tcpClient->write(data, size);
		//return m_tcpClient->write(data, size);
	}

	//const mycp::tstring & GetReceiveData(void) const {return m_sReceiveData;}
	//void ClearData(void) {m_sReceiveData.clear();}
	bool IsDisconnection(void) const {return m_bDisconnect;}
	bool IsException(void) const {return m_bException;}

private:
	virtual bool isInvalidate(void) const {return m_tcpClient.get() == 0 || !m_tcpClient->is_open();}

	// IoService_Handler
	virtual void OnIoServiceException(void){m_bException = true;}
	///////////////////////////////////////////////
	// for TcpClient_Handler
	virtual void OnConnected(const TcpClientPointer& tcpClient)
	{
		m_connectReturned = true;
		m_bDisconnect = false;
		//if (tcpClient->is_open())
		//{
		//	tcp::endpoint local_endpoint = tcpClient->socket()->local_endpoint();
		//	tcp::endpoint remote_endpoint = tcpClient->socket()->remote_endpoint();
		//	m_ipLocal = CCgcAddress(local_endpoint.address().to_string(), local_endpoint.port(), CCgcAddress::ST_TCP);
		//	m_ipRemote = CCgcAddress(remote_endpoint.address().to_string(), remote_endpoint.port(), CCgcAddress::ST_TCP);
		//}
	}
	virtual void OnConnectError(const TcpClientPointer& tcpClient, const boost::system::error_code & error){m_connectReturned = true;m_bDisconnect=true;}
	virtual void OnReceiveData(const TcpClientPointer& tcpClient, const mycp::asio::ReceiveBuffer::pointer& data)
	{
		BOOST_ASSERT (tcpClient.get() != 0);
		BOOST_ASSERT (data.get() != 0);

		if (data->size() <= 0) return;
		if (m_pCallback != NULL)
			m_pCallback->OnReceiveData(data);
		//unsigned char lpszBuffer[1024];
		//memcpy(lpszBuffer,data->data(),data->size());
		//m_tSendRecv = time(0);
		//this->parseData(CCgcData::create(data->data(), data->size()));
		//m_sReceiveData.append(mycp::tstring((const char*)data->data(), data->size()));
	}
	virtual void OnDisconnect(const TcpClientPointer& tcpClient, const boost::system::error_code & error){m_connectReturned = true;m_bDisconnect=true;}

private:
	IoService::pointer m_ipService;
	TcpClient::pointer m_tcpClient;
	bool m_connectReturned;
	bool m_bDisconnect;
	bool m_bException;

	TcpClient_Callback* m_pCallback;
	//mycp::tstring m_sReceiveData;
};

} // namespace entboost

#endif // __CgcTcpClient_h__
