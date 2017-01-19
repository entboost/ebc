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

// cgcAttributes.h file here
#ifndef __cgcattributes_head__
#define __cgcattributes_head__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "cgcvalueinfo.h"

namespace mycp {

class cgcKeyValue
{
public:
	typedef boost::shared_ptr<cgcKeyValue> pointer;

	void setKey(const tstring& key) {m_key = key;}
	const tstring& getKey(void) const {return m_key;}

	void setValue(const cgcValueInfo::pointer& value) {m_value = value;}
	cgcValueInfo::pointer getValue(void) const {return m_value;}

	cgcKeyValue(const tstring& key, const cgcValueInfo::pointer& value)
		: m_key(key), m_value(value)
	{}
private:
	tstring m_key;
	cgcValueInfo::pointer m_value;
};
const cgcKeyValue::pointer cgcNullKeyValue;
#define CGC_KEYVALUE(K, V) cgcKeyValue::pointer(new cgcKeyValue(K, V))

class cgcAttributes
{
public:
	typedef boost::shared_ptr<cgcAttributes> pointer;

	virtual void setProperty(const tstring& key, const cgcValueInfo::pointer& value, bool clear = true) = 0;
	virtual void setProperty(int key, const cgcValueInfo::pointer& value, bool clear = true) = 0;
	virtual void setProperty(bigint key, const cgcValueInfo::pointer& value, bool clear = true) = 0;
	virtual void setProperty(void* key, const cgcValueInfo::pointer& value, bool clear = true) = 0;

	virtual cgcValueInfo::pointer getProperty(const tstring& key) const = 0;
	virtual cgcValueInfo::pointer getProperty(const tstring& key,bool erase) = 0;
	virtual cgcValueInfo::pointer getProperty(int key) const = 0;
	virtual cgcValueInfo::pointer getProperty(int key,bool erase) = 0;
	virtual cgcValueInfo::pointer getProperty(bigint key) const = 0;
	virtual cgcValueInfo::pointer getProperty(bigint key,bool erase) = 0;
	virtual cgcValueInfo::pointer getProperty(void* key) const = 0;
	virtual cgcValueInfo::pointer getProperty(void* key,bool erase) = 0;

	virtual bool getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues) const = 0;
	virtual bool getProperty(const tstring& key, std::vector<cgcValueInfo::pointer>& outValues,bool erase) = 0;
	virtual bool getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues) const = 0;
	virtual bool getProperty(int key, std::vector<cgcValueInfo::pointer>& outValues,bool erase) = 0;
	virtual bool getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues) const = 0;
	virtual bool getProperty(bigint key, std::vector<cgcValueInfo::pointer>& outValues,bool erase) = 0;
	virtual bool getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues) const = 0;
	virtual bool getProperty(void* key, std::vector<cgcValueInfo::pointer>& outValues,bool erase) = 0;

	virtual bool getSPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const = 0;
	virtual bool getIPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const = 0;
	virtual bool getBPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const = 0;
	virtual bool getPPropertys(std::vector<cgcKeyValue::pointer>& outKVs) const = 0;

	virtual size_t getPropertySize(const tstring& key) const = 0;
	virtual size_t getPropertySize(int key) const = 0;
	virtual size_t getPropertySize(bigint key) const = 0;
	virtual size_t getPropertySize(void* key) const = 0;

	virtual void delProperty(const tstring& key) = 0;
	virtual void delProperty(int key) = 0;
	virtual void delProperty(bigint key) = 0;
	virtual void delProperty(void* key) = 0;

	virtual bool delProperty(const tstring& key, const cgcValueInfo::pointer& pValue) = 0;
	virtual bool delProperty(int key, const cgcValueInfo::pointer& pValue) = 0;
	virtual bool delProperty(bigint key, const cgcValueInfo::pointer& pValue) = 0;
	virtual bool delProperty(void* key, const cgcValueInfo::pointer& pValue) = 0;

	virtual void cleanSPropertys(void) = 0;
	virtual void cleanIPropertys(void) = 0;
	virtual void cleanBPropertys(void) = 0;
	virtual void cleanPPropertys(void) = 0;
	virtual void cleanAllPropertys(void) = 0;

	// ** map
	virtual StringObjectMapPointer	getStringAttributes(int attributeName = 1, bool newIfNotExist = true) = 0;
	virtual LongObjectMapPointer	getLongAttributes(int attributeName = 1, bool newIfNotExist = true) = 0;
	virtual BigIntObjectMapPointer	getBigIntAttributes(int attributeName = 1, bool newIfNotExist = true) = 0;
	virtual VoidObjectMapPointer	getVoidAttributes(int attributeName = 1, bool newIfNotExist = true) = 0;
	virtual StringObjectMapPointer	getStringAttributes(const tstring & attributeName, bool newIfNotExist = true) = 0;
	virtual LongObjectMapPointer	getLongAttributes(const tstring & attributeName, bool newIfNotExist = true) = 0;
	//virtual BigIntObjectMapPointer	getBigIntAttributes(const tstring & attributeName, bool newIfNotExist = true) = 0;
	virtual VoidObjectMapPointer	getVoidAttributes(const tstring & attributeName, bool newIfNotExist = true) = 0;

	// Return the old attributeName(attributeKey) value.
	virtual cgcObject::pointer setAttribute(int attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(int attributeName, int key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(int attributeName, bigint key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(int attributeName, void* key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, const tstring & key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, int key, const cgcObject::pointer& pObject,bool force = true) = 0;
	virtual cgcObject::pointer setAttribute(const tstring & attributeName, void* key, const cgcObject::pointer& pObject,bool force = true) = 0;

	virtual cgcObject::pointer getAttribute(int attributeName, const tstring & key) const = 0;
	virtual cgcObject::pointer getAttribute(int attributeName, int key) const = 0;
	virtual cgcObject::pointer getAttribute(int attributeName, bigint key) const = 0;
	virtual cgcObject::pointer getAttribute(int attributeName, void* key) const = 0;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, const tstring & key) const = 0;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, int key) const = 0;
	//virtual cgcObject::pointer getAttribute(const tstring & attributeName, bigint key) const = 0;
	virtual cgcObject::pointer getAttribute(const tstring & attributeName, void* key) const = 0;

	virtual bool existAttribute(int attributeName, const tstring & key) const = 0;
	virtual bool existAttribute(int attributeName, const tstring & key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(int attributeName, int key) const = 0;
	virtual bool existAttribute(int attributeName, int key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(int attributeName, bigint key) const = 0;
	virtual bool existAttribute(int attributeName, bigint key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(int attributeName, void* key) const = 0;
	virtual bool existAttribute(int attributeName, void* key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(const tstring & attributeName, const tstring & key) const = 0;
	virtual bool existAttribute(const tstring & attributeName, const tstring & key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(const tstring & attributeName, int key) const = 0;
	virtual bool existAttribute(const tstring & attributeName, int key, const cgcObject::pointer& pObject) const = 0;
	virtual bool existAttribute(const tstring & attributeName, void* key) const = 0;
	virtual bool existAttribute(const tstring & attributeName, void* key, const cgcObject::pointer& pObject) const = 0;

	virtual cgcObject::pointer removeAttribute(int attributeName, const tstring & key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(int attributeName, int key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(int attributeName, bigint key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(int attributeName, void* key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, const tstring & key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, int key, bool deleteIfEmpty = true) = 0;
	virtual cgcObject::pointer removeAttribute(const tstring & attributeName, void* key, bool deleteIfEmpty = true) = 0;

	virtual bool removeAttribute(int attributeName, const tstring & key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(int attributeName, int key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(int attributeName, bigint key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(int attributeName, void* key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(const tstring & attributeName, const tstring & key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(const tstring & attributeName, int key, const cgcObject::pointer& pObject) = 0;
	virtual bool removeAttribute(const tstring & attributeName, void* key, const cgcObject::pointer& pObject) = 0;

	virtual void clearStringAtrributes(int attributeName) = 0;
	virtual void clearLongAtrributes(int attributeName) = 0;
	virtual void clearBigIntAtrributes(int attributeName) = 0;
	virtual void clearVoidAtrributes(int attributeName) = 0;
	virtual void clearStringAtrributes(const tstring & attributeName) = 0;
	virtual void clearLongAtrributes(const tstring & attributeName) = 0;
	virtual void clearVoidAtrributes(const tstring & attributeName) = 0;

	//virtual void clearAllStringAtrributes(void) = 0;
	//virtual void clearAllULongAtrributes(void) = 0;

	// ** list
	virtual ObjectListPointer	getListAttributes(int attributeName, bool newIfNotExist) = 0;
	virtual ObjectListPointer	getListAttributes(bigint attributeName, bool newIfNotExist) = 0;
	virtual ObjectListPointer	getListAttributes(const tstring & attributeName, bool newIfNotExist) = 0;
	virtual ObjectListPointer	getListAttributes(void* attributeName, bool newIfNotExist) = 0;

	virtual void addListAttribute(int attributeName, const cgcObject::pointer& pObject, bool is_lock = true) = 0;
	virtual void addListAttribute(bigint attributeName, const cgcObject::pointer& pObject, bool is_lock = true) = 0;
	virtual void addListAttribute(const tstring& attributeName, const cgcObject::pointer& pObject, bool is_lock = true) = 0;
	virtual void addListAttribute(void* attributeName, const cgcObject::pointer& pObject, bool is_lock = true) = 0;

	virtual cgcObject::pointer getBackAttribute(int attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getBackAttribute(bigint attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getBackAttribute(const tstring& attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getBackAttribute(void* attributeName, bool is_pop=true) = 0;

	virtual void setFrontAttribute(int attributeName, const cgcObject::pointer& pObject) = 0;
	virtual void setFrontAttribute(bigint attributeName, const cgcObject::pointer& pObject) = 0;
	virtual void setFrontAttribute(const tstring& attributeName, const cgcObject::pointer& pObject) = 0;
	virtual void setFrontAttribute(void* attributeName, const cgcObject::pointer& pObject) = 0;

	virtual cgcObject::pointer getFrontListAttribute(int attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getFrontListAttribute(bigint attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getFrontListAttribute(const tstring& attributeName, bool is_pop=true) = 0;
	virtual cgcObject::pointer getFrontListAttribute(void* attributeName, bool is_pop=true) = 0;

	virtual bool isListAttributeEmtpy(int attributeName) const = 0;
	virtual bool isListAttributeEmtpy(bigint attributeName) const = 0;
	virtual bool isListAttributeEmtpy(const tstring& attributeName) const = 0;
	virtual bool isListAttributeEmtpy(void* attributeName) const = 0;

	virtual size_t getListAttributeSize(int attributeName) const = 0;
	virtual size_t getListAttributeSize(bigint attributeName) const = 0;
	virtual size_t getListAttributeSize(const tstring& attributeName) const = 0;
	virtual size_t getListAttributeSize(void* attributeName) const = 0;

	virtual void clearListAttribute(int attributeName, bool is_lock = true) = 0;
	virtual void clearListAttribute(bigint attributeName, bool is_lock = true) = 0;
	virtual void clearListAttribute(const tstring& attributeName, bool is_lock = true) = 0;
	virtual void clearListAttribute(void* attributeName, bool is_lock = true) = 0;

	// nClearType: 0x1=clear map only; 0x2= clear list only; 0x3=clear map & list
	virtual void clearAllAtrributes(int nClearType=0x3) = 0;

};

const cgcAttributes::pointer cgcNullAttributes;

} // namespace mycp

#endif // __cgcattributes_head__
