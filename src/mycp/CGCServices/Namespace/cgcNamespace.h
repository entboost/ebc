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

// cgcNamespace.h file here
#ifndef __cgcnamespace_head__
#define __cgcnamespace_head__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcService.h>

#define USES_NAMESPACE		1	// [0/1]

namespace cgc{

class cgcKVInfo
{
public:
	typedef boost::shared_ptr<cgcKVInfo> pointer;
	static cgcKVInfo::pointer create(const tstring& key, const tstring& value)
	{
		return cgcKVInfo::pointer(new cgcKVInfo(key, value));
	}

	void key(const tstring& key) {m_key = key;}
	const tstring& key(void) const {return m_key;}
	void value(const tstring& value) {m_value = value;}
	const tstring& value(void) const {return m_value;}

	cgcKVInfo(const tstring& key, const tstring& value)
		: m_key(key), m_value(value)
	{}
private:
	tstring m_key;
	tstring m_value;
};

const cgcKVInfo::pointer cgcNullKVInfo;

#define CGC_KV(k, v) cgcKVInfo::create(k, v)


class cgcNamespace
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcNamespace> pointer;

	// Get all key values.
	virtual bool get(const tstring& key, std::vector<tstring>& outValues) const = 0;
	virtual bool get(std::vector<cgcKVInfo::pointer>& outValues) const = 0;

	// Get first value.
	virtual tstring get(const tstring& key, const tstring& defaultvalue = _T("")) const = 0;
	virtual int get(const tstring& key, int defaultvalue) const = 0;

	// -1: error
	virtual int count(const tstring& key, const tstring& value) const = 0;
	virtual int count(cgcKVInfo::pointer kv) const = 0;
	virtual int count(const tstring& key) const = 0;
	virtual int count(void) const = 0;

	virtual bool exist(const tstring& key, const tstring& value) const = 0;
	virtual bool exist(cgcKVInfo::pointer kv) const = 0;
	virtual bool exist(const tstring& key) const = 0;
	virtual bool exist(void) const = 0;

	// Not exist created.
	virtual bool set(cgcKVInfo::pointer kv) = 0;
	virtual bool set(const tstring& key, const tstring& value) = 0;

	// Delete all value of the key.
	virtual bool del(const tstring& key, const tstring& value) = 0;
	virtual bool del(cgcKVInfo::pointer kv) = 0;
	virtual bool del(const tstring& key) = 0;
	virtual bool del(void) = 0;

	virtual bool add(const tstring& key, const tstring& value) = 0;
	virtual bool add(cgcKVInfo::pointer kv) = 0;
	virtual bool add(const std::vector<cgcKVInfo::pointer>& values) = 0;
	virtual bool add(const tstring& key, const std::vector<tstring>& values) = 0;

};

#define CGC_NAMESPACESERVICE_DEF(s) boost::static_pointer_cast<cgcNamespace, cgcServiceInterface>(s)

} // cgc namespace

#endif // __cgcnamespace_head__
