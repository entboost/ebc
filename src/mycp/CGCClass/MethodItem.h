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

// MethodItem.h file here
#ifndef __MethodItem_h__
#define __MethodItem_h__

#include "../ThirdParty/stl/stldef.h"
#include "dlldefine.h"

namespace mycp {

class CGCCLASS_CLASS MethodItem
{
public:
	MethodItem(void);
	MethodItem(long id, const std::string & name, const std::string & method);
	virtual ~MethodItem(void);

public:
	void setId(long newValue) {this->m_id = newValue;}
	long getId(void) const {return this->m_id;}

	void setName(const std::string & newValue) {this->m_name = newValue;}
	const std::string& getName(void) const {return this->m_name;}

	void setMethod(const std::string & newValue) {this->m_method = newValue;}
	const std::string& getMethod(void) const {return this->m_method;}

	virtual void Serialize(bool isStoring, std::fstream& ar);

private:
	long m_id;
	std::string m_name;
	std::string m_method;

};

//typedef	std::list<MethodItem> MethodItemList;
//typedef	MethodItemList::const_iterator MethodItemListIter;
//
//typedef	std::map<std::string, MethodItem*> MethodItemMap;
//typedef	MethodItemMap::const_iterator MethodItemMapIter;
//typedef std::pair<std::string, MethodItem*> MethodItemMapPair;

} // namespace mycp

#endif // __MethodItem_h__
