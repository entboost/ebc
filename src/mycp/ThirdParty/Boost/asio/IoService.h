// IoService.h file here
#ifndef __IoService_h__
#define __IoService_h__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
//
//#ifdef WIN32
//#include <Mmsystem.h>
//#else
//#include <time.h>   
//inline unsigned long timeGetTime()  
//{  
//	unsigned long uptime = 0;  
//	struct timespec on;  
//	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)  
//		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;  
//	return uptime;  
//} 
//#endif

namespace mycp {
namespace asio {

class IoService_Handler
{
public:
	typedef boost::shared_ptr<IoService_Handler> pointer;
	virtual void OnIoServiceException(void)=0;
};
const IoService_Handler::pointer ConstNullIoService_Handler;

///////////////////////////////////////////////
// IoService class
class IoService
{
public:
	typedef boost::shared_ptr<IoService> pointer;
	typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr; 
	static IoService::pointer create(void) {return IoService::pointer(new IoService());}

	void start(IoService_Handler::pointer handler=ConstNullIoService_Handler, int nThreadStackSize=200)
	{
		m_bKilled = false;
		m_pHandler = handler;
		if (m_pProcEventLoop.get() == NULL)
		{
			boost::thread_attributes attrs;
			attrs.set_stack_size(1024*nThreadStackSize);	// 200K
			m_pProcEventLoop = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&IoService::do_event_loop, this)));
			//m_pProcEventLoop = new boost::thread(boost::bind(&IoService::do_event_loop, this));
		}
	}

	void stop(void)
	{
		m_bKilled = true;
		try
		{
			m_ioservice->stop();
			if (m_pProcEventLoop.get()!=NULL)
			{
				m_pProcEventLoop->join();
				m_pProcEventLoop.reset();
			}
		}catch(const std::exception&)
		{}
		catch(const boost::exception&)
		{}
		catch(...)
		{}
		m_pHandler.reset();
	}
	bool is_killed(void) const {return m_bKilled;}
	bool is_start(void) const {return (m_pProcEventLoop.get() != NULL)?true:false;}
	boost::asio::io_service & ioservice(void) {return *m_ioservice;}
	void do_poll(void)
	{
		boost::system::error_code ec;
		m_ioservice->poll(ec);
	}
	void do_run(void)
	{
		boost::system::error_code ec;
		m_ioservice->run(ec);
	}
	void do_reset(void)
	{
		try
		{
			m_ioservice->reset();
		}catch (const std::exception &)
		{
		}catch(...)
		{
		}
	}

	void on_exception(void)
	{
		if (m_pHandler.get()!=NULL)
			m_pHandler->OnIoServiceException();
	}
private:
	void do_event_loop(void)
		//static void do_event_loop(IoService * pIoService)
	{
		//if (pIoService != NULL)
		{
			//boost::asio::io_service & pService = pIoService->ioservice();
			//boost::asio::io_service::work work(pService);	// 保证run不会退出
			while (!is_killed()) {
				try {
#ifdef WIN32
					Sleep(5);
#else
					usleep(5000);
#endif
					//do_run();
					do_poll();

					//pIoService->do_run();
					//boost::system::error_code ec;
					//pService.poll(ec);
					continue;
					//boost::system::error_code ec;
					//pService.run(ec);
					//pService.reset();
					//printf("do_event_loop exit:%s=%d\n",ec.message().c_str(),ec.value());
					//break;
				}
				catch (std::exception & e) {
					do_reset();
					//pService.reset();
#ifdef WIN32
					printf("do_event_loop std::exception. %s, lasterror=%d\n", e.what(), (int)GetLastError());
#else
					printf("do_event_loop std::exception. %s\n", e.what());
#endif
					//const std::string sMessage = e.what();
					//if (sMessage.find("remote_endpoint: Transport endpoint is not connected")==0)
					//{
					//	continue;
					//}
					on_exception();
					break;
				}
				catch(boost::exception&) {
					do_reset();
					//pService.reset();
#ifdef WIN32
					printf("do_event_loop boost::exception. lasterror=%d\n", (int)GetLastError());
#else
					printf("do_event_loop boost::exception.\n");
#endif
					on_exception();
					break;
				}
				catch(...) {
					do_reset();
					//pService.reset();
#ifdef WIN32
					printf("do_event_loop ... exception. lasterror=%d\n", (int)GetLastError());
#else
					printf("do_event_loop exception. \n");
#endif
					on_exception();
					break;
				}
			}
		}
	}

public:
	IoService(void)
		: m_bKilled(false)
	{
		m_ioservice = io_service_ptr(new boost::asio::io_service);
	}
	virtual ~IoService(void)
	{
		stop();
		m_ioservice.reset();
	}
private:
	bool m_bKilled;
	io_service_ptr m_ioservice;
	boost::shared_ptr<boost::thread> m_pProcEventLoop;
	IoService_Handler::pointer m_pHandler;
};

} // namespace asio
} // namespace mycp

#endif // __IoService_h__
