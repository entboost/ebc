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

// cgcuses.h file here
#ifndef __cgcuses_head__
#define __cgcuses_head__

namespace mycp {

	// Use jrtp library.
#define USES_RTP		0		// [0,1]

	// Use osip and eXosip library.
#define USES_SIP		0		// [0,1]

	// Use bodb database library.
#define USES_BODB		1		// [0,1]

}

#endif // __cgcuses_head__
