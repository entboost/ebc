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
#include "databaseinfo.h"

namespace bo
{

	CDatabaseInfo::pointer CDatabaseInfo::create(const tstring & name, DatabaseType type, const tstring & note)
	{
		return CDatabaseInfo::pointer(new CDatabaseInfo(name, type, note));
	}

	bool CDatabaseInfo::createTable(const CTableInfo::pointer& tableInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);

		if (m_tables.exist(tableInfo->name()))
		{
			return false;
		}

		if (tableInfo->id() == 0)
			tableInfo->id(this->getNextTableId());
		m_tables.insert(tableInfo->name(), tableInfo);
		m_tables2.insert(tableInfo->id(), tableInfo);
		return true;
	}

	bool CDatabaseInfo::createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);
		if (fieldInfo->id() == 0)
			fieldInfo->id(this->getNextFieldId());
		return tableInfo->createField(fieldInfo);
	}
	CFieldInfo::pointer CDatabaseInfo::dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		return tableInfo->dropField(fieldname);
	}
	bool CDatabaseInfo::modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);
		return tableInfo->modifyField(fieldname,fieldInfo);
	}

	//CTableInfo::pointer CDatabaseInfo::createTable(const tstring & tableName)
	//{
	//	CTableInfo::pointer tableInfo;
	//	if (!m_tables.find(tableName, tableInfo))
	//	{
	//		tableInfo = CTableInfo::create(this->getNextTableId(), tableName);
	//		m_tables.insert(tableInfo->name(), tableInfo);
	//		m_tables2.insert(tableInfo->id(), tableInfo);
	//	}
	//	return tableInfo;
	//}

	//CFieldInfo::pointer CDatabaseInfo::createField(CTableInfo::pointer tableInfo, const tstring & fieldName, FieldType fieldtype, uinteger len)
	//{
	//	BOOST_ASSERT (tableInfo.get() != 0);

	//	CFieldInfo::pointer fieldInfo = tableInfo->getFieldInfo(fieldName);
	//	if (fieldInfo.get() == 0)
	//	{
	//		fieldInfo = CFieldInfo::create(this->getNextFieldId(), fieldName, fieldtype, len);
	//		tableInfo->createField(fieldInfo);
	//	}
	//	return fieldInfo;
	//}

	//CFieldInfo::pointer createFiele(const tstring & fieldName)
	//{

	//}

	bool CDatabaseInfo::existTable(const tstring & tablename)
	{
		return m_tables.exist(tablename);
	}

	CTableInfo::pointer CDatabaseInfo::getTableInfo(uinteger tableid)
	{
		CTableInfo::pointer tableInfo;
		m_tables2.find(tableid, tableInfo);
		return tableInfo;
	}
	CTableInfo::pointer CDatabaseInfo::getTableInfo(const tstring & tablename, bool erase)
	{
		CTableInfo::pointer tableInfo;
		m_tables.find(tablename, tableInfo, erase);
		return tableInfo;
	}

	CDatabaseInfo::CDatabaseInfo(const tstring & name, DatabaseType type, const tstring & note)
		: m_type(type), m_name(name), m_note(note)
		, m_currentTableId(0), m_currentFieldId(0)
	{}
	CDatabaseInfo::~CDatabaseInfo(void)
	{
		m_tables.clear();
		m_tables2.clear();
	}

	void CDatabaseInfo::Serialize(bool isStoring, tfstream& ar)
	{
		ar.clear();
		short len = 0;
		if (isStoring)
		{
			// m_currentTableId
			ar.write((const char*)&m_currentTableId, INTEGER_SIZE);
			// m_currentFieldId
			ar.write((const char*)&m_currentFieldId, INTEGER_SIZE);
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
		}else
		{
			char * buffer = 0;

			// m_currentTableId
			ar.read((char*)&m_currentTableId, INTEGER_SIZE);
			// m_currentFieldId
			ar.read((char*)&m_currentFieldId, INTEGER_SIZE);
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
				buffer = new char[len];
				ar.read(buffer, len);
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]-i-len;
				}
				m_note = tstring(buffer, len);
				delete []buffer;
			}
			ar.seekg(MAX_NOTE_SIZE-len, std::ios::cur);
		}

	}


} // namespace bo

