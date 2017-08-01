/*
    Bodb is a software library that implements a simple SQL database engine.
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

// dataheadinfo.h file here
#ifndef __dataheadinfo_h__
#define __dataheadinfo_h__

#include "../stl/stldef.h"

namespace bo
{
	class CDataHeadInfo
	{
	public:
		enum DataHeadInfoType
		{
			DHIT_NORMAL	= 0x1

			, DHIT_UNKNOWN	= 0xf
		};

		typedef boost::shared_ptr<CDataHeadInfo> pointer;
		static CDataHeadInfo::pointer create(DataHeadInfoType type, unsigned int objectid)
		{
			return CDataHeadInfo::pointer(new CDataHeadInfo(type, objectid));
		}

		//unsigned int id(void) const {return m_id;}
		DataHeadInfoType type(void) const {return m_type;}
		unsigned int objectid(void) const {return m_objectid;}
		unsigned short	count(void) const {return m_count;}

		static unsigned short getdataheadinfoSize(void)
		{
			return sizeof(DataHeadInfoType)		// m_type
				+ sizeof(unsigned int)			// m_objectid
				+ sizeof(unsigned short);		// m_count
		}
	public:
		CDataHeadInfo(void)
			: m_type(DHIT_UNKNOWN), m_objectid(0)
			, m_count(0)
		{}
		CDataHeadInfo(DataHeadInfoType type, unsigned int objectid)
			: m_type(type), m_objectid(objectid)
			, m_count(0)
		{}
		virtual ~CDataHeadInfo(void)
		{}

		virtual void Serialize(bool isStoring, tfstream& ar)
		{
			if (isStoring)
			{
				// m_id
				//ar.write((const char*)&m_id, sizeof(m_id));

				// m_type
				ar.write((const char*)&m_type, sizeof(m_type));
				// m_objectid
				ar.write((const char*)&m_objectid, sizeof(m_objectid));
				// m_count
				ar.write((const char*)&m_count, sizeof(m_count));
			}else
			{
				// m_id
				//ar.read((char*)&m_id, sizeof(m_id));

				// m_type
				ar.read((char*)&m_type, sizeof(m_type));
				// m_objectid
				ar.read((char*)&m_objectid, sizeof(m_objectid));
				// m_count
				ar.read((char*)&m_count, sizeof(m_count));
			}
			
		}
	private:
		//unsigned int	m_id;
		DataHeadInfoType	m_type;
		unsigned int	m_objectid;
		unsigned short	m_count;
	};

} // namespace bo

#endif // __dataheadinfo_h__

