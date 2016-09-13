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

// cgcaddress.h file here
#ifndef __cgcaddress_h__
#define __cgcaddress_h__

#include "../ThirdParty/stl/stldef.h"

class CCgcAddress
{
public:
	enum SocketType
	{
		ST_TCP = 1
		, ST_UDP
		, ST_RTP
		, ST_UNKNOWN = 0xf
	};

	CCgcAddress(const CCgcAddress & avp)
	{
		equal(avp);
	}
	const CCgcAddress & operator = (const CCgcAddress & avp)
	{
		equal(avp);
		return *this;
	}

	void address(const tstring & sAddress) {
		const tstring::size_type find = sAddress.find(":");
		if (find != tstring::npos)
		{
			m_ip = sAddress.substr(0, find);
			m_port = atoi(sAddress.substr(find+1).c_str());;
		}
	}
	void address(const tstring & ip, unsigned short port) {
		m_ip = ip;
		m_port = port;
	}
	tstring address(void) const {
		char buffer[32];
		sprintf(buffer, "%s:%d", m_ip.c_str(), m_port);
		return buffer;
	}
	void socketType(SocketType newv) {m_socketType = newv;}
	SocketType socketType(void) const {return m_socketType;}

	const tstring& getip(void) const {return m_ip;}
	unsigned short getport(void) const {return m_port;}

	void reset(void)
	{
		m_ip = _T("127.0.0.1");
		m_port = 8010;
		m_socketType = ST_UNKNOWN;
	}

protected:
	void equal(const CCgcAddress & ca)
	{
		this->m_ip = ca.getip();
		this->m_port = ca.getport();
		this->m_socketType = ca.socketType();
	}

public:
	CCgcAddress(const tstring & sAddress="127.0.0.1:8010", SocketType socketType = ST_UDP)
		: m_port(0), m_socketType(socketType)
	{
		address(sAddress);
	}
	CCgcAddress(const tstring & ip, unsigned short port, SocketType socketType)
		: m_ip(ip), m_port(port),m_socketType(socketType)
	{
		//address(ip, port);
	}
	virtual ~CCgcAddress(void)
	{}
private:
	tstring	m_ip;
	unsigned short m_port;
	SocketType m_socketType;
};

const CCgcAddress constDefaultCgcAddress;

#endif // __cgcaddress_h__
