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

// database.h file here
#ifndef __database_h__
#define __database_h__

#include <boost/thread.hpp>
#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include "databaseinfo.h"
#include "resultset.h"
#include "pageheadinfo.h"
#include "modifyinfo.h"
#include "fieldcompare.h"
#include "bobase.h"
//#include "fielddois.h"

//const tstring ConstSystemTable1_Name	= "T$SYS_TABLEINFO";
//const tstring ConstSystemTable1_Field1	= "F$SYS_TABLEID";
//const tstring ConstSystemTable1_Field2	= "F$SYS_RECORDID";

namespace bo
{
	class CSelectInfo
	{
	public:
		CTableInfo::pointer m_pFirstTableInfo;
		std::list<std::list<CFieldCompare::pointer> > wheres;
		CLockMap<void*,CTableInfo::pointer> m_pOutPutTableInfoList;
		CSelectInfo(const CTableInfo::pointer& pFirstTableInfo)
			: m_pFirstTableInfo(pFirstTableInfo)
		{}
		CSelectInfo(void)
		{}
	};

	class CDatabase
	{
	public:
		typedef boost::shared_ptr<CDatabase> pointer;
		static CDatabase::pointer create(const CDatabaseInfo::pointer& dbinfo);

		void setpath(const tstring & path);
		const tstring & getpath(void) const;
		bool isopened(void) const;
		bool open(void);
		void close(void);
		std::string getDbName(void) const {return m_dbinfo.get() == 0 ? "" : m_dbinfo->name();}
		bool setOption(OptionType nOption, bo::uinteger nValue);
		bo::uinteger getOption(OptionType nOption) const;

		bool createTable(const CTableInfo::pointer&);
		bool dropTable(const tstring & tablename);
		bool createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo);
		bool dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname);
		bool modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo);
		//CTableInfo::pointer createTable(const tstring & tableName);
		CTableInfo::pointer getTableInfo(const tstring & tableName);
		//CFieldInfo::pointer createField(CTableInfo::pointer tableInfo, const tstring & fieldName, FieldType fieldtype, uinteger len=0);

		bool rename(const tstring & tableName, const tstring & newName);
		bool update(const tstring & tableName);
		bool dropdefault(const tstring & tableName, const tstring & fieldName);
		bool setdefault(const tstring & tableName, const tstring & fieldName, tagItemValue * defaultValue);

		bool insert(const CRecordLine::pointer& recordLine);

		//int deleters(CTableInfo::pointer tableInfo, const std::list<CFieldCompare::pointer> & wheres);
		int deleters(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres);

		CResultSet::pointer select(const tstring & tableName);
		CResultSet::pointer select(const CSelectInfo& pSelectInfo, bool distinct);
		//CResultSet::pointer select(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, bool distinct);

		//int update(CTableInfo::pointer tableInfo, const std::list<CFieldCompare::pointer> & wheres, CRecordLine::pointer updateVal);
		int update(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CRecordLine::pointer& updateVal);
		//CDatabaseInfo::pointer databaseinfo(void) const {return m_dbinfo;}

	public:
		void do_proc_db(void);

		CDatabase(void);
		CDatabase(const CDatabaseInfo::pointer& dbinfo);
		virtual ~CDatabase(void);

	private:
		static void proc_db(CDatabase * owner);

		void loadDbfile(uinteger pht);

		void writeNewBlackPage(tfstream& ar) const;
		void writeBlackNull(tfstream& ar, usmallint size) const;
		void writeData2(tfstream& ar, const CRecordLine::pointer& recordLine, CModifyInfo::ModifyInfoFlag modifyFlag);

		//CPageHeadInfo::pointer findPage(usmallint needSize);
		//CPageHeadInfo::pointer findInfoPage(usmallint needSize);
		CPageHeadInfo::pointer findTableInfoPage(usmallint needSize);
		CPageHeadInfo::pointer findFieldInfoPage(usmallint needSize);
		void UnUsePage(const CPageHeadInfo::pointer& pageHeadInfo);

	private:
		CDatabaseInfo::pointer	m_dbinfo;
		tstring m_path;
		bool m_killed;
		boost::thread * m_proc;
		tfstream	m_fdb;
		bool		m_bLoadError;
		bool		m_bFullMemory;

		// resultset
		CLockMap<void*, CResultSet::pointer> m_results;	// CTableInfo*
		//CLockMap<uinteger, CRecordLine::pointer> m_records;

		// Modifys
		CLockList<CModifyInfo::pointer> m_modifys;

		// Pages
		//CLockList<CPageHeadInfo::pointer> m_pages;
		uinteger m_curpageid;
		CLockList<CPageHeadInfo::pointer> m_tableinfopages;
		CLockList<CPageHeadInfo::pointer> m_fieldinfopages;
		CLockList<CPageHeadInfo::pointer> m_unusepages;
		CLockMap<uinteger, CPageHeadInfo::pointer> m_tablepages;	// TableId ->

		//CLockMap<uinteger, CFieldDOIs::pointer>	m_record

		//CTableInfo::pointer m_sysTableRecordId;
	};

} // namespace bo

#endif // __database_h__

