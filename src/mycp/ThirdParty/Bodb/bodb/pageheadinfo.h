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

// pageheadinfo.h file here
#ifndef __pageheadinfo_h__
#define __pageheadinfo_h__

#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include <boost/shared_ptr.hpp>
#include "bodbdef.h"

namespace bo
{
	const usmallint MAX_PAGEHEADINFO_SIZE =
		INTEGER_SIZE							// m_id
		+ 4										// m_type
		+ INTEGER_SIZE							// m_subtype
		+ INTEGER_SIZE							// m_objectid
		+ SMALLINT_SIZE							// m_count
		+ SMALLINT_SIZE							// m_unused
		;

	class CPageHeadInfo
	{
	public:
		enum PageHeadType
		{
			PHT_INFO		= 0x1
			, PHT_DATA		= 0x2
			, PHT_DATA2		= 0x4

			, PHT_UNKNOWN	= 0xf
		};
		enum PageHeadSubType1
		{
			PST_8000		= 0x1
			, PST_1000
			, PST_200
			, PST_50
			, PST_20

		};
		enum PageHeadSubType2
		{
			PST_TABLEINFO	= 0x1
			, PST_FIELDINFO
		};

		typedef boost::shared_ptr<CPageHeadInfo> pointer;
		static CPageHeadInfo::pointer create(void);
		static CPageHeadInfo::pointer create(uinteger id, PageHeadType type, uinteger objectid);

		void id(uinteger newv) {m_id = newv;}
		uinteger id(void) const {return m_id;}
		void type(PageHeadType newv) {m_type = newv;}
		PageHeadType type(void) const {return m_type;}

		void subtype(uinteger newv) {m_subtype = newv;}
		uinteger subtype(void) const {return m_subtype;}

		void objectid(uinteger newv) {m_objectid = newv;}
		uinteger objectid(void) const {return m_objectid;}

		usmallint	count(void) const {return m_count;}
		void icount(void) {++m_count;}
		void dcount(void) {--m_count;}
		void unused(usmallint newv) {m_unused = newv;}
		void dunused(usmallint dv) {m_unused -= dv;}
		void iunused(usmallint iv) {m_unused += iv;}
		usmallint	unused(void) const {return m_unused;}

		void reset(void);

		//static usmallint getPageHeadInfoSize(void);

	public:
		CLockMap<uinteger, usmallint> m_idindexs;	// Id -> index

	public:
		CPageHeadInfo(void);
		CPageHeadInfo(uinteger id, PageHeadType type, uinteger objectid);
		virtual ~CPageHeadInfo(void);

		virtual void Serialize(bool isStoring, tfstream& ar);
	private:
		uinteger	m_id;
		PageHeadType	m_type;
		uinteger	m_subtype;
		uinteger	m_objectid;
		usmallint	m_count;
		usmallint	m_unused;
	};

	const CPageHeadInfo::pointer boNullPageHeadInfo;
} // namespace bo

#endif // __pageheadinfo_h__

