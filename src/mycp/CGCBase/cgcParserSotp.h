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

// cgcParserSotp.h file here
#ifndef _cgcparsersotp_head_included__
#define _cgcparsersotp_head_included__

#include <vector>
#include "cgcparameter.h"
#include "cgcattachment.h"
#include "cgcParserBase.h"
#include "cgcrtpobject.h"

namespace mycp {

class cgcParserSotp
	: public cgcParserBase
{
public:
	typedef boost::shared_ptr<cgcParserSotp> pointer;

	// protocol
	virtual SOTP_PROTO_VERSION getSotpVersion(void) const = 0;
	virtual bool isSessionProto(void) const = 0;
	virtual bool isAppProto(void) const = 0;
	virtual bool isAckProto(void) const = 0;
	virtual bool isP2PProto(void) const = 0;
	//virtual bool isClusterProto(void) const = 0;
	// proto type
	virtual bool isOpenType(void) const = 0;
	virtual bool isCloseType(void) const = 0;
	virtual bool isActiveType(void) const = 0;
	virtual bool isCallType(void) const = 0;
	//virtual bool isQueryType(void) const = 0;
	virtual int getProtoType(void) const = 0;

	virtual bool isResulted(void) const = 0;
//	virtual const tstring & getResultString(void) const = 0;
	virtual int getResultValue(void) const = 0;

	virtual bool isNeedAck(void) const = 0;
	virtual bool hasSeq(void) const = 0;
	virtual unsigned short getSeq(void) const = 0;
	virtual void setSid(const tstring & newValue) = 0;
	virtual const tstring & getSid(void) const = 0;		// SessionId
	virtual void setCallid(unsigned long newValue) = 0;
	virtual unsigned long getCallid(void) const = 0;			// CallId
	virtual void setSign(unsigned long newValue) = 0;
	virtual unsigned long getSign(void) const = 0;				// Sign
	virtual void setSslPublicKey(const tstring & newValue) = 0;
	virtual const tstring & getSslPublicKey(void) const = 0;
	virtual bool isSslRequest(void) const = 0;
	virtual bool getProtoItem(int nItemType, unsigned long * pOutItemValue) const = 0;
	
	virtual bool isRtpCommand(void) const = 0;
	virtual bool isRtpData(void) const = 0;
	virtual const tagSotpRtpCommand& getRtpCommand(void) const = 0;
	virtual const tagSotpRtpDataHead& getRtpDataHead(void) const = 0;

//	virtual const tstring & getProtoValue(void) const = 0;	// SessionId for SES:protocol

	// Request Parameters
	virtual cgcParameter::pointer getRecvParameter(const tstring & sParamName) const = 0;
	virtual bool getRecvParameter(const tstring & sParamName, std::vector<cgcParameter::pointer>& outParams) const = 0;

	virtual tstring getRecvParameterValue(const tstring & sParamName, const char* sDefault = "") const = 0;
	virtual tstring getRecvParameterValue(const tstring & sParamName, const tstring& sDefault = "") const = 0;
	virtual int getRecvParameterValue(const tstring & sParamName, int nDefault) const = 0;
	virtual bigint getRecvParameterValue(const tstring & sParamName, bigint tDefault) const = 0;
	virtual bool getRecvParameterValue(const tstring & sParamName, bool bDefault) const = 0;
	virtual double getRecvParameterValue(const tstring & sParamName, double fDefault) const = 0;
	virtual std::size_t getRecvParameterCount(void) const = 0;
	virtual const cgcParameterMap & getRecvParameters(void) const = 0;
	// Request attachment
	virtual bool isRecvHasAttachInfo(void) const = 0;
	virtual const cgcAttachment::pointer& getRecvAttachment(void) const = 0;

	//////////////////////////////////////////////////
	// Response:
	virtual void setResEncoding(const tstring & sEncoding = _T("GBK")) = 0;
	virtual tstring getSessionResult(int retCode, const tstring & sSessionId, unsigned short seq, bool bNeedAck,const tstring& sSslPublicKey, int nAcceptEncoding) const = 0;
	virtual tstring getAppCallResult(int retCode, unsigned short seq, bool bNeedAck) = 0;
	virtual tstring getAppCallResultHead(int retCode) = 0;
	virtual tstring getAppCallResultData(unsigned short seq, bool bNeedAck) = 0;
	virtual tstring getAckResult(unsigned short seq) = 0;
	virtual unsigned char * getAttachString(const cgcAttachment::pointer& pAttach, unsigned int & pOutSize) const = 0;
	// P2P
	virtual tstring getP2PTry(void) const = 0;

	// Response Parameters
	virtual void setResParameter(const cgcParameter::pointer& parameter, bool bSetForce) = 0;
	virtual void addResParameter(const cgcParameter::pointer& parameter, bool bAddForce) = 0;
	virtual void addResParameters(const std::vector<cgcParameter::pointer>& parameters, bool bAddForce) = 0;
	virtual void delResParameter(const tstring& paramName) = 0;
	virtual void clearResParameter(void) = 0;

	virtual std::size_t getResParameterCount(void) const = 0;
	// Response attachment
	virtual void setResAttachName(const tstring & name)=0;
	virtual void setResAttachInfo(bigint total, bigint index)=0;
	virtual void setResAttachData(const unsigned char * attachData, unsigned int attachSize)=0;
	virtual void setResAttachData2(unsigned char * attachData, unsigned int attachSize)=0;
	virtual void setResAttach(const cgcAttachment::pointer& pAttach)=0;
	virtual bool isResHasAttachInfo(void) const = 0;
	virtual const cgcAttachment::pointer& getResAttachment(void) const = 0;
	virtual unsigned char * getResAttachString(unsigned int & pOutSize) = 0;

	// Response ssl
	virtual unsigned char * getResSslString(const tstring& sSslPassword,unsigned int & pOutSize) = 0;

	//virtual const MyParameterMap & GetResponseParameterMap(void) const {return m_mapParameters;}
};

const cgcParserSotp::pointer cgcNullParserSotpService;

#define CGC_PARSERSOTPSERVICE_DEF(s) boost::static_pointer_cast<cgcParserSotp, cgcServiceInterface>(s)

} // namespace mycp

#endif // _cgcparsersotp_head_included__
