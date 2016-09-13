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

// cgcparameter.h file here
#ifndef __cgcparameter_head__
#define __cgcparameter_head__

#include <boost/shared_ptr.hpp>
#include "cgcvalueinfo.h"

namespace mycp {

// cgcParameter
class cgcParameter
	: public cgcValueInfo
{
public:
	typedef boost::shared_ptr<cgcParameter> pointer;

	void setName(const tstring& v) {m_name = v;}
	const tstring& getName(void) const {return m_name;}

	cgcParameter(const tstring& name, ValueType type, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(type, a), m_name(name){}
	cgcParameter(const tstring& name, int v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, bigint v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	//cgcParameter(const tstring& name, time_t v, ValueAttribute a = ATTRIBUTE_BOTH)
	//	: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, bool v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, double v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, const void* v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, const char* v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, const tstring & v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, cgcObject::pointer v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, cgcValueInfo::pointer v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
	cgcParameter(const tstring& name, const std::vector<cgcValueInfo::pointer> & v, ValueAttribute a = ATTRIBUTE_BOTH)
		: cgcValueInfo(v, a), m_name(name){}
private:
	tstring m_name;
};
const cgcParameter::pointer cgcNullParameter;

#define CGC_PARAMETER(n, v) cgcParameter::pointer(new cgcParameter(n, v))

// cgcParameterMap class
class cgcParameterMap
	: public CLockMultiMap<tstring, cgcParameter::pointer>
{
public:
	typedef boost::shared_ptr<cgcParameterMap> pointer;

	cgcParameter::pointer getParameter(const tstring & paramName) const
	{
		cgcParameter::pointer result;
		CLockMultiMap<tstring, cgcParameter::pointer>::find(paramName, result);
		return result;
	}
	tstring getParameterValue(const tstring & sParamName, const char * sDefault = "") const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return sDefault;
		return param->getStrValue();
	}
	tstring getParameterValue(const tstring & sParamName, const tstring& sDefault) const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return sDefault;
		return param->getStrValue();
	}
	int getParameterValue(const tstring & sParamName, int nDefault) const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return nDefault;
		return param->getIntValue();
	}
	bigint getParameterValue(const tstring & sParamName, bigint nDefault) const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return nDefault;
		return param->getBigIntValue();
	}

	bool getParameterValue(const tstring & sParamName, bool bDefault) const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return bDefault;
		return param->getBooleanValue();
	}
	//time_t getParameterValue(const tstring & sParamName, time_t tDefault) const
	//{
	//	cgcParameter::pointer param = getParameter(sParamName);
	//	if (param.get() == NULL) return tDefault;
	//	if (param->getType() != cgcParameter::TYPE_TIME)
	//	{
	//		cgcValueInfo::pointer temp = param->copy();
	//		temp->totype(cgcParameter::TYPE_TIME);
	//		return temp->getTime();
	//	}
	//	return param->getTime();
	//}

	double getParameterValue(const tstring & sParamName, double fDefault) const
	{
		cgcParameter::pointer param = getParameter(sParamName);
		if (param.get() == NULL) return fDefault;
		return param->getFloatValue();
	}

	virtual ~cgcParameterMap(void)
	{
		clear();
	}
};

} // namespace mycp

#endif // __cgcparameter_head__
