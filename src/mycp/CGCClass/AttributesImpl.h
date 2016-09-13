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

// AttributesImpl.h file here
#ifndef _ATTRIBUTESIMPL_HEAD_INCLUDED__
#define _ATTRIBUTESIMPL_HEAD_INCLUDED__

#include "../CGCBase/includeall.h"

namespace mycp {

class AttributesImpl
	: public cgcAttributes
{
private:
	CStringObjectMap<int>		m_mapIntStringPointer;
	CLongObjectMap<int>			m_mapIntLongPointer;
	CBigIntObjectMap<int>		m_mapIntBigIntPointer;
	CVoidObjectMap<int>			m_mapIntVoidPointer;
	CStringObjectMap<tstring>	m_mapStrStringPointer;
	CLongObjectMap<tstring>		m_mapStrLongPointer;
	CVoidObjectMap<tstring>		m_mapStrVoidPointer;

	//CLockMultiMap<tstring, tstring> m_strStrPropertys;
	//CLockMultiMap<tstring, int>		m_strIntPropertys;
	//CLockMultiMap<int, tstring>		m_intStrPropertys;
	//CLockMultiMap<int, int>			m_intIntPropertys;

	CLockMultiMap<tstring, cgcValueInfo::pointer> m_strPropertys;
	CLockMultiMap<int, cgcValueInfo::pointer> m_intPropertys;
	CLockMultiMap<bigint, cgcValueInfo::pointer> m_bigintPropertys;
	CLockMultiMap<void*, cgcValueInfo::pointer> m_pointerPropertys;

public:
	AttributesImpl(void);
	virtual ~AttributesImpl(void);

public:
	// attribute
	virtual void setProperty(const tstring& key, const cgcValueInfo::pointer& value, bool clear = true);
	virtual void setProperty(int key, const cgcValueInfo::pointer& value, bool clear = true);
	virtual void setProperty(bigint key, const cgcValueInfo::pointer& value, bool clear = true);
	virtual void setProperty(void* key, const cgcValueInfo::pointer& value, bool clear = true);

	virtual cgcValueInfo::pointer getProperty(const tstring& key) const;
	virtual cgcValueInfo::pointer getProperty(const tstring& key,bool erase);
	virtual cgcValueInfo::pointer getProperty(int key) const;
	virtual cgcValueInfo::pointer getProperty(int key,bool erase);
	virtual cgcValueInfo::pointer getProperty(bigint key) const;
	virtual cgcValueInfo::pointer getProperty(bigint key,bool erase);
	virtual cgcValueInfo::pointer getProperty(void* key) const;
	virtual cgcValueInfo::pointer getProperty(void* key,bool erase);

	virtual bool getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues) const;
	virtual bool getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues,bool erase);
	virtual bool getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues) const;
	virtual bool getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues,bool erase);
	virtual bool getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues) const;
	virtual bool getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues,bool erase);
	virtual bool getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues) const;
	virtual bool getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues,bool erase);

	virtual bool getSPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const;
	virtual bool getIPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const;
	virtual bool getBPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const;
	virtual bool getPPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const;

	virtual size_t getPropertySize(const tstring& key) const;
	virtual size_t getPropertySize(int key) const;
	virtual size_t getPropertySize(bigint key) const;
	virtual size_t getPropertySize(void* key) const;

	virtual void delProperty(const tstring& key);
	virtual void delProperty(int key);
	virtual void delProperty(bigint key);
	virtual void delProperty(void* key);

	virtual void cleanSPropertys(void);
	virtual void cleanIPropertys(void);
	virtual void cleanBPropertys(void);
	virtual void cleanPPropertys(void);
	virtual void cleanAllPropertys(void);

	virtual StringObjectMapPointer	getStringAttributes(int attributeName, bool newIfNotExist);
	virtual LongObjectMapPointer	getLongAttributes(int attributeName, bool newIfNotExist);
	virtual BigIntObjectMapPointer	getBigIntAttributes(int attributeName, bool newIfNotExist);
	virtual VoidObjectMapPointer	getVoidAttributes(int attributeName, bool newIfNotExist);
	virtual StringObjectMapPointer	getStringAttributes(const tstring & attributeName, bool newIfNotExist);
	virtual LongObjectMapPointer	getLongAttributes(const tstring & attributeName, bool newIfNotExist);
	virtual VoidObjectMapPointer	getVoidAttributes(const tstring & attributeName, bool newIfNotExist);

	virtual cgcObject::pointer setAttribute(int attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(int attributeName, int key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(int attributeName, bigint key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(int attributeName, void* key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, int key, const cgcObject::pointer& pObject,bool force = true);
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, void* key, const cgcObject::pointer& pObject,bool force = true);
	
	//virtual int setValue(int key, const tstring& value);
	//virtual int setValue(int key, void* value);

	virtual cgcObject::pointer getAttribute(int attributeName, const tstring & key) const;
	virtual cgcObject::pointer getAttribute(int attributeName, int key) const;
	virtual cgcObject::pointer getAttribute(int attributeName, bigint key) const;
	virtual cgcObject::pointer getAttribute(int attributeName, void* key) const;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, const tstring & key) const;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, int key) const;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, void* key) const;

	virtual bool existAttribute(int attributeName, const tstring & key) const;
	virtual bool existAttribute(int attributeName, int key) const;
	virtual bool existAttribute(int attributeName, bigint key) const;
	virtual bool existAttribute(int attributeName, void* key) const;
	virtual bool existAttribute(const tstring & attributeName, const tstring & key) const;
	virtual bool existAttribute(const tstring & attributeName, int key) const;
	virtual bool existAttribute(const tstring & attributeName, void* key) const;

	virtual cgcObject::pointer removeAttribute(int attributeName, const tstring & key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(int attributeName, int key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(int attributeName, bigint key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(int attributeName, void* key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, const tstring & key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, int key, bool deleteIfEmpty);
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, void* key, bool deleteIfEmpty);

	virtual void clearStringAtrributes(int attributeName);
	virtual void clearLongAtrributes(int attributeName);
	virtual void clearBigIntAtrributes(int attributeName);
	virtual void clearVoidAtrributes(int attributeName);
	virtual void clearStringAtrributes(const tstring & attributeName);
	virtual void clearLongAtrributes(const tstring & attributeName);
	virtual void clearVoidAtrributes(const tstring & attributeName);

	//virtual void clearAllStringAtrributes(void);
	//virtual void clearAllLongAtrributes(void);

	virtual void clearAllAtrributes(void);
};

} // namespace mycp

#endif // _ATTRIBUTESIMPL_HEAD_INCLUDED__
