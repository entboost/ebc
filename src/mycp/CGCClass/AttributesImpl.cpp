/*
    MYCP is a HTTP and C++ Web Application Server.
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

#include "AttributesImpl.h"

namespace mycp {

AttributesImpl::AttributesImpl(void)
{
}

AttributesImpl::~AttributesImpl(void)
{
	clearAllAtrributes();
	cleanAllPropertys();
}
void AttributesImpl::setProperty(const tstring& key, const cgcValueInfo::pointer& value, bool clear)
{
	m_strPropertys.insert(key, value, clear);
}

void AttributesImpl::setProperty(int key, const cgcValueInfo::pointer& value, bool clear)
{
	m_intPropertys.insert(key, value, clear);
}
void AttributesImpl::setProperty(bigint key, const cgcValueInfo::pointer& value, bool clear)
{
	m_bigintPropertys.insert(key, value, clear);
}
void AttributesImpl::setProperty(void* key, const cgcValueInfo::pointer& value, bool clear)
{
	m_pointerPropertys.insert(key, value, clear);
}

cgcValueInfo::pointer AttributesImpl::getProperty(const tstring& key) const
{
	cgcValueInfo::pointer result;
	return m_strPropertys.find(key, result) ? result : cgcNullValueInfo;
}
cgcValueInfo::pointer AttributesImpl::getProperty(const tstring& key,bool erase)
{
	cgcValueInfo::pointer result;
	return m_strPropertys.find(key, result,erase) ? result : cgcNullValueInfo;
}

cgcValueInfo::pointer AttributesImpl::getProperty(int key) const
{
	cgcValueInfo::pointer result;
	return m_intPropertys.find(key, result) ? result : cgcNullValueInfo;
}
cgcValueInfo::pointer AttributesImpl::getProperty(int key,bool erase)
{
	cgcValueInfo::pointer result;
	return m_intPropertys.find(key, result,erase) ? result : cgcNullValueInfo;
}

cgcValueInfo::pointer AttributesImpl::getProperty(bigint key) const
{
	cgcValueInfo::pointer result;
	return m_bigintPropertys.find(key, result) ? result : cgcNullValueInfo;
}
cgcValueInfo::pointer AttributesImpl::getProperty(bigint key,bool erase)
{
	cgcValueInfo::pointer result;
	return m_bigintPropertys.find(key, result,erase) ? result : cgcNullValueInfo;
}

cgcValueInfo::pointer AttributesImpl::getProperty(void* key) const
{
	cgcValueInfo::pointer result;
	return m_pointerPropertys.find(key, result) ? result : cgcNullValueInfo;
}
cgcValueInfo::pointer AttributesImpl::getProperty(void* key,bool erase)
{
	cgcValueInfo::pointer result;
	return m_pointerPropertys.find(key, result,erase) ? result : cgcNullValueInfo;
}

bool AttributesImpl::getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues) const
{
	return m_strPropertys.find(key, outValues);
}
bool AttributesImpl::getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues,bool erase)
{
	return m_strPropertys.find(key, outValues,erase);
}

bool AttributesImpl::getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues) const
{
	return m_intPropertys.find(key, outValues);
}
bool AttributesImpl::getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues,bool erase)
{
	return m_intPropertys.find(key, outValues,erase);
}

bool AttributesImpl::getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues) const
{
	return m_bigintPropertys.find(key, outValues);
}
bool AttributesImpl::getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues,bool erase)
{
	return m_bigintPropertys.find(key, outValues,erase);
}

bool AttributesImpl::getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues) const
{
	return m_pointerPropertys.find(key, outValues);
}
bool AttributesImpl::getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues,bool erase)
{
	return m_pointerPropertys.find(key, outValues,erase);
}

bool AttributesImpl::getSPropertys(std::vector<cgcKeyValue::pointer>& outValues) const
{
	AUTO_CONST_RLOCK(m_strPropertys);
	CLockMultiMap<tstring, cgcValueInfo::pointer>::const_iterator iter;
	for (iter=m_strPropertys.begin(); iter!=m_strPropertys.end(); iter++)
	{
		 outValues.push_back(CGC_KEYVALUE(iter->first, iter->second));
	}

	return !m_strPropertys.empty();
}

bool AttributesImpl::getIPropertys(std::vector<cgcKeyValue::pointer>& outValues) const
{
	AUTO_CONST_RLOCK(m_intPropertys);
	CLockMultiMap<int, cgcValueInfo::pointer>::const_iterator iter;
	for (iter=m_intPropertys.begin(); iter!=m_intPropertys.end(); iter++)
	{
		char buffer[12];
		sprintf(buffer, "%d", iter->first);
		 outValues.push_back(CGC_KEYVALUE(buffer, iter->second));
	}
	return !m_intPropertys.empty();
}
bool AttributesImpl::getBPropertys(std::vector<cgcKeyValue::pointer>& outValues) const
{
	AUTO_CONST_RLOCK(m_bigintPropertys);
	CLockMultiMap<bigint, cgcValueInfo::pointer>::const_iterator iter;
	for (iter=m_bigintPropertys.begin(); iter!=m_bigintPropertys.end(); iter++)
	{
		char buffer[24];
#ifdef WIN32
		sprintf(buffer, "%I64d", iter->first);
#else
		sprintf(buffer, "%lld", iter->first);
#endif
		 outValues.push_back(CGC_KEYVALUE(buffer, iter->second));
	}
	return !m_bigintPropertys.empty();
}

bool AttributesImpl::getPPropertys(std::vector<cgcKeyValue::pointer>& outValues) const
{
	AUTO_CONST_RLOCK(m_pointerPropertys);
	CLockMultiMap<void*, cgcValueInfo::pointer>::const_iterator iter;
	for (iter=m_pointerPropertys.begin(); iter!=m_pointerPropertys.end(); iter++)
	{
		char buffer[24];
		sprintf(buffer, "%p", iter->first);
		 outValues.push_back(CGC_KEYVALUE(buffer, iter->second));
	}
	return !m_pointerPropertys.empty();
}

size_t AttributesImpl::getPropertySize(const tstring& key) const
{
	return m_strPropertys.sizek(key);
}

size_t AttributesImpl::getPropertySize(int key) const
{
	return m_intPropertys.sizek(key);
}
size_t AttributesImpl::getPropertySize(bigint key) const
{
	return m_bigintPropertys.sizek(key);
}

size_t AttributesImpl::getPropertySize(void* key) const
{
	return m_pointerPropertys.sizek(key);
}

void AttributesImpl::delProperty(const tstring& key)
{
	m_strPropertys.remove(key);
}

void AttributesImpl::delProperty(int key)
{
	m_intPropertys.remove(key);
}
void AttributesImpl::delProperty(bigint key)
{
	m_bigintPropertys.remove(key);
}
void AttributesImpl::delProperty(void* key)
{
	m_pointerPropertys.remove(key);
}

void AttributesImpl::cleanSPropertys(void)
{
	m_strPropertys.clear();
}

void AttributesImpl::cleanIPropertys(void)
{
	m_intPropertys.clear();
}
void AttributesImpl::cleanBPropertys(void)
{
	m_bigintPropertys.clear();
}

void AttributesImpl::cleanPPropertys(void)
{
	m_pointerPropertys.clear();
}

void AttributesImpl::cleanAllPropertys(void)
{
	m_strPropertys.clear();
	m_intPropertys.clear();
	m_bigintPropertys.clear();
	m_pointerPropertys.clear();
}

StringObjectMapPointer AttributesImpl::getStringAttributes(int attributeName, bool newIfNotExist)
{
	StringObjectMapPointer result;
	if (!m_mapIntStringPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = StringObjectMapPointer(new CObjectMap<tstring>); 
			StringObjectMapPointer pTemp;
			m_mapIntStringPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
		}
	}

	return result;
}

LongObjectMapPointer AttributesImpl::getLongAttributes(int attributeName, bool newIfNotExist)
{
	LongObjectMapPointer result;
	if (!m_mapIntLongPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = LongObjectMapPointer(new CObjectMap<int>); 
			//m_mapIntLongPointer.insert(attributeName, result);
			LongObjectMapPointer pTemp;
			m_mapIntLongPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
		}
	}

	return result;
}

BigIntObjectMapPointer AttributesImpl::getBigIntAttributes(int attributeName, bool newIfNotExist)
{
	BigIntObjectMapPointer result;
	if (!m_mapIntBigIntPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = BigIntObjectMapPointer(new CObjectMap<bigint>); 
			BigIntObjectMapPointer pTemp;
			m_mapIntBigIntPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
			//m_mapIntBigIntPointer.insert(attributeName, result);
		}
	}

	return result;
}

VoidObjectMapPointer AttributesImpl::getVoidAttributes(int attributeName, bool newIfNotExist)
{
	VoidObjectMapPointer result;
	if (!m_mapIntVoidPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = VoidObjectMapPointer(new CObjectMap<void*>); 
			VoidObjectMapPointer pTemp;
			m_mapIntVoidPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
			//m_mapIntVoidPointer.insert(attributeName, result);
		}
	}

	return result;
}

StringObjectMapPointer AttributesImpl::getStringAttributes(const tstring & attributeName, bool newIfNotExist)
{
	StringObjectMapPointer result;
	if (!m_mapStrStringPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = StringObjectMapPointer(new CObjectMap<tstring>); 
			StringObjectMapPointer pTemp;
			m_mapStrStringPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
			//m_mapStrStringPointer.insert(attributeName, result);
		}
	}

	return result;
}

LongObjectMapPointer AttributesImpl::getLongAttributes(const tstring & attributeName, bool newIfNotExist)
{
	LongObjectMapPointer result;
	if (!m_mapStrLongPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = LongObjectMapPointer(new CObjectMap<int>); 
			LongObjectMapPointer pTemp;
			m_mapStrLongPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
			//m_mapStrLongPointer.insert(attributeName, result);
		}
	}

	return result;
}

VoidObjectMapPointer AttributesImpl::getVoidAttributes(const tstring & attributeName, bool newIfNotExist)
{
	VoidObjectMapPointer result;
	if (!m_mapStrVoidPointer.find(attributeName, result))
	{
		if (newIfNotExist)
		{
			result = VoidObjectMapPointer(new CObjectMap<void*>); 
			VoidObjectMapPointer pTemp;
			m_mapStrVoidPointer.insert(attributeName,result,false,&pTemp);
			if (pTemp.get()!=0)
				result = pTemp;
			//m_mapStrVoidPointer.insert(attributeName, result);
		}
	}

	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(int attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer = getStringAttributes(attributeName, true);
	BOOST_ASSERT (stringMapPointer.get() != NULL);

	stringMapPointer->insert(key, pObject,force,&result);
	//stringMapPointer->find(key, result, true);
	//stringMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(int attributeName, int key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer = getLongAttributes(attributeName, true);
	BOOST_ASSERT (longMapPointer.get() != NULL);

	longMapPointer->insert(key, pObject,force,&result);
	//longMapPointer->find(key, result, true);
	//longMapPointer->insert(key, pObject);
	return result;
}
cgcObject::pointer AttributesImpl::setAttribute(int attributeName, bigint key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	BigIntObjectMapPointer longMapPointer = getBigIntAttributes(attributeName, true);
	BOOST_ASSERT (longMapPointer.get() != NULL);

	longMapPointer->insert(key, pObject,force,&result);
	//longMapPointer->find(key, result, true);
	//longMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(int attributeName, void* key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer = getVoidAttributes(attributeName, true);
	BOOST_ASSERT (voidMapPointer.get() != NULL);

	voidMapPointer->insert(key, pObject,force,&result);
	//voidMapPointer->find(key, result, true);
	//voidMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(const tstring & attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer = getStringAttributes(attributeName, true);
	BOOST_ASSERT (stringMapPointer.get() != NULL);

	stringMapPointer->insert(key, pObject,force,&result);
	//stringMapPointer->find(key, result, true);
	//stringMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(const tstring & attributeName, int key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer = getLongAttributes(attributeName, true);
	BOOST_ASSERT (longMapPointer.get() != NULL);

	longMapPointer->insert(key, pObject,force,&result);
	//longMapPointer->find(key, result, true);
	//longMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::setAttribute(const tstring & attributeName, void* key, const cgcObject::pointer& pObject,bool force)
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer = getVoidAttributes(attributeName, true);
	BOOST_ASSERT (voidMapPointer.get() != NULL);

	voidMapPointer->insert(key, pObject,force,&result);
	//voidMapPointer->find(key, result, true);
	//voidMapPointer->insert(key, pObject);
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(int attributeName, const tstring & key) const
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer;
	if (m_mapIntStringPointer.find(attributeName, stringMapPointer))
	{
		stringMapPointer->find(key, result);
	}
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(int attributeName, int key) const
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer;
	if (m_mapIntLongPointer.find(attributeName, longMapPointer))
	{
		longMapPointer->find(key, result);
	}
	return result;
}
cgcObject::pointer AttributesImpl::getAttribute(int attributeName, bigint key) const
{
	cgcObject::pointer result;
	BigIntObjectMapPointer longMapPointer;
	if (m_mapIntBigIntPointer.find(attributeName, longMapPointer))
	{
		longMapPointer->find(key, result);
	}
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(int attributeName, void* key) const
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer;
	if (m_mapIntVoidPointer.find(attributeName, voidMapPointer))
	{
		voidMapPointer->find(key, result);
	}
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(const tstring & attributeName, const tstring & key) const
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer;
	if (m_mapStrStringPointer.find(attributeName, stringMapPointer))
	{
		stringMapPointer->find(key, result);
	}
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(const tstring & attributeName, int key) const
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer;
	if (m_mapStrLongPointer.find(attributeName, longMapPointer))
	{
		longMapPointer->find(key, result);
	}
	return result;
}

cgcObject::pointer AttributesImpl::getAttribute(const tstring & attributeName, void* key) const
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer;
	if (m_mapStrVoidPointer.find(attributeName, voidMapPointer))
	{
		voidMapPointer->find(key, result);
	}
	return result;
}

bool AttributesImpl::existAttribute(int attributeName, const tstring & key) const
{
	StringObjectMapPointer stringMapPointer;
	if (m_mapIntStringPointer.find(attributeName, stringMapPointer))
	{
		return stringMapPointer->exist(key);
	}
	return false;
}

bool AttributesImpl::existAttribute(int attributeName, int key) const
{
	LongObjectMapPointer longMapPointer;
	if (m_mapIntLongPointer.find(attributeName, longMapPointer))
	{
		return longMapPointer->exist(key);
	}
	return false;
}
bool AttributesImpl::existAttribute(int attributeName, bigint key) const
{
	BigIntObjectMapPointer longMapPointer;
	if (m_mapIntBigIntPointer.find(attributeName, longMapPointer))
	{
		return longMapPointer->exist(key);
	}
	return false;
}

bool AttributesImpl::existAttribute(int attributeName, void* key) const
{
	VoidObjectMapPointer voidMapPointer;
	if (m_mapIntVoidPointer.find(attributeName, voidMapPointer))
	{
		return voidMapPointer->exist(key);
	}
	return false;
}

bool AttributesImpl::existAttribute(const tstring & attributeName, const tstring & key) const
{
	StringObjectMapPointer stringMapPointer;
	if (m_mapStrStringPointer.find(attributeName, stringMapPointer))
	{
		return stringMapPointer->exist(key);
	}
	return false;
}

bool AttributesImpl::existAttribute(const tstring & attributeName, int key) const
{
	LongObjectMapPointer longMapPointer;
	if (m_mapStrLongPointer.find(attributeName, longMapPointer))
	{
		return longMapPointer->exist(key);
	}
	return false;
}

bool AttributesImpl::existAttribute(const tstring & attributeName, void* key) const
{
	VoidObjectMapPointer voidMapPointer;
	if (m_mapStrVoidPointer.find(attributeName, voidMapPointer))
	{
		return voidMapPointer->exist(key);
	}
	return false;
}

cgcObject::pointer AttributesImpl::removeAttribute(int attributeName, const tstring & key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer = getStringAttributes(attributeName, false);

	if (stringMapPointer.get() != NULL)
	{
		stringMapPointer->find(key, result, true);

		if (deleteIfEmpty && stringMapPointer->empty())
		{
			m_mapIntStringPointer.remove(attributeName);
		}
	}
	return result;
}

cgcObject::pointer AttributesImpl::removeAttribute(int attributeName, int key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer = getLongAttributes(attributeName, false);

	if (longMapPointer.get() != NULL)
	{
		longMapPointer->find(key, result, true);

		if (deleteIfEmpty && longMapPointer->empty())
		{
			m_mapIntLongPointer.remove(attributeName);
		}
	}
	return result;
}
cgcObject::pointer AttributesImpl::removeAttribute(int attributeName, bigint key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	BigIntObjectMapPointer longMapPointer = getBigIntAttributes(attributeName, false);

	if (longMapPointer.get() != NULL)
	{
		longMapPointer->find(key, result, true);

		if (deleteIfEmpty && longMapPointer->empty())
		{
			m_mapIntBigIntPointer.remove(attributeName);
		}
	}
	return result;
}

cgcObject::pointer AttributesImpl::removeAttribute(int attributeName, void* key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer = getVoidAttributes(attributeName, false);

	if (voidMapPointer.get() != NULL)
	{
		voidMapPointer->find(key, result, true);

		if (deleteIfEmpty && voidMapPointer->empty())
		{
			m_mapIntVoidPointer.remove(attributeName);
		}
	}
	return result;
}

cgcObject::pointer AttributesImpl::removeAttribute(const tstring & attributeName, const tstring & key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	StringObjectMapPointer stringMapPointer = getStringAttributes(attributeName, false);

	if (stringMapPointer.get() != NULL)
	{
		stringMapPointer->find(key, result, true);

		if (deleteIfEmpty && stringMapPointer->empty())
		{
			m_mapStrStringPointer.remove(attributeName);
		}
	}
	return result;
}

cgcObject::pointer AttributesImpl::removeAttribute(const tstring & attributeName, int key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	LongObjectMapPointer longMapPointer = getLongAttributes(attributeName, false);

	if (longMapPointer.get() != NULL)
	{
		longMapPointer->find(key, result, true);

		if (deleteIfEmpty && longMapPointer->empty())
		{
			m_mapStrLongPointer.remove(attributeName);
		}
	}
	return result;
}

cgcObject::pointer AttributesImpl::removeAttribute(const tstring & attributeName, void* key, bool deleteIfEmpty)
{
	cgcObject::pointer result;
	VoidObjectMapPointer voidMapPointer = getVoidAttributes(attributeName, false);

	if (voidMapPointer.get() != NULL)
	{
		voidMapPointer->find(key, result, true);

		if (deleteIfEmpty && voidMapPointer->empty())
		{
			m_mapStrVoidPointer.remove(attributeName);
		}
	}
	return result;
}

void AttributesImpl::clearStringAtrributes(int attributeName)
{
	m_mapIntStringPointer.remove(attributeName);
}

void AttributesImpl::clearLongAtrributes(int attributeName)
{
	m_mapIntLongPointer.remove(attributeName);
}
void AttributesImpl::clearBigIntAtrributes(int attributeName)
{
	m_mapIntBigIntPointer.remove(attributeName);
}

void AttributesImpl::clearVoidAtrributes(int attributeName)
{
	m_mapIntVoidPointer.remove(attributeName);
}

void AttributesImpl::clearStringAtrributes(const tstring & attributeName)
{
	m_mapStrStringPointer.remove(attributeName);
}

void AttributesImpl::clearLongAtrributes(const tstring & attributeName)
{
	m_mapStrLongPointer.remove(attributeName);
}

void AttributesImpl::clearVoidAtrributes(const tstring & attributeName)
{
	m_mapStrVoidPointer.remove(attributeName);
}

//void AttributesImpl::clearAllStringAtrributes(void)
//{
//	m_mapIntStringPointer.clear();
//	m_mapStrStringPointer.clear();
//}
//
//void AttributesImpl::clearAllULongAtrributes(void)
//{
//	m_mapIntLongPointer.clear();
//	m_mapStrLongPointer.clear();
//}

void AttributesImpl::clearAllAtrributes(void)
{
	m_mapIntStringPointer.clear();
	m_mapIntLongPointer.clear();
	m_mapIntBigIntPointer.clear();
	m_mapIntVoidPointer.clear();
	m_mapStrStringPointer.clear();
	m_mapStrLongPointer.clear();
	m_mapStrVoidPointer.clear();
}

} // namespace mycp
