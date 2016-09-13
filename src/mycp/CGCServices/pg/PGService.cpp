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

#define USES_PGCDBC		1		// [0,1]


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

#endif // WIN32

#include <CGCBase/cdbc.h>
using namespace cgc;

#if (USES_PGCDBC)
#include "db/pool.h"

#ifdef WIN32
#pragma comment(lib,"libpq.lib")
#endif // WIN32

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
				result_clean(m_sink, m_resultset);
				sink_pool_put (m_sink);
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

	CCDBCResultSet(Sink * sink, Result * resultset, int rows)
		: m_sink(sink), m_resultset(resultset), m_rows(rows)
	{
		m_cols = result_cn(m_sink, m_resultset);
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

			for(int i=0; i<m_cols; i++)
			{
				tstring s = result_get(m_sink, m_resultset, m_currentIndex, i);
				record.push_back(CGC_VALUEINFO(s));
			}
		}catch(...)
		{
			// ...
		}

		return CGC_VALUEINFO(record);
	}

private:
	Sink *		m_sink;
	Result *	m_resultset;
	int			m_rows;
	int			m_cols;
	int			m_currentIndex;
};

#define CDBC_RESULTSET(sink, res, rows) CCDBCResultSet::pointer(new CCDBCResultSet(sink, res, rows))

const int escape_size = 1;
const std::string escape_in[] = {"&lsquo;"};
const std::string escape_out[] = {"'"};

class CPgCdbc
	: public cgcCDBCService
{
public:
	typedef boost::shared_ptr<CPgCdbc> pointer;

	CPgCdbc(void)
		: m_isopen(false)
		, m_tLastTime(0)
	{}
	virtual ~CPgCdbc(void)
	{
		finalService();
	}
	virtual bool initService(cgcValueInfo::pointer parameter)
	{
		if (isServiceInited()) return true;
		m_bServiceInited = true;
		return isServiceInited();
	}
	virtual void finalService(void)
	{
		close();
		m_cdbcInfo.reset();
		m_bServiceInited = false;
	}
private:
	virtual tstring serviceName(void) const {return _T("PGCDBC");}

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
			tstring sHost = m_cdbcInfo->getHost();
			tstring::size_type find = sHost.find(_T(":"));
			if (find == tstring::npos)
				return false;
			tstring sDbHost = sHost.substr(0, find);
			int nDbPort = atoi(sHost.substr(find+1).c_str());
			unsigned short nMin = m_cdbcInfo->getMinSize();
			unsigned short nMax = m_cdbcInfo->getMaxSize();

			int ret = sink_pool_init(POSTSINK,
				nMin, nMax,
				sDbHost.c_str(),
				nDbPort,
				0,
				m_cdbcInfo->getDatabase().c_str(),
				m_cdbcInfo->getAccount().c_str(),
				m_cdbcInfo->getSecure().c_str());
			if (ret < 0)
			{
				printf("%s\n", "connect to database failed!, start failed!");
				return false;
			}

		}catch(...)
		{
			return false;
		}

		m_isopen = true;
		return true;
	}
	virtual bool open(void) {return open(m_cdbcInfo);}
	virtual void close(void)
	{
		if (m_isopen)
		{
			m_isopen = false;
			m_tLastTime = time(0);
			try
			{
				sink_pool_cleanup();
			}catch(...)
			{
			}
			m_results.clear();
		}
	}
	virtual bool isopen(void) const
	{
		return m_isopen;
	}
	virtual time_t lasttime(void) const {return m_tLastTime;}

	virtual int execute(const char * exeSql)
	{
		if (exeSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		Result *res;
		int ret = 0;
		int state;
		try
		{
			m_tLastTime = time(0);
			Sink *sink = sink_pool_get();

			res= sink_exec (sink, exeSql);
			state = result_state (sink, res);
			if((state != RES_COMMAND_OK) &&
				(state != RES_TUPLES_OK)  &&
				(state != RES_COPY_IN)  &&
				(state != RES_COPY_OUT))
			{
				result_clean(sink, res);
				sink_pool_put (sink);
				return -1;
			}
			ret = result_rn (sink, res);
			result_clean (sink, res);
			sink_pool_put (sink);
		}catch(...)
		{
			return -1;
		}
		return ret;
	}

	virtual int select(const char * selectSql, int& outCookie)
	{
		if (selectSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		int rows = 0;
		try
		{
			m_tLastTime = time(0);
			Sink *sink = sink_pool_get();

			Result *res=0;
			int    state;
			res= sink_exec( sink, selectSql);
			state = result_state (sink, res);
			if((state != RES_COMMAND_OK) &&
				(state != RES_TUPLES_OK)  &&
				(state != RES_COPY_IN)    &&
				(state != RES_COPY_OUT))
			{
				result_clean(sink, res);
				sink_pool_put (sink);
				return 0;
			}
			rows = result_rn(sink, res);
			if (rows > 0)
			{
				outCookie = (int)res;
				m_results.insert(outCookie, CDBC_RESULTSET(sink, res, rows));
			}else
			{
				result_clean(sink, res);
				sink_pool_put (sink);
			}
		}catch(...)
		{
			return -1;
		}
		return rows;
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
		return false;
		//return mysql_autocommit(m_mysql, autocommit ? 1 : 0) == 1;
	}
	virtual bool commit(void)
	{
		if (!isopen()) return false;
		return false;
		//return mysql_commit(m_mysql) == 1;
	}
	virtual bool rollback(void)
	{
		if (!isopen()) return false;
		return false;
		//return mysql_rollback(m_mysql) == 1;
	}
private:
	bool m_isopen;
	time_t m_tLastTime;
	CLockMap<int, CCDBCResultSet::pointer> m_results;
	cgcCDBCInfo::pointer m_cdbcInfo;
};

const int ATTRIBUTE_NAME = 1;
cgcAttributes::pointer theAppAttributes;
//CPgCdbc::pointer theBodbCdbc;
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
			CPgCdbc::pointer service = CGC_OBJECT_CAST<CPgCdbc>(iter->second);
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
	CPgCdbc::pointer bodbCdbc = CPgCdbc::pointer(new CPgCdbc());
	outService = bodbCdbc;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), outService);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL || theAppAttributes.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}

#endif // USES_PGCDBC
