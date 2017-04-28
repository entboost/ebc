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

#ifdef _MSC_VER // WIN32
#pragma warning(disable:4267 4819 4996)
#endif // WIN32

#include "../CGCBase/cgcdef.h"
#include "../CGCClass/SotpCallTable2.h"
#include "SotpRtpSource.h"

namespace mycp {

#ifdef _MSC_VER //WIN32
#include <Windows.h>
#include <Mmsystem.h>
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif
#else
#include <time.h>   
inline unsigned long timeGetTime()  
{  
	unsigned long uptime = 0;  
	struct timespec on;  
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)  
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;  
	return uptime;  
} 
#endif


CSotpRtpSource::CSotpRtpSource(bool bServerMode, bigint nRoomId,bigint nSrcId,bigint nParam)
: m_bServerMode(bServerMode), m_nRoomId(nRoomId), m_nSrcId(nSrcId), m_nParam(nParam)
, m_tLastTime(0), m_tLastCallbackTime(0)
, m_nLastFrameTimestamp(0), m_nWaitForFrameSeq(-1), m_bWaitforNextKeyVideo(false), m_nLostData(0)
, m_pReliableSendBuffer(NULL), m_nReliableBufferSize(0)
, /*theLostSeqInfo1(1,6), */theLostSeqInfo2(15,25)//, theLostSeqInfo3(13,20)
//, /*theLostSeqInfo1(1,6), */theLostSeqInfo2(7,25)//, theLostSeqInfo3(13,20)
, m_nLastPacketSeq(-1)
, m_nLastExpectSeq(-1)
, m_nCurrentSeq(0)
#ifdef USES_FILE_LOG
, m_flog(NULL)
#endif
, m_pCallbackUserData(NULL)
, m_pSendBuffer(NULL), m_nSendBufferSize(0)

{
	//printf("**** CSotpRtpSource() srdid=%lld\n",m_nSrcId);
	m_tLastTime = time(0);
	memset(m_pReliableQueue,0,sizeof(m_pReliableQueue));

	m_NAKRequestCommand.m_nVersion = SOTP_RTP_COMMAND_VERSION;
	m_NAKRequestCommand.m_nCommand = SOTP_RTP_COMMAND_DATA_REQUEST;
	m_NAKRequestCommand.m_nRoomId = htonll(m_nRoomId);
	m_NAKRequestCommand.m_nSrcId = htonll(m_nSrcId);
}
CSotpRtpSource::~CSotpRtpSource(void)
{
	//printf("**** ~CSotpRtpSource() srdid=%lld\n",m_nSrcId);

#ifdef USES_FILE_LOG
	if (m_flog!=NULL)
	{
		fclose(m_flog);
		m_flog = 0;
	}
#endif
	m_pRemote.reset();
	theLostSeqInfo2.clear();
	m_pSinkRecvList.clear();
	m_pReceiveFrames.clear();
	m_pFramePool.clear();
	{
		m_nReliableBufferSize = 0;
		if (m_pReliableSendBuffer!=0)
		{
			delete[] m_pReliableSendBuffer;
			m_pReliableSendBuffer = 0;
		}
		boost::mutex::scoped_lock lock(m_pReliableMutex);
		for(int i=0; i< RELIABLE_QUEUE_SIZE; ++i)
		{
			if (m_pReliableQueue[i] != 0)
			{
				delete m_pReliableQueue[i];
				m_pReliableQueue[i] = NULL;
			}
		}
	}
	if (m_pSendBuffer)
	{
		delete[] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}
}

//void CSotpRtpSource::AddSinkSend(bigint nToId)
//{
//	m_pSinkSendList.insert(nToId,true);
//}
//void CSotpRtpSource::DelSinkSend(bigint nToId)
//{
//	m_pSinkSendList.remove(nToId);
//}
//bool CSotpRtpSource::IsSinkSend(bigint nToId) const
//{
//	return m_pSinkSendList.exist(nToId);
//}

void CSotpRtpSource::AddSinkRecv(bigint nDestId)
{
	m_pSinkRecvList.insert(nDestId,true,false);
}
void CSotpRtpSource::DelSinkRecv(bigint nDestId)
{
	m_pSinkRecvList.remove(nDestId);
}
bool CSotpRtpSource::IsSinkRecv(bigint nDestId) const
{
	return m_pSinkRecvList.exist(nDestId);
}
void CSotpRtpSource::ClearSinkRecv(bool bLock)
{
	m_pSinkRecvList.clear(bLock);
}

void CSotpRtpSource::CaculateMissedPackets(const tagSotpRtpDataHead& pRtpDataHead,const cgcRemote::pointer& pcgcRemote)
{
	boost::mutex::scoped_lock lock(m_pCaculateMissedPacketsMutex);
	const uint16 nSeq = ntohs(pRtpDataHead.m_nSeq);
	if (nSeq==0 && pRtpDataHead.m_nTimestamp==0) return;
	const uint8 nNAKType = pRtpDataHead.m_nNAKType;
	if (nNAKType>=SOTP_RTP_NAK_REQUEST_1)
	{
		if (m_bServerMode && m_nLastExpectSeq>=0)
		{
			if (m_nLastExpectSeq==nSeq)
			{
				m_nLastExpectSeq = -1;
				return;
			}
			sendNAKRequest(m_nLastExpectSeq+1,1,pcgcRemote);
			m_nLastExpectSeq = -1;
		}else if (!m_bServerMode)
		{
			if (m_pLastExpect1.m_nExpectSeq==nSeq)
			{
				m_pLastExpect1.Init();
				return;
			}else if (m_pLastExpect2.m_nExpectSeq==nSeq)
			{
				m_pLastExpect2.Init();
				return;
			}else if (m_pLastExpect1.m_nExpectSeq!=-1 && (++m_pLastExpect1.m_nCount)>=2)
			{
				sendNAKRequest(m_pLastExpect1.m_nExpectSeq+1,1,pcgcRemote);
				m_pLastExpect1.Init();
			}else if (m_pLastExpect2.m_nExpectSeq!=-1 && (++m_pLastExpect2.m_nCount)>=2)
			{
				sendNAKRequest(m_pLastExpect2.m_nExpectSeq+1,1,pcgcRemote);
				m_pLastExpect2.Init();
			}
		}
	}
	if (nNAKType>=SOTP_RTP_NAK_REQUEST_2)
	{
		unsigned short sendSeq = 0;
		int sendCount = 0;
		bool bSendNAKREquest = false;
		//if (theLostSeqInfo1.recv(nSeq, m_nLastPacketSeq, sendSeq, sendCount, !bSendNAKREquest))
		//{
		//	bSendNAKREquest = true;
		//	sendNAKRequest(sendSeq+sendCount, sendCount, pcgcRemote);
		//}
		if (theLostSeqInfo2.recv(nSeq, m_nLastPacketSeq, sendSeq, sendCount, !bSendNAKREquest))
		{
			bSendNAKREquest = true;
			sendNAKRequest(sendSeq+sendCount, sendCount, pcgcRemote);
		}
		//if (theLostSeqInfo3.recv(nSeq, m_nLastPacketSeq, sendSeq, sendCount, !bSendNAKREquest))
		//{
		//	bSendNAKREquest = true;
		//	sendNAKRequest(sendSeq+sendCount, sendCount, pcgcRemote);
		//}
	}

#ifdef USES_FILE_LOG
	char lpszBuffer[260];
	const uint32 ts = ntohl(pRtpDataHead.m_nTimestamp);
#ifdef WIN32
	sprintf(lpszBuffer,"f:\\%lld-%d-%d.txt",this->GetSrcId(),ts,(int)this);
#else
	sprintf(lpszBuffer,"/%lld-%d-%d.txt",this->GetSrcId(),ts,(int)this);
#endif
	if (m_flog==NULL)
		m_flog = fopen(lpszBuffer,"w+");
	
	if (m_flog!=NULL)
	{
		sprintf(lpszBuffer,"ts=%d: recv seq=%d,last seq=%d\n",ts,nSeq,m_nLastPacketSeq);
		fwrite(lpszBuffer,1,strlen(lpszBuffer),m_flog);
	}
#endif

	const uint16 expectSeq = m_nLastPacketSeq + 1;
	if (nSeq == expectSeq || m_nLastPacketSeq==-1 ||
		(nSeq>=1 && nSeq<=10 && m_nLastPacketSeq>300 && m_nLastPacketSeq<65200))	// ?客户端重新进入
	{
		m_nLastPacketSeq = nSeq;
		return;
	}

	//// 计算相距多少个SEQ
	//int intervalseqs = nSeq - m_nLastPacketSeq;
	//if (intervalseqs<0)
	//	intervalseqs = (intervalseqs*-1)-1;
	//else if (intervalseqs>0)
	//	intervalseqs--;

	int n = 0;
	if (nSeq>expectSeq && (nSeq-expectSeq) < 0x7F00)	// 正常大小SEQ，缺少前面SEQ
	{
		// *效果好像好一些，就不知流量如何；
		m_nLastPacketSeq = nSeq;
		//const uint16 nMaxResendCount = (ntohl(pRtpDataHead.m_nTotleLength)/ntohs(pRtpDataHead.m_nUnitLength))>10?(MAX_RESEND_COUNT+3):MAX_RESEND_COUNT;
		const uint16 nMaxResendCount = MAX_RESEND_COUNT;
		if ((nSeq-expectSeq)>nMaxResendCount)
			return;	// 网络过差，重发数据也没用，优化流量；
		n = nSeq-expectSeq;
		if (nNAKType>=SOTP_RTP_NAK_REQUEST_2)
		{
			//if (intervalseqs >= nMaxResendCount)
			//{
			//	//theLostSeqInfo1.clear();
			//	theLostSeqInfo2.clear();
			//	//theLostSeqInfo3.clear();
			//	intervalseqs = 0;	// 过期数据重发也无用,用于网络特别差时减少流量
			//	//intervalseqs = nMaxResendCount;	// 过期数据重发也无用,用于网络特别差时减少流量
			//}
			if (n>1)
			//if (intervalseqs>0)
			{
				//theLostSeqInfo1.lost(nSeq-intervalseqs, intervalseqs);
				theLostSeqInfo2.lost(nSeq-n, n);
				//theLostSeqInfo3.lost(nSeq-intervalseqs, intervalseqs);
			}
		}
	}
	else if (expectSeq>nSeq && (expectSeq - nSeq)> 0x7F00)	// seq 重新从头开始计算
	//else if (expectSeq > nSeq && (intervalseqs > 200))
	{
		m_nLastPacketSeq = nSeq;
		const int diff = nSeq + (65535 - expectSeq);
		const uint16 nMaxResendCount = MAX_RESEND_COUNT;
		//const uint16 nMaxResendCount = (ntohl(pRtpDataHead.m_nTotleLength)/ntohs(pRtpDataHead.m_nUnitLength))>10?(MAX_RESEND_COUNT+3):MAX_RESEND_COUNT;
		n = (diff > nMaxResendCount) ? nMaxResendCount : diff;

		if (nNAKType>=SOTP_RTP_NAK_REQUEST_2)
		{
			// 先发送65535前面
			int diff1 = 65535 - expectSeq;
			if (diff1 > nMaxResendCount)
				diff1 = nMaxResendCount;
			//theLostSeqInfo1.lost(65535-diff1, diff1+1);
			theLostSeqInfo2.lost(65535-diff1, diff1+1);
			//theLostSeqInfo3.lost(65535-diff1, diff1+1);

			// 发送65535后,0开始数据
			const int diff2 = nSeq>0?(nSeq-1):0;
			if (diff2 >= nMaxResendCount)
			{
				//theLostSeqInfo1.clear();
				theLostSeqInfo2.clear();
				//theLostSeqInfo3.clear();
				//diff2 = 0;	// 过期数据重发也无用,用于网络特别差时减少流量
			}else if (diff2 > 0)
			{
				//theLostSeqInfo1.lost(nSeq-diff2, diff2);
				theLostSeqInfo2.lost(nSeq-diff2, diff2);
				//theLostSeqInfo3.lost(nSeq-diff2, diff2);
			}

			//if (intervalseqs > nMaxResendCount)
			//	intervalseqs = nMaxResendCount;	// 过期数据重发也无用,用于网络特别差时减少流量

			//theLostSeqInfo1.lost(seq-intervalseqs, intervalseqs);
			//theLostSeqInfo2.lost(seq-intervalseqs, intervalseqs);
			//theLostSeqInfo3.lost(seq-intervalseqs, intervalseqs);
		}
	}

#ifdef USES_FILE_LOG
	if (m_flog!=NULL)
	{
		sprintf(lpszBuffer,"ts=%d: expect seq=%d,last seq=%d,n=%d\n",ts,expectSeq,m_nLastPacketSeq,n);
		fwrite(lpszBuffer,1,strlen(lpszBuffer),m_flog);
	}
#endif

	if (nNAKType>=SOTP_RTP_NAK_REQUEST_1 && n>0)	// *音频 感觉网络差时,音频效果好一些
	{
		if (n==1)
		{
			if (m_bServerMode)
				m_nLastExpectSeq = expectSeq;	// 临时记录缺失数据，下一个seq包会自动判断是否需要请求补偿，优化流量。
			else
			{
				if (m_pLastExpect1.m_nExpectSeq==-1)
					m_pLastExpect1.Init(expectSeq);
				else
				{
					if (m_pLastExpect2.m_nExpectSeq!=-1)
						sendNAKRequest(m_pLastExpect2.m_nExpectSeq+1,1,pcgcRemote);
					m_pLastExpect2.Init(expectSeq);
				}
			}
		}else
		{
			sendNAKRequest(nSeq,n,pcgcRemote);
		}
	}
}
void CSotpRtpSource::sendNAKRequest(unsigned short nSeq, unsigned short nCount,const cgcRemote::pointer& pcgcRemote)
{
	m_NAKRequestCommand.u.m_nDataRequest.m_nSeq = htons(nSeq);
	m_NAKRequestCommand.u.m_nDataRequest.m_nCount = htons(nCount);
	size_t nSendSize = 0;
	unsigned char lpszBuffer[64];	// SOTP_RTP_COMMAND_SIZE=25
	SotpCallTable2::toRtpCommand(m_NAKRequestCommand,lpszBuffer,nSendSize);
	pcgcRemote->sendData((const unsigned char*)lpszBuffer,nSendSize);
}
bool CSotpRtpSource::UpdateReliableQueue(CSotpRtpReliableMsg* pRtpMsgIn, CSotpRtpReliableMsg** pRtpMsgOut)
{
	bool bResult = false;
	const uint16 nSeq = ntohs(pRtpMsgIn->m_pRtpDataHead.m_nSeq);
	const int i = nSeq%RELIABLE_QUEUE_SIZE;
	boost::mutex::scoped_lock lock(m_pReliableMutex);
	if (m_pReliableQueue[i] != 0)
	{
		bResult = (m_pReliableQueue[i]->m_pRtpDataHead.m_nSeq==pRtpMsgIn->m_pRtpDataHead.m_nSeq&&m_pReliableQueue[i]->m_pRtpDataHead.m_nTimestamp==pRtpMsgIn->m_pRtpDataHead.m_nTimestamp)?true:false;
		if (pRtpMsgOut==NULL)
			delete m_pReliableQueue[i];
		else
			*pRtpMsgOut = m_pReliableQueue[i];
	}
	m_pReliableQueue[i] = pRtpMsgIn;
	return bResult;
}

CSotpRtpFrame::pointer CSotpRtpSource::GetPool(const tagSotpRtpDataHead& pRtpDataHead)
{
	CSotpRtpFrame::pointer pFrame;
	if (!m_pFramePool.front(pFrame))
	{
		pFrame = CSotpRtpFrame::create(pRtpDataHead);
	}else
	{
		pFrame->Init(pRtpDataHead);
	}
	return pFrame;
}

void CSotpRtpSource::SetPool(const CSotpRtpFrame::pointer& pFrame)
{
	if (pFrame.get()!=NULL && m_pFramePool.size()<20)
	{
		m_pFramePool.add(pFrame);
	}
}

void CSotpRtpSource::PushRtpData(const tagSotpRtpDataHead& pRtpDataHead,const cgcAttachment::pointer& pAttackment)
{
	if (pRtpDataHead.m_nIndex>=SOTP_RTP_MAX_PACKETS_PER_FRAME) return;
	const uint32 ts = pRtpDataHead.m_nTimestamp;
	// the packet expire time.
	if (ts<m_nLastFrameTimestamp && ts>0 && (m_nLastFrameTimestamp-ts) < 0xFFFF)		// 前面过期数据
	{
		return;
	}else if (ts>m_nLastFrameTimestamp && (ts-m_nLastFrameTimestamp)>20*1000)	// 20S，中间停止后，后面继续
	{
		m_nLastPacketSeq = -1;
	}

	CSotpRtpFrame::pointer pFrame;
	if (!m_pReceiveFrames.find(ts,pFrame))
	{
		pFrame = GetPool(pRtpDataHead);
		pFrame->BuildBuffer(pRtpDataHead.m_nTotleLength+1);
		//pFrame = CSotpRtpFrame::create(pRtpDataHead);
		//pFrame->m_pPayload = new char[pRtpDataHead.m_nTotleLength+1];
		if (pFrame->m_pPayload==0)
		{
			return;
		}
		//memset(pFrame->m_pPayload, 0, pRtpDataHead.m_nTotleLength+1);
		memcpy(&pFrame->m_pRtpHead,&pRtpDataHead,SOTP_RTP_DATA_HEAD_SIZE);
		pFrame->m_nFirstSeq = pRtpDataHead.m_nSeq-pRtpDataHead.m_nIndex;	// *seq重头开始也正常
		pFrame->m_nPacketNumber = (pRtpDataHead.m_nTotleLength+pRtpDataHead.m_nUnitLength-1)/pRtpDataHead.m_nUnitLength;
		if (ts>0 && !m_bServerMode)
		{
#ifdef _MSC_VER // WIN32
			// 900 ms 是正常允许延迟
			// min(1000,xx)ms 是FPS的间隔时间，最大 1000ms 1FPS
			// min(4500,xx)ms 是计算包大小延迟时间，每2KB左右，增加100ms 延迟，最大 4500ms
			const short nOffset = 900 + min(1000,((ts>m_nLastFrameTimestamp)?(ts-m_nLastFrameTimestamp):(m_nLastFrameTimestamp-ts)));
			pFrame->m_nExpireTime = timeGetTime() + nOffset + min(4500,((pFrame->m_nPacketNumber/2)*100));
#else
			const short nOffset = 900 + std::min(900,(int)((ts>m_nLastFrameTimestamp)?(ts-m_nLastFrameTimestamp):(m_nLastFrameTimestamp-ts)));
			pFrame->m_nExpireTime = timeGetTime() + nOffset + std::min(4500,((pFrame->m_nPacketNumber/2)*100));
#endif
		}
		//if (pRtpDataHead.m_nTotleLength>20*1024 || pRtpDataHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO_I || pRtpDataHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO)
		//	pFrame->m_nExpireTime = timeGetTime()+1800;
		//else
		//	pFrame->m_nExpireTime = timeGetTime()+800;
		CSotpRtpFrame::pointer pFromTemp;
		m_pReceiveFrames.insert(ts,pFrame,false,&pFromTemp);
		if (pFromTemp!=NULL)
		{
			SetPool(pFrame);
			pFrame = pFromTemp;
		}
	}

	if (ts>0 && m_nLastFrameTimestamp == 0)
	{
		// 不能删除，用于解决中间进房间出视频慢问题
		m_nLastFrameTimestamp = pFrame->m_pRtpHead.m_nTimestamp - 1;	// ??这行代码没用，但也没有影响；
		m_nWaitForFrameSeq = (int)(unsigned short)(pFrame->m_nFirstSeq);
	}

	//fill frame data.
	if (pFrame->m_nFilled[pRtpDataHead.m_nIndex] != 1)
	{
		const uint32 nDataOffset = ((uint32)pRtpDataHead.m_nIndex)*pRtpDataHead.m_nUnitLength;
		const uint32 nDataLength = (pRtpDataHead.m_nTotleLength-nDataOffset)>=pRtpDataHead.m_nUnitLength?pRtpDataHead.m_nUnitLength:(pRtpDataHead.m_nTotleLength-nDataOffset);
		if (nDataLength==pAttackment->getAttachSize())
		{
			pFrame->m_nFilled[pRtpDataHead.m_nIndex] = 1;
			memcpy((char*)pFrame->m_pPayload + nDataOffset,(const unsigned char*)pAttackment->getAttachData(),nDataLength);
		}
	}
}

void CSotpRtpSource::GetWholeFrame(HSotpRtpFrameCallback pCallback, void* nUserData)
{
	//uint16 nCount = 0;
	while (!m_pReceiveFrames.empty())
	{
		const uint32 tNow = timeGetTime();
		BoostWriteLock wrlock(m_pReceiveFrames.mutex());
		CLockMap<uint32,CSotpRtpFrame::pointer>::iterator pIter = m_pReceiveFrames.begin();
		for (; pIter!=m_pReceiveFrames.end(); pIter++)
		{
			const CSotpRtpFrame::pointer& pFrame = pIter->second;
			if (pFrame->m_pRtpHead.m_nTimestamp==0 && pFrame->m_pRtpHead.m_nSeq==0 && pFrame->IsWholeFrame())
			{
				// OK
				const CSotpRtpFrame::pointer pFrameTemp = pFrame;
				m_pReceiveFrames.erase(pIter);
				//callback the frame.
				wrlock.unlock();	// **
				if(pCallback!=0)
					pCallback(this->GetSrcId(), pFrameTemp, 0, nUserData);
				SetPool(pFrameTemp);
				break;
			}else if ((m_nWaitForFrameSeq == -1 || ((unsigned short)(m_nWaitForFrameSeq)) == pFrame->m_nFirstSeq) && pFrame->IsWholeFrame())
				//}else if (IsWholeFrame(pFrame) && (m_nWaitForFrameSeq == -1 || ((unsigned short)(m_nWaitForFrameSeq)) == pFrame->m_nFirstSeq))
			{
				// OK
				const CSotpRtpFrame::pointer pFrameTemp = pFrame;
				m_pReceiveFrames.erase(pIter);
				m_nLastFrameTimestamp = pFrameTemp->m_pRtpHead.m_nTimestamp;
				m_nWaitForFrameSeq = (int)(uint16)(pFrameTemp->m_nFirstSeq + pFrameTemp->m_nPacketNumber);
				if (m_bWaitforNextKeyVideo && pFrameTemp->m_pRtpHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO_I)
					m_bWaitforNextKeyVideo = false;

				if (!m_bWaitforNextKeyVideo || pFrameTemp->m_pRtpHead.m_nDataType != SOTP_RTP_NAK_DATA_VIDEO)
				{
					//callback the frame.
					const uint16 nLostDataTemp = m_nLostData;
					m_nLostData = 0;
					wrlock.unlock();	// **
					if(pCallback!=0)
						pCallback(this->GetSrcId(), pFrameTemp, nLostDataTemp, nUserData);
				}
				SetPool(pFrameTemp);
				break;
			}else if (pFrame->m_nExpireTime < tNow)
			{
				// expire time
				const CSotpRtpFrame::pointer pFrameTemp = pFrame;
				m_pReceiveFrames.erase(pIter);
				m_nLastFrameTimestamp = pFrameTemp->m_pRtpHead.m_nTimestamp;
				m_nWaitForFrameSeq = (int)(uint16)(pFrameTemp->m_nFirstSeq + pFrameTemp->m_nPacketNumber);

				if (!m_bWaitforNextKeyVideo && pFrameTemp->IsWholeFrame())
				{
					//callback the frame.
					const uint16 nLostDataTemp = m_nLostData;
					m_nLostData = 0;
					wrlock.unlock();	// **
					if(pCallback!=0)
						pCallback(this->GetSrcId(), pFrameTemp, nLostDataTemp, nUserData);
				}else
				{
					m_nLostData++;
					m_bWaitforNextKeyVideo = (pFrameTemp->m_pRtpHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO_I||pFrameTemp->m_pRtpHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO)?true:false;
				}
				SetPool(pFrameTemp);
				break;
			}else// if ((++nCount)>=2)
			{
				return;
				//break;
			}
		}
	}
}


void CSotpRtpSource::SendReliableMsg(unsigned short nStartSeq,unsigned short nEndSeq,const cgcRemote::pointer& pcgcRemote)
{
	int i = 0;
	for (int k=nStartSeq; k<=nEndSeq; ++k)
	{
		i= k%RELIABLE_QUEUE_SIZE;
		boost::mutex::scoped_lock lock(m_pReliableMutex);
		const CSotpRtpReliableMsg* pRtpReliableMsg = m_pReliableQueue[i];
		if (pRtpReliableMsg!=NULL && ntohs(pRtpReliableMsg->m_pRtpDataHead.m_nSeq)==k)
		{
			const size_t nSizeTemp = 20+SOTP_RTP_DATA_HEAD_SIZE+pRtpReliableMsg->m_pAttachment->getAttachSize();
			if (m_pReliableSendBuffer==NULL)
			{
				m_nReliableBufferSize = nSizeTemp;
				m_pReliableSendBuffer = new unsigned char[m_nReliableBufferSize];
			}else if (nSizeTemp>m_nReliableBufferSize)
			{
				delete[] m_pReliableSendBuffer;
				m_nReliableBufferSize = nSizeTemp;
				m_pReliableSendBuffer = new unsigned char[m_nReliableBufferSize];
			}
			size_t nSendSize = 0;
			SotpCallTable2::toRtpData(pRtpReliableMsg->m_pRtpDataHead,pRtpReliableMsg->m_pAttachment,m_pReliableSendBuffer,nSendSize);
			pcgcRemote->sendData(m_pReliableSendBuffer,nSendSize);
		}
	}
}
void CSotpRtpSource::SendRegisterSink(const cgcRemote::pointer& pcgcRemote)
{
	if (pcgcRemote.get()==NULL)
		return;
	unsigned char lpszBuffer[64];	// SOTP_RTP_COMMAND_SIZE=25
	tagSotpRtpCommand pRtpCommand;
	pRtpCommand.m_nVersion = SOTP_RTP_COMMAND_VERSION;
	pRtpCommand.m_nCommand = SOTP_RTP_COMMAND_REGISTER_SINK;
	pRtpCommand.m_nRoomId = htonll(this->GetRoomId());
	pRtpCommand.m_nSrcId = htonll(this->GetSrcId());

	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pSinkRecvList.mutex()));
	CLockMap<bigint,bool>::const_iterator pIter = m_pSinkRecvList.begin();
	for (; pIter!=m_pSinkRecvList.end(); pIter++)
	{
		const bigint nDestId = pIter->first;
		pRtpCommand.u.m_nDestId = htonll(nDestId);

		size_t nSendSize = 0;
		SotpCallTable2::toRtpCommand(pRtpCommand,lpszBuffer,nSendSize);
		pcgcRemote->sendData(lpszBuffer,nSendSize);
	}

}
unsigned char* CSotpRtpSource::GetSendBuffer(unsigned int nNeedSize)
{
	//if (nNeedSize==0) return NULL;
	if (m_pSendBuffer==NULL)
	{
		m_nSendBufferSize = nNeedSize;
		m_pSendBuffer = new unsigned char[m_nSendBufferSize];
	}else if (nNeedSize>m_nSendBufferSize)
	{
		delete[] m_pSendBuffer;
		m_nSendBufferSize = nNeedSize;
		m_pSendBuffer = new unsigned char[m_nSendBufferSize];
	}
	return m_pSendBuffer;
}

} // namespace mycp
