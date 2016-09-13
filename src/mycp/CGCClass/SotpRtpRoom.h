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

// SotpRtpRoom.h file here
#ifndef __SotpRtpRoom_h__
#define __SotpRtpRoom_h__

#include "../ThirdParty/stl/lockmap.h"
#include "SotpRtpSource.h"

namespace mycp {

//typedef void (FAR *SinkExpireCallback) (void* pUserData, void* pRtpRoom,void* pRtpSourc);
class CSotpRtpCallback
{
public:
	virtual bool onRegisterSource(bigint nRoomId, bigint nSourceId, bigint nParam, void* pUserData) = 0;
	virtual void onUnRegisterSource(bigint nRoomId, bigint nSourceId, bigint nParam, void* pUserData) = 0;
	virtual bool onRegisterSink(bigint nRoomId, bigint nSourceId, bigint nDestId, void* pUserData) = 0;
};

class CSotpRtpRoom
{
public:
	typedef boost::shared_ptr<CSotpRtpRoom> pointer;
	static CSotpRtpRoom::pointer create(bool bServerMode, bigint nRoomId)
	{
		return CSotpRtpRoom::pointer(new CSotpRtpRoom(bServerMode, nRoomId));
	}
	bool IsServerMode(void) const {return m_bServerMode;}
	bigint GetRoomId(void) const {return m_nRoomId;}
	bool IsEmpty(void) const {return m_pSourceList.empty();}

	CSotpRtpSource::pointer RegisterSource(bigint nSrcId, bigint nParam, const cgcRemote::pointer& pcgcRemote, CSotpRtpCallback* pCallback, void* pUserData);
	bool UnRegisterSource1(bigint nSrcId, bigint* pOutParam);	// for client
	bool UnRegisterSource2(bigint nSrcId, bigint nParam, CSotpRtpCallback* pCallback, void* pUserData);		// for server
	bool RegisterSink(bigint nSrcId,bigint nDestId,const cgcRemote::pointer& pcgcRemote, CSotpRtpCallback* pCallback, void* pUserData);
	bool UnRegisterSink(bigint nSrcId,bigint nDestId,const cgcRemote::pointer& pcgcRemote);
	bool UnRegisterAllSink(bigint nSrcId, const cgcRemote::pointer& pcgcRemote);
	bool UnRegisterAllSink(const CSotpRtpSource::pointer& pRtpSrcSource, const cgcRemote::pointer& pcgcRemote, bool bUnRegisterAllSinkForce=false);

	CSotpRtpSource::pointer GetRtpSource(bigint nSrcId) const;
	bool IsRegisterSource(bigint nSrcId) const;
	void BroadcastRtpData(const tagSotpRtpDataHead& pRtpDataHead,const cgcAttachment::pointer& pAttackment) const;
	void CheckRegisterSourceLive(time_t tNow,short nExpireSecond, CSotpRtpCallback* pCallback=NULL,void* pUserData=NULL);
	void CheckRegisterSinkLive(time_t tNow,short nExpireSecond,bigint nSrcId, const cgcRemote::pointer& pcgcRemote);	// for client

	void ClearAll(void);

	CSotpRtpRoom(bool bServerMode, bigint nRoomId);
	virtual ~CSotpRtpRoom(void);
private:
	bool m_bServerMode;
	bigint m_nRoomId;
	CLockMap<bigint,CSotpRtpSource::pointer> m_pSourceList;
};

} // namespace mycp

#endif // __SotpRtpRoom_h__
