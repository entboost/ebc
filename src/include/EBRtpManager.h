#pragma once
#include <string>
#include "pop_sotpclient.h"
#include "eb_objects.h"

namespace entboost {

class CEBRtpManager;
class CEBRtpHangler
{
public:
	virtual void OnProcessTimer(const CEBRtpManager* pLCOwner) {}	// 1秒定时事件
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CEBRtpManager* pLCOwner) {}
	virtual void OnInvalidateSession(int nResultCode,const CEBRtpManager* pLCOwner) {}
	virtual void OnResultValue(int nResultValue,const CEBRtpManager* pLCOwner) {}

	virtual void OnRtpFrame(mycp::bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount, mycp::uint32 nUserData, CEBRtpManager* pLCOwner) {}
	
	virtual void OnRDOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}
	virtual void OnRDOffResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}

	virtual void OnP2POk(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}
	virtual void OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}
	virtual void OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}
	virtual void OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner) {}

};

class CEBRtpManager
	: public CPOPSotpClient
{
public:
	typedef boost::shared_ptr<CEBRtpManager> pointer;
	static CEBRtpManager::pointer create(CEBRtpHangler * pPOPHandler,bool bProcessTimer=false)
	{
		return CEBRtpManager::pointer(new CEBRtpManager(pPOPHandler,bProcessTimer));
	}
	CEBRtpManager(CEBRtpHangler * pPOPHandler,bool bProcessTimer);
	virtual ~CEBRtpManager(void);

public:
	int Start(const CCgcAddress & address, const tstring& sAppName,unsigned int bindPort, int nUsesSsl, int nMaxWaitSecons=2);
	bool IsStart(void) const;
	void Stop(void);
	void SetEncoding(const std::string& v);

	int SendRDOn(mycp::bigint nId,mycp::bigint nFromUserId,mycp::bigint nParam, int LogonType, CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendRDOff(mycp::bigint nId,mycp::bigint nFromUserId,mycp::bigint nParam, CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	void SendCloseSession(void);

	mycp::bigint m_nP2PTryOwnerAccount;
	int m_nOnP2POk;	// -1=初始 0=等于ssl pwd 1=p2p ok
	int m_nP2PData;	// 1是内网；
	void SetDestAddress(int nUserIndex,const std::string& sToAddress);
	int SendP2PRequest(mycp::bigint nFromUid,mycp::bigint nChatId,mycp::bigint nToAccount,const tstring& sLocapIp,int nRequestData);
	int SendP2PResponse(mycp::bigint nFromUid,mycp::bigint nChatId,mycp::bigint nToAccount,const tstring& sLocapIp,int nResponseData,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendP2PTry(mycp::bigint nCallId,mycp::bigint nChatId,mycp::bigint nFromAccount,int nTryCount,int nP2PData);
	void SendP2PTest(unsigned short nTestCount);

	void SetTranSpeedLimit(int nLimit=64);
	void SetRtpCallbackData(mycp::uint32 nUserData);
	unsigned short GetLocalPort(void) const;

	int SetSourceId(mycp::bigint nSrcId);
	mycp::bigint GetSourceId(void) const;
	int RegisterSource(mycp::bigint nRoomId, mycp::bigint nParam);
	int UnRegisterSource(mycp::bigint nRoomId);
	int UnRegisterAllSource(void);
	int RegisterSink(mycp::bigint nRoomId, mycp::bigint nDestId);
	int UnRegisterSink(mycp::bigint nRoomId, mycp::bigint nDestId);
	int UnRegisterAllSink(mycp::bigint nRoomId);
	int UnRegisterAllSink(void);
	int SendRtpData(mycp::bigint nRoomId,const unsigned char* pData,mycp::uint32 nSize,mycp::uint32 nTimestamp=0,mycp::uint8 nDataType=SOTP_RTP_NAK_DATA_AUDIO,mycp::uint8 nNAKType=SOTP_RTP_NAK_REQUEST_1);

	//void SetParameter(const std::string& v) {m_sParameter = v;}
	//const std::string& GetParameter(void) const {return m_sParameter;}
	void SetParameter(int v) {m_nParameter = v;}
	int GetParameter(void) const {return m_nParameter;}

	//static void response_thread_svr(void* lparam);
	void process_thread_svr(void);
	bool IsKilled(void) const {return m_bKilled;}
	void DoProcess(void);
	//void DoResponse(void);
private:
	void ProcP2PTry(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnCgcResponse(const cgcParserSotp & response);
	virtual void OnRtpFrame(mycp::bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount, mycp::uint32 nUserData);

	CLockMap<unsigned long, CPOPSotpRequestInfo::pointer> m_pRequestList;			// callid->
	CLockList<CPOPSotpRequestInfo::pointer> m_pReqResponseList;
	//CLockList<CPOPSotpResponseInfo::pointer> m_pResponseList;

	bool			m_bProcessTimer;
	bool			m_bKilled;
	//boost::thread * m_pResponseThread;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	CEBRtpHangler *	m_pPOPHandler;
	// info
	//std::string		m_sParameter;
	int				m_nParameter;
	bool m_bRDOn;
};

} // namespace entboost
