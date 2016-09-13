// UdpEndPoint.h file here
#ifndef __UdpEndPoint_h__
#define __UdpEndPoint_h__

#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::udp;
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace mycp {
namespace asio {

const size_t Max_UdpSocket_ReceiveSize	= 32*1024;
////////////////////////
// UdpEndPoint class
class UdpEndPoint
	: public boost::enable_shared_from_this<UdpEndPoint>
{
public:
	typedef boost::shared_ptr<UdpEndPoint> pointer;
	static pointer create(size_t capacity = Max_UdpSocket_ReceiveSize)
	{
		return pointer(new UdpEndPoint(capacity));
	}

	virtual ~UdpEndPoint()
	{
#ifdef _DEBUG
		std::cout << "UdpEndPoint destructor" << std::endl;
#endif
		delete[] m_buffer;
	}

	void init2(void)
	{
		boost::asio::ip::address address = remote_endpoint.address();
		if (address.is_v4())
		{
			m_nIpAddress = address.to_v4().to_ulong();
			m_nId = m_nIpAddress*100 + remote_endpoint.port();	// ? +port	
		}else
		{
			// ??? IPV6
			//m_nIpAddress = address.to_v6().to_v4().to_ulong();
			m_nIpAddress = address.to_v6().scope_id();
			m_nId = m_nIpAddress + remote_endpoint.port();	
			//m_nId = address.to_v6().scope_id() + remote_endpoint.port();	
		}
	}
	udp::endpoint& endpoint(void) {return remote_endpoint;}
	const unsigned char * buffer(void) const {return m_buffer;}
	unsigned long getId(void) const {return m_nId;}
	unsigned long getIpAddress(void) const {return m_nIpAddress;}
	void size(int newv) {m_size = newv;}
	int size(void) const {return m_size;}
	int bufferSize(void) const {return m_nBufferSize;}
	bool init(int capacity)
	{
		if (m_buffer==NULL)
		{
			m_nBufferSize = capacity+1;
			m_buffer = new unsigned char[m_nBufferSize];
			if (m_buffer==NULL)
				return false;
		}else if (m_nBufferSize<=capacity)
		{
			delete[] m_buffer;
			m_buffer = NULL;
			m_nBufferSize = capacity+1;
			m_buffer = new unsigned char[m_nBufferSize];
			if (m_buffer==NULL)
				return false;
		}
		m_size = 0;
		m_buffer[0] = '\0';
		return true;
	}
private:
	UdpEndPoint(size_t capacity)
		: m_nId(0),m_nIpAddress(0),m_size(0),m_buffer(NULL),m_nBufferSize(0)
	{
		if (capacity == 0 || capacity > Max_UdpSocket_ReceiveSize)
			capacity = Max_UdpSocket_ReceiveSize;
		init((int)capacity);
		//m_buffer = new unsigned char[m_nBufferSize];
		//memset(m_buffer, 0,  m_nBufferSize);
	}

private:
	unsigned long m_nId;
	unsigned long m_nIpAddress;	// host byte order
	udp::endpoint remote_endpoint;
	int m_size;
	unsigned char* m_buffer;
	int m_nBufferSize;
};

} // namespace asio
} // namespace mycp

#endif // __UdpEndPoint_h__
