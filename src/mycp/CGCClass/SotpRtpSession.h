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

// SotpRtpSession.h file here
#ifndef __SotpRtpSession_h__
#define __SotpRtpSession_h__

#include "../CGCBase/cgcrtpobject.h"
#include "../ThirdParty/stl/lockmap.h"
#include "SotpRtpRoom.h"
//#include "SotpRtpSink.h"

namespace mycp {

class CSotpRtpSession
{
public:
	typedef boost::shared_ptr<CSotpRtpSession> pointer;
	static CSotpRtpSession::pointer create(bool bServerMode)
	{
		return CSotpRtpSession::pointer(new CSotpRtpSession(bServerMode));
	}
	// for server
	//void SetSotpRtpCallback(CSotpRtpCallback* pCallback) {m_pSotpRtpCallback = pCallback;}
	// for client
	void SetRtpFrameCallback(HSotpRtpFrameCallback pCallback, void * pUserData) {m_pRtpFrameCallback = pCallback;m_nCbUserData=pUserData;}
	//void SetCbUserData(void* nUserData) {m_nCbUserData = nUserData;}
	bool doRtpCommand(const tagSotpRtpCommand& pRtpCommand, const cgcRemote::pointer& pcgcRemote,bool bSendRtpCommand,CSotpRtpCallback* pCallback=NULL,void* pUserData=NULL);
	void UnRegisterAllRoomSink(bigint nSrcId);	// for client
	bool doRtpData(const tagSotpRtpDataHead& pRtpDataHead,const cgcAttachment::pointer& pAttackment, const cgcRemote::pointer& pcgcRemote);

	void CheckRegisterSourceLive(short nExpireSecond, CSotpRtpCallback* pCallback=NULL,void* pUserData=NULL);	// for server
	void CheckRegisterSinkLive(short nExpireSecond, bigint nSrcId, const cgcRemote::pointer& pcgcRemote);	// for client

	void GetRoomIdList(std::vector<bigint>& pOutRoomIdList) const;
	bool IsRegisterSource(bigint nRoomId, bigint nSrcId) const;
	bool IsRegisterSink(bigint nRoomId, bigint nSrcId, bigint nDestId) const;

	bool RegisterSource(bigint nRoomId, bigint nSrcId, bigint nParam, const cgcRemote::pointer& pcgcRemote, CSotpRtpCallback* pCallback=NULL,void* pUserData=NULL);
	CSotpRtpRoom::pointer GetRtpRoom(bigint nRoomId,bool bCreateNew);
	CSotpRtpRoom::pointer GetRtpRoom(bigint nRoomId) const;

	void ClearAll(void);
	bool IsRoomEmpty(void) const;

	CSotpRtpSession(bool bServerMode);
	virtual ~CSotpRtpSession(void);
private:
	bool m_bServerMode;
	CSotpRtpMsgPool m_pRtpMsgPool;	// for server
	//CSotpRtpCallback* m_pSotpRtpCallback;
	HSotpRtpFrameCallback m_pRtpFrameCallback;
	void* m_nCbUserData;
	CLockMap<bigint,CSotpRtpRoom::pointer> m_pRoomList;
};

} // namespace mycp

#endif // __SotpRtpSession_h__
