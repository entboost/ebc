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

// cgcfunc.h file here
#ifndef __cgcfunc_head__
#define __cgcfunc_head__

namespace mycp {

// ******** App function ********
// CGC_Module_Init(): Before the module start to load this mothod.
// extern "C" bool CGC_API CGC_Module_Init(void) {}
typedef bool (FAR *FPCGC_Module_Init)(void);
typedef bool (FAR *FPCGC_Module_Init2)(MODULE_INIT_TYPE nInitType);

// Before the module exit to load this mothod.
// extern "C" void CGC_API CGC_Module_Free(void) {}
typedef void (FAR *FPCGC_Module_Free)(void);
typedef void (FAR *FPCGC_Module_Free2)(MODULE_FREE_TYPE nFreeType);

// extern "C" void CGC_API CGC_Module_AutoUpdateData(const cgcAttributes::pointer& pOldAttributes) {}
typedef void (FAR *FPCGC_Module_AutoUpdateData)(const cgcAttributes::pointer& pOldAttributes);

// extern "C" void CGC_API CGC_Module_ResetService(const tstring& sServiceName) {}
typedef void (FAR *FPCGC_Module_ResetService)(const tstring& sServiceName);

// CGC_Session_Open
// extern "C" bool CGC_API CGC_Session_Open(const cgcSession::pointer& pHandler) {}
typedef bool (FAR *FPCGC_Session_Open)(const cgcSession::pointer& pHandler);

// CGC_Session_Close
// extern "C" void CGC_API CGC_Session_Close(const cgcSession::pointer& pHandler) {}
typedef void (FAR *FPCGC_Session_Close)(const cgcSession::pointer& pHandler);

// CGC_Remote_Close
// extern "C" void CGC_API CGC_Remote_Close(const cgcSession::pointer& pHandler,unsigned long nRemoteId) {}
typedef void (FAR *FPCGC_Remote_Close)(const cgcSession::pointer& pHandler,unsigned long nRemoteId);

typedef void (FAR *FPCGC_Remote_Change)(const cgcSession::pointer& pHandler,const cgcRemote::pointer& pcgcRemote);

// extern "C" int CGC_API YourFuncName(const cgcSotpRequest::pointer & request, cgcSotpResponse::pointer response){}
typedef int (FAR *FPCGCApi)(const cgcSotpRequest::pointer & request, const cgcSotpResponse::pointer& response);

typedef int (FAR *FPCGC_SYNC)(const tstring& sSyncName, int nSyncType, const tstring& sSyncData);

// extern "C" HTTP_STATUSCODE CGC_API doGET(const cgcHttpRequest::pointer & request, cgcHttpResponse::pointer response){}
typedef HTTP_STATUSCODE (FAR *FPCGCHttpApi)(const cgcHttpRequest::pointer & request, const cgcHttpResponse::pointer& response);

// ******** Service function ********
// extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer& outService, const cgcValueInfo::pointer& parameter) {}
typedef void (FAR *FPCGC_GetService)(cgcServiceInterface::pointer& outService, const cgcValueInfo::pointer& parameter);
// extern "C" void CGC_API CGC_ResetService(cgcServiceInterface::pointer inService) {}
typedef void (FAR *FPCGC_ResetService)(const cgcServiceInterface::pointer & inService);

// ******** System function ********
typedef void (FAR *FPCGC_SetApplicationHandler)(const cgcApplication::pointer& pHandler);
typedef void (FAR *FPCGC_SetSystemHandler)(const cgcSystem::pointer& pHandler);
typedef void (FAR *FPCGC_SetServiceManagerHandler)(const cgcServiceManager::pointer& pHandler);
//typedef void (FAR *FPCGC_SetCommunicationsHandler)(cgcCommunications::pointer pHandler);

// ******** sotp rtp function ********
// extern "C" bool CGC_API CGC_Rtp_Register_Source(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam) {}
typedef bool (FAR *FPCGC_Rtp_Register_Source)(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam);

// extern "C" bool CGC_API CGC_Rtp_UnRegister_Source(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam) {}
typedef bool (FAR *FPCGC_Rtp_UnRegister_Source)(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam);

// extern "C" bool CGC_API CGC_Rtp_Register_Sink(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nDestId) {}
typedef bool (FAR *FPCGC_Rtp_Register_Sink)(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nDestId);

const tstring theGetSotpClientHandlerApiName("CGC_GetSotpClientHandler");
// extern "C" bool CGC_API CGC_GetSotpClientHandler(DoSotpClientHandler::pointer& pOutSotpClientHandler, const cgcValueInfo::pointer& parameter) {}
typedef bool (FAR *FPCGC_GetSotpClientHandler)(DoSotpClientHandler::pointer& pOutSotpClientHandler, const cgcValueInfo::pointer& parameter);

} // namespace mycp

#endif // __cgcfunc_head__
