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

// modifyinfo.h file here
#ifndef __modifyinfo_h__
#define __modifyinfo_h__

#include "../stl/stldef.h"
#include "../stl/lockmap.h"
#include "tableinfo.h"
#include "fieldinfo.h"
#include "recordline.h"

namespace bo
{

	class CModifyInfo
	{
	public:
		enum ModifyInfoType
		{
			MIT_DATABASEINFO	= 0x1
			, MIT_TABLEINFO
			, MIT_FIELDINFO
			, MIT_RECORDLINE

			, MIT_UNKNOWN		= 0xf
		};
		enum ModifyInfoFlag
		{
			MIF_ADD				= 0x1
			, MIF_UPDATE
			, MIF_DELETE
			, MIF_UNKNOWN		= 0xf
		};

		typedef boost::shared_ptr<CModifyInfo> pointer;
		static CModifyInfo::pointer create(void)
		{
			return CModifyInfo::pointer(new CModifyInfo());
		}
		static CModifyInfo::pointer create(ModifyInfoType type, ModifyInfoFlag flag)
		{
			return CModifyInfo::pointer(new CModifyInfo(type, flag));
		}
		static CModifyInfo::pointer create(ModifyInfoFlag flag, const CTableInfo::pointer& tableInfo)
		{
			return CModifyInfo::pointer(new CModifyInfo(flag, tableInfo));
		}
		static CModifyInfo::pointer create(ModifyInfoFlag flag, const CFieldInfo::pointer& fieldInfo)
		{
			return CModifyInfo::pointer(new CModifyInfo(flag, fieldInfo));
		}
		static CModifyInfo::pointer create(ModifyInfoFlag flag, const CRecordLine::pointer& recordLine)
		{
			return CModifyInfo::pointer(new CModifyInfo(flag, recordLine));
		}

		ModifyInfoType type(void) const {return m_type;}
		ModifyInfoFlag flag(void) const {return m_flag;}

		void tableInfo(CTableInfo::pointer newv) {m_tableInfo = newv;}
		CTableInfo::pointer tableInfo(void) const {return m_tableInfo;}
		CFieldInfo::pointer fieldInfo(void) const {return m_fieldInfo;}
		CRecordLine::pointer recordLine(void) const {return m_recordLine;}

	public:
		CModifyInfo(void)
			: m_type(MIT_UNKNOWN), m_flag(MIF_UNKNOWN)
		{}
		CModifyInfo(ModifyInfoType type, ModifyInfoFlag flag)
			: m_type(type), m_flag(flag)
		{}
		CModifyInfo(ModifyInfoFlag flag, const CTableInfo::pointer& tableInfo)
			: m_type(MIT_TABLEINFO), m_flag(flag), m_tableInfo(tableInfo)
		{
			BOOST_ASSERT (m_tableInfo.get() != 0);
		}
		CModifyInfo(ModifyInfoFlag flag, const CFieldInfo::pointer& fieldInfo)
			: m_type(MIT_FIELDINFO), m_flag(flag), m_fieldInfo(fieldInfo)
		{
			BOOST_ASSERT (m_fieldInfo.get() != 0);
		}
		CModifyInfo(ModifyInfoFlag flag, const CRecordLine::pointer& recordLine)
			: m_type(MIT_RECORDLINE), m_flag(flag), m_recordLine(recordLine)
		{
			BOOST_ASSERT (m_recordLine.get() != 0);
		}
		virtual ~CModifyInfo(void)
		{}

		virtual void Serialize(bool isStoring, tfstream& ar)
		{
			//short len = 0;
			if (isStoring)
			{
				// m_type
				ar.write((const char*)&m_type, sizeof(m_type));
				// m_flag
				ar.write((const char*)&m_flag, sizeof(m_flag));

			}else
			{
				//char * buffer = 0;

				// m_type
				ar.read((char*)&m_type, sizeof(m_type));
				// m_flag
				ar.read((char*)&m_flag, sizeof(m_flag));
			}
			
		}
	private:
		ModifyInfoType	m_type;
		ModifyInfoFlag	m_flag;
		CTableInfo::pointer m_tableInfo;
		CFieldInfo::pointer m_fieldInfo;
		CRecordLine::pointer m_recordLine;
	};

} // namespace bo

#endif // __modifyinfo_h__

