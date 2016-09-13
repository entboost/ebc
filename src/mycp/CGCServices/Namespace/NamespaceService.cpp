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

#ifdef WIN32
#pragma warning(disable:4018 4267 4584 4996)
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

// cgc head
#include <CGCBase/app.h>
#include "cgcNamespace.h"
using namespace cgc;

#if (USES_NAMESPACE)
#include <bodb/bodb2.h>
using namespace bo;

class CNamespaceService
	: public cgcNamespace
{
public:
	typedef boost::shared_ptr<CNamespaceService> pointer;

	static CNamespaceService::pointer create(CBodbHandler::pointer bodb, const tstring& space)
	{
		return CNamespaceService::pointer(new CNamespaceService(bodb, space));
	}

	virtual tstring serviceName(void) const {return _T("NAMESPACESERVICE");}
	virtual bool initService(cgcValueInfo::pointer parameter = cgcNullValueInfo)
	{
		if (this->isServiceInited()) return true;
		return cgcServiceInterface::initService();
	}
	virtual void finalService(void)
	{
		if (!isServiceInited()) return;
		cgcServiceInterface::finalService();
		m_bodbHandler.reset();
	}

protected:
	virtual bool get(const tstring& key, std::vector<tstring>& outValues) const
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			for (int i=0; i<resultset->rscount; i++)
			{
				CFieldVariant varValue(resultset->rsvalues[i]->fieldvalues[0]);

				outValues.push_back(varValue.getString());
			}

			bodb_free(resultset);
		}

		return true;
	}
	virtual bool get(std::vector<cgcKVInfo::pointer>& outValues) const
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "SELECT key,value FROM keyvalue_t WHERE namespace='%s'", m_sNamespace.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			for (int i=0; i<resultset->rscount; i++)
			{
				CFieldVariant varKey(resultset->rsvalues[i]->fieldvalues[0]);
				CFieldVariant varValue(resultset->rsvalues[i]->fieldvalues[1]);

				outValues.push_back(CGC_KV(varKey.getString(), varValue.getString()));
			}

			bodb_free(resultset);
		}

		return true;
	}
	virtual tstring get(const tstring& key, const tstring& defaultvalue) const
	{
		if (!isServiceInited()) return defaultvalue;
		if (!m_bodbHandler->isopen()) return defaultvalue;

		tstring result(defaultvalue);
		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			for (int i=0; i<resultset->rscount; i++)
			{
				CFieldVariant varValue(resultset->rsvalues[i]->fieldvalues[0]);

				result = varValue.getString();
				break;
			}

			bodb_free(resultset);
		}

		return result;
	}
	virtual int get(const tstring& key, int defaultvalue) const
	{
		if (!isServiceInited()) return defaultvalue;
		if (!m_bodbHandler->isopen()) return defaultvalue;

		int result = defaultvalue;
		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			for (int i=0; i<resultset->rscount; i++)
			{
				CFieldVariant varValue(resultset->rsvalues[i]->fieldvalues[0]);

				result = varValue.getInt();
				break;
			}

			bodb_free(resultset);
		}

		return result;
	}

	virtual int count(const tstring& key, const tstring& value) const
	{
		if (!isServiceInited()) return -1;
		if (!m_bodbHandler->isopen()) return -1;

		int result = 0;

		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		// ??
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s' and key='%s' and value='%s'",
			m_sNamespace.c_str(), key.c_str(), value.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			result = resultset->rscount;
			bodb_free(resultset);
		}

		return result;
	}
	virtual int count(cgcKVInfo::pointer kv) const
	{
		return kv.get() == NULL ? -1 : count(kv->key(), kv->value());
	}
	virtual int count(const tstring& key) const
	{
		if (!isServiceInited()) return -1;
		if (!m_bodbHandler->isopen()) return -1;

		int result = 0;

		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		// ??
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			result = resultset->rscount;
			bodb_free(resultset);
		}

		return result;
	}
	virtual int count(void) const
	{
		if (!isServiceInited()) return -1;
		if (!m_bodbHandler->isopen()) return -1;

		int result = 0;

		PRESULTSET resultset = 0;
		char sql[2048];
		memset(sql, 0, sizeof(sql));
		// ??
		sprintf(sql, "SELECT value FROM keyvalue_t WHERE namespace='%s'", m_sNamespace.c_str());
		m_bodbHandler->execsql(sql, &resultset);
		if (resultset != 0)
		{
			result = resultset->rscount;
			bodb_free(resultset);
		}

		return result;
	}

	virtual bool exist(const tstring& key, const tstring& value) const
	{
		return count(key, value) > 0;
	}
	virtual bool exist(cgcKVInfo::pointer kv) const
	{
		return count(kv) > 0;
	}
	virtual bool exist(const tstring& key) const
	{
		return count(key) > 0;
	}
	virtual bool exist(void) const
	{
		return count() > 0;
	}

	virtual bool set(cgcKVInfo::pointer kv)
	{
		return kv.get() == NULL ? false : set(kv->key(), kv->value());
	}
	virtual bool set(const tstring& key, const tstring& value)
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "SELECT * FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		int ret = m_bodbHandler->execsql(sql);

		if (ret < 1)
		{
			memset(sql, 0, sizeof(sql));
			sprintf(sql, "INSERT INTO keyvalue_t(namespace,key,value) VALUES('%s','%s','%s')",
				m_sNamespace.c_str(), key.c_str(), value.c_str());
		}else
		{
			memset(sql, 0, sizeof(sql));
			sprintf(sql, "UPDATE keyvalue_t SET value='%s' WHERE namespace='%s' and key='%s'",
				value.c_str(), m_sNamespace.c_str(), key.c_str());
		}
		m_bodbHandler->execsql(sql);
		
		return true;
	}

	virtual bool del(const tstring& key, const tstring& value)
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		char sql[1024];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "DELETE FROM keyvalue_t WHERE namespace='%s' and key='%s' and value='%s'",
			m_sNamespace.c_str(), key.c_str(), value.c_str());
		m_bodbHandler->execsql(sql);
	
		return true;
	}
	virtual bool del(cgcKVInfo::pointer kv)
	{
		return kv.get() == NULL ? false : del(kv->key(), kv->value());
	}
	virtual bool del(const tstring& key)
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		char sql[1024];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "DELETE FROM keyvalue_t WHERE namespace='%s' and key='%s'", m_sNamespace.c_str(), key.c_str());
		m_bodbHandler->execsql(sql);
	
		return true;
	}
	virtual bool del(void)
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		char sql[1024];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "DELETE FROM keyvalue_t WHERE namespace='%s'", m_sNamespace.c_str());
		m_bodbHandler->execsql(sql);
	
		return true;
	}

	virtual bool add(const tstring& key, const tstring& value)
	{
		if (!isServiceInited()) return false;
		if (!m_bodbHandler->isopen()) return false;

		char sql[2048];
		memset(sql, 0, sizeof(sql));
		sprintf(sql, "INSERT INTO keyvalue_t(namespace,key,value) VALUES('%s','%s','%s')",
			m_sNamespace.c_str(), key.c_str(), value.c_str());
		m_bodbHandler->execsql(sql);
		
		return true;
	}
	virtual bool add(cgcKVInfo::pointer kv)
	{
		return kv.get() == NULL ? false : add(kv->key(), kv->value());
	}
	virtual bool add(const std::vector<cgcKVInfo::pointer>& values)
	{
		for (size_t i=0; i<values.size(); i++)
		{
			if (!add(values[i]))
				return false;
		}

		return true;
	}
	virtual bool add(const tstring& key, const std::vector<tstring>& values)
	{
		for (size_t i=0; i<values.size(); i++)
		{
			if (!add(key, values[i]))
				return false;
		}

		return true;
	}

public:
	CNamespaceService(CBodbHandler::pointer bodb, const tstring& sNamespace)
		: m_bodbHandler(bodb), m_sNamespace(sNamespace)
	{
		BOOST_ASSERT (m_bodbHandler.get() != NULL);
	}
	~CNamespaceService(void)
	{
		finalService();
	}

private:
	CBodbHandler::pointer m_bodbHandler;
	tstring m_sNamespace;

};

const int ATTRIBUTE_NAME = 1;
CBodbHandler::pointer theBodbHandler;
cgcAttributes::pointer theAppAttributes;

extern "C" bool CGC_API CGC_Module_Init(void)
{
	theAppAttributes = theApplication->getAttributes(true);
	assert (theAppAttributes.get() != NULL);
	return true;
}

extern "C" void CGC_API CGC_Module_Free(void)
{
	VoidObjectMapPointer mapLogServices = theAppAttributes->getVoidAttributes(ATTRIBUTE_NAME, false);
	if (mapLogServices.get() != NULL)
	{
		CObjectMap<void*>::iterator iter;
		for (iter=mapLogServices->begin(); iter!=mapLogServices->end(); iter++)
		{
			CNamespaceService::pointer service = CGC_OBJECT_CAST<CNamespaceService>(iter->second);
			if (service.get() != NULL)
			{
				service->finalService();
			}
		}
	}
	theAppAttributes.reset();

	bo::bodb_exit(theBodbHandler);
	theBodbHandler.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (parameter.get() == NULL) return;

	cgcParameterMap::pointer initParameters = theApplication->getInitParameters();
	tstring sDatabaseName = initParameters->getParameterValue("DatabaseName", "KV");

	tstring sBodbPath = theApplication->getAppConfPath();
	//sBodbPath.append("/bodb");
	if (theBodbHandler.get() == NULL)
	{
		// Initialize bodb.
		theBodbHandler = bo::bodb_init(sBodbPath.c_str());
		if (theBodbHandler.get() == NULL)
		{
			return;
		}

		// Use KeyValue database.
		if (!theBodbHandler->use(sDatabaseName))
		{
			bo::bodb_exit(theBodbHandler);
			theBodbHandler.reset();
			return;
		}
	}

	tstring sNamespace = parameter->getStr().empty() ? _T("DEFAULT") : parameter->getStr();

	CNamespaceService::pointer serviceCreate = CNamespaceService::create(theBodbHandler, sNamespace);
	if (!serviceCreate->initService()) return;

	outService = serviceCreate;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), serviceCreate);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}

#endif // USES_NAMESPACE
