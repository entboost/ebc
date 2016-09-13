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

// IpFormat.h file here
#ifndef __IpFormat_h__
#define __IpFormat_h__

#include <string>
#include <boost/format.hpp>

class IpFormat
{
private:
	std::string m_ip;
	unsigned int m_port;

public:
	std::string ip(void) const {return m_ip;}
	unsigned int port(void) const {return m_port;}
	std::string to_string(void) const
	{
		boost::format formatIp(_T("%s:%d"));
		return std::string((formatIp%m_ip.c_str()%m_port).str());
	}

	void reset(void)
	{
		m_ip = _T("127.0.0.1");
		m_port = 0;
	}
public:
	IpFormat(const std::string & ip, unsigned int port)
	{
		m_ip = ip;
		m_port = port;
	}
	IpFormat(void)
		: m_ip(_T("127.0.0.1"))
		, m_port(0)
	{
	}
	~IpFormat(void)
	{
	}
};

#endif // __IpFormat_h__
