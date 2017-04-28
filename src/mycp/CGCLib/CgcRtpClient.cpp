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
#pragma warning(disable:4267 4312 4819 4996)
#endif // WIN32

#include "CgcRtpClient.h"
#include <fstream>

#if (USES_RTP)
namespace cgc
{

CgcRtpClient::CgcRtpClient(void)
: CgcBaseClient(_T("RTP"))

{
}

CgcRtpClient::~CgcRtpClient(void)
{
	StopClient();
}

int CgcRtpClient::startClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize)
{
	if (m_pDoRtpHandler.get() == NULL)
	{
		std::vector<std::string> pList;
		if (CgcBaseClient::ParseString(sCgcServerAddr.c_str(),":",pList)!=2)
		{
			return -1;
		}
		const std::string sIp = CgcBaseClient::GetHostIp(pList[0].c_str());
		unsigned short nPort = atoi(pList[1].c_str());
		char lpszBuffer[50];
		sprintf(lpszBuffer,"%s:%d",sIp.c_str(),nPort);
		m_ipRemote = CCgcAddress(lpszBuffer, CCgcAddress::ST_RTP);
		if (bindPort == 0)
		{
			for (int i=0; i<10; i++)
			{
				m_pDoRtpHandler = m_rtp.startRtp(8028+i*2);
				if (m_pDoRtpHandler.get() != NULL)
					break;
			}
		}else
		{
			m_pDoRtpHandler = m_rtp.startRtp(bindPort);
		}

		if (m_pDoRtpHandler.get() == NULL) return -1;

		//tstring sIp = m_ipRemote.getip();
		//unsigned int nPort = (unsigned int)m_ipRemote.getport();
		m_pDoRtpHandler->doAddDest(sIp.c_str(), nPort);
		m_pDoRtpHandler->doSetRtpHandler(this);
	}

	return 0;
}

void CgcRtpClient::stopClient(void)
{
	if (m_pDoRtpHandler.get() != NULL)
	{
		DoRtpHandler::pointer pDoRtpHandlerTemp = m_pDoRtpHandler;
		m_pDoRtpHandler.reset();
		m_rtp.stopRtp(pDoRtpHandlerTemp);
	}
}

bool CgcRtpClient::isInvalidate(void) const
{
	return m_pDoRtpHandler.get() == NULL;
}

//void CgcRtpClient::setRemoteAddr(unsigned long destIp, unsigned int destPort)
//{
//	// ???
//
//	m_rtpClient.CleaerDest();
//	m_rtpClient.AddDest(m_destIp, m_destPort);
//
//}

bool CgcRtpClient::setRemoteAddr(const tstring & sRemoteAddr)
{
	if (isInvalidate()) return false;
	std::vector<std::string> pList;
	if (CgcBaseClient::ParseString(sRemoteAddr.c_str(),":",pList)==2)
	{
		const std::string sIp = CgcBaseClient::GetHostIp(pList[0].c_str());
		unsigned short nPort = atoi(pList[1].c_str());
		char lpszBuffer[50];
		sprintf(lpszBuffer,"%s:%d",sIp.c_str(),nPort);
		m_ipRemote = CCgcAddress(lpszBuffer, CCgcAddress::ST_RTP);
		//tstring sIp = m_ipRemote.getip();
		//unsigned int nPort = (unsigned int)m_ipRemote.getport();
		m_pDoRtpHandler->doClearDest();
		m_pDoRtpHandler->doAddDest(sIp.c_str(), nPort);
		return true;
	}
	return false;
}

void CgcRtpClient::setMediaType(unsigned short mediatype)
{
	if (isInvalidate()) return;

	m_pDoRtpHandler->doSetMediaType(mediatype);
}

void CgcRtpClient::onReceiveEvent(CRTPData::pointer receiveData, DoRtpHandler::pointer pDoRtpHandler, void * param)
{
	BOOST_ASSERT(receiveData.get() != NULL);

	m_tSendRecv = time(0);
	//m_destIp = destIp;
	//m_destPort = destPort;

	CCgcData::pointer cgcData = CCgcData::create();
	long size = receiveData->size();
	cgcData->putdata(receiveData->data(size), size);
	cgcData->attach((void*)receiveData->timestamp());
	this->parseData(cgcData,0);
}

size_t CgcRtpClient::sendData(const unsigned char * data, size_t size)
{
	if (data == 0 || isInvalidate()) return 0;

	m_tSendRecv = time(0);
	return m_pDoRtpHandler->doSendData(data, size, 0);
}

size_t CgcRtpClient::sendData(const unsigned char * data, size_t size, unsigned int timestamp)
{
	if (data == 0 || isInvalidate()) return 0;

	m_tSendRecv = time(0);
	return m_pDoRtpHandler->doSendData(data, size, timestamp);
}

} // namespace cgc
#endif // USES_RTP
