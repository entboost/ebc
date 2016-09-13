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

// PpSotp2.h file here
// sotp protocol parser
#ifndef __ppSotp2_h__
#define __ppSotp2_h__

#include "dlldefine.h"
#include "ParseCgcSotp2.h"
#include "ModuleItem.h"
#include "../CGCBase/cgcParserSotp.h"
#include "SotpCallTable2.h"

namespace mycp {

class CPPSotp2
	: public cgcParserSotp
	, public SotpCallTable2
{
private:
	// Request
	ParseCgcSotp2 m_cgcInvoke;
	// Response
	cgcAttachment::pointer m_attach;
	cgcParserCallback* m_pCallback;
public:
	CPPSotp2(void);
	virtual ~CPPSotp2(void);

	// cgcServiceInterface
	virtual tstring serviceName(void) const {return _T("ParserSotp");}

	////////////////////////////////////////////////////////
	// cgcParserSotp handler
	//////////////////////////////////////////
	// Parser
//	virtual bool initParser(void) {return true;}
//	virtual void exitParser(void) {}

	// ssl
	//void SetSslPrivateKey(const tstring& newv) {m_cgcInvoke.setSslPrivateKey(newv);}
	//const tstring& GetSslPrivateKey(void) const {return m_cgcInvoke.getSslPrivateKey();}
	//void SetSslPrivatePwd(const tstring& newv) {m_cgcInvoke.setSslPrivatePwd(newv);}
	//const tstring& GetSslPrivatePwd(void) const {return m_cgcInvoke.getSslPrivatePwd();}
	//void SetSslPassword(const tstring& newv) {m_cgcInvoke.setSslPassword(newv);}
	const tstring& GetSslPassword(void) const {return m_cgcInvoke.getSslPassword();}
	int GetAcceptEncoding(void) const {return m_cgcInvoke.getAcceptEncoding();}

	//////////////////////////////////////////////
	// Request
	virtual void setParseCallback(cgcParserCallback* pCallback) {m_pCallback = pCallback;}
	virtual bool doParse(const unsigned char * requestData, size_t size,const char* sEncoding="");

	virtual void init(void);
	virtual SOTP_PROTO_VERSION getSotpVersion(void) const {return m_cgcInvoke.getSotpVersion();}
	virtual bool isSessionProto(void) const {return m_cgcInvoke.isSessionProto();}
	virtual bool isAppProto(void) const {return m_cgcInvoke.isAppProto();}
	virtual bool isAckProto(void) const {return m_cgcInvoke.isAckProto();}
	virtual bool isP2PProto(void) const {return m_cgcInvoke.isP2PProto();}
	//virtual bool isClusterProto(void) const {return m_cgcInvoke.isClusterProto();}

	virtual bool isOpenType(void) const {return m_cgcInvoke.isOpenType();}
	virtual bool isCloseType(void) const {return m_cgcInvoke.isCloseType();}
	virtual bool isActiveType(void) const {return m_cgcInvoke.isActiveType();}
	virtual bool isCallType(void) const {return m_cgcInvoke.isCallType();}
	//virtual bool isQueryType(void) const {return m_cgcInvoke.isQueryType();}
	virtual int getProtoType(void) const {return m_cgcInvoke.getProtoType();}

	virtual bool isResulted(void) const {return m_cgcInvoke.isResulted();}
//	virtual const tstring & getResultString(void) const {return m_cgcInvoke.getResultString();}
	virtual int getResultValue(void) const {return m_cgcInvoke.getResultValue();}

	virtual unsigned short getSeq(void) const {return m_cgcInvoke.getSeq();}
	virtual bool hasSeq(void) const {return m_cgcInvoke.hasSeq();}
	virtual bool isNeedAck(void) const {return m_cgcInvoke.isNeedAck();}
	virtual void setSid(const tstring & newValue) {m_cgcInvoke.setSid(newValue);}
	virtual const tstring & getSid(void) const {return m_cgcInvoke.getSid();}
	virtual void setCallid(unsigned long newValue) {m_cgcInvoke.setCallid(newValue);}
	virtual unsigned long getCallid(void) const {return m_cgcInvoke.getCallid();}
	virtual void setSign(unsigned long newValue) {m_cgcInvoke.setSign(newValue);}
	virtual unsigned long getSign(void) const {return m_cgcInvoke.getSign();}
	virtual void setSslPublicKey(const tstring & newValue) {m_cgcInvoke.setSslPublicKey(newValue);}
	virtual const tstring & getSslPublicKey(void) const {return m_cgcInvoke.getSslPublicKey();}
	virtual bool isSslRequest(void) const {return m_cgcInvoke.isSslRequest();}
	virtual bool getProtoItem(int nItemType, unsigned long * pOutItemValue) const {return m_cgcInvoke.getProtoItem(nItemType, pOutItemValue);}

	virtual bool isRtpCommand(void) const {return m_cgcInvoke.isRtpCommand();}
	virtual bool isRtpData(void) const {return m_cgcInvoke.isRtpData();}
	virtual const tagSotpRtpCommand& getRtpCommand(void) const {return m_cgcInvoke.getRtpCommand();}
	virtual const tagSotpRtpDataHead& getRtpDataHead(void) const {return m_cgcInvoke.getRtpDataHead();}

	//virtual const tstring & getProtoValue(void) const {return m_cgcInvoke.getProtoValue();}
	virtual const tstring & getModuleName(void) const {return m_cgcInvoke.getAppName();}
	virtual const tstring & getFunctionName(void) const {return m_cgcInvoke.getApiName();}
	virtual tstring getAccount(void) const {return m_cgcInvoke.getAccount();}
	virtual tstring getSecure(void) const {return m_cgcInvoke.getPasswd();}
	virtual tstring getPasswd(void) const {return m_cgcInvoke.getPasswd();}

	// Request
	virtual cgcParameter::pointer getRecvParameter(const tstring & sParamName) const {return m_cgcInvoke.getParameter(sParamName);}
	virtual bool getRecvParameter(const tstring & sParamName, std::vector<cgcParameter::pointer>& outParams) const {return m_cgcInvoke.getParameter(sParamName, outParams);}

	virtual tstring getRecvParameterValue(const tstring & sParamName, const char* defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual tstring getRecvParameterValue(const tstring & sParamName, const tstring& defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual int getRecvParameterValue(const tstring & sParamName, int defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual bigint getRecvParameterValue(const tstring & sParamName, bigint defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual bool getRecvParameterValue(const tstring & sParamName, bool defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual double getRecvParameterValue(const tstring & sParamName, double defaultv) const {return m_cgcInvoke.getParameterValue(sParamName, defaultv);}
	virtual std::size_t getRecvParameterCount(void) const {return m_cgcInvoke.getParameterCount();}
	virtual const cgcParameterMap & getRecvParameters(void) const {return m_cgcInvoke.getParameters();}
	virtual bool isRecvHasAttachInfo(void) const {return m_cgcInvoke.getAttachInfo()->isHasAttach();}
	virtual const cgcAttachment::pointer& getRecvAttachment(void) const {return m_cgcInvoke.getAttachInfo();}

	////////////////////////////////////////////////////////
	// Response
	virtual void setResEncoding(const tstring & sEncoding) {SotpCallTable2::setEncoding(sEncoding);}
	virtual tstring getSessionResult(int retCode, const tstring & sSessionId, unsigned short seq, bool bNeedAck, const tstring& sSslPublicKey, int nAcceptEncoding) const;
	virtual tstring getAppCallResult(int retCode, unsigned short seq, bool bNeedAck);
	virtual tstring getAppCallResultHead(int retCode);
	virtual tstring getAppCallResultData(unsigned short seq, bool bNeedAck);
	virtual tstring getAckResult(unsigned short seq);
	virtual unsigned char * getAttachString(const cgcAttachment::pointer& pAttach, unsigned int & pOutSize) const {return SotpCallTable2::toAttachString(getSotpVersion(),pAttach, pOutSize);}
	// P2P
	virtual tstring getP2PTry(void) const {return SotpCallTable2::toP2PTry(this->getSotpVersion());}

	// Parameters
	virtual void setResParameter(const cgcParameter::pointer& parameter, bool bSetForce) {SotpCallTable2::setParameter(parameter,bSetForce);}
	virtual void addResParameter(const cgcParameter::pointer& parameter, bool bAddForce) {SotpCallTable2::addParameter(parameter,bAddForce);}
	virtual void addResParameters(const std::vector<cgcParameter::pointer> & parameters, bool bAddForce) {SotpCallTable2::addParameters(parameters,bAddForce);}
	virtual void delResParameter(const tstring& paramName) {SotpCallTable2::delParameter(paramName);}
	virtual void clearResParameter(void) {SotpCallTable2::clearParameter();}
	virtual std::size_t getResParameterCount(void) const {return SotpCallTable2::getParameterSize();}

	// for attachment
	virtual void setResAttachName(const tstring & name);
	virtual void setResAttachInfo(bigint total, bigint index);
	virtual void setResAttachData(const unsigned char * attachData, unsigned int attachSize);
	virtual void setResAttachData2(unsigned char * attachData, unsigned int attachSize);
	virtual void setResAttach(const cgcAttachment::pointer& pAttach);
	virtual bool isResHasAttachInfo(void) const {return m_attach->isHasAttach();}
	virtual const cgcAttachment::pointer& getResAttachment(void) const {return m_attach;}
	virtual unsigned char * getResAttachString(unsigned int & pOutSize);

	// for ssl
	virtual unsigned char * getResSslString(const tstring& sSslPassword,unsigned int & pOutSize);

};

} // namespace mycp

#endif // __ppSotp2_h__
