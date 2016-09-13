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

// cgcSmartString.h file here
#ifndef __mycp_cgcSmartString_head__
#define __mycp_cgcSmartString_head__

#pragma warning(disable:4996)

#include <string>
#include <boost/shared_ptr.hpp>
//#include "cgcobject.h"

#ifdef CGC_SMART_STRING_USES_BSTR

#define Z_A2W_CP_EX(lpa, nChars, cp) (\
	((_lpa_ex = lpa) == NULL) ? NULL : (\
	_convert_ex = (::MultiByteToWideChar(cp, 0, _lpa_ex, (int)(strlen(_lpa_ex)), NULL, 0) + 1), \
	FAILED(::ATL::AtlMultiply(&_convert_ex, _convert_ex, static_cast<int>(sizeof(WCHAR)))) ? NULL : \
	ATLA2WHELPER(	\
	(LPWSTR)_ATL_SAFE_ALLOCA(_convert_ex, _ATL_SAFE_ALLOCA_DEF_THRESHOLD), \
	_lpa_ex, \
	_convert_ex, \
	(cp))))

#define Z_W2A_CP_EX(lpw, nChars, cp) (\
	((_lpw_ex = lpw) == NULL) ? NULL : (\
	_convert_ex = (::WideCharToMultiByte(cp, 0, _lpw_ex, (int)(wcslen(_lpw_ex)), NULL, 0, NULL, NULL) + 1), \
	ATLW2AHELPER(	\
	(LPSTR)_ATL_SAFE_ALLOCA(_convert_ex, _ATL_SAFE_ALLOCA_DEF_THRESHOLD), \
	_lpw_ex, \
	_convert_ex, \
	(cp))))

#undef USES_CONVERSION
#undef A2W
#undef W2A
#undef A2W_CP
#undef W2A_CP
#undef A2W_CP_EX
#undef W2A_CP_EX
// 强制使用安全版本
#define USES_CONVERSION	USES_CONVERSION_EX
#define A2W(lpa)	A2W_EX(lpa, 0)
#define W2A(lpw)	W2A_EX(lpw, 0)
#define A2W_CP_EX(lpa, cp)	Z_A2W_CP_EX(lpa, 0, cp)
#define W2A_CP_EX(lpw, cp)	Z_W2A_CP_EX(lpw, 0, cp)
#define A2W_CP(lpa, cp)	A2W_CP_EX(lpa, cp)
#define W2A_CP(lpw, cp)	W2A_CP_EX(lpw, cp)

#define A2W_UTF8(lpa)	A2W_CP_EX(lpa, CP_UTF8)
#define W2A_UTF8(lpw)	W2A_CP_EX(lpw, CP_UTF8)
#define A2W_ACP(lpa)	A2W_CP_EX(lpa, CP_ACP)
#define W2A_ACP(lpw)	W2A_CP_EX(lpw, CP_ACP)

inline BSTR A2BSTR_CP(LPCSTR lpa, UINT cp)
{
	USES_CONVERSION;
	return CComBSTR(A2W_CP_EX(lpa, cp)).Detach();
}

inline BSTR A2BSTR_UTF8(LPCSTR lpa)
{
	return A2BSTR_CP(lpa, CP_UTF8);
}

#define BSTR2A_EX(bstr) (bstr==NULL?"":W2A_EX(bstr,0))

#endif

namespace mycp {

class cgcSmartString
	//: public cgcObject
{
public:
	typedef boost::shared_ptr<cgcSmartString> pointer;
	//typedef typename std::string::size_type size_type;
	//typedef typename std::string::npos npos;

	static cgcSmartString::pointer create(void);
	static cgcSmartString::pointer create(char data);
	static cgcSmartString::pointer create(const char* data);
	static cgcSmartString::pointer create(const char* data, size_t count);
	static cgcSmartString::pointer create(const std::string& data);
	static cgcSmartString::pointer create(const cgcSmartString& data);
	static cgcSmartString::pointer create(const cgcSmartString::pointer& data);
#ifdef CGC_SMART_STRING_USES_BSTR
	static cgcSmartString::pointer create(BSTR bstr);
	static cgcSmartString::pointer create(BSTR bstr, UINT cp, bool bFreeInputBSTR=false);
#endif
	cgcSmartString(void);
	cgcSmartString(char data);
	cgcSmartString(const char* data);
	cgcSmartString(const char* data, size_t count);
	cgcSmartString(const std::string& data);
	cgcSmartString(const cgcSmartString& data);
	cgcSmartString(const cgcSmartString::pointer& data);
#ifdef CGC_SMART_STRING_USES_BSTR
	cgcSmartString(BSTR bstr);
	cgcSmartString(BSTR bstr, UINT cp, bool bFreeInputBSTR=false);
#endif
	virtual ~cgcSmartString(void);

#ifdef CGC_SMART_STRING_USES_BSTR
	const cgcSmartString& operator =  (BSTR bstr);
#endif
	const cgcSmartString& operator =  (char data);
	const cgcSmartString& operator =  (const char* data);
	const cgcSmartString& operator =  (const std::string& data);
	const cgcSmartString& operator =  (const cgcSmartString& data);
#ifdef CGC_SMART_STRING_USES_BSTR
	const cgcSmartString& operator += (BSTR bstr);
#endif
	const cgcSmartString& operator += (char data);
	const cgcSmartString& operator += (const char* data);
	const cgcSmartString& operator += (const std::string& data);
	const cgcSmartString& operator += (const cgcSmartString& data);
#ifdef CGC_SMART_STRING_USES_BSTR
	cgcSmartString operator + (BSTR bstr);
#endif
	cgcSmartString operator + (char data);
	cgcSmartString operator + (const char* data);
	cgcSmartString operator + (const std::string& data);
	cgcSmartString operator + (const cgcSmartString& data);
#ifdef CGC_SMART_STRING_USES_BSTR
	friend cgcSmartString operator + (BSTR data1, const cgcSmartString& data2) {cgcSmartString temp(data1); temp+=data2; return temp;}
#endif
	friend cgcSmartString operator + (char data1, const cgcSmartString& data2) {cgcSmartString temp(data1); temp+=data2; return temp;}
	friend cgcSmartString operator + (const char* data1, const cgcSmartString& data2) {cgcSmartString temp(data1); temp+=data2; return temp;}
	friend cgcSmartString operator + (const std::string& data1, const cgcSmartString& data2) {cgcSmartString temp(data1); temp+=data2; return temp;}
	friend cgcSmartString operator + (const cgcSmartString& data1, const cgcSmartString& data2) {cgcSmartString temp(data1); temp+=data2; return temp;}
#ifdef CGC_SMART_STRING_USES_BSTR
	friend bool operator == (BSTR data1, const cgcSmartString& data2) {return data2==data1;}
#endif
	friend bool operator == (const char* data1, const cgcSmartString& data2) {return data2==data1;}
	friend bool operator == (const std::string& data1, const cgcSmartString& data2) {return data2==data1;}

	//const cgcSmartString& operator*() {return *this;}
	//const cgcSmartString& operator->() {return *this};

	operator std::string() const {return toString();}
	//operator std::basic_string<char>() const {return toString();}
	operator const char*() const {return this->c_str();}

#ifdef CGC_SMART_STRING_USES_BSTR
	bool operator == (BSTR data) const;
#endif
	bool operator == (const char* data) const;
	bool operator == (const std::string& data) const;
	bool operator == (const cgcSmartString& data) const;
	bool operator != (const char* data) const;
	bool operator != (const std::string& data) const;
	bool operator != (const cgcSmartString& data) const;
	bool operator > (const char* data) const;
	bool operator > (const std::string& data) const;
	bool operator > (const cgcSmartString& data) const;
	bool operator >= (const char* data) const;
	bool operator >= (const std::string& data) const;
	bool operator >= (const cgcSmartString& data) const;
	bool operator < (const char* data) const;
	bool operator < (const std::string& data) const;
	bool operator < (const cgcSmartString& data) const;
	bool operator <= (const char* data) const;
	bool operator <= (const std::string& data) const;
	bool operator <= (const cgcSmartString& data) const;

#ifdef CGC_SMART_STRING_USES_BSTR
	BSTR CopyBSTR(int cp=CP_ACP) const {USES_CONVERSION_EX; return ::SysAllocString(A2W_CP_EX(this->c_str(),cp));}
#endif
	const char* c_str(void) const {return m_sData->c_str();}
	const char* data(void) const {return m_sData->c_str();}
	const std::string* str(void) const {return m_sData;}
	const std::string& string(void) const {return *m_sData;}
	std::string& string(void) {return *m_sData;}
	size_t size(void) const {return m_sData->size();}
	size_t length(void) const {return m_sData->length();}
	bool empty(void) const {return m_sData->empty();}
	void clear(void) {m_sData->clear();}
	int compare(const char* data) const {return m_sData->compare(data);}
	int compare(const std::string& data) const {return m_sData->compare(data);}
	int compare(const cgcSmartString& data) const {return m_sData->compare(data.c_str());}

#ifdef CGC_SMART_STRING_USES_BSTR
	void assign(BSTR bstr) {USES_CONVERSION_EX; m_sData->assign(BSTR2A_EX(bstr));}
#endif
	void assign(char data) {m_sData->assign((std::string::size_type)1,data);}
	void assign(const char* data) {m_sData->assign(data);}
	void assign(const char* data, size_t count) {m_sData->assign(data, count);}
	void assign(const std::string& data) {m_sData->assign(data);}
	void assign(const cgcSmartString& data) {m_sData->assign(data.c_str(), data.size());}
#ifdef CGC_SMART_STRING_USES_BSTR
	void append(BSTR bstr) {USES_CONVERSION_EX; m_sData->append(BSTR2A_EX(bstr));}
#endif
	void append(char data) {m_sData->append((std::string::size_type)1,data);}
	void append(const char* data) {m_sData->append(data);}
	void append(const char* data, size_t count) {m_sData->append(data, count);}
	void append(const std::string& data) {m_sData->append(data);}
	void append(const cgcSmartString& data) {m_sData->append(data.c_str(), data.size());}
#ifdef CGC_SMART_STRING_USES_BSTR
	void fromString(BSTR bstr) {USES_CONVERSION_EX; *m_sData = BSTR2A_EX(bstr);}
#endif
	void fromString(const char* data) {*m_sData = data;}
	void fromString(const std::string& data) {*m_sData = data.c_str();}
	void fromString(const cgcSmartString& data) {*m_sData = data.c_str();}
	std::string toString(void) const {std::string s(m_sData->c_str()); return s;}
	cgcSmartString substr(std::string::size_type off, std::string::size_type count=std::string::npos) const {return m_sData->substr(off, count);}

	void erase(std::string::size_type off=0, std::string::size_type count=std::string::npos) {m_sData->erase(off, count);}
	void insert(std::string::size_type off, char data) {m_sData->insert(off, (std::string::size_type)1, data);}
	void insert(std::string::size_type off, const char* data) {m_sData->insert(off, data);}
	void insert(std::string::size_type off, const char* data, std::string::size_type count) {m_sData->insert(off, data, count);}
	void insert(std::string::size_type off, const std::string& data) {m_sData->insert(off, data);}
	void insert(std::string::size_type off, const cgcSmartString& data) {m_sData->insert(off, data.c_str(), data.size());}
	//void insert(std::string::const_iterator _where, char data) {m_sData->insert(_where, data);}
	void replace(std::string::size_type off, std::string::size_type count, const char* data) {m_sData->replace(off, count, data);}
	void replace(std::string::size_type off, std::string::size_type count, const std::string& data) {m_sData->replace(off, count, data);}
	void replace(std::string::size_type off, std::string::size_type count, const cgcSmartString& data) {m_sData->replace(off, count, data.c_str(), data.size());}
	//void replace(std::string::const_iterator _first, std::string::const_iterator _last, const char* data) {m_sData->replace(_first, _last, data);}
	//void replace(std::string::const_iterator _first, std::string::const_iterator _last, const std::string& data) {m_sData->replace(_first, _last, data);}
	//void replace(std::string::const_iterator _first, std::string::const_iterator _last, const cgcSmartString& data) {m_sData->replace(_first, _last, data.c_str(), data.size());}

	std::string::size_type find_first_of(char data, std::string::size_type off=0) const {return m_sData->find_first_of(data, off);}
	std::string::size_type find_first_of(const char* data, std::string::size_type off=0) const {return m_sData->find_first_of(data, off);}
	std::string::size_type find_first_of(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->find_first_of(data, off, count);}
	std::string::size_type find_first_of(const std::string& data, std::string::size_type off=0) const {return m_sData->find_first_of(data, off);}
	std::string::size_type find_first_of(const cgcSmartString& data, std::string::size_type off=0) const {return m_sData->find_first_of(data.c_str(), off, data.size());}
	std::string::size_type find_first_not_of(char data, std::string::size_type off=0) const {return m_sData->find_first_not_of(data, off);}
	std::string::size_type find_first_not_of(const char* data, std::string::size_type off=0) const {return m_sData->find_first_not_of(data, off);}
	std::string::size_type find_first_not_of(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->find_first_not_of(data, off, count);}
	std::string::size_type find_first_not_of(const std::string& data, std::string::size_type off=0) const {return m_sData->find_first_not_of(data, off);}
	std::string::size_type find_first_not_of(const cgcSmartString& data, std::string::size_type off=0) const {return m_sData->find_first_not_of(data.c_str(), off, data.size());}
	std::string::size_type find_last_of(char data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_of(data, off);}
	std::string::size_type find_last_of(const char* data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_of(data, off);}
	std::string::size_type find_last_of(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->find_last_of(data, off, count);}
	std::string::size_type find_last_of(const std::string& data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_of(data, off);}
	std::string::size_type find_last_of(const cgcSmartString& data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_of(data.c_str(), off, data.size());}
	std::string::size_type find_last_not_of(char data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_not_of(data, off);}
	std::string::size_type find_last_not_of(const char* data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_not_of(data, off);}
	std::string::size_type find_last_not_of(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->find_last_not_of(data, off, count);}
	std::string::size_type find_last_not_of(const std::string& data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_not_of(data, off);}
	std::string::size_type find_last_not_of(const cgcSmartString& data, std::string::size_type off=std::string::npos) const {return m_sData->find_last_not_of(data.c_str(), off, data.size());}

	std::string::size_type find(char data, std::string::size_type off=0) const {return m_sData->find(data, off);}
	std::string::size_type find(const char* data, std::string::size_type off=0) const {return m_sData->find(data, off);}
	std::string::size_type find(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->find(data, off, count);}
	std::string::size_type find(const std::string& data, std::string::size_type off=0) const {return m_sData->find(data, off);}
	std::string::size_type find(const cgcSmartString& data, std::string::size_type off=0) const {return m_sData->find(data.c_str(), off, data.size());}
	std::string::size_type rfind(char data, std::string::size_type off=std::string::npos) const {return m_sData->rfind(data, off);}
	std::string::size_type rfind(const char* data, std::string::size_type off=std::string::npos) const {return m_sData->rfind(data, off);}
	std::string::size_type rfind(const char* data, std::string::size_type off, std::string::size_type count) const {return m_sData->rfind(data, off, count);}
	std::string::size_type rfind(const std::string& data, std::string::size_type off=std::string::npos) const {return m_sData->rfind(data, off);}
	std::string::size_type rfind(const cgcSmartString& data, std::string::size_type off=std::string::npos) const {return m_sData->rfind(data.c_str(), off, data.size());}

	//std::string::iterator first(void) {return m_sData->first();}
	std::string::iterator begin(void) {return m_sData->begin();}
	std::string::const_iterator begin(void) const {return m_sData->begin();}
	std::string::iterator end(void) {return m_sData->end();}
	std::string::const_iterator end(void) const {return m_sData->end();}
private:
	std::string* m_sData;
};

inline cgcSmartString::pointer cgcSmartString::create(void){return cgcSmartString::pointer(new cgcSmartString());}
inline cgcSmartString::pointer cgcSmartString::create(char data){return cgcSmartString::pointer(new cgcSmartString(data));}
inline cgcSmartString::pointer cgcSmartString::create(const char* data){return cgcSmartString::pointer(new cgcSmartString(data));}
inline cgcSmartString::pointer cgcSmartString::create(const char* data, size_t count){return cgcSmartString::pointer(new cgcSmartString(data, count));}
inline cgcSmartString::pointer cgcSmartString::create(const std::string& data){return cgcSmartString::pointer(new cgcSmartString(data));}
inline cgcSmartString::pointer cgcSmartString::create(const cgcSmartString& data){return cgcSmartString::pointer(new cgcSmartString(data));}
inline cgcSmartString::pointer cgcSmartString::create(const cgcSmartString::pointer& data){return cgcSmartString::pointer(new cgcSmartString(data));}
#ifdef CGC_SMART_STRING_USES_BSTR
inline cgcSmartString::pointer cgcSmartString::create(BSTR bstr){return cgcSmartString::pointer(new cgcSmartString(bstr));}
inline cgcSmartString::pointer cgcSmartString::create(BSTR bstr, UINT cp, bool bFreeInputBSTR){return cgcSmartString::pointer(new cgcSmartString(bstr,cp,bFreeInputBSTR));}
#endif
inline cgcSmartString::cgcSmartString(void) {m_sData = new std::string;}
inline cgcSmartString::cgcSmartString(char data) {m_sData = new std::string((std::string::size_type)1,data);}
inline cgcSmartString::cgcSmartString(const char* data) {m_sData = new std::string(data);}
inline cgcSmartString::cgcSmartString(const char* data, size_t count) {m_sData = new std::string(data, count);}
inline cgcSmartString::cgcSmartString(const std::string& data) {m_sData = new std::string(data.c_str());}
inline cgcSmartString::cgcSmartString(const cgcSmartString& data) {m_sData = new std::string(data.c_str());}
inline cgcSmartString::cgcSmartString(const cgcSmartString::pointer& data) {
	if (data.get()==NULL)
		m_sData = new std::string;
	else
		m_sData = new std::string(data->c_str());
}
#ifdef CGC_SMART_STRING_USES_BSTR
inline cgcSmartString::cgcSmartString(BSTR bstr) {
	USES_CONVERSION_EX;
	m_sData = new std::string(BSTR2A_EX(bstr));
}
inline cgcSmartString::cgcSmartString(BSTR bstr, UINT cp, bool bFreeInputBSTR) {
	USES_CONVERSION_EX;
	m_sData = new std::string(W2A_CP_EX(bstr,cp));
	if (bFreeInputBSTR && bstr != NULL)
	{
		::SysFreeString(bstr);
	}
}
#endif

inline cgcSmartString::~cgcSmartString(void){
	if (m_sData)
	{
		m_sData->clear();
		delete m_sData;
		m_sData = NULL;
	}
}

#ifdef CGC_SMART_STRING_USES_BSTR
inline const cgcSmartString& cgcSmartString::operator = (BSTR bstr) {USES_CONVERSION_EX; *m_sData = BSTR2A_EX(bstr); return *this;}
#endif
inline const cgcSmartString& cgcSmartString::operator = (char data) {m_sData->assign((std::string::size_type)1, data); return *this;}
inline const cgcSmartString& cgcSmartString::operator = (const char* data) {*m_sData = data; return *this;}
inline const cgcSmartString& cgcSmartString::operator = (const std::string& data) {*m_sData = data.c_str(); return *this;}
inline const cgcSmartString& cgcSmartString::operator = (const cgcSmartString& data) {*m_sData = data.c_str(); return *this;}
#ifdef CGC_SMART_STRING_USES_BSTR
inline const cgcSmartString& cgcSmartString::operator += (BSTR bstr) {USES_CONVERSION_EX; *m_sData += BSTR2A_EX(bstr); return *this;}
#endif
inline const cgcSmartString& cgcSmartString::operator += (char data) {*m_sData += data; return *this;}
inline const cgcSmartString& cgcSmartString::operator += (const char* data) {*m_sData += data; return *this;}
inline const cgcSmartString& cgcSmartString::operator += (const std::string& data) {*m_sData += data.c_str(); return *this;}
inline const cgcSmartString& cgcSmartString::operator += (const cgcSmartString& data) {*m_sData += data.c_str(); return *this;}
#ifdef CGC_SMART_STRING_USES_BSTR
inline cgcSmartString cgcSmartString::operator + (BSTR bstr) {
	cgcSmartString temp(*m_sData);
	USES_CONVERSION_EX;
	temp += BSTR2A_EX(bstr);
	return temp;
}
#endif
inline cgcSmartString cgcSmartString::operator + (char data) {
	cgcSmartString temp(*m_sData);
	temp.append(data);
	return temp;
}
inline cgcSmartString cgcSmartString::operator + (const char* data) {
	cgcSmartString temp(*m_sData);
	temp += data;
	return temp;
}
inline cgcSmartString cgcSmartString::operator + (const std::string& data) {
	cgcSmartString temp(*m_sData);
	temp += data;
	return temp;
}
inline cgcSmartString cgcSmartString::operator + (const cgcSmartString& data) {
	cgcSmartString temp(*m_sData);
	temp += data;
	return temp;
}

#ifdef CGC_SMART_STRING_USES_BSTR
inline bool cgcSmartString::operator == (BSTR data) const {USES_CONVERSION_EX; return data==NULL?false:(*m_sData==BSTR2A_EX(data));}
#endif
inline bool cgcSmartString::operator == (const char* data) const {return data==NULL?false:(*m_sData==data);}
inline bool cgcSmartString::operator == (const std::string& data) const {return *m_sData==data;}
inline bool cgcSmartString::operator == (const cgcSmartString& data) const {return *m_sData==data.c_str();}
inline bool cgcSmartString::operator != (const char* data) const {return data==NULL?true:(*m_sData!=data);}
inline bool cgcSmartString::operator != (const std::string& data) const {return *m_sData!=data;}
inline bool cgcSmartString::operator != (const cgcSmartString& data) const {return *m_sData!=data.c_str();}
inline bool cgcSmartString::operator > (const char* data) const {return data==NULL?true:(*m_sData>data);}
inline bool cgcSmartString::operator > (const std::string& data) const {return *m_sData>data;}
inline bool cgcSmartString::operator > (const cgcSmartString& data) const {return *m_sData>data.c_str();}
inline bool cgcSmartString::operator >= (const char* data) const {return data==NULL?true:(*m_sData>=data);}
inline bool cgcSmartString::operator >= (const std::string& data) const {return *m_sData>=data;}
inline bool cgcSmartString::operator >= (const cgcSmartString& data) const {return *m_sData>=data.c_str();}
inline bool cgcSmartString::operator < (const char* data) const {return data==NULL?false:(*m_sData<data);}
inline bool cgcSmartString::operator < (const std::string& data) const {return *m_sData<data;}
inline bool cgcSmartString::operator < (const cgcSmartString& data) const {return *m_sData<data.c_str();}
inline bool cgcSmartString::operator <= (const char* data) const {return data==NULL?false:(*m_sData<=data);}
inline bool cgcSmartString::operator <= (const std::string& data) const {return *m_sData<=data;}
inline bool cgcSmartString::operator <= (const cgcSmartString& data) const {return *m_sData<=data.c_str();}

#define USES_CGC_STRING
#ifdef USES_CGC_STRING
#define tstring cgcSmartString
//typedef cgcSmartString tstring;
#endif

}	// namespace mycp


#endif // __mycp_cgcSmartString_head__
