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

#define USES_ADOCDBC		1		// [0,1]

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


#if (USES_ADOCDBC)

#import "d:/msado15.dll" no_namespace rename("EOF","adoEOF")
//#import "c:/program files/common files/system/ado/msado15.dll" no_namespace rename("EOF","adoEOF")

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
		if (FAILED(m_resultset->Move((long)moveIndex))) return cgcNullValueInfo;
		m_currentIndex = moveIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer first(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (FAILED(m_resultset->MoveFirst())) return cgcNullValueInfo;
		m_currentIndex = 0;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer next(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (m_currentIndex+1 == m_rows) return cgcNullValueInfo;
		if (FAILED(m_resultset->MoveNext())) return cgcNullValueInfo;
		++m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer previous(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (m_currentIndex == 0) return cgcNullValueInfo;
		if (FAILED(m_resultset->MovePrevious())) return cgcNullValueInfo;
		--m_currentIndex;
		return getCurrentRecord();
	}
	cgcValueInfo::pointer last(void)
	{
		if (m_resultset == NULL || m_rows == 0) return cgcNullValueInfo;
		if (FAILED(m_resultset->MoveLast())) return cgcNullValueInfo;
		m_currentIndex = m_rows - 1;
		return getCurrentRecord();
	}
	void reset(void)
	{
		if (m_resultset)
		{
			m_resultset.Release();
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

	CCDBCResultSet(_RecordsetPtr resultset, int rows)
		: m_resultset(resultset), m_rows(rows)
	{
		//FieldsPtr pFields = m_resultset->GetFields();
		//pFields->GetCount();
		//m_cols = result_cn(m_sink, m_resultset);
	}
	~CCDBCResultSet(void)
	{
		reset();
	}

protected:
	cgcValueInfo::pointer getCurrentRecord(void) const
	{
		assert (m_resultset != NULL);
		assert (m_currentIndex >= 0 && m_currentIndex < (int)m_rows);
		if (m_resultset->GetadoEOF())
			return cgcNullValueInfo;

		std::vector<cgcValueInfo::pointer> record;
		try
		{
			FieldsPtr pFields = m_resultset->GetFields();
			int m_cols = pFields->GetCount();
			for(int i=0; i<m_cols; i++)
			{
				_variant_t var;
				var = m_resultset->GetCollect(_variant_t((long)i));
				tstring s = (LPCTSTR)(_bstr_t)var;
				record.push_back(CGC_VALUEINFO(s));
			}
		}catch(_com_error e)
		{
		}catch(...)
		{
			// ...
		}

		return CGC_VALUEINFO(record);
	}

private:
	_RecordsetPtr	m_resultset;
	int				m_rows;
	//int				m_cols;
	int				m_currentIndex;
};

#define CDBC_RESULTSET(res, rows) CCDBCResultSet::pointer(new CCDBCResultSet(res, rows))


class CAdoCdbc
	: public cgcCDBCService
{
public:
	typedef boost::shared_ptr<CAdoCdbc> pointer;

	CAdoCdbc(void)
		: m_isopen(false)
		, m_nCurrentCookie(0)
	{}
	~CAdoCdbc(void)
	{
		finalService();
	}
	virtual bool initService(cgcValueInfo::pointer parameter)
	{
		if (isServiceInited()) return true;
		//::CoInitialize(NULL);
		//AfxOleInit();
		HRESULT hr = ::OleInitialize(NULL);
		printf("OleInitialize! 0x%x\n", hr);
		hr = m_connection.CreateInstance(__uuidof(Connection));
		//hr = m_connection.CreateInstance("ADODB.Connection");
		if (FAILED(hr))
		{
			printf("create ADODB.Connection failed! 0x%x\n", hr);
			return false;
		}
		m_bServiceInited = true;
		return isServiceInited();
	}
	virtual void finalService(void)
	{
		close();
		m_cdbcInfo.reset();
		m_connection.Release();
		m_bServiceInited = false;
	}
private:
	virtual tstring serviceName(void) const {return _T("ADOCDBC");}

	virtual void escape_string_in(std::string & str)
	{
		//bo::bodb_escape_string_in(str);
	}
	virtual void escape_string_out(std::string & str)
	{
		//bo::bodb_escape_string_out(str);
	}
	
	virtual bool open(const cgcCDBCInfo::pointer& cdbcInfo)
	{
		if (cdbcInfo.get() == NULL) return false;
		if (!isServiceInited()) return false;

		if (m_cdbcInfo.get() == cdbcInfo.get() && this->isopen()) return true;

		// close old database;
		close();
		m_cdbcInfo = cdbcInfo;

		try
		{
/*
CString strSRC;
strSRC="Driver=SQL Server;Server=";
strSRC+="suppersoft";
strSRC+=";Database=";
strSRC+="mydb";
strSRC+=";UID=SA;PWD=";
Driver=SQL Server;Server=suppersoft;Database=mydb;UID=SA;PWD=

*/
			_bstr_t bstrSRC(m_cdbcInfo->getConnection().c_str());
			if (FAILED(m_connection->Open(bstrSRC,"","",-1)))
			{
				printf("%s\n", "connect to database failed!, start failed!");
				return false;
			}

		}catch(_com_error e)
		{
			return false;
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
			m_connection->Close();
			m_results.clear();
		}
	}
	virtual bool isopen(void) const
	{
		return m_isopen;
	}

	virtual int execute(const char * exeSql)
	{
		if (exeSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		try
		{
			//tstring sCharset = m_cdbcInfo->getCharset();
			//if (!sCharset.empty())
			//{
			//	char lpszCharsetCommand[100];
			//	sprintf(lpszCharsetCommand, "set character_set_connection=%s;", sCharset.c_str());
			//	m_connection->Execute(lpszCharsetCommand, NULL, adCmdText);     
			//	sprintf(lpszCharsetCommand, "set character_set_results=%s;", sCharset.c_str());
			//	m_connection->Execute(lpszCharsetCommand, NULL, adCmdText);     
			//	sprintf(lpszCharsetCommand, "set character_set_client=%s;", sCharset.c_str());
			//	m_connection->Execute(lpszCharsetCommand, NULL, adCmdText);     
			//}

			_variant_t RecordsAffected;
			m_connection->Execute(_bstr_t(exeSql),&RecordsAffected,adCmdText);
			return 0;
		}catch(_com_error e)
		{
			return -1;
		}catch(...)
		{
			return -1;
		}
	}

	virtual int select(const char * selectSql, int& outCookie)
	{
		if (selectSql == NULL || !isServiceInited()) return -1;
		if (!open()) return -1;

		int rows = 0;
		try
		{

			_RecordsetPtr pPtr;
			if (FAILED(pPtr.CreateInstance("ADODB.Recordset")))
			{
				printf("Create ADODB.Recordset Instance failed!\n");
				return -1;
			}

			_variant_t varSRC(m_cdbcInfo->getConnection().c_str());
			_variant_t varSQL(selectSql);
			if(FAILED(pPtr->Open(varSQL,varSRC,adOpenStatic,adLockOptimistic,adCmdText)))
			{
				printf("Open table failed!\n");
				pPtr.Release();
				return -1;
			}

			//_variant_t RecordsAffected;
			//_RecordsetPtr pPtr = m_connection->Execute(_bstr_t(selectSql),&RecordsAffected,adCmdText);
			//if (pPtr == NULL)
			//{
			//	printf("select failed!\n");
			//	return 0;
			//}

			rows = (int)pPtr->GetRecordCount();
			printf("SQL=%s; ROWS=%d\n", selectSql, rows);
			if (rows > 0)
			{
				//outCookie = (int)pPtr;
				outCookie = ++m_nCurrentCookie;
				m_results.insert(outCookie, CDBC_RESULTSET(pPtr, rows));
			}else
			{
				pPtr.Release();
			}
		}catch(_com_error e)
		{
			printf("SQL=%s; ERROR=%s\n", selectSql, e.ErrorMessage());
			return -1;
		}catch(...)
		{
			printf("SQL=%s; ERROR\n", selectSql);
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
	CLockMap<int, CCDBCResultSet::pointer> m_results;
	cgcCDBCInfo::pointer m_cdbcInfo;
	_ConnectionPtr m_connection;
	int m_nCurrentCookie;
};

const int ATTRIBUTE_NAME = 1;
cgcAttributes::pointer theAppAttributes;
//CAdoCdbc::pointer theBodbCdbc;
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
			CAdoCdbc::pointer service = CGC_OBJECT_CAST<CAdoCdbc>(iter->second);
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
	CAdoCdbc::pointer bodbCdbc = CAdoCdbc::pointer(new CAdoCdbc());
	outService = bodbCdbc;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), outService);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL || theAppAttributes.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}

#endif // USES_ADOCDBC
