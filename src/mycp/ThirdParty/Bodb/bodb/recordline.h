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

// recordline.h file here
#ifndef __recordline_h__
#define __recordline_h__

#include <stl/stldef.h>
#include <stl/lockmap.h>
#include "tableinfo.h"
#include "fieldvariant.h"
#include "fieldcompare.h"
#include <boost/thread/mutex.hpp>

namespace bo
{
//#define RECORD_LINE_EXT_DATA_TRUE_RESULT_FLAG	0x1
//#define RECORD_LINE_EXT_DATA_DELETE_FLAG		0x2

	class CRecordLine
	{
	public:
		typedef boost::shared_ptr<CRecordLine> pointer;
		static CRecordLine::pointer create(const CTableInfo::pointer& tableInfo);
		static CRecordLine::pointer create(uinteger id, const CTableInfo::pointer& tableInfo);

		CFieldVariant::pointer getVariant(uinteger fieldId) const;
		CFieldVariant::pointer getVariant(const tstring & fieldName) const;
		bool equalFieldVariant(uinteger fieldId, const CFieldVariant::pointer& compare) const;
		bool doCompareFieldVariant(uinteger fieldId, CFieldCompare::FieldCompareType compareType, const CFieldVariant::pointer& compare) const;

		void addVariant(const CFieldInfo::pointer& fieldInfo, const CFieldVariant::pointer& variant, bool bAddForce = true);
		void addVariant(bo::uinteger fieldId, const tstring& fieldName, const CFieldVariant::pointer& variant, bool bAddForce = true);
		void delVariant(const CFieldInfo::pointer& fieldInfo);

		bool addVariant(const tstring & fieldName, bool value);
		bool addVariantInt(const tstring & fieldName, int value);
		bool addVariantReal(const tstring & fieldName, double value);
		bool addVariantUInt(const tstring & fieldName, uinteger value);
		bool addVariantBigInt(const tstring & fieldName, bigint value);
		bool addVariant(const tstring & fieldName, const char * value, uinteger size);
		bool addVariantNull(const tstring & fieldName);
		bool addVariantDefault(const tstring & fieldName);
		bool addVariantCurrentTime(const tstring & fieldName);

		bool addVariant(short fieldIndex, const char * value, uinteger size);
		bool addVariant(short fieldIndex, bool value);
		bool addVariantInt(short fieldIndex, int value);
		bool addVariantReal(short fieldIndex, double value);
		bool addVariantBigInt(short fieldIndex, bigint value);
		bool addVariantNull(short fieldIndex);
		bool addVariantDefault(short fieldIndex);
		bool addVariantCurrentTime(short fieldIndex);
		bool addVariantDefault2(uinteger fieldId);

		bool setNullDefaultVariant(uinteger fieldId);

		void updateVariant(const CFieldInfo::pointer& fieldInfo, const CFieldVariant::pointer& fieldVariant);
		void setVariantNull(const CFieldInfo::pointer& fieldInfo);

		CRecordLine::pointer Clone(bo::uinteger recordId) const;
		void AddRecordLine(const CRecordLine::pointer& pRecordLine, bool bAddForce = true);
		const CLockMap<uinteger, CFieldVariant::pointer>& GetVariantList(void) const {return m_variants;}
		//CFieldVariant::pointer moveFirst(void);
		//CFieldVariant::pointer moveNext(void);
		//CFieldVariant::pointer movePrev(void);
		//CFieldVariant::pointer moveLast(void);
		//uinteger getFieldId(void) const;
		//CFieldInfo::pointer getFieldInfo(void) const;
		uinteger size(void) const;

		void id(uinteger newv) {m_id = newv;}
		uinteger id(void) const {return m_id;}
		CTableInfo::pointer	tableInfo(void) const {return m_tableInfo;}
		
		usmallint getLineSize(void) const;
		//bo::uinteger m_nExtData;
		CLockMap<void*,bool> m_pExtDataList;
	public:
		CRecordLine(uinteger id, const CTableInfo::pointer& tableInfo);
		virtual ~CRecordLine(void);

		virtual void Serialize(bool isStoring, tfstream& ar, usmallint & outDoSize);
	private:
		uinteger m_id;
		CTableInfo::pointer	m_tableInfo;
		CLockMap<uinteger, CFieldVariant::pointer> m_variants;	// FieldId ->
		CLockMap<tstring, CFieldVariant::pointer> m_variants2;		// FieldName ->
		//CLockMap<uinteger, CFieldVariant::pointer>::iterator m_curiter;
		//boost::shared_mutex m_mutex; 
	};

	const CRecordLine::pointer boNullRecordLine;

} // namespace bo

#endif // __recordline_h__

