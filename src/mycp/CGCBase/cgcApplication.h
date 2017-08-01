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

// cgcApplication.h file here
#ifndef __cgcapplication_head__
#define __cgcapplication_head__

//#define USES_SYNC_VERSION

#include "cgcparameter.h"
#include "cgcattachment.h"
#include "cgcLock.h"
#include "cgcdlldefine.h"
#include "cgcRequest.h"
#include "cgcResponse.h"
#include "cgcSession.h"
#include "cgcLogService.h"
//#ifdef USES_SYNC_VERSION
//#include "../CGCLib/CgcClientHandler.h"
//#endif

namespace mycp {

class cgcOnTimerHandler
	: public cgcLock
{
public:
	typedef boost::shared_ptr<cgcOnTimerHandler> pointer;

	virtual bool IsThreadSafe(void) const {return false;}

        virtual void OnTimeout(unsigned int /*nIDEvent*/, const void * /*pvParam*/) = 0;
        virtual void OnTimerExit(unsigned int /*nIDEvent*/, const void * /*pvParam*/) {}
};

const cgcOnTimerHandler::pointer cgcNullOnTimerHandler;

class cgcApplication
	//: public cgcAttributes
{
public:
	typedef boost::shared_ptr<cgcApplication> pointer;

	// Setting in [CGCP_PATH]/conf/[APPNAME]/params.xml file.
	virtual bool isInited(void) const =0;
	virtual long getMajorVersion(void) const =0;
	virtual long getMinorVersion(void) const =0;
	virtual cgcParameterMap::pointer getInitParameters(void) const = 0;

	virtual unsigned long getApplicationId(void) const = 0;
	virtual tstring getApplicationName(void) const = 0;
	virtual tstring getApplicationFile(void) const = 0;
	virtual tstring getApplicationType(void) const = 0;
	virtual MODULETYPE getModuleType(void) const = 0;

	// Return [CGCP_PATH]/conf/[APPNAME] value.
	virtual tstring getAppConfPath(void) const =0;
	
	// Log parameters(path, file ...) setting in [LOGSERVICE_PATH]/[APPNAME].xml file.
	virtual cgcLogService::pointer logService(void) const = 0;
	virtual void logService(cgcLogService::pointer logService) = 0;
	virtual void log(LogLevel level, const char* format,...) = 0;
	virtual void log(LogLevel level, const wchar_t* format,...) = 0;
	virtual void log2(LogLevel level, const char* sData) = 0;
	virtual void log2(LogLevel level, const wchar_t* sData) = 0;

	// The SetTimer method sets a timer on the clock.
	virtual unsigned int SetTimer(unsigned int nIDEvent, unsigned int nElapse, cgcOnTimerHandler::pointer handler, bool bOneShot = false, const void * pvParam = NULL, int nThreadStackSize=0) = 0;
	// The KillTimer method destroys the specified timer.
	virtual void KillTimer(unsigned int nIDEvent) = 0;
	// The KillAllTimer method destroys all timers.
	virtual void KillAllTimer(void) = 0;

	virtual cgcAttributes::pointer getAttributes(bool create) = 0;
	virtual cgcAttributes::pointer getAttributes(void) const = 0;
	virtual cgcAttributes::pointer createAttributes(void) const = 0;

	virtual tstring getExecuteResult(void) const =0;
	virtual void clearExecuteResult(void) {setExecuteResult("");}
	virtual void setExecuteResult(const tstring & executeResult) =0;
//
//	// io service
//	virtual IoService::pointer getIoService(bool bCreateAndStart = true) = 0;
//	virtual void resetIoService(bool bStopIoService = true) = 0;
//
//	virtual int sendSyncData(const tstring& sSyncName, int nDataType, const char* sDataString, size_t nDataSize, bool bBackup) =0;
//	virtual int sendSyncFile(const tstring& sSyncName, int nDataType, const tstring& sFileName, const tstring& sFilePath, bool bBackup) = 0;
//	// nSocketType: 1=tcp; 2=udp
//	// sAddress: format=ip:port
//#ifdef USES_SYNC_VERSION
//	virtual DoSotpClientHandler::pointer getSotpClientHandler(int nSocketType, const tstring& sAddress, const tstring& sAppName, int bindPort=0, int nThreadStackSize=200) =0;
//#endif

};

} // namespace mycp

#endif // __CGCAPPLICATION_HEAD__
