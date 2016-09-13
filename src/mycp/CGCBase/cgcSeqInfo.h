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

// cgcSeqInfo.h file here
#ifndef _CGCSEQINFO_HEAD_INCLUDED__
#define _CGCSEQINFO_HEAD_INCLUDED__

//#include "dlldefine.h"
//#include "CGCClass/cgcclassinclude.h"

#include <boost/shared_ptr.hpp>
#include <time.h>

namespace mycp {

class cgcSeqInfo
{
public:
	typedef boost::shared_ptr<cgcSeqInfo> pointer;
	static cgcSeqInfo::pointer create(void)
	{
		return cgcSeqInfo::pointer(new cgcSeqInfo());
	}
	static cgcSeqInfo::pointer create(unsigned short seq, unsigned long cid, unsigned long sign, unsigned int timeoutResends=0, unsigned int timeoutSeconds=0)
	{
		return cgcSeqInfo::pointer(new cgcSeqInfo(seq, cid, sign, timeoutResends, timeoutSeconds));
	}

	cgcSeqInfo(void)
		: m_seq(0), m_cid(0), m_sign(0),m_userData(0)
		, m_callData(0), m_dataSize(0), m_callName(_T(""))
		, m_sessionId(_T(""))
		, m_sendTime(0)
		, m_timeoutResends(5), m_timeoutSeconds(4)
		, m_resendTimes(0)
	{
	}
	cgcSeqInfo(unsigned short seq, unsigned long cid, unsigned long sign, unsigned int timeoutResends=0, unsigned int timeoutSeconds=0)
		: m_seq(seq), m_cid(cid), m_sign(sign),m_userData(0)
		, m_callData(0), m_dataSize(0), m_callName(_T(""))
		, m_sessionId(_T(""))
		, m_sendTime(time(0))
		, m_timeoutResends(timeoutResends), m_timeoutSeconds(timeoutSeconds)
		, m_resendTimes(0)
	{
	}
	virtual ~cgcSeqInfo(void)
	{
		clearCallData();
	}

public:
	unsigned short getSeq(void) const {return m_seq;}
	unsigned long getCid(void) const {return m_cid;}

	void setSign(unsigned long newValue) {m_sign = newValue;}
	unsigned long getSign(void) const {return m_sign;}
	void setUserData(unsigned int newValue) {m_userData = newValue;}
	unsigned int getUserData(void) const {return m_userData;}

	void setCallData(unsigned char * data, unsigned int size)
	{
		clearCallData();
		m_dataSize = size;
		m_callData = data;
	}
	void setCallData(const unsigned char * data, unsigned int size)
	{
		clearCallData();
		m_dataSize = size;
		if (data != 0 && m_dataSize > 0)
		{
			m_callData = new unsigned char[m_dataSize+1];
			memcpy(m_callData, data, m_dataSize);
			m_callData[m_dataSize] = '\0';
		}

	}
	const unsigned char * getCallData(void) const{return m_callData;}
	unsigned int getDataSize(void) const {return m_dataSize;}

	void setTimeoutResends(unsigned int newValue) {m_timeoutResends = newValue;}
	unsigned int getTimeoutResends(void) const {return m_timeoutResends;}

	void setTimeoutSeconds(unsigned int newValue) {m_timeoutSeconds = newValue;}
	unsigned int getTimeoutSeconds(void) const {return m_timeoutSeconds;}

	void setCallName(const tstring & newValue) {m_callName = newValue;}
	const tstring & getCallName(void) const {return m_callName;}

	void setSessionId(const tstring & newValue) {m_sessionId = newValue;}
	const tstring & getSessionId(void) const {return m_sessionId;}

	void setSendTime(void) {m_sendTime=time(0);}
	time_t getSendTime(void) const {return m_sendTime;}

	void SetAddress(const tstring& sAddress) {m_sAddress=sAddress;}
	const tstring& GetAddress(void) const {return m_sAddress;}

	bool isTimeout(void)
	{
		if (m_timeoutSeconds == 0) return false;

		time_t tNow = time(0);

		//
		// system time had been changed, or this is the init cgcSeqInfo
		if (tNow <= m_sendTime || m_sendTime == 0)
		{
			m_sendTime = tNow;
			return false;
		}

		return (tNow - m_sendTime) >= 3;
		//unsigned int timerInterval = m_resendTimes+2;	// 2,3,4
		//if (timerInterval > m_timeoutSeconds)
		//	timerInterval = m_timeoutSeconds;
		//return (tNow - m_sendTime) >= timerInterval;
		////
		//// timeout
		//return (time_t)(m_sendTime + m_timeoutSeconds) <= tNow;
	}
	unsigned int increaseResends(void) {return ++m_resendTimes;}
	bool canResendAgain(void) const {return m_resendTimes < m_timeoutResends;}

	//cgcRemote::pointer m_cgcRemote;
protected:
	void clearCallData(void)
	{
		m_dataSize = 0;
		if (m_callData)
		{
			delete[] m_callData;
			m_callData = 0;
		}
	}

private:
	unsigned short m_seq;
	unsigned long m_cid;
	unsigned long m_sign;
	unsigned int m_userData;

	unsigned char * m_callData;
	unsigned int m_dataSize;
	tstring m_callName;
	tstring m_sessionId;

	time_t m_sendTime;
	unsigned int m_timeoutResends;
	unsigned int m_timeoutSeconds;

	unsigned int m_resendTimes;
	tstring m_sAddress;
};

} // namespace mycp

#endif // _CGCSEQINFO_HEAD_INCLUDED__
