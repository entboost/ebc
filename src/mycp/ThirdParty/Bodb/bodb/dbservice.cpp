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

#ifdef WIN32
#pragma warning(disable:4267)
#endif //WIN32

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "dbservice.h"
#ifdef _UNICODE
typedef boost::filesystem::wpath boosttpath;
#else
typedef boost::filesystem::path boosttpath;
#endif // _UNICODE

namespace bo
{
	void CDbService::setpath(const tstring & path)
	{
		m_path = path;

		namespace fs = boost::filesystem;

		if (m_path.empty())
		{
			fs::path currentPath(fs::initial_path());
			m_path = currentPath.string();
		}
		boosttpath pathRoot(m_path.string());
		//boosttpath pathRoot(m_path, fs::native);
		if (!boost::filesystem::exists(pathRoot))
		{
			fs::create_directory(pathRoot);
		}
	}
//#define USES_DEBUG
#ifdef USES_DEBUG
	FILE * theLogFile = NULL;
#endif
	bool CDbService::start(void)
	{
		if (m_path.empty()) return false;
		if (m_started) return true;

#ifdef USES_DEBUG
		if (theLogFile == NULL)
			theLogFile = fopen("d:\\entboost_log.txt","w+");
		char lpszBuffer[1024];
		sprintf(lpszBuffer,"path=%s\n",m_path.c_str());
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif

		namespace fs = boost::filesystem;
		boosttpath pathRoot(m_path.string());
		//boosttpath pathRoot(m_path, fs::native);
		if (!boost::filesystem::is_directory(pathRoot) ||
			!boost::filesystem::exists(pathRoot))
		{
			return false;
		}
		fs::directory_iterator dir_iter(pathRoot),dir_end;
		for(;dir_iter !=dir_end;++dir_iter)
		{
			if(fs::is_directory(*dir_iter))
			{
				//const boosttpath & dbPath = *dir_iter;
				const tstring dbname = dir_iter->path().leaf().string();
#ifdef USES_DEBUG
				sprintf(lpszBuffer,"dbname=%s\n",dbname.c_str());
				fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
				CDatabase::pointer database;
				if (!m_databases.find(dbname, database))
				{
					CDatabaseInfo::pointer dbinfo = CDatabaseInfo::create(dbname);
					database = CDatabase::create(dbinfo);
					database->setpath(m_path);
					m_databases.insert(dbname, database);
				}
			}
			//std::cout<<(*dir_iter).leaf();
		}
		// ??
		//boosttpath pathXmlFile(xmlFile, fs::native);
		//if (boost::filesystem::exists(pathXmlFile))
		//if (fs::ne
		//{

		//}

#ifdef USES_DEBUG
		sprintf(lpszBuffer,"db isze=%d\n",m_databases.size());
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
		m_started = true;
		return true;
	}
	bool CDbService::isstarted(void) const
	{
		return m_started;
	}
	
	void CDbService::stop(void)
	{
#ifdef USES_DEBUG
		if (theLogFile)
		{
			fclose(theLogFile);
			theLogFile = NULL;
		}
#endif
		if (m_started)
		{
			m_started = false;
			{
				CLockMap<tstring, CDatabase::pointer>::const_iterator iter;
				AUTO_WLOCK(m_databases);
				for (iter=m_databases.begin(); iter!=m_databases.end(); iter++)
				{
					iter->second->close();
				}
				m_databases.clear(false);
			}
			close();
		}
	}

	CDatabase::pointer CDbService::create(const tstring & dbname)
	{
		CDatabase::pointer database;
		if (m_databases.find(dbname, database))
		{
			return database;
		}

		if (m_path.empty()) return database;
		if (!m_started) return database;

		namespace fs = boost::filesystem;
		boosttpath pathRoot(m_path.string());
		//boosttpath pathRoot(m_path, fs::native);
		if (!boost::filesystem::is_directory(pathRoot) ||
			!boost::filesystem::exists(pathRoot))
			return database;

		// Create database directory.
		tstring dbfile(m_path);
		dbfile.append(_T("/"));
		dbfile.append(dbname);
		boosttpath pathDatabase(dbfile.string());
		//boosttpath pathDatabase(dbfile, fs::native);
		if (fs::exists(pathDatabase))
			return database;
		if (!fs::create_directory(pathDatabase))
		{
			return database;
		}

		CDatabaseInfo::pointer dbinfo = CDatabaseInfo::create(dbname);
		database = CDatabase::create(dbinfo);

		//// Create T$SYS_TABLEINFO Table
		//CTableInfo::pointer tableInfo = CTableInfo::create(ConstSystemTable1_Name);
		//database->createTable(tableInfo);
		//database->createField(tableInfo, CFieldInfo::create(ConstSystemTable1_Field1, bo::FT_UINTEGER));
		//database->createField(tableInfo, CFieldInfo::create(ConstSystemTable1_Field2, bo::FT_UINTEGER));

		database->setpath(m_path);
		m_databases.insert(dbname, database);

		return database;
	}

	void CDbService::create_default_table(void)
	{
		CTableInfo::pointer tableInfo = getTableInfo("bodb_sys_account");
		if (tableInfo.get() == NULL)
		{
			tableInfo = CTableInfo::create("bodb_sys_account", CTableInfo::TT_SYSTEM);
			if (CDbService::createTable(tableInfo))
			{
				CDbService::createField(tableInfo, CFieldInfo::create("account", bo::FT_VARCHAR, 64));
				CDbService::createField(tableInfo, CFieldInfo::create("password", bo::FT_VARCHAR, 64));
				CDbService::createField(tableInfo, CFieldInfo::create("status", bo::FT_INTEGER));
				CRecordLine::pointer recordLine = CRecordLine::create(tableInfo);
				if (m_account.empty())
				{
					m_account = "system";
				}
				recordLine->addVariant("account", m_account.c_str(), m_account.size());
				recordLine->addVariant("password", m_password.c_str(), m_password.size());
				CDbService::insert(recordLine);
			}
		}else if (tableInfo->type()!=CTableInfo::TT_SYSTEM)
		{
			// ?
			tableInfo->type(CTableInfo::TT_SYSTEM);
			if (isopen())
				m_curdb->update("bodb_sys_account");
		}

		tableInfo = getTableInfo("bodb_sys_option");
		if (tableInfo.get() == NULL)
		{
			tableInfo = CTableInfo::create("bodb_sys_option", CTableInfo::TT_SYSTEM);
			if (CDbService::createTable(tableInfo))
			{
				CFieldInfo::pointer pFieldInfo = CFieldInfo::create("option", bo::FT_INTEGER);
				pFieldInfo->constraintType(FIELD_CONSTRAINT_PRIMARYKEY);
				CDbService::createField(tableInfo, pFieldInfo);
				CDbService::createField(tableInfo, CFieldInfo::create("value", bo::FT_VARCHAR, 64));
			}
		}else if (tableInfo->type()!=CTableInfo::TT_SYSTEM)
		{
			// ?
			tableInfo->type(CTableInfo::TT_SYSTEM);
			if (isopen())
				m_curdb->update("bodb_sys_option");
		}
	}

	void CDbService::setaccount(const tstring & account, const tstring & password)
	{
		m_account = account;
		m_password = password;
	}

	bool CDbService::use(const tstring & dbname)
	{
		if (!m_started) return false;

		if (isopen() && m_curdb->getDbName() == dbname)
		{
			return true;
		}
		close();

#ifdef USES_DEBUG
		char lpszBuffer[1024];
		sprintf(lpszBuffer,"use=%s\n",dbname.c_str());
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
		if (m_databases.find(dbname, m_curdb))
		{
#ifdef USES_DEBUG
			sprintf(lpszBuffer,"path=%s\n",m_path.c_str());
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
			m_curdb->setpath(m_path);
			if (m_curdb->open())
			{
				CTableInfo::pointer tableInfo = CDbService::getTableInfo("bodb_sys_account");
				if (tableInfo.get() == NULL)	// New Create Database
				{
					// Create System User Table
					create_default_table();
					return true;
				}
				create_default_table();	// ** for bodb_sys_option

				CFieldInfo::pointer fieldAccountInfo = tableInfo->getFieldInfo("account");
				CFieldInfo::pointer fieldPasswordInfo = tableInfo->getFieldInfo("password");
				if (fieldAccountInfo.get() == NULL || fieldPasswordInfo.get() == NULL)
					return true;

				CFieldVariant::pointer variantAccount = CFieldVariant::create(fieldAccountInfo->type());
				variantAccount->setString(m_account.c_str(), m_account.size());
				CFieldVariant::pointer variantPassword = CFieldVariant::create(fieldPasswordInfo->type());
				variantPassword->setString(m_password.c_str(), m_password.size());

				CSelectInfo pSelectInfo(tableInfo);
				std::list<CFieldCompare::pointer> wheres;
				wheres.push_back(CFieldCompare::create(tableInfo, CFieldCompare::FCT_EQUAL, fieldAccountInfo, variantAccount,0));
				wheres.push_back(CFieldCompare::create(tableInfo, CFieldCompare::FCT_EQUAL, fieldPasswordInfo, variantPassword,0));
				pSelectInfo.wheres.push_back(wheres);
				CResultSet::pointer rs = CDbService::select(pSelectInfo, false);
				if (rs.get() == NULL || rs->size() == 0)
				{
#ifdef USES_DEBUG
					sprintf(lpszBuffer,"password error %s,%s\n",m_account.c_str(),m_password.c_str());
					fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
					this->close();
				}else
				{
					return true;
				}
			}else
			{
#ifdef USES_DEBUG
				sprintf(lpszBuffer,"open error exist db=%s\n",dbname.c_str());
				fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
			}
		}else
		{
#ifdef USES_DEBUG
			sprintf(lpszBuffer,"not exist db=%s\n",dbname.c_str());
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
		}

		return false;
	}

	bool CDbService::isopen(void) const
	{
		return m_curdb.get() == 0 ? false : m_curdb->isopened();
	}

	bool CDbService::exist(const tstring & dbname) const
	{
		return m_databases.exist(dbname);
	}

	bool CDbService::drop(const tstring & dbname)
	{
		CDatabase::pointer database;
		if (!m_databases.find(dbname, database, true))
		{
			return false;
		}

		if (database.get() == m_curdb.get())
		{
			m_curdb.reset();
		}
		database->close();

		std::string dbpath(database->getpath());
		dbpath.append("/");
		dbpath.append(database->getDbName());

		namespace fs = boost::filesystem;
		boosttpath dbpathRoot(dbpath);
		//boosttpath dbpathRoot(dbpath, fs::native);
		if (fs::exists(dbpathRoot) && fs::is_directory(dbpathRoot))
		{
			fs::directory_iterator dir_iter(dbpathRoot),dir_end;
			for (;dir_iter !=dir_end;++dir_iter)
			{
				if (!fs::is_directory(*dir_iter))
				{
					//const boosttpath & dbPath = *dir_iter;
					tstring dbextension = dir_iter->path().extension().string();
					if (dbextension.compare(".bdf") == 0)
					{
						fs::remove(dir_iter->path());
					}
				}
			}

			if (fs::is_empty(dbpathRoot))
			{
				fs::remove(dbpathRoot);
			}
		}

		return true;
	}

	bool CDbService::rename(const tstring & tableName, const tstring & newName)
	{
		if (!isopen()) return false;
		return m_curdb->rename(tableName, newName);
	}
	bool CDbService::dropdefault(const tstring & tableName, const tstring & fieldName)
	{
		if (!isopen()) return false;
		return m_curdb->dropdefault(tableName, fieldName);
	}
	bool CDbService::setdefault(const tstring & tableName, const tstring & fieldName, tagItemValue * defaultValue)
	{
		if (!isopen()) return false;
		return m_curdb->setdefault(tableName, fieldName, defaultValue);
	}
	bool CDbService::setoption(OptionType nOption, bo::uinteger nValue)
	{
		if (!isopen()) return false;
		return m_curdb->setOption(nOption, nValue);
	}

	//CDatabase::pointer CDbService::opendb(const tstring & dbname)
	//{
	//	CDatabase::pointer database;
	//	if (m_databases.find(dbname, database))
	//	{
	//		database->setpath(m_path);
	//		database->open();
	//	}

	//	return database;
	//}

	void CDbService::close(void)
	{
		if (m_curdb.get() != 0)
		{
			m_curdb->close();
			m_curdb.reset();
		}
	}

	CTableInfo::pointer CDbService::getTableInfo(const tstring & tableName)
	{
		if (!isopen()) return boNullTableInfo;
		return m_curdb->getTableInfo(tableName);
	}

	CResultSet::pointer CDbService::select(const tstring & tableName)
	{
		if (!isopen()) return boNullResultSet;
		return m_curdb->select(tableName);
	}

	CResultSet::pointer CDbService::select(const CSelectInfo& pSelectInfo, bool distinct)
	//CResultSet::pointer CDbService::select(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CLockMap<void*,CTableInfo::pointer>& pOutPutTableInfoList, bool distinct)
	{
		if (!isopen()) return boNullResultSet;
		return m_curdb->select(pSelectInfo, distinct);
	}

	bool CDbService::insert(const CRecordLine::pointer& recordLine)
	{
		if (!isopen()) return false;
		return m_curdb->insert(recordLine);
	}

	int CDbService::update(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CRecordLine::pointer& updateVal)
	{
		if (!isopen()) return -1;
		return m_curdb->update(tableInfo, wheres, updateVal);
	}

	int CDbService::deleters(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres)
	{
		if (!isopen()) return -1;
		return m_curdb->deleters(tableInfo, wheres);
	}

	bool CDbService::createTable(const CTableInfo::pointer& tableInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);

		if (!isopen()) return false;

		return m_curdb->createTable(tableInfo);
	}

	bool CDbService::dropTable(const tstring & tablename)
	{
		if (!isopen()) return false;

		return m_curdb->dropTable(tablename);
	}

	bool CDbService::createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);
		if (!isopen()) return false;
		return m_curdb->createField(tableInfo, fieldInfo);
	}
	bool CDbService::dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		if (!isopen()) return false;
		return m_curdb->dropField(tableInfo, fieldname);
	}
	bool CDbService::modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);
		if (!isopen()) return false;
		return m_curdb->modifyField(tableInfo, fieldname,fieldInfo);
	}

	CDbService::CDbService(void)
		: m_started(false), m_path(_T(""))
	{
	}
	CDbService::~CDbService(void)
	{
		stop();
	}

} // namespace bo

