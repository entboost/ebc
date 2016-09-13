// buffer.h file here
#ifndef __buffer_h__
#define __buffer_h__

#include <boost/shared_ptr.hpp>

class CBuffer
{
public:
	typedef boost::shared_ptr<CBuffer> pointer;
	static CBuffer::pointer create(void){
		return CBuffer::pointer(new CBuffer(0, 0));
	}
	static CBuffer::pointer create(const void * buffer, long size){
		return CBuffer::pointer(new CBuffer(buffer, size));
	}

	// Return the old size.
	long remalloc(long newsize)
	{
		long result = m_size;
		if (newsize > m_size)
		{
			void * tempbuffer = new void*[newsize];
			if (m_buffer != 0 && m_size > 0)
			{
				memcpy(tempbuffer, m_buffer, m_size);
				delete[] m_buffer;
			}
			m_buffer = tempbuffer;
			m_size = newsize;
		}
		return result;
	}
	void set(const void * buffer, long size)
	{
		remalloc(size);

		if (buffer != 0 && size > 0)
		{
			memcpy(m_buffer, buffer, size);
		}
	}

	const void * buffer(void) const {return m_buffer;}
	long size(void) const {return m_size;}

public:
	CBuffer(const void * buffer, long size)
		: m_buffer(0), m_size(size)
	{
		if (buffer && m_size > 0)
		{
			m_buffer = new void*[m_size];
			memcpy(m_buffer, buffer, m_size);
		}
	}
	~CBuffer(void){
		if (m_buffer)
		{
			delete[] m_buffer;
		}
	}

private:
	void *	m_buffer;
	long	m_size;
};

#endif // __buffer_h__
