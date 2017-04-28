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

// cgcCDBCService.h file here
#ifndef __cgccdbcservice_head__
#define __cgccdbcservice_head__

#include <boost/shared_ptr.hpp>
#include "cgccdbcs.h"
#include "cgcService.h"

namespace mycp {

class cgcCDBCService
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcCDBCService> pointer;

	virtual void escape_string_in(tstring & str) = 0;
	virtual void escape_string_out(tstring & str) = 0;

	virtual bool open(const cgcCDBCInfo::pointer& cdbcInfo) = 0;
	virtual bool open(void) = 0;
	virtual void close(void) = 0;
	virtual bool isopen(void) const = 0;
	virtual time_t lasttime(void) const = 0;

	// INSERT, UPDATE, DELETE
	// >=  0 : OK
	// == -1 : ERROR
	virtual mycp::bigint execute(const char * exeSql) {return execute(exeSql,0);}
	virtual mycp::bigint execute(const char * exeSql, int nTransaction) = 0;

	// SELECT
	// outCookie > 0 : OK
	virtual mycp::bigint select(const char * selectSql) {return 0;}
	virtual mycp::bigint select(const char * selectSql, int& outCookie, int nCacheMinutes = 0) = 0;

	// Return ResultSet count
	virtual mycp::bigint size(int cookie) const = 0;
	// rows: -1=not exist
	virtual mycp::bigint rows(int cookie) const {return this->size(cookie);}
	// cols: -1=not exist
	virtual int cols(int cookie) const = 0;

	// Return current index.
	virtual mycp::bigint index(int cookie) const = 0;

	virtual cgcValueInfo::pointer cols_name(int cookie) const = 0;
	virtual cgcValueInfo::pointer index(int cookie, mycp::bigint moveIndex) = 0;
	virtual cgcValueInfo::pointer first(int cookie) = 0;
	virtual cgcValueInfo::pointer next(int cookie) = 0;
	virtual cgcValueInfo::pointer previous(int cookie) = 0;
	virtual cgcValueInfo::pointer last(int cookie) = 0;
	virtual void reset(int cookie) = 0;

	// * new version
	virtual int trans_begin(void) {return 0;}
	virtual bool trans_rollback(int nTransaction) {return false;}
	virtual mycp::bigint trans_commit(int nTransaction) {return 0;}

	virtual bool backup_database(const char * sBackupTo, const char* sProgram) {return false;}
	virtual bool restore_database(const char * sRestoreFrom, const char* sProgram) {return false;}

	// * old version
	virtual bool auto_commit(bool autocommit) {return false;}
	virtual bool commit(void) {return false;}
	virtual bool rollback(void) {return false;}

	void set_datasource(const tstring& v) {m_sDatasource = v;}
	const tstring& get_datasource(void) const {return m_sDatasource;}
private:
	tstring m_sDatasource;
};

const cgcCDBCService::pointer cgcNullCDBCService;

#define CGC_CDBCSERVICE_DEF(s) boost::static_pointer_cast<cgcCDBCService, cgcServiceInterface>(s)

} // namespace mycp

#endif // __cgccdbcservice_head__
