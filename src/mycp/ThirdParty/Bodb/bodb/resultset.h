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

// resultset.h file here
#ifndef __resultset_h__
#define __resultset_h__

#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include "tableinfo.h"
#include "recordline.h"

namespace bo
{
	class CResultSet
	{
	public:
		typedef boost::shared_ptr<CResultSet> pointer;
		static CResultSet::pointer create(CTableInfo::pointer tableInfo)
		{
			return CResultSet::pointer(new CResultSet(tableInfo));
		}

		void OrderBy(const std::vector<bo::uinteger>& pOrderBys, bool bDesc = false);
		void LimitOffset(bo::bigint nOffset,bo::bigint nLimit);

		const CLockList<CRecordLine::pointer>& GetRecordList(void) const {return m_records;}
		CLockList<CRecordLine::pointer>& GetRecordList2(void) {return m_records;}
		void RemoteRecord(bo::uinteger nRecordId);
		//CRecordLine::pointer moveFirst(void);
		//CRecordLine::pointer moveNext(void);
		//CRecordLine::pointer movePrev(void);
		//CRecordLine::pointer moveLast(void);
		//CRecordLine::pointer deleteCurrent(void);	// Delete OK MoveNext.
		uinteger size(void) const;
		bool empty(void) const;

		void close(void);
		void getFieldEqualList(bo::uinteger fieldId, const CFieldVariant::pointer& variant, std::vector<CRecordLine::pointer>& pOutList);
		void addRecord(const CRecordLine::pointer& record);
		CRecordLine::pointer getRecord(uinteger recordId);

		CTableInfo::pointer	getTableInfo(void) const {return m_tableInfo;}
		
	public:
		CResultSet(CTableInfo::pointer tableInfo)
			: m_tableInfo(tableInfo)
		{
			BOOST_ASSERT (m_tableInfo.get() != 0);
			//m_curiter = m_records.begin();
		}
		virtual ~CResultSet(void)
		{
			close();
		}

	private:
		CTableInfo::pointer	m_tableInfo;
		CLockList<CRecordLine::pointer> m_records;
		CLockMap<uinteger, CRecordLine::pointer> m_records2;	// RecordId ->
		//CLockList<CRecordLine::pointer>::iterator m_curiter;
	};

	const CResultSet::pointer boNullResultSet;

} // namespace bo

#endif // __resultset_h__

