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

#include "recordline.h"

namespace bo
{
	CRecordLine::pointer CRecordLine::create(const CTableInfo::pointer& tableInfo)
	{
		BOOST_ASSERT (tableInfo.get() != 0);
		return CRecordLine::pointer(new CRecordLine(tableInfo->getNextRecordId(), tableInfo));
	}
	CRecordLine::pointer CRecordLine::create(uinteger id, const CTableInfo::pointer& tableInfo)
	{
		return CRecordLine::pointer(new CRecordLine(id, tableInfo));
	}

	CFieldVariant::pointer CRecordLine::getVariant(uinteger fieldId) const
	{
		CFieldVariant::pointer variant;
		m_variants.find(fieldId, variant);
		return variant;
	}
	CFieldVariant::pointer CRecordLine::getVariant(const tstring & fieldName) const
	{
		CFieldVariant::pointer variant;
		m_variants2.find(fieldName, variant);
		return variant;
	}
	bool CRecordLine::equalFieldVariant(uinteger fieldId, const CFieldVariant::pointer& compare) const
	{
		CFieldVariant::pointer variant;
		if (!m_variants.find(fieldId, variant))
			return false;
		return variant->equal(compare);
	}
	bool CRecordLine::doCompareFieldVariant(uinteger fieldId, CFieldCompare::FieldCompareType compareType, const CFieldVariant::pointer& compare) const
	{
		CFieldVariant::pointer variant;
		if (!m_variants.find(fieldId, variant))
			return false;
		bool result = false;
		switch (compareType)
		{
		case CFieldCompare::FCT_EQUAL:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->equal(compare);
			break;
		case CFieldCompare::FCT_UNEQUAL:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->unequal(compare);
			break;
		case CFieldCompare::FCT_GREATER:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->greater(compare);
			break;
		case CFieldCompare::FCT_GREATEREQUAL:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->greaterEqual(compare);
			break;
		case CFieldCompare::FCT_LESS:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->less(compare);
			break;
		case CFieldCompare::FCT_LESSEQUAL:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->lessEqual(compare);
			break;
		case CFieldCompare::FCT_ISNULL:
			result = variant->isNull();
			break;
		case CFieldCompare::FCT_ISNOTNULL:
			result = variant->isNotNull();
			break;
		case CFieldCompare::FCT_LIKE:
			BOOST_ASSERT (compare.get() != 0);
			result = variant->isLike(compare);
			break;
		default:
			break;
		}

		return result;
	}

	void CRecordLine::addVariant(const CFieldInfo::pointer& fieldInfo, const CFieldVariant::pointer& variant, bool bAddForce)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);
		BOOST_ASSERT (variant.get() != 0);
		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant, bAddForce);
		m_variants2.insert(fieldInfo->name(), variant, bAddForce);
		//m_curiter = m_variants.end();	// restart for m_curiter
	}
	void CRecordLine::addVariant(bo::uinteger fieldId, const tstring& fieldName, const CFieldVariant::pointer& variant, bool bAddForce)
	{
		BOOST_ASSERT (variant.get() != 0);
		//BoostWriteLock wtlock(m_mutex);
		m_variants.insert(fieldId, variant, bAddForce);
		if (!fieldName.empty())
			m_variants2.insert(fieldName, variant, bAddForce);
		//m_curiter = m_variants.end();	// restart for m_curiter
	}

	void CRecordLine::delVariant(const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);
		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.remove(fieldInfo->id());
		m_variants2.remove(fieldInfo->name());
		//m_curiter = m_variants.end();	// restart for m_curiter
	}

	bool CRecordLine::addVariant(const tstring & fieldName, bool value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setBool(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantInt(const tstring & fieldName, int value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setInt(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantReal(const tstring & fieldName, double value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setReal(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantUInt(const tstring & fieldName, uinteger value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setIntu(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantBigInt(const tstring & fieldName, bigint value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setBigInt(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}

	bool CRecordLine::addVariant(const tstring & fieldName, const char * value, uinteger size)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (size > fieldInfo->size1())
		{
			size = fieldInfo->size1();
		}
		if (!variant->setString(value, size))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}

	bool CRecordLine::addVariantNull(const tstring & fieldName)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		variant->setNull();

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantDefault(const tstring & fieldName)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->defaultValue();
		if (variant.get() == 0)
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantCurrentTime(const tstring & fieldName)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldName);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setCurrentTime())
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariant(short fieldIndex, const char * value, uinteger size)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (size > fieldInfo->size1())
		{
			size = fieldInfo->size1();
		}
		if (!variant->setString(value, size))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariant(short fieldIndex, bool value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setBool(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantInt(short fieldIndex, int value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setInt(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantReal(short fieldIndex, double value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setReal(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantBigInt(short fieldIndex, bigint value)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setBigInt(value))
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}

	bool CRecordLine::addVariantNull(short fieldIndex)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		variant->setNull();

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantDefault(short fieldIndex)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->defaultValue();
		if (variant.get() == 0)
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantCurrentTime(short fieldIndex)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo2(fieldIndex);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->buildFieldVariant();
		if (!variant->setCurrentTime())
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::addVariantDefault2(uinteger fieldId)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldId);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer variant = fieldInfo->defaultValue();
		if (variant.get() == 0)
			return false;

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.insert(fieldInfo->id(), variant);
		m_variants2.insert(fieldInfo->name(), variant);
		//m_curiter = m_variants.end();	// restart for m_curiter
		return true;
	}
	bool CRecordLine::setNullDefaultVariant(uinteger fieldId)
	{
		CFieldInfo::pointer fieldInfo = m_tableInfo->getFieldInfo(fieldId);
		if (fieldInfo.get() == 0 || fieldInfo->autoincrement())
			return false;

		CFieldVariant::pointer defaultVariant = fieldInfo->defaultValue();
		if (defaultVariant.get() == 0)
			return false;

		CFieldVariant::pointer variant;
		if (m_variants.find(fieldId, variant))
		{
			if (variant->isNull())
			{
				variant = defaultVariant;
				return true;
			}
		}else
		{
			//BoostWriteLock wtlock(m_mutex);
			//boost::mutex::scoped_lock lock(m_mutex);
			m_variants.insert(fieldInfo->id(), defaultVariant);
			m_variants2.insert(fieldInfo->name(), defaultVariant);
			//m_curiter = m_variants.end();	// restart for m_curiter
			return true;
		}
		return false;
	}
	void CRecordLine::updateVariant(const CFieldInfo::pointer& fieldInfo, const CFieldVariant::pointer& fieldVariant)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);
		BOOST_ASSERT (fieldVariant.get() != 0);

		//BoostWriteLock wtlock(m_mutex);
		//boost::mutex::scoped_lock lock(m_mutex);
		m_variants.remove(fieldInfo->id());
		m_variants.insert(fieldInfo->id(), fieldVariant);
		m_variants2.remove(fieldInfo->name());
		m_variants2.insert(fieldInfo->name(), fieldVariant);
		//m_curiter = m_variants.end();	// restart for m_curiter
	}
	void CRecordLine::setVariantNull(const CFieldInfo::pointer& fieldInfo)
	{
		BOOST_ASSERT (fieldInfo.get() != 0);

		CFieldVariant::pointer variant;
		if (m_variants.find(fieldInfo->id(), variant))
		{
			variant->setNull();
		}
	}

	CRecordLine::pointer CRecordLine::Clone(bo::uinteger recordId) const
	{
		CRecordLine::pointer pResult = CRecordLine::create(recordId, m_tableInfo);
		AUTO_CONST_RLOCK(m_variants);
		CLockMap<uinteger, CFieldVariant::pointer>::const_iterator pIter = m_variants.begin();
		for (; pIter!=m_variants.end(); pIter++)
		{
			const uinteger fieldId = pIter->first;;
			CFieldVariant::pointer fieldVariant = pIter->second;
			pResult->addVariant(fieldId,"",fieldVariant);
		}
		//CRecordLine * pOwner = const_cast<CRecordLine*>(this);
		//CFieldVariant::pointer fieldVariant = pOwner->moveFirst();
		//while (fieldVariant.get() != 0)
		//{
		//	//CFieldInfo::pointer pFieldInfo = getFieldInfo();
		//	//if (pFieldInfo.get()!=0)
		//	//	pResult->addVariant(pFieldInfo,fieldVariant);
		//	//else
		//		pResult->addVariant(this->getFieldId(),"",fieldVariant);
		//	fieldVariant = pOwner->moveNext();
		//}
		return pResult;
	}
	void CRecordLine::AddRecordLine(const CRecordLine::pointer& pRecordLine, bool bAddForce)
	{
		const CLockMap<uinteger, CFieldVariant::pointer>& pVariantList = pRecordLine->GetVariantList();
		AUTO_CONST_RLOCK(pVariantList);
		CLockMap<uinteger, CFieldVariant::pointer>::const_iterator pIter = pVariantList.begin();
		for (; pIter!=pVariantList.end(); pIter++)
		{
			const uinteger fieldId = pIter->first;;
			CFieldVariant::pointer fieldVariant = pIter->second;
			addVariant(fieldId,"",fieldVariant, bAddForce);
		}

		//CFieldVariant::pointer fieldVariant = pRecordLine->moveFirst();
		//while (fieldVariant.get() != 0)
		//{
		//	//CFieldInfo::pointer pFieldInfo = pRecordLine->getFieldInfo();
		//	//if (pFieldInfo.get()!=0)
		//	//	addVariant(pFieldInfo,fieldVariant);
		//	//else
		//		addVariant(pRecordLine->getFieldId(),"", fieldVariant);
		//	fieldVariant = pRecordLine->moveNext();
		//}
	}

	//CFieldVariant::pointer CRecordLine::moveFirst(void)
	//{
	//	BoostReadLock rdlock(m_mutex);
	//	//boost::mutex::scoped_lock lock(m_mutex);
	//	m_curiter = m_variants.begin();
	//	if (m_curiter == m_variants.end())
	//		return boNullFieldVariant;

	//	return m_curiter->second;
	//}
	//CFieldVariant::pointer CRecordLine::moveNext(void)
	//{
	//	BoostReadLock rdlock(m_mutex);
	//	//boost::mutex::scoped_lock lock(m_mutex);
	//	if (m_curiter == m_variants.end() || (++m_curiter) == m_variants.end())
	//		return boNullFieldVariant;

	//	return m_curiter->second;
	//}
	//CFieldVariant::pointer CRecordLine::movePrev(void)
	//{
	//	BoostReadLock rdlock(m_mutex);
	//	//boost::mutex::scoped_lock lock(m_mutex);
	//	if (m_curiter == m_variants.begin() || (--m_curiter) == m_variants.begin())
	//		return boNullFieldVariant;

	//	return m_curiter->second;
	//}
	//CFieldVariant::pointer CRecordLine::moveLast(void)
	//{
	//	BoostReadLock rdlock(m_mutex);
	//	//boost::mutex::scoped_lock lock(m_mutex);
	//	m_curiter = m_variants.end();
	//	if (m_curiter == m_variants.begin())
	//		return boNullFieldVariant;

	//	m_curiter--;
	//	return m_curiter->second;
	//}
	//uinteger CRecordLine::getFieldId(void) const
	//{
	//	uinteger result = 0;
	//	if (m_curiter != m_variants.end())
	//	{
	//		result = m_curiter->first;
	//	}
	//	return result;
	//}
	//CFieldInfo::pointer CRecordLine::getFieldInfo(void) const
	//{
	//	return m_tableInfo->getFieldInfo(getFieldId());
	//}

	uinteger CRecordLine::size(void) const
	{
		return (uinteger)m_variants.size();
	}

	CRecordLine::CRecordLine(uinteger id, const CTableInfo::pointer& tableInfo)
		: m_id(id), m_tableInfo(tableInfo)
		//, m_nExtData(0)
	{
		BOOST_ASSERT (m_tableInfo.get() != 0);
		//m_curiter = m_variants.begin();
	}
	CRecordLine::~CRecordLine(void)
	{
		m_variants.clear();
		m_variants2.clear();
	}
	usmallint CRecordLine::getLineSize(void) const
	{
		usmallint result = 0;
		result += INTEGER_SIZE;		// m_id

		// ? lock
		const CLockList<CFieldInfo::pointer> & fields = m_tableInfo->fields();
		CLockList<CFieldInfo::pointer>::const_iterator iter;
		for (iter=fields.begin(); iter!=fields.end(); iter++)
		{
			result += (*iter)->linedatasize();
			//switch ((*iter)->type())
			//{
			////case FT_CHAR:
			//case FT_VARCHAR:
			//case FT_NCHAR:
			//case FT_NVARCHAR:
			//case FT_BINARY:
			//case FT_VARBINARY:
			//case bo::FT_CLOB:
			//case bo::FT_NCLOB:
			//case bo::FT_BLOB:
			//	result += INTEGER_SIZE;	// add b yhz
			//	break;
			//default:
			//	result += (*iter)->datasize();
			//	break;
			//}
		}
		return result;
	}

	void CRecordLine::Serialize(bool isStoring, tfstream& ar, usmallint & outDoSize)
	{
		const CLockList<CFieldInfo::pointer> & fields = m_tableInfo->fields();
		CLockList<CFieldInfo::pointer>::const_iterator iter;

		ar.clear();
		//uinteger len = 0;
		//uinteger realsize = 0;
		if (isStoring)
		{
			// m_id
			ar.write((const char*)&m_id, INTEGER_SIZE);
			outDoSize += INTEGER_SIZE;

			for (iter=fields.begin(); iter!=fields.end(); iter++)
			{
				//uinteger tellp = ar.tellp();
				CFieldVariant::pointer variant;
				m_variants.find((*iter)->id(), variant);

				if (variant.get() != 0)
				{
					uinteger doSize = 0;
					variant->Serialize(isStoring, ar, (*iter)->size1(), false, doSize);
				}else
				{
					ar.seekp((*iter)->linedatasize(), std::ios::cur);
				}
				outDoSize += (*iter)->linedatasize();

				//switch ((*iter)->type())
				//{
				//case bo::FT_VARCHAR:
				//case bo::FT_CLOB:
				//case bo::FT_NVARCHAR:
				//case bo::FT_NCLOB:
				//case bo::FT_VARBINARY:
				//case bo::FT_BLOB:
				//	{
				//		outDoSize += INTEGER_SIZE;
				//		uinteger doSize = 0;
				//		if (variant.get() != 0)
				//		{
				//			variant->Serialize(isStoring, ar, (*iter)->size1(), false, doSize);
				//		}else
				//		{
				//			ar.seekp(INTEGER_SIZE, std::ios::cur);
				//		}
				//	}break;
				//default:
				//	{
				//		outDoSize += (*iter)->datasize();
				//		uinteger doSize = 0;
				//		if (variant.get() != 0)
				//		{
				//			variant->Serialize(isStoring, ar, (*iter)->size1(), false, doSize);
				//		}else
				//		{
				//			ar.seekp((*iter)->datasize(), std::ios::cur);
				//		}
				//	}break;
				//}

			}
		}else
		{
			// m_id
			ar.read((char*)&m_id, sizeof(m_id));
			outDoSize += INTEGER_SIZE;

			for (iter=fields.begin(); iter!=fields.end(); iter++)
			{
				//uinteger tellg = ar.tellg();
				CFieldVariant::pointer variant = CFieldVariant::create((*iter)->type());

				outDoSize += (*iter)->linedatasize();
				//switch ((*iter)->type())
				//{
				//case bo::FT_VARCHAR:
				//case bo::FT_CLOB:
				//case bo::FT_NVARCHAR:
				//case bo::FT_NCLOB:
				//case bo::FT_VARBINARY:
				//case bo::FT_BLOB:
				//	{
				//		outDoSize += INTEGER_SIZE;
				//	}break;
				//default:
				//	{
				//		outDoSize += (*iter)->datasize();
				//	}break;
				//}
				uinteger doSize = 0;
				variant->Serialize(isStoring, ar, (*iter)->size1(), false, doSize);
				this->addVariant((*iter), variant);

			}
		}

	}

} // namespace bo
