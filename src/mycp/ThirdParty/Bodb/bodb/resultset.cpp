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

#include "resultset.h"


namespace bo
{
	struct compare_records
	{
		bool m_bDesc;
		bo::uinteger m_orderbyField;
		bool operator()(const CRecordLine::pointer& line1, const CRecordLine::pointer& line2)
		{
			const CFieldVariant::pointer fieldvar1 = line1->getVariant(m_orderbyField);
			const CFieldVariant::pointer fieldvar2 = line2->getVariant(m_orderbyField);
			if (fieldvar1.get() == 0 || fieldvar2.get() == 0)
				return false;

			if (m_bDesc)
			{
				if (fieldvar1->greater(fieldvar2))
				{
					return true;
				}else
				{
					return false;
				}
			}else
			{
				if (fieldvar1->less(fieldvar2))
				{
					return true;
				}else
				{
					return false;
				}
			}
		}
	};

	void CResultSet::OrderBy(const std::vector<bo::uinteger>& pOrderBys, bool bDesc)
	{
		if (!pOrderBys.empty())
		{
			compare_records compare;
			compare.m_bDesc = bDesc;
			for (size_t i=0; i<pOrderBys.size(); i++)
			{
				compare.m_orderbyField = pOrderBys[i];
				m_records.sort(compare);
			}
			//moveFirst();
		}
	}
	void CResultSet::LimitOffset(bo::bigint nOffset,bo::bigint nLimit)
	{
		// *offset pop_front
		while(nOffset>0 && nOffset>0)
		{
			m_records.pop_front();
			nOffset--;
		}
		// *limit pop_back
		while(nLimit>0 && m_records.size()>nLimit)
		{
			m_records.pop_back();
		}
	}
	void CResultSet::RemoteRecord(bo::uinteger nRecordId)
	{
		m_records2.remove(nRecordId);
	}

	//CRecordLine::pointer CResultSet::moveFirst(void)
	//{
	//	m_curiter = m_records.begin();
	//	if (m_curiter == m_records.end())
	//		return boNullRecordLine;

	//	return *m_curiter;
	//}

	//CRecordLine::pointer CResultSet::moveNext(void)
	//{
	//	if (++m_curiter == m_records.end())
	//		return boNullRecordLine;

	//	return *m_curiter;
	//}

	//CRecordLine::pointer CResultSet::movePrev(void)
	//{
	//	if (m_curiter == m_records.begin() || --m_curiter == m_records.begin())
	//		return boNullRecordLine;

	//	return *m_curiter;
	//}

	//CRecordLine::pointer CResultSet::moveLast(void)
	//{
	//	m_curiter = m_records.end();
	//		if (m_curiter == m_records.begin())
	//		return boNullRecordLine;

	//	m_curiter--;
	//	return *m_curiter;
	//}

	//CRecordLine::pointer CResultSet::deleteCurrent(void)
	//{
	//	if (m_curiter == m_records.end())
	//		return boNullRecordLine;

	//	m_curiter = m_records.erase(m_curiter);
	//	if (m_curiter == m_records.end())
	//		return boNullRecordLine;
	//	else
	//		return *m_curiter;
	//}
	uinteger CResultSet::size(void) const
	{
		return (uinteger)m_records.size();
	}
	bool CResultSet::empty(void) const
	{
		return m_records.empty();
	}
	void CResultSet::close(void)
	{
		m_records.clear();
		m_records2.clear();
	}
	void CResultSet::getFieldEqualList(bo::uinteger fieldId, const CFieldVariant::pointer& variant, std::vector<CRecordLine::pointer>& pOutList)
	{
		BoostReadLock rdLock(m_records.mutex());
		CLockList<CRecordLine::pointer>::iterator pIter = m_records.begin();
		for (; pIter!=m_records.end(); pIter++)
		{
			CRecordLine::pointer pRecordLine = *pIter;
			if (pRecordLine->equalFieldVariant(fieldId,variant))
			{
				pOutList.push_back(pRecordLine);
			}
		}
	}

	void CResultSet::addRecord(const CRecordLine::pointer& record)
	{
		BOOST_ASSERT (record.get() != 0);
		m_records.add(record);
		m_records2.insert(record->id(), record);
	}
	CRecordLine::pointer CResultSet::getRecord(uinteger recordId)
	{
		CRecordLine::pointer result;
		m_records2.find(recordId, result);
		return result;
	}

} // namespace bo

