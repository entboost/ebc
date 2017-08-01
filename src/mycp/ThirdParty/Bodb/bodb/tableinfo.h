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

// tableinfo.h file here
#ifndef __tableinfo_h__
#define __tableinfo_h__

#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include "fieldinfo.h"
//#include "recordline.h"
#include "pageheadinfo.h"
//#include "fielddois.h"
#include "recorddois.h"

namespace bo
{
	const usmallint MAX_TABLEINFO_SIZE =
		INTEGER_SIZE							// m_currentRecordId
		+ INTEGER_SIZE							// m_id
		+ 4										// m_type
		+ SMALLINT_SIZE+MAX_NAME_SIZE			// m_name
		+ SMALLINT_SIZE+MAX_NOTE_SIZE			// m_note
		;

	class CTableInfo
	{
	public:
		enum TableType
		{
			TT_NORMAL	= 0x1
			, TT_SYSTEM = 0x2

			, TT_UNKNOWN	= 0xf
		};

		typedef boost::shared_ptr<CTableInfo> pointer;
		static CTableInfo::pointer create(void);
		static CTableInfo::pointer create(const tstring & name, TableType type = TT_NORMAL, const tstring & note = _T(""));

		bool createField(const CFieldInfo::pointer& fieldInfo);
		CFieldInfo::pointer dropField(const tstring& fieldname);
		bool modifyField(const tstring& fieldname,const CFieldInfo::pointer& fieldInfo);

		bool existField(const tstring & fieldname) const;
		CFieldInfo::pointer getFieldInfo(uinteger fieldid);
		CFieldInfo::pointer getFieldInfo(const tstring & fieldname);
		CFieldInfo::pointer getFieldInfo2(short fieldindex);

		//CFieldInfo::pointer moveFirst(void);
		//CFieldInfo::pointer moveNext(void);
		//CFieldInfo::pointer movePrev(void);
		//CFieldInfo::pointer moveLast(void);

		const CLockList<CFieldInfo::pointer> & fields(void) const {return m_fields;}

		void id(uinteger newv) {m_id = newv;}
		uinteger id(void) const {return m_id;}
		void name(const tstring & newv) {m_name = newv;}
		const tstring & name(void) const {return m_name;}
		TableType type(void) const {return m_type;}
		void type(TableType  newv) {m_type = newv;}
		void note(const tstring & newv) {m_note = newv;}
		const tstring & note(void) const {return m_note;}

		uinteger getNextRecordId(void) {return ++m_currentRecordId;}
		void resetRecordId(void) {m_currentRecordId = 0;}

		//static usmallint getMaxTableInfoSize(void);
		
		usmallint getMaxTableFieldSize(void) const;
		usmallint getMaxFieldInfoSize(void) const;

		CPageHeadInfo::pointer findDataPage(usmallint needSize);
		CPageHeadInfo::pointer findData2Page(usmallint needSize, CPageHeadInfo::PageHeadSubType1 subtype);
		CPageHeadInfo::pointer findFieldInfoPage(usmallint needSize);

	public:
		CLockMap<uinteger, CPageHeadInfo::pointer> m_datapages;		// PageId ->
		CLockMap<uinteger, CPageHeadInfo::pointer> m_data2pages;	// PageId ->
		CLockList<CPageHeadInfo::pointer> m_fieldinfopages;
		CLockMap<uinteger, CPageHeadInfo::pointer> m_rlpages;	// RecordLineId ->
		CLockMap<uinteger, CPageHeadInfo::pointer> m_fipages;	// FieldInfoId ->

		CLockMap<uinteger, CRecordDOIs::pointer>	m_recordfdois;	// RecordLineId ->

	public:
		CTableInfo(void);
		CTableInfo(const tstring & name, TableType type, const tstring & note);
		virtual ~CTableInfo(void);

		virtual void Serialize(bool isStoring, tfstream& ar);
	private:
		uinteger	m_id;
		TableType	m_type;
		tstring		m_name;
		tstring		m_note;
		CLockList<CFieldInfo::pointer> m_fields;
		CLockMap<tstring, CFieldInfo::pointer> m_fields2;		// Field Name ->
		CLockMap<uinteger, CFieldInfo::pointer> m_fields3;	// Field Id ->
		//CLockList<CFieldInfo::pointer>::iterator m_curiter;

		// id
		uinteger m_currentRecordId;
	};

	const CTableInfo::pointer boNullTableInfo;

} // namespace bo

#endif // __tableinfo_h__

