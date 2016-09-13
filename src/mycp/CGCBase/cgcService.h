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

// cgcService.h file here
#ifndef __cgcService_head__
#define __cgcService_head__

#include <boost/shared_ptr.hpp>
#include "cgcdlldefine.h"
#include "cgcvalueinfo.h"
#include "cgcAttributes.h"

namespace mycp {

class cgcServiceCall
	: public cgcObject
{
public:
	typedef boost::shared_ptr<cgcServiceCall> pointer;

	virtual bool callService(int function, const cgcValueInfo::pointer& inParam = cgcNullValueInfo, cgcValueInfo::pointer outParam = cgcNullValueInfo) {return false;}
	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam = cgcNullValueInfo, cgcValueInfo::pointer outParam = cgcNullValueInfo) {return false;}
};

class cgcServiceInterface
	: public cgcServiceCall
{
public:
	typedef boost::shared_ptr<cgcServiceInterface> pointer;

	virtual tstring serviceName(void) const {return _T("");}
	virtual bool initService(cgcValueInfo::pointer parameter = cgcNullValueInfo) {m_bServiceInited = true; return true;}
	virtual void finalService(void) {clearCallback();m_bServiceInited = false;}
	bool isServiceInited(void) const {return m_bServiceInited;}
	
	virtual cgcAttributes::pointer getAttributes(void) const {return cgcNullAttributes;}
	const cgcValueInfo::pointer& getServiceInfo(void) const {return m_serviceInfo;}

	void clearCallback(void) {m_callback1 = (cgcServiceInterface*)0; m_callback2.reset();}
	void setCallback1(cgcServiceCall * callback) {m_callback1 = callback;}
	void setCallback2(cgcServiceCall::pointer callback) {m_callback2 = callback;}

	cgcServiceInterface(void)
		: m_bServiceInited(false), m_callback1((cgcServiceInterface*)0)
	{
		m_serviceInfo = CGC_VALUEINFO((int)0);
	}
protected:
	bool m_bServiceInited;
	cgcValueInfo::pointer m_serviceInfo;
	cgcServiceCall * m_callback1;			// default m_callback1
	cgcServiceCall::pointer m_callback2;	// or m_callback2
};

const cgcServiceInterface::pointer cgcNullServiceInterface;

} // namespace mycp

#endif // __cgcService_head__
