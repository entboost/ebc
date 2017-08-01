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

#include <string.h>
#include "fieldinfo.h"

namespace bo
{
	CFieldInfo::pointer CFieldInfo::create(void)
	{
		return CFieldInfo::pointer(new CFieldInfo());
	}
	CFieldInfo::pointer CFieldInfo::create(const tstring & name, FieldType type, uinteger size1, const tstring & note)
	{
		return CFieldInfo::pointer(new CFieldInfo( name, type, size1, note));
	}

	CFieldInfo::CFieldInfo(void)
		: m_id(0), m_type(FT_UNKNOWN), m_name(_T("")), m_note(_T(""))
		, m_size1(0), m_size2(0)
		, m_constraint(FIELD_CONSTRAINT_UNKNOWN), m_constraintName(_T(""))
		, m_defaultValueType(DVT_None)
		, m_references(false), m_autoincrement(false)
		, m_datasize(0), m_defdatasize(0)
	{
	}
	CFieldInfo::CFieldInfo(const tstring & name, FieldType type, uinteger size1, const tstring & note)
		: m_id(0), m_type(type), m_name(name), m_note(note)
		, m_size1(size1), m_size2(0)
		, m_constraint(FIELD_CONSTRAINT_UNKNOWN), m_constraintName(_T(""))
		, m_defaultValueType(DVT_None)
		, m_references(false), m_autoincrement(false)
		, m_datasize(0), m_defdatasize(0)
		//: m_id(0), m_name(name), m_type(type), m_size1(size1), m_size2(0), m_note(note)
		//, m_references(false), m_autoincrement(false)
		//, m_defaultValueType(DVT_None)
	{
		countDataSize();
	}
	CFieldInfo::~CFieldInfo(void)
	{}

	CFieldVariant::pointer CFieldInfo::buildFieldVariant(void) const
	{
		CFieldVariant::pointer result = CFieldVariant::create(this->type());
		switch (result->VARTYPE)
		{
		case FT_FLOAT:
			result->v.floatVal.size1 = this->size1();
			break;
		case FT_NUMERIC:
			{
				result->v.numericVal = new NUMERIC;
				result->v.numericVal->size1 = this->size1();
				result->v.numericVal->size2 = this->size2();
			}break;
		default:
			break;
		}

		return result;
	}
	void CFieldInfo::defaultValueType(DefaultValueType newv)
	{
		if (newv == DVT_Variant)
		{
			buildDefaultValue();
		}else
		{
			m_defaultValueType = newv;
		}
	}
	CFieldVariant::pointer CFieldInfo::buildDefaultValue(void)
	{
		if (m_default.get() == 0)
			m_default = CFieldVariant::create(m_type);
		m_defaultValueType = DVT_Variant;
		return m_default;
	}
	void CFieldInfo::dropDefaultValue(void)
	{
		m_defaultValueType = DVT_None;
		m_default.reset();
	}

	void CFieldInfo::Serialize(bool isStoring, tfstream& ar)
	{
		ar.clear();
		short len = 0;
		if (isStoring)
		{
			// m_id
			ar.write((const char*)&m_id, INTEGER_SIZE);

			// m_type
			ar.write((const char*)&m_type, 4);

			// m_name
			len = m_name.length();
			len = len > MAX_NAME_SIZE ? MAX_NAME_SIZE : len;
			ar.write((const char*)(&len), SMALLINT_SIZE);
			if (len > 0)
			{
				char * buffer = new char[len+2];
				strcpy(buffer, m_name.c_str());
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]+(char)i+(char)len;
				}
				ar.write(buffer, len);
				delete[] buffer;
				//ar.write(m_name.c_str(), len);
			}
			ar.seekp(MAX_NAME_SIZE-len, std::ios::cur);

			// m_note
			len = m_note.length();
			len = len > MAX_NOTE_SIZE ? MAX_NOTE_SIZE : len;
			ar.write((const char*)(&len), SMALLINT_SIZE);
			if (len > 0)
			{
				char * buffer = new char[len+2];
				strcpy(buffer, m_note.c_str());
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]+(char)i+(char)len;
				}
				ar.write(buffer, len);
				delete[] buffer;
				//ar.write(m_note.c_str(), len);
			}
			ar.seekp(MAX_NOTE_SIZE-len, std::ios::cur);

			// m_size1 & m_size2
			ar.write((const char*)&m_size1, INTEGER_SIZE);
			ar.write((const char*)&m_size2, SMALLINT_SIZE);

			// m_constraint
			ar.write((const char*)&m_constraint, 4);
			// m_references
			ar.write((const char*)&m_references, BOOLEAN_SIZE);
			// m_autoincrement
			ar.write((const char*)&m_autoincrement, BOOLEAN_SIZE);

			// m_default
			//if (m_defaultValueType == DVT_Variant && m_default.get() == NULL)
			//	m_defaultValueType = DVT_None;
			ar.write((const char*)&m_defaultValueType, 4);
			if (m_defaultValueType == DVT_Variant)
			{
				BOOST_ASSERT (m_default.get() != NULL);

				uinteger doSize = 0;
				m_default->Serialize(isStoring, ar, MAX_DEFAULT_CHAR_SIZE, true, doSize);
				if (MAX_DEFAULT_VARIANT_SIZE > doSize)
					ar.seekp(MAX_DEFAULT_VARIANT_SIZE - doSize, std::ios::cur);
			}else
			{
				//ar.seekp(m_defdatasize, std::ios::cur);
				ar.seekp(MAX_DEFAULT_VARIANT_SIZE, std::ios::cur);
			}

		}else
		{
			char * buffer = 0;

			// m_id
			ar.read((char*)&m_id, INTEGER_SIZE);

			// m_type
			ar.read((char*)&m_type, 4);

			// m_name
			ar.read((char*)(&len), SMALLINT_SIZE);
			len = len > MAX_NAME_SIZE ? MAX_NAME_SIZE : len;
			if (len > 0)
			{
				buffer = new char[len+2];
				ar.read(buffer, len);
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]-(char)i-(char)len;
				}
				m_name = tstring(buffer, len);
				delete[] buffer;
			}
			ar.seekg(MAX_NAME_SIZE-len, std::ios::cur);

			// m_note
			ar.read((char*)(&len), SMALLINT_SIZE);
			len = len > MAX_NOTE_SIZE ? MAX_NOTE_SIZE : len;
			if (len > 0)
			{
				buffer = new char[len+2];
				ar.read(buffer, len);
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]-(char)i-(char)len;
				}
				m_note = tstring(buffer, len);
				delete []buffer;
			}
			ar.seekg(MAX_NOTE_SIZE-len, std::ios::cur);

			// m_size1 & m_size2
			ar.read((char*)&m_size1, INTEGER_SIZE);
			ar.read((char*)&m_size2, SMALLINT_SIZE);
			countDataSize();

			// m_constraint
			ar.read((char*)&m_constraint, 4);
			// m_references
			ar.read((char*)&m_references, BOOLEAN_SIZE);
			// m_autoincrement
			ar.read((char*)&m_autoincrement, BOOLEAN_SIZE);

			// m_default
			ar.read((char*)&m_defaultValueType, 4);
			if (m_defaultValueType == DVT_Variant)
			{
				m_default = CFieldVariant::create(m_type);
				uinteger doSize = 0;
				m_default->Serialize(isStoring, ar, MAX_DEFAULT_CHAR_SIZE, true, doSize);
				if (MAX_DEFAULT_VARIANT_SIZE > doSize)
					ar.seekg(MAX_DEFAULT_VARIANT_SIZE - doSize, std::ios::cur);
			}else
			{
				//ar.seekg(m_defdatasize, std::ios::cur);
				ar.seekg(MAX_DEFAULT_VARIANT_SIZE, std::ios::cur);
			}
		}
	}
	void CFieldInfo::countDataSize(void)
	{
		switch (m_type)
		{
		case FT_BOOLEAN:
			m_datasize = BOOLEAN_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_TINYINT:
		case FT_UTINYINT:
			m_datasize = TINYINT_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_SMALLINT:
		case FT_USMALLINT:
			m_datasize = SMALLINT_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_INTEGER:
		case FT_UINTEGER:
			m_datasize = INTEGER_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_BIGINT:
		case FT_UBIGINT:
			m_datasize = BIGINT_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_NUMERIC:
			m_datasize = NUMERIC_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_FLOAT:
			m_datasize = FLOAT_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_REAL:
			m_datasize = REAL_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_DOUBLE:
			m_datasize = DOUBLE_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_DATE:
			m_datasize = DATE_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_TIME:
			m_datasize = TIME_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_TIMESTAMP:
			m_datasize = TIMESTAMP_SIZE;
			m_defdatasize = m_datasize;
			break;
		case FT_CHAR:
		case FT_VARCHAR:
		case FT_NCHAR:
		case FT_NVARCHAR:
		case FT_BINARY:
		case FT_VARBINARY:
			m_size1 = m_size1 > MAX_CHAR_SIZE ? MAX_CHAR_SIZE : m_size1;
			m_size1 = m_size1 == 0 ? 1 : m_size1;
			m_datasize = INTEGER_SIZE;
			//m_datasize = m_size1 + INTEGER_SIZE;
			m_defdatasize = MAX_DEFAULT_CHAR_SIZE + INTEGER_SIZE;
			break;
		case FT_CLOB:
		case FT_NCLOB:
		case FT_BLOB:
			m_size1 = m_size1 > MAX_CLOB_SIZE ? MAX_CLOB_SIZE : m_size1;
			m_size1 = m_size1 == 0 ? 1 : m_size1;
			m_datasize = INTEGER_SIZE;
			//m_datasize = m_size1 + INTEGER_SIZE;
			m_defdatasize = 0;
			break;
		default:
			break;
		}
	}


} // namespace

