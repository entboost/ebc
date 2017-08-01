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

// databaseinfo.h file here
#ifndef __databaseinfo_h__
#define __databaseinfo_h__

#include "../stl/stldef.h"
#include "../stl/locklist.h"
#include "tableinfo.h"

namespace bo
{
	const usmallint MAX_DATABASEINFO_SIZE =
		INTEGER_SIZE * 2						// m_currentTableId + m_currentFieldId
		+ 4										// m_type
		+ SMALLINT_SIZE+MAX_NAME_SIZE			// m_name
		+ SMALLINT_SIZE+MAX_NOTE_SIZE			// m_note
		;

	class CDatabaseInfo
	{
	public:
		enum DatabaseType
		{
			DT_Normal = 0x1
			//, DT_x

		};

		typedef boost::shared_ptr<CDatabaseInfo> pointer;
		static CDatabaseInfo::pointer create(const tstring & name, DatabaseType type = DT_Normal, const tstring & note = _T(""));

		bool createTable(const CTableInfo::pointer& tableInfo);
		bool createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo);
		CFieldInfo::pointer dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname);
		bool modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo);
		//CTableInfo::pointer createTable(const tstring & tableName);
		//CFieldInfo::pointer createField(CTableInfo::pointer tableInfo, const tstring & fieldName, FieldType fieldtype, uinteger len);

		bool existTable(const tstring & tablename);
		CTableInfo::pointer getTableInfo(uinteger tableid);
		CTableInfo::pointer getTableInfo(const tstring & tablename, bool erase);
		const CLockMap<tstring, CTableInfo::pointer>& tables(void) const {return m_tables;}

		//void name(const tstring & newv) {m_name = newv;}
		const tstring & name(void) const {return m_name;}
		DatabaseType type(void) const {return m_type;}
		void note(const tstring & newv) {m_note = newv;}
		const tstring & note(void) const {return m_note;}

		//static usmallint getMaxDatabaseInfoSize(void);

		uinteger getTableId(void) const {return m_currentTableId;}
		void setTableId(uinteger v) {m_currentTableId = v;}
		uinteger getNextTableId(void) {return ++m_currentTableId;}
		uinteger getFieldId(void) const {return m_currentFieldId;}
		void setFieldId(uinteger v) {m_currentFieldId = v;}
		uinteger getNextFieldId(void) {return ++m_currentFieldId;}

	public:
		CDatabaseInfo(const tstring & name, DatabaseType type, const tstring & note);
		virtual ~CDatabaseInfo(void);

		virtual void Serialize(bool isStoring, tfstream& ar);
	private:
		DatabaseType	m_type;
		tstring			m_name;
		tstring			m_note;
		CLockMap<tstring, CTableInfo::pointer>		m_tables;	// Table Name ->
		CLockMap<uinteger, CTableInfo::pointer> m_tables2;	// Table Id ->

		// id
		uinteger	m_currentTableId;
		uinteger	m_currentFieldId;

	};

	const CDatabaseInfo::pointer boNullDatabaseInfo;

} // namespace bo

#endif // __databaseinfo_h__

