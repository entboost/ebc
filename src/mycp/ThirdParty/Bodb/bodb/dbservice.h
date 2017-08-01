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

// dbservice.h file here
#ifndef __dbservice_h__
#define __dbservice_h__

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <stl/stldef.h>
#include <stl/lockmap.h>
#include "bodbhandler.h"

namespace bo
{
	class CDbService
		: public CBodbHandler
	{
	public:
		typedef boost::shared_ptr<CDbService> pointer;
		static CDbService::pointer create(void)
		{
			return CDbService::pointer(new CDbService());
		}

		void setpath(const tstring & path = "");
		virtual bool start(void);
		virtual bool isstarted(void) const;
		virtual void stop(void);

		virtual int execsql(const char * sql, PRESULTSET * outResultSet = 0) {return 0;}

		virtual CDatabase::pointer create(const tstring & dbname);
		virtual void setaccount(const tstring & account, const tstring & password);
		virtual bool use(const tstring & dbname);
		virtual bool isopen(void) const;
		virtual void close(void);
		virtual bool exist(const tstring & dbname) const;
		virtual bool drop(const tstring & dbname);

		virtual bool rename(const tstring & tableName, const tstring & newName);
		virtual bool dropdefault(const tstring & tableName, const tstring & fieldName);
		virtual bool setdefault(const tstring & tableName, const tstring & fieldName, tagItemValue * defaultValue);

		virtual bool setoption(OptionType nOption, bo::uinteger nValue);

		//CDatabase::pointer opendb(const tstring & dbname);

		CTableInfo::pointer getTableInfo(const tstring & tablename);

		CResultSet::pointer select(const tstring & tableName);
		CResultSet::pointer select(const CSelectInfo& pSelectInfo, bool distinct);
		//CResultSet::pointer select(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CLockMap<void*,CTableInfo::pointer>& pOutPutTableInfoList, bool distinct);

		bool insert(const CRecordLine::pointer& recordLine);
		int update(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CRecordLine::pointer& updateVal);
		int deleters(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres);

		bool createTable(const CTableInfo::pointer& tableInfo);
		bool dropTable(const tstring & tablename);
		bool createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo);
		bool dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname);
		bool modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo);

		CDbService(void);
		virtual ~CDbService(void);

	private:
		void create_default_table(void);

	private:
		bool m_started;
		tstring m_account;
		tstring m_password;
		tstring m_path;
		CLockMap<tstring, CDatabase::pointer> m_databases;
		CDatabase::pointer m_curdb;
	};

	const CDbService::pointer boNullDbService;

} // namespace bo

#endif // __dbservice_h__

