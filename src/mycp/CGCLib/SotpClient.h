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

// SotpClient.h file here
#ifndef __SotpClient_h__
#define __SotpClient_h__

#include "dlldefine.h"
#include "CgcClientHandler.h"
#include "../CGCBase/cgcaddress.h"
#include "../ThirdParty/stl/lockmap.h"
#include "CgcBaseClient.h"

namespace mycp {

class CSotpClient
//class LIBSOTPCLIENT_CLASS CSotpClient
{
public:
	DoSotpClientHandler::pointer startClient(const CCgcAddress & address, unsigned int bindPort=0, int nThreadStackSize=200);
	void stopClient(DoSotpClientHandler::pointer pDoHandler);
	void stopAllClient(void);

	void SetIoService(mycp::asio::IoService::pointer pIoService, bool bExitStop = false) {m_pIoService = pIoService; m_bExitStopIoService=bExitStop;}
	void ResetIoService(void) {m_pIoService.reset();}
	mycp::asio::IoService::pointer GetIoService(void) const {return m_pIoService;}

	void SetUserData(unsigned int v) {m_nUserData = v;}
	unsigned int GetUserData(void) const {return m_nUserData;}

public:
	CSotpClient(void);
	virtual ~CSotpClient(void);

private:
	bool m_bExitStopIoService;
	mycp::asio::IoService::pointer m_pIoService;
	CLockMap<DoSotpClientHandler*, CgcBaseClient::pointer> theSotpClientList;
	unsigned int m_nUserData;
};

} // namespace mycp

#endif // __SotpClient_h__
