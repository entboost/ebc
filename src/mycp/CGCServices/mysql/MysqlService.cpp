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

#define USES_MYSQLCDBC		1		// [0,1]

#ifdef WIN32
#pragma warning(disable:4267)
#include <windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//#pragma comment(lib, "mysqlcppconn.lib")
//#pragma comment(lib, "mysqlcppconn-static.lib")

#endif // WIN32

#include <CGCBase/cdbc.h>
using namespace cgc;


#if (USES_MYSQLCDBC)

#include <mysql.h>
#ifdef WIN32
#pragma comment(lib,"libmySQL.lib")
#endif // WIN32

my_ulonglong GetRowsNumber(MYSQL_RES * res)
{
	return mysql_num_rows(res);
}

class CCDBCResultSet
{
public:
	typedef boost::shared_ptr<CCDBCResultSet> pointer;

	int size(void) const
	{
		return m_rows;
	}
	int index(void) const
	{
		//return m_resultset == NULL ? -1 : (int)m_resultset->getRow();
		return m_currentIndex;
	}
	cgcValueInfo::pointer index(int moveIndex)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (moveIndex < 0 || (moveIndex+1) > m_rows) return cgcNullValueInfo;
		//if (!m_resultset->relative(moveIndex)) return cgcNullValueInfo;

		m_currentIndex = moveIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer first(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		//if (!m_resultset->first()) return cgcNullValueInfo;

		m_currentIndex = 0;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer next(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (m_currentIndex+1 == m_rows) return cgcNullValueInfo;
		//if (!m_resultset->next()) return cgcNullValueInfo;

		++m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer previous(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (m_currentIndex == 0) return cgcNullValueInfo;
		//if (!m_resultset->previous()) return cgcNullValueInfo;

		--m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer last(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		//if (!m_resultset->last()) return cgcNullValueInfo;

		m_currentIndex = m_rows - 1;
		return getCurrentRecord();
	}
	void reset(void)
	{
		if (m_resultset)
		{
			try
			{
				mysql_free_result(m_resultset);
			}catch(...)
			{}
			m_resultset = NULL;
		}
		m_rows = 0;

		//try
		//{
		//	if (m_resultset)
		//	{
		//		m_resultset->close();
		//		delete m_resultset;
		//		m_resultset = NULL;
		//	}
		//	if (m_stmt)
		//	{
		//		delete m_stmt;
		//		m_stmt = NULL;
		//	}
		//}catch(sql::SQLException &e)
		//{
		//}

		//m_currentIndex = 0;
	}

	//CCDBCResultSet(sql::ResultSet * resultset, sql::Statement * stmt)
	//	: m_resultset(resultset), m_stmt(stmt)//, m_currentIndex(0)
	//{}
	CCDBCResultSet(MYSQL_RES * resultset)
		: m_resultset(resultset), m_rows(0)
	{
		m_rows = mysql_num_rows(m_resultset);
	}
	virtual ~CCDBCResultSet(void)
	{
		reset();
	}

protected:
	cgcValueInfo::pointer getCurrentRecord(void) const
	{
		assert (m_resultset != NULL);
		assert (m_currentIndex >= 0 && m_currentIndex < (int)m_rows);
		//if (m_resultset->isBeforeFirst() || m_resultset->isAfterLast() || m_resultset->isClosed())
		//	return cgcNullValueInfo;

		std::vector<cgcValueInfo::pointer> record;
		try
		{
			mysql_data_seek(m_resultset, (my_ulonglong)m_currentIndex);
			MYSQL_ROW_OFFSET rowoffset = mysql_row_tell(m_resultset);
			if (rowoffset == NULL)
				return cgcNullValueInfo;
			//MYSQL_ROW row = mysql_fetch_row(m_resultset);
			//MYSQL_ROW row = m_resultset->current_row;
			MYSQL_ROW row = rowoffset->data;
			if (row != NULL)
			{
				for(unsigned int i=0; i<m_resultset->field_count; i++)
				{
					tstring s = row[i] ? (const char*)row[i] : "NULL";
					record.push_back(CGC_VALUEINFO(s));
				}
			}
		}catch(...)
		{
			// ...
		}

		//for (int i=1; i<35; i++)
		//{
		//	try
		//	{
		//		//if (m_resultset->isNull(i))
		//		//	break;
		//		//int s = m_resultset->getInt(i);
		//		sql::SQLString s = m_resultset->getString(i);
		//		record.push_back(CGC_VALUEINFO(s));
		//	} catch (sql::SQLException &e)
		//	{
		//		break;
		//	}
		//}
		return CGC_VALUEINFO(record);
	}

private:
	MYSQL_RES * m_resultset;
	int			m_rows;
	int			m_fields;
	//sql::ResultSet *	m_resultset;
	//sql::Statement *	m_stmt;
	int					m_currentIndex;
};

#define CDBC_RESULTSET(r) CCDBCResultSet::pointer(new CCDBCResultSet(r))

const int escape_size = 1;
const std::string escape_in[] = {"&lsquo;"};
const std::string escape_out[] = {"'"};


class CMysqlCdbc
	: public cgcCDBCService
{
public:
	typedef boost::shared_ptr<CMysqlCdbc> pointer;

	CMysqlCdbc(const tstring& path)
		//: m_driver(NULL), m_con(NULL)
		: m_mysql(NULL)
		, m_tLastTime(0)
	{}
	virtual ~CMysqlCdbc(void)
	{
		finalService();
	}
	virtual bool initService(cgcValueInfo::pointer parameter)
	{
		if (isServiceInited()) return true;

		//try
		//{
		//	if (m_driver == NULL)
		//		m_driver = get_driver_instance();
		//	if (m_driver == NULL)
		//		return false;
		//} catch (sql::SQLException &e)
		//{
		//	return false;
		//}

		m_bServiceInited = true;
		return isServiceInited();
	}
	virtual void finalService(void)
	{
		close();

		//m_driver = NULL;
		m_cdbcInfo.reset();
		m_bServiceInited = false;
	}
private:
	virtual tstring serviceName(void) const {return _T("MYSQLCDBC");}

	static const std::string & replace(std::string & strSource, const std::string & strFind, const std::string &strReplace)
	{
		tstring::size_type pos=0;
		tstring::size_type findlen=strFind.size();
		tstring::size_type replacelen=strReplace.size();
		while ((pos=strSource.find(strFind, pos)) != tstring::npos)
		{
			strSource.replace(pos, findlen, strReplace);
			pos += replacelen;
		}
		return strSource;
	}

	virtual void escape_string_in(std::string & str)
	{
		for (int i=0; i<escape_size; i++)
			replace(str, escape_out[i], escape_in[i]);
	}
	virtual void escape_string_out(std::string & str)
	{
		for (int i=0; i<escape_size; i++)
			replace(str, escape_in[i], escape_out[i]);
	}

	virtual bool open(const cgcCDBCInfo::pointer& cdbcInfo)
	{
		if (cdbcInfo.get() == NULL) return false;
		if (!isServiceInited()) return false;

		if (m_cdbcInfo.get() == cdbcInfo.get() && this->isopen()) return true;

		// close old database;
		close();
		m_cdbcInfo = cdbcInfo;
		m_tLastTime = time(0);

		try
		{
			m_mysql = mysql_init((MYSQL*)NULL);
			if (m_mysql == NULL)
				return false;

			if (!mysql_real_connect(m_mysql, m_cdbcInfo->getHost().c_str(), m_cdbcInfo->getAccount().c_str(),
				m_cdbcInfo->getSecure().c_str(), m_cdbcInfo->getDatabase().c_str(), 0, NULL, 0))
			{
				mysql_close(m_mysql);
				m_mysql = NULL;
				return false;
			}

			tstring sCharset = m_cdbcInfo->getCharset();
			if (sCharset.empty())
			{
				sCharset = "utf8";
			}
			char lpszCharsetCommand[100];
			sprintf(lpszCharsetCommand, "set names %s;", sCharset.c_str());
			mysql_query(m_mysql, lpszCharsetCommand);
			//mysql_query("set names utf8;");
		}catch(...)
		{
			return false;
		}

		//try
		//{
		//	//m_con = m_driver->connect("tcp://127.0.0.1:3306", "root", "root");
		//	m_con = m_driver->connect(cdbcInfo->getConnection().c_str(), cdbcInfo->getAccount().c_str(), cdbcInfo->getSecure().c_str());
		//	if (m_con == NULL)
		//		return false;

		//	m_con->setSchema(cdbcInfo->getDatabase().c_str());

		//} catch (sql::SQLException &e)
		//{
		//	m_con = NULL;
		//	return false;
		//}

		return true;
	}
	virtual bool open(void) {return open(m_cdbcInfo);}
	virtual void close(void)
	{
		try
		{
			if (m_mysql)
			{
				mysql_close(m_mysql);
				m_mysql = NULL;
				m_tLastTime = time(0);
			}
		}catch(...)
		{
		}
		//if (m_con)
		//{
		//	delete m_con;
		//	m_con = NULL;
		//}

		m_results.clear();
	}
	virtual bool isopen(void) const
	{
		return m_mysql != NULL;
		//return (m_con != NULL && !m_con->isClosed());
	}
	virtual time_t lasttime(void) const {return m_tLastTime;}

	virtual int execute(const char * exeSql)
	{
		if (exeSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		int ret = 0;
		try
		{
			if(mysql_query(m_mysql, exeSql))
				return -1;

			ret = (int)mysql_affected_rows(m_mysql);
			//resultset = mysql_store_result(m_mysql);
		}catch(...)
		{
			return -1;
		}
		//try
		//{
		//	sql::Statement * stmt = m_con->createStatement();
		//	ret = stmt->executeUpdate(exeSql);
		//	delete stmt;
		//} catch (sql::SQLException &e)
		//{
		//	return -1;
		//}
		return ret;
	}

	virtual int select(const char * selectSql, int& outCookie)
	{
		if (selectSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		MYSQL_RES * resultset = NULL;
		int rows = 0;
		try
		{
			if(mysql_query(m_mysql, selectSql))
				return -1;

			resultset = mysql_store_result(m_mysql);
			rows = (int)mysql_num_rows(resultset);

			if (rows <= 0)
			{
				mysql_free_result(resultset);
				resultset = NULL;
			}
		}catch(...)
		{
			return -1;
		}
		if (rows > 0)
		{
			outCookie = (int)resultset;
			m_results.insert(outCookie, CDBC_RESULTSET(resultset));
		}
		return rows;
		//sql::ResultSet * resultset = NULL;
		//sql::Statement * stmt = NULL;
		//try
		//{
		//	stmt = m_con->createStatement();
		//	resultset = stmt->executeQuery(selectSql);
		//	//resultset = stmt->executeQuery("SELECT 'Hello World!' AS _message");
		//}catch (sql::SQLException &e)
		//{
		//	return -1;
		//}

		//if (resultset != NULL)
		//{
		//	if (resultset->rowsCount() > 0)
		//	{
		//		outCookie = (int)stmt;
		//		m_results.insert(outCookie, CDBC_RESULTSET(resultset, stmt));
		//	}else
		//	{
		//		delete resultset;
		//		resultset = NULL;
		//	}
		//}
		//return resultset == NULL ? 0 : (int)resultset->rowsCount();
	}

	virtual int size(int cookie) const
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->size() : -1;
	}
	virtual int index(int cookie) const
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->index() : -1;
	}

	virtual cgcValueInfo::pointer index(int cookie, int moveIndex)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->index(moveIndex) : cgcNullValueInfo;
	}
	virtual cgcValueInfo::pointer first(int cookie)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->first() : cgcNullValueInfo;
	}
	virtual cgcValueInfo::pointer next(int cookie)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->next() : cgcNullValueInfo;
	}
	virtual cgcValueInfo::pointer previous(int cookie)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->previous() : cgcNullValueInfo;
	}
	virtual cgcValueInfo::pointer last(int cookie)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->last() : cgcNullValueInfo;
	}
	virtual void reset(int cookie)
	{
		CCDBCResultSet::pointer cdbcResultSet;
		if (m_results.find(cookie, cdbcResultSet, true))
		{
			cdbcResultSet->reset();
		}
	}

	virtual bool auto_commit(bool autocommit)
	{
		if (!isopen()) return false;

		return mysql_autocommit(m_mysql, autocommit ? 1 : 0) == 1;
	}
	virtual bool commit(void)
	{
		if (!isopen()) return false;

		return mysql_commit(m_mysql) == 1;
	}
	virtual bool rollback(void)
	{
		if (!isopen()) return false;

		return mysql_rollback(m_mysql) == 1;
	}
private:
	//sql::Driver * m_driver;
	//sql::Connection * m_con;
	MYSQL * m_mysql;

	time_t m_tLastTime;
	CLockMap<int, CCDBCResultSet::pointer> m_results;
	cgcCDBCInfo::pointer m_cdbcInfo;
};

const int ATTRIBUTE_NAME = 1;
cgcAttributes::pointer theAppAttributes;
//CMysqlCdbc::pointer theBodbCdbc;
tstring theAppConfPath;

// 模块初始化函数，可选实现函数
extern "C" bool CGC_API CGC_Module_Init(void)
{
	theAppAttributes = theApplication->getAttributes(true);
	assert (theAppAttributes.get() != NULL);

	theAppConfPath = theApplication->getAppConfPath();
	return true;
}

// 模块退出函数，可选实现函数
extern "C" void CGC_API CGC_Module_Free(void)
{
	VoidObjectMapPointer mapLogServices = theAppAttributes->getVoidAttributes(ATTRIBUTE_NAME, false);
	if (mapLogServices.get() != NULL)
	{
		CObjectMap<void*>::iterator iter;
		for (iter=mapLogServices->begin(); iter!=mapLogServices->end(); iter++)
		{
			CMysqlCdbc::pointer service = CGC_OBJECT_CAST<CMysqlCdbc>(iter->second);
			if (service.get() != NULL)
			{
				service->finalService();
			}
		}
	}
	theAppAttributes.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (theAppAttributes.get() == NULL) return;
	CMysqlCdbc::pointer bodbCdbc = CMysqlCdbc::pointer(new CMysqlCdbc(theAppConfPath));
	outService = bodbCdbc;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), outService);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL || theAppAttributes.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}

#endif // USES_MYSQLCDBC
