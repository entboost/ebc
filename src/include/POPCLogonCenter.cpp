#ifdef WIN32
#pragma warning(disable:4819 4267)
#endif // WIN32
#include "POPCLogonCenter.h"
#include "eb_defines.h"
#include "md5.h"

namespace entboost {

CPOPCLogonCenter::CPOPCLogonCenter(CPOPLCHandler * pPOPHandler,bool bProcessTimer)
: m_bProcessTimer(bProcessTimer)
, m_bKilled(false)
//, m_pResponseThread(NULL)
, m_pPOPHandler(pPOPHandler)
, m_nLogonType(EB_LOGON_TYPE_UNKNOWN)
, m_nLineState(EB_LINE_STATE_UNKNOWN)
, m_nParameter(0)

{
	m_sAppId = 0;
}

CPOPCLogonCenter::~CPOPCLogonCenter(void)
{
	Stop();
}

//void CPOPCLogonCenter::response_thread_svr(void* lparam)
//{
//	CPOPCLogonCenter * pSvr = (CPOPCLogonCenter*)lparam;
//	pSvr->DoResponse();
//}
void CPOPCLogonCenter::process_thread_svr(void)
{
	try
	{
		DoProcess();
	}catch(std::exception&)
	{}catch(...)
	{}
}
int CPOPCLogonCenter::Start(const CCgcAddress & address, unsigned int bindPort,int nUsesSsl, short nMaxWaitSecons, bool bBuildProcessThread)
{
	if (!StartClient(address, bindPort))
		return -1;
	sotp()->doSetResponseHandler((CgcClientHandler*)this);
	sotp()->doSetAppName(POP_APP_NAME_LOGON_CENTER);
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_SOTP_VERSION,SOTP_PROTO_VERSION_21);
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,nUsesSsl);
	//sotp()->doSetConfig(SOTP_CLIENT_CONFIG_ACCEPT_ENCODING,SOTP_DATA_ENCODING_GZIP|SOTP_DATA_ENCODING_DEFLATE);
	sotp()->doSendOpenSession(nMaxWaitSecons);
#ifndef _QT_MAKE_
	sotp()->doSetEncoding(_T("UTF8"));
#endif
	sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND);	// 这是短连接，外面会自动关闭

	m_bKilled = false;
	boost::thread_attributes attrs;
    attrs.set_stack_size(CGC_THREAD_STACK_MIN);
	//if (m_pResponseThread == NULL)
	//	m_pResponseThread = new boost::thread(attrs,boost::bind(response_thread_svr, (void*)this));
	if (bBuildProcessThread && m_pProcessThread.get() == NULL)
		m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CPOPCLogonCenter::process_thread_svr, this)));
	return 0;
}
bool CPOPCLogonCenter::IsStart(void) const
{
	return IsClientStarted();
}
void CPOPCLogonCenter::Stop(void)
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
}
void CPOPCLogonCenter::SetEncoding(const std::string& v)
{
	if (IsClientStarted())
	{
		sotp()->doSetEncoding(v);
	}
}
void CPOPCLogonCenter::DoProcess(void)
{
	unsigned int nIndex = 0;
	while (!m_bKilled)
	{
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
		if (((nIndex++)%100)!=99)
			continue;
		// process timeout
		{
			// 处理有已经返回数据，超过10S没有新数据，删除requestlist，用于提高效率
			BoostWriteLock wtlock(m_pReqResponseList.mutex());
			CLockList<CPOPSotpRequestInfo::pointer>::iterator pIterReqResponstList = m_pReqResponseList.begin();
			for (; pIterReqResponstList!=m_pReqResponseList.end(); )
			{
				const CPOPSotpRequestInfo::pointer& pRequestInfo = *pIterReqResponstList;
				if (pRequestInfo->GetResponseTime()>0 && pRequestInfo->GetResponseTime() + 10 < time(0))
				{
					m_pRequestList.remove(pRequestInfo->GetCallId());
					m_pReqResponseList.erase(pIterReqResponstList++);
				}else
				{
					pIterReqResponstList++;
				}
			}
		}

		bool bContinue = m_pRequestList.empty()?false:true;
		while (bContinue)
		{
			bContinue = false;
			BoostReadLock rdlock(m_pRequestList.mutex());
			CLockMap<unsigned long, CPOPSotpRequestInfo::pointer>::iterator pIter;
			for (pIter=m_pRequestList.begin(); pIter!=m_pRequestList.end(); pIter++)
			{
				const CPOPSotpRequestInfo::pointer& pRequestInfo = pIter->second;
				if (pRequestInfo->GetResponseTime()==0 && (pRequestInfo->GetRequestTime() + pRequestInfo->GetRequestTimeout()) < time(0))
				{
					// timeout
					const unsigned long nCid = pIter->first;
					const CPOPSotpRequestInfo::pointer pRequestInfoTemp = pRequestInfo;
					rdlock.unlock();
					m_pRequestList.remove(nCid);
					m_pPOPHandler->OnTimeout(pRequestInfoTemp,this);
					bContinue = m_pRequestList.empty()?false:true;
					break;
				}
			}
		}
	}
}

//void CPOPCLogonCenter::DoResponse(void)
//{
//	while (!m_bKilled)
//	{
//		CPOPSotpResponseInfo::pointer pResponseInfo;
//		if (!m_pResponseList.front(pResponseInfo))
//		{
//#ifdef WIN32
//			Sleep(10);
//#else
//			usleep(10000);
//#endif
//			continue;
//		}
//
//		CPOPSotpRequestInfo::pointer pRequestInfo;
//		m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo, true);
//		const int nResultValue = pResponseInfo->GetResultValue();
//		if (nResultValue == -103)
//		{
//			m_pPOPHandler->OnInvalidateSession(this);
//			continue;
//		}
//		m_pPOPHandler->OnResultValue(nResultValue,this);
//
//		switch (pResponseInfo->GetCallSign())
//		{
//		case EB_SIGN_L_QUERY:
//			{
//				m_pPOPHandler->OnLCULQueryResponse(pRequestInfo, pResponseInfo,this);
//			}break;
//		case EB_SIGN_L_LOGON:
//			{
//				if (nResultValue == EB_STATE_OK)
//				{
//					int nOAuthType = pResponseInfo->m_pResponseList.getParameterValue("oauth_type", (int)0);
//					if (nOAuthType != 0)
//						m_nLogonType = nOAuthType;
//					m_nLogonAccount = pResponseInfo->m_pResponseList.getParameterValue("account", "");
//					if ((m_nLogonType&EB_LOGON_TYPE_VISITOR)==EB_LOGON_TYPE_VISITOR)
//					{
//						m_nUserName = m_nLogonAccount;
//					}else
//					{
//						m_nUserName = pResponseInfo->m_pResponseList.getParameterValue("username", "");
//					}
//				}else if (nResultValue != EB_STATE_OAUTH_FORWARD)
//				{
//					m_nLogonType = EB_LOGON_TYPE_UNKNOWN;
//					m_nLogonPassword.clear();
//					m_sAppId.clear();
//				}
//				m_pPOPHandler->OnLCULLogonResponse(pRequestInfo, pResponseInfo,this);
//			}break;
//		default:
//			break;
//		}
//	}
//}

void CPOPCLogonCenter::OnCgcResponse(const cgcParserSotp & response)
{
	const long nSotpCallId = response.getCallid();
	//const long nSotpSign = response.getSign();
	const long nResultValue = response.getResultValue();
	if (nResultValue < 0)
	//if (nResultValue == -102 || nResultValue == -103)
	{
		m_pPOPHandler->OnInvalidateSession(nResultValue,this);
		return;
	}else if (response.isSessionProto() && response.isOpenType() && response.isResulted())
	{
		m_pPOPHandler->OnSessionOpen(nResultValue,response.getSid(),this);
		return;
	}
	m_pPOPHandler->OnResultValue(nResultValue,this);
	CPOPSotpRequestInfo::pointer pRequestInfo;
	//if (m_pRequestList.find(nSotpCallId, pRequestInfo, true))
	if (m_pRequestList.find(nSotpCallId, pRequestInfo))
	{
		if (pRequestInfo->GetResponseTime()==0)
		{
			m_pReqResponseList.add(pRequestInfo);
		}
		pRequestInfo->SetResponseTime();
	}

	//// '-103': invalidate session handle
	//if (nResultValue == -103)
	//{
	//	CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
	//	m_pResponseList.add(pResponseInfo);
	//	return ;
	//}

	// ???需要判断，哪些是多次返回数据的；

	switch (response.getSign())
	{
	case EB_SIGN_P_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("pk", response.getRecvParameterValue("pk", (mycp::bigint)0)));
			m_pPOPHandler->OnLCULPInfoResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_Q_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("license", response.getRecvParameterValue("license", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-start-time", response.getRecvParameterValue("l-start-time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-end-time", response.getRecvParameterValue("l-end-time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-count", response.getRecvParameterValue("l-count", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-user", response.getRecvParameterValue("l-user", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-pc", response.getRecvParameterValue("l-pc", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-android", response.getRecvParameterValue("l-android", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("l-ios", response.getRecvParameterValue("l-ios", (int)0)));
			
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("s-id", response.getRecvParameterValue("s-id", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("s-state", response.getRecvParameterValue("s-state", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("s-capacity", response.getRecvParameterValue("s-capacity", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("s-online-time", response.getRecvParameterValue("s-online-time", "")));
			m_pPOPHandler->OnLCULQInfoResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_L_RESETPWD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnLCResetPwdResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_L_FINDPWD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (eb::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("key", response.getRecvParameterValue("key", "")));
			m_pPOPHandler->OnLCFindPwdResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_L_REGAUTH:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnLCRegAuthResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_L_QUERY:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (eb::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account-type", response.getRecvParameterValue("account-type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_code", response.getRecvParameterValue("dep_code", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-appname", response.getRecvParameterValue("um-appname", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-server", response.getRecvParameterValue("um-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-http-server", response.getRecvParameterValue("um-http-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-key", response.getRecvParameterValue("um-key", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
				m_pPOPHandler->OnLCULQueryResponse(pRequestInfo, pResponseInfo,this);
			//m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_L_LOGON:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (nResultValue == EB_STATE_OAUTH_FORWARD)
			{
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("req_code", response.getRecvParameterValue("req_code", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("oauth_url", response.getRecvParameterValue("oauth_url", "")));
			}else if (nResultValue == EB_STATE_OK)
			{
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("req_code", response.getRecvParameterValue("req_code", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("oauth_type", response.getRecvParameterValue("oauth_type", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("app-name", response.getRecvParameterValue("app-name", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (eb::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("http-address", response.getRecvParameterValue("http-address", "")));
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("http-address", response.getRecvParameterValue("http-address", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("online-key", response.getRecvParameterValue("online-key", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("app-online-key", response.getRecvParameterValue("app-online-key", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("lc-list", response.getRecvParameterValue("lc-list", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("lc-http-list", response.getRecvParameterValue("lc-http-list", "")));
				// 客户端应用
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("system-setting", response.getRecvParameterValue("system-setting", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account-prefix", response.getRecvParameterValue("account-prefix", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("open-register", response.getRecvParameterValue("open-register", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("license-user", response.getRecvParameterValue("license-user", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("open-visitor", response.getRecvParameterValue("open-visitor", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("forget-pwd-url", response.getRecvParameterValue("forget-pwd-url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("register-url", response.getRecvParameterValue("register-url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-logo-url", response.getRecvParameterValue("ent-logo-url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("product-name", response.getRecvParameterValue("product-name", "")));
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("send-reg-mail", response.getRecvParameterValue("send-reg-mail", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("passwd-auth-mode", response.getRecvParameterValue("passwd-auth-mode", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("acm-key", response.getRecvParameterValue("acm-key", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("username", response.getRecvParameterValue("username", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area1", response.getRecvParameterValue("area1", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area1s", response.getRecvParameterValue("area1s", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area2", response.getRecvParameterValue("area2", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area2s", response.getRecvParameterValue("area2s", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area3", response.getRecvParameterValue("area3", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area3s", response.getRecvParameterValue("area3s", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area4", response.getRecvParameterValue("area4", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("area4s", response.getRecvParameterValue("area4s", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("add", response.getRecvParameterValue("add", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("url", response.getRecvParameterValue("url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("gender", response.getRecvParameterValue("gender", (int)EB_GENDER_UNKNOWN)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("tel", response.getRecvParameterValue("tel", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("mobile", response.getRecvParameterValue("mobile", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("birthday", response.getRecvParameterValue("birthday", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("zipcode", response.getRecvParameterValue("zipcode", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("setting", response.getRecvParameterValue("setting", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("default_emp", response.getRecvParameterValue("default_emp", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-rid", response.getRecvParameterValue("head-rid", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-cm-server", response.getRecvParameterValue("head-cm-server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-http-server", response.getRecvParameterValue("head-http-server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-md5", response.getRecvParameterValue("head-md5", "")));
				// 
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("deploy-id", response.getRecvParameterValue("deploy-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("license-type", response.getRecvParameterValue("license-type", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auto-open-subid", response.getRecvParameterValue("auto-open-subid", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("save-conversations", response.getRecvParameterValue("save-conversations", (int)3)));
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("group-msg-subid", response.getRecvParameterValue("group-msg-subid", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("conversations-url", response.getRecvParameterValue("conversations-url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("server-version", response.getRecvParameterValue("server-version", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("default-url", response.getRecvParameterValue("default-url", "")));
			}
			if (nResultValue == EB_STATE_OK)
			{
				int nOAuthType = pResponseInfo->m_pResponseList.getParameterValue("oauth_type", (int)0);
				if (nOAuthType != 0)
					m_nLogonType = nOAuthType;
				m_nLogonAccount = pResponseInfo->m_pResponseList.getParameterValue("account", "");
				if ((m_nLogonType&EB_LOGON_TYPE_VISITOR)==EB_LOGON_TYPE_VISITOR)
				{
					m_nUserName = m_nLogonAccount;
				}else
				{
					m_nUserName = pResponseInfo->m_pResponseList.getParameterValue("username", "");
				}
			}else if (nResultValue != EB_STATE_OAUTH_FORWARD)
			{
				m_nLogonType = EB_LOGON_TYPE_UNKNOWN;
				m_nLogonPassword.clear();
				m_sAppId = 0;
			}
			m_pPOPHandler->OnLCULLogonResponse(pRequestInfo, pResponseInfo,this);
			//m_pResponseList.add(pResponseInfo);
		}break;
	default:
		break;
	}
}

//int CPOPCLogonCenter::SendLCQuery(mycp::bigint nPhone,const tstring & sAccount,int nReg,CPOPSotpRequestInfo::pointer pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	if (nPhone > 0)
//		sotp()->doAddParameter(CGC_PARAMETER("phone", nPhone));
//	sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
//	if (nReg == 1)
//		sotp()->doAddParameter(CGC_PARAMETER("reg", nReg));
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_L_QUERY, EB_CALL_NAME_LC_QUERY, true, constNullAttchment, &nCallId))
//	{
//		if (pRequestInfo.get() != NULL)
//		{
//			pRequestInfo->SetRequestTime();
//			pRequestInfo->SetCallId(nCallId);
//			m_pRequestList.insert(nCallId, pRequestInfo);
//		}
//		return 0;
//	}
//	return -2;
//}

int CPOPCLogonCenter::SendLCQueryInfo(const tstring& sSystemKey,int nQueryLicenseInfo,int nQueryServiceId,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (sSystemKey.empty()) return -1;
	sotp()->doBeginCallLock();
	if (sSystemKey!="1")	// ???test
	sotp()->doAddParameter(CGC_PARAMETER("system-key", sSystemKey),false);
	if (nQueryLicenseInfo==1)
		sotp()->doAddParameter(CGC_PARAMETER("q-license", 1));
	if (nQueryServiceId>0)
		sotp()->doAddParameter(CGC_PARAMETER("q-serviceid", nQueryServiceId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_Q_INFO, EB_CALL_NAME_Q_INFO, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCLogonCenter::SendLCPutInfo(mycp::bigint nPutKey,EB_PUT_INFO_TYPE nPutType,const tstring& sPutValue,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("pk", nPutKey));
	sotp()->doAddParameter(CGC_PARAMETER("pt", (int)nPutType));
	sotp()->doAddParameter(CGC_PARAMETER("pv", sPutValue),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_P_INFO, EB_CALL_NAME_P_INFO, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

//int CPOPCLogonCenter::SendLCReq(const tstring & sAccount,CPOPSotpRequestInfo::pointer pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_L_QUERY, EB_CALL_NAME_LC_QUERY, true, constNullAttchment, &nCallId))
//	{
//		if (pRequestInfo.get() != NULL)
//		{
//			pRequestInfo->SetRequestTime();
//			pRequestInfo->SetCallId(nCallId);
//			m_pRequestList.insert(nCallId, pRequestInfo);
//		}
//		return 0;
//	}
//	return -2;
//}
int CPOPCLogonCenter::SendLCQuery(mycp::bigint nFromIpAddress,mycp::bigint sAppId,const tstring& sAppOk,EB_REQEUST_TYPE nReqType,int nSubType,mycp::bigint nFromUid,const tstring& sCallKey,mycp::bigint nMemberId,
								  mycp::bigint sDepCode,const tstring & sAccount,CPOPSotpRequestInfo::pointer pRequestInfo)
//int CPOPCLogonCenter::SendLCQuery(mycp::bigint nFromUid,const tstring& sCallKey,mycp::bigint nMemberId,mycp::bigint sDepCode,
//								  const tstring & sAccount,int nReg,int nNeedFromInfo,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (sAppId>0)
		sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk),false);		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("rt", (int)nReqType));
	if (nSubType!=0)
		sotp()->doAddParameter(CGC_PARAMETER("st", (int)nSubType));
	sotp()->doAddParameter(CGC_PARAMETER("call_key", sCallKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
	if (nFromUid>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFromUid));
	//if (nReg == 1)
	//	sotp()->doAddParameter(CGC_PARAMETER("reg", nReg));
	//if (nNeedFromInfo==1)
	//	sotp()->doAddParameter(CGC_PARAMETER("need_f_info", nNeedFromInfo));
	if (sDepCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", sDepCode));
	if (nMemberId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("member_code", nMemberId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_QUERY, EB_CALL_NAME_LC_QUERY, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCLogonCenter::SendLCLogon(const tstring& sUserData, int nSDKVersion,mycp::bigint nFromIpAddress,const CPOPLogonInfo::pointer& pLogonInfo,mycp::bigint sAppId,const tstring& sAppOk,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	BOOST_ASSERT (pLogonInfo.get() != 0);
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (!sUserData.empty())
		sotp()->doAddParameter(CGC_PARAMETER("us_data", sUserData));
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (nSDKVersion!=0)	// 支持负数
		sotp()->doAddParameter(CGC_PARAMETER("sdk-ver", nSDKVersion));
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)pLogonInfo->m_nLogonType));
	if (pLogonInfo->m_nUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("uid", pLogonInfo->m_nUserId));			// ** 1.12新版本；
	else if (!pLogonInfo->m_sAccount.empty())
		sotp()->doAddParameter(CGC_PARAMETER("account", pLogonInfo->m_sAccount));
	//if (pLogonInfo->m_nUserId>0)
	//{
	//	char lpszAccount[24];
	//	sprintf(lpszAccount,"%lld",pLogonInfo->m_nUserId);
	//	sotp()->doAddParameter(CGC_PARAMETER("account", (tstring)lpszAccount));
	//}else
	//	sotp()->doAddParameter(CGC_PARAMETER("account", pLogonInfo->m_sAccount),false);
	if (!pLogonInfo->m_sReqCode.empty())
		sotp()->doAddParameter(CGC_PARAMETER("req_code", pLogonInfo->m_sReqCode));	// 第三方验证有用
	if (!pLogonInfo->m_sACMKey.empty())
		sotp()->doAddParameter(CGC_PARAMETER("acm-key", pLogonInfo->m_sACMKey));		// 重新登录
	if (sAppId>0)
		sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk),false);		// 登录有用
	if (!pLogonInfo->m_sOAuthKey.empty())
		sotp()->doAddParameter(CGC_PARAMETER("oauth_key", pLogonInfo->m_sOAuthKey));
	if ((pLogonInfo->m_nLogonType&EB_LOGON_TYPE_OAUTH)==EB_LOGON_TYPE_OAUTH)
	{
		//tstring sOAuthKey = "";
		//if (pLogonInfo->m_nUserId>0 && !pLogonInfo->m_sPassword.empty())
		//{
		//}
		if ((pLogonInfo->m_nUserId>0 || !pLogonInfo->m_sAccount.empty()) && !pLogonInfo->m_sPassword.empty()) // **32表示已经加密过
		{
			// 用于验证用户帐号密码
			if (pLogonInfo->m_sPassword.size()==32 ||
				pLogonInfo->m_nPasswdAuthMode==PASSWD_AUTH_MODE_PBKDF2 ||
				(pLogonInfo->m_nLogonType&EB_LOGON_TYPE_MAIL_TEST)!=0)
			{
				sotp()->doAddParameter(CGC_PARAMETER("pwd", pLogonInfo->m_sPassword));
			}else
			{
				entboost::MD5 md5;
				tstring strtemp;
				if (pLogonInfo->m_nPasswdAuthMode==PASSWD_AUTH_MODE_MD5)// && (pLogonInfo->m_nLogonType&EB_LOGON_TYPE_EMAIL)==EB_LOGON_TYPE_EMAIL)
				{
					// md5(passwd)
					strtemp = pLogonInfo->m_sPassword;
				}else
				{
					if (pLogonInfo->m_nUserId>0)
					{
						// 使用UID
						char lpszBuffer[128];
						sprintf(lpszBuffer,"%lld%s",pLogonInfo->m_nUserId,pLogonInfo->m_sPassword.c_str());
						strtemp = lpszBuffer;
					}else
					{
						strtemp = pLogonInfo->m_sAccount+pLogonInfo->m_sPassword;
					}
					//// 恩布默认（所有UID和PHONE密码）
					////strtemp.append(pLogonInfo->m_sAccount+pLogonInfo->m_sPassword);
					//// 使用UID
					//char lpszBuffer[128];
					//sprintf(lpszBuffer,"%lld%s",pLogonInfo->m_nUserId,pLogonInfo->m_sPassword.c_str());
					//strtemp = lpszBuffer;
				}
				md5.update((const unsigned char*)strtemp.c_str(),(unsigned int)strtemp.size());
				md5.finalize();
				const tstring sPassword = md5.hex_digest();
				sotp()->doAddParameter(CGC_PARAMETER("pwd", sPassword));
			}
		}
	}else if ((pLogonInfo->m_nLogonType&EB_LOGON_TYPE_APPID)==EB_LOGON_TYPE_APPID)
	{
		if (pLogonInfo->m_sAppId>0)
			sotp()->doAddParameter(CGC_PARAMETER("app_id", pLogonInfo->m_sAppId));
		entboost::MD5 md5;
		char strtemp[128];
		sprintf(strtemp,"%lld%s",pLogonInfo->m_sAppId,pLogonInfo->m_sPassword.c_str());
		md5.update((const unsigned char*)strtemp,(unsigned int)strlen(strtemp));
		md5.finalize();
		const tstring sPassword = md5.hex_digest();
		sotp()->doAddParameter(CGC_PARAMETER("pwd", sPassword));
	}else if (pLogonInfo->m_sACMKey.empty() && (pLogonInfo->m_nLogonType&EB_LOGON_TYPE_VISITOR)!=EB_LOGON_TYPE_VISITOR)
	{
		if (pLogonInfo->m_sPassword.size()==32 ||
			pLogonInfo->m_nPasswdAuthMode==PASSWD_AUTH_MODE_PBKDF2 ||
			(pLogonInfo->m_nLogonType&EB_LOGON_TYPE_MAIL_TEST)!=0)
		{
			// 已经加密
			sotp()->doAddParameter(CGC_PARAMETER("pwd", pLogonInfo->m_sPassword));
		}else if (!pLogonInfo->m_sPassword.empty())
		{
			entboost::MD5 md5;
			tstring strtemp;
			if (pLogonInfo->m_nPasswdAuthMode==PASSWD_AUTH_MODE_MD5)
			{
				// md5(passwd)
				strtemp = pLogonInfo->m_sPassword;
			}else
			{
				// 恩布默认
				if (pLogonInfo->m_nUserId>0)
				{
					// 使用UID
					char lpszBuffer[128];
					sprintf(lpszBuffer,"%lld%s",pLogonInfo->m_nUserId,pLogonInfo->m_sPassword.c_str());
					strtemp = lpszBuffer;
				}else
				{
					strtemp = pLogonInfo->m_sAccount+pLogonInfo->m_sPassword;
				}
			}
			md5.update((const unsigned char*)strtemp.c_str(),(unsigned int)strtemp.size());
			md5.finalize();
			const tstring sPassword = md5.hex_digest();
			sotp()->doAddParameter(CGC_PARAMETER("pwd", sPassword));
		}
	}

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_LOGON, EB_CALL_NAME_LC_LOGON, true))
	{
		m_nLogonType = pLogonInfo->m_nLogonType;
		m_nLineState = pLogonInfo->m_nLineState;
		m_nLogonPassword = pLogonInfo->m_sPassword;
		m_sAppId = pLogonInfo->m_sAppId;
		m_sReqCode = pLogonInfo->m_sReqCode;
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCLogonCenter::SendLCAuthAppId(int nSDKVersion,mycp::bigint nFromIpAddress,const CPOPLogonInfo::pointer& pLogonInfo,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	BOOST_ASSERT (pLogonInfo.get() != 0);
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (nSDKVersion!=0)	// 支持负数
		sotp()->doAddParameter(CGC_PARAMETER("sdk-ver", nSDKVersion));
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)pLogonInfo->m_nLogonType));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", pLogonInfo->m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("pwd", pLogonInfo->m_sPassword));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_LOGON, EB_CALL_NAME_LC_LOGON, true))
	{
		m_nLogonType = pLogonInfo->m_nLogonType;
		m_nLineState = pLogonInfo->m_nLineState;
		m_nLogonPassword = pLogonInfo->m_sPassword;
		m_sAppId = pLogonInfo->m_sAppId;
		m_sReqCode = pLogonInfo->m_sReqCode;
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCLogonCenter::SendLCRegAuth(const tstring& sAuthKey, mycp::bigint nUserId,int nRegCode, const tstring& sResetPwd,int nAuthOnly,mycp::bigint sAppId,const tstring& sAppOk,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("auth_key", sAuthKey),false);
	if (sAppId>0)
		sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk),false);		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("reg_code", nRegCode));
	sotp()->doAddParameter(CGC_PARAMETER("reset_pwd", sResetPwd),false);
	if (nAuthOnly==1)
		sotp()->doAddParameter(CGC_PARAMETER("auth_only", 1));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_REGAUTH, EB_CALL_NAME_LC_REGAUTH, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCLogonCenter::SendLCFindPwd(mycp::bigint nFromIpAddress,const tstring& sAccount,mycp::bigint sAppId,const tstring& sAppOk,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (sAppId>0)
		sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk),false);		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_FINDPWD, EB_CALL_NAME_LC_FINDPWD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCLogonCenter::SendLCResetPwd(mycp::bigint nFromIpAddress,const tstring& sFindKey,mycp::bigint nUserId,const tstring& sAccount,
									 const tstring& sCurrentPwd,const tstring& sResetPwd,const tstring& sSalt,mycp::bigint sAppId,const tstring& sAppOk,CPOPSotpRequestInfo::pointer pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (sAppId>0)
		sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));		// 登录有用
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk),false);		// 登录有用
	if (nUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	else
		sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
	sotp()->doAddParameter(CGC_PARAMETER("cur_pwd", sCurrentPwd),false);
	sotp()->doAddParameter(CGC_PARAMETER("key", sFindKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("reset_pwd", sResetPwd),false);
	sotp()->doAddParameter(CGC_PARAMETER("salt", sSalt),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_L_RESETPWD, EB_CALL_NAME_LC_RESETPWD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

} // namespace entboost
