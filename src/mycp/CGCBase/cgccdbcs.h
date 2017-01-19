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

// cgccdbcs.h file here
#ifndef __cgccdbcs_head__
#define __cgccdbcs_head__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "cgcobject.h"

namespace mycp {

class cgcCDBCInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<cgcCDBCInfo> pointer;

	void setName(const tstring& v) {m_name = v;}
	const tstring& getName(void) const {return m_name;}
	void setDatabase(const tstring& v) {m_database = v;}
	const tstring& getDatabase(void) const {return m_database;}
	void setHost(const tstring& v) {m_host = v;}
	const tstring& getHost(void) const {return m_host;}

	void setAccount(const tstring& v) {m_account = v;}
	const tstring& getAccount(void) const {return m_account;}
	void setSecure(const tstring& v) {m_secure = v;}
	const tstring& getSecure(void) const {return m_secure;}

	void setConnection(const tstring& v) {m_connection = v;}
	const tstring& getConnection(void) const {return m_connection;}

	void setCharset(const tstring& v) {m_charset = v;}
	const tstring& getCharset(void) const {return m_charset;}

	void setMinSize(unsigned short v) {m_nMinSize = v;}
	unsigned short getMinSize(void) const {return m_nMinSize;}
	void setMaxSize(unsigned short v) {m_nMaxSize = v;}
	unsigned short getMaxSize(void) const {return m_nMaxSize;}

	//void setSyncName(const tstring& v) {m_sSyncName = v;}
	//const tstring& getSyncName(void) const {return m_sSyncName;}

	cgcCDBCInfo(const tstring& name, const tstring& database)
		: m_name(name), m_database(database), m_host("")
		, m_account(""), m_secure("")
		, m_connection(""), m_charset("")
		, m_nMinSize(3),m_nMaxSize(10)
	{}
	virtual cgcObject::pointer copyNew(void) const
	{
		cgcCDBCInfo::pointer result = cgcCDBCInfo::pointer(new cgcCDBCInfo(m_name,m_database));
		result->setHost(this->m_host);
		result->setAccount(this->m_account);
		result->setSecure(this->m_secure);
		result->setConnection(this->m_connection);
		result->setCharset(this->m_charset);
		result->setMinSize(this->m_nMinSize);
		result->setMaxSize(this->m_nMaxSize);
		return result;		
	}

protected:
	tstring m_name;
	tstring m_database;
	tstring m_host;
	tstring m_account;
	tstring m_secure;
	tstring m_connection;
	tstring m_charset;
	unsigned short m_nMinSize;	// min connection
	unsigned short m_nMaxSize;	// max connection
	//tstring m_sSyncName;
};
const cgcCDBCInfo::pointer cgcNullCDBCInfo;

#define CGC_CDBCINFO(name,database) cgcCDBCInfo::pointer(new cgcCDBCInfo(name,database))

class cgcDataSourceInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<cgcDataSourceInfo> pointer;

	const tstring& getName(void) const {return m_name;}
	const tstring& getCDBCService(void) const {return m_cdbcService;}
	cgcCDBCInfo::pointer getCDBCInfo(void) const {return m_cdbcInfo;}

	cgcDataSourceInfo(const tstring& name, const tstring& cdbcService, cgcCDBCInfo::pointer cdbcInfo)
		: m_name(name), m_cdbcService(cdbcService), m_cdbcInfo(cdbcInfo)
	{
		assert (m_cdbcInfo.get() != NULL);
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		cgcCDBCInfo::pointer pNewCDBCInfo = CGC_OBJECT_CAST<cgcCDBCInfo>(m_cdbcInfo->copyNew());
		cgcDataSourceInfo::pointer result = cgcDataSourceInfo::pointer(new cgcDataSourceInfo(m_name,m_cdbcService,pNewCDBCInfo));
		return result;
	}
private:
	tstring m_name;
	tstring m_cdbcService;
	cgcCDBCInfo::pointer m_cdbcInfo;
};
const cgcDataSourceInfo::pointer cgcNullDataSourceInfo;

#define CGC_DSINFO(name,cdbcservice,cdbcinfo) cgcDataSourceInfo::pointer(new cgcDataSourceInfo(name,cdbcservice,cdbcinfo))

} // namespace mycp

#endif // __cgccdbcs_head__
