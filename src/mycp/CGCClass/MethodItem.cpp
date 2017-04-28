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

#ifdef _MSC_VER //WIN32
#pragma warning(disable:4267)
#endif // WIN32

#include "CGCClass.h"
#include "MethodItem.h"

namespace mycp {

MethodItem::MethodItem(void)
: m_id(0), m_name(_T("")), m_method(_T(""))
{}
MethodItem::MethodItem(long id, const std::string & name, const std::string & method)
: m_id(id), m_name(name), m_method(method)
{}
MethodItem::~MethodItem(void)
{}

/* ? SOTP/2.0
std::string getString(void) const{
boost::format fMethod("<method id=\"%d\" name=\"%s\" />");
std::string result((fMethod%m_id%m_name.c_str()).str());
return result;
}
*/

void MethodItem::Serialize(bool isStoring, std::fstream& ar)
{
	if (!ar.is_open()) return;

	if (isStoring)
	{
		std::string::size_type len = 0;

		// m_id
		ar.write((const char*)(&m_id), sizeof(m_id));

		// m_name
		len = m_name.length();
		ar.write((const char*)(&len), sizeof(std::string::size_type));
		ar.write(m_name.c_str(), len);

		// m_method
		len = m_method.length();
		ar.write((const char*)(&len), sizeof(std::string::size_type));
		ar.write(m_method.c_str(), len);

	}else
	{
		char * buffer = 0;
		std::string::size_type len = 0;

		// m_id
		ar.read((char*)(&m_id), sizeof(m_id));

		// m_name
		ar.read((char*)(&len), sizeof(std::string::size_type));
		buffer = new char[len+1];
		ar.read(buffer, len);
		buffer[len] = '\0';
		m_name = buffer;
		delete []buffer;

		// m_method
		ar.read((char*)(&len), sizeof(std::string::size_type));
		buffer = new char[len+1];
		ar.read(buffer, len);
		buffer[len] = '\0';
		m_method = buffer;
		delete []buffer;

	}
}

} // namespace mycp
