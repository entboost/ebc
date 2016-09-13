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

#ifdef WIN32
#pragma warning(disable:4267 4819 4996)
#include <winsock2.h>
#endif // WIN32

//#define libSOTPCLIENT_EXPORTS

#include "IncludeBase.h"
#include "SotpClient.h"
#include "../ThirdParty/stl/lockmap.h"
#include "CgcTcpClient.h"
#include "CgcUdpClient.h"
#include "CgcRtpClient.h"

namespace mycp {

//CLockMap<DoSotpClientHandler*, CgcBaseClient::pointer> theSotpClientList;

CSotpClient::CSotpClient(void)
: m_bExitStopIoService(true)
, m_nUserData(0)
{

}
CSotpClient::~CSotpClient(void)
{
	stopAllClient();
}

DoSotpClientHandler::pointer CSotpClient::startClient(const CCgcAddress & pAddress, unsigned int bindPort, int nThreadStackSize)
{
	CgcBaseClient::pointer cgcClient;
	switch (pAddress.socketType())
	{
	case CCgcAddress::ST_TCP:
		cgcClient = mycp::CgcTcpClient::create();
		break;
	case CCgcAddress::ST_UDP:
		cgcClient = mycp::CgcUdpClient::create();
		break;
	case CCgcAddress::ST_RTP:
#if (USES_RTP)
		cgcClient = CgcRtpClient::create();
#else
		return cgcClient;
#endif // USES_RTP
		break;
	default:
		return cgcClient;
	}
	if (m_pIoService.get()!=NULL)
	{
		((DoSotpClientHandler*)cgcClient.get())->doSetIoService(m_pIoService,m_bExitStopIoService);
	}
	if (cgcClient->StartClient(pAddress.address(), bindPort, nThreadStackSize) != 0)
	{
		cgcClient.reset();
		return cgcClient;
	}
	DoSotpClientHandler * handler = (DoSotpClientHandler*)cgcClient.get();
	theSotpClientList.insert(handler, cgcClient);
	return cgcClient;
}

void CSotpClient::stopClient(DoSotpClientHandler::pointer pDoHandler)
{
	CgcBaseClient::pointer cgcClient;
	if (theSotpClientList.find(pDoHandler.get(), cgcClient, true))
	{
		//pDoHandler->doSetResponseHandler(NULL);
		cgcClient->StopClient();
	}
}

void CSotpClient::stopAllClient(void)
{
	AUTO_LOCK(theSotpClientList);
	//for_each(theSotpClientList.begin(), theSotpClientList.end(),
	//	boost::bind(&CgcBaseClient::StopClient, boost::bind(&std::map<unsigned long, CgcBaseClient::pointer>::value_type::second,_1)));
	CLockMap<DoSotpClientHandler*, CgcBaseClient::pointer>::iterator pIter;
	for (pIter=theSotpClientList.begin(); pIter!=theSotpClientList.end(); pIter++)
	{
		pIter->second->StopClient();
	}
	theSotpClientList.clear(false);
	//m_pIoService.reset();
}

} // namespace mycp
