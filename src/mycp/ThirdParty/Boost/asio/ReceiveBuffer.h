// ReceiveBuffer.h file here
#ifndef __ReceiveBuffer_h__
#define __ReceiveBuffer_h__

namespace mycp {
namespace asio {

const size_t Max_ReceiveBuffer_ReceiveSize	= 64*1024;	// 16*1024

//////////////////////////////////////////////
// ReceiveBuffer class
class ReceiveBuffer
{
public:
	typedef boost::shared_ptr<ReceiveBuffer> pointer;
	static ReceiveBuffer::pointer create(void) {return ReceiveBuffer::pointer(new ReceiveBuffer());}
	void reset(void)
	{
		m_size = 0;
		m_buffer[0] = '\0';
	}
	const unsigned char * data(void) const {return m_buffer;}
	void size(size_t newv) {m_size = newv > Max_ReceiveBuffer_ReceiveSize ? Max_ReceiveBuffer_ReceiveSize : newv; m_buffer[m_size] = '\0';}
	size_t size(void) const {return m_size;}
public:
	ReceiveBuffer(void)
		: m_size(0)
	{
		m_buffer = new unsigned char[Max_ReceiveBuffer_ReceiveSize+1];
		if (m_buffer!=NULL)
			memset(m_buffer,0,Max_ReceiveBuffer_ReceiveSize+1);
	}
	virtual ~ReceiveBuffer(void)
	{
		if (m_buffer!=NULL)
			delete[] m_buffer;
	}
private:
    unsigned char* m_buffer;//[Max_ReceiveBuffer_ReceiveSize];
	size_t m_size;
};

} // namespace asio
} // namespace mycp

#endif // __ReceiveBuffer_h__
