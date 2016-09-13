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

// ParseCgcSotp2.h file here
#ifndef __ParseCgcSotp2_h__
#define __ParseCgcSotp2_h__

//#include <boost/lexical_cast.hpp>
#include "../CGCBase/cgcparameter.h"
#include "../CGCBase/cgcattachment.h"
#include "../CGCBase/cgcParserSotp.h"
#include "ModuleItem.h"
#include "MethodItem.h"
//#include "info/ClusterSvr.h"

#include "dlldefine.h"

namespace mycp {

class CGCCLASS_CLASS ParseCgcSotp2
{
public:
	ParseCgcSotp2(void);
	virtual ~ParseCgcSotp2(void);

public:
	// 21=SOTP/2.1
	// 20=SOTP/2.0
	SOTP_PROTO_VERSION getSotpVersion(void) const {return m_nSotpVersion;}
	bool isSessionProto(void) const {return m_nProtoType<=SOTP_PROTO_TYPE_ACTIVE;}
	bool isAppProto(void) const {return m_nProtoType==SOTP_PROTO_TYPE_CALL;}
	void setAppProto(void) {m_nProtoType=SOTP_PROTO_TYPE_CALL;}
	bool isAckProto(void) const {return m_nProtoType==SOTP_PROTO_TYPE_ACK;}
	bool isP2PProto(void) const {return m_nProtoType==SOTP_PROTO_TYPE_P2P;}
	bool isRTPProto(void) const {return m_nProtoType==SOTP_PROTO_TYPE_RTP;}
	//bool isClusterProto(void) const {return m_nCgcProto==3;}
	int getProtoType(void) const {return m_nProtoType;}

	void setAppName(const tstring & newv) {m_sApp = newv;}
	const tstring & getAppName(void) const {return m_sApp;}

	void setApiName(const tstring & newv) {m_sApi = newv;}
	const tstring & getApiName(void) const {return m_sApi;}

	//void setProtoValue(const tstring & newv) {m_sProtoValue = newv;}
	//const tstring & getProtoValue(void) const {return m_sProtoValue;}

	bool isOpenType(void) const {return m_nProtoType==SOTP_PROTO_TYPE_OPEN;}
	bool isCloseType(void) const {return m_nProtoType==SOTP_PROTO_TYPE_CLOSE;}
	bool isActiveType(void) const {return m_nProtoType==SOTP_PROTO_TYPE_ACTIVE;}
	void setCallType(void) {m_nProtoType = SOTP_PROTO_TYPE_CALL;}
	bool isCallType(void) const {return m_nProtoType==SOTP_PROTO_TYPE_CALL;}

	//bool isQueryType(void) const {return m_nProtoType==20;}
	//bool isVerifyType(void) const {return m_nProtoType==21;}

	bool isResulted(void) const {return m_bResulted;}
//	const tstring & getResultString(void) const {return m_sResultValue;}
	int getResultValue(void) const {return m_nResultValue;}

	void setAccount(const tstring & newv) {m_sAccount = newv;}
	const tstring & getAccount(void) const {return m_sAccount;}

	void setPasswd(const tstring & newv) {m_sPasswd = newv;}
	const tstring & getPasswd(void) const {return m_sPasswd;}

	bool hasSeq(void) const {return m_bHasSeq;}
	unsigned short getSeq(void) const {return m_seq;}
	bool isNeedAck(void) const {return m_bNeedAck;}
	void setSid(const tstring & newValue) {m_sSid = newValue;}
	const tstring & getSid(void) const {return m_sSid;}
	void setSslPublicKey(const tstring & newValue) {m_sSslPublicKey = newValue;}
	bool isSslRequest(void) const {return m_sSslPublicKey.empty()?false:true;}
	bool getProtoItem(int nItemType, unsigned long * pOutItemValue) const;
	const tstring & getSslPublicKey(void) const {return m_sSslPublicKey;}
	//void setSslPrivateKey(const tstring & newValue) {m_sSslPrivateKey = newValue;}
	//const tstring & getSslPrivateKey(void) const {return m_sSslPrivateKey;}
	//void setSslPrivatePwd(const tstring & newValue) {m_sSslPrivatePwd = newValue;}
	//const tstring & getSslPrivatePwd(void) const {return m_sSslPrivatePwd;}
	//void setSslPassword(const tstring & newValue) {m_sSslPassword = newValue;}
	const tstring & getSslPassword(void) const {return m_sSslPassword;}
	int getAcceptEncoding(void) const {return m_nAcceptEncoding;}
	int getDataEncoding(void) const {return m_nDataEncoding;}
	void setCallid(unsigned long newValue) {m_nCallId = newValue;}
	unsigned long getCallid(void) const {return m_nCallId;}
	void setSign(unsigned long newValue) {m_nSign = newValue;}
	unsigned long getSign(void) const {return m_nSign;}

	bool isRtpCommand(void) const {return m_bRtpCommand;}
	bool isRtpData(void) const {return m_bRtpData;}
	const tagSotpRtpCommand& getRtpCommand(void) const {return m_pSotpRtpCommand;}
	const tagSotpRtpDataHead& getRtpDataHead(void) const {return m_pSotpRtpDataHead;}

	//
	// parameter
	size_t getParameterCount(void) const {return this->m_parameterMap.size();}
	const cgcParameterMap & getParameters(void) const {return this->m_parameterMap;}
	cgcParameter::pointer getParameter(const tstring & paramName) const {return m_parameterMap.getParameter(paramName);}
	bool getParameter(const tstring & paramName, std::vector<cgcParameter::pointer>& outParams) const {return m_parameterMap.find(paramName, outParams);}
	tstring getParameterValue(const tstring & paramName, const char* defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}
	tstring getParameterValue(const tstring & paramName, const tstring& defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}
	int getParameterValue(const tstring & paramName, int defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}
	bigint getParameterValue(const tstring & paramName, bigint defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}
	bool getParameterValue(const tstring & paramName, bool defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}
	double getParameterValue(const tstring & paramName, double defaultValue) const {return m_parameterMap.getParameterValue(paramName, defaultValue);}

	const cgcAttachment::pointer& getAttachInfo(void) const {return m_attach;}

	//
	// cluster
//	size_t getClusterSvrCount(void) const {return this->m_custerSvrList.size();}
//	const ClusterSvrList & getClusters(void) const {return m_custerSvrList;}
//	int getClusters(ClusterSvrList & listResult);

	void Reset(bool bResetAttach);
	void addParameter(const cgcParameter::pointer& parameter);

public:
	void setParseCallback(cgcParserCallback* pCallback) {m_pCallback = pCallback;}
	bool parseBuffer(const unsigned char * pBuffer,size_t size,const char* sEncoding="");

protected:
	// 把SOTP协议，改到2.0版本
#ifdef WIN32
	static std::string str_convert(const char * strSource, int sourceCodepage, int targetCodepage);
#endif
	const char * parseOneLine(const char * pLineBuffer,size_t nBufferSize);
	bool sotpCompare(const char * pBuffer, const char * pCompare, int & leftIndex);
	//char* m_pUnZipDataTBuffer;
	//size_t m_nUnZipDataBufferSize;
	//size_t m_nUnZipDataBufferIndex;
	//static bool MyUnZipDataCallBack(const unsigned char* pData, unsigned long nSize, unsigned int nUserData);
	unsigned char* rebuildBuffer(size_t bBufferSize);
private:
	tstring m_sEncoding;
	cgcParameterMap m_parameterMap;
	cgcAttachment::pointer m_attach;
	bool m_bRtpCommand;
	bool m_bRtpData;
	tagSotpRtpCommand m_pSotpRtpCommand;
	tagSotpRtpDataHead m_pSotpRtpDataHead;
	cgcParserCallback* m_pCallback;
	//ClusterSvrList m_custerSvrList;

	SOTP_PROTO_VERSION m_nSotpVersion;
	//int m_nCgcProto;		// 1: session, 2: app, 3: cluster 4: p2p
	SOTP_PROTO_TYPE m_nProtoType;		// 1:open, 2:close, 3:active, 10:call, 20:query, 21:verify
	//tstring m_sProtoValue;
	bool m_bHasSeq;
	unsigned short m_seq;
	bool m_bNeedAck;		// default false
	tstring m_sSid;				// session id
	tstring m_sApp;				// app name
	tstring m_sApi;				// api name
	tstring m_sSslPublicKey;
	//tstring m_sSslPrivateKey;
	//tstring m_sSslPrivatePwd;
	tstring m_sSslPassword;			// *
	int m_nAcceptEncoding;
	int m_nDataEncoding;
	int m_nDataSize;
	unsigned char* m_pSslDecryptData;
	size_t m_nSslDecrptyBufferSize;
	unsigned long m_nCallId;			// call id
	unsigned long m_nSign;				// sign
	bool m_bResulted;
//	tstring m_sResultValue;
	int m_nResultValue;

	tstring m_sAccount;		// for open session, query cluster
	tstring m_sPasswd;
	ModuleItem::EncryptionType m_et;
};

} // namespace mycp

#endif // __ParseCgcSotp2_h__
