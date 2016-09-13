#ifdef WIN32
#pragma warning(disable:4267 4819 4996)
#endif // WIN32

#include "CgcTcpClient.h"
#include "cgcaddress.h"
#include <boost/format.hpp>

namespace cgc
{
CgcTcpClient::CgcTcpClient(void)
: m_connectReturned(false)
, m_bDisconnect(true)
, m_sReceiveData(_T(""))
{
}

CgcTcpClient::~CgcTcpClient(void)
{
	stopClient();
}

std::string CgcTcpClient::GetHostIp(const char * lpszHostName,const char* lpszDefault)
{
	struct hostent *host_entry;
	//struct sockaddr_in addr;
	/* 即要解析的域名或主机名 */
	host_entry=gethostbyname(lpszHostName);
	//printf("%s\n", dn_or_ip);
	char lpszIpAddress[50];
	memset(lpszIpAddress, 0, sizeof(lpszIpAddress));
	if(host_entry!=0)
	{
		//printf("解析IP地址: ");
		sprintf(lpszIpAddress, "%d.%d.%d.%d",
			(host_entry->h_addr_list[0][0]&0x00ff),
			(host_entry->h_addr_list[0][1]&0x00ff),
			(host_entry->h_addr_list[0][2]&0x00ff),
			(host_entry->h_addr_list[0][3]&0x00ff));
		return lpszIpAddress;
	}else
	{
		return lpszDefault;
	}
}

int CgcTcpClient::startClient(const tstring & sCgcServerAddr, unsigned int bindPort)
{
	if (m_tcpClient.get() != 0) return 0;

	CCgcAddress cgcAddress = CCgcAddress(sCgcServerAddr, CCgcAddress::ST_TCP);
	const unsigned short nPort = (unsigned short)cgcAddress.getport();
	const tstring sInIp = cgcAddress.getip();
	std::string sIp;
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
	tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(sIp.c_str()), nPort);
	m_tcpClient->connect(m_ipService->ioservice(), endpoint);
	m_ipService->start();
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
	if (m_tcpClient.get() != 0)
	{
		//m_tcpClient->resetHandler();
		m_tcpClient->disconnect();
	}
	if (m_ipService.get() != 0)
	{
		m_ipService->stop();
	}
	m_tcpClient.reset();
	m_ipService.reset();
}

size_t CgcTcpClient::sendData(const unsigned char * data, size_t size)
{
	BOOST_ASSERT(m_tcpClient.get() != 0);

	if (data == 0 || isInvalidate()) return 0;

	return m_tcpClient->write(data, size);
}

bool CgcTcpClient::isInvalidate(void) const
{
	return m_tcpClient.get() == 0 || !m_tcpClient->is_open();
}

void CgcTcpClient::OnConnected(const TcpClientPointer& tcpClient)
{
	//BOOST_ASSERT (tcpClient.get() != 0);

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

void CgcTcpClient::OnReceiveData(const TcpClientPointer& tcpClient, const ReceiveBuffer::pointer& data)
{
	BOOST_ASSERT (tcpClient.get() != 0);
	BOOST_ASSERT (data.get() != 0);

	if (data->size() <= 0) return;
	//m_tSendRecv = time(0);
	//this->parseData(CCgcData::create(data->data(), data->size()));
	m_sReceiveData.append(tstring((const char*)data->data(), data->size()));
}

} // namespace cgc
