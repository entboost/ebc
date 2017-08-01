#include "EBRtpManager.h"
//#include "eb_defines.h"

namespace entboost {

CEBRtpManager::CEBRtpManager(CEBRtpHangler * pPOPHandler,bool bProcessTimer)
: m_bProcessTimer(bProcessTimer)
, m_bKilled(false)
//, m_pResponseThread(NULL)
, m_pPOPHandler(pPOPHandler)
, m_nParameter(0)
, m_bRDOn(false)

{
	m_nP2PTryOwnerAccount = 0;
	m_nP2PData = 0;
	m_nOnP2POk = -1;
}

CEBRtpManager::~CEBRtpManager(void)
{
	Stop();
}

//void CEBRtpManager::response_thread_svr(void* lparam)
//{
//	CEBRtpManager * pSvr = (CEBRtpManager*)lparam;
//	pSvr->DoResponse();
//}
void CEBRtpManager::process_thread_svr(void)
{
	while (true)
	{
		try
		{
			DoProcess();
		}catch(std::exception&)
		{}catch(...)
		{}
		if (IsKilled())
		{
			break;
		}
#ifdef WIN32
		Sleep(1000);
#else
		usleep(1000000);
#endif
	}
}
int CEBRtpManager::Start(const CCgcAddress & address, const tstring& sAppName, unsigned int bindPort,int nUsesSsl, int nMaxWaitSecons)
{
	if (!StartClient(address, bindPort, 500))
		return -1;
	sotp()->doSetResponseHandler((CgcClientHandler*)this);
	sotp()->doSetAppName(sAppName);	// EB_APP_NAME_RD_SERVER
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_SOTP_VERSION,SOTP_PROTO_VERSION_21);
	//sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,nUsesSsl);
	//sotp()->doSetConfig(SOTP_CLIENT_CONFIG_ACCEPT_ENCODING,SOTP_DATA_ENCODING_GZIP|SOTP_DATA_ENCODING_DEFLATE);
	sotp()->doSendOpenSession();
	sotp()->doSetEncoding(_T("UTF8"));
	sotp()->doStartActiveThread(8);	// 通过这个心跳线程，实现定期重新注册

	m_bKilled = false;
	//if (m_pResponseThread == NULL)
	//	m_pResponseThread = new boost::thread(attrs,boost::bind(response_thread_svr, (void*)this));
	if (m_pProcessThread.get() == NULL)
	{
		boost::thread_attributes attrs;
		attrs.set_stack_size(CGC_THREAD_STACK_MIN);
		m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CEBRtpManager::process_thread_svr, this)));
	}
	return 0;
}
bool CEBRtpManager::IsStart(void) const
{
	return IsClientStarted();
}
void CEBRtpManager::Stop(void)
{
	StopClient();
	m_bKilled = true;
	//if (m_pResponseThread != NULL)
	//{
	//	m_pResponseThread->join();
	//	delete m_pResponseThread;
	//	m_pResponseThread = NULL;
	//}
	if (m_pProcessThread.get() != NULL)
	{
		m_pProcessThread->join();
		m_pProcessThread.reset();
	}

	m_bRDOn = false;
}
void CEBRtpManager::SetEncoding(const std::string& v)
{
	if (IsClientStarted())
	{
		sotp()->doSetEncoding(v);
	}
}
void CEBRtpManager::DoProcess(void)
{
	unsigned int nIndex = 0;
	while (!m_bKilled)
	{
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
		if ((nIndex++)%100!=99)
			continue;
		// process timeout
		{
			// 处理有已经返回数据，超过10S没有新数据，删除requestlist，用于提高效率
			BoostWriteLock wtlock(m_pReqResponseList.mutex());
			CLockList<CPOPSotpRequestInfo::pointer>::iterator pIterReqResponstList = m_pReqResponseList.begin();
			for (; pIterReqResponstList!=m_pReqResponseList.end(); )
			{
				const CPOPSotpRequestInfo::pointer& pRequestInfo = *pIterReqResponstList;
				if (pRequestInfo->GetResponseTime()>0 && pRequestInfo->GetRequestTime() + 10 < time(0))
				{
					m_pRequestList.remove(pRequestInfo->GetCallId());
					m_pReqResponseList.erase(pIterReqResponstList++);
				}else
				{
					pIterReqResponstList++;
				}
			}
		}

		// process timeout
		try
		{
			if (m_bProcessTimer)
				m_pPOPHandler->OnProcessTimer(this);
		}catch(std::exception&)
		{}catch(...)
		{}

		{
			BoostReadLock rdlock(m_pRequestList.mutex());
			CLockMap<unsigned long, CPOPSotpRequestInfo::pointer>::iterator pIter;
			for (pIter=m_pRequestList.begin(); pIter!=m_pRequestList.end(); pIter++)
			{
				const CPOPSotpRequestInfo::pointer& pRequestInfo = pIter->second;
				if (pRequestInfo->GetResponseTime()==0 && pRequestInfo->GetRequestTime() + 20 < time(0))
				{
					// timeout
					const unsigned long nCid = pIter->first;
					const CPOPSotpRequestInfo::pointer pRequestInfoTemp = pRequestInfo;
					rdlock.unlock();
					m_pRequestList.remove(nCid);
					m_pPOPHandler->OnTimeout(pRequestInfoTemp,this);
					break;
				}
			}
		}
	}
}

void CEBRtpManager::OnRtpFrame(mycp::bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount, mycp::uint32 nUserData)
{
	if (m_pPOPHandler!=NULL)
		m_pPOPHandler->OnRtpFrame(nSrcId, pRtpFrame, nLostCount, nUserData,this);
}

int CEBRtpManager::SetSourceId(mycp::bigint nSrcId)
{
	if (!IsClientStarted()) return -1;
	sotp()->doSetRtpSourceId(nSrcId);
	return 0;
}
mycp::bigint CEBRtpManager::GetSourceId(void) const
{
	if (!IsClientStarted()) return 0;
	return sotp()->doGetRtpSourceId();
}

int CEBRtpManager::RegisterSource(mycp::bigint nRoomId, mycp::bigint nParam)
{
	if (!IsClientStarted()) return -1;
	//if (!m_bRDOn)
	//{
	//	return this->SendRDOn(nRoomId,sotp()->doGetRtpSourceId(),nParam,EB_LOGON_TYPE_PC);
	//}
	return sotp()->doRegisterSource(nRoomId, nParam)?0:-2;
}
int CEBRtpManager::UnRegisterSource(mycp::bigint nRoomId)
{
	if (!IsClientStarted()) return -1;
	sotp()->doUnRegisterSource(nRoomId);
	return 0;
}
int CEBRtpManager::UnRegisterAllSource(void)
{
	if (!IsClientStarted()) return -1;
	sotp()->doUnRegisterAllSource();
	return 0;
}
int CEBRtpManager::RegisterSink(mycp::bigint nRoomId, mycp::bigint nDestId)
{
	if (!IsClientStarted()) return -1;
	return sotp()->doRegisterSink(nRoomId, nDestId)?0:-2;
}
int CEBRtpManager::UnRegisterSink(mycp::bigint nRoomId, mycp::bigint nDestId)
{
	if (!IsClientStarted()) return -1;
	sotp()->doUnRegisterSink(nRoomId, nDestId);
	return 0;
}
int CEBRtpManager::UnRegisterAllSink(mycp::bigint nRoomId)
{
	if (!IsClientStarted()) return -1;
	sotp()->doUnRegisterAllSink(nRoomId);
	return 0;
}
int CEBRtpManager::UnRegisterAllSink(void)
{
	if (!IsClientStarted()) return -1;
	sotp()->doUnRegisterAllSink();
	return 0;
}
int CEBRtpManager::SendRtpData(mycp::bigint nRoomId,const unsigned char* pData,mycp::uint32 nSize,mycp::uint32 nTimestamp,mycp::uint8 nDataType,mycp::uint8 nNAKType)
{
	if (!IsClientStarted()) return -1;
	sotp()->doSendRtpData(nRoomId, pData, nSize, nTimestamp, nDataType, nNAKType);
	return 0;
}

int CEBRtpManager::SendRDOn(mycp::bigint nId,mycp::bigint nFromUserId,mycp::bigint nParam, int LogonType, CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;

	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("id", nId));
	sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUserId));
	sotp()->doAddParameter(CGC_PARAMETER("param", nParam));
	sotp()->doAddParameter(CGC_PARAMETER("type", LogonType));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_RTP_ON, EB_CALL_NAME_RTP_ON, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBRtpManager::SendRDOff(mycp::bigint nId,mycp::bigint nFromUserId,mycp::bigint nParam, CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;

	this->SendCloseSession();

	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("id", nId));
	sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUserId));
	sotp()->doAddParameter(CGC_PARAMETER("param", nParam));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_RTP_OFF, EB_CALL_NAME_RTP_OFF, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
void CEBRtpManager::SendCloseSession(void)
{
	if (!IsClientStarted()) return;
	sotp()->doSendCloseSession();
}

void CEBRtpManager::SetDestAddress(int nUserIndex,const std::string& sToAddress)
{
	if (!IsClientStarted()) return;
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_CURRENT_INDEX,nUserIndex);
	sotp()->doSetRemoteAddr(sToAddress);
	if (m_nP2PTryOwnerAccount>0)
		SendP2PTest(3);
}
int CEBRtpManager::SendP2PRequest(mycp::bigint nFromUid,mycp::bigint sChatId,mycp::bigint nToAccount,const tstring& sLocapIp,int nRequestData)
{
	if (!IsClientStarted()) return -1;
	//m_pChatInfo->SetChatId(sChatId);
	//m_pChatInfo->m_bAccountChat = (bool)(nFromUid==sChatId);
	//if (!m_pChatInfo->m_pChatUserList.exist(nFromUid))
	//{
	//	m_pChatInfo->m_pChatUserList.insert(nFromUid, CEBAccountInfo::create(nFromUid,""));
	//}
	//if (!m_pChatInfo->m_pChatUserList.exist(nToAccount))
	//{
	//	m_pChatInfo->m_pChatUserList.insert(nToAccount, CEBAccountInfo::create(nToAccount,""));
	//}
	if (m_nP2PTryOwnerAccount==0)
	{
		m_nP2PTryOwnerAccount = nFromUid;
		//sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,EB_MAX_HEART_BEAT_SECOND);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,32*1024);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,128);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,128);
	}
	//if (m_sOwnerAccount==0)
	//	m_sOwnerAccount = m_nP2PTryOwnerAccount;

	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUid));
	sotp()->doAddParameter(CGC_PARAMETER("to-uid", nToAccount));
	sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
	sotp()->doAddParameter(CGC_PARAMETER("local-ip", sLocapIp),false);
	sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nRequestData));
	if (sotp()->doSendAppCall(EB_SIGN_P2P_REQUEST, EB_CALL_NAME_P2P_REQ,true))
	{
		//if (pRequestInfo.get() != NULL)
		//{
		//	pRequestInfo->SetRequestTime();
		//	pRequestInfo->SetCallId(nCallId);
		//	m_pRequestList.insert(nCallId, pRequestInfo);
		//}
		return 0;
	}
	return -2;
}
int CEBRtpManager::SendP2PResponse(mycp::bigint nFromUid,mycp::bigint sChatId,mycp::bigint nToAccount,const tstring& sLocapIp,int nResponseData,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	//m_pChatInfo->SetChatId(sChatId);
	//m_pChatInfo->m_bAccountChat = (bool)(nFromUid==sChatId);
	//if (!m_pChatInfo->m_pChatUserList.exist(nFromUid))
	//{
	//	m_pChatInfo->m_pChatUserList.insert(nFromUid, CEBAccountInfo::create(nFromUid,""));
	//}
	//if (!m_pChatInfo->m_pChatUserList.exist(nToAccount))
	//{
	//	m_pChatInfo->m_pChatUserList.insert(nToAccount, CEBAccountInfo::create(nToAccount,""));
	//}
	if (m_nP2PTryOwnerAccount==0)
	{
		m_nP2PTryOwnerAccount = nFromUid;
		//sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,EB_MAX_HEART_BEAT_SECOND);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,32*1024);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,128);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,128);
	}
	//if (m_sOwnerAccount==0)
	//	m_sOwnerAccount = m_nP2PTryOwnerAccount;

	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUid));
	sotp()->doAddParameter(CGC_PARAMETER("to-uid", nToAccount));
	sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
	sotp()->doAddParameter(CGC_PARAMETER("local-ip", sLocapIp),false);
	sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nResponseData));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_P2P_RESPONSE, EB_CALL_NAME_P2P_RES,true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBRtpManager::SendP2PTry(mycp::bigint sCallId,mycp::bigint sChatId,mycp::bigint nFromAccount,int nTryCount,int nP2PData)
{
	if (!IsClientStarted()) return -1;
	//m_pChatInfo->SetChatId(sChatId);
	//m_pChatInfo->SetCallId(sCallId);
	if (m_nP2PTryOwnerAccount==0)
	{
		m_nP2PTryOwnerAccount = nFromAccount;
		//sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND/2,EB_MAX_HEART_BEAT_SECOND/2);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,32*1024);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,128);
		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,128);
	}
	//if (m_sOwnerAccount==0)
	//	m_sOwnerAccount = m_nP2PTryOwnerAccount;
	m_nP2PData = nP2PData;

	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from-uid", m_nP2PTryOwnerAccount));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
	sotp()->doAddParameter(CGC_PARAMETER("try-count", nTryCount));
	sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nP2PData));
	if (sotp()->doSendAppCall(EB_SIGN_P2P_TRY, EB_CALL_NAME_P2P_TRY,true))
	{
		//if (pRequestInfo.get() != NULL)
		//{
		//	pRequestInfo->SetRequestTime();
		//	pRequestInfo->SetCallId(nCallId);
		//	m_pRequestList.insert(nCallId, pRequestInfo);
		//}
		return 0;
	}
	return -2;
}
void CEBRtpManager::SendP2PTest(unsigned short nTestCount)
{
	if (!IsClientStarted()) return;
	sotp()->doSendP2PTry(nTestCount);
}

void CEBRtpManager::SetTranSpeedLimit(int nLimit)
{
	if (!IsClientStarted()) return;
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_TRAN_SPEED_LIMIT,nLimit);
}
void CEBRtpManager::SetRtpCallbackData(mycp::uint32 nUserData)
{
	if (!IsClientStarted()) return;
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_RTP_CB_USERDATA,nUserData);
}

unsigned short CEBRtpManager::GetLocalPort(void) const
{
	if (!IsClientStarted()) return 0;
	return sotp()->doGetLocalPort();
}

void CEBRtpManager::ProcP2PTry(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
	// 收到对方P2P请求
	const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
	const mycp::bigint sCallId = pResponseInfo->m_pResponseList.getParameterValue("call-id", (mycp::bigint)0);
	const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
	const int nTryCount = pResponseInfo->m_pResponseList.getParameterValue("try-count", (int)0);
	const int nP2PData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
	if (nTryCount < 5)
	{
		// 回复P2PTRY
		// nFromAccount=0：表示使用当前from用户；
		if (m_nOnP2POk==1 && nTryCount<=1)	// 0,1，重新请求；
			m_nOnP2POk = -1;
		SendP2PTry(sCallId,sChatId,0,nTryCount+1,nP2PData);
	}else if (m_nOnP2POk!=1)
	{
		// 已经打通P2P
		//m_nOnP2POk = 0;
		//if (!m_sSslPublicKey.empty() && !m_sSslPrivateKey.empty() && !m_sSslPrivatePwd.empty())
		//{
		//	unsigned int nHasSslPassword = 0;
		//	sotp()->doGetConfig(SOTP_CLIENT_CONFIG_HAS_SSL_PASSWORD,&nHasSslPassword);
		//	if (nHasSslPassword==0)
		//	{
		//		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PUBLIC_KEY,(unsigned int)m_sSslPublicKey.c_str());
		//		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_KEY,(unsigned int)m_sSslPrivateKey.c_str());
		//		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_PWD,(unsigned int)m_sSslPrivatePwd.c_str());
		//		sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,1);
		//		if (sFromAccount>m_nP2PTryOwnerAccount)
		//		{
		//			// 发送SSL证书，获取通讯密码sslpassword，对方也会生成相同密码
		//			sotp()->doSendOpenSession(0);
		//		}
		//		return;
		//	}
		//}
		m_nOnP2POk = 1;
		for (int i=0;i<3;i++)
		{
			// 发送多次，确保对方能收到，保证二边互通；
			SendP2PTry(sCallId,sChatId,m_nP2PTryOwnerAccount,nTryCount+1,nP2PData);
#ifdef WIN32
			Sleep(5);
#else
			usleep(5000);
#endif
		}

		sotp()->doStartActiveThread(15,10);
		//sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,10);
		m_pPOPHandler->OnP2POk(pRequestInfo, pResponseInfo,this);
	}
}

void CEBRtpManager::OnCgcResponse(const cgcParserSotp & response)
{
	const long nSotpCallId = response.getCallid();
	const long nSotpSign = response.getSign();
	const long nResultValue = response.getResultValue();
	// -102: 错误模块代码
	if (nResultValue < 0)
	//if (nResultValue == -102 || nResultValue == -103)
	{
		m_pPOPHandler->OnInvalidateSession(nResultValue,this);
		return;
	}
//	//m_nActiveTimeoutCount = 0;
//	if (m_nOnP2POk==0 && m_pChatInfo.get()!=NULL && response.isResulted() && response.isOpenType())
//	{
//		for (int i=0;i<3;i++)
//		{
//			// 发送多次，确保对方能收到，保证二边互通；
//			SendP2PTry(m_pChatInfo->GetCallId(),m_pChatInfo->GetChatId(),0,5,m_nP2PData);
//#ifdef WIN32
//			Sleep(5);
//#else
//			usleep(5000);
//#endif
//		}
//	}

	CPOPSotpRequestInfo::pointer pRequestInfo;
	//m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo, true);
	if (m_pRequestList.find(nSotpCallId, pRequestInfo))
	{
		if (pRequestInfo->GetResponseTime()==0)
		{
			m_pReqResponseList.add(pRequestInfo);
		}
		pRequestInfo->SetResponseTime();
	}
	switch (response.getSign())
	{
	case EB_SIGN_P2P_RESPONSE:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//if (m_pResponseThread==NULL)
				m_pPOPHandler->OnCMP2PResponseRes(pRequestInfo, pResponseInfo,this);
			//else
			//	m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_P2P_TRY:
		{
			// 收到对方P2P请求
			if (response.getFunctionName()==EB_CALL_NAME_P2P_TRY)
			{
				//MessageBox(NULL,"EB_SIGN_P2P_TRY","EB_SIGN_P2P_TRY",MB_OK);
				CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("try-count", response.getRecvParameterValue("try-count", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
				ProcP2PTry(pRequestInfo,pResponseInfo);
			}
		}break;
	case EB_SIGN_FP2P_REQUEST:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("local-ip", response.getRecvParameterValue("local-ip", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("remote-ip", response.getRecvParameterValue("remote-ip", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
			//if (m_pResponseThread==NULL)
				m_pPOPHandler->OnFCMP2PRequest(pRequestInfo, pResponseInfo,this);
			//else
			//	m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FP2P_RESPONSE:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("local-ip", response.getRecvParameterValue("local-ip", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("remote-ip", response.getRecvParameterValue("remote-ip", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
			//if (m_pResponseThread==NULL)
				m_pPOPHandler->OnFCMP2PResponse(pRequestInfo, pResponseInfo,this);
			//else
			//	m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_RTP_ON:
		{
			if (nResultValue==EB_STATE_OK)
				m_bRDOn = true;
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnRDOnResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_RTP_OFF:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnRDOffResponse(pRequestInfo, pResponseInfo,this);
			m_bRDOn = false;
		}break;
	//case EB_SIGN_FU_MSG:
	//	{
	//		CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
	//		//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from", response.getRecvParameterValue("from", "")));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-content", response.getRecvParameterValue("msg-content", "")));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
	//		//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
	//		//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emo-count", response.getRecvParameterValue("emo-count", (int)-1)));
	//		//m_pPOPHandler->OnFUMIUMsg(pRequestInfo, pResponseInfo, this);
	//		//m_pResponseList.add(pResponseInfo);
	//		if (m_pResponseThread==NULL)
	//			ProcFUMsg(pRequestInfo, pResponseInfo);
	//		else
	//			m_pResponseList.add(pResponseInfo);
	//	}break;
	default:
		break;
	}
}

} // namespace entboost
