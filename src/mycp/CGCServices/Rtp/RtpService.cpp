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
#include <winsock2.h>
#include <windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // WIN32

#include <boost/shared_ptr.hpp>
#include <CGCBase/app.h>
#include "cgcRtp.h"
using namespace cgc;

#if (USES_RTP)
#include <ThirdParty/libRTP/libRTP.h>

class CRtpService
	: public cgcRtp
{
public:
	typedef boost::shared_ptr<CRtpService> pointer;

	static CRtpService::pointer create(void)
	{
		return CRtpService::pointer(new CRtpService());
	}

	CRtpService(void)
	{
	}
	~CRtpService(void)
	{
		m_rtp.stopAll();
	}

	virtual tstring serviceName(void) const {return _T("RTPSERVICE");}

private:
	CRtp m_rtp;

protected:
	virtual DoRtpHandler::pointer startRtp(unsigned short rtpPort) {return m_rtp.startRtp(rtpPort);}
	virtual void stopRtp(DoRtpHandler::pointer handler) {m_rtp.stopRtp(handler);}
};

CRtpService::pointer gRtpServicePointer;

// 模块初始化函数，可选实现函数
extern "C" bool CGC_API CGC_Module_Init(void)
{
#ifdef WIN32
	WSADATA wsaData;
	int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( err != 0 ) {
		return false;
	}
#endif // WIN32
	return true;
}

// 模块退出函数，可选实现函数
extern "C" void CGC_API CGC_Module_Free(void)
{
	gRtpServicePointer.reset();
#ifdef WIN32
	WSACleanup();
#endif // WIN32
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (gRtpServicePointer.get() == NULL)
	{
		gRtpServicePointer = CRtpService::pointer(new CRtpService());
	}
	outService = gRtpServicePointer;
}

#endif // USES_RTP
