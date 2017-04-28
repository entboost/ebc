// TcpConnection.h file here
#ifndef __TcpConnection_h__
#define __TcpConnection_h__

#include <boost/asio.hpp>
#include "boost_socket.h"
using boost::asio::ip::tcp;
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../stl/locklist.h"
#include <iostream>
#include "ReceiveBuffer.h"

////////////////////////
//
namespace mycp {
namespace asio {

class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPointer;
class TcpConnection_Handler
{
public:
	typedef boost::shared_ptr<TcpConnection_Handler> pointer;
	virtual void OnRemoteAccept(const TcpConnectionPointer& pRemote)=0;
	virtual bool OnRemoteRecv(const TcpConnectionPointer& pRemote, const ReceiveBuffer::pointer& data)=0;
	virtual void OnRemoteClose(const TcpConnection* pRemote, int nErrorCode)=0;
};

const TcpConnection_Handler::pointer NullTcpConnectionHandler;

#define USES_TCP_CONNECTION_OWNER_POINTER

class TcpConnection
	: public boost::enable_shared_from_this<TcpConnection>
{
private:
	unsigned long m_nId;
	unsigned long m_nIpAddress;	// host byte order
	//tcp::socket m_socket;
	boost_socket_base<tcp::socket>* m_socket;
	bool m_bClosed;
	tcp::endpoint m_localEndPoint;
	tcp::endpoint m_remoteEndPoint;
	TcpConnection_Handler::pointer m_handler;
	//bool m_killed;
	//boost::thread * m_proc_data;
	//CLockList<ReceiveBuffer::pointer> m_datas;
	//CLockList<ReceiveBuffer::pointer> m_unused;
	ReceiveBuffer::pointer m_pBuffer;
	size_t m_unusedsize;
	//size_t m_maxbuffersize;
	size_t m_nReadSleep;
#ifdef USES_TCP_CONNECTION_OWNER_POINTER
	TcpConnectionPointer m_pOwner;
#endif
public:
	//typedef boost::shared_ptr<TcpConnection> pointer;
#ifdef USES_OPENSSL
	static TcpConnectionPointer create(boost::asio::io_service& io_service, const TcpConnection_Handler::pointer& handler,boost::asio::ssl::context* ctx=NULL)
	{
		return TcpConnectionPointer(new TcpConnection(io_service, handler,ctx));
	}
#else
	static TcpConnectionPointer create(boost::asio::io_service& io_service, const TcpConnection_Handler::pointer& handler)
	{
		return TcpConnectionPointer(new TcpConnection(io_service, handler));
	}
#endif
	virtual ~TcpConnection(void)
	{
		//m_killed = true;
		//if (m_proc_data)
		//{
		//	m_proc_data->join();
		//	delete m_proc_data;
		//}
		//m_datas.clear();
		//m_unused.clear();
		//m_socket.close();
		if (m_socket!=NULL)
		{
			close();
			delete m_socket;
			m_socket = NULL;
		}
		m_pBuffer.reset();
#ifdef _DEBUG
		std::cout << "TcpConnection destructor" << std::endl;
#endif
		m_handler.reset();
	}

	boost_socket_base<tcp::socket>* socket(void) {return m_socket;}
	//tcp::socket::lowest_layer_type& lowest_layer(void) {return m_socket->lowest_layer();}
	//tcp::socket& socket(void) {return m_socket;}
	const tcp::endpoint& remote_endpoint(void) const {return m_remoteEndPoint;}
	const tcp::endpoint& local_endpoint(void) const {return m_localEndPoint;}
	std::string server_address(void) const
	{
		boost::system::error_code error;
		return m_localEndPoint.address().to_string(error);
	}
	unsigned long getId(void) const {return m_nId;}
	unsigned long getIpAddress(void) const {return m_nIpAddress;}
	//void setUnusedSize(size_t v = 10) {m_unusedsize = v;}
	//void setMaxBufferSize(size_t v = Max_ReceiveBuffer_ReceiveSize) {m_maxbuffersize = v;}
	size_t GetReadSleep(void) const {return m_nReadSleep;}
	void SetReadSleep(size_t v) {m_nReadSleep = v;}
	void close(void)
	{
		if (!m_bClosed && m_socket!=NULL)
		{
			//printf("**** TcpConnection::close()\n");
			m_bClosed = true;
			try{
				m_socket->close();
			}catch (std::exception&)
			{
			}catch (boost::exception&)
			{
			}catch(...)
			{}
		}
#ifdef USES_TCP_CONNECTION_OWNER_POINTER
		m_pOwner.reset();
#endif
	}
	bool is_closed(void) const {return m_bClosed;}

	bool start(unsigned long nId)
	{
		boost::system::error_code error;
		m_localEndPoint = m_socket->lowest_layer().local_endpoint(error);
		m_remoteEndPoint = m_socket->lowest_layer().remote_endpoint(error);
		if (error)
			return false;

		//static unsigned short static_id_index = 0;
		//char lpszId[24];
		//sprintf(lpszId, "%04d%03d%03d", (int)time(0)%2000,rand()%1000,(++static_id_index)%1000);
		//m_nId = atoi(lpszId);
		m_nId = nId;
		boost::asio::ip::address address = m_remoteEndPoint.address();
		if (address.is_v4())
		{
			m_nIpAddress = address.to_v4().to_ulong();
		}else
		{
			// ??? IPV6
			m_nIpAddress = address.to_v6().scope_id();
		}
#ifdef USES_TCP_CONNECTION_OWNER_POINTER
		if (m_pOwner.get()==NULL)
			m_pOwner = shared_from_this();
#else
		const TcpConnectionPointer m_pOwner = shared_from_this();
#endif
		//try
		//{
		//	if (m_proc_data == 0)
		//		m_proc_data = new boost::thread(boost::bind(&TcpConnection::do_proc_data, this));
		//}catch (std::exception & e)
		//{
		//	printf("TcpConnection start exception. %s\n0x%x\n", e.what(),m_proc_data);
		//	return false;
		//}
		//printf("connect start...\n");
#ifdef USES_OPENSSL
		if (m_socket->is_ssl())
		{
			//printf("**** async_handshake\n");
			try
			{
				boost::asio::ssl::stream<tcp::socket> * p = m_socket->get_ssl_socket();
				p->async_handshake(boost::asio::ssl::stream_base::server,boost::bind(&TcpConnection::handle_handshake,
					this,m_pOwner,boost::asio::placeholders::error));
			}catch (std::exception &)
			{
//#ifdef WIN32
//				printf("start_accept exception. %s, lasterror=%d\n", e.what(), GetLastError());
//#else
//				printf("start_accept exception. %s\n", e.what());
//#endif
			}catch (boost::exception &)
			{}
			catch(...)
			{}
			return true;
		}
#endif
		if (m_handler.get() != NULL)
			m_handler->OnRemoteAccept(m_pOwner);
		start_read();
		return true;
	}
#ifdef USES_OPENSSL
	void handle_handshake(const TcpConnectionPointer& pconnection,const boost::system::error_code& e)
	{
		if(!e)
		{
			if (m_handler.get() != NULL)
				m_handler->OnRemoteAccept(pconnection);
			if (m_nReadSleep > 0)
			{
#ifdef WIN32
				Sleep(m_nReadSleep);
#else
				usleep(m_nReadSleep*1000);
#endif
			}
			start_read();
		}else
		{
			//printf("**** handle_handshake %s,%d\n",e.message().c_str(),e.value());
			try{
				boost::system::error_code ec;
				m_socket->get_ssl_socket()->shutdown(ec);
			}catch (std::exception&)
			{
			}catch (boost::exception&)
			{
			}catch(...)
			{}
			close();
			//m_socket->close();
		}
	}
#endif
	void write_handler(const boost::system::error_code& ec)
     {
		 if(ec)
			 std::cout<< "Send data error!" << std::endl;
		 //else
			// std::cout<< *pstr << " 已发送" << std::endl;
	}
	void read_some_handler(const ReceiveBuffer::pointer& buffer, const boost::system::error_code& error, std::size_t size)
	//void read_some_handler(const ReceiveBuffer::pointer& buffer, const TcpConnectionPointer& pOwner,const boost::system::error_code& error, std::size_t size)
	{
		//printf("**** read_some_handler %s,%d;%x\n",error.message().c_str(),error.value(),buffer.get());
		if ( !error && !m_bClosed)
		{
			buffer->size(size);
			//m_datas.add(buffer);
			bool bStartRead = true;
			if (m_handler.get() != NULL)
			{
				try
				{
#ifndef USES_TCP_CONNECTION_OWNER_POINTER
					const TcpConnectionPointer m_pOwner = shared_from_this();
#endif
					bStartRead = m_handler->OnRemoteRecv(m_pOwner, buffer);
					//bStartRead = m_handler->OnRemoteRecv(pOwner, buffer);
				}catch(std::exception&)
				{
				}catch(boost::exception&)
				{
				}catch(...)
				{}
			}
			if (bStartRead)
				start_read();
		}else
		{
			ShowNetWorkError(error);
			//if (error.value()==335544539)	// short read
			//{
			//	printf("****(%d)\n%s\n",buffer->size(),buffer->data());
			//	if (m_handler.get() != NULL)
			//		m_handler->OnRemoteRecv(this, buffer);
			//	start_read();
			//	return;
			//}
			//try
			//{
			//	//printf("**** m_handler->OnRemoteClose 111\n");
			//	if (m_handler.get() != NULL)
			//		m_handler->OnRemoteClose(this);
			//	//printf("**** m_handler->OnRemoteClose 222\n");
			//	// 9 Bad file descriptor
			//	// 125 Operation canceled
			//	// ** 不主动关闭，不会异常，但会有很多CLOSE_WAIT连接
			//	if (error.value()!=125)	// 125 Operation canceled
			//	{
			//		m_socket->close();
			//	}
			//	//printf("**** m_handler->OnRemoteClose end\n");
			//}catch (std::exception& e)
			//{
			//	printf("**** read_some_handler exception. %s\n", e.what());
			//}catch(...)
			//{
			//}

			// ? add some error_code
			//switch (error.value())
			//{
			//case 104:	// Connection reset by peer
			//	break;
			////case 9:		//  Bad file descriptor
			////case 125:	// Operation canceled
			////	//m_socket.close();
			////	//break;
			////case 10054:	// 远程主机强迫关闭一个现有连接
			////case 1236:	// 本地系统终止网络连接
			////case 121:	// 信号灯超时时间已到
			////case 110:	// Connection time out
			////case 2:		// End of file
			//default:
			//	{
					if (m_handler.get() != NULL)
						m_handler->OnRemoteClose(this, error.value());
					try
					{
						//if (!m_socket.is_open())
						{
#ifdef USES_OPENSSL
							if (m_socket->is_ssl())
							{
								boost::system::error_code ec;
								m_socket->get_ssl_socket()->shutdown(ec);
							}
#endif
							//m_socket->close();
						}
					}catch (std::exception&)
					{
					}catch (boost::exception &)
					{}
					catch(...)
					{}
					close();
			//	}break;
			//}
		}
	}

//	void proc_Data(void)
//	{
//		while (!m_killed)
//		{
//			ReceiveBuffer::pointer buffer;
//			if (!m_datas.front(buffer))
//			{
//#ifdef WIN32
//				Sleep(5);
//#else
//				usleep(5000);
//#endif
//				continue;
//			}
//
//			if (m_handler.get() != NULL)
//				m_handler->OnRemoteRecv(this, buffer);
//				//m_handler->OnRemoteRecv(shared_from_this(), buffer);
//
//			if (m_unused.size() < m_unusedsize)
//			{
//				buffer->reset();
//				m_unused.add(buffer);
//			}
//		}
//	}

private:
	void start_read(void)
	{
		// **这里不做TRY CATCH，由IOSERVICE统一处理，有问题，会自动重启服务
		// 2017-04-18 有异常，去掉 try 测试情况；
		//try
		//{
			//printf("**** start_read->async_read_some,socket=%x,buffer=%x\n",m_socket,m_pBuffer.get());
			m_socket->async_read_some(boost::asio::buffer(const_cast<unsigned char*>(m_pBuffer->data()), Max_ReceiveBuffer_ReceiveSize),
				boost::bind(&TcpConnection::read_some_handler, this, m_pBuffer,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			//m_socket->async_read_some(boost::asio::buffer(const_cast<unsigned char*>(m_pBuffer->data()), m_maxbuffersize),
			//	boost::bind(&TcpConnection::read_some_handler, this, m_pBuffer,shared_from_this(),
			//	boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

			//ReceiveBuffer::pointer newBuffer;
			//if (!m_unused.front(newBuffer))
			//	newBuffer = ReceiveBuffer::create();
			//m_socket.async_read_some(boost::asio::buffer(const_cast<unsigned char*>(newBuffer->data()), m_maxbuffersize),
			//	boost::bind(&TcpConnection::read_some_handler, this, newBuffer,
			//	boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
//		}catch (std::exception &)
//		{
////#ifdef WIN32
////			printf("start_read exception. %s, lasterror=%d\n", e.what(), GetLastError());
////#else
////			printf("start_read exception. %s\n", e.what());
////#endif
//		}catch (boost::exception&)
//		{
//		}catch(...)
//		{
//		}
	}

#ifdef USES_OPENSSL
	TcpConnection(boost::asio::io_service& io_service, const TcpConnection_Handler::pointer& handler,boost::asio::ssl::context* ctx=NULL)
#else
	TcpConnection(boost::asio::io_service& io_service, const TcpConnection_Handler::pointer& handler)
#endif
		: m_nId(0),m_nIpAddress(0)
		//, m_socket(io_service)
		, m_bClosed(false)
		, m_handler(handler)
		//, m_killed(false), m_proc_data(0)
		, m_unusedsize(10)//, m_maxbuffersize(Max_ReceiveBuffer_ReceiveSize)
		,m_nReadSleep(0)
	{
		//m_unused.clear(false);
		m_pBuffer = ReceiveBuffer::create(Max_ReceiveBuffer_ReceiveSize);
#ifdef USES_OPENSSL
		if (ctx)
			m_socket = new boost_ssl_socket<tcp::socket>(io_service,*ctx);
		else
			m_socket = new boost_socket<tcp::socket>(io_service);
#else
		m_socket = new boost_socket<tcp::socket>(io_service);
#endif
	}

	//void handle_write(const boost::system::error_code& /*error*/,
	//	size_t /*bytes_transferred*/)
	//{
	//}

	void ShowNetWorkError( const boost::system::error_code &error )
	{
		printf("**** Tcp Connection: %s(%d) ****\n",error.message().c_str(),error.value());
	}

	//static void do_proc_data(TcpConnection * owner)
	//{
	//	BOOST_ASSERT (owner != 0);
	//	owner->proc_Data();
	//}
};

} // namespace asio
} // namespace mycp

#endif // __TcpConnection_h__
