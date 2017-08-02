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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "bodbdef.h"
#include "database.h"
#include "infoheadinfo.h"
#include "dataheadinfo.h"
#include "data2headinfo.h"
#include "resultset.h"
#include "../stl/cgcSmartString.h"

#ifdef WIN32
#include "Windows.h"
#endif // WIN32

#ifdef _UNICODE
typedef boost::filesystem::wpath boosttpath;
#else
typedef boost::filesystem::path boosttpath;
#endif // _UNICODE

namespace bo
{
	CDatabase::pointer CDatabase::create(const CDatabaseInfo::pointer& dbinfo)
	{
		return CDatabase::pointer(new CDatabase(dbinfo));
	}
	void CDatabase::setpath(const tstring & path)
	{
		m_path = path;
	}
	const tstring & CDatabase::getpath(void) const
	{
		return m_path;
	}

	CDatabase::CDatabase(void)
		: m_killed(false), m_proc(0)
		, m_curpageid(0)
	{
		m_bLoadError = false;
		m_bFullMemory = false;
	}
	CDatabase::CDatabase(const CDatabaseInfo::pointer& dbinfo)
		: m_dbinfo(dbinfo)
		, m_killed(false), m_proc(0)
		, m_curpageid(0)
	{

		m_bLoadError = false;
		m_bFullMemory = false;
		BOOST_ASSERT (m_dbinfo.get() != 0);
	}
	CDatabase::~CDatabase(void)
	{
		close();
	}
	
	bool CDatabase::isopened(void) const
	{
		return m_proc != 0;
	}

//#define USES_DEBUG
#ifdef USES_DEBUG
	FILE * theLogFile = NULL;
#endif

	bool CDatabase::open(void)
	{
		if (isopened()) return true;

#ifdef USES_DEBUG
		if (theLogFile == NULL)
			theLogFile = fopen("d:\\entboost_log2.txt","w+");
		char lpszBuffer[1024];
		sprintf(lpszBuffer,"path=%s\n",m_path.c_str());
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif

		tstring dbfilename(m_path);
#ifdef WIN32
		dbfilename.append(_T("\\"));
#else
		dbfilename.append(_T("/"));
#endif
		dbfilename.append(m_dbinfo->name());
#ifdef WIN32
		dbfilename.append(_T("\\"));
#else
		dbfilename.append(_T("/"));
#endif
		dbfilename.append(m_dbinfo->name());
		dbfilename.append(_T(".bdf"));

		namespace fs = boost::filesystem;
		boosttpath pathFilename(dbfilename.string());
		//boosttpath pathFilename(dbfilename, fs::native);
		if (!boost::filesystem::exists(pathFilename))
		{
			// ?
			m_fdb.open(dbfilename.c_str(), std::ios::out|std::ios::binary);
			m_fdb.close();

			// The new database.
			m_modifys.add(CModifyInfo::create(CModifyInfo::MIT_DATABASEINFO, CModifyInfo::MIF_ADD));
		}

		m_fdb.open(dbfilename.c_str(), std::ios::in|std::ios::out|std::ios::binary);
		if (!m_fdb.is_open())
		{
#ifdef USES_DEBUG
			sprintf(lpszBuffer,"open file error =%s,%d\n",dbfilename.c_str(), GetLastError());
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
			m_modifys.clear();
			return false;
		}

		m_bLoadError = false;
		//loadDbfile(CPageHeadInfo::PHT_INFO|CPageHeadInfo::PHT_DATA);
		loadDbfile(CPageHeadInfo::PHT_INFO);
		loadDbfile(CPageHeadInfo::PHT_DATA);
		loadDbfile(CPageHeadInfo::PHT_DATA2);

		m_killed = false;
		m_proc = new boost::thread(boost::bind(&CDatabase::proc_db, this));

		CTableInfo::pointer tableInfo = getTableInfo("bodb_sys_option");
		if (tableInfo.get() != NULL)	// New Create Database
		{
			CFieldInfo::pointer fieldOptionInfo = tableInfo->getFieldInfo("option");
			CFieldInfo::pointer fieldValueInfo = tableInfo->getFieldInfo("value");
			if (fieldOptionInfo.get() != NULL && fieldValueInfo.get() != NULL)
			{
				CFieldVariant::pointer variantOption = CFieldVariant::create(fieldOptionInfo->type());
				variantOption->setIntu(OPTION_FULL_MEMORY_MODE);
				CFieldVariant::pointer variantValue = CFieldVariant::create(fieldValueInfo->type());
				variantValue->setString("1", 1);

				CSelectInfo pSelectInfo(tableInfo);
				std::list<CFieldCompare::pointer> wheres;
				wheres.push_back(CFieldCompare::create(tableInfo, CFieldCompare::FCT_EQUAL, fieldOptionInfo, variantOption,0));
				wheres.push_back(CFieldCompare::create(tableInfo, CFieldCompare::FCT_EQUAL, fieldValueInfo, variantValue,0));
				pSelectInfo.wheres.push_back(wheres);
				CResultSet::pointer rs = select(pSelectInfo, false);
				if (rs.get()!=NULL && !rs->empty())
					m_bFullMemory = true;
			}
		}

		//m_resultset = CResultSet::create();

#ifdef USES_DEBUG
			sprintf(lpszBuffer,"open file ok =%s\n",dbfilename.c_str());
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theLogFile);
#endif
		return !m_bLoadError;
	}

	void CDatabase::loadDbfile(uinteger pht)
	{
		BOOST_ASSERT (m_fdb.is_open());

		m_fdb.seekg(0, std::ios::end);
		uinteger nTotal = m_fdb.tellg();
		m_fdb.seekg(0, std::ios::beg);
		uinteger pageSize = nTotal/PAGE_BLOCK_SIZE;

 		for (uinteger i=0; i<pageSize; i++)
		{
			CPageHeadInfo::pointer pageHeadInfo = CPageHeadInfo::create();
			pageHeadInfo->Serialize(false, m_fdb);
			//m_pages.add(pageHeadInfo);
			m_curpageid = pageHeadInfo->id()+1;

			if ((pht & (uinteger)pageHeadInfo->type()) != (uinteger)pageHeadInfo->type())
			{
				m_fdb.seekg(PAGE_BLOCK_SIZE*(i+1), std::ios::beg);
				continue;
			}
			m_fdb.clear();
			switch (pageHeadInfo->type())
			{
			case CPageHeadInfo::PHT_INFO:
				{
					if (i==0)
					{
						// CDatabaseInfo
						m_dbinfo->Serialize(false, m_fdb);
						m_tableinfopages.add(pageHeadInfo);
					}else if (pageHeadInfo->subtype() == CPageHeadInfo::PST_TABLEINFO)
					{
						m_tableinfopages.add(pageHeadInfo);
					}else if (pageHeadInfo->subtype() == CPageHeadInfo::PST_FIELDINFO)
					{
						m_fieldinfopages.add(pageHeadInfo);
					}else
					{
						BOOST_ASSERT (false);
					}

					usmallint readCount = 0;
					usmallint readIndex = 0;
					usmallint offset = 0;
					while (readCount < pageHeadInfo->count())
					{
						// Read index
						readIndex++;
						m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
						m_fdb.read((char*)&offset, SMALLINT_SIZE);

						if (offset > 0)
						{
							readCount++;
							m_fdb.seekg(PAGE_BLOCK_SIZE*pageHeadInfo->id()+offset, std::ios::beg);

							uinteger idkey = 0;
							if (pageHeadInfo->subtype() == CPageHeadInfo::PST_TABLEINFO)
							{
								// CTableInfo
								CTableInfo::pointer tableInfo = CTableInfo::create();
								tableInfo->Serialize(false, m_fdb);
								m_dbinfo->createTable(tableInfo);
								m_tablepages.insert(tableInfo->id(), pageHeadInfo);
								idkey = tableInfo->id();

								if (tableInfo->id() > m_dbinfo->getTableId())
									m_dbinfo->setTableId(tableInfo->id());

								//if (tableInfo->name() == ConstSystemTable1_Name)
								//	m_sysTableRecordId = tableInfo;

							}else if (pageHeadInfo->subtype() == CPageHeadInfo::PST_FIELDINFO)
							{
								// CFieldInfo
								uinteger tableid = 0;
								m_fdb.read((char*)&tableid, INTEGER_SIZE);
								CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableid);
								BOOST_ASSERT (tableInfo.get() != 0);

								CFieldInfo::pointer fieldInfo = CFieldInfo::create();
								fieldInfo->Serialize(false, m_fdb);
								tableInfo->createField(fieldInfo);
								tableInfo->m_fipages.insert(fieldInfo->id(), pageHeadInfo);
								idkey = fieldInfo->id();

								if (fieldInfo->id() > m_dbinfo->getFieldId())
									m_dbinfo->setFieldId(fieldInfo->id());
							}

							pageHeadInfo->m_idindexs.insert(idkey, readIndex-1);
						}
					}
					m_fdb.seekg(PAGE_BLOCK_SIZE*(i+1), std::ios::beg);
				}break;
			case CPageHeadInfo::PHT_DATA:
				{
					CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(pageHeadInfo->objectid());
					BOOST_ASSERT (tableInfo.get() != 0);
					tableInfo->m_datapages.insert(pageHeadInfo->id(), pageHeadInfo);

					CResultSet::pointer resultSet;
					if (!m_results.find(tableInfo.get(), resultSet))
					{
						resultSet = CResultSet::create(tableInfo);
						m_results.insert(tableInfo.get(), resultSet);
					}

					usmallint readCount = 0;
					usmallint readIndex = 0;
					usmallint offset = 0;
					while (readCount < pageHeadInfo->count())
					{
						// Read index
						readIndex++;
						m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
						m_fdb.read((char*)&offset, SMALLINT_SIZE);

						if (offset > 0)
						{
							readCount++;
							m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
							usmallint readSize = 0;
							CRecordLine::pointer recordLine = CRecordLine::create(0, tableInfo);
							recordLine->Serialize(false, m_fdb, readSize);
							resultSet->addRecord(recordLine);
							tableInfo->m_rlpages.insert(recordLine->id(), pageHeadInfo);
							//m_records.insert(recordLine->id(), recordLine);

							pageHeadInfo->m_idindexs.insert(recordLine->id(), readIndex-1);
						}
					}
					m_fdb.seekg(PAGE_BLOCK_SIZE*(i+1), std::ios::beg);
				}break;
			case CPageHeadInfo::PHT_DATA2:
				{
					CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(pageHeadInfo->objectid());
					BOOST_ASSERT (tableInfo.get() != 0);
					tableInfo->m_data2pages.insert(pageHeadInfo->id(), pageHeadInfo);

					{
						usmallint readCount = 0;
						usmallint readIndex = 0;
						usmallint offset = 0;
						while (readCount < pageHeadInfo->count())
						{
							// Read index
							readIndex++;
							if (pageHeadInfo->subtype() == CPageHeadInfo::PST_8000)
							{
								offset = MAX_PAGEHEADINFO_SIZE;
							}else
							{
								m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
								m_fdb.read((char*)&offset, SMALLINT_SIZE);
							}

							if (offset > 0)
							{
								readCount++;
								if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
								{
									m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
								}

								uinteger recordId = 0;
								uinteger fieldId = 0;
								uinteger bufferIndex = 0;
								usmallint realSize = 0;
								m_fdb.read((char*)&recordId, INTEGER_SIZE);
								m_fdb.read((char*)&fieldId, INTEGER_SIZE);
								m_fdb.read((char*)&bufferIndex, INTEGER_SIZE);
								m_fdb.read((char*)&realSize, SMALLINT_SIZE);

								CResultSet::pointer tableResultSet;
								m_results.find(tableInfo.get(), tableResultSet);
								BOOST_ASSERT (tableResultSet.get() != 0);
								if (tableResultSet.get()==NULL)
								{
									m_bLoadError = true;
									// error
									//if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
									//{
									//	//m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
									//	//writeBlackNull(m_fdb, (INTEGER_SIZE*3)+SMALLINT_SIZE);	// (recordId+fieldId+bufferIndex)+realSize
									//	m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
									//	writeBlackNull(m_fdb, SMALLINT_SIZE);					// readIndex
									//}
									tableInfo->m_data2pages.remove(pageHeadInfo->id());
									UnUsePage(pageHeadInfo);
									break;
								}
								CRecordLine::pointer recordLine = tableResultSet->getRecord(recordId);
								BOOST_ASSERT (recordLine.get() != 0);
								if (recordLine.get()==NULL)
								{
									m_bLoadError = true;
									// error
									//if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
									//{
									//	//m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
									//	//writeBlackNull(m_fdb, (INTEGER_SIZE*3)+SMALLINT_SIZE);	// (recordId+fieldId+bufferIndex)+realSize
									//	m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
									//	writeBlackNull(m_fdb, SMALLINT_SIZE);					// readIndex
									//}
									tableInfo->m_data2pages.remove(pageHeadInfo->id());
									UnUsePage(pageHeadInfo);
									break;
								}
								CFieldVariant::pointer fieldVariant = recordLine->getVariant(fieldId);
								BOOST_ASSERT (fieldVariant.get() != 0);
								if (fieldVariant.get()==NULL)
								{
									m_bLoadError = true;
									// error
									//if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
									//{
									//	//m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
									//	//writeBlackNull(m_fdb, (INTEGER_SIZE*3)+SMALLINT_SIZE);	// (recordId+fieldId+bufferIndex)+realSize
									//	m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*readIndex, std::ios::beg);
									//	writeBlackNull(m_fdb, SMALLINT_SIZE);					// readIndex
									//}
									tableInfo->m_data2pages.remove(pageHeadInfo->id());
									UnUsePage(pageHeadInfo);
									break;
								}

								CRecordDOIs::pointer recordDOIs;
								if (!tableInfo->m_recordfdois.find(recordId, recordDOIs))
								{
									recordDOIs = CRecordDOIs::create(recordId);
									tableInfo->m_recordfdois.insert(recordId, recordDOIs);
								}
								CFieldDOIs::pointer fieldDOIs;
								if (!recordDOIs->m_dois.find(fieldId, fieldDOIs))
								{
									fieldDOIs = CFieldDOIs::create(fieldId);
									recordDOIs->m_dois.insert(fieldId, fieldDOIs);
								}
								fieldDOIs->m_dois.add(CData2OffsetInfo::create(pageHeadInfo, offset, readIndex-1));

								switch (fieldVariant->VARTYPE)
								{
								case bo::FT_VARCHAR:
									{
										// ??
										if (bufferIndex + realSize > fieldVariant->v.varcharVal.size)
											break;

										if (fieldVariant->v.varcharVal.buffer == NULL)
										{
											fieldVariant->v.varcharVal.buffer = new char[fieldVariant->v.varcharVal.size+1];
										}
										m_fdb.read(fieldVariant->v.varcharVal.buffer+bufferIndex, realSize);
										for (int i=0; i<realSize; i+=2)
										{
											fieldVariant->v.varcharVal.buffer[bufferIndex+i] = fieldVariant->v.varcharVal.buffer[bufferIndex+i]-(char)i-(char)realSize;
										}
										if (bufferIndex + realSize == fieldVariant->v.varcharVal.size)
										{
											fieldVariant->v.varcharVal.buffer[fieldVariant->v.varcharVal.size] = '\0';
										}
									}break;
								case bo::FT_CLOB:
									{
										if (fieldVariant->v.clobVal.size == 0) break;
										if (fieldVariant->v.clobVal.buffer == NULL)
										{
											fieldVariant->v.clobVal.buffer = new char[fieldVariant->v.clobVal.size+1];
										}
										m_fdb.read(fieldVariant->v.clobVal.buffer+bufferIndex, realSize);
										for (int i=0; i<realSize; i+=2)
										{
											fieldVariant->v.clobVal.buffer[bufferIndex+i] = fieldVariant->v.clobVal.buffer[bufferIndex+i]-(char)i-(char)realSize;
										}
										if (bufferIndex + realSize == fieldVariant->v.clobVal.size)
										{
											fieldVariant->v.clobVal.buffer[fieldVariant->v.clobVal.size] = '\0';
										}
									}break;
								default:
									break;
								}
							}
						}
					}

					m_fdb.seekg(PAGE_BLOCK_SIZE*(i+1), std::ios::beg);
				}break;
			case CPageHeadInfo::PHT_UNKNOWN:
				m_fdb.seekg(PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE, std::ios::cur);
				m_unusepages.add(pageHeadInfo);
				break;
			default:
				{
					BOOST_ASSERT (false);
					// error pagehead info.
					// add by hd, 2014-10-24
					pageHeadInfo->id(i);
					UnUsePage(pageHeadInfo);
					m_fdb.seekg(PAGE_BLOCK_SIZE*(i+1), std::ios::beg);
				}break;
			}
		}

	}

	void CDatabase::UnUsePage(const CPageHeadInfo::pointer& pageHeadInfo)
	{
		pageHeadInfo->reset();
		m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
		pageHeadInfo->Serialize(true, m_fdb);
		writeBlackNull(m_fdb, PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE);
		m_unusepages.add(pageHeadInfo);
	}
	void CDatabase::close(void)
	{
		// ??? do m_modifys

		m_killed = true;
		if (m_proc != 0)
		{
			boost::thread * threadtemp = m_proc;
			m_proc = 0;

			threadtemp->join();
			delete threadtemp;
		}

		m_results.clear();
		//m_records.clear();
		m_modifys.clear();
		//m_pages.clear();
		m_tablepages.clear();
		m_tableinfopages.clear();
		m_fieldinfopages.clear();
		m_unusepages.clear();
	}
	bool CDatabase::setOption(OptionType nOption, bo::uinteger nValue)
	{
		switch (nOption)
		{
		case OPTION_FULL_MEMORY_MODE:
			{
				const bool bFullMemoryTemp = nValue==1?true:false;
				if (m_bFullMemory != bFullMemoryTemp)
				{
					m_bFullMemory = bFullMemoryTemp;

					CTableInfo::pointer pTableInfo = getTableInfo("bodb_sys_option");
					if (pTableInfo.get()==NULL)
						break;
					CFieldInfo::pointer fieldOptionInfo = pTableInfo->getFieldInfo("option");
					CFieldInfo::pointer fieldValueInfo = pTableInfo->getFieldInfo("value");
					if (fieldOptionInfo.get()==NULL || fieldValueInfo.get()==NULL)
						break;

					CRecordLine::pointer recordLine = CRecordLine::create(pTableInfo);
					CFieldVariant::pointer pOptionVariant = CFieldVariant::create(bo::FT_INTEGER);
					pOptionVariant->setIntu(OPTION_FULL_MEMORY_MODE);
					recordLine->addVariant(fieldOptionInfo, pOptionVariant);
					CFieldVariant::pointer pValueVariant = CFieldVariant::create(bo::FT_VARCHAR);
					pValueVariant->setIntu(nValue);
					recordLine->addVariant(fieldValueInfo, pValueVariant);

					CFieldVariant::pointer variantOption = CFieldVariant::create(fieldOptionInfo->type());
					variantOption->setIntu(OPTION_FULL_MEMORY_MODE);

					std::list<std::list<CFieldCompare::pointer> > topwheres;
					std::list<CFieldCompare::pointer> wheres;
					wheres.push_back(CFieldCompare::create(pTableInfo, CFieldCompare::FCT_EQUAL, fieldOptionInfo, pOptionVariant,0));
					topwheres.push_back(wheres);
					if (update(pTableInfo, topwheres, recordLine)==0)
					{
						insert(recordLine);
					}
				}
			}break;
		default:
			return false;
			break;
		}
		return true;
	}
	bo::uinteger CDatabase::getOption(OptionType nOption) const
	{
		switch (nOption)
		{
		case OPTION_FULL_MEMORY_MODE:
			return m_bFullMemory?1:0;
			break;
		default:
			break;
		}
		return 0;
	}

	bool CDatabase::createTable(const CTableInfo::pointer& tableInfo)
	{
		if (m_killed) return false;
		BOOST_ASSERT (tableInfo.get() != 0);

		if (m_dbinfo->createTable(tableInfo))
		{
			m_modifys.add(CModifyInfo::create(CModifyInfo::MIT_DATABASEINFO, CModifyInfo::MIF_UPDATE));	// update table_id
			m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, tableInfo));
			return true;
		}
		return false;
	}

	bool CDatabase::dropTable(const tstring & tablename)
	{
		if (m_killed) return false;
		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tablename, true);
		if (tableInfo.get() == 0)
		{
			return true;
		}

		// DROP Table Data
		CResultSet::pointer resultSet;
		if (m_results.find(tableInfo.get(), resultSet, true))
		{
			const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
			BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
			CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
			for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
			//CRecordLine::pointer recordLine = resultSet->moveFirst();
			//while (recordLine.get() != 0)
			{
				CRecordLine::pointer recordLine = *pIterRecordList;
				m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
				//recordLine = resultSet->moveNext();
			}
		}

		// DROP FIELDINFO
		const CLockList<CFieldInfo::pointer> & pFieldList = tableInfo->fields();
		BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pFieldList.mutex()));
		CLockList<CFieldInfo::pointer>::const_iterator pIterField = pFieldList.begin();
		for (; pIterField!=pFieldList.end(); pIterField++)
		//CFieldInfo::pointer fieldInfo = tableInfo->moveFirst();
		//while (fieldInfo.get() != 0)
		{
			CFieldInfo::pointer fieldInfo = *pIterField;
			CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_DELETE, fieldInfo);
			modifyInfo->tableInfo(tableInfo);
			m_modifys.add(modifyInfo);
			//fieldInfo = tableInfo->moveNext();
		}

		// DROP TABLEINFO
		m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, tableInfo));

		return false;
	}

	bool CDatabase::createField(const CTableInfo::pointer& tableInfo, const CFieldInfo::pointer& fieldInfo)
	{
		if (m_killed) return false;
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);

		if (m_dbinfo->createField(tableInfo, fieldInfo))
		{
			//const CLockMap<tstring, CTableInfo::pointer>& pTables = m_dbinfo->tables();
			//CLockMap<tstring, CTableInfo::pointer>::const_iterator pIter1 = pTables.begin();
			//for (; pIter1!=pTables.end(); pIter1++)
			//{
			//	CTableInfo::pointer pTableInfo = pIter1->secnd;
			//	const CLockList<CFieldInfo::pointer>& pFields = pTableInfo->fields();
			//	CLockList<CFieldInfo::pointer>::const_iterator pIter2 = pFields.begin();
			//	{
			//		CFieldInfo::pointer pFieldInfo = *pIter2;
			//	}
			//}
			m_modifys.add(CModifyInfo::create(CModifyInfo::MIT_DATABASEINFO, CModifyInfo::MIF_UPDATE));	// update field_id

			CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_ADD, fieldInfo);
			modifyInfo->tableInfo(tableInfo);
			m_modifys.add(modifyInfo);

			// Set all the NULL fieldvariant to default value.
			CResultSet::pointer resultSet;
			if (m_results.find(tableInfo.get(), resultSet))
			{
				const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
				BoostWriteLock wtLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
				CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
				for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
				//CRecordLine::pointer recordLine = resultSet->moveFirst();
				//while (recordLine.get() != 0)
				{
					CRecordLine::pointer recordLine = *pIterRecordList;
					recordLine->setNullDefaultVariant(fieldInfo->id());
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, recordLine));
					//recordLine = resultSet->moveNext();
				}
			}
			return true;
		}
		return false;
	}
	bool CDatabase::dropField(const CTableInfo::pointer& tableInfo, const tstring& fieldname)
	{
		if (m_killed) return false;
		BOOST_ASSERT (tableInfo.get() != 0);
		CFieldInfo::pointer fieldInfo = tableInfo->getFieldInfo(fieldname);
		//CFieldInfo::pointer fieldInfo = m_dbinfo->dropField(tableInfo, fieldname);
		if (fieldInfo.get() != NULL)
		{
			// delete fieldinfo
			CResultSet::pointer resultSet;
			if (m_results.find(tableInfo.get(), resultSet))
			{
				// delete value
				{
					const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
					BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
					CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
					for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
						//CRecordLine::pointer recordLine = resultSet->moveFirst();
						//while (recordLine.get() != 0)
					{
						CRecordLine::pointer recordLine = *pIterRecordList;
						m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
						//recordLine = resultSet->moveNext();
					}
				}
				while (!m_modifys.empty())
				{
#ifdef WIN32
					Sleep(10);
#else
					usleep(10000);
#endif
				}
			}
			tableInfo->dropField(fieldname);	// ** drop FieldInfo
			CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_DELETE, fieldInfo);
			modifyInfo->tableInfo(tableInfo);
			m_modifys.add(modifyInfo);
			// add value 
			if (resultSet.get() != NULL)
			{
				{
					const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
					BoostWriteLock wtLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
					CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
					for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
						//CRecordLine::pointer recordLine = resultSet->moveFirst();
						//while (recordLine.get() != 0)
					{
						CRecordLine::pointer recordLine = *pIterRecordList;
						recordLine->delVariant(fieldInfo);
						m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, recordLine));
						//recordLine = resultSet->moveNext();
					}
				}
				while (!m_modifys.empty())
				{
#ifdef WIN32
					Sleep(10);
#else
					usleep(10000);
#endif
				}
			}
			return true;
		}
		return false;
	}
	bool CDatabase::modifyField(const CTableInfo::pointer& tableInfo, const tstring& fieldname, const CFieldInfo::pointer& fieldInfo)
	{
		if (m_killed) return false;
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (fieldInfo.get() != 0);

		if (m_dbinfo->modifyField(tableInfo, fieldname,fieldInfo))
		{
			CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_UPDATE, fieldInfo);
			modifyInfo->tableInfo(tableInfo);
			m_modifys.add(modifyInfo);
			CResultSet::pointer resultSet;
			if (m_results.find(tableInfo.get(), resultSet))
			{
				const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
				BoostWriteLock wtLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
				CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
				for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
				//CRecordLine::pointer recordLine = resultSet->moveFirst();
				//while (recordLine.get() != 0)
				{
					// delete before
					CRecordLine::pointer recordLine = *pIterRecordList;
					//m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, recordLine));
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, recordLine));
					//recordLine = resultSet->moveNext();
				}
			}
			return true;
		}
		return false;
	}

	//CTableInfo::pointer CDatabase::createTable(const tstring & tableName)
	//{
	//	CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableName);
	//	if (tableInfo.get() == 0)
	//	{
	//		tableInfo = m_dbinfo->createTable(tableName);
	//		BOOST_ASSERT (tableInfo.get() != 0);

	//		m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, tableInfo));
	//	}

	//	return tableInfo;
	//}
	CTableInfo::pointer CDatabase::getTableInfo(const tstring & tableName)
	{
		return m_dbinfo->getTableInfo(tableName, false);
	}

	bool CDatabase::rename(const tstring & tablename, const tstring & newName)
	{
		if (m_killed) return false;
		if (newName.empty()) return false;

		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tablename, false);
		if (tableInfo.get() == 0)
		{
			return false;
		}else if (tableInfo->name()==newName)
		{
			return true;
		}
		tableInfo->name(newName);
		m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, tableInfo));
		return true;
	}
	bool CDatabase::update(const tstring & tableName)
	{
		if (m_killed) return false;
		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableName, false);
		if (tableInfo.get() == 0)
		{
			return false;
		}
		m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, tableInfo));
		return true;
	}

	bool CDatabase::dropdefault(const tstring & tableName, const tstring & fieldName)
	{
		if (m_killed) return false;
		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableName, false);
		if (tableInfo.get() == 0)
		{
			return false;
		}

		CFieldInfo::pointer fieldInfo = tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0)
		{
			return false;
		}

		if (fieldInfo->hasDefaultValue())
		{
			fieldInfo->dropDefaultValue();
			CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_UPDATE, fieldInfo);
			modifyInfo->tableInfo(tableInfo);
			m_modifys.add(modifyInfo);
		}
		return true;
	}
	bool CDatabase::setdefault(const tstring & tableName, const tstring & fieldName, tagItemValue * defaultValue)
	{
		if (m_killed) return false;
		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableName, false);
		if (tableInfo.get() == 0)
		{
			return false;
		}

		CFieldInfo::pointer fieldInfo = tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0)
		{
			return false;
		}

		CFieldVariant::pointer defaultVariant = fieldInfo->buildDefaultValue();
		if (!ItemValue2Variant(defaultValue, defaultVariant))
		{
			return false;
		}

		// Set all the NULL fieldvariant to default value.
		CResultSet::pointer resultSet;
		if (m_results.find(tableInfo.get(), resultSet))
		{
			const CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList();
			BoostWriteLock wtLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
			CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
			for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
			//CRecordLine::pointer recordLine = resultSet->moveFirst();
			//while (recordLine.get() != 0)
			{
				CRecordLine::pointer recordLine = *pIterRecordList;
				recordLine->setNullDefaultVariant(fieldInfo->id());
				m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, recordLine));
				//recordLine = resultSet->moveNext();
			}
		}

		CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_UPDATE, fieldInfo);
		modifyInfo->tableInfo(tableInfo);
		m_modifys.add(modifyInfo);
		return true;
	}

	//CFieldInfo::pointer CDatabase::createField(CTableInfo::pointer tableInfo, const tstring & fieldName, FieldType fieldtype, uinteger len)
	//{
	//	BOOST_ASSERT (tableInfo.get() != 0);

	//	CFieldInfo::pointer fieldInfo = tableInfo->getFieldInfo(fieldName);
	//	if (fieldInfo.get() == 0)
	//	{
	//		fieldInfo = m_dbinfo->createField(tableInfo, fieldName, fieldtype, len);
	//		BOOST_ASSERT (tableInfo.get() != 0);

	//		CModifyInfo::pointer modifyInfo = CModifyInfo::create(CModifyInfo::MIF_ADD, fieldInfo);
	//		modifyInfo->tableInfo(tableInfo);
	//		m_modifys.add(modifyInfo);
	//	}

	//	return fieldInfo;
	//}
	bool CDatabase::insert(const CRecordLine::pointer& recordLine)
	{
		if (!this->isopened()|| m_killed) return false;
		BOOST_ASSERT (recordLine.get() != 0);
		BOOST_ASSERT (recordLine->tableInfo().get() != 0);

		CResultSet::pointer resultSet;
		if (!m_results.find(recordLine->tableInfo().get(), resultSet))
		{
			resultSet = CResultSet::create(recordLine->tableInfo());
			m_results.insert(recordLine->tableInfo().get(), resultSet);
		}
		bool bGetFirstPkList = false;
		std::vector<CRecordLine::pointer> pCheckPkList;

		// build default variant
		CTableInfo::pointer tableInfo = recordLine->tableInfo();
		const CLockList<CFieldInfo::pointer> & pFieldList = tableInfo->fields();
		BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pFieldList.mutex()));
		CLockList<CFieldInfo::pointer>::const_iterator pIterField = pFieldList.begin();
		for (; pIterField!=pFieldList.end(); pIterField++)
		//CFieldInfo::pointer fieldInfo =  tableInfo->moveFirst();
		//while (fieldInfo.get() != NULL)
		{
			CFieldInfo::pointer fieldInfo = *pIterField;
			CFieldVariant::pointer variant = recordLine->getVariant(fieldInfo->id());
			if (variant.get() == NULL)
			{
				if (fieldInfo->autoincrement())
				{
					variant = fieldInfo->buildFieldVariant();
					variant->setIntu(recordLine->id());
					recordLine->addVariant(fieldInfo, variant);
				}else
				{
					switch (fieldInfo->defaultValueType())
					{
					case CFieldInfo::DVT_CurrentTimestamp:
						{
							variant = fieldInfo->buildFieldVariant();
							variant->setCurrentTime();
							recordLine->addVariant(fieldInfo, variant);
						}break;
					default:
						{
							variant = fieldInfo->defaultValue();
							if (variant.get() == NULL)
							{
								// NOT NULL
								if (fieldInfo->isnotnull() || fieldInfo->isprimarykey())
								{
									return false;
								}
								variant = fieldInfo->buildFieldVariant();
								recordLine->addVariant(fieldInfo, variant);
							}else
							{
								recordLine->addVariant(fieldInfo, variant);
							}
						}break; 
					}
				}
			}else if (fieldInfo->constraintType()==FIELD_CONSTRAINT_PRIMARYKEY || fieldInfo->constraintType()==FIELD_CONSTRAINT_UNIQUE)
			{
				if (!bGetFirstPkList)
				{
					bGetFirstPkList = true;
					resultSet->getFieldEqualList(fieldInfo->id(), variant, pCheckPkList);
				}else if (!pCheckPkList.empty())
				{
					std::vector<CRecordLine::pointer> pCheckPkListTemp;
					for (size_t i=0;i<pCheckPkList.size(); i++)
					{
						CRecordLine::pointer pRecordLine = pCheckPkList[i];
						if (pRecordLine->equalFieldVariant(fieldInfo->id(),variant))
						{
							pCheckPkListTemp.push_back(pRecordLine);
						}
					}
					pCheckPkList.clear();
					for (size_t i=0;i<pCheckPkListTemp.size(); i++)
					{
						pCheckPkList.push_back(pCheckPkListTemp[i]);
					}
				}
			}

			//fieldInfo = tableInfo->moveNext();
		}
		if (!pCheckPkList.empty())
		{
			pCheckPkList.clear();
			return false;
		}

		resultSet->addRecord(recordLine);
		//m_records.insert(recordLine->id(), recordLine);
		if (!m_bFullMemory || tableInfo->type()==CTableInfo::TT_SYSTEM)
			m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_ADD, recordLine));

		// ? for current_recordid
		m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, tableInfo));
		return true;
	}
/*
	int CDatabase::deleters(CTableInfo::pointer tableInfo, const std::list<CFieldCompare::pointer> & wheres)
	{
		BOOST_ASSERT (tableInfo.get() != 0);

		CResultSet::pointer resultSet;
		if (!m_results.find(tableInfo, resultSet))
			return 0;

		int result = 0;
		CRecordLine::pointer recordLine = resultSet->moveFirst();
		while (recordLine.get() != 0)
		{
			bool compareResult = false;

			std::list<CFieldCompare::pointer>::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				bool compareAnd = (*iter)->compareAnd();
				CFieldCompare::FieldCompareType compareType = (*iter)->compareType();
				CFieldInfo::pointer compareField = (*iter)->compareField();
				CFieldVariant::pointer compareVariant = (*iter)->compareVariant();

				if (iter != wheres.begin() && !compareResult && compareAnd)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAnd)
				{
					// TRUE
					break;
				}

				CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
				if (varField.get() == 0)
					return result;

				compareResult = (*iter)->doCompare(varField);
			}

			if (compareResult)
			{
				result++;
				m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
				recordLine = resultSet->deleteCurrent();
			}else
				recordLine = resultSet->moveNext();
		}

		return result;
	}*/

	int CDatabase::deleters(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres)
	{
		if (!this->isopened()|| m_killed) return -1;
		BOOST_ASSERT (tableInfo.get() != 0);

		CResultSet::pointer resultSet;
		if (!m_results.find(tableInfo.get(), resultSet))
			return 0;

		std::vector<bo::uinteger> pRemoveList;
		int result = 0;
		CLockList<CRecordLine::pointer>& pRecordList = resultSet->GetRecordList2();
		BoostWriteLock wtLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
		CLockList<CRecordLine::pointer>::iterator pIterRecordList = pRecordList.begin();
		for (; pIterRecordList!=pRecordList.end(); )
		//CRecordLine::pointer recordLine = resultSet->moveFirst();
		//while (recordLine.get() != 0)
		{
			CRecordLine::pointer recordLine = *pIterRecordList;
			bool compareResult = true;

			std::list<std::list<CFieldCompare::pointer> >::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				bool compareResultSub = false;	// current compare result
				bool compareAndSub = false;
				short compareWhereLevel = 0;

				const std::list<CFieldCompare::pointer> & wheresub = *iter;
				std::list<CFieldCompare::pointer>::const_iterator itersub;
				for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
				{
					bool compareAnd = (*itersub)->compareAnd();
					CFieldInfo::pointer compareField = (*itersub)->compareField();
					CFieldVariant::pointer compareVariant = (*itersub)->compareVariant();
					short nWhereLevel = (*itersub)->whereLevel();

					// 2.0
					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (!compareResultSub && compareAnd && (compareWhereLevel == nWhereLevel || nWhereLevel==0))
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd && nWhereLevel == 0)
					{
						// TRUE
						break;
					}else if (compareResultSub && !compareAnd && compareWhereLevel == nWhereLevel)
					{
						// TRUE
						continue;
					}

					/* 1.0
					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (itersub != wheresub.begin() && !compareResultSub && compareAnd)
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd)
					{
						// TRUE
						break;
					}
					*/

					CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
					if (varField.get() == 0) return result;
					compareResultSub = (*itersub)->doCompare(varField);
					compareWhereLevel = nWhereLevel;
				}

				/* 1.0
				if (iter!=wheres.begin() && !compareResult && compareAndSub)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAndSub)
				{
					// TRUE
					break;
				}
				*/
				compareResult = compareResultSub;
				break;	// 2.0
				/*
				bool compareResultSub = false;
				bool compareAndSub = false;

				const std::list<CFieldCompare::pointer> & wheresub = *iter;
				std::list<CFieldCompare::pointer>::const_iterator itersub;
				for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
				{
					bool compareAnd = (*itersub)->compareAnd();
					//CFieldCompare::FieldCompareType compareType = (*itersub)->compareType();
					CFieldInfo::pointer compareField = (*itersub)->compareField();
					CFieldVariant::pointer compareVariant = (*itersub)->compareVariant();

					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (itersub != wheresub.begin() && !compareResultSub && compareAnd)
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd)
					{
						// TRUE
						break;
					}

					CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
					if (varField.get() == 0)
						return result;

					compareResultSub = (*itersub)->doCompare(varField);
				}

				if (iter!=wheres.begin() && !compareResult && compareAndSub)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAndSub)
				{
					// TRUE
					break;
				}

				compareResult = compareResultSub;*/
			}

			if (compareResult)
			{
				result++;
				if (!m_bFullMemory || tableInfo->type()==CTableInfo::TT_SYSTEM)
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_DELETE, recordLine));
				//recordLine = resultSet->deleteCurrent();
				pRemoveList.push_back(recordLine->id());
				pIterRecordList = pRecordList.erase(pIterRecordList);
			}else
			{
				pIterRecordList++;
				//recordLine = resultSet->moveNext();
			}
		}
		for (size_t i=0; i<pRemoveList.size(); i++)
		{
			resultSet->RemoteRecord(pRemoveList[i]);
		}
		if (resultSet->empty())
		{
			m_results.remove(tableInfo.get());
		}

		return result;
	}

	CResultSet::pointer CDatabase::select(const tstring & tableName)
	{
		if (!this->isopened()|| m_killed) return boNullResultSet;
		CResultSet::pointer result;
		CTableInfo::pointer tableInfo = m_dbinfo->getTableInfo(tableName, false);
		if (tableInfo.get() != 0)
		{
			m_results.find(tableInfo.get(), result);
		}

		return result;
	}
/*
	CResultSet::pointer CDatabase::select(CTableInfo::pointer tableInfo, const std::list<CFieldCompare::pointer> & wheres)
	{
		BOOST_ASSERT (tableInfo.get() != 0);

		CResultSet::pointer result = CResultSet::create(tableInfo);
		CResultSet::pointer tableResultSet;
		if (!m_results.find(tableInfo, tableResultSet))
			return result;

		CRecordLine::pointer recordLine = tableResultSet->moveFirst();
		while (recordLine.get() != 0)
		{
			bool compareResult = false;

			std::list<CFieldCompare::pointer>::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				bool compareAnd = (*iter)->compareAnd();
				CFieldCompare::FieldCompareType compareType = (*iter)->compareType();
				CFieldInfo::pointer compareField = (*iter)->compareField();
				CFieldVariant::pointer compareVariant = (*iter)->compareVariant();

				if (iter != wheres.begin() && !compareResult && compareAnd)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAnd)
				{
					// TRUE
					break;
				}

				CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
				if (varField.get() == 0)
					return result;

				compareResult = (*iter)->doCompare(varField);
			}

			if (compareResult)
			{
				result->addRecord(recordLine);
			}
			recordLine = tableResultSet->moveNext();
		}

		return result;
	}
*/
	CResultSet::pointer CDatabase::select(const CSelectInfo& pSelectInfo, bool distinct)
	//CResultSet::pointer CDatabase::select(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, bool distinct)
	{
		if (!this->isopened()|| m_killed) return boNullResultSet;

		const CTableInfo::pointer& tableInfo = pSelectInfo.m_pFirstTableInfo;
		BOOST_ASSERT (tableInfo.get() != 0);
		const std::list<std::list<CFieldCompare::pointer> > & wheres = pSelectInfo.wheres;
		const CLockMap<void*,CTableInfo::pointer>& m_pOutPutTableInfoList = pSelectInfo.m_pOutPutTableInfoList;

		CTableInfo::pointer pFirstTableInfo;
		CLockMap<tstring,CTableInfo::pointer> pWhereTableList;
		{
			// 先取出所有 where 的 table info
			std::list<std::list<CFieldCompare::pointer> >::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				const std::list<CFieldCompare::pointer> & wheresub = *iter;
				std::list<CFieldCompare::pointer>::const_iterator itersub;
				for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
				{
					const CTableInfo::pointer& compareTable = (*itersub)->tableInfo();
					if (pFirstTableInfo.get()==0)
						pFirstTableInfo = compareTable;
					//if (pTableInfoList.exist(compareTable->name()))
					{
						pWhereTableList.insert(compareTable->name(), compareTable, false, 0, false);
					}
					const CTableInfo::pointer& compareTable2 = (*itersub)->tableInfo2();
					if (compareTable2.get()!=0 /*&& pTableInfoList.exist(compareTable2->name())*/)
					{
						pWhereTableList.insert(compareTable2->name(), compareTable2, false, 0, false);
					}
				}
			}
		}
		const bool bMutilTable = pWhereTableList.size()>1?true:false;
		bo::uinteger nRecordId = 0;

		CLockMap<void*, CResultSet::pointer> resultstemp1;
		CLockMap<void*, CResultSet::pointer> resultstemp2;	// 关联表数据
		CResultSet::pointer result;// = CResultSet::create(tableInfo);
		CTableInfo::pointer pToDoTableInfo;
		CTableInfo::pointer pTableInfo = pFirstTableInfo.get()==0?tableInfo:pFirstTableInfo;
		while (true)
		{
			result = CResultSet::create(pTableInfo);
			pWhereTableList.remove(pTableInfo->name());

			CResultSet::pointer tableResultSet;
			if (!resultstemp1.find(pTableInfo.get(),tableResultSet) && !m_results.find(pTableInfo.get(), tableResultSet))
			{
				return result;
			}

			bool bCompareFinished = wheres.empty()?true:false;
			//CRecordLine::pointer pFinishedRecordLineTrueTemp;
			const CLockList<CRecordLine::pointer>& pRecordList = tableResultSet->GetRecordList();
			BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
			CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
			for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
			{
				std::vector<CRecordLine::pointer> pFinishedRecordLineTrueTempList;
				CRecordLine::pointer recordLine = *pIterRecordList;
				bool compareResult = true;
				std::list<std::list<CFieldCompare::pointer> >::const_iterator iter;
				for (iter=wheres.begin(); iter!=wheres.end(); iter++)
				{
					bool compareResultSub = false;	// current compare result
					bool compareAndSub = false;
					short compareWhereLevel = 0;
					bool bFirstCompare = true;

					const std::list<CFieldCompare::pointer> & wheresub = *iter;
					std::list<CFieldCompare::pointer>::const_iterator itersub;
					for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
					{
						const CFieldCompare::pointer& pFieldCompare = *itersub;
						const bool compareAnd = (*itersub)->compareAnd();
						const CTableInfo::pointer& compareTable = pFieldCompare->tableInfo();
						const CTableInfo::pointer& compareTable2 = pFieldCompare->tableInfo2();
						const CFieldInfo::pointer& compareField = pFieldCompare->compareField();
						const CFieldInfo::pointer& compareField2 = pFieldCompare->compareField2();
						//const CFieldVariant::pointer& compareVariant = pFieldCompare->compareVariant();
						const short nWhereLevel = pFieldCompare->whereLevel();

						if (resultstemp1.exist(compareTable.get(),false) && (compareTable2.get()==0 || resultstemp1.exist(compareTable2.get(),false)))
						//if (pFieldCompare->GetFinished())
						{
							// 当前 where 已经做过比较
							continue;
						}

						// 2.0
						if (bFirstCompare || itersub == wheresub.begin())
						{
							compareAndSub = compareAnd;
						}else if (!compareResultSub && compareAnd && (compareWhereLevel == nWhereLevel || compareWhereLevel==0))	// AND
						{
							// FALSE
							break;
						}else if (compareResultSub && !compareAnd && compareWhereLevel == 0)										// OR
						{
							// TRUE
							break;
						}else if (compareResultSub && !compareAnd && compareWhereLevel == nWhereLevel)								// OR
						{
							// TRUE
							continue;
						}
						if (compareTable.get() != pTableInfo.get())
						{
							// 不是当前比较 where table
							CResultSet::pointer pFinishedResultSet;
							if (compareTable2.get()!=0 && compareTable2.get()==pTableInfo.get() && compareField2.get()!=0 && resultstemp1.find(compareTable.get(),pFinishedResultSet))
							{
								// 找到前面已经比较成功结果
								bFirstCompare = false;
								bCompareFinished = true;
								compareResultSub = false;
								resultstemp2.insert(compareTable.get(),pFinishedResultSet,false,0,false);
								const CLockList<CRecordLine::pointer>& pFinishedRecordList = pFinishedResultSet->GetRecordList();
								BoostReadLock rdLockFinishedRecordList(const_cast<boost::shared_mutex&>(pFinishedRecordList.mutex()));
								CLockList<CRecordLine::pointer>::const_iterator pIterFinishedRecordList = pFinishedRecordList.begin();
								for (; pIterFinishedRecordList!=pFinishedRecordList.end(); pIterFinishedRecordList++)
								{
									// 跟前一个结果集比较
									CRecordLine::pointer pFinishedRecordLine = *pIterFinishedRecordList;
									CFieldVariant::pointer varField = recordLine->getVariant(compareField2->id());
									if (pFinishedRecordLine->doCompareFieldVariant(compareField->id(),pFieldCompare->compareType(),varField))
									{
										// 比较成功
										compareResultSub = true;
										pFinishedRecordLineTrueTempList.push_back(pFinishedRecordLine);
										//pFinishedRecordLineTrueTemp = pFinishedRecordLine;
										//pFinishedRecordLine->m_pExtDataList.insert(recordLine.get(),true,false);
										//recordLine->m_pExtDataList.insert(pFinishedRecordLine.get(),true,false);
										//pFinishedRecordLine->m_nExtData |= RECORD_LINE_EXT_DATA_TRUE_RESULT_FLAG;
										if (!m_pOutPutTableInfoList.exist(compareTable.get()))
											break;
									}
								}
							}else
							{
								if (pToDoTableInfo.get()==0)
									pToDoTableInfo = compareTable;
							}
							compareWhereLevel = nWhereLevel;
							continue;
						}else if (compareTable2.get()!=0 && compareField2.get()!=0 && compareTable2.get()!=compareTable.get())
						{
							// 当前 where 第二比较是其他 table
							CResultSet::pointer pFinishedResultSet;
							if (compareTable.get()==pTableInfo.get() && resultstemp1.find(compareTable2.get(),pFinishedResultSet))
							{
								// 找到前面已经比较成功结果
								bFirstCompare = false;
								bCompareFinished = true;
								compareResultSub = false;
								resultstemp2.insert(compareTable2.get(),pFinishedResultSet,false,0,false);
								const CLockList<CRecordLine::pointer>& pFinishedRecordList = pFinishedResultSet->GetRecordList();
								BoostReadLock rdLockFinishedRecordList(const_cast<boost::shared_mutex&>(pFinishedRecordList.mutex()));
								CLockList<CRecordLine::pointer>::const_iterator pIterFinishedRecordList = pFinishedRecordList.begin();
								for (; pIterFinishedRecordList!=pFinishedRecordList.end(); pIterFinishedRecordList++)
								{
									// 跟前一个结果集比较
									CRecordLine::pointer pFinishedRecordLine = *pIterFinishedRecordList;
									CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
									if (pFinishedRecordLine->doCompareFieldVariant(compareField2->id(),pFieldCompare->compareType(),varField))
									{
										// 比较成功
										compareResultSub = true;
										pFinishedRecordLineTrueTempList.push_back(pFinishedRecordLine);
										//pFinishedRecordLineTrueTemp = pFinishedRecordLine;
										//pFinishedRecordLine->m_nExtData |= RECORD_LINE_EXT_DATA_TRUE_RESULT_FLAG;
										if (!m_pOutPutTableInfoList.exist(compareTable2.get()))
											break;
									}
								}
							}else
							{
								if (pToDoTableInfo.get()==0)
									pToDoTableInfo = compareTable2;
							}
							compareWhereLevel = nWhereLevel;
							continue;
						}

						bFirstCompare = false;
						CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
						if (varField.get() == 0) return result;
						compareResultSub = pFieldCompare->doCompare(varField);
						compareWhereLevel = nWhereLevel;
						//pFieldCompare->SetFinished();
						bCompareFinished = true;
					}

					compareResult = compareResultSub;
					break;	// 2.0
				}

				if (compareResult)
				{
					if (bMutilTable)
					{
						CRecordLine::pointer record2 = recordLine->Clone(++nRecordId);
						for (size_t i=0; i<pFinishedRecordLineTrueTempList.size(); i++)
						{
							CRecordLine::pointer pLine =  pFinishedRecordLineTrueTempList[i];
							pLine->m_pExtDataList.insert(record2.get(),true,false);
							record2->m_pExtDataList.insert(pLine.get(),true,false);
						}
						result->addRecord(record2);
					}else
					{
						result->addRecord(recordLine);
					}

				//}else if (bMutilTable && !pFinishedRecordLineTrueTempList.empty())
				//{
				//	// 前面找到的记录，最后比较失败；
				//	for (size_t i=0; i<pFinishedRecordLineTrueTempList.size(); i++)
				//	{
				//		CRecordLine::pointer pLine =  pFinishedRecordLineTrueTempList[i];
				//		pLine->m_nExtData &= ~RECORD_LINE_EXT_DATA_TRUE_RESULT_FLAG;
				//	}
				}
				pFinishedRecordLineTrueTempList.clear();
				if (!bCompareFinished)
					break;
			}
			if (bMutilTable && !resultstemp2.empty(false))
			{
				// 有关联表数据；
				while (!resultstemp2.empty(false))
				{
					CLockMap<void*, CResultSet::pointer>::iterator pIterResultsTemp = resultstemp2.begin();
					CResultSet::pointer pFinishedResultSet = pIterResultsTemp->second;
					const void * pTableInfoPoint = pIterResultsTemp->first;
					resultstemp2.erase(pIterResultsTemp);

					// 先清除比较失败数据；
					CLockList<CRecordLine::pointer>& pFinishedRecordList = pFinishedResultSet->GetRecordList2();
					CLockList<CRecordLine::pointer>::iterator pIterFinishedRecordList = pFinishedRecordList.begin();
					for (; pIterFinishedRecordList!=pFinishedRecordList.end(); )
					{
						CRecordLine::pointer pFinishedRecordLine = *pIterFinishedRecordList;
						if (pFinishedRecordLine->m_pExtDataList.empty())
						{
							pIterFinishedRecordList = pFinishedRecordList.erase(pIterFinishedRecordList);
						}else
						{
							pIterFinishedRecordList++;
						}
					}
					const bool nExistResult = m_pOutPutTableInfoList.exist(result->getTableInfo().get());
					const bool nExistFinished = m_pOutPutTableInfoList.exist(pFinishedResultSet->getTableInfo().get());
					CResultSet::pointer result1;
					CResultSet::pointer result2;
					if (nExistResult && !nExistFinished)
					{
						result1 = result;
						result2 = pFinishedResultSet;
					}else if (!nExistResult && nExistFinished)
					{
						result1 = pFinishedResultSet;
						result2 = result;
						result = result1;
					}else
					{
						result1 = pFinishedResultSet->size()>=result->size()?pFinishedResultSet:result;
						result2 = pFinishedResultSet->size()>=result->size()?result:pFinishedResultSet;
						result = result1;
					}

					const CLockList<CRecordLine::pointer>& pFinishedRecordList1 = result1->GetRecordList();
					const CLockList<CRecordLine::pointer>& pFinishedRecordList2 = result2->GetRecordList();
					CLockList<CRecordLine::pointer>::const_iterator pIterFinishedRecordList1 = pFinishedRecordList1.begin();
					for (; pIterFinishedRecordList1!=pFinishedRecordList1.end(); pIterFinishedRecordList1++)
					{
						CRecordLine::pointer pFinishedRecordLine1 = *pIterFinishedRecordList1;

						CLockList<CRecordLine::pointer>::const_iterator pIterFinishedRecordList2 = pFinishedRecordList2.begin();
						for (; pIterFinishedRecordList2!=pFinishedRecordList2.end(); pIterFinishedRecordList2++)
						{
							CRecordLine::pointer pFinishedRecordLine2 = *pIterFinishedRecordList2;
							if (pFinishedRecordLine1->m_pExtDataList.remove(pFinishedRecordLine2.get()))
							{
								pFinishedRecordLine1->AddRecordLine(pFinishedRecordLine2, false);
							}
							//pFinishedRecordLine1->AddRecordLine(pFinishedRecordLine2, false);
							////pFinishedRecordLine2->AddRecordLine(pFinishedRecordLine1,false);
						}
					}
					//resultstemp1.insert((void*)pTableInfoPoint, result, true);	// 关联比较成功，记下临时result
					resultstemp1.insert((void*)pTableInfoPoint, result1, true, 0, false);	// 关联比较成功，记下临时result
				}
			}
			if (bCompareFinished)
			{
				resultstemp1.insert(pTableInfo.get(), result, true, 0, false);	// 记下临时result
			}

			if (pToDoTableInfo.get()!=0)
			{
				// 找到前面未比较表数据；
				pTableInfo = pToDoTableInfo;
				pToDoTableInfo.reset();
				continue;
			}
			if (pWhereTableList.empty())
			{
				if (bMutilTable)
				{
					if (!m_pOutPutTableInfoList.exist(pTableInfo.get()))
					{
						// 不是当前输入表数据，更换默认表数据；
						resultstemp1.find(tableInfo.get(),result,true);
					//}else if (result->getTableInfo().get()!=tableInfo.get() && resultstemp1.exist(tableInfo.get()))
					//{
					//	// 使用默认表数据；
					//	resultstemp1.find(tableInfo.get(),result,true);
					}
					resultstemp1.remove(tableInfo.get(),false);	// 去掉默认表数据
					resultstemp1.remove(pTableInfo.get(),false);	// 去掉当前表数据
				}
				break;
			}
			CLockMap<tstring,CTableInfo::pointer>::iterator pIter = pWhereTableList.begin();
			pTableInfo = pIter->second;
		}
		while (bMutilTable && !resultstemp1.empty(false))
		{
			// 检查是否有未关联表数据，需要加到一起输出；
			CLockMap<void*, CResultSet::pointer>::iterator pIterResultsTemp = resultstemp1.begin();
			CResultSet::pointer pResultSetTrueTemp = pIterResultsTemp->second;
			resultstemp1.erase(pIterResultsTemp);
			if (pResultSetTrueTemp.get()!=result.get())
			{
				// 没有关联表，（有多余表数据，需要加到一起输出）
				// ??
			}
		}

		return result;
	}
/*
	int CDatabase::update(CTableInfo::pointer tableInfo, const std::list<CFieldCompare::pointer> & wheres, CRecordLine::pointer updateVal)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (updateVal.get() != 0);

		int result = 0;
		CResultSet::pointer tableResultSet;
		if (!m_results.find(tableInfo, tableResultSet))
			return 0;

		CRecordLine::pointer recordLine = tableResultSet->moveFirst();
		while (recordLine.get() != 0)
		{
			bool compareResult = false;

			std::list<CFieldCompare::pointer>::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				bool compareAnd = (*iter)->compareAnd();
				CFieldCompare::FieldCompareType compareType = (*iter)->compareType();
				CFieldInfo::pointer compareField = (*iter)->compareField();
				CFieldVariant::pointer compareVariant = (*iter)->compareVariant();

				if (iter != wheres.begin() && !compareResult && compareAnd)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAnd)
				{
					// TRUE
					break;
				}

				CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
				if (varField.get() == 0)
					return result;

				compareResult = (*iter)->doCompare(varField);
			}

			if (compareResult)
			{
				bool update = false;
				CFieldInfo::pointer fieldInfoTable = tableInfo->moveFirst();
				while (fieldInfoTable.get() != 0)
				{
					CFieldVariant::pointer updateFieldVariant = updateVal->getVariant(fieldInfoTable->id());
					if (updateFieldVariant.get() != 0)
					{
						update = true;
						recordLine->updateVariant(fieldInfoTable, updateFieldVariant);
					}

					fieldInfoTable = tableInfo->moveNext();
				}

				if (update)
				{
					++result;
					m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, recordLine));
				}
			}

			recordLine = tableResultSet->moveNext();
		}

		return result;
	}
*/
	int CDatabase::update(const CTableInfo::pointer& tableInfo, const std::list<std::list<CFieldCompare::pointer> > & wheres, const CRecordLine::pointer& updateVal)
	{
		if (!this->isopened()|| m_killed) return -1;
		BOOST_ASSERT (tableInfo.get() != 0);
		BOOST_ASSERT (updateVal.get() != 0);

		int result = 0;
		CResultSet::pointer tableResultSet;
		if (!m_results.find(tableInfo.get(), tableResultSet))
			return 0;

		const CLockList<CRecordLine::pointer>& pRecordList = tableResultSet->GetRecordList();
		BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
		CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
		for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
		//CRecordLine::pointer recordLine = tableResultSet->moveFirst();
		//while (recordLine.get() != 0)
		{
			CRecordLine::pointer recordLine = *pIterRecordList;
			bool compareResult = true;

			std::list<std::list<CFieldCompare::pointer> >::const_iterator iter;
			for (iter=wheres.begin(); iter!=wheres.end(); iter++)
			{
				bool compareResultSub = false;	// current compare result
				bool compareAndSub = false;
				short compareWhereLevel = 0;

				const std::list<CFieldCompare::pointer> & wheresub = *iter;
				std::list<CFieldCompare::pointer>::const_iterator itersub;
				for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
				{
					bool compareAnd = (*itersub)->compareAnd();
					CFieldInfo::pointer compareField = (*itersub)->compareField();
					CFieldVariant::pointer compareVariant = (*itersub)->compareVariant();
					short nWhereLevel = (*itersub)->whereLevel();

					// 2.0
					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (!compareResultSub && compareAnd && (compareWhereLevel == nWhereLevel || nWhereLevel==0))
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd && nWhereLevel == 0)
					{
						// TRUE
						break;
					}else if (compareResultSub && !compareAnd && compareWhereLevel == nWhereLevel)
					{
						// TRUE
						continue;
					}

					/* 1.0
					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (itersub != wheresub.begin() && !compareResultSub && compareAnd)
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd)
					{
						// TRUE
						break;
					}
					*/

					CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
					if (varField.get() == 0) return result;
					compareResultSub = (*itersub)->doCompare(varField);
					compareWhereLevel = nWhereLevel;
				}

				/* 1.0
				if (iter!=wheres.begin() && !compareResult && compareAndSub)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAndSub)
				{
					// TRUE
					break;
				}
				*/
				compareResult = compareResultSub;
				break;	// 2.0
				/*
				bool compareResultSub = false;
				bool compareAndSub = false;

				const std::list<CFieldCompare::pointer> & wheresub = *iter;
				std::list<CFieldCompare::pointer>::const_iterator itersub;
				for (itersub=wheresub.begin(); itersub!=wheresub.end(); itersub++)
				{
					bool compareAnd = (*itersub)->compareAnd();
					//CFieldCompare::FieldCompareType compareType = (*itersub)->compareType();
					CFieldInfo::pointer compareField = (*itersub)->compareField();
					CFieldVariant::pointer compareVariant = (*itersub)->compareVariant();

					if (itersub == wheresub.begin())
					{
						compareAndSub = compareAnd;
					}else if (itersub != wheresub.begin() && !compareResultSub && compareAnd)
					{
						// FALSE
						break;
					}else if (compareResultSub && !compareAnd)
					{
						// TRUE
						break;
					}

					CFieldVariant::pointer varField = recordLine->getVariant(compareField->id());
					if (varField.get() == 0)
						return result;

					compareResultSub = (*itersub)->doCompare(varField);
				}

				if (iter!=wheres.begin() && !compareResult && compareAndSub)
				{
					// FALSE
					break;
				}else if (compareResult && !compareAndSub)
				{
					// TRUE
					break;
				}

				compareResult = compareResultSub;*/
			}

			if (compareResult)
			{
				bool update = false;
				const CLockList<CFieldInfo::pointer> & pFieldList = tableInfo->fields();
				BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pFieldList.mutex()));
				CLockList<CFieldInfo::pointer>::const_iterator pIterField = pFieldList.begin();
				for (; pIterField!=pFieldList.end(); pIterField++)
				//CFieldInfo::pointer fieldInfoTable = tableInfo->moveFirst();
				//while (fieldInfoTable.get() != 0)
				{
					CFieldInfo::pointer fieldInfoTable = *pIterField;
					CFieldVariant::pointer updateFieldVariant = updateVal->getVariant(fieldInfoTable->id());
					if (updateFieldVariant.get() != 0)
					{
						update = true;
						recordLine->updateVariant(fieldInfoTable, updateFieldVariant);
					}

					//fieldInfoTable = tableInfo->moveNext();
				}

				if (update)
				{
					++result;
					if (!m_bFullMemory || tableInfo->type()==CTableInfo::TT_SYSTEM)
						m_modifys.add(CModifyInfo::create(CModifyInfo::MIF_UPDATE, recordLine));
				}
			}

			//recordLine = tableResultSet->moveNext();
		}

		return result;
	}

	void CDatabase::do_proc_db(void)
	{
		BOOST_ASSERT (m_dbinfo.get() != 0);
		BOOST_ASSERT (m_fdb.is_open());

		// .bdf: main data file
		// .bnf: data file
		// .blf: log file

		while (!m_killed || !m_modifys.empty())
		{
			CModifyInfo::pointer modifyInfo;
			if (!m_modifys.front(modifyInfo))
			{
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			}

			m_fdb.clear();
			switch (modifyInfo->type())
			{
			case CModifyInfo::MIT_RECORDLINE:
				{
					CRecordLine::pointer recordLine = modifyInfo->recordLine();
					BOOST_ASSERT (recordLine.get() != 0);
					CTableInfo::pointer tableInfo = recordLine->tableInfo();
					BOOST_ASSERT (tableInfo.get() != 0);

					if (modifyInfo->flag() == CModifyInfo::MIF_ADD)
					{
						CPageHeadInfo::pointer pageHeadInfo = tableInfo->findDataPage(recordLine->getLineSize()+SMALLINT_SIZE);
						if (pageHeadInfo.get() == 0)
						{
							// ?? Maybe the PHT_DATA before the PHT_INFO error.
							if (m_unusepages.front(pageHeadInfo))
							{
								pageHeadInfo->type(CPageHeadInfo::PHT_DATA);
								pageHeadInfo->objectid(tableInfo->id());
							}else
							{
								writeNewBlackPage(m_fdb);
								pageHeadInfo = CPageHeadInfo::create(m_curpageid++, CPageHeadInfo::PHT_DATA, tableInfo->id()); 
								//m_pages.add(pageHeadInfo);
							}
							tableInfo->m_datapages.insert(pageHeadInfo->id(), pageHeadInfo);
						}
						tableInfo->m_rlpages.insert(recordLine->id(), pageHeadInfo);

						// Serialize CPageHeadInfo.
						m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
						pageHeadInfo->icount();
						pageHeadInfo->dunused(recordLine->getLineSize()+SMALLINT_SIZE);
						pageHeadInfo->Serialize(true, m_fdb);

						// Serialize CRecordLine.
						short offset = 0;
						short findCount = 0;
						m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE, std::ios::beg);
						while (true)
						{
							m_fdb.read((char*)&offset, SMALLINT_SIZE);
							if (offset == 0)
								break;
							findCount++;
							m_fdb.seekg(-2*(short)(SMALLINT_SIZE), std::ios::cur);
						}
						pageHeadInfo->m_idindexs.insert(recordLine->id(), findCount);

						// Write CRecordLine
						offset = MAX_PAGEHEADINFO_SIZE+(findCount)*recordLine->getLineSize();
						m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
						usmallint outDoSize = 0;
						recordLine->Serialize(true, m_fdb, outDoSize);

						// Write offset
						m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(findCount+1), std::ios::beg);
						m_fdb.write((const char*)&offset, SMALLINT_SIZE);

						// Write VARCHAR
						writeData2(m_fdb, recordLine, modifyInfo->flag());
					}else if (modifyInfo->flag() == CModifyInfo::MIF_UPDATE)
					{
						CPageHeadInfo::pointer pageHeadInfo;
						if (tableInfo->m_rlpages.find(recordLine->id(), pageHeadInfo, false))
						{
							usmallint index = 0;
							pageHeadInfo->m_idindexs.find(recordLine->id(), index, false);

							// Write CRecordLine
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id()+MAX_PAGEHEADINFO_SIZE+recordLine->getLineSize()*index, std::ios::beg);
							usmallint outDoSize = 0;
							recordLine->Serialize(true, m_fdb, outDoSize);

							// Update VARCHAR
							writeData2(m_fdb, recordLine, modifyInfo->flag());
						}
					}else if (modifyInfo->flag() == CModifyInfo::MIF_DELETE)
					{
						CPageHeadInfo::pointer pageHeadInfo;
						if (tableInfo->m_rlpages.find(recordLine->id(), pageHeadInfo, true))
						{
							// Serialize CPageHeadInfo.
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
							if (pageHeadInfo->count() > 1)
							{
								pageHeadInfo->dcount();
								pageHeadInfo->iunused(recordLine->getLineSize()+SMALLINT_SIZE);
								pageHeadInfo->Serialize(true, m_fdb);

								usmallint index = 0;
								pageHeadInfo->m_idindexs.find(recordLine->id(), index, true);

								// Write CRecordLine
								if (index > 0)
									m_fdb.seekp(recordLine->getLineSize()*index, std::ios::cur);
								writeBlackNull(m_fdb, recordLine->getLineSize());

								// Write offset
								m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(index+1), std::ios::beg);
								writeBlackNull(m_fdb, SMALLINT_SIZE);
							}else
							{
								tableInfo->m_datapages.remove(pageHeadInfo->id());
								pageHeadInfo->reset();
								pageHeadInfo->Serialize(true, m_fdb);
								writeBlackNull(m_fdb, PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE);
								m_unusepages.add(pageHeadInfo);
							}
							// Delete VARCHAR
							writeData2(m_fdb, recordLine, modifyInfo->flag());
						}
					}

				}break;
			case CModifyInfo::MIT_DATABASEINFO:
				{
					CPageHeadInfo::pointer pageHeadInfo;
					if (modifyInfo->flag() == CModifyInfo::MIF_ADD)
					{
						writeNewBlackPage(m_fdb);
						pageHeadInfo = CPageHeadInfo::create(m_curpageid++, CPageHeadInfo::PHT_INFO, 0);
						pageHeadInfo->subtype(CPageHeadInfo::PST_TABLEINFO);
						m_tableinfopages.add(pageHeadInfo);

						// Serialize CPageHeadInfo.
						m_fdb.seekp(0, std::ios::beg);
						pageHeadInfo->dunused(MAX_DATABASEINFO_SIZE);
						pageHeadInfo->Serialize(true, m_fdb);
					}else if (modifyInfo->flag() == CModifyInfo::MIF_UPDATE)
					{
						// **
					}else
					{
						//m_tableinfopages.front(pageHeadInfo, false);
					}

					//BOOST_ASSERT (pageHeadInfo.get() != 0);

					// Serialize CDatabaseInfo.
					m_fdb.seekp(MAX_PAGEHEADINFO_SIZE, std::ios::beg);
					m_dbinfo->Serialize(true, m_fdb);
				}break;
			case CModifyInfo::MIT_TABLEINFO:
				{
					CTableInfo::pointer tableInfo = modifyInfo->tableInfo();
					BOOST_ASSERT (tableInfo.get() != 0);

					if (modifyInfo->flag() == CModifyInfo::MIF_ADD)
					{
						CPageHeadInfo::pointer pageHeadInfo = findTableInfoPage(MAX_TABLEINFO_SIZE + SMALLINT_SIZE);
						if (pageHeadInfo.get() == 0)
						{
							if (m_unusepages.front(pageHeadInfo))
							{
								pageHeadInfo->type(CPageHeadInfo::PHT_INFO);
							}else
							{
								writeNewBlackPage(m_fdb);
								pageHeadInfo = CPageHeadInfo::create(m_curpageid++, CPageHeadInfo::PHT_INFO, 0);
								//m_pages.add(pageHeadInfo);
							}
							pageHeadInfo->subtype(CPageHeadInfo::PST_TABLEINFO);
							m_tableinfopages.add(pageHeadInfo);
						}
						m_tablepages.insert(tableInfo->id(), pageHeadInfo);

						// Serialize CPageHeadInfo.
						m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
						pageHeadInfo->icount();
						pageHeadInfo->dunused(MAX_TABLEINFO_SIZE + SMALLINT_SIZE);
						pageHeadInfo->Serialize(true, m_fdb);

						// Serialize CTableInfo.
						short offset = 0;
						short findCount = 0;
						m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE, std::ios::beg);
						while (true)
						{
							findCount++;
							m_fdb.read((char*)&offset, SMALLINT_SIZE);
							if (offset == 0)
								break;
							m_fdb.seekg(-2*(short)(SMALLINT_SIZE), std::ios::cur);
						}

						pageHeadInfo->m_idindexs.insert(tableInfo->id(), findCount-1);

						// Write CTableInfo
						offset = MAX_PAGEHEADINFO_SIZE+(findCount-1)*MAX_TABLEINFO_SIZE;
						if (pageHeadInfo->id() == 0)
						{
							offset += MAX_DATABASEINFO_SIZE;
						}
						m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
						tableInfo->Serialize(true, m_fdb);

						// Write offset
						m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*findCount, std::ios::beg);
						m_fdb.write((const char*)&offset, SMALLINT_SIZE);
					}else if (modifyInfo->flag() == CModifyInfo::MIF_UPDATE)
					{
						CPageHeadInfo::pointer pageHeadInfo;
						if (m_tablepages.find(tableInfo->id(), pageHeadInfo, false))
						{
							usmallint index = 0;
							pageHeadInfo->m_idindexs.find(tableInfo->id(), index, false);

							// Write CTableInfo
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id()+MAX_PAGEHEADINFO_SIZE+MAX_TABLEINFO_SIZE*index, std::ios::beg);
							if (pageHeadInfo->id() == 0)
								m_fdb.seekp(MAX_DATABASEINFO_SIZE, std::ios::cur);

							tableInfo->Serialize(true, m_fdb);
						}

					}else if (modifyInfo->flag() == CModifyInfo::MIF_DELETE)
					{
						// DELETE TABLE DATA
						CPageHeadInfo::pointer result;
						CLockMap<uinteger, CPageHeadInfo::pointer>::iterator iter;
						for (iter=tableInfo->m_datapages.begin(); iter!=tableInfo->m_datapages.end(); iter++)
						{
							CPageHeadInfo::pointer pageHeadInfo = iter->second;

							// Serialize CPageHeadInfo.
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
							pageHeadInfo->reset();
							pageHeadInfo->Serialize(true, m_fdb);
							writeBlackNull(m_fdb, PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE);
							m_unusepages.add(pageHeadInfo);
						}
						tableInfo->m_datapages.clear();

						CPageHeadInfo::pointer pageHeadInfo;
						if (m_tablepages.find(tableInfo->id(), pageHeadInfo, true))
						{
							// Serialize CPageHeadInfo.
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
							if (pageHeadInfo->count() > 1 || pageHeadInfo->id() == 0)
							{
								pageHeadInfo->dcount();
								pageHeadInfo->iunused(MAX_TABLEINFO_SIZE+SMALLINT_SIZE);
								pageHeadInfo->Serialize(true, m_fdb);

								usmallint index = 0;
								pageHeadInfo->m_idindexs.find(tableInfo->id(), index, true);

								// Write CTableInfo
								if (pageHeadInfo->id() == 0)
									m_fdb.seekp(MAX_DATABASEINFO_SIZE, std::ios::cur);

								m_fdb.seekp(MAX_TABLEINFO_SIZE*index, std::ios::cur);
								writeBlackNull(m_fdb, MAX_TABLEINFO_SIZE);

								// Write offset
								m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(index+1), std::ios::beg);
								writeBlackNull(m_fdb, SMALLINT_SIZE);
							}else
							{
								pageHeadInfo->reset();
								pageHeadInfo->Serialize(true, m_fdb);
								writeBlackNull(m_fdb, PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE);
								m_unusepages.add(pageHeadInfo);
							}
						}
					}
				}break;
			case CModifyInfo::MIT_FIELDINFO:
				{
					CTableInfo::pointer tableInfo = modifyInfo->tableInfo();
					CFieldInfo::pointer fieldInfo = modifyInfo->fieldInfo();
					BOOST_ASSERT (tableInfo.get() != 0);
					BOOST_ASSERT (fieldInfo.get() != 0);

					if (modifyInfo->flag() == CModifyInfo::MIF_ADD)
					{
						CPageHeadInfo::pointer pageHeadInfo = findFieldInfoPage(INTEGER_SIZE+MAX_FIELDINFO_SIZE+SMALLINT_SIZE);
						if (pageHeadInfo.get() == 0)
						{
							// Maybe PST_FIELDINFO before the PST_TABLEINFO error.
							/*if (m_unusepages.front(pageHeadInfo))
							{
								pageHeadInfo->type(CPageHeadInfo::PHT_INFO);
								pageHeadInfo->objectid(1);
							}else*/
							{
								writeNewBlackPage(m_fdb);
								pageHeadInfo = CPageHeadInfo::create(m_curpageid++, CPageHeadInfo::PHT_INFO, 0);
								//m_pages.add(pageHeadInfo);
							}
							pageHeadInfo->subtype(CPageHeadInfo::PST_FIELDINFO);
							m_fieldinfopages.add(pageHeadInfo);
						}
						tableInfo->m_fipages.insert(fieldInfo->id(), pageHeadInfo);

						// Serialize CPageHeadInfo.
						m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
						pageHeadInfo->icount();
						pageHeadInfo->dunused(INTEGER_SIZE+MAX_FIELDINFO_SIZE+SMALLINT_SIZE);
						//pageHeadInfo->dunused(INTEGER_SIZE+MAX_FIELDINFO_SIZE+fieldInfo->defdatasize()+SMALLINT_SIZE);
						pageHeadInfo->Serialize(true, m_fdb);

						// Serialize CFieldInfo
						short offset = 0;
						short findCount = 0;
						m_fdb.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE, std::ios::beg);
						while (true)
						{
							findCount++;
							m_fdb.read((char*)&offset, SMALLINT_SIZE);
							if (offset == 0)
								break;
							m_fdb.seekg(-2*(short)(SMALLINT_SIZE), std::ios::cur);
						}

						pageHeadInfo->m_idindexs.insert(fieldInfo->id(), findCount-1);

						// Write CFieldInfo
						offset = MAX_PAGEHEADINFO_SIZE+(findCount-1)*(INTEGER_SIZE+MAX_FIELDINFO_SIZE);
						m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id()+offset, std::ios::beg);
						uinteger tableid = tableInfo->id();
						m_fdb.write((const char*)&tableid, INTEGER_SIZE);
						fieldInfo->Serialize(true, m_fdb);

						// Write offset
						m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*findCount, std::ios::beg);
						m_fdb.write((const char*)&offset, SMALLINT_SIZE);
					}else if (modifyInfo->flag() == CModifyInfo::MIF_UPDATE)
					{
						CPageHeadInfo::pointer pageHeadInfo;
						if (tableInfo->m_fipages.find(fieldInfo->id(), pageHeadInfo, false))
						{
							usmallint index = 0;
							pageHeadInfo->m_idindexs.find(fieldInfo->id(), index, false);

							// Write CFieldInfo
							short offset = MAX_PAGEHEADINFO_SIZE+index*MAX_FIELDINFO_SIZE+(index+1)*INTEGER_SIZE;
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id()+offset, std::ios::beg);
							fieldInfo->Serialize(true, m_fdb);
						}

					}else if (modifyInfo->flag() == CModifyInfo::MIF_DELETE)
					{
						CPageHeadInfo::pointer pageHeadInfo;
						if (tableInfo->m_fipages.find(fieldInfo->id(), pageHeadInfo, true))
						{
							// Serialize CPageHeadInfo.
							m_fdb.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
							pageHeadInfo->dcount();
							pageHeadInfo->iunused(INTEGER_SIZE+MAX_FIELDINFO_SIZE+SMALLINT_SIZE);
							pageHeadInfo->Serialize(true, m_fdb);

							usmallint index = 0;
							pageHeadInfo->m_idindexs.find(fieldInfo->id(), index, true);

							// Write CFieldInfo
							m_fdb.seekp((INTEGER_SIZE+MAX_FIELDINFO_SIZE)*index, std::ios::cur);
							writeBlackNull(m_fdb, INTEGER_SIZE+MAX_FIELDINFO_SIZE);

							// Write offset
							m_fdb.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(index+1), std::ios::beg);
							writeBlackNull(m_fdb, SMALLINT_SIZE);
						}
					}

				}break;
			default:
				break;
			}

			m_fdb.flush();
		}
		m_fdb.close();
	}
	void CDatabase::writeNewBlackPage(tfstream& ar) const
	{
		ar.seekp(0, std::ios::end);
		char * tempBuffer = new char[PAGE_BLOCK_SIZE+1];
		memset(tempBuffer, 0, PAGE_BLOCK_SIZE+1);
		ar.write(tempBuffer, PAGE_BLOCK_SIZE);
		delete[] tempBuffer;
	}
	void CDatabase::writeBlackNull(tfstream& ar, usmallint size) const
	{
		char * tempBuffer = new char[size+1];
		memset(tempBuffer, 0, size+1);
		ar.write(tempBuffer, size);
		delete[] tempBuffer;
	}
	void CDatabase::writeData2(tfstream& ar, const CRecordLine::pointer& recordLine, CModifyInfo::ModifyInfoFlag modifyFlag)
	{
		BOOST_ASSERT (recordLine.get() != 0);
		CTableInfo::pointer tableInfo = recordLine->tableInfo();
		BOOST_ASSERT (tableInfo.get() != 0);

		const CLockMap<uinteger, CFieldVariant::pointer>& pVariantList = recordLine->GetVariantList();
		AUTO_CONST_RLOCK(pVariantList);
		CLockMap<uinteger, CFieldVariant::pointer>::const_iterator pIter = pVariantList.begin();
		for (; pIter!=pVariantList.end(); pIter++)
		//CFieldVariant::pointer fieldVariant = recordLine->moveFirst();
		//while (fieldVariant.get() != NULL)
		{
			const uinteger fieldId = pIter->first;;
			CFieldVariant::pointer fieldVariant = pIter->second;

			if (modifyFlag == CModifyInfo::MIF_UPDATE || modifyFlag == CModifyInfo::MIF_DELETE)
			{
				// Clear CPageHeadInfo.
				CRecordDOIs::pointer recordDOIs;
				if (tableInfo->m_recordfdois.find(recordLine->id(), recordDOIs, false))
				{
					CFieldDOIs::pointer fieldDOIs;
					if (recordDOIs->m_dois.find(fieldId, fieldDOIs, true))
					{
						while (true)
						{
							CData2OffsetInfo::pointer data2OffsetInfo;
							if (!fieldDOIs->m_dois.front(data2OffsetInfo))
							{
								break;
							}
							CPageHeadInfo::pointer pageHeadInfo = data2OffsetInfo->page();
							BOOST_ASSERT (pageHeadInfo.get() != NULL);

							usmallint needSize = 0;
							switch (pageHeadInfo->subtype())
							{
							case CPageHeadInfo::PST_8000:
								{
									needSize = 8000;
								}break;
							case CPageHeadInfo::PST_1000:
								{
									needSize = 1000;
								}break;
							case CPageHeadInfo::PST_200:
								{
									needSize = 200;
								}break;
							case CPageHeadInfo::PST_50:
								{
									needSize = 50;
								}break;
							case CPageHeadInfo::PST_20:
								{
									needSize = 20;
								}break;
							default:
								assert (false);
								break;
							}
							// Serialize CPageHeadInfo.
							ar.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
							if (pageHeadInfo->count() > 1)
							{
								pageHeadInfo->dcount();
								pageHeadInfo->iunused(INTEGER_SIZE*3+SMALLINT_SIZE+needSize);
								if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
									pageHeadInfo->iunused(SMALLINT_SIZE);
								pageHeadInfo->Serialize(true, ar);

								// Serialize varchar
								ar.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id()+data2OffsetInfo->offset(), std::ios::beg);
								writeBlackNull(ar, INTEGER_SIZE*3+SMALLINT_SIZE+needSize);

								// Serialize offset
								if (pageHeadInfo->subtype() != CPageHeadInfo::PST_8000)
								{
									ar.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(data2OffsetInfo->index()+1), std::ios::beg);
									writeBlackNull(ar, SMALLINT_SIZE);
								}
							}else
							{
								tableInfo->m_data2pages.remove(pageHeadInfo->id());
								pageHeadInfo->reset();
								pageHeadInfo->Serialize(true, ar);
								writeBlackNull(ar, PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE);
								m_unusepages.add(pageHeadInfo);
							}
						}
					}
				}
			}

			if (modifyFlag == CModifyInfo::MIF_ADD || modifyFlag == CModifyInfo::MIF_UPDATE)
			{
				uinteger towriteSize = 0;
				uinteger writedSize = 0;
				const char * towriteBuffer = NULL;
				switch (fieldVariant->VARTYPE)
				{
				case bo::FT_VARCHAR:
					{
						towriteSize = fieldVariant->v.varcharVal.size;
						towriteBuffer = fieldVariant->v.varcharVal.buffer;
					}break;
				case bo::FT_CLOB:
					{
						towriteSize = fieldVariant->v.clobVal.size;
						towriteBuffer = fieldVariant->v.clobVal.buffer;
					}break;
				default:
					break;
				}
				if (towriteBuffer==0)
				{
					//fieldVariant = recordLine->moveNext();
					continue;
				}

				const uinteger recordId = recordLine->id();
				while (towriteSize > writedSize)
				{
					const uinteger unwritesize = towriteSize - writedSize;
					CPageHeadInfo::PageHeadSubType1 subtype = CPageHeadInfo::PST_8000;
					usmallint needSize = 0;
					//if (unwritesize >= (PAGE_BLOCK_SIZE-MAX_PAGEHEADINFO_SIZE))
					if (unwritesize >= 8000)
					{
						subtype = CPageHeadInfo::PST_8000;
						needSize = 8000;
					}else if (unwritesize >= 1000)
					{
						subtype = CPageHeadInfo::PST_1000;
						needSize = 1000;
					}else if (unwritesize >= 200)
					{
						subtype = CPageHeadInfo::PST_200;
						needSize = 200;
					}else if (unwritesize >= 50)
					{
						if (unwritesize > 150)
						{
							subtype = CPageHeadInfo::PST_200;
							needSize = 200;
						}else
						{
							subtype = CPageHeadInfo::PST_50;
							needSize = 50;
						}
					}else
					{
						if (unwritesize > 40)
						{
							subtype = CPageHeadInfo::PST_50;
							needSize = 50;
						}else
						{
							subtype = CPageHeadInfo::PST_20;
							needSize = 20;
						}
					}
					const char * writeBuffer = towriteBuffer + writedSize;
					const usmallint writeSize = unwritesize > needSize ? needSize : unwritesize;
					CPageHeadInfo::pointer pageHeadInfo = tableInfo->findData2Page(needSize+SMALLINT_SIZE*2+INTEGER_SIZE*3, subtype);
					if (pageHeadInfo.get() == 0)
					{
						if (m_unusepages.front(pageHeadInfo))
						{
							pageHeadInfo->type(CPageHeadInfo::PHT_DATA2);
							pageHeadInfo->objectid(tableInfo->id());
						}else
						{
							writeNewBlackPage(ar);
							pageHeadInfo = CPageHeadInfo::create(m_curpageid++, CPageHeadInfo::PHT_DATA2, tableInfo->id()); 
						}
						pageHeadInfo->subtype(subtype);
						tableInfo->m_data2pages.insert(pageHeadInfo->id(), pageHeadInfo);
					}

					// Serialize CPageHeadInfo.
					ar.seekp(PAGE_BLOCK_SIZE*pageHeadInfo->id(), std::ios::beg);
					pageHeadInfo->icount();
					pageHeadInfo->dunused(INTEGER_SIZE*3+SMALLINT_SIZE+needSize);
					if (subtype != CPageHeadInfo::PST_8000)
					{
						// offset index size
						pageHeadInfo->dunused(SMALLINT_SIZE);
					}
					pageHeadInfo->Serialize(true, ar);

					// Serialize value.
					short offset = 0;
					short findIndex = 0;
					if (subtype != CPageHeadInfo::PST_8000)
					{
						ar.seekg(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE, std::ios::beg);
						while (true)
						{
							ar.read((char*)&offset, SMALLINT_SIZE);
							if (offset == 0)
								break;
							findIndex++;
							ar.seekg(-2*(short)(SMALLINT_SIZE), std::ios::cur);
						}

						//pageHeadInfo->m_idindexs.insert(recordLine->id(), findIndex);

						offset = MAX_PAGEHEADINFO_SIZE+(findIndex)*(INTEGER_SIZE*3+SMALLINT_SIZE+needSize);
						ar.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id())+offset, std::ios::beg);
					}

					// Write value
					CRecordDOIs::pointer recordDOIs;
					if (!tableInfo->m_recordfdois.find(recordId, recordDOIs))
					{
						recordDOIs = CRecordDOIs::create(recordId);
						tableInfo->m_recordfdois.insert(recordId, recordDOIs);
					}
					CFieldDOIs::pointer fieldDOIs;
					if (!recordDOIs->m_dois.find(fieldId, fieldDOIs))
					{
						fieldDOIs = CFieldDOIs::create(fieldId);
						recordDOIs->m_dois.insert(fieldId, fieldDOIs);
					}
					fieldDOIs->m_dois.add(CData2OffsetInfo::create(pageHeadInfo, offset, findIndex));

					//fieldVariant
					ar.write((const char*)&recordId, INTEGER_SIZE);		// record id
					ar.write((const char*)&fieldId, INTEGER_SIZE);		// field id
					ar.write((const char*)&writedSize, INTEGER_SIZE);	// buffer index
					ar.write((const char*)&writeSize, SMALLINT_SIZE);	// real size
					if (fieldVariant->VARTYPE == bo::FT_VARCHAR || fieldVariant->VARTYPE == bo::FT_CLOB)
					{
						char * buffer = new char[writeSize+1];
						memcpy(buffer, writeBuffer, writeSize);
						for (int i=0; i<writeSize; i+=2)
						{
							buffer[i] = buffer[i]+(char)i+(char)writeSize;
						}
						buffer[writeSize] = '\0';
						ar.write(buffer, writeSize);
						delete[] buffer;
					}else
					{
						ar.write(writeBuffer, writeSize);
					}

					// Write offset
					if (subtype != CPageHeadInfo::PST_8000)
					{
						ar.seekp(PAGE_BLOCK_SIZE*(pageHeadInfo->id()+1)-SMALLINT_SIZE*(findIndex+1), std::ios::beg);
						ar.write((const char*)&offset, SMALLINT_SIZE);
					}
					writedSize += writeSize;
				}
			}
			//fieldVariant = recordLine->moveNext();
		}
		if (modifyFlag == CModifyInfo::MIF_DELETE)
		{
			tableInfo->m_recordfdois.remove(recordLine->id());
		}
	}


	CPageHeadInfo::pointer CDatabase::findTableInfoPage(usmallint needSize)
	{
		// ? lock
		CPageHeadInfo::pointer result;
		CLockList<CPageHeadInfo::pointer>::iterator iter;
		for (iter=m_tableinfopages.begin(); iter!=m_tableinfopages.end(); iter++)
		{
			if ((*iter)->unused() > needSize)
			{
				result = (*iter);
				break;
			}
		}
		return result;
	}
	CPageHeadInfo::pointer CDatabase::findFieldInfoPage(usmallint needSize)
	{
		// ? lock
		CPageHeadInfo::pointer result;
		CLockList<CPageHeadInfo::pointer>::iterator iter;
		for (iter=m_fieldinfopages.begin(); iter!=m_fieldinfopages.end(); iter++)
		{
			if ((*iter)->unused() > needSize)
			{
				result = (*iter);
				break;
			}
		}
		return result;
	}
	void CDatabase::proc_db(CDatabase * owner)
	{
		BOOST_ASSERT (owner != 0);
		owner->do_proc_db();
	}


} // namespace bo

