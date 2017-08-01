// StreamData.h file here
#ifndef __StreamData_h__
#define __StreamData_h__

#include "boost/shared_ptr.hpp"

#ifndef NULL
#define NULL 0
#endif
class LIBDS_CLASS CStreamData
{
public:
	typedef boost::shared_ptr<CStreamData> pointer;

	static CStreamData::pointer create(void)
	{
		return CStreamData::pointer(new CStreamData());
	}
	static CStreamData::pointer create(const unsigned char * data, unsigned int size)
	{
		return CStreamData::pointer(new CStreamData(data, size));
	}

public:
	void putdata(unsigned char * data, unsigned int size) {clear(); m_data = data; m_size = size;}
	void setdata(const unsigned char * data, unsigned int size) {setnew(data, size);}
	const unsigned char * data(void) const {return m_data;}
	unsigned int size(void) const {return m_size;}
	void attach(void * newv) {m_attach = newv;}
	void * attach(void) const {return m_attach;}

private:
	void clear(void)
	{
		m_size = 0;
		if (m_data != NULL)
		{
			delete[] m_data;
			m_data = NULL;
		}
	}
	void setnew(const unsigned char * data, unsigned int size)
	{
		clear();
		m_size = size;
		if (data != NULL && m_size > 0)
		{
			m_data = new unsigned char[m_size];
			memcpy(m_data, data, m_size);
		}
	}
public:
	CStreamData(void)
		: m_data(NULL), m_size(0)
		, m_attach(NULL)
	{}
	CStreamData(const unsigned char * data, unsigned int size)
		: m_data(NULL), m_size(size)
		, m_attach(NULL)
	{
		setnew(data, size);
	}
	virtual ~CStreamData(void)
	{
		clear();
	}


private:
	unsigned char * m_data;
	unsigned int	m_size;
	void *			m_attach;
};


#endif // __StreamData_h__
