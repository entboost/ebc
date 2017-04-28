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
	static ReceiveBuffer::pointer create(size_t nBufferSize = Max_ReceiveBuffer_ReceiveSize) {return ReceiveBuffer::pointer(new ReceiveBuffer(nBufferSize));}
	void reset(void)
	{
		m_dataSize = 0;
		if (m_buffer!=NULL)
			m_buffer[0] = '\0';
	}
	const unsigned char * data(void) const {return m_buffer;}
	void size(size_t newv) {m_dataSize = newv > Max_ReceiveBuffer_ReceiveSize ? Max_ReceiveBuffer_ReceiveSize : newv; m_buffer[m_dataSize] = '\0';}
	size_t size(void) const {return m_dataSize;}

	unsigned int getBufferSize(void) const {return m_bufferSize;}
	bool setBufferSize(unsigned int bufferSize)
	{
		if (bufferSize == 0)
			return false;
		if (m_buffer==NULL)
		{
			m_bufferSize = bufferSize;
			m_buffer = new unsigned char[m_bufferSize];
		}else if (m_bufferSize<bufferSize)
		{
			clearData(true);
			m_bufferSize = bufferSize;
			m_buffer = new unsigned char[m_bufferSize];
		}else
		{
			m_buffer[0] = '\0';
			return true;
		}
		if (m_buffer==NULL)
		{
			m_bufferSize = 0;
			return false;
		}
		m_buffer[0] = '\0';
		return true;
	}
	void clearData(bool bDelete)
	{
		m_dataSize = 0;
		if (m_buffer != NULL)
		{
			if (bDelete)
			{
				delete m_buffer;
				m_buffer = NULL;
				m_bufferSize = 0;
			}else
			{
				m_buffer[0] = '\0';
			}
		}
	}
public:
	ReceiveBuffer(size_t nBufferSize = Max_ReceiveBuffer_ReceiveSize)
		: m_dataSize(0), m_bufferSize(0)
	{
		m_bufferSize = nBufferSize+1;
		m_buffer = new unsigned char[m_bufferSize];
		if (m_buffer!=NULL)
			memset(m_buffer,0,m_bufferSize);
	}
	virtual ~ReceiveBuffer(void)
	{
		if (m_buffer!=NULL)
			delete[] m_buffer;
	}
private:
	unsigned char* m_buffer;//[Max_ReceiveBuffer_ReceiveSize];
	size_t m_dataSize;
	size_t m_bufferSize;
};

} // namespace asio
} // namespace mycp

#endif // __ReceiveBuffer_h__
