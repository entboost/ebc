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

// cgcApplication2.h file here
#ifndef __cgcapplication2_head__
#define __cgcapplication2_head__

#define USES_SYNC_VERSION

#include "cgcApplication.h"

#ifdef USES_SYNC_VERSION
#include "../CGCLib/CgcClientHandler.h"
#endif

namespace mycp {

class cgcApplication2
	: public cgcApplication
{
public:
	typedef boost::shared_ptr<cgcApplication2> pointer;
	typedef enum SOTP_CLIENT_SOCKET_TYPE
	{
		SOTP_CLIENT_SOCKET_TYPE_TCP = 1
		, SOTP_CLIENT_SOCKET_TYPE_UDP
	};

	// io service
	virtual mycp::asio::IoService::pointer getIoService(bool bCreateAndStart = true) = 0;
	virtual void resetIoService(bool bStopIoService = true) = 0;

	virtual int sendSyncData(const tstring& sSyncName, int nDataType, const char* sDataString, size_t nDataSize, bool bBackup) =0;
	virtual int sendSyncFile(const tstring& sSyncName, int nDataType, const tstring& sFileName, const tstring& sFilePath, bool bBackup) = 0;
	// sAddress: format=ip:port
#ifdef USES_SYNC_VERSION
	virtual DoSotpClientHandler::pointer getSotpClientHandler(const tstring& sAddress, const tstring& sAppName, SOTP_CLIENT_SOCKET_TYPE nSocketType=SOTP_CLIENT_SOCKET_TYPE_UDP, bool bUserSsl=false, bool bKeepAliveSession=false, int bindPort=0, int nThreadStackSize=200) =0;
#endif

};

//cgc::cgcApplication2::pointer theApplication2 = CGC_APPLICATION2_CAST(theApplication);
inline boost::shared_ptr<cgcApplication2> CGC_APPLICATION2_CAST(boost::shared_ptr<cgcApplication> const & r)
{
	return boost::static_pointer_cast<cgcApplication2, cgcApplication>(r);
}
//template<typename T> boost::shared_ptr<T> CGC_APPLICATION2_CAST(boost::shared_ptr<cgcApplication> const & r)
//{
//	return boost::static_pointer_cast<T, cgcApplication>(r);
//}

} // namespace mycp

#endif // __CGCAPPLICATION_HEAD__
