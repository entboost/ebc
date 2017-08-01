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

#include "pageheadinfo.h"
#include "bodbdef.h"

namespace bo
{
	CPageHeadInfo::pointer CPageHeadInfo::create(void)
	{
		return CPageHeadInfo::pointer(new CPageHeadInfo());
	}
	CPageHeadInfo::pointer CPageHeadInfo::create(uinteger id, PageHeadType type, uinteger objectid)
	{
		return CPageHeadInfo::pointer(new CPageHeadInfo(id, type, objectid));
	}

	void CPageHeadInfo::reset(void)
	{
		m_type = PHT_UNKNOWN;
		m_subtype = 0;
		m_objectid = 0;
		m_count = 0;
		m_unused = PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE;
		m_idindexs.clear();
	}

	//usmallint CPageHeadInfo::getPageHeadInfoSize(void)
	//{
	//	return sizeof(uinteger)		// m_id
	//		+ sizeof(PageHeadType)		// m_type
	//		+ sizeof(uinteger)		// m_objectid
	//		+ sizeof(usmallint)	// m_count
	//		+ sizeof(usmallint)	// m_unused
	//		;
	//}

	CPageHeadInfo::CPageHeadInfo(void)
		: m_id(0), m_type(PHT_UNKNOWN), m_subtype(0), m_objectid(0)
		, m_count(0), m_unused(PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE)
	{}
	CPageHeadInfo::CPageHeadInfo(uinteger id, PageHeadType type, uinteger objectid)
		: m_id(id), m_type(type), m_subtype(0), m_objectid(objectid)
		, m_count(0), m_unused(PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE)
	{}
	CPageHeadInfo::~CPageHeadInfo(void)
	{
		m_idindexs.clear();
	}

	void CPageHeadInfo::Serialize(bool isStoring, tfstream& ar)
	{
		ar.clear();
		if (isStoring)
		{
			// m_id
			ar.write((const char*)&m_id, INTEGER_SIZE);

			// m_type
			ar.write((const char*)&m_type, 4);
			// m_subtype
			ar.write((const char*)&m_subtype, INTEGER_SIZE);
			// m_objectid
			ar.write((const char*)&m_objectid, INTEGER_SIZE);
			// m_count
			ar.write((const char*)&m_count, SMALLINT_SIZE);
			// m_unused
			ar.write((const char*)&m_unused, SMALLINT_SIZE);
		}else
		{
			// m_id
			ar.read((char*)&m_id, INTEGER_SIZE);

			// m_type
			ar.read((char*)&m_type, 4);
			// m_subtype
			ar.read((char*)&m_subtype, INTEGER_SIZE);
			// m_objectid
			ar.read((char*)&m_objectid, INTEGER_SIZE);
			// m_count
			ar.read((char*)&m_count, SMALLINT_SIZE);
			// m_unused
			ar.read((char*)&m_unused, SMALLINT_SIZE);
		}

	}


} // namespace bo
