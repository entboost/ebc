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
#endif // WIN32

#include "../CGCBase/cgcdef.h"
#include "../CGCClass/SotpCallTable2.h"
#include "SotpRtpRoom.h"

namespace mycp {

CSotpRtpRoom::CSotpRtpRoom(bool bServerMode,bigint nRoomId)
: m_bServerMode(bServerMode), m_nRoomId(nRoomId)

{
	//printf("**** CSotpRtpRoom() roomid=%lld\n",m_nRoomId);
}
CSotpRtpRoom::~CSotpRtpRoom(void)
{
	//printf("**** ~CSotpRtpRoom() roomid=%lld\n",m_nRoomId);
	ClearAll();
}

void CSotpRtpRoom::ClearAll(void)
{
	m_pSourceList.clear();
}

CSotpRtpSource::pointer CSotpRtpRoom::RegisterSource(bigint nSrcId, bigint nParam, const cgcRemote::pointer& pcgcRemote, CSotpRtpCallback* pCallback, void* pUserData)
{
	if (nSrcId==0)
		return NullSotpRtpSource;

	CSotpRtpSource::pointer pRtpSource;
	if (!m_pSourceList.find(nSrcId,pRtpSource))
	{
		if (m_bServerMode && pCallback!=NULL)
		{
			if (!pCallback->onRegisterSource(this->GetRoomId(), nSrcId, nParam, pUserData))
				return NullSotpRtpSource;
		}
		pRtpSource = CSotpRtpSource::create(m_bServerMode,m_nRoomId,nSrcId,nParam);
		CSotpRtpSource::pointer pRtpSourceTemp;
		m_pSourceList.insert(nSrcId,pRtpSource,false,&pRtpSourceTemp);
		if (pRtpSourceTemp.get()!=NULL)
		{
			pRtpSource = pRtpSourceTemp;
		}
		pRtpSource->SetRemote(pcgcRemote);
		pRtpSource->SetCallbackUserData(pUserData);
		if (m_bServerMode && pCallback!=NULL)	// *
			pRtpSource->SetLastCallbackTime();
	}else
	{
		if (m_bServerMode && pCallback!=NULL)
		{
			if (pcgcRemote.get()==NULL)
				return NullSotpRtpSource;
			if (pRtpSource->GetRemoteId()!=pcgcRemote->getRemoteId() || (pRtpSource->GetLastCallbackTime()+600)<time(0))
			{
				// remote id 不同（或者超过10分钟），需要验证一次；
				if (!pCallback->onRegisterSource(this->GetRoomId(), nSrcId, nParam, pUserData))
					return NullSotpRtpSource;
				if (pRtpSource->GetRemoteId()!=pcgcRemote->getRemoteId())
					pRtpSource->SetRemote(pcgcRemote);
				pRtpSource->SetLastCallbackTime();
			}else if (pRtpSource->isRemoteInvalidate())
			{
				// 更新remoteid
				pRtpSource->SetRemote(pcgcRemote);
			}
		}
		pRtpSource->SetParam(nParam);
		pRtpSource->SetLastTime();
	}
	return pRtpSource;
}
bool CSotpRtpRoom::UnRegisterSource1(bigint nSrcId, bigint* pOutParam)
{
	if (!this->m_bServerMode)
	{
		UnRegisterAllSink(nSrcId, NullcgcRemote);

		if (pOutParam==NULL)
			return m_pSourceList.remove(nSrcId);
		CSotpRtpSource::pointer pRtpSource;
		if (!m_pSourceList.find(nSrcId,pRtpSource,true))
		{
			return false;
		}
		*pOutParam = pRtpSource->GetParam();
		return true;
	}
	return false;
}
bool CSotpRtpRoom::UnRegisterSource2(bigint nSrcId, bigint nParam, CSotpRtpCallback* pCallback, void* pUserData)
{
	if (this->m_bServerMode)
	{
		CSotpRtpSource::pointer pRtpSrcSource;
		if (!m_pSourceList.find(nSrcId,pRtpSrcSource))
		{
			return false;
		}
		if (pRtpSrcSource->GetParam()!=nParam)
			return false;
		if (pCallback!=NULL)
			pCallback->onUnRegisterSource(this->GetRoomId(), nSrcId, nParam, pUserData==NULL?pRtpSrcSource->GetCallbackUserData():pUserData);
		return m_pSourceList.remove(nSrcId);
	}
	return false;
}
bool CSotpRtpRoom::RegisterSink(bigint nSrcId,bigint nDestId,const cgcRemote::pointer& pcgcRemote, CSotpRtpCallback* pCallback, void* pUserData)
{
	CSotpRtpSource::pointer pRtpSrcSource;
	if (!m_pSourceList.find(nSrcId,pRtpSrcSource))
		return false;

	if (this->m_bServerMode)
	{
		if (pcgcRemote.get()==NULL || pRtpSrcSource->GetRemoteId()!=pcgcRemote->getRemoteId())
		{
			//printf("*** CSotpRtpRoom::RegisterSink %d<>%d\n",pRtpSrcSource->GetRemoteId(),pcgcRemote->getRemoteId());
			return false;
		}
		if (pCallback!=NULL)
		{
			if (pRtpSrcSource->IsSinkRecv(nDestId))
				return true;
			//printf("*** Callback->onRegisterSink \n");
			if (!pCallback->onRegisterSink(this->GetRoomId(), nSrcId, nDestId, pUserData))
				return false;
		}
	}else
	{
		CSotpRtpSource::pointer pRtpDestSource = RegisterSource(nDestId,0,NullcgcRemote,pCallback,pUserData);
		if (pRtpDestSource.get()==NULL)
			return false;
	}
	pRtpSrcSource->AddSinkRecv(nDestId);
	return true;
}
bool CSotpRtpRoom::UnRegisterSink(bigint nSrcId,bigint nDestId,const cgcRemote::pointer& pcgcRemote)
{
	CSotpRtpSource::pointer pRtpSrcSource;
	if (m_pSourceList.find(nSrcId,pRtpSrcSource))
	{
		if (m_bServerMode)
		{
			if (pcgcRemote.get()==NULL || pRtpSrcSource->GetRemoteId()!=pcgcRemote->getRemoteId())
				return false;
		}
		pRtpSrcSource->DelSinkRecv(nDestId);
	}

	if (!m_bServerMode)
	{
		return UnRegisterSource1(nDestId, 0);
	}
	return true;
}
bool CSotpRtpRoom::UnRegisterAllSink(bigint nSrcId, const cgcRemote::pointer& pcgcRemote)
{
	CSotpRtpSource::pointer pRtpSrcSource;
	if (m_pSourceList.find(nSrcId,pRtpSrcSource))
	{
		return UnRegisterAllSink(pRtpSrcSource, pcgcRemote);
	}
	return false;
}
bool CSotpRtpRoom::UnRegisterAllSink(const CSotpRtpSource::pointer& pRtpSrcSource, const cgcRemote::pointer& pcgcRemote, bool bUnRegisterAllSinkForce)
{
	if (m_bServerMode)
	{
		if (!bUnRegisterAllSinkForce && (pcgcRemote.get()==NULL || pcgcRemote->getRemoteId()!=pRtpSrcSource->GetRemoteId()))
			return false;
	}else
	{
		const CLockMap<bigint,bool>& pList = pRtpSrcSource->GetSinkRecvList();
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(pList.mutex()));
		CLockMap<bigint,bool>::const_iterator pIter = pList.begin();
		for (; pIter!=pList.end(); pIter++)
		{
			const bigint nDestId = pIter->first;
			UnRegisterSource1(nDestId, 0);
		}
	}
	pRtpSrcSource->ClearSinkRecv(true);
	return true;
}

CSotpRtpSource::pointer CSotpRtpRoom::GetRtpSource(bigint nSrcId) const
{
	CSotpRtpSource::pointer pRtpSource;
	m_pSourceList.find(nSrcId,pRtpSource);
	return pRtpSource;
}
bool CSotpRtpRoom::IsRegisterSource(bigint nSrcId) const
{
	return m_pSourceList.exist(nSrcId);
}

void CSotpRtpRoom::BroadcastRtpData(const tagSotpRtpDataHead& pRtpDataHead,const cgcAttachment::pointer& pAttackment) const
{
	const bigint nRealSrcId = ntohll(pRtpDataHead.m_nSrcId);
	{
		CSotpRtpSource::pointer pRtpSrcSource;
		if (!m_pSourceList.find(nRealSrcId,pRtpSrcSource))
		{
			return;
		}
		boost::mutex::scoped_lock lockSrcSendBuffer(pRtpSrcSource->m_pSendBufferMutex);
		unsigned char * pSendBuffer = pRtpSrcSource->GetSendBuffer(20+SOTP_RTP_DATA_HEAD_SIZE+pAttackment->getAttachSize());
		bool bSet2RtpData = false;

		size_t nSendSize = 0;
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pSourceList.mutex()));
		CLockMap<bigint,CSotpRtpSource::pointer>::const_iterator pIter = m_pSourceList.begin();
		for (; pIter!=m_pSourceList.end(); pIter++)
		{
			const bigint nSrcId = pIter->first;
			if (nSrcId==nRealSrcId)
				continue;
			CSotpRtpSource::pointer pRtpDestSource = pIter->second;
			if (!pRtpDestSource->IsSinkRecv(nRealSrcId))
			{
				//printf("*** CSotpRtpRoom::BroadcastRtpData %lld not recv %lld\n",pRtpDestSource->GetSrcId(),nRealSrcId);
				continue;
			}
			const cgcRemote::pointer& pcgcRemote = pRtpDestSource->GetRemote();
			if (pcgcRemote.get()!=NULL)
			{
				if (!bSet2RtpData)
				{
					bSet2RtpData = true;
					SotpCallTable2::toRtpData(pRtpDataHead,pAttackment,pSendBuffer,nSendSize);
				}
				pcgcRemote->sendData(pSendBuffer, nSendSize);
				//const int ret = pcgcRemote->sendData(pSendBuffer, nSendSize);
				//if (ret!=0)
				//{
				//	printf("*** CSotpRtpRoom::BroadcastRtpData %lld sendData error=%d\n",pRtpDestSource->GetSrcId(),ret);
				//}
			//}else
			//{
			//	printf("*** CSotpRtpRoom::BroadcastRtpData %lld pcgcRemote is NULL\n",pRtpDestSource->GetSrcId());
			}
		}
	}
}
void CSotpRtpRoom::CheckRegisterSourceLive(time_t tNow,short nExpireSecond, CSotpRtpCallback* pCallback,void* pUserData)
{
	std::vector<bigint> pRemoveList;
	{
		BoostReadLock rdlock(m_pSourceList.mutex());
		CLockMap<bigint,CSotpRtpSource::pointer>::iterator pIter = m_pSourceList.begin();
		for (; pIter!=m_pSourceList.end(); pIter++)
		{
			CSotpRtpSource::pointer pRtpSrcSource = pIter->second;
			if (tNow-pRtpSrcSource->GetLastTime()>nExpireSecond)
			{
				if (pCallback!=NULL)
					pCallback->onUnRegisterSource(this->GetRoomId(), pRtpSrcSource->GetSrcId(), pRtpSrcSource->GetParam(), pUserData==NULL?pRtpSrcSource->GetCallbackUserData():pUserData);
				UnRegisterAllSink(pRtpSrcSource, pRtpSrcSource->GetRemote(), true);
				pRemoveList.push_back(pRtpSrcSource->GetSrcId());
			}
		}
	}
	for (size_t i=0;i<pRemoveList.size();i++)
	{
		m_pSourceList.remove(pRemoveList[i]);
	}
}
void CSotpRtpRoom::CheckRegisterSinkLive(time_t tNow,short nExpireSecond,bigint nSrcId, const cgcRemote::pointer& pcgcRemote)
{
	//bool bCallbackRoomOnly = false;
	if (!m_bServerMode)
	{
		unsigned char pSendBuffer[64];
		tagSotpRtpCommand pRtpCommand;
		pRtpCommand.m_nVersion = SOTP_RTP_COMMAND_VERSION;
		pRtpCommand.m_nCommand = SOTP_RTP_COMMAND_REGISTER_SOURCE;
		pRtpCommand.m_nRoomId = htonll(this->GetRoomId());
		pRtpCommand.m_nSrcId = htonll(nSrcId);
		BoostReadLock rdlock(m_pSourceList.mutex());
		CLockMap<bigint,CSotpRtpSource::pointer>::iterator pIter = m_pSourceList.begin();
		for (; pIter!=m_pSourceList.end(); pIter++)
		{
			CSotpRtpSource::pointer pRtpSrcSource = pIter->second;
			if (nSrcId==pRtpSrcSource->GetSrcId() && (tNow-pRtpSrcSource->GetLastTime())>nExpireSecond)
			{
				if (pcgcRemote.get()!=NULL)
				{
					pRtpCommand.u.m_nDestId = htonll(pRtpSrcSource->GetParam());
					size_t nSendSize = 0;
					SotpCallTable2::toRtpCommand(pRtpCommand,pSendBuffer,nSendSize);
					pcgcRemote->sendData(pSendBuffer,nSendSize);

					// **
					pRtpSrcSource->SendRegisterSink(pcgcRemote);
				}
				break;
			}
		}
	}
}

} // namespace mycp
