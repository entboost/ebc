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

// fieldinfo.h file here
#ifndef __fieldinfo_h__
#define __fieldinfo_h__

#include "../stl/stldef.h"
#include "../stl/locklist.h"
#include <boost/shared_ptr.hpp>
#include "bodbdef.h"
#include "fieldvariant.h"

namespace bo
{
	const usmallint MAX_FIELDINFO_SIZE =
		INTEGER_SIZE								// m_id
		+ 4											// m_type
		+ SMALLINT_SIZE+MAX_NAME_SIZE				// m_name
		+ SMALLINT_SIZE+MAX_NOTE_SIZE				// m_note
		+ INTEGER_SIZE								// m_size1
		+ SMALLINT_SIZE								// m_size2
		+ 4											// m_constraint
		+ BOOLEAN_SIZE								// m_references
		+ BOOLEAN_SIZE								// m_autoincrement
		+ 4+MAX_DEFAULT_VARIANT_SIZE				// default_value
		;

	class CFieldInfo
	{
	public:
		typedef boost::shared_ptr<CFieldInfo> pointer;
		static CFieldInfo::pointer create(void);
		static CFieldInfo::pointer create(const tstring & name, FieldType type, uinteger size1 = 0, const tstring & note = _T(""));
		//static usmallint getMaxFieldInfoSize(void);
		//static usmallint getFieldTypeSize(FieldType FieldType);

		enum DefaultValueType
		{
			DVT_None
			, DVT_Variant
			, DVT_CurrentTimestamp

		};

	public:
		void id(uinteger newv) {m_id = newv;}
		uinteger id(void) const {return m_id;}
		const tstring & name(void) const {return m_name;}
		FieldType type(void) const {return m_type;}
		void note(const tstring & newv) {m_note = newv;}
		const tstring & note(void) const {return m_note;}

		// size1 & size2
		uinteger size1(void) const {return m_size1;}
		void size2(usmallint newv) {m_size2 = newv;}
		uinteger size2(void) const {return m_size2;}
		uinteger linedatasize(void) const {return m_datasize;}
		usmallint defdatasize(void) const {return m_defdatasize;}

		// 
		void constraintType(FieldConstraintType newv) {m_constraint = newv;}
		FieldConstraintType constraintType(void) const {return m_constraint;}
		bool isnotnull(void) const {return m_constraint == FIELD_CONSTRAINT_NOTNULL;}
		bool isunique(void) const {return m_constraint == FIELD_CONSTRAINT_UNIQUE;}
		bool isprimarykey(void) const {return m_constraint == FIELD_CONSTRAINT_PRIMARYKEY;}

		void references(bool newv) {m_references = newv;}
		bool references(void) const {return m_references;}
		void autoincrement(bool newv) {m_autoincrement = newv;}
		bool autoincrement(void) const {return m_autoincrement;}

		//void defaultValue(CFieldVariant::pointer newv) {m_default = newv;}
		CFieldVariant::pointer buildFieldVariant(void) const;
		// Default variant
		void defaultValueType(DefaultValueType newv);
		DefaultValueType defaultValueType(void) const {return m_defaultValueType;}

		CFieldVariant::pointer buildDefaultValue(void);
		CFieldVariant::pointer defaultValue(void) const {return m_default;}
		bool hasDefaultValue(void) const {return m_default.get() != 0;}
		void dropDefaultValue(void);

	public:
		CFieldInfo(void);
		CFieldInfo(const tstring & name, FieldType type, uinteger size1, const tstring & note);
		virtual ~CFieldInfo(void);

		virtual void Serialize(bool isStoring, tfstream& ar);

	private:
		void countDataSize(void);

	private:
		uinteger			m_id;
		FieldType				m_type;
		tstring					m_name;
		tstring					m_note;
		uinteger			m_size1;
		usmallint			m_size2;
		FieldConstraintType		m_constraint;
		tstring					m_constraintName;
		DefaultValueType		m_defaultValueType;
		CFieldVariant::pointer	m_default;
		bool					m_references;
		bool					m_autoincrement;

		uinteger			m_datasize;
		usmallint			m_defdatasize;
	};
	const CFieldInfo::pointer NullFieldInfo;

} // namespace

#endif // __fieldinfo_h__

