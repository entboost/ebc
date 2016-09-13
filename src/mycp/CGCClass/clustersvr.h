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

// ClusterSvr.h file here
//#pragma once
#ifndef _CLUSTERSVR_H__
#define _CLUSTERSVR_H__

#include "../dlldefine.h"
#include "../tchar.h"
#include <string>
#include <list>
#include <fstream>

namespace cgc
{

class CGCCLASS_CLASS ClusterSvr
{
public:
	ClusterSvr(void)
		: m_sName(_T(""))
		, m_sHost(_T(""))
		//, m_nPort(0)
		, m_sCode(_T(""))
		, m_nRank(0)
	{}
	ClusterSvr(const tstring & name, const tstring & host, int port, const tstring & code, int rank)
		: m_sName(name)
		, m_sHost(host)
		//, m_nPort(port)
		, m_sCode(code)
		, m_nRank(rank)
	{}
	~ClusterSvr(void)
	{}

	ClusterSvr(const ClusterSvr & v)
		: m_sName(v.getName())
		, m_sHost(v.getHost())
		//, m_nPort(v.getPort())
		, m_sCode(v.getCode())
		, m_nRank(v.getRank())
	{}
	ClusterSvr & operator=(const ClusterSvr & v){
		m_sName = v.getName();
		m_sHost = v.getHost();
		//m_nPort = v.getPort();
		m_sCode = v.getCode();
		m_nRank = v.getRank();
		return *this;
	}

public:
	void setName(const tstring & newValue) {m_sName = newValue;}
	const tstring & getName(void) const {return m_sName;}

	void setHost(const tstring & newValue) {m_sHost = newValue;}
	const tstring & getHost(void) const {return m_sHost;}

//	void setPort(int newv) {m_nPort = newv;}
//	int getPort(void) const {return m_nPort;}

	void setCode(const tstring & newValue) {m_sCode = newValue;}
	const tstring & getCode(void) const {return m_sCode;}

	void setRank(int newValue) {m_nRank = newValue;}
	int getRank(void) const {return m_nRank;}

	virtual void Serialize(bool isStoring, tfstream& ar){
		if (!ar.is_open()) return;

		if (isStoring)
		{
			tstring::size_type len = 0;

			// m_sName
			len = m_sName.length();
			ar.write((const TCHAR*)(&len), sizeof(tstring::size_type));
			ar.write(m_sName.c_str(), len);

			// m_sHost
			len = m_sHost.length();
			ar.write((const TCHAR*)(&len), sizeof(tstring::size_type));
			ar.write(m_sHost.c_str(), len);

			// m_nPort
		//	ar.write((const TCHAR*)(&m_nPort), sizeof(m_nPort));

			// m_sCode
			len = m_sCode.length();
			ar.write((const TCHAR*)(&len), sizeof(tstring::size_type));
			ar.write(m_sCode.c_str(), len);

			// m_nRank
			ar.write((const TCHAR*)(&m_nRank), sizeof(m_nRank));

		}else
		{
			TCHAR * buffer = 0;
			tstring::size_type len = 0;

			// m_sName
			ar.read((TCHAR*)(&len), sizeof(tstring::size_type));
			buffer = new TCHAR[len+1];
			ar.read(buffer, len);
			buffer[len] = '\0';
			m_sName = buffer;
			delete []buffer;

			// m_sHost
			ar.read((TCHAR*)(&len), sizeof(tstring::size_type));
			buffer = new TCHAR[len+1];
			ar.read(buffer, len);
			buffer[len] = '\0';
			m_sHost = buffer;
			delete []buffer;

			// m_nPort
		//	ar.read((TCHAR*)(&m_nPort), sizeof(m_nPort));

			// m_sCode
			ar.read((TCHAR*)(&len), sizeof(tstring::size_type));
			buffer = new TCHAR[len+1];
			ar.read(buffer, len);
			buffer[len] = '\0';
			m_sCode = buffer;

			// m_nRank
			ar.read((TCHAR*)(&m_nRank), sizeof(m_nRank));

		}
	}

private:
	tstring m_sName;
	tstring m_sHost;	// ip:port format
	tstring m_sCode;
	int m_nRank;
	//int m_nPort;
};

typedef std::list<ClusterSvr*> ClusterSvrList;

} // cgc namespace

#endif // _CLUSTERSVR_H__
