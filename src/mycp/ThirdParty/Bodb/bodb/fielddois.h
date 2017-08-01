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

// fielddois.h file here
#ifndef __fielddois_h__
#define __fielddois_h__

#include "../stl/stldef.h"
#include "../stl/locklist.h"
#include <boost/shared_ptr.hpp>
#include "data2offsetinfo.h"

namespace bo
{
	class CFieldDOIs
	{
	public:
		typedef boost::shared_ptr<CFieldDOIs> pointer;
		static CFieldDOIs::pointer create(unsigned int fieldid)
		{
			return CFieldDOIs::pointer(new CFieldDOIs(fieldid));
		}

		CLockList<CData2OffsetInfo::pointer> m_dois;

	public:
		unsigned int fieldid(void) const {return m_fieldid;}

	public:
		CFieldDOIs(unsigned int fieldid)
			: m_fieldid(fieldid)
		{
		}
		virtual ~CFieldDOIs(void)
		{
			m_dois.clear();
		}

	private:
		unsigned int	m_fieldid;
	};

} // namespace bo

#endif // __fielddois_h__

