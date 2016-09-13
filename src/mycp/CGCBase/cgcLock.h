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

// cgcLock.h file here
#ifndef __cgclock_head__
#define __cgclock_head__

#include <boost/thread/mutex.hpp>

namespace mycp {

class cgcLock
{
public:
	boost::mutex & GetMutex(void) {return m_mutex;}
	const boost::mutex & GetMutex(void) const {return m_mutex;}

protected:
	boost::mutex m_mutex;
};


#ifndef CGC_AUTO_LOCK
#define CGC_AUTO_LOCK(l) boost::mutex::scoped_lock lock(l.GetMutex())
#endif // CGC_AUTO_LOCK

} // namespace mycp

#endif // __cgclock_head__
