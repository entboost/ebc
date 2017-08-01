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

// bodb2.h file here
#ifndef __bodb2_h__
#define __bodb2_h__

#include "bodbhandler.h"

namespace bo
{
	CBodbHandler::pointer bodb_init(const char * path = "");
	void bodb_exit(CBodbHandler::pointer &);

	void bodb_free(PRESULTSET resultset);

	void bodb_escape_string_in(std::string & sql);
	void bodb_escape_string_out(std::string & sql);
} // namespace

#ifdef WIN32
#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "bodb60d.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "bodb70d.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "bodb71d.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "bodb80d.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "bodb90d.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "bodb60.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "bodb70.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "bodb71.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "bodb80.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "bodb90.lib")
#endif

#endif // _DEBUG
#endif // WIN32

#endif // __bodb2_h__
