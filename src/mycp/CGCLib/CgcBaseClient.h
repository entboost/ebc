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

// CgcBaseClient.h file here
#ifndef __CgcBaseClient_h__
#define __CgcBaseClient_h__

//
// include
#include "IncludeBase.h"
#include <boost/enable_shared_from_this.hpp>
#include "CgcClientHandler.h"
#include "../ThirdParty/stl/lockmap.h"
#include "../ThirdParty/stl/aes.h"
#include "../ThirdParty/stl/rsa.h"
#include "../CGCBase/cgcaddress.h"
//#include "cgcaddress.h"
#include "dlldefine.h"
#include "../CGCBase/cgcSeqInfo.h"
#include "../CGCClass/SotpRtpSession.h"

namespace mycp {
// typedef
//
//
// BoostThreadList
#ifndef BoostThreadList
typedef std::list<boost::thread*> BoostThreadList;
typedef BoostThreadList::const_iterator BoostThreadListCIter;
#endif
//
typedef std::map<unsigned long, void*> ULongPtrMap;
typedef std::pair<unsigned long, void*> ULongPtrPair;

#define MAX_CID_MASKS_SIZE 500//120

class CGCLIB_CLASS CgcBaseClient
	: public SotpCallTable2
	, public DoSotpClientHandler
	, public cgcParserCallback
	, public boost::enable_shared_from_this<CgcBaseClient>
{
public:
	typedef boost::shared_ptr<CgcBaseClient> pointer;

	enum ClientState
	{
		Init_Client		= 0x1
		, Start_Client	= 0x2
		, Stop_Client	= 0x4
		, Exit_Client	= 0x8

	};

	CgcBaseClient(const tstring & clientType);
	virtual ~CgcBaseClient(void);

	static int ParseString(const char * lpszString, const char * lpszInterval, std::vector<std::string> & pOut);
	static bool IsIpAddress(const char* pString, size_t nLen);
	static std::string GetHostIp(const char * lpszHostName,const char* lpszDefault);

	class CIndexInfo
	{
	public:
		typedef boost::shared_ptr<CIndexInfo> pointer;
		//std::string m_sSessionId;
		tstring m_sSslPassword;
		//int m_nAcceptEncoding;

		static CIndexInfo::pointer create(void)
		{
			return CIndexInfo::pointer(new CIndexInfo());
		}
		//static CIndexInfo::pointer create(const std::string& sSessionId,const std::string& sSslPassword)
		//{
		//	return CIndexInfo::pointer(new CIndexInfo(sSessionId, sSslPassword));
		//}
		//CIndexInfo(const std::string& sSessionId,const std::string& sSslPassword)
		//	: m_sSessionId(sSessionId), m_sSslPassword(sSslPassword)
		//{}
		CIndexInfo(void)
			//: m_nAcceptEncoding(SOTP_DATA_ENCODING_UNKNOWN);
		{}
	};

protected:
	virtual int startClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize) = 0;
	virtual void stopClient(void) = 0;
	virtual bool isInvalidate(void) const = 0;
	virtual size_t sendData(const unsigned char * data, size_t size) = 0;
	virtual size_t sendData(const unsigned char * data, size_t size, unsigned int timestamp) {return 0;}	// for RTP
	virtual size_t recvData(unsigned char * buffer, size_t size) {return 0;}
	virtual void parseData(const unsigned char * data, size_t size,unsigned long nRemoteId);
	//virtual void parseData(const CCgcData::pointer& recvData,unsigned long nRemoteId);
	virtual bool setRemoteAddr(const tstring & sRemoteAddr) {return true;}
	virtual void setMediaType(unsigned short mediatype) {}	// for RTP

	/////////////////////////////////////////////////////////////////////////////////
	// cgcParserCallback 
	virtual tstring onGetSslPrivateKey(void) const {return m_pRsaSrc.GetPrivateKey();}
	virtual tstring onGetSslPrivatePwd(void) const {return m_pRsaSrc.GetPrivatePwd();}
	virtual tstring onGetSslPassword(const tstring& sSessionId) const;
	//virtual tstring onGetSslPassword(const tstring& sSessionId) const {return (sSessionId.empty() || getSessionId()==sSessionId)?m_sSslPassword:"";}

	// DoSotpClientHandler handler
	virtual void doSetResponseHandler(CgcClientHandler * newValue) {setHandler(newValue);}
	virtual const CgcClientHandler * doGetResponseHandler(void) const {return getHandler();}
	virtual void doSetDisableSotpParser(bool newv) {m_bDisableSotpparser = newv;}

	virtual bool doSetConfig(int nConfig, unsigned long nInValue);
	virtual void doGetConfig(int nConfig, unsigned long* nOutValue) const;
	virtual void doFreeConfig(int nConfig, unsigned long nInValue) const;

	virtual void doSetIoService(mycp::asio::IoService::pointer pIoService, bool bExitStop = false) {m_ipService = pIoService; m_bExitStopIoService=bExitStop;}
	virtual void doResetIoService(void) {m_ipService.reset();}
	virtual mycp::asio::IoService::pointer doGetIoService(void) const {return m_ipService;}

	// session
	virtual bool doSendOpenSession(short nMaxWaitSecons,unsigned long * pOutCallId) {return sendOpenSession(nMaxWaitSecons,pOutCallId);}
	virtual void doSendCloseSession(unsigned long * pOutCallId) {sendCloseSession(pOutCallId);}
	virtual bool doIsSessionOpened(void) const {return isSessionOpened();}
	virtual const tstring & doGetSessionId(void) const {return getSessionId();}

	// app call
	virtual bool doIsInvalidate(void) const {return isInvalidate();}
	virtual void doBeginCallLock(void) {beginCallLock();}
	virtual bool doSendAppCall(unsigned long nCallSign, const tstring & sCallName, bool bNeedAck,const cgcAttachment::pointer& pAttach, unsigned long * pOutCallId){
		return sendAppCall(nCallSign,sCallName,bNeedAck,pAttach,pOutCallId);}
	virtual unsigned long doGetNextCallId(void) {return getNextCallId();}
	virtual bool doSendAppCall2(unsigned long nCallId, unsigned long nCallSign, const tstring & sCallName, bool bNeedAck = true,const cgcAttachment::pointer& pAttach = constNullAttchment,bool bDisableZip=false){
		return sendAppCall2(nCallId,nCallSign,sCallName,bNeedAck,pAttach,bDisableZip);}
	virtual bool doSendSyncCall(unsigned long nCallId, const tstring& sSyncName, bool bNeedAck = true,const cgcAttachment::pointer& pAttach = constNullAttchment){
		return sendSyncCall(nCallId,sSyncName,bNeedAck,pAttach);}
	virtual bool doSendCallResult(long nResult,unsigned long nCallId,unsigned long nCallSign=0,bool bNeedAck = true,const cgcAttachment::pointer& pAttach = constNullAttchment,bool bDisableZip=false){
		return sendCallResult(nResult,nCallId,nCallSign,bNeedAck,pAttach,bDisableZip);}
	virtual void doSendP2PTry(unsigned short nTryCount) {sendP2PTry(nTryCount);}

	unsigned int m_nUserData;
	// sotp rtp
	CSotpRtpMsgPool m_pRtpBufferPool;
	CSotpRtpMsgPool m_pRtpMsgPool;
	CSotpRtpSession m_pRtpSession;
	bigint m_nSrcId;
	uint32 m_nRtpCbUserData;
	uint32 m_nTranSpeedLimit;
	uint32 m_nDefaultSleep1;	// 每8个包，等待时间，64KB，默认50ms
	uint32 m_nDefaultSleep2;	// 每50个包，等待时间，64KB，默认500ms
	uint16 m_nDefaultPackageSize;	// 默认1100字节，每增加64KB，增加512
	cgcRemote::pointer m_pOwnerRemote;
	virtual void doSetRtpSourceId(bigint nSrcId) {m_nSrcId = nSrcId;}
	virtual bigint doGetRtpSourceId(void) const {return m_nSrcId;}
	virtual bool doRegisterSource(bigint nRoomId, bigint nParam);
	virtual void doUnRegisterSource(bigint nRoomId);
	virtual bool doIsRegisterSource(bigint nRoomId) const;
	virtual void doUnRegisterAllSource(void);
	virtual bool doRegisterSink(bigint nRoomId, bigint nDestId);
	virtual void doUnRegisterSink(bigint nRoomId, bigint nDestId);
	virtual void doUnRegisterAllSink(bigint nRoomId);
	virtual void doUnRegisterAllSink(void);
	virtual bool doIsRegisterSink(bigint nRoomId, bigint nDestId) const;
	boost::mutex m_pSendRtpMutex;
	virtual bool doSendRtpData(bigint nRoomId,const unsigned char* pData,uint32 nSize,uint32 nTimestamp,uint8 nDataType,uint8 nNAKType);

	// threads
	virtual void doSetCIDTResends(unsigned short timeoutResends, unsigned short timeoutSeconds) {setCIDTResends(timeoutResends, timeoutSeconds);}
	//virtual void doStartRecvThreads(unsigned short nRecvThreads) {/*StartRecvThreads(nRecvThreads)*/;}
	virtual void doStartActiveThread(unsigned short nActiveWaitSeconds,unsigned short nSendP2PTrySeconds) {StartActiveThread(nActiveWaitSeconds,nSendP2PTrySeconds);}

	// parameter
	virtual void doAddParameter(const cgcParameter::pointer& parameter, bool bAddForce = true) {addParameter(parameter,bAddForce);}
	virtual void doAddParameters(const std::vector<cgcParameter::pointer>& parameters, bool bAddForce = true) {addParameters(parameters,bAddForce);}
	virtual size_t doGetParameterSize(void) const {return getParameterSize();}

	// info
	virtual void doSetEncoding(const tstring & newv) {setEncoding(newv);}
	virtual const tstring & doGetEncoding(void) const {return getEncoding();}
	virtual void doSetAppName(const tstring & newv) {setAppName(newv);}
	virtual const tstring & doGetAppName(void) const {return getAppName();}
	virtual void doSetAccount(const tstring & account, const tstring & passwd) {setAccount(account); setPasswd(passwd);}
	virtual void doGetAccount(tstring & account, tstring & passwd) const {account=getAccount(); passwd=getPasswd();}
	virtual const tstring & doGetClientType(void) const {return GetClientType();}

	// other
	virtual time_t doGetLastSendRecvTime(void) const {return m_tSendRecv;}
	virtual bool doSetRemoteAddr(const tstring & newv) {return setRemoteAddr(newv);}
	virtual tstring doGetRemoteAddr(void) const {return m_ipRemote.address();}
	virtual const tstring& doGetLocalIp(void) const {return m_ipLocal.getip();}
	virtual unsigned short doGetLocalPort(void) const {return m_ipLocal.getport();}
	virtual void doSetMediaType(unsigned short newv) {setMediaType(newv);}	// for RTP
	virtual size_t doSendData(const unsigned char * data, size_t size) {return sendData(data, size);}
	virtual size_t doSendData(const unsigned char * data, size_t size, unsigned int timestamp) {return sendData(data, size, timestamp);}

public:
	//
	// WSAStartup
	static int WSAInit(void);
	//
	// WSACleanup
	static void WSAExit(void);

	//
	// CwsClientHandler
	void setHandler(CgcClientHandler * newValue=NULL) {m_pHandler = newValue;}
	const CgcClientHandler * getHandler(void) const {return m_pHandler;}

	//
	// Inspects whether already started the cws client.
	bool isStarted(void) const {return isClientState(Start_Client);}

	//
	// Set client path to load the client infomation. Default "" for current path.
	// For example cluster list.
	// return true that have client info.
	bool setClientPath(const tstring & sClientPath = _T(""));

	const tstring & getClientPath(void) const {return m_currentPath;}

	//
	// callid timeout resends, set '0' do not resend
	void setCIDTResends(unsigned short timeoutResends=2, unsigned short timeoutSeconds=4);

	//
	// Start the cws client.
	// sCgcServerAddr: ip:port Format
	// nActiveWaitSeconds: how many second regular active the session.
	//  >0, cycle seconds, <=0, do not regular
	//int StartClient(const tstring & sCwssHostName, u_short nCwssPort=8089, int nRecvThreads=2, int nActiveWaitSeconds=60);
	int StartClient(const tstring & sCgcServerAddr, unsigned int bindPort, int nThreadStackSize);

	void StartCIDTimeout(void);
	// nRecvThreads: 0 <= nRecvThreads <= 20
	//void StartRecvThreads(unsigned short nRecvThreads = 2);
	//void StopRecvThreads(void);

	void StartActiveThread(unsigned short nActiveWaitSeconds = 30,unsigned short nSendP2PTrySeconds=20);
	void StopActiveThread(void);
	// 
	// by clusterlist info
	//int StartClient(int nRecvThreads=0, int nActiveWaitSeconds=60);
	int NextCluster(void);
	//
	// Stop the cws client.
	void StopClient(bool exitClient = true);

	const tstring & GetClientType(void) const {return m_clientType;}

	ClientState GetCientState(void) const {return m_clientState;}
	bool isClientState(ClientState cs) const {return m_clientState == cs;}

	//
	// cluster
	int sendQueryClusterSvr(const tstring & sAccount=_T(""), const tstring & sPasswd=_T(""), unsigned long * pCallId = 0);
	// verify current clusterSvr
	int sendVerifyClusterSvr(unsigned long * pCallId = 0);
	//
	// cluster infomation
//	size_t getClusterSvrCount(void) const {return this->m_custerSvrList.size();}
//	const ClusterSvrList & getClusters(void) const {return m_custerSvrList;}

	//
	// Send open session request.
	bool sendOpenSession(short nMaxWaitSecons,unsigned long * pCallId = 0);
	//
	// from sendOpenSession(...)
//	const tstring & getAppName(void) const {return m_sAppName;}
//	const tstring & getAccount(void) const {return m_sAccount;}
//	const tstring & getPasswd(void) const {return m_sPasswd;}
	//
	// Send close session request.
	void sendCloseSession(unsigned long * pCallId = 0);
	//
	// Send active session request.
	void sendActiveSession(unsigned long * pCallId = 0);
	void sendP2PTry(unsigned short nTryCount);

	void RtpCheckRegisterSink(void);
	//void ReRegisterSink(CSotpRtpRoom* pSotpRtpRoom,CSotpRtpSource* pSotpRtpSourc);
	void OnRtpFrame(bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, uint16 nLostCount);

	//
	// Send app call request.
	//   return 0: send succeeded.
	// nCIDTimeoutSeconds:
	//   ==0: do not check timeout
	//   if want recv OnCIDTimeout event, then recommand set to 10-20.
	//int sendAppCall(unsigned long nCallSign, const tstring & sCallName, const tstring & sAppName=L"", const Attachment * pAttach = NULL, unsigned long * pCallId = 0);
	void beginCallLock(void);	// lock
	bool sendAppCall(unsigned long nCallSign, const tstring & sCallName, bool bNeedAck,const cgcAttachment::pointer& pAttach = constNullAttchment, unsigned long * pCallId = 0);
	bool sendAppCall2(unsigned long nCallId, unsigned long nCallSign, const tstring & sCallName, bool bNeedAck,const cgcAttachment::pointer& pAttach = constNullAttchment,bool bDisableZip=false);
	bool sendSyncCall(unsigned long nCallId, const tstring& sSyncName, bool bNeedAck,const cgcAttachment::pointer& pAttach = constNullAttchment);
	bool sendCallResult(long nResult,unsigned long nCallId,unsigned long nCallSign,bool bNeedAck = true,const cgcAttachment::pointer& pAttach = constNullAttchment,bool bDisableZip=false);

	//
	// cid ptr(data)
	// 
	// The SetCidData method set the cid specifies data, and return the original cid data.
	void * SetCidData(unsigned long cid, void * pData);
	void * GetCidData(unsigned long cid);
	void * RemoveCidData(unsigned long cid);
	void RemoveAllCidData(void);
	void RemoveAllCidData(ULongPtrMap &mapCidPtr);

	tstring getLocaleAddr(u_short & portOut) const;
	tstring getRemoteAddr(void) const;

protected:
	//
	// static thread
	//static void do_proc_CgcClient(CgcBaseClient * udpclient);
	void do_proc_activesession(void);
	void do_proc_cid_timeout(void);
	//static void do_proc_activesession(const CgcBaseClient::pointer& udpclient);
	//static void do_proc_cid_timeout(CgcBaseClient * udpclient);

	//
	// 
	bool isTimeToActiveSes(void) const;
	bool isTimeToSendP2PTry(void) const;

	// seq
	bool checkSeqTimeout(void);

	//
	// Serialize
	virtual void ClearClientInfo(void);
	virtual void SaveClientInfo(void);
	virtual void Serialize(bool isStoring, tfstream& ar);

private:
	//
	// socket
	size_t RecvData(unsigned char * buffer, size_t size);
	//size_t SendData(const unsigned char * data, size_t size);

	void addSeqInfo(const unsigned char * callData, unsigned int dataSize, unsigned short seq,  unsigned long cid, unsigned long sign=0,unsigned int nUserData=0);
	bool addSeqInfo(unsigned char * callData, unsigned int dataSize, unsigned short seq, unsigned long cid, unsigned long sign=0,unsigned int nUserData=0);

protected:
	time_t m_tSendRecv;
	//unsigned long m_destIp;
	//unsigned short m_destPort;
	CCgcAddress m_ipLocal;
	CCgcAddress m_ipRemote;

	// ssl
	tstring m_sSslPassword;	// from mycp server
	int m_nAcceptEncoding;
	CRSA m_pRsaSrc;
	int m_nUserSslInfo;		// 1=mem; 2=file
	CIndexInfo::pointer m_pCurrentIndexInfo;
	CLockMap<unsigned int,CIndexInfo::pointer> m_pIndexInfoList;
	// ioservice
	bool m_bExitStopIoService;
	mycp::asio::IoService::pointer m_ipService;
private:
	CgcClientHandler * m_pHandler;
//	unsigned int m_clientState;
	ClientState m_clientState;
	tstring m_clientType;
	bool m_bDisableSotpparser;

	boost::mutex m_sendMutex;
	boost::mutex m_recvMutex;
	boost::mutex::scoped_lock * m_pSendLock;
	//BoostThreadList m_listBoostThread;
	boost::shared_ptr<boost::thread> m_threadActiveSes;
	boost::shared_ptr<boost::thread> m_threadCIDTimeout;
	unsigned short m_nActiveWaitSeconds;					// 
	unsigned short m_nSendP2PTrySeconds;

//	tstring m_sAppName;						// for open session
//	tstring m_sAccount;						// for open session
//	tstring m_sPasswd;						// for open session
//	ClusterSvrList m_custerSvrList;

	class CReceiveInfo
	{
	public:
		typedef boost::shared_ptr<CReceiveInfo> pointer;
		static CReceiveInfo::pointer create(void)
		{
			return CReceiveInfo::pointer(new CReceiveInfo());
		}
		unsigned int m_nDataIndex;
		time_t m_tLastCid;
		boost::mutex m_recvSeq;
		int m_pReceiveCidMasks[MAX_CID_MASKS_SIZE];
		CReceiveInfo(void)
			: m_nDataIndex(0), m_tLastCid(0)
		{
			memset(&m_pReceiveCidMasks,-1,sizeof(m_pReceiveCidMasks));
		}
	};
	CLockMap<unsigned long, CReceiveInfo::pointer> m_pReceiveInfoList;	// support P2P
	//CReceiveInfo::pointer m_pDefaultReceiveInfo;
	//time_t m_tLastCid;
	//boost::mutex m_recvSeq;
	//int m_pReceiveCidMasks[MAX_CID_MASKS_SIZE];
	// cid map
	CLockMap<unsigned short, cgcSeqInfo::pointer> m_mapSeqInfo;
	unsigned int m_timeoutSeconds;			// default '0', do not resend
	unsigned int m_timeoutResends;			// default '0', do not resend

	//
	// cid ptr(data)
	boost::mutex m_mutexCidPtrMap;
	ULongPtrMap m_mapCidPtr;

	tstring m_currentPath;
	SOTP_PROTO_VERSION theProtoVersion;

};

} // namespace cgc
#endif // __CgcBaseClient_h__
