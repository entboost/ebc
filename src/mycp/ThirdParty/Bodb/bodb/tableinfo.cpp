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
#include "tableinfo.h"

namespace bo
{

	CTableInfo::pointer CTableInfo::create(void)
	{
		return CTableInfo::pointer(new CTableInfo());
	}
	CTableInfo::pointer CTableInfo::create(const tstring & name, TableType type, const tstring & note)
	{
		return CTableInfo::pointer(new CTableInfo( name, type, note));
	}

	bool CTableInfo::createField(const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);

		if (m_fields2.exist(fieldInfo->name()))
		{
			return false;
		}

		m_fields.add(fieldInfo);
		m_fields2.insert(fieldInfo->name(), fieldInfo);
		m_fields3.insert(fieldInfo->id(), fieldInfo);
		return true;
	}
	CFieldInfo::pointer CTableInfo::dropField(const tstring& fieldname)
	{
		CFieldInfo::pointer fieldInfo;
		if (!m_fields2.find(fieldname,fieldInfo,true))
		{
			return NullFieldInfo;
		}
		m_fields3.remove(fieldInfo->id());

		BoostWriteLock wtlock(m_fields.mutex());
		CLockList<CFieldInfo::pointer>::iterator pIter = m_fields.begin();
		for (;pIter!=m_fields.end(); pIter++)
		{
			CFieldInfo::pointer pDropFieldInfo = *pIter;
			if (pDropFieldInfo->name() == fieldInfo->name())
			{
				m_fields.erase(pIter);
				break;
			}
		}
		return fieldInfo;
	}
	bool CTableInfo::modifyField(const tstring& fieldname,const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);

		CFieldInfo::pointer pOldFieldInfo;
		if (!m_fields2.find(fieldInfo->name(),pOldFieldInfo,true))
		{
			return false;
		}
		fieldInfo->id(pOldFieldInfo->id());
		m_fields3.remove(pOldFieldInfo->id());
		m_fields2.insert(fieldInfo->name(), fieldInfo);
		m_fields3.insert(fieldInfo->id(), fieldInfo);
		return true;
	}
	bool CTableInfo::existField(const tstring & fieldname) const
	{
		return m_fields2.exist(fieldname);
	}

	CFieldInfo::pointer CTableInfo::getFieldInfo(uinteger fieldid)
	{
		CFieldInfo::pointer fieldInfo;
		m_fields3.find(fieldid, fieldInfo);
		return fieldInfo;
	}
	CFieldInfo::pointer CTableInfo::getFieldInfo(const tstring & fieldname)
	{
		CFieldInfo::pointer fieldInfo;
		m_fields2.find(fieldname, fieldInfo);
		return fieldInfo;
	}

	CFieldInfo::pointer CTableInfo::getFieldInfo2(short fieldindex)
	{
		CFieldInfo::pointer result;
		short curIndex = 0;
		CLockList<CFieldInfo::pointer>::iterator iter;
		for (iter=m_fields.begin(); iter!=m_fields.end(); iter++)
		{
			if (curIndex++ == fieldindex)
			{
				result = *iter;
				break;
			}
		}

		return result;
	}

	//CFieldInfo::pointer CTableInfo::moveFirst(void)
	//{
	//	CFieldInfo::pointer resultNull;
	//	m_curiter = m_fields.begin();
	//
	//	if (m_curiter == m_fields.end())
	//		return resultNull;

	//	return *m_curiter;
	//}
	//CFieldInfo::pointer CTableInfo::moveNext(void)
	//{
	//	CFieldInfo::pointer resultNull;
	//	if (++m_curiter == m_fields.end())
	//		return resultNull;

	//	return *m_curiter;
	//}

	//CFieldInfo::pointer CTableInfo::movePrev(void)
	//{
	//	CFieldInfo::pointer resultNull;
	//	if (m_curiter == m_fields.begin() || --m_curiter == m_fields.begin())
	//		return resultNull;

	//	return *m_curiter;
	//}

	//CFieldInfo::pointer CTableInfo::moveLast(void)
	//{
	//	CFieldInfo::pointer resultNull;
	//	m_curiter = m_fields.end();
	//
	//	if (m_curiter == m_fields.begin())
	//		return resultNull;

	//	m_curiter--;
	//	return *m_curiter;
	//}

	//usmallint CTableInfo::getMaxTableInfoSize(void)
	//{
	//	return sizeof(uinteger)				// m_currentRecordId
	//		+ sizeof(uinteger)				// m_id
	//		+ sizeof(TableType)					// m_type
	//		+ sizeof(short)+MAX_NAME_SIZE		// m_name
	//		+ sizeof(short)+MAX_NOTE_SIZE;		// m_note
	//}

	usmallint CTableInfo::getMaxTableFieldSize(void) const
	{
		return MAX_TABLEINFO_SIZE + getMaxFieldInfoSize();
	}
	usmallint CTableInfo::getMaxFieldInfoSize(void) const
	{
		return m_fields.size() * MAX_FIELDINFO_SIZE;
	}

	CTableInfo::CTableInfo(void)
		: m_id(0), m_type(TT_UNKNOWN), m_name(_T("")), m_note(_T(""))
		, m_currentRecordId(0)
	{
		//m_curiter = m_fields.begin();
	}
	CTableInfo::CTableInfo(const tstring & name, TableType type, const tstring & note)
		: m_id(0), m_type(type), m_name(name), m_note(note)
		, m_currentRecordId(0)
	{
		//m_curiter = m_fields.begin();
	}
	CTableInfo::~CTableInfo(void)
	{
		m_fields.clear();
		m_fields2.clear();
		m_fields3.clear();
		m_datapages.clear();
		m_data2pages.clear();
		m_fieldinfopages.clear();
		m_rlpages.clear();
		m_fipages.clear();
		m_recordfdois.clear();
	}

	CPageHeadInfo::pointer CTableInfo::findDataPage(usmallint needSize)
	{
		// ? lock
		CLockMap<uinteger, CPageHeadInfo::pointer>::iterator iter;
		for (iter=m_datapages.begin(); iter!=m_datapages.end(); iter++)
		{
			if (iter->second->unused() > needSize)
			{
				return iter->second;
			}
		}
		return boNullPageHeadInfo;
	}
	CPageHeadInfo::pointer CTableInfo::findData2Page(usmallint needSize, CPageHeadInfo::PageHeadSubType1 subtype)
	{
		// ? lock
		CLockMap<uinteger, CPageHeadInfo::pointer>::iterator iter;
		for (iter=m_data2pages.begin(); iter!=m_data2pages.end(); iter++)
		{
			if (iter->second->subtype() == (uinteger)subtype && iter->second->unused() > needSize)
			{
				return iter->second;
			}
		}
		return boNullPageHeadInfo;
	}
	CPageHeadInfo::pointer CTableInfo::findFieldInfoPage(usmallint needSize)
	{
		// ? lock
		CLockList<CPageHeadInfo::pointer>::iterator iter;
		for (iter=m_fieldinfopages.begin(); iter!=m_fieldinfopages.end(); iter++)
		{
			if ((*iter)->unused() > needSize)
			{
				return (*iter);
			}
		}
		return boNullPageHeadInfo;
	}

	void CTableInfo::Serialize(bool isStoring, tfstream& ar)
	{
		ar.clear();
		short len = 0;
		if (isStoring)
		{
			//m_currentRecordId
			ar.write((const char*)&m_currentRecordId, INTEGER_SIZE);
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
				char * buffer = new char[len+1];
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

			// m_currentRecordId
			ar.read((char*)&m_currentRecordId, INTEGER_SIZE);
			// m_id
			ar.read((char*)&m_id, INTEGER_SIZE);
			// m_type
			ar.read((char*)&m_type, 4);

			// m_name
			ar.read((char*)(&len), SMALLINT_SIZE);
			len = len > MAX_NAME_SIZE ? MAX_NAME_SIZE : len;
			if (len > 0)
			{
				buffer = new char[len+1];
				ar.read(buffer, len);
				for (int i=0; i<len; i+=2)
				{
					buffer[i] = buffer[i]-(char)i-(char)len;
				}
				m_name = tstring(buffer, len);
				delete []buffer;
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
