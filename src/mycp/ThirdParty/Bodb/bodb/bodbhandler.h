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

// bodbhandler.h file here
#ifndef __bodbhandler_h__
#define __bodbhandler_h__

#include <boost/shared_ptr.hpp>
#include "bodbdef.h"
#include "database.h"

namespace bo
{
	void bodb_free(PRESULTSET resultset);

	class CBodbHandler
	{
	public:
		typedef boost::shared_ptr<CBodbHandler> pointer;

		virtual bool start(void) = 0;
		virtual bool isstarted(void) const = 0;
		virtual void stop(void) = 0;

		virtual int execsql(const char * sql, PRESULTSET * outResultSet = 0) = 0;

		virtual CDatabase::pointer create(const tstring & dbname) = 0;
		virtual void setaccount(const tstring & account, const tstring & password) = 0;
		virtual bool use(const tstring & dbname) = 0;
		virtual bool isopen(void) const = 0;
		virtual void close(void) = 0;
		virtual bool exist(const tstring & dbname) const = 0;
		virtual bool drop(const tstring & dbname) = 0;

		virtual bool rename(const tstring & tableName, const tstring & newName) = 0;
		virtual bool dropdefault(const tstring & tableName, const tstring & fieldName) = 0;
		virtual bool setdefault(const tstring & tableName, const tstring & fieldName, tagItemValue * defaultValue) = 0;

		virtual bool setoption(OptionType nOption, bo::uinteger nValue) = 0;

	};

	const CBodbHandler::pointer boNullBodbHandler;

} // namespace


#endif // __bodbhandler_h__

