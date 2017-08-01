// ebsharedbuffer.h file here
#ifndef __ebsharedbuffer_h__
#define __ebsharedbuffer_h__

class CEBSharedBuffer
{
public:
	typedef boost::shared_ptr<CEBSharedBuffer> pointer;
	static CEBSharedBuffer::pointer create(unsigned int nSize)
	{
		return CEBSharedBuffer::pointer(new CEBSharedBuffer(nSize));
	}

	char* buffer(void) {return m_buffer;}
	const char* buffer(void) const {return m_buffer;}
	unsigned int size(void) const {return m_nSize;}
	void Reset(void)
	{
		if (m_buffer)
			memset(m_buffer,0,sizeof(m_buffer));
	}
	CEBSharedBuffer(unsigned int nSize)
		: m_nSize(nSize)
	{
		m_buffer = new char[m_nSize];
		Reset();
	}
	CEBSharedBuffer(void)
		: m_buffer(NULL)
		, m_nSize(0)
	{
	}
	virtual ~CEBSharedBuffer(void)
	{
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}
	}
private:
	char* m_buffer;
	unsigned int m_nSize;
};
#define EB_NEW_SHARED_BUFFER(s) (CEBSharedBuffer::create(s))


#endif // __ebsharedbuffer_h__
