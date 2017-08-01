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

// fieldcompare.h file here
#ifndef __fieldcompare_h__
#define __fieldcompare_h__

#include "fieldinfo.h"
#include "fieldvariant.h"

namespace bo
{
	class CFieldCompare
	{
	public:
		enum FieldCompareType
		{
			FCT_EQUAL = 0x1
			, FCT_UNEQUAL
			, FCT_GREATER
			, FCT_GREATEREQUAL
			, FCT_LESS
			, FCT_LESSEQUAL
			, FCT_ISNULL
			, FCT_ISNOTNULL
			, FCT_BETWEENAND
			, FCT_LIKE

			, FCT_UNKNOWN		= 0xf
		};

		typedef boost::shared_ptr<CFieldCompare> pointer;
		//static CFieldCompare::pointer create(void)
		//{
		//	return CFieldCompare::pointer(new CFieldCompare());
		//}
		static CFieldCompare::pointer create(const CTableInfo::pointer& tableInfo, const CTableInfo::pointer& tableInfo2, FieldCompareType type, const CFieldInfo::pointer& field, const CFieldInfo::pointer& field2,short nWhereLevel)
		{
			return CFieldCompare::pointer(new CFieldCompare(tableInfo, tableInfo2, type, field, field2, nWhereLevel));
		}
		static CFieldCompare::pointer create(const CTableInfo::pointer& tableInfo, FieldCompareType type, const CFieldInfo::pointer& field,short nWhereLevel)
		{
			return CFieldCompare::pointer(new CFieldCompare(tableInfo, type, field,nWhereLevel));
		}
		static CFieldCompare::pointer create(const CTableInfo::pointer& tableInfo, FieldCompareType type, const CFieldInfo::pointer& field,const CFieldVariant::pointer& variant,short nWhereLevel)
		{
			return CFieldCompare::pointer(new CFieldCompare(tableInfo, type, field, variant,nWhereLevel));
		}

		void compareAnd(bool newv) {m_compareAnd = newv;}
		bool compareAnd(void) const {return m_compareAnd;}

		const CTableInfo::pointer& tableInfo(void) const {return m_tableInfo;}
		const CTableInfo::pointer& tableInfo2(void) const {return m_tableInfo2;}
		FieldCompareType compareType(void) const {return m_compareType;}
		const CFieldInfo::pointer& compareField(void) const {return m_compareField;}
		const CFieldInfo::pointer& compareField2(void) const {return m_compareField2;}
		const CFieldVariant::pointer& compareVariant(void) const {return m_compareVariant;}
		short whereLevel(void) const {return m_nWhereLevel;}
		//void SetFinished(void) {m_bFinished = true;}
		//bool GetFinished(void) const {return m_bFinished;}

		bool doCompare(const CFieldVariant::pointer& doCompareVariant)
		{
			BOOST_ASSERT (doCompareVariant.get() != 0);

			bool result = false;
			switch (m_compareType)
			{
			case FCT_EQUAL:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->equal(m_compareVariant);
				break;
			case FCT_UNEQUAL:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->unequal(m_compareVariant);
				break;
			case FCT_GREATER:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->greater(m_compareVariant);
				break;
			case FCT_GREATEREQUAL:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->greaterEqual(m_compareVariant);
				break;
			case FCT_LESS:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->less(m_compareVariant);
				break;
			case FCT_LESSEQUAL:
				BOOST_ASSERT (m_compareVariant.get() != 0);
				result = doCompareVariant->lessEqual(m_compareVariant);
				break;
			case FCT_ISNULL:
				result = doCompareVariant->isNull();
				break;
			case FCT_ISNOTNULL:
				result = doCompareVariant->isNotNull();
				break;
			case FCT_LIKE:
				result = doCompareVariant->isLike(m_compareVariant);
				break;
			default:
				break;
			}

			return result;
		}

	public:
		//CFieldCompare(void)
		//	: m_compareAnd(true), m_compareType(FCT_UNKNOWN)
		//{}
		CFieldCompare(const CTableInfo::pointer& tableInfo, const CTableInfo::pointer& tableInfo2, FieldCompareType type,const CFieldInfo::pointer& field,const CFieldInfo::pointer& field2,short nWhereLevel)
			: m_tableInfo(tableInfo), m_tableInfo2(tableInfo2), m_compareAnd(true), m_compareType(type), m_compareField(field), m_compareField2(field2), m_nWhereLevel(nWhereLevel)
			//, m_bFinished(false)
		{
			BOOST_ASSERT (m_tableInfo.get() != 0);
			BOOST_ASSERT (m_tableInfo2.get() != 0);
			BOOST_ASSERT (m_compareField.get() != 0);
			BOOST_ASSERT (m_compareField2.get() != 0);
		}
		CFieldCompare(const CTableInfo::pointer& tableInfo, FieldCompareType type,const CFieldInfo::pointer& field,short nWhereLevel)
			: m_tableInfo(tableInfo), m_compareAnd(true), m_compareType(type), m_compareField(field),m_nWhereLevel(nWhereLevel)
			//, m_bFinished(false)
		{
			BOOST_ASSERT (m_tableInfo.get() != 0);
			BOOST_ASSERT (m_compareField.get() != 0);
		}
		CFieldCompare(const CTableInfo::pointer& tableInfo, FieldCompareType type, const CFieldInfo::pointer& field, const CFieldVariant::pointer& variant,short nWhereLevel)
			: m_tableInfo(tableInfo), m_compareAnd(true), m_compareType(type), m_compareField(field), m_compareVariant(variant),m_nWhereLevel(nWhereLevel)
			//, m_bFinished(false)
		{
			BOOST_ASSERT (m_tableInfo.get() != 0);
			BOOST_ASSERT (m_compareField.get() != 0);
			BOOST_ASSERT (m_compareVariant.get() != 0);
		}
		virtual ~CFieldCompare(void)
		{}

	private:
		CTableInfo::pointer		m_tableInfo;
		CTableInfo::pointer		m_tableInfo2;
		bool					m_compareAnd;
		FieldCompareType		m_compareType;
		CFieldInfo::pointer		m_compareField;
		CFieldInfo::pointer		m_compareField2;
		CFieldVariant::pointer	m_compareVariant;
		short					m_nWhereLevel;		// (xxx) OR (xxx)
		//bool					m_bFinished;
	};

} // namespace bo

#endif // __fieldcompare_h__

