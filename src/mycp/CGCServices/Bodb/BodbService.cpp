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

#define USES_BODBCDBC		1		// [0,1]

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


#if (USES_BODBCDBC)

#include <bodb/bodb2.h>
using namespace bo;

class CCDBCResultSet
{
public:
	typedef boost::shared_ptr<CCDBCResultSet> pointer;

	int size(void) const
	{
		return m_resltset == NULL ? -1 : m_resltset->rscount;
	}
	int index(void) const
	{
		return m_currentIndex;
	}
	cgcValueInfo::pointer index(int moveIndex)
	{
		if (m_resltset == NULL || m_resltset->rscount == 0) return cgcNullValueInfo;
		if (moveIndex < 0 || (moveIndex+1) > m_resltset->rscount) return cgcNullValueInfo;

		m_currentIndex = moveIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer first(void)
	{
		if (m_resltset == NULL || m_resltset->rscount == 0) return cgcNullValueInfo;

		m_currentIndex = 0;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer next(void)
	{
		if (m_resltset == NULL || m_resltset->rscount == 0) return cgcNullValueInfo;
		if (m_currentIndex+1 == m_resltset->rscount) return cgcNullValueInfo;

		++m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer previous(void)
	{
		if (m_resltset == NULL || m_resltset->rscount == 0) return cgcNullValueInfo;
		if (m_currentIndex == 0) return cgcNullValueInfo;

		--m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer last(void)
	{
		if (m_resltset == NULL || m_resltset->rscount == 0) return cgcNullValueInfo;

		m_currentIndex = m_resltset->rscount - 1;
		return getCurrentRecord();
	}
	void reset(void)
	{
		bodb_free(m_resltset);
		m_resltset = NULL;
		m_currentIndex = 0;
	}

	CCDBCResultSet(PRESULTSET resultset)
		: m_resltset(resultset), m_currentIndex(0)
	{}
	~CCDBCResultSet(void)
	{
		reset();
	}

protected:
	cgcValueInfo::pointer getCurrentRecord(void) const
	{
		assert (m_resltset != NULL);
		assert (m_currentIndex >= 0 && m_currentIndex < m_resltset->rscount);

		std::vector<cgcValueInfo::pointer> record;
		for (short i=0; i<m_resltset->rsvalues[m_currentIndex]->fieldcount; i++)
		{
			CFieldVariant varField(m_resltset->rsvalues[m_currentIndex]->fieldvalues[i]);
			switch (varField.VARTYPE)
			{
			case FT_BOOLEAN:
				record.push_back(CGC_VALUEINFO(varField.getBool()));
				break;
			case FT_TINYINT:
			case FT_UTINYINT:
			case FT_SMALLINT:
			case FT_USMALLINT:
			case FT_INTEGER:
			case FT_UINTEGER:
				record.push_back(CGC_VALUEINFO(varField.getInt()));
				break;
			case FT_BIGINT:
			case FT_UBIGINT:
			case FT_NUMERIC:
				{
					cgcValueInfo::pointer pValueInfo = CGC_VALUEINFO(cgcValueInfo::TYPE_BIGINT);
					pValueInfo->setBigInt(varField.getBigInt());
					record.push_back(pValueInfo);
				}break;
			case FT_FLOAT:
			case FT_REAL:
			case FT_DOUBLE:
				record.push_back(CGC_VALUEINFO(varField.getReal()));
				break;
			default:
				record.push_back(CGC_VALUEINFO(varField.getString()));
				break;
			}
		}
		return CGC_VALUEINFO(record);
	}

private:
	PRESULTSET	m_resltset;
	int			m_currentIndex;
};

#define BODB_RESULTSET(r) CCDBCResultSet::pointer(new CCDBCResultSet(r))


class CBodbCdbc
	: public cgcCDBCService
{
public:
	typedef boost::shared_ptr<CBodbCdbc> pointer;

	CBodbCdbc(const tstring& path)
		: m_currentCookie(0), m_apppath(path)
	{}
	~CBodbCdbc(void)
	{
		finalService();
	}
	virtual bool initService(cgcValueInfo::pointer parameter)
	{
		if (isServiceInited()) return true;

		tstring sBodbPath = parameter.get() == NULL ? m_apppath : parameter->getStr();

		m_bodbHandler = bo::bodb_init(sBodbPath.c_str());
		m_bServiceInited = (m_bodbHandler.get() != NULL);
		return m_bServiceInited;
	}
	virtual void finalService(void)
	{
		if (isServiceInited())
		{
			m_cdbcInfo.reset();
			close();

			bo::bodb_exit(m_bodbHandler);
			m_bodbHandler.reset();
			m_bServiceInited = false;
		}
	}
private:
	virtual tstring serviceName(void) const {return _T("BODBCDBC");}

	virtual void escape_string_in(std::string & str)
	{
		bo::bodb_escape_string_in(str);
	}
	virtual void escape_string_out(std::string & str)
	{
		bo::bodb_escape_string_out(str);
	}
	virtual bool open(const cgcCDBCInfo::pointer& cdbcInfo)
	{
		if (cdbcInfo.get() == NULL) return false;
		if (!isServiceInited()) return false;

		if (m_cdbcInfo.get() == cdbcInfo.get() && this->isopen()) return true;

		// close old database;
		close();
		m_cdbcInfo = cdbcInfo;

		char sql[1024];
		sprintf(sql, "USE %s", cdbcInfo->getDatabase().c_str());

		m_bodbHandler->setaccount(m_cdbcInfo->getAccount(), m_cdbcInfo->getSecure());
		int ret = m_bodbHandler->execsql(sql);
		m_serviceInfo->setInt(ret);
		return ret == 0;
	}
	virtual bool open(void) {return open(m_cdbcInfo);}
	virtual void close(void)
	{
		if (isServiceInited() && m_bodbHandler->isopen())
		{
			m_currentCookie = 0;
			m_results.clear();
			m_bodbHandler->close();
		}
	}
	virtual bool isopen(void) const
	{
		return (isServiceInited() && m_bodbHandler->isopen());
	}

	virtual int execute(const char * exeSql)
	{
		if (exeSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		int ret = m_bodbHandler->execsql(exeSql);
		//m_serviceInfo->setInt(ret);
		return ret;
	}

	virtual int select(const char * selectSql, int& outCookie)
	{
		if (selectSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		PRESULTSET resultset = 0;
		m_bodbHandler->execsql(selectSql, &resultset);

		if (resultset != NULL)
		{
			if (resultset->rscount > 0)
			{
				outCookie = ++m_currentCookie;
				m_results.insert(outCookie, BODB_RESULTSET(resultset));
			}else
			{
				bodb_free(resultset);
				resultset = NULL;
			}
		}
		return resultset == NULL ? 0 : resultset->rscount;
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

private:
	CBodbHandler::pointer m_bodbHandler;
	CLockMap<int, CCDBCResultSet::pointer> m_results;
	int m_currentCookie;
	cgcCDBCInfo::pointer m_cdbcInfo;

	tstring m_apppath;
};

const int ATTRIBUTE_NAME = 1;
cgcAttributes::pointer theAppAttributes;
//CBodbCdbc::pointer theBodbCdbc;
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
			CBodbCdbc::pointer service = CGC_OBJECT_CAST<CBodbCdbc>(iter->second);
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
	CBodbCdbc::pointer bodbCdbc = CBodbCdbc::pointer(new CBodbCdbc(theAppConfPath));
	outService = bodbCdbc;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), outService);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL || theAppAttributes.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}

#endif // USES_BODBCDBC
