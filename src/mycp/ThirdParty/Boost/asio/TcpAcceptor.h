// TcpAcceptor.h file here
#ifndef __TcpAcceptor_h__
#define __TcpAcceptor_h__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
using boost::asio::ip::tcp;
#include "TcpConnection.h"

namespace mycp {
namespace asio {

///////////////////////////////////////////////
// TcpAcceptor class
class TcpAcceptor
{
private:
	tcp::acceptor * m_acceptor;
	TcpConnection_Handler::pointer m_handler;
#ifdef USES_OPENSSL
	boost::asio::ssl::context* m_sslctx;
#endif
	size_t m_nReadSleep;

public:
	typedef boost::shared_ptr<TcpAcceptor> pointer;
	static TcpAcceptor::pointer create(void) {return TcpAcceptor::pointer(new TcpAcceptor());}
#ifdef USES_OPENSSL
	void set_ssl_ctx(boost::asio::ssl::context* ctx) {m_sslctx=ctx;}
#endif
	size_t GetReadSleep(void) const {return m_nReadSleep;}
	void SetReadSleep(size_t v) {m_nReadSleep = v;}
	bool start(boost::asio::io_service & ioservice, size_t tcpPort, const TcpConnection_Handler::pointer& handler)
	{
		try
		{
			m_handler = handler;
			if (m_acceptor == NULL)
				m_acceptor = new tcp::acceptor(ioservice, tcp::endpoint(tcp::v4(), tcpPort));
			boost::system::error_code ec;
			m_acceptor->set_option(boost::asio::socket_base::reuse_address(true),ec);
			//m_acceptor->set_option(boost::asio::socket_base::enable_connection_aborted(true));
			//m_acceptor->set_option(boost::asio::socket_base::keep_alive(true));
			m_acceptor->set_option(boost::asio::socket_base::send_buffer_size(64*1024),ec);
			m_acceptor->set_option(boost::asio::socket_base::receive_buffer_size(64*1024),ec);
			start_accept();
			return true;
		}catch (std::exception & e)
		{
#ifdef WIN32
			printf("start exception. %s, lasterror=%d\n", e.what(), GetLastError());
#else
			printf("start exception. %s\n", e.what());
#endif
		}catch (boost::exception&)
		{
		}catch(...)
		{}
		return false;
	}
	void stop(void)
	{
		if (m_acceptor)
		{
			boost::system::error_code ec;
			m_acceptor->close(ec);
			delete m_acceptor;
			m_acceptor = NULL;
		}
		m_handler.reset();
	}
private:
	void start_accept(void)
	{
		// 这里不做TRY CATCH，由IOSERVICE统一处理，有问题，会自动重启服务
		// ~~2017-05-26 去掉 try 测试长时间应用不能收到消息情况；
		try {
			if (m_acceptor != NULL) {
				//printf("start_accept...\n");
#ifdef USES_OPENSSL
				TcpConnectionPointer new_connection = TcpConnection::create(m_acceptor->get_io_service(), m_handler,m_sslctx);
#else
				TcpConnectionPointer new_connection = TcpConnection::create(m_acceptor->get_io_service(), m_handler);
#endif
				if (m_sslctx==NULL) {
					m_acceptor->async_accept(*new_connection->socket()->get_socket(),
						boost::bind(&TcpAcceptor::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
				}
				else {
					m_acceptor->async_accept(new_connection->socket()->get_ssl_socket()->lowest_layer(),
						boost::bind(&TcpAcceptor::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
				}
			}
		}
		catch (std::exception & e) {
#ifdef WIN32
			printf("start_accept exception. %s, lasterror=%d\n", e.what(), GetLastError());
#else
			printf("start_accept exception. %s\n", e.what());
#endif
		}
		catch (boost::exception&) {
		}
		catch(...) {
		}
	}

	void handle_accept(const TcpConnectionPointer& new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
//#ifdef WIN32
//			Sleep(1);
//#else
//			usleep(1000);
//#endif
			new_connection->SetReadSleep(this->m_nReadSleep);
			static unsigned short static_id_index = 0;
			char lpszId[24];
			sprintf(lpszId, "%04d%03d%03d", (int)time(0)%2000,rand()%1000,(++static_id_index)%1000);
			new_connection->start(atoi(lpszId));
		}else
		{
			printf("handle_accept error: %d=%s\n", error.value(),error.message().c_str());
		}
		start_accept();	//准备下一个client
	}

public:
	TcpAcceptor(void)
		: m_acceptor(NULL)
#ifdef USES_OPENSSL
		, m_sslctx(NULL)
#endif
		, m_nReadSleep(0)
	{
	}
	virtual ~TcpAcceptor(void)
	{
		stop();
	}

};

} // namespace asio
} // namespace mycp

#endif // __TcpAcceptor_h__
