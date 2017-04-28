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

#ifdef _MSC_VER // WIN32
#pragma warning(disable:4267)
#endif // WIN32

#include "CGCClass.h"
#include "ModuleItem.h"

#ifdef WIN32
#include <tchar.h>
#else
#include "tchar.h"
#endif // WIN32

namespace mycp {

ModuleItem::ModuleItem(void)
: m_name(_T(""))
, m_module(_T("")), m_param("")
, m_type(MODULE_UNKNOWN), m_protocol(MODULE_PROTOCOL_SOTP)
, m_bAllowAll(false)
, m_bAuthAccount(false)
, m_nCommPort(0)
, m_pModuleHandle(0)
, m_fpSessionOpen(0), m_fpSessionClose(0), m_fpRemoteChange(0), m_fpRemoteClose(0)
, m_fpGetService(0), m_fpResetService(0)
, m_lockState(LS_NONE)
, m_bDisable(false)

{
}
ModuleItem::ModuleItem(int type, const tstring & name, const tstring & module)
: m_name(name)
, m_module(module), m_param("")
, m_type((MODULETYPE)type), m_protocol(MODULE_PROTOCOL_SOTP)
, m_bAllowAll(false)
, m_bAuthAccount(false)
, m_nCommPort(0)
, m_pModuleHandle(0)
, m_fpSessionOpen(0), m_fpSessionClose(0), m_fpRemoteChange(0), m_fpRemoteClose(0)
, m_fpGetService(0), m_fpResetService(0)
, m_lockState(LS_NONE)
, m_bDisable(false)

{
}
ModuleItem::~ModuleItem(void)
{
	m_mapAllowMethods.clear();
	m_mapAuths.clear();
}

ModuleItem::pointer ModuleItem::create(void)
{
	return ModuleItem::pointer(new ModuleItem());
}

ModuleItem::pointer ModuleItem::create(int type, const tstring & name, const tstring & module)
{
	return ModuleItem::pointer(new ModuleItem(type, name, module));
}

ModuleItem::EncryptionType ModuleItem::getEncryption(const tstring & sET)
{
	if (sET.compare(_T("MD5"))){
		return ET_MD5;
	}
	return ET_NONE;
}

tstring ModuleItem::getEncryption(EncryptionType nET)
{
	switch (nET)
	{
	case ET_MD5:
		return _T("MD5");
	default:
		break;
	}
	return _T("NONE");
}

//
// MODULETYPE
MODULETYPE ModuleItem::getModuleType(const tstring & sModuleType)
{
	MODULETYPE result(MODULE_UNKNOWN);
	if (sModuleType.compare(_T("APP")) == 0)
	{
		result = MODULE_APP;
	}else if (sModuleType.compare(_T("COMM")) == 0)
	{
		result = MODULE_COMM;
	}else if (sModuleType.compare(_T("PARSER")) == 0)
	{
		result = MODULE_PARSER;
	}else if (sModuleType.compare(_T("SERVER")) == 0)
	{
		result = MODULE_SERVER;
	}else if (sModuleType.compare(_T("LOG")) == 0)
	{
		result = MODULE_LOG;
	}
	return result;
}

tstring ModuleItem::getModuleType(MODULETYPE moduleType)
{
	tstring result(_T("UNKNOWN"));
	switch (moduleType)
	{
	case MODULE_APP:
		result = _T("APP");
		break;
	case MODULE_COMM:
		result = _T("COMM");
		break;
	case MODULE_PARSER:
		result = _T("PARSER");
		break;
	case MODULE_SERVER:
		result = _T("SERVER");
		break;
	case MODULE_LOG:
		result = _T("LOG");
		break;
	default:
		break;
	}
	return result;
}
//
// LockState
ModuleItem::LockState ModuleItem::getLockState(const tstring & sLockState)
{
	LockState result(LS_NONE);
	if (sLockState.compare(_T("LS_WAIT")) == 0)
	{
		result = LS_WAIT;
	}else if (sLockState.compare(_T("LS_RETURN")) == 0)
	{
		result = LS_RETURN;
	}
	return result;
}
tstring ModuleItem::getLockState(LockState lockState)
{
	tstring result(_T("LS_NONE"));
	switch (lockState)
	{
	case LS_WAIT:
		result = _T("LS_WAIT");
		break;
	case LS_RETURN:
		result = _T("LS_RETURN");
		break;
	default:
		break;
	}
	return result;
}

bool ModuleItem::getAllowMethod(const tstring & invokeName, tstring & methodName) const
{
	StringMapIter pIter = m_mapAllowMethods.find(invokeName);
	if (pIter != m_mapAllowMethods.end())
	{
		methodName = pIter->second;
		return true;
	}
	if (m_bAllowAll)
	{
		methodName = invokeName;
		return true;
	}
	return false;
}

bool ModuleItem::authAccount(const tstring & account, const tstring & passwd) const
{
	//
	// 不必验证用户
	//
	if (!this->m_bAuthAccount) return true;

	StringMapIter pIter = m_mapAuths.find(account);
	if (pIter != m_mapAuths.end())
	{
		return passwd.compare(pIter->second) == 0;
	}
	return false;
}
	
// ? SOTP/2.0
/* ?	tstring getString(void) const{
boost::wformat fModule(_T("<module appname=\"%s\" />"));
tstring result((fModule%m_name.c_str()).str());
return result;
}*/

void ModuleItem::Serialize(bool isStoring, std::fstream& ar)
{
	if (!ar.is_open()) return;

	//if (isStoring)
	//{
	//	tstring::size_type len = 0;

	//	// m_name
	//	len = m_name.length();
	//	ar.write((const TCHAR*)(&len), sizeof(tstring::size_type));
	//	ar.write(m_name.c_str(), len);

	//	// m_module
	//	len = m_module.length();
	//	ar.write((const TCHAR*)(&len), sizeof(tstring::size_type));
	//	ar.write(m_module.c_str(), len);

	//	// m_type
	//	ar.write((const TCHAR*)(&m_type), sizeof(m_type));

	//	// m_bAllowAll
	//	ar.write((const TCHAR*)(&m_bAllowAll), sizeof(m_bAllowAll));

	//	// m_bAuthAccount
	//	ar.write((const TCHAR*)(&m_bAuthAccount), sizeof(m_bAuthAccount));

	//	// m_nCommPort
	//	ar.write((const TCHAR*)(&m_nCommPort), sizeof(m_nCommPort));

	//	// m_lockState
	//	ar.write((const TCHAR*)(&m_lockState), sizeof(m_lockState));
	//}else
	//{
	//	TCHAR * buffer = 0;
	//	tstring::size_type len = 0;

	//	// m_name
	//	ar.read((TCHAR*)(&len), sizeof(tstring::size_type));
	//	buffer = new TCHAR[len+1];
	//	ar.read(buffer, len);
	//	buffer[len] = '\0';
	//	m_name = buffer;
	//	delete []buffer;

	//	// m_module
	//	ar.read((TCHAR*)(&len), sizeof(tstring::size_type));
	//	buffer = new TCHAR[len+1];
	//	ar.read(buffer, len);
	//	buffer[len] = '\0';
	//	m_module = buffer;
	//	delete []buffer;

	//	// m_type
	//	ar.read((TCHAR*)(&m_type), sizeof(m_type));

	//	// m_bAllowAll
	//	ar.read((TCHAR*)(&m_bAllowAll), sizeof(m_bAllowAll));

	//	// m_bAuthAccount
	//	ar.read((TCHAR*)(&m_bAuthAccount), sizeof(m_bAuthAccount));

	//	// m_nCommPort
	//	ar.read((TCHAR*)(&m_nCommPort), sizeof(m_nCommPort));

	//	// m_lockState
	//	ar.read((TCHAR*)(&m_lockState), sizeof(m_lockState));
	//}
}


} // namespace mycp

