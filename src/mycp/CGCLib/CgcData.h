/*
    MYCP is a HTTP and C++ Web Application Server.
    Copyright (C) 2009-2010  Akee Yang <akee.yang@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// CgcData.h file here
#ifndef __CgcData_h__
#define __CgcData_h__

#include "boost/shared_ptr.hpp"

#ifndef NULL
#define NULL 0
#endif
class CCgcData
{
public:
	typedef boost::shared_ptr<CCgcData> pointer;

	static CCgcData::pointer create(void)
	{
		return CCgcData::pointer(new CCgcData());
	}
	static CCgcData::pointer create(const unsigned char * data, size_t size)
	{
		return CCgcData::pointer(new CCgcData(data, size));
	}

public:
	void putdata(unsigned char * data, size_t size) {clear(); m_data = data; m_size = size;}
	void setdata(const unsigned char * data, size_t size) {setnew(data, size);}
	const unsigned char * data(void) const {return m_data;}
	size_t size(void) const {return m_size;}
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
	void setnew(const unsigned char * data, size_t size)
	{
		clear();
		m_size = size;
		if (data != NULL && m_size > 0)
		{
			m_data = new unsigned char[m_size+1];
			memcpy(m_data, data, m_size);
			m_data[m_size] = '\0';
		}
	}
public:
	CCgcData(void)
		: m_data(NULL), m_size(0)
		, m_attach(NULL)
	{}
	CCgcData(const unsigned char * data, size_t size)
		: m_data(NULL), m_size(size)
		, m_attach(NULL)
	{
		setnew(data, size);
	}
	virtual ~CCgcData(void)
	{
		clear();
	}


private:
	unsigned char * m_data;
	size_t	m_size;
	void *	m_attach;
};


#endif // __CgcData_h__
