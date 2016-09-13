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

// cgcrtpobject.h file here
#ifndef __cgcrtpobject_h__
#define __cgcrtpobject_h__

#include "cgcdef.h"
#include "cgcobject.h"

namespace mycp {

#pragma pack(push)
#pragma pack(1)
struct tagSotpRtpDataRequest
{
	uint16			m_nSeq;
	uint16			m_nCount;
};
#define SOTP_RTP_COMMAND_VERSION 2
struct tagSotpRtpCommand
{
	uint8			m_nVersion;
	uint8			m_nCommand;				// see SOTP_RTP_COMMAND_TYPE
	bigint			m_nRoomId;
	bigint			m_nSrcId;
	union
	{
		bigint				m_nDestId;
		tagSotpRtpDataRequest	m_nDataRequest;	// for SOTP_RTP_COMMAND_DATA_REQUEST
	}u;
};
#define SOTP_RTP_COMMAND_SIZE 26
//const uint16 SOTP_RTP_COMMAND_SIZE = sizeof(tagSotpRtpCommand);
struct tagSotpRtpDataHead
{
	bigint		m_nRoomId;
	bigint		m_nSrcId;
	uint16		m_nSeq;
	uint8		m_nNAKType;		// see SOTP_RTP_NAK_TYPE
	uint8		m_nDataType;	// see SOTP_RTP_DATA_TYPE
	uint32		m_nTimestamp;
	uint32		m_nTotleLength;
	uint16		m_nUnitLength;
	uint16		m_nIndex;
};
#define SOTP_RTP_DATA_HEAD_SIZE 32
//const uint16 SOTP_RTP_DATA_HEAD_SIZE = sizeof(tagSotpRtpDataHead);
#pragma pack(pop)

#define SOTP_RTP_MAX_PACKETS_PER_FRAME	512	// 64
#define SOTP_RTP_MAX_PAYLOAD_LENGTH		1100

class CSotpRtpFrame
{
public:
	typedef boost::shared_ptr<CSotpRtpFrame> pointer;
	static CSotpRtpFrame::pointer create(void)
	{
		return CSotpRtpFrame::pointer(new CSotpRtpFrame());
	}
	static CSotpRtpFrame::pointer create(const tagSotpRtpDataHead& pRtpHead)
	{
		return CSotpRtpFrame::pointer(new CSotpRtpFrame(pRtpHead));
	}

	uint16			m_nFirstSeq;
	uint16			m_nPacketNumber;
	uint8*			m_nFilled;
	uint32			m_nExpireTime;
	tagSotpRtpDataHead	m_pRtpHead;
	char*				m_pPayload;

	bool IsWholeFrame(void) const {
		uint16 i = 0;
		for(; i < m_nPacketNumber; i++) {
			if (m_nFilled[i] != 1) {
				return false;
			}
		}
		return true;
	}
	char* BuildBuffer(unsigned int nBufferSize)
	{
		if (m_pPayload==NULL)
		{
			m_nBufferSize = nBufferSize;
			m_pPayload = new char[m_nBufferSize];
		}else if (m_nBufferSize < nBufferSize)
		{
			delete[] m_pPayload;
			m_pPayload = NULL;
			m_nBufferSize = nBufferSize;
			m_pPayload = new char[m_nBufferSize];
		}
		return m_pPayload;
	}
	void Init(const tagSotpRtpDataHead& pRtpHead)
	{
		m_nFirstSeq = 0;
		m_nPacketNumber = 0;
		m_nExpireTime = 0;
		memset(m_nFilled,0,SOTP_RTP_MAX_PACKETS_PER_FRAME);
		memcpy(&m_pRtpHead,&pRtpHead,SOTP_RTP_DATA_HEAD_SIZE);
	}
	CSotpRtpFrame(void)
		: m_nFirstSeq(0)
		, m_nPacketNumber(0)
		, m_nExpireTime(0)
		, m_pPayload(NULL)
		, m_nBufferSize(0)
	{
		m_nFilled = new uint8[SOTP_RTP_MAX_PACKETS_PER_FRAME];
		memset(m_nFilled,0,SOTP_RTP_MAX_PACKETS_PER_FRAME);
		memset(&m_pRtpHead,0,SOTP_RTP_DATA_HEAD_SIZE);
	}
	CSotpRtpFrame(const tagSotpRtpDataHead&	pRtpHead)
		: m_nFirstSeq(0)
		, m_nPacketNumber(0)
		, m_nExpireTime(0)
		, m_pPayload(NULL)
		, m_nBufferSize(0)
	{
		m_nFilled = new uint8[SOTP_RTP_MAX_PACKETS_PER_FRAME];
		memset(m_nFilled,0,SOTP_RTP_MAX_PACKETS_PER_FRAME);
		memcpy(&m_pRtpHead,&pRtpHead,SOTP_RTP_DATA_HEAD_SIZE);
	}
	virtual ~CSotpRtpFrame(void)
	{
		if (m_pPayload!=NULL)
			delete[] m_pPayload;
		if (m_nFilled!=NULL)
			delete[] m_nFilled;
	}
private:
	unsigned int m_nBufferSize;

};

//typedef void (* HSotpRtpFrameCallback)(bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, uint16 nLostCount, uint32 nUserData);

} // namespace mycp


#endif // __cgcrtpobject_h__
