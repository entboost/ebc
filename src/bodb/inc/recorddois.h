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

// recorddois.h file here
#ifndef __recorddois_h__
#define __recorddois_h__

#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include <boost/shared_ptr.hpp>
#include "fielddois.h"

namespace bo
{
	class CRecordDOIs
	{
	public:
		typedef boost::shared_ptr<CRecordDOIs> pointer;
		static CRecordDOIs::pointer create(unsigned int recordid)
		{
			return CRecordDOIs::pointer(new CRecordDOIs(recordid));
		}

		CLockMap<unsigned int, CFieldDOIs::pointer> m_dois;		// FieldId ->

	public:
		unsigned int recordid(void) const {return m_recordid;}

	public:
		CRecordDOIs(unsigned int recordid)
			: m_recordid(recordid)
		{
		}
		virtual ~CRecordDOIs(void)
		{
			m_dois.clear();
		}

	private:
		unsigned int	m_recordid;
	};

} // namespace bo

#endif // __recorddois_h__

