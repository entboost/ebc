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
#include "../stl/lockmap.h"
#include "bodb2.h"
#include "../sqlparser/sqlparser.h"
#include "dbservice.h"
#include "bobase.h"
#include <boost/date_time.hpp>
using namespace boost::gregorian;  
using namespace boost::local_time;  
using namespace boost::posix_time; 

using namespace bo;

//CDbService::pointer gDb;
namespace bo
{
	bool GetOrderBys(const CTableInfo::pointer& tableInfo, const CLockMap<tstring,CTableInfo::pointer>& pTableInfoList, tagItems * itemOrderBys, std::vector<bo::uinteger> & outTopOrderBys);
	bool GetWheres(const CTableInfo::pointer& tableInfo, const CLockMap<tstring,CTableInfo::pointer>& pTableInfoList, tagItems * itemWheres, std::list<std::list<CFieldCompare::pointer> > & outTopWheres);
	bool GetValues(CTableInfo::pointer tableInfo, tagItems * itemValues, CRecordLine::pointer outRecordLine);
	FieldConstraintType ToConstraintType(enum_field_constraint type);

	class CBodb
		: public CDbService
	{
	public:
		typedef boost::shared_ptr<CBodb> pointer;
		static CBodb::pointer create(void)
		{
			return CBodb::pointer(new CBodb());
		}

	protected:
		static CFieldInfo::pointer GetFieldInfo(const tagField * field)
		{
			CFieldInfo::pointer fieldInfo;
			if (field == NULL || field->field_name==NULL) return NullFieldInfo;
			switch (field->field_type)
			{
			case SQL_TYPE_BOOLEAN:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_BOOLEAN);
				break;
			case SQL_TYPE_TINYINT:
				fieldInfo = CFieldInfo::create(field->field_name, field->unsigned_field ? bo::FT_UTINYINT : bo::FT_TINYINT);
				break;
			case SQL_TYPE_SMALLINT:
				fieldInfo = CFieldInfo::create(field->field_name, field->unsigned_field ? bo::FT_USMALLINT : bo::FT_SMALLINT);
				break;
			case SQL_TYPE_INTEGER:
				fieldInfo = CFieldInfo::create(field->field_name, field->unsigned_field ? bo::FT_UINTEGER : bo::FT_INTEGER);
				break;
			case SQL_TYPE_BIGINT:
				fieldInfo = CFieldInfo::create(field->field_name, field->unsigned_field ? bo::FT_UBIGINT : bo::FT_BIGINT);
				break;
			case SQL_TYPE_NUMERIC:
			case SQL_TYPE_DECIMAL:
				{
					fieldInfo = CFieldInfo::create(field->field_name, bo::FT_NUMERIC, field->size1);
					fieldInfo->size2(field->size2);
				}break;
			case SQL_TYPE_FLOAT:
				{
					if (field->size1 > MAX_NUMERIC_SIZE)
					{
						return NullFieldInfo;
						//breakError = true;
						//break;
					}
					fieldInfo = CFieldInfo::create(field->field_name, bo::FT_FLOAT, field->size1);
				}break;
			case SQL_TYPE_REAL:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_REAL);
				break;
			case SQL_TYPE_DOUBLE:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_DOUBLE);
				break;
			case SQL_TYPE_DATE:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_DATE);
				break;
			case SQL_TYPE_TIME:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_TIME);
				break;
			case SQL_TYPE_TIMESTAMP:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_TIMESTAMP);
				break;
			case SQL_TYPE_CHAR:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_CHAR, field->size1);
				break;
			case SQL_TYPE_VARCHAR:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_VARCHAR, field->size1);
				break;
			case SQL_TYPE_CLOB:
				fieldInfo = CFieldInfo::create(field->field_name, bo::FT_CLOB, field->size1 == 0 ? MAX_CLOB_SIZE : field->size1);
				break;
			default:
				break;
			}

			//if (breakError) break;

			BOOST_ASSERT (fieldInfo.get() != 0);
			fieldInfo->constraintType(ToConstraintType(field->constraint));

			if (field->default_value != 0)
			{
				switch (field->default_value->value_type)
				{
				case VALUE_FUNC_CURRENT_TIMESTAMP:
					{
						fieldInfo->defaultValueType(CFieldInfo::DVT_CurrentTimestamp);
					}break;
				default:
					{
						CFieldVariant::pointer defaultVariant = fieldInfo->buildDefaultValue();
						if (!ItemValue2Variant(field->default_value, defaultVariant))
						{
							return NullFieldInfo;
							//breakError = true;
							//break;
						}
					}break;
				}
			}

			fieldInfo->references(field->references);
			fieldInfo->autoincrement(field->auto_increment);
			return fieldInfo;
		}

		virtual int execsql(const char * sql, PRESULTSET * outResultSet = 0)
		{
			bool breakError = false;
			int result = -1;
			tagSP * sp = parse_exec(sql);
			if (sp != 0)
			{
				switch (sp->sql_command)
				{
				case SQLCOM_SELECT:
					{
						CLockMap<tstring,CTableInfo::pointer> pTableInfoList;
						CSelectInfo pSelectInfo;
						for (short i=0;i<sp->tables->itemcount; i++)
						{
							tagTable * table = (tagTable*)sp->tables->items[i]->item_handle;
							if (table == 0 || table->table_name == 0)
							{
								pTableInfoList.clear(false);
								break;
							}
							CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
							if (tableInfo.get() == 0)
							{
								pTableInfoList.clear(false);
								break;
							}
							if (pSelectInfo.m_pFirstTableInfo.get()==0)
								pSelectInfo.m_pFirstTableInfo = tableInfo;
							pTableInfoList.insert(table->table_name,tableInfo,true,0,false);
							if (table->alias_name != 0)
							{
								pTableInfoList.insert(table->alias_name,tableInfo,false,0,false);
							}
						}
						if (pTableInfoList.empty(false))
						{
							// ? error
							break;
						}

						// WHERES
						if (!GetWheres(pSelectInfo.m_pFirstTableInfo, pTableInfoList, sp->wheres, pSelectInfo.wheres))
						{
							break;
						}
						// ORDER BY
						std::vector<bo::uinteger> orderbys;
						if (!GetOrderBys(pSelectInfo.m_pFirstTableInfo, pTableInfoList, sp->orderbys, orderbys))
						{
							break;
						}

						// 先获取输出表；
						if (sp->items == 0)
						{
							//pSelectInfo.m_pOutPutTableInfoList.insert(pSelectInfo.m_pFirstTableInfo.get(), pSelectInfo.m_pFirstTableInfo);
						}else
						{
							for (short i=0; i<sp->items->itemcount; i++)
							{
								enum_funciton_type functionType = FUNCTION_UNKNOWN;
								void * functionHandle = NULL;
								CFieldVariant::pointer fieldVariant;
								switch (sp->items->items[i]->item_type)
								{
								case STRING_ITEM:
									{
										const char * tablename = (const char *)sp->items->items[i]->table_name;
										//const char * fieldname = (const char *)sp->items->items[i]->item_handle;
										CTableInfo::pointer pTableInfo;
										if (tablename==0)
										{
											//pOutPutTableInfoList.insert(pFirstTableInfo.get(), pFirstTableInfo,false);
										}else if (pTableInfoList.find(tablename,pTableInfo))
										{
											pSelectInfo.m_pOutPutTableInfoList.insert(pTableInfo.get(), pTableInfo,false,0,false);
										}
									}break;
								case FUNC_ITEM:
									{
										tagFunc * pFunc = (tagFunc*)sp->items->items[i]->item_handle;
										functionType = pFunc->function_type;
										switch (functionType)
										{
										case FUNCTION_TO_INT:
										case FUNCTION_SECOND:
										case FUNCTION_MINUTE:
										case FUNCTION_HOUR:
										case FUNCTION_MONTH:
										case FUNCTION_YEAR:
										case FUNCTION_YDAY:
										case FUNCTION_MDAY:
										case FUNCTION_WDAY:
											{
												if (pFunc->items->itemcount != 1)
												{
													breakError = true;
													break;
												}
												const char * tablename = (const char *)pFunc->items->items[0]->table_name;
												//const char * fieldname = (const char *)pFunc->items->items[0]->item_handle;
												CTableInfo::pointer pTableInfo;
												if (tablename==0)
												{
													//pOutPutTableInfoList.insert(pFirstTableInfo.get(), pFirstTableInfo,false);
												}else if (pTableInfoList.find(tablename,pTableInfo))
												{
													pSelectInfo.m_pOutPutTableInfoList.insert(pTableInfo.get(), pTableInfo,false,0,false);
												}
											}break;
										case FUNCTION_DATE_FORMAT:
											{
												if (pFunc->items->itemcount != 2)
												{
													breakError = true;
													break;
												}
												const char * tablename = (const char *)pFunc->items->items[0]->table_name;
												//const char * fieldname = (const char *)pFunc->items->items[0]->item_handle;
												functionHandle = pFunc->items->items[1]->item_handle;
												CTableInfo::pointer pTableInfo;
												if (tablename==0)
												{
													//pOutPutTableInfoList.insert(pFirstTableInfo.get(), pFirstTableInfo,false);
												}else if (pTableInfoList.find(tablename,pTableInfo))
												{
													pSelectInfo.m_pOutPutTableInfoList.insert(pTableInfo.get(), pTableInfo,false,0,false);
												}
											}break;
										default:
											break;
										}
									}break;
								default:
									break;
								}
							}
						}
						if (breakError)
							break;

						const bool distinct = sp->parameter == (void*)1;
						CResultSet::pointer rs = CDbService::select(pSelectInfo, false);
						rs->OrderBy(orderbys, sp->orderbydesc==1);
						rs->LimitOffset(sp->offset, sp->limit);
						result = rs->size();

						if (outResultSet == 0)
							break;

						*outResultSet = new RESULTSET;
						memset(*outResultSet, 0, sizeof(RESULTSET));

						if (result > 0)
						{
							(*outResultSet)->rsvalues = new PRECORDLINE[result];
						}

						const CLockList<CRecordLine::pointer>& pRecordList = rs->GetRecordList();
						BoostReadLock rdLockRecordList(const_cast<boost::shared_mutex&>(pRecordList.mutex()));
						CLockList<CRecordLine::pointer>::const_iterator pIterRecordList = pRecordList.begin();
						for (; pIterRecordList!=pRecordList.end(); pIterRecordList++)
						{
							CRecordLine::pointer recordLine = *pIterRecordList;
							if (distinct)
							{
								bool equalRecordLine = false;
								for (int i=0; i<(*outResultSet)->rscount; i++)
								{
									int j=0;
									equalRecordLine = true;
									if (sp->items == 0)
									{
										const CLockMap<uinteger, CFieldVariant::pointer>& pVariantList = recordLine->GetVariantList();
										AUTO_CONST_RLOCK(pVariantList);
										CLockMap<uinteger, CFieldVariant::pointer>::const_iterator pIter = pVariantList.begin();
										for (; pIter!=pVariantList.end(); pIter++)
										{
											CFieldVariant::pointer fieldVariant = pIter->second;
											PFIELDVARIANT fieldVariantCompare = (*outResultSet)->rsvalues[i]->fieldvalues[j++];
											if(!fieldVariant->equal(CFieldVariant::create(fieldVariantCompare)))
											{
												equalRecordLine = false;
												break;
											}
										}

										//CFieldVariant::pointer fieldVariant = recordLine->moveFirst();
										//while (fieldVariant.get() != 0)
										//{
										//	PFIELDVARIANT fieldVariantCompare = (*outResultSet)->rsvalues[i]->fieldvalues[j++];
										//	if(!fieldVariant->equal(CFieldVariant::create(fieldVariantCompare)))
										//	{
										//		equalRecordLine = false;
										//		break;
										//	}
										//	fieldVariant = recordLine->moveNext();
										//}
									}else
									{
										for (j=0; j<(*outResultSet)->rsvalues[i]->fieldcount; j++)
										{
											PFIELDVARIANT fieldVariantCompare = (*outResultSet)->rsvalues[i]->fieldvalues[j];
											const char * tablename = (const char *)sp->items->items[j]->table_name;
											const char * fieldname = (const char *)sp->items->items[j]->item_handle;
											CFieldVariant::pointer fieldVariant;
											CTableInfo::pointer pTableInfo;
											if (tablename==0)
											{
												fieldVariant = recordLine->getVariant(fieldname);
											}else if (pTableInfoList.find(tablename,pTableInfo))
											{
												CFieldInfo::pointer pFieldInfo = pTableInfo->getFieldInfo(fieldname);
												if (pFieldInfo.get()==0)
													break;
												fieldVariant = recordLine->getVariant(pFieldInfo->id());
											}
											BOOST_ASSERT (fieldVariant.get() != NULL);
											if(!fieldVariant->equal(CFieldVariant::create(fieldVariantCompare)))
											{
												equalRecordLine = false;
												break;
											}
										}
									}

									if (equalRecordLine)
									{
										break;
									}
								}
								if (equalRecordLine)
								{
									//recordLine = rs->moveNext();
									continue;
								}
							}

							if (sp->items == 0)
							{
								if (recordLine->size() > 0)
								{
									(*outResultSet)->rscount++;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1] = new RECORDLINE;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount = 0;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldvalues = new PFIELDVARIANT[recordLine->size()];
								}
								const CLockMap<uinteger, CFieldVariant::pointer>& pVariantList = recordLine->GetVariantList();
								AUTO_CONST_RLOCK(pVariantList);
								CLockMap<uinteger, CFieldVariant::pointer>::const_iterator pIter = pVariantList.begin();
								for (; pIter!=pVariantList.end(); pIter++)
								{
									CFieldVariant::pointer fieldVariant = pIter->second;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount++;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldvalues[(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount-1] = fieldVariant->getValue();
								}
							}else
							{
								if (sp->items->itemcount > 0)
								{
									(*outResultSet)->rscount++;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1] = new RECORDLINE;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount = 0;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldvalues = new PFIELDVARIANT[sp->items->itemcount];
								}

								for (short i=0; i<sp->items->itemcount; i++)
								{
									enum_funciton_type functionType = FUNCTION_UNKNOWN;
									void * functionHandle = NULL;
									CFieldVariant::pointer fieldVariant;
									switch (sp->items->items[i]->item_type)
									{
									case STRING_ITEM:
										{
											const char * tablename = (const char *)sp->items->items[i]->table_name;
											const char * fieldname = (const char *)sp->items->items[i]->item_handle;
											CTableInfo::pointer pTableInfo;
											if (tablename==0)
											{
												fieldVariant = recordLine->getVariant(fieldname);
											}else if (pTableInfoList.find(tablename,pTableInfo))
											{
												CFieldInfo::pointer pFieldInfo = pTableInfo->getFieldInfo(fieldname);
												if (pFieldInfo.get()==0)
													break;
												fieldVariant = recordLine->getVariant(pFieldInfo->id());
											}
										}break;
									case FUNC_ITEM:
										{
											tagFunc * pFunc = (tagFunc*)sp->items->items[i]->item_handle;
											functionType = pFunc->function_type;
											switch (functionType)
											{
											case FUNCTION_TO_INT:
											case FUNCTION_SECOND:
											case FUNCTION_MINUTE:
											case FUNCTION_HOUR:
											case FUNCTION_MONTH:
											case FUNCTION_YEAR:
											case FUNCTION_YDAY:
											case FUNCTION_MDAY:
											case FUNCTION_WDAY:
												{
													if (pFunc->items->itemcount != 1)
													{
														breakError = true;
														break;
													}
													const char * tablename = (const char *)pFunc->items->items[0]->table_name;
													const char * fieldname = (const char *)pFunc->items->items[0]->item_handle;
													CTableInfo::pointer pTableInfo;
													if (tablename==0)
													{
														fieldVariant = recordLine->getVariant(fieldname);
													}else if (pTableInfoList.find(tablename,pTableInfo))
													{
														CFieldInfo::pointer pFieldInfo = pTableInfo->getFieldInfo(fieldname);
														if (pFieldInfo.get()==0)
															break;
														fieldVariant = recordLine->getVariant(pFieldInfo->id());
													}
												}break;
											case FUNCTION_DATE_FORMAT:
												{
													if (pFunc->items->itemcount != 2)
													{
														breakError = true;
														break;
													}
													const char * tablename = (const char *)pFunc->items->items[0]->table_name;
													const char * fieldname = (const char *)pFunc->items->items[0]->item_handle;
													functionHandle = pFunc->items->items[1]->item_handle;
													CTableInfo::pointer pTableInfo;
													if (tablename==0)
													{
														fieldVariant = recordLine->getVariant(fieldname);
													}else if (pTableInfoList.find(tablename,pTableInfo))
													{
														CFieldInfo::pointer pFieldInfo = pTableInfo->getFieldInfo(fieldname);
														if (pFieldInfo.get()==0)
															break;
														fieldVariant = recordLine->getVariant(pFieldInfo->id());
													}
												}break;
											default:
												break;
											}

										}break;
									default:
										break;
									}

									if (fieldVariant.get() == 0)
									{
										breakError = true;
										break;
									}

									PFIELDVARIANT pFieldVariant = NULL;
									switch (functionType)
									{
									case FUNCTION_TO_INT:
										{
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(fieldVariant->getInt());
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_SECOND:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_sec);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_MINUTE:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_min);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_HOUR:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_hour);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_MONTH:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_mon + 1);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_YEAR:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_year + 1900);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_YDAY:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_yday);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_MDAY:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_mday);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_WDAY:
										{
											botime_t nTime = (botime_t)fieldVariant->getBigInt();
											tm * pTm = localtime((const time_t*)&nTime);
											CFieldVariant fieldVariantTemp(FT_INTEGER);
											fieldVariantTemp.setInt(pTm->tm_wday);
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									case FUNCTION_DATE_FORMAT:
										{
											const char * sFormatString = (const char *)functionHandle;

											botime_t tFormatTime = fieldVariant->getInt();
											boost::local_time::time_zone_ptr m_tz = boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone("+8")); 

											boost::posix_time::ptime pt = from_time_t(tFormatTime);
											boost::local_time::local_date_time dt(pt, m_tz); 

											time_facet * facet = new time_facet(sFormatString);
											std::stringstream ss;
											ss.imbue(std::locale(std::cout.getloc(), facet));
											ss << dt.local_time();

											char buffer[50];
											memset(buffer, 0, 50);
											ss.get(buffer, 50);

											CFieldVariant fieldVariantTemp(FT_VARCHAR);
											fieldVariantTemp.setString(buffer, strlen(buffer));
											pFieldVariant = fieldVariantTemp.getValue();
										}break;
									default:
										pFieldVariant = fieldVariant->getValue();
										break;
									}

									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount++;
									(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldvalues[(*outResultSet)->rsvalues[(*outResultSet)->rscount-1]->fieldcount-1] = pFieldVariant;
								}
							}

							if (breakError)
								break;
							//recordLine = rs->moveNext();
						}

					}break;
				case SQLCOM_DELETE:
					{
						//tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						//if (table == 0 || table->table_name == 0)
						//	break;
						//CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
						//if (tableInfo.get() == 0)
						//	break;
						CLockMap<tstring,CTableInfo::pointer> pTableInfoList;
						CTableInfo::pointer pFirstTableInfo;
						for (short i=0;i<sp->tables->itemcount; i++)
						{
							tagTable * table = (tagTable*)sp->tables->items[i]->item_handle;
							if (table == 0 || table->table_name == 0)
							{
								pTableInfoList.clear();
								break;
							}
							CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
							if (tableInfo.get() == 0)
							{
								pTableInfoList.clear();
								break;
							}
							if (pFirstTableInfo.get()==NULL)
								pFirstTableInfo = tableInfo;
							pTableInfoList.insert(table->table_name,tableInfo);
							if (table->alias_name != 0)
							{
								pTableInfoList.insert(table->alias_name,tableInfo,false);
							}
						}
						if (pTableInfoList.empty())
						{
							// ? error
							break;
						}
						// WHERES
						std::list<std::list<CFieldCompare::pointer> > topwheres;
						if (GetWheres(pFirstTableInfo, pTableInfoList, sp->wheres, topwheres))
						{
							result = CDbService::deleters(pFirstTableInfo, topwheres);
						}

					}break;
				case SQLCOM_UPDATE:
					{
						if (sp->values == 0) break;

						//tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						//if (table == 0 || table->table_name == 0)
						//	break;
						//CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
						//if (tableInfo.get() == 0)
						//	break;

						CLockMap<tstring,CTableInfo::pointer> pTableInfoList;
						CTableInfo::pointer pFirstTableInfo;
						for (short i=0;i<sp->tables->itemcount; i++)
						{
							tagTable * table = (tagTable*)sp->tables->items[i]->item_handle;
							if (table == 0 || table->table_name == 0)
							{
								pTableInfoList.clear();
								break;
							}
							CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
							if (tableInfo.get() == 0)
							{
								pTableInfoList.clear();
								break;
							}
							if (pFirstTableInfo.get()==NULL)
								pFirstTableInfo = tableInfo;
							pTableInfoList.insert(table->table_name,tableInfo);
							if (table->alias_name != 0)
							{
								pTableInfoList.insert(table->alias_name,tableInfo,false);
							}
						}

						// VALUES
						CRecordLine::pointer recordLine = CRecordLine::create(pFirstTableInfo);	// ???
						if (!GetValues(pFirstTableInfo, sp->values, recordLine))
							break;

						// WHERES
						std::list<std::list<CFieldCompare::pointer> > topwheres;
						if (GetWheres(pFirstTableInfo, pTableInfoList, sp->wheres, topwheres))
						{
							result = CDbService::update(pFirstTableInfo, topwheres, recordLine);
						}
					}break;
				case SQLCOM_INSERT:
					{
						if (sp->values == 0) break;

						tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						if (table == 0 || table->table_name == 0)
							break;
						CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
						if (tableInfo.get() == 0)
							break;

						CRecordLine::pointer recordLine = CRecordLine::create(tableInfo);
						if (!GetValues(tableInfo, sp->values, recordLine))
							break;

						result = CDbService::insert(recordLine) ? 1 : 0;
					}break;
				case SQLCOM_CREATE_GTTABLE:
				case SQLCOM_CREATE_LTTABLE:
					{
						// ???
					}break;


				case SQLCOM_CREATE_TABLE:
					{
						tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						if (table == 0 || table->table_name == 0)
							break;
						CTableInfo::pointer tableInfo = CTableInfo::create(table->table_name);
						if (!CDbService::createTable(tableInfo))
							break;

						for (short i=0; i<sp->fields->itemcount; i++)
						{
							const tagField * field = (tagField*)sp->fields->items[i]->item_handle;
							CFieldInfo::pointer fieldInfo = GetFieldInfo(field);
							if (fieldInfo.get()==NULL)
							{
								breakError = true;
								break;
							}
							if (!CDbService::createField(tableInfo, fieldInfo))
								break;
						}

						if (!breakError)
							result = 0;
					}break;
				case SQLCOM_ALTER_TABLE:
					{
						tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						if (table == 0 || table->table_name == 0)
							break;

						if (sp->items == 0)
							break;

						for (short i=0; i<sp->items->itemcount; i++)
						{
							tagParameter * item = (tagParameter*)sp->items->items[i]->item_handle;

							switch (item->param_type)
							{
							case PARAMETER_RENAME:
								{
									char * newname = (char*)item->parameter;
									if (newname == 0)
										break;

									CDbService::rename(table->table_name, newname);
								}break;
							case PARAMETER_ADDCOLUMN:
								{
									tagField * field = (tagField *)item->parameter;
									CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
									if (tableInfo.get() == 0)
										break;
									CFieldInfo::pointer fieldInfo = GetFieldInfo(field);
									if (fieldInfo.get()==NULL)
									{
										break;
									}
									if (CDbService::createField(tableInfo, fieldInfo))
										result = 0;
								}break;
							case PARAMETER_MODIFYCOLUMN:
								{
									tagField * field = (tagField *)item->parameter;
									CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
									if (tableInfo.get() == 0)
										break;
									CFieldInfo::pointer fieldInfo = GetFieldInfo(field);
									if (fieldInfo.get()==NULL)
									{
										break;
									}
									if (CDbService::modifyField(tableInfo, fieldInfo->name(), fieldInfo))
										result = 0;
								}break;
							case PARAMETER_DROPCOLUMN:
								{
									CTableInfo::pointer tableInfo = CDbService::getTableInfo(table->table_name);
									if (tableInfo.get() == 0)
										break;
									tagField * field = (tagField *)item->parameter;
									if (field == 0 || field->field_name == 0)
										break;
									if (CDbService::dropField(tableInfo, field->field_name))
										result = 0;
									//bool cascade = sp->parameter == (void*)1;
									// ???
								}break;
							case PARAMETER_COLUMNSETDEFAULT:
								{
									tagField * field = (tagField *)item->parameter;
									if (field == 0 || field->default_value == 0)
										break;

									if (CDbService::setdefault(table->table_name, field->field_name, field->default_value))
										result = 0;
								}break;
							case PARAMETER_COLUMNDROPDEFAULT:
								{
									tagField * field = (tagField *)item->parameter;
									if (field == 0 || field->field_name == 0)
										break;

									if (CDbService::dropdefault(table->table_name, field->field_name))
										result = 0;
								}break;

							default:
								break;
							}

						}
						// ?
						/*					if (sp->parameter == 0)
						break;

						switch (sp->parameter->param_type)
						{
						case PARAMETER_RENAME:
						{
						char * newname = (char*)sp->parameter->parameter;
						if (newname == 0)
						break;

						CDbService::rename(table->table_name, newname);
						}break;
						case PARAMETER_ADDCOLUMN:
						{
						}break;
						default:
						break;
						}*/
					}break;
				case SQLCOM_DROP_TABLE:
					{
						tagTable * table = (tagTable*)sp->tables->items[0]->item_handle;
						if (table == 0 || table->table_name == 0)
							break;

						//bool cascade = sp->parameter == (void*)1;
						CDbService::dropTable(table->table_name);
						result = 0;
					}break;
				case SQLCOM_CREATE_DATABASE:
					{
						if (sp->dbname == 0)
							break;
						CDbService::create(sp->dbname);
						result = 0;
					}break;
				case SQLCOM_USE_DATABASE:
					{
						if (sp->dbname == 0)
							break;

						if (CDbService::use(sp->dbname))
						{
							result = 0;
						}
					}break;
				case SQLCOM_DROP_DATABASE:
					{
						if (sp->dbname == 0)
							break;

						CDbService::drop(sp->dbname);
						result = 0;
					}break;
				case SQLCOM_ALTER_DATABASE:
					{
						// 
						if (sp->dbname != 0)
						{
							if (!CDbService::use(sp->dbname))
								break;
						}

						for (short i=0; i<sp->items->itemcount; i++)
						{
							tagParameter * item = (tagParameter*)sp->items->items[i]->item_handle;

							switch (item->param_type)
							{
							case PARAMETER_RENAME:
								{
									char * newname = (char*)item->parameter;
									if (newname == 0)
										break;

									//CDbService::rename(table->table_name, newname);
								}break;
							case PARAMETER_SET:
								{
									char * pParameter = (char*)item->parameter;
									if (pParameter == 0)
										break;
									char * pValue = (char*)item->parameter2;
									if (pValue == 0)
										break;
									if (strcmp(pParameter,"full_memory")==0)
									{
										if (CDbService::setoption(OPTION_FULL_MEMORY_MODE,atoi(pValue)))
											result = 0;
									}
								}break;
							default:
								break;
							}
						}
					}break;
				default:
					break;
				}

				parse_free(sp);
			}

			return result;

		}
	};

	CLockMap<void*, CBodb::pointer> m_bodbs;

	CBodbHandler::pointer bodb_init(const char * path)
	{
		CBodb::pointer bodbService = CBodb::create();
		bodbService->setpath(path);
		if (!bodbService->start())
		{
			bodbService.reset();
			return boNullBodbHandler;
		}
		m_bodbs.insert(bodbService.get(), bodbService);
		return bodbService;
	}

	void bodb_exit(CBodbHandler::pointer & handle)
	{
		CBodb::pointer bodbService;
		if (m_bodbs.find(handle.get(), bodbService, true))
		{
			bodbService->stop();
		}
		bodbService.reset();
	}

	CFieldCompare::FieldCompareType ToCompareType(enum_compare_type type)
	{
		switch (type)
		{
		case COMPARE_EQUAL:
			return CFieldCompare::FCT_EQUAL;
		case COMPARE_UNEQUAL:
			return CFieldCompare::FCT_UNEQUAL;
		case COMPARE_GREATER:
			return CFieldCompare::FCT_GREATER;
		case COMPARE_GREATEREQUAL:
			return CFieldCompare::FCT_GREATEREQUAL;
		case COMPARE_LESS:
			return CFieldCompare::FCT_LESS;
		case COMPARE_LESSEQUAL:
			return CFieldCompare::FCT_LESSEQUAL;
		case COMPARE_ISNULL:
			return CFieldCompare::FCT_ISNULL;
		case COMPARE_ISNOTNULL:
			return CFieldCompare::FCT_ISNOTNULL;
		case COMPARE_BETWEENAND:
			return CFieldCompare::FCT_BETWEENAND;
		case COMPARE_LIKE:
			return CFieldCompare::FCT_LIKE;
		default:
			// ??
			break;
		}
		return CFieldCompare::FCT_UNKNOWN;
	}

	FieldConstraintType ToConstraintType(enum_field_constraint type)
	{
		switch (type)
		{
		case SQL_CONSTRAINT_NOTNULL:
			return FIELD_CONSTRAINT_NOTNULL;
		case SQL_CONSTRAINT_UNIQUE:
			return FIELD_CONSTRAINT_UNIQUE;
		case SQL_CONSTRAINT_PRIMARYKEY:
			return FIELD_CONSTRAINT_PRIMARYKEY;
		default:
			break;
		}
		return FIELD_CONSTRAINT_UNKNOWN;
	}

	bool GetOrderBys(const CTableInfo::pointer& tableInfo, const CLockMap<tstring,CTableInfo::pointer>& pTableInfoList, tagItems * itemOrderBys, std::vector<bo::uinteger> & outTopOrderBys)
	{
		if (itemOrderBys == 0) return true;
		for (short i=0; i<itemOrderBys->itemcount; i++)
		{
			const char * pTableName = (const char*)itemOrderBys->items[i]->table_name;
			const char * pOrderByField = (const char*)itemOrderBys->items[i]->item_handle;

			CTableInfo::pointer pTableInfo = tableInfo;
			if (pTableName!=0 && !pTableInfoList.find(pTableName,pTableInfo))
			{
				return false;
			}
			if (pOrderByField == 0)
			{
				return false;
			}
			CFieldInfo::pointer pFieldInfo = pTableInfo->getFieldInfo(pOrderByField);
			if (pFieldInfo.get()==0)
				return false;
			outTopOrderBys.push_back(pFieldInfo->id());
		}
		return true;
	}
	bool GetWheres(const CTableInfo::pointer& tableInfo, const CLockMap<tstring,CTableInfo::pointer>& pTableInfoList, tagItems * itemWheres, std::list<std::list<CFieldCompare::pointer> > & outTopWheres)
	{
		if (itemWheres == 0) return true;
		std::list<CFieldCompare::pointer> wheres;
		for (short i=0; i<itemWheres->itemcount; i++)
		{
			const tagWhere * pWhere = (tagWhere*)itemWheres->items[i]->item_handle;

			CTableInfo::pointer pTableInfo = tableInfo;
			if (pWhere->table_name!=0 && !pTableInfoList.find(pWhere->table_name,pTableInfo))
			{
				return false;
			}

			CFieldInfo::pointer fieldInfo;
			if (pWhere->field_name != 0)
			{
				fieldInfo = pTableInfo->getFieldInfo(pWhere->field_name);
			}else
			{
				//fieldInfo = tableInfo->getFieldInfo(i);
			}

			if (fieldInfo.get() == 0)
			{
				return false;
			}
			CFieldVariant::pointer variant = CFieldVariant::create(fieldInfo->type());
			switch (pWhere->compare_type)
			{
			case COMPARE_EQUAL:
			case COMPARE_UNEQUAL:
			case COMPARE_GREATER:
			case COMPARE_GREATEREQUAL:
			case COMPARE_LESS:
			case COMPARE_LESSEQUAL:
			case COMPARE_LIKE:
				{
					tagItemValue * itemValue = (tagItemValue*)pWhere->value_handle;
					if (itemValue == 0)
					{
						return false;
					}
					CFieldCompare::pointer pFieldCompare;
					if (itemValue->table_name != 0 && itemValue->field_name != 0)
					{
						CTableInfo::pointer pTableInfo2;
						if (!pTableInfoList.find(itemValue->table_name,pTableInfo2))
						{
							return false;
						}
						CFieldInfo::pointer fieldInfo2 = pTableInfo2->getFieldInfo(itemValue->field_name);
						if (fieldInfo2.get() == 0)
							return false;
						pFieldCompare = CFieldCompare::create(pTableInfo,pTableInfo2,ToCompareType(pWhere->compare_type),fieldInfo,fieldInfo2,pWhere->where_level);
					}else if (itemValue->table_name != 0 || itemValue->field_name != 0)
					{
						return false;
					}else if (ItemValue2Variant(itemValue, variant))
					{
						pFieldCompare = CFieldCompare::create(pTableInfo,ToCompareType(pWhere->compare_type),fieldInfo,variant,pWhere->where_level);
					}else
					{
						return false;
					}
					pFieldCompare->compareAnd(pWhere->and_where);
					wheres.push_back(pFieldCompare);
				}break;
			case COMPARE_ISNULL:
			case COMPARE_ISNOTNULL:
				{
					CFieldCompare::pointer pFieldCompare = CFieldCompare::create(pTableInfo,ToCompareType(pWhere->compare_type),fieldInfo,pWhere->where_level);
					pFieldCompare->compareAnd(pWhere->and_where);
					wheres.push_back(pFieldCompare);
				}break;
			default:
				break;
			}
		}

		outTopWheres.push_back(wheres);
		return true;
	}

	bool GetValues(CTableInfo::pointer tableInfo, tagItems * itemValues, CRecordLine::pointer outRecordLine)
	{
		for (short i=0; i<itemValues->itemcount; i++)
		{
			tagValue * value = (tagValue*)itemValues->items[i]->item_handle;

			if (value->field_name != 0)
			{
				switch (value->value.value_type)
				{
				case VALUE_STRING:
					{
						int size = strlen(value->value.u.value_string);
						if (!outRecordLine->addVariant(value->field_name, value->value.u.value_string, size))
						{
							return false;
						}
					}break;
				case VALUE_INT:
					{
						if (!outRecordLine->addVariantInt(value->field_name, value->value.u.value_int))
						{
							return false;
						}
					}break;
				case VALUE_BIGINT:
					{
						if (!outRecordLine->addVariantBigInt(value->field_name, value->value.u.value_bigint))
						{
							return false;
						}
					}break;
				case VALUE_REAL:
					{
						if (!outRecordLine->addVariantReal(value->field_name, value->value.u.value_real))
						{
							return false;
						}
					}break; 
				case VALUE_BOOL:
					{
						if (!outRecordLine->addVariant(value->field_name, value->value.u.value_bool))
						{
							return false;
						}
					}break; 
				case VALUE_NULL:
					{
						if (!outRecordLine->addVariantNull(value->field_name))
						{
							return false;
						}
					}break;
				case VALUE_DEFAULT:
					{
						if (!outRecordLine->addVariantDefault(value->field_name))
						{
							return false;
						}
					}
				case VALUE_FUNC_CURRENT_TIMESTAMP:
					{
						if (!outRecordLine->addVariantCurrentTime(value->field_name))
						{
							return false;
						}
					}break;
				default:
					break;
				}
			}else
			{
				switch (value->value.value_type)
				{
				case VALUE_STRING:
					{
						int size = strlen(value->value.u.value_string);
						if (!outRecordLine->addVariant(i, value->value.u.value_string, size))
						{
							return false;
						}
					}break;
				case VALUE_INT:
					{
						if (!outRecordLine->addVariantInt(i, value->value.u.value_int))
						{
							return false;
						}
					}break;
				case VALUE_BIGINT:
					{
						if (!outRecordLine->addVariantBigInt(i, value->value.u.value_bigint))
						{
							return false;
						}
					}break;
				case VALUE_REAL:
					{
						if (!outRecordLine->addVariantReal(i, value->value.u.value_real))
						{
							return false;
						}
					}break;
				case VALUE_BOOL:
					{
						if (!outRecordLine->addVariant(i, value->value.u.value_bool))
						{
							return false;
						}
					}break;
				case VALUE_NULL:
					{
						if (!outRecordLine->addVariantNull(i))
						{
							return false;
						}
					}break;
				case VALUE_DEFAULT:
					{
						if (!outRecordLine->addVariantDefault(i))
						{
							return false;
						}
					}
				case VALUE_FUNC_CURRENT_TIMESTAMP:
					{
						if (!outRecordLine->addVariantCurrentTime(i))
						{
							return false;
						}
					}break;
				default:
					break;
				}
			}
		}
		return true;
	}

	void bodb_free(PRESULTSET resultset)
	{
		if (resultset != 0)
		{
			for (int i=0; i<resultset->rscount; i++)
			{
				for (short j=0; j<resultset->rsvalues[i]->fieldcount; j++)
				{
					FreeFieldVariant(resultset->rsvalues[i]->fieldvalues[j], true);
				}
				if (resultset->rsvalues[i]->fieldvalues != 0)
				{
					delete[] resultset->rsvalues[i]->fieldvalues;
				}
				delete resultset->rsvalues[i];		// add by yhz 2011-08-29
			}

			if (resultset->rsvalues != 0)
			{
				delete[] resultset->rsvalues;
			}
			delete resultset;
		}
	}

	const std::string & replace(std::string & strSource, const std::string & strFind, const std::string &strReplace)
	{
		tstring::size_type pos=0;
		tstring::size_type findlen=strFind.size();
		tstring::size_type replacelen=strReplace.size();
		while ((pos=strSource.find(strFind, pos)) != tstring::npos)
		{
			strSource.replace(pos, findlen, strReplace);
			pos += replacelen;
		}
		return strSource;
	}

	const int escape_size = 1;
	const std::string escape_in[] = {"&lsquo;"};
	const std::string escape_out[] = {"'"};

	void bodb_escape_string_in(std::string & sql)
	{
		for (int i=0; i<escape_size; i++)
			replace(sql, escape_out[i], escape_in[i]);
	}
	void bodb_escape_string_out(std::string & sql)
	{
		for (int i=0; i<escape_size; i++)
			replace(sql, escape_in[i], escape_out[i]);
	}

}	// namespace

