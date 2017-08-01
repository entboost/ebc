// TcpClient.h file here
#ifndef __entoost_TcpClient_h__
#define __entoost_TcpClient_h__

#include <boost/asio.hpp>
#include <ThirdParty/Boost/asio/boost_socket.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <ThirdParty/stl/locklist.h>
//#include "../../stl/locklist.h"
using boost::asio::ip::tcp;
#include <ThirdParty/Boost/asio/ReceiveBuffer.h>
//#include "ReceiveBuffer.h"
//using namespace cgc;

namespace entboost {

//////////////////////////////////////////////
// TcpClient_Handler
class TcpClient;
typedef boost::shared_ptr<TcpClient> TcpClientPointer;
class TcpClient_Handler
{
public:
	typedef boost::shared_ptr<TcpClient_Handler> pointer;

	virtual void OnConnected(const TcpClientPointer& tcpClient) = 0;
	virtual void OnConnectError(const TcpClientPointer& tcpClient, const boost::system::error_code & error) = 0;
	virtual void OnReceiveData(const TcpClientPointer& tcpClient, const mycp::asio::ReceiveBuffer::pointer& data) = 0;
	virtual void OnDisconnect(const TcpClientPointer & tcpClient,const boost::system::error_code & error) = 0;
};

const TcpClient_Handler::pointer NullTcpClientHandler;

///////////////////////////////////////////////
// TcpClient class
class TcpClient
	: public boost::enable_shared_from_this<TcpClient>
{
public:
	typedef boost::shared_ptr<TcpClient> pointer;
	static TcpClient::pointer create(const TcpClient_Handler::pointer& handler)
	{
		return TcpClient::pointer(new TcpClient(handler));
	}

	void setUnusedSize(size_t v = 10) {m_unusedsize = v;}
	void setMaxBufferSize(size_t v = mycp::asio::Max_ReceiveBuffer_ReceiveSize) {m_maxbuffersize = v;}
	void setDisconnectWaittingData(int nWaittingTime) {m_nDisconnectWaittingData = nWaittingTime;}

#ifdef USES_OPENSSL
	void connect(boost::asio::io_service& pIoService, tcp::endpoint& endpoint,boost::asio::ssl::context* ctx=NULL, bool bReceiveData=true, int nThreadStackSize=200)
#else
	void connect(boost::asio::io_service& pIoService, tcp::endpoint& endpoint, bool bReceiveData=true, int nThreadStackSize=200)
#endif
	{
		if (m_socket == NULL)
		{
#ifdef USES_OPENSSL
			if (ctx)
			{
				//ctx->set_password_callback(boost::bind(&TcpClient::getSSLPassword, this));
				m_socket = new mycp::asio::boost_ssl_socket<tcp::socket>(pIoService,*ctx);
				m_socket->get_ssl_socket()->set_verify_mode(boost::asio::ssl::verify_peer);  
				m_socket->get_ssl_socket()->set_verify_callback(boost::bind(&TcpClient::verify_certificate, this, _1, _2)); 
				//ctx->set_password_callback(boost::bind(&TcpClient::get_password, this));
				//m_socket->get_ssl_socket()->set_option(boost::asio::socket_base::send_buffer_size(64*1024));
				//m_socket->get_ssl_socket()->set_option(boost::asio::socket_base::receive_buffer_size(64*1024));
			}else
			{
				m_socket = new mycp::asio::boost_socket<tcp::socket>(pIoService);
				boost::system::error_code ec;
				m_socket->get_socket()->set_option(boost::asio::socket_base::send_buffer_size(64*1024),ec);
				m_socket->get_socket()->set_option(boost::asio::socket_base::receive_buffer_size(64*1024),ec);
			}
#else
			m_socket = new tcp::socket(pIoService);
			boost::system::error_code ec;
			m_socket->set_option(boost::asio::socket_base::send_buffer_size(64*1024),ec);
			m_socket->set_option(boost::asio::socket_base::receive_buffer_size(64*1024),ec);
#endif
		}
		if (m_proc_data.get() == NULL && bReceiveData)
		{
			boost::thread_attributes attrs;
			attrs.set_stack_size(1024*nThreadStackSize);	// 200K
			m_proc_data = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&TcpClient::proc_Data, this)));
			//m_proc_data = new boost::thread(boost::bind(&TcpClient::do_proc_data, this));
		}

#ifdef USES_OPENSSL
		m_socket->lowest_layer().async_connect(endpoint,
			//m_socket.async_connect(endpoint,
			boost::bind(&TcpClient::connect_handler, this,
			boost::asio::placeholders::error)
			);

		//if (m_socket->is_ssl())
		//{
		//	boost::asio::ssl::stream<tcp::socket> * p = m_socket->get_ssl_socket();
		//	p->async_handshake(boost::asio::ssl::stream_base::client,boost::bind(&TcpClient::handle_handshake,
		//		this,shared_from_this(),boost::asio::placeholders::error));
		//}else
		//{
		//	m_socket->get_socket()->async_connect(endpoint,
		//		//m_socket.async_connect(endpoint,
		//		boost::bind(&TcpClient::connect_handler, this,
		//		boost::asio::placeholders::error)
		//		);
		//}
#else
		m_socket->async_connect(endpoint,
			//m_socket.async_connect(endpoint,
			boost::bind(&TcpClient::connect_handler, this,
			boost::asio::placeholders::error)
			);
#endif
	}
	void disconnect(void)
	{
		close(true);
		try
		{
			if (m_proc_data.get()!=NULL)
			{
				m_proc_data->join();
				m_proc_data.reset();
			}
		}catch(const std::exception&)
		{}
		catch(const boost::exception&)
		{}
		catch(...)
		{}

		m_datas.clear();
		m_unused.clear();
	}
	void close(bool bDeleteSocket=false)
	{
		try
		{
			boost::mutex::scoped_lock lock(m_mutex);
			if (m_socket!=NULL && !m_bClosed)
			{
				m_bClosed = true;
				m_socket->close();
			}
			if (bDeleteSocket && m_socket!=NULL)
			{
				delete m_socket;
				m_socket = NULL;
			}
		}catch(const std::exception&)
		{}
		catch(const boost::exception&)
		{}
		catch(...)
		{}
	}
#ifdef USES_OPENSSL
	//void setSSLPassword(const tstring& sPassword) {m_sSSLPassword = sPassword;}
	//std::string getSSLPassword(void) const {return m_sSSLPassword;}

	bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)  
	{  
		// The verify callback can be used to check whether the certificate that is  
		// being presented is valid for the peer. For example, RFC 2818 describes  
		// the steps involved in doing this for HTTPS. Consult the OpenSSL  
		// documentation for more details. Note that the callback is called once  
		// for each certificate in the certificate chain, starting from the root  
		// certificate authority.  

		// In this example we will simply print the certificate's subject name.  
		try
		{
			char subject_name[512];  
			X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
			if (cert!=NULL)
			{
				X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 512);
				//std::cout << "Verifying " << subject_name << "\n";  
			}
		}catch(const std::exception&)
		{}
		catch(const boost::exception&)
		{}
		catch(...)
		{}
		return true;
		//return preverified;  
	} 
	void handle_handshake(const TcpClient::pointer& pTcpClient,const boost::system::error_code& error)
	{
		if(!error)
		{
			if (m_handler.get() != NULL)
			{
				//m_handler->OnConnected(*this);
				m_handler->OnConnected(shared_from_this());
			}
			async_read_some();
		}else
		{
			//std::string sError = error.message();
			close();
			if (m_handler.get() != NULL)
			{
				//m_handler->OnConnectError(*this, error);
				m_handler->OnConnectError(shared_from_this(), error);
			}
		}
	}
#endif
	bool is_open(void) const
	{
		return m_socket != NULL && m_socket->is_open();
		//return m_socket.is_open();
	}
	bool is_empty_data(void) const {return m_datas.empty();}
	size_t write(const unsigned char * data, size_t size)
	{
		if (m_socket == 0) return 0;
		boost::system::error_code ec;
#ifdef USES_OPENSSL
		return boost::asio::write(*m_socket, boost::asio::buffer(data,size), boost::asio::transfer_all(), ec);
#else
		return boost::asio::write(*m_socket, boost::asio::buffer(data,size), boost::asio::transfer_all(), ec);
		//return boost::asio::write(*m_socket, boost::asio::buffer(data, size),ec);
#endif
	}

#ifdef USES_OPENSSL
	mycp::asio::boost_socket_base<tcp::socket>* socket(void) {return m_socket;}
	tcp::socket::lowest_layer_type& lowest_layer(void) {return m_socket->lowest_layer();}
#else
	tcp::socket * socket(void) const {return m_socket;}
#endif

	void proc_Data(void)
	{
		TcpClientPointer pTcpClient = shared_from_this();
		while (m_socket != 0)
		{
			mycp::asio::ReceiveBuffer::pointer buffer;
			if (!m_datas.front(buffer))
			{
#ifdef WIN32
				Sleep(5);
#else
				usleep(5000);
#endif
				continue;
			}
			m_bInOnReceiveData = true;
			if (m_handler.get() != NULL)
			{
				//m_handler->OnReceiveData(*this, buffer);
				try
				{
					m_handler->OnReceiveData(pTcpClient, buffer);
				}catch(const std::exception&)
				{
				}catch(...)
				{}
			}
			m_bInOnReceiveData = false;

			if (m_unused.size() < m_unusedsize)
			{
				buffer->reset();
				m_unused.add(buffer);
			}
		}
	}

	void setHandler(TcpClient_Handler::pointer v) {m_handler = v;}
	void async_read_some(void)
	{
		if (m_socket == 0 || m_proc_data.get()==NULL) return;

		try
		{
			mycp::asio::ReceiveBuffer::pointer newBuffer;
			if (!m_unused.front(newBuffer))
				newBuffer = mycp::asio::ReceiveBuffer::create();
			m_socket->async_read_some(boost::asio::buffer(const_cast<unsigned char*>(newBuffer->data()), m_maxbuffersize),
				boost::bind(&TcpClient::read_some_handler,this, newBuffer,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
				);
		}catch(const std::exception&)
		{
		}catch(const boost::exception&)
		{
		}catch(...)
		{}
	}

private:

	void connect_handler(const boost::system::error_code& error)
	{
		if (m_socket == 0) return;

		if(!error)
		{
#ifdef USES_OPENSSL
			if (m_socket->is_ssl())
			{
				m_socket->get_ssl_socket()->async_handshake(boost::asio::ssl::stream_base::client,boost::bind(&TcpClient::handle_handshake,
					this,shared_from_this(),boost::asio::placeholders::error));
				return;
			}
#endif
			if (m_handler.get() != NULL)
			{
				//m_handler->OnConnected(*this);
				m_handler->OnConnected(shared_from_this());
			}
			async_read_some();
		}else
		{
			close();
			if (m_handler.get() != NULL)
			{
				//m_handler->OnConnectError(*this, error);
				m_handler->OnConnectError(shared_from_this(), error);
			}
		}
	}
	void read_some_handler(mycp::asio::ReceiveBuffer::pointer newBuffer, const boost::system::error_code& error, std::size_t size)
	{
		if (m_socket == 0) return;
		// ???·µ»Ø2´íÎó£»

		if(!error)
		{
			newBuffer->size(size);
			m_datas.add(newBuffer);
			//if (m_handler)
			//	m_handler->OnReceiveData(*this, newBuffer);
			async_read_some();
		}else
		{
			// ??
			if (m_handler.get() != NULL)
			{
				if (m_nDisconnectWaittingData>0)
				{
					for (int i=0;i<m_nDisconnectWaittingData*50;i++)
					{
#ifdef WIN32
						Sleep(20);
#else
						usleep(20000);
#endif
						if (!m_bInOnReceiveData && m_datas.empty())
							break;
					}
					m_handler->OnDisconnect(shared_from_this(), error);
				}
			}
			close();
		}
	}
	//void handle_write(const boost::system::error_code& error)
	//{
	//}

	//static void do_proc_data(TcpClient * owner)
	//{
	//	BOOST_ASSERT (owner != 0);
	//	owner->proc_Data();
	//}

public:
	TcpClient(const TcpClient_Handler::pointer& handler)
		: m_socket(0)
		, m_bClosed(false)
		, m_handler(handler)
		, m_unusedsize(10), m_maxbuffersize(mycp::asio::Max_ReceiveBuffer_ReceiveSize)
		, m_nDisconnectWaittingData(6), m_bInOnReceiveData(false)
	{
	}
	virtual ~TcpClient(void)
	{
		disconnect();
		m_handler.reset();
	}
private:
#ifdef USES_OPENSSL
	mycp::asio::boost_socket_base<tcp::socket>* m_socket;
#else
	tcp::socket * m_socket;
#endif
	boost::mutex m_mutex;
	bool m_bClosed;
	TcpClient_Handler::pointer m_handler;
	boost::shared_ptr<boost::thread> m_proc_data;
	CLockList<mycp::asio::ReceiveBuffer::pointer> m_datas;
	CLockList<mycp::asio::ReceiveBuffer::pointer> m_unused;
	size_t m_unusedsize;
	size_t m_maxbuffersize;
	//tstring m_sSSLPassword;
	int m_nDisconnectWaittingData;	// default 6s
	bool m_bInOnReceiveData;
};

} // namespace entboost

#endif // __TcpClient_h__
