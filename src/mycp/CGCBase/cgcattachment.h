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

// cgcattachment.h file here
#ifndef __cgcattachment_head__
#define __cgcattachment_head__

#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>

namespace mycp {

#ifdef WIN32
	typedef __int64				bigint;
#define cgc_atoi64(a) _atoi64(a)
#else
	typedef long long			bigint;
#define cgc_atoi64(a) atoll(a)
#endif

// Attachment For SOTP/2.0
class cgcAttachment
{
protected:
	tstring m_name;	// name
	bigint m_total;
	bigint m_index;
	unsigned int m_len;
	unsigned char * m_data;
private:
	unsigned int m_bufferSize;
public:
	typedef boost::shared_ptr<cgcAttachment> pointer;
	static cgcAttachment::pointer create(void)
	{
		return cgcAttachment::pointer(new cgcAttachment());
	}
	static cgcAttachment::pointer create(const cgcAttachment* p)
	{
		return cgcAttachment::pointer(new cgcAttachment(p));
	}
	static cgcAttachment::pointer create(const cgcAttachment::pointer& p)
	{
		return cgcAttachment::pointer(new cgcAttachment(p.get()));
	}

	cgcAttachment(void)
		: m_name(_T(""))
		, m_total(0)
		, m_index(0)
		, m_len(0)
		, m_data(0), m_bufferSize(0)
	{
	}
	cgcAttachment(const cgcAttachment* p)
		: m_name(_T(""))
		, m_total(0)
		, m_index(0)
		, m_len(0)
		, m_data(0), m_bufferSize(0)
	{
		this->operator = (p);
	}
	virtual ~cgcAttachment(void)
	{
		clear(true);
	}
	const cgcAttachment& operator = (const cgcAttachment& p) {return this->operator = (&p);}
	const cgcAttachment& operator = (const cgcAttachment::pointer& p) {return this->operator = (p.get());}
	const cgcAttachment& operator = (const cgcAttachment* p)
	{
		if (p!=NULL)
		{
			m_name = p->m_name;
			m_total = p->m_total;
			m_index = p->m_index;
			setAttach(p->m_data,p->m_len);
			//m_len = p->m_len;
			//// m_data
			//m_bufferSize = p->m_bufferSize;
			//if (m_bufferSize>0 && p->m_data!=NULL)
			//{
			//	m_data = new unsigned char[m_bufferSize];
			//}
		}
		return *this;
	}
	cgcAttachment::pointer copyNew(void) const {return cgcAttachment::create(this);}
public:
	bool isHasAttach(void) const {return m_data != 0 && m_len > 0;}

	// name
	void setName(const tstring & newValue) {m_name = newValue;}
	const tstring & getName(void) const {return m_name;}
	// total
	void setTotal(bigint newValue) {m_total = newValue;}
	bigint getTotal(void) const {return m_total;}
	// index
	void setIndex(bigint newValue) {m_index = newValue;}
	bigint getIndex(void) const {return m_index;}
	// len
	void setAttach(const unsigned char * pAttachData, unsigned int nAttachSize)
	{
		if (pAttachData == NULL || nAttachSize == 0)
		{
			clear(false);
			return;
		}
		if (m_bufferSize < nAttachSize)
		{
			clearAttachData(true);
			m_bufferSize = nAttachSize+1;
			m_data = new unsigned char[m_bufferSize];
			if (m_data==NULL)
			{
				// memory error
				clearAttachData(true);
				return;
			}
			memset(m_data, 0, m_bufferSize);
		}
		m_len = nAttachSize;
		if (m_data!=pAttachData)
			memcpy(m_data, pAttachData, m_len);
	}
	void setAttach2(unsigned char * pAttachData, unsigned int nAttachSize)
	{
		clearAttachData(true);
		if (pAttachData == NULL || nAttachSize == 0)
		{
			return;
		}
		m_len = nAttachSize;
		m_bufferSize = nAttachSize;
		m_data = pAttachData;
	}
	const unsigned char * getAttachData(void) const {return m_data;}
	unsigned char * getAttachData(unsigned int& pOutAttachSize)
	{
		unsigned char * result = m_data;
		pOutAttachSize = m_len;
		m_data = 0;
		m_bufferSize = 0;
		m_len = 0;
		return result;
	}
	unsigned int getAttachSize(void) const {return m_len;}
	unsigned int getBufferSize(void) const {return m_bufferSize;}

	void clear(bool bClearBuffer = true)
	{
		clearAttachData(bClearBuffer);
		m_name = _T("");
		m_total = 0;
		m_index = 0;
	}
	void rebuildBuffer(unsigned int nBufferSize, bool bMemset = true)
	{
		if (m_data==NULL)
		{
			m_bufferSize = nBufferSize;
			m_data = new unsigned char[m_bufferSize];
		}else if (m_bufferSize < nBufferSize)
		{
			clearAttachData(true);
			m_bufferSize = nBufferSize;
			m_data = new unsigned char[m_bufferSize];
		}
		if (bMemset)
			memset(m_data, 0, nBufferSize);
	}
protected:
	void clearAttachData(bool bClearBuffer)
	{
		m_len = 0;
		if (bClearBuffer)
		{
			m_bufferSize = 0;
			if (m_data)
			{
				delete[] m_data;
				m_data = NULL;
			}
		}
	}
};
const cgcAttachment::pointer NullcgcAttachment;

} // namespace mycp

#endif // __cgcattachment_head__
