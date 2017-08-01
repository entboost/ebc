#pragma once
#include <string>
#include "pop_sotpclient.h"
#include "eb_objects.h"
//#include "pop_richmsg.h"
#include "chatroomobjects.h"
//#include "pophandler.h"
#include "SendFileThread.h"

namespace entboost {

class CPOPCChatManager;
class CPOPCMHandler
{
public:
	virtual void OnProcessTimer(const CPOPCChatManager* pCMOwner) {}	// N秒定时事件
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnInvalidateSession(int nResultCode,const CPOPCChatManager* pCMOwner) {}

	virtual void OnSave2CloudDriveResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo,mycp::bigint sResourceId, EB_STATE_CODE nResultValue,const CPOPCChatManager* pCMOwner) {}
	virtual void OnSetResourceResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo,mycp::bigint sResourceId, bool bError,const CPOPCChatManager* pCMOwner) =0;
	virtual void OnMsgReceipt(EB_STATE_CODE nResultValue,int nAckType,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CEBChatInfo::pointer& pChatInfo,mycp::bigint nMsgId,mycp::bigint sFromAccount, const CPOPCChatManager* pCMOwner) =0;
	virtual void OnSendingRich(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CEBChatRoomRichMsg::pointer& pRichMsg,mycp::bigint sFromAccount, mycp::bigint sToAccount, int nPrivate,const CPOPCChatManager* pCMOwner) =0;
	virtual void OnSendingFile(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnSentFile(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo, mycp::bigint sReceivedAccount,const CPOPCChatManager* pCMOwner) =0;
	virtual void OnCancelFile(EB_STATE_CODE nResultValue, const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo, mycp::bigint sCancelAccount,const CPOPCChatManager* pCMOwner) =0;
	//virtual void OnReceivedFile(const tstring & sFromAccount, const CChatMsgInfo::pointer& pChatMsgInfo) {}
	virtual void OnFilePercent(const CPOPSotpRequestInfo::pointer & pReqeustInfo,mycp::bigint sResourceId,mycp::bigint nMsgId, double percent, int nSeconds1, int nSeconds2, int nSpeech1, int nSpeech2,const CPOPCChatManager* pCMOwner) =0;
	virtual void OnReceivingRes(const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPCChatManager* pCMOwner) {}

	virtual int OnFCMMsg(const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {return 0;}
	virtual void OnCMMsgResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnFCMExit(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnCMExitResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnFCMEnterResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnCMEnterResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnEnterAnother(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnFCMNotify(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}

	virtual void OnP2POk(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}
	virtual void OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) {}

};

class CSpeechInfo
{
public:
	double m_percent;
	int m_nCurSpeed;		// 当前速度
	int m_nAvrSpeed;		// 平均速度
	time_t m_time;

	CSpeechInfo(void)
		: m_percent(0.0)
		, m_nCurSpeed(0)
		, m_nAvrSpeed(0)
		, m_time(0)
	{
	}
	CSpeechInfo(const CSpeechInfo& pObj)
	{
		CSpeechInfo::operator=(pObj);
	}
	const CSpeechInfo& operator=(const CSpeechInfo& pObj)
	{
		m_percent = pObj.m_percent;
		m_nCurSpeed = pObj.m_nCurSpeed;
		m_nAvrSpeed = pObj.m_nAvrSpeed;
		m_time = pObj.m_time;
		return *this;
	}
};

class CPOPCChatManager
	: public CPOPSotpClient
{
public:
	typedef boost::shared_ptr<CPOPCChatManager> pointer;
	static CPOPCChatManager::pointer create(CPOPCMHandler * pPOPHandler,int nProcessTimer=0)
	{
		return CPOPCChatManager::pointer(new CPOPCChatManager(pPOPHandler,nProcessTimer));
	}
	CPOPCChatManager(CPOPCMHandler * pPOPHandler,int nProcessTimer=0);
	virtual ~CPOPCChatManager(void);

	void cm_response_thread_svr(void);
	void cm_process_thread_svr(void);

	static int ParseString(const char * lpszString, const char * lpszInterval, std::vector<int> & pOut);
public:
	// nUsesSsl 0/1
	int Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort,bool bBuildResponseThread,bool bBuildProcessThread,int nUsesSsl,int nMaxWaitSecons);
	bool IsStart(void) const;
	void Stop(void);
	void SetEncoding(const std::string& v);
	void SetAutoAckSuccess(bool bAutoAck = true) {m_bAutoAckSuccess = bAutoAck;}
	bool GetAutoAckSuccess(void) const {return m_bAutoAckSuccess;}

	void SetParameter(const std::string& v) {m_sParameter = v;}
	const std::string& GetParameter(void) const {return m_sParameter;}
	void SetBigData(mycp::bigint v) {m_nBigData = v;}
	mycp::bigint GetBigData(void) const {return m_nBigData;}

	//int SendCRSet(const tstring & sResourceId,const tstring& sAccount,const tstring& sResourceKey,const tstring& sFilePath,size_t nFileSize);

	// 发给对方客户端
	bool m_bLocalIpAddress;
	mycp::bigint m_nP2PTryOwnerAccount;
	int m_nOnP2POk;	// -1=初始 0=等于ssl pwd 1=p2p ok
	int m_nP2PData;	// 1是内网；
	void SetDestAddress(int nUserIndex,const std::string& sToAddress);
	int SendP2PRequest(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocapIp,int nRequestData);
	int SendP2PResponse(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocapIp,int nResponseData,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendP2PTry(cr::bigint nCallId,cr::bigint nChatId,cr::bigint nFromAccount,int nTryCount,int nP2PData);
	void SendP2PTest(unsigned short nTestCount);
	//int SendVRequest(int nVideoType=2);
	//int SendVAck(bool bAccept);
	//int SendVEnd(void);
	int SendCMReceiveResource(mycp::bigint sResourceId, const tstring & sSaveTo);
	int SendCMReceiveFile(mycp::bigint nMsgId, const tstring & sSaveTo);
	int SendCMSave2CloudDrive(mycp::bigint nMsgId);
	int SendCMAck(mycp::bigint nMsgId, mycp::bigint sResourceId, EB_MSG_ACK_TYPE ackType,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	eb::bigint GetNextBigId(void);
	//int SendCMMsg(mycp::bigint nFrom, EB_RICH_SUB_TYPE subtype, const tstring& sUserData,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	//int SendCMMsg(mycp::bigint nFrom, const tstring& sMapPosData,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendCMMsg(const tstring & sPathName,const tstring & sFileName,const tstring& sMD5,mycp::bigint nFileSize,unsigned int nPackLen,mycp::bigint sResourceId,mycp::bigint nFrom,mycp::bigint sTo,int nPrivate,int nSendOffFile,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendCMMsg(const CEBChatRoomRichMsg::pointer& pRichMsg,unsigned int nPackLen,mycp::bigint nFrom,mycp::bigint sTo, int nPrivate=0,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendCMExit(int nExitSes,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendCMEnter(mycp::bigint nAppId,mycp::bigint nFromIpAddress,mycp::bigint sFromAccount,int nType,mycp::bigint sDepCode, mycp::bigint sCallId, mycp::bigint sChatId, const tstring & sChatKey, CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	int SendCMNotify(cr::bigint nToUserId, int nNotifyType, cr::bigint nNotifyId, const tstring& sNotifyData,CPOPSotpRequestInfo::pointer pRequestInfo = NullPOPSotpRequestInfo);
	bool IsEnterOk(void) const {return m_bEnterOk;}
	mycp::bigint GetOwnerAccount(void) const;
	mycp::bigint GetP2POwnerAccount(void) const {return m_nP2PTryOwnerAccount;}

	mycp::bigint GetChatId(void) const {return m_pChatInfo->GetChatId();}
	mycp::bigint GetCallId(void) const {return m_pChatInfo->GetCallId();}
	CEBChatInfo::pointer GetChatInfo(void) const;
	CChatMsgInfo::pointer GetChatMsgInfo(mycp::bigint nMsgId, bool erase) const;
	void DelChatMsgInfo(mycp::bigint nMsgId);
	void GetFileSpeechInfo(mycp::bigint nMsgId,CSpeechInfo & pOutSpeechInfo,int &pOutFiles);

	bool doSendDSAck(const CChatMsgInfo::pointer& pChatMsgInfo);
	bool IsKilled(void) const {return m_bKilled;}
	void DoProcess(void);
	void DoProcess2(unsigned int nTenMSIndex);
	void DoResponse(void);
	int SendDSSend(mycp::bigint nMsgId, const cgcAttachment::pointer& pAttach, bool bDisableZip, int nAttachEncoding=0, mycp::bigint nZipSize=0);
	int SendDSCheck(mycp::bigint nMsgId,bool bNeedAck,CPOPSotpRequestInfo::pointer pRequestInfo);

	bool IsMsgSender(const CChatMsgInfo::pointer & pChatMsgInfo) const;
	mycp::bigint GetMsgSender(const CChatMsgInfo::pointer & pChatMsgInfo) const;

private:
	int SendDSAck(mycp::bigint nMsgId, const tstring & sOS, bool bNeedAck);
	int SendDSAck(mycp::bigint nMsgId);

	void ProcP2PTry(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	//void ProcFCMP2P(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcFCMExit(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcCMMAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcFCMMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcCMMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcFCMAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcFDSSend(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,bool bP2PMsg);
	void ProcessMsgSendOk(const CPOPSotpRequestInfo::pointer & pRequestInfo,const CChatMsgInfo::pointer& pChatMsgInfo, int nResultValue);
	void ProcDSCheck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcDSSend(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcDSAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	void ProcFUMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	//void ProcCRSet(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	//void ProcFCMNotify(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnCgcResponse(const cgcParserSotp & response);
	short m_nActiveTimeoutCount;
	virtual void OnActiveTimeout(void);

	bool DelSendFileThread(mycp::bigint nMsgId);

	bool m_bEnterOk;
	mycp::bigint m_sOwnerAccount;
	int m_nLogonType;
	//tstring m_sChatId;
	CLockMap<unsigned long, CPOPSotpRequestInfo::pointer> m_pRequestList;			// callid->
	CLockList<CPOPSotpRequestInfo::pointer> m_pReqResponseList;
	CLockList<CPOPSotpResponseInfo::pointer> m_pResponseList;
	//CLockMap<tstring, CChatMsgInfo::pointer> m_pChatMsgList;						// msgid->
	//CLockMap<tstring, CEBChatInfo::pointer> m_pChatList;								// chat-id->
	CEBChatInfo::pointer m_pChatInfo;
	CLockMap<mycp::bigint, CSendFileThread::pointer> m_pSendFileThreadList;					// msgid->
	CLockMap<mycp::bigint, CSpeechInfo> m_pFilePercent;					// msgid->

	int				m_nProcessTimer;
	bool			m_bKilled;
	boost::shared_ptr<boost::thread> m_pResponseThread;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	CPOPCMHandler *	m_pPOPHandler;
	std::string		m_sParameter;
	mycp::bigint		m_nBigData;
	bool			m_bAutoAckSuccess;
};

} // namespace entboost
