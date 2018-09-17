#ifdef WIN32
#pragma warning(disable:4819)
#endif // WIN32
//#include "StdAfx.h"
#include "EBCAppCenter.h"
#include "../include/eb_defines.h"

namespace entboost {

CEBCAppCenter::CEBCAppCenter(CEBAPHandler * pPOPHandler,int bProcessTimer)
: m_bProcessTimer(bProcessTimer)
, m_bKilled(false)
, m_pPOPHandler(pPOPHandler)

{
	m_sAppId = 0;
	m_nBigData = 0;
	m_nActiveTimeoutCount = 0;
}

CEBCAppCenter::~CEBCAppCenter(void)
{
	Stop();
}

//void CEBCAppCenter::response_thread_svr(void* lparam)
//{
//	CEBCAppCenter * pSvr = (CEBCAppCenter*)lparam;
//	pSvr->DoResponse();
//}
void CEBCAppCenter::process_thread_svr(void)
{
	try
	{
		DoProcess();
	}catch(std::exception&)
	{}catch(...)
	{}
}
// ???改成单一证书，提高性能；

int CEBCAppCenter::Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort, int nUsesSsl)
{
	if (!StartClient(address, bindPort))
		return -1;
	sotp()->doSetResponseHandler((CgcClientHandler*)this);
	if (sAppName.empty())
		sotp()->doSetAppName(POP_APP_NAME_APP_CENTER);
	else
		sotp()->doSetAppName(sAppName);
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_SOTP_VERSION,SOTP_PROTO_VERSION_21);
	sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,nUsesSsl);
	//sotp()->doSetConfig(SOTP_CLIENT_CONFIG_ACCEPT_ENCODING,SOTP_DATA_ENCODING_GZIP|SOTP_DATA_ENCODING_DEFLATE);
	sotp()->doSendOpenSession();
#ifndef _QT_MAKE_
	sotp()->doSetEncoding(_T("UTF8"));
#endif
	sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND);

	m_bKilled = false;
	//if (m_pResponseThread == NULL)
	//	m_pResponseThread = new boost::thread(attrs,boost::bind(response_thread_svr, (void*)this));
    if (m_pProcessThread.get() == 0) {
        m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CEBCAppCenter::process_thread_svr, this)));
//        boost::thread_attributes attrs;
//        attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//        m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CEBCAppCenter::process_thread_svr, this)));
    }
	return 0;
}
bool CEBCAppCenter::IsStart(void) const
{
	return IsClientStarted();
}
void CEBCAppCenter::Stop(void)
{
	StopClient();
	m_bKilled = true;
	if (m_pResponseThread.get() != NULL)
	{
		m_pResponseThread->join();
		m_pResponseThread.reset();
	}
	if (m_pProcessThread.get() != NULL)
	{
		m_pProcessThread->join();
		m_pProcessThread.reset();
	}
}
void CEBCAppCenter::DoProcess(void)
{
	unsigned int nIndex = 0;
	unsigned int theSecond = 0;
	while (!m_bKilled)
	{
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
		if (((nIndex++)%100)!=99)
			continue;

		theSecond++;
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

		try
		{
			if (m_bProcessTimer>0 && (theSecond%m_bProcessTimer)==0)
				m_pPOPHandler->OnProcessTimer(this);
		}catch(std::exception&)
		{}catch(...)
		{}

		// process timeout
		{
			BoostReadLock rdlock(m_pRequestList.mutex());
			CLockMap<unsigned long, CPOPSotpRequestInfo::pointer>::iterator pIter;
			for (pIter=m_pRequestList.begin(); pIter!=m_pRequestList.end(); pIter++)
			{
				const CPOPSotpRequestInfo::pointer& pRequestInfo = pIter->second;
				if (pRequestInfo->GetResponseTime()==0 && pRequestInfo->GetRequestTime() + 20 < time(0))
				//if (pRequestInfo->GetRequestTime() + 18 < time(0))
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

//void CEBCAppCenter::DoResponse(void)
//{
//	while (!m_bKilled)
//	{
//		CPOPSotpResponseInfo::pointer pResponseInfo;
//		if (!m_pResponseList.front(pResponseInfo))
//		{
//			Sleep(10);
//			continue;
//		}
//
//		CPOPSotpRequestInfo::pointer pRequestInfo;
//		if (m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo))
//		{
//			if (pRequestInfo->GetResponseTime()==0)
//			{
//				m_pReqResponseList.add(pRequestInfo);
//			}
//			pRequestInfo->SetResponseTime();
//		}
//		const int nResultValue = pResponseInfo->GetResultValue();
//		if (nResultValue < 0)
//		//if (nResultValue == -103)
//		{
//			m_pPOPHandler->OnInvalidateSession(nResultValue,this);
//			continue;
//		}
//		m_pPOPHandler->OnResultValue(nResultValue,this);
//
//		switch (pResponseInfo->GetCallSign())
//		{
//		case EB_SIGN_FA_MSG:
//			{
//				m_pPOPHandler->OnAPFAMsg(pRequestInfo, pResponseInfo,this);
//			}break;
//		case EB_SIGN_A_MACK:
//			{
//				m_pPOPHandler->OnAPAMackResponse(pRequestInfo, pResponseInfo,this);
//			}break;
//		case EB_SIGN_A_MSG:
//			{
//				m_pPOPHandler->OnAPMsgResponse(pRequestInfo, pResponseInfo,this);
//			}break;
//		case EB_SIGN_A_OFF:
//			{
//				m_pPOPHandler->OnAPAOffResponse(pRequestInfo, pResponseInfo,this);
//				m_sAppId = "";
//				m_sAppOnlineKey = "";
//			}break;
//		case EB_SIGN_A_ON:
//			{
//				if (nResultValue == EB_STATE_OK && pRequestInfo.get() != NULL)
//				{
//					m_sAppId = pRequestInfo->m_pRequestList.getParameterValue("ap-app-id", "");
//					m_sAppOnlineKey = pRequestInfo->m_pRequestList.getParameterValue("ap-app-online-key", "");
//				}
//				m_pPOPHandler->OnAPAOnResponse(pRequestInfo, pResponseInfo,this);
//			}break;
//		default:
//			break;
//		}
//	}
//}

void CEBCAppCenter::OnActiveTimeout(void)
{
	// -200：stoped
	m_nActiveTimeoutCount++;
	if (m_nActiveTimeoutCount >= 2)
	{
		m_pPOPHandler->OnInvalidateSession(-200,this);
		m_nActiveTimeoutCount = 0;
	}
}

void CEBCAppCenter::OnCgcResponse(const cgcParserSotp & response)
{
	const long nSotpCallId = response.getCallid();
	//const unsigned long nSotpSign = response.getSign();
	const long nResultValue = response.getResultValue();
	// '-103': invalidate session handle
	if (nResultValue < 0)
	//if (nResultValue == -103)
	{
		m_pPOPHandler->OnInvalidateSession(nResultValue,this);
		//CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
		//m_pResponseList.add(pResponseInfo);
		return ;
	}
	m_nActiveTimeoutCount = 0;
	CPOPSotpRequestInfo::pointer pRequestInfo;
	if (m_pRequestList.find(nSotpCallId, pRequestInfo))
	{
		if (pRequestInfo->GetResponseTime()==0)
		{
			m_pReqResponseList.add(pRequestInfo);
		}
		pRequestInfo->SetResponseTime();
	}
	m_pPOPHandler->OnResultValue(nResultValue,this);

	switch (response.getSign())
	{
	case EB_SIGN_EXE_JOB:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("job_exec_id", response.getRecvParameterValue("job_exec_id", (mycp::bigint)0)));
			m_pPOPHandler->OnExeJobResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_SYS_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnSysEditResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_SYNC_USERINFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnSyncUserInfoResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_SYNC_EDITPWD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnSyncEditPwdResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_FA_MSG:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_app_id", response.getRecvParameterValue("from_app_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg_id", response.getRecvParameterValue("msg_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("type", response.getRecvParameterValue("type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("name", response.getRecvParameterValue("name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("content", response.getRecvParameterValue("content", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ext_data", response.getRecvParameterValue("ext_data", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("time", response.getRecvParameterValue("time", "")));
			m_pPOPHandler->OnAPFAMsg(pRequestInfo, pResponseInfo,this);
			//m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_A_MSG:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg_id", response.getRecvParameterValue("msg_id", (mycp::bigint)0)));
				m_pPOPHandler->OnAPMsgResponse(pRequestInfo, pResponseInfo,this);
			//m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_AU_MAIL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnAPAUMail(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_AU_MSG:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnAPAUMsg(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_A_MACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnAPAMackResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_A_ON:
		{
			if (nResultValue == EB_STATE_OK && pRequestInfo.get() != NULL)
			{
				m_sAppId = pRequestInfo->m_pRequestList.getParameterValue("ap-app-id", (mycp::bigint)0);
				m_sAppOnlineKey = pRequestInfo->m_pRequestList.getParameterValue("ap-app-online-key", "");
			}
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("enable_level", response.getRecvParameterValue("enable_level", (int)0)));
			m_pPOPHandler->OnAPAOnResponse(pRequestInfo, pResponseInfo,this);
		}break;
	case EB_SIGN_A_OFF:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			m_pPOPHandler->OnAPAOffResponse(pRequestInfo, pResponseInfo,this);
			m_sAppId = 0;
			m_sAppOnlineKey = "";
			//m_pResponseList.add(pResponseInfo);
		}break;
	default:
		break;
	}
}

int CEBCAppCenter::SendAOn(mycp::bigint nFromAddress,mycp::bigint sAppId,const tstring& sAppOnlineKey, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;

	CPOPSotpRequestInfo::pointer pRequestInfo2 = pRequestInfo.get()!=NULL?pRequestInfo:CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_LOGON);
	pRequestInfo2->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-id", sAppId));
	pRequestInfo2->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-online-key", sAppOnlineKey));

	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_online_key", sAppOnlineKey));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	pRequestInfo2->SetCallId(nCallId);
	m_pRequestList.insert(nCallId, pRequestInfo2);
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_A_ON, EB_CALL_NAME_A_ON, true))
	{
		//if (pRequestInfo.get() == NULL)
		//{
		//	CPOPSotpRequestInfo::pointer pRequestInfo2 = CPOPSotpRequestInfo::create(nCallId, EB_REQ_TYPE_LOGON);
		//	pRequestInfo2->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-id", sAppId));
		//	pRequestInfo2->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-online-key", sAppOnlineKey));
		//	m_pRequestList.insert(nCallId, pRequestInfo2);
		//}else
		//{
		//	pRequestInfo->SetResponseTime(0);
		//	pRequestInfo->SetRequestTime();
		//	pRequestInfo->SetCallId(nCallId);
		//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-id", sAppId));
		//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ap-app-online-key", sAppOnlineKey));
		//	m_pRequestList.insert(nCallId, pRequestInfo);
		//}
		return 0;
	}
	m_pRequestList.remove(nCallId);
	return -2;
}
int CEBCAppCenter::SendAOff(mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_online_key", m_sAppOnlineKey));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_A_OFF, EB_CALL_NAME_A_OFF, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CEBCAppCenter::SendAMsg(mycp::bigint nFromAddress,const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	if (pAPMsgInfo.m_nUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("uid", pAPMsgInfo.m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("account", pAPMsgInfo.m_sAccount),false);
	sotp()->doAddParameter(CGC_PARAMETER("to_app_id", pAPMsgInfo.m_sFromAppId));
	if (pAPMsgInfo.m_nMsgType!=0)
		sotp()->doAddParameter(CGC_PARAMETER("type", pAPMsgInfo.m_nMsgType));
	sotp()->doAddParameter(CGC_PARAMETER("name", pAPMsgInfo.m_sMsgName),false);
	sotp()->doAddParameter(CGC_PARAMETER("content", pAPMsgInfo.m_sMsgContent),false);
	if (pAPMsgInfo.m_nExtData!=0)
		sotp()->doAddParameter(CGC_PARAMETER("ext_data", pAPMsgInfo.m_nExtData));
	sotp()->doAddParameter(CGC_PARAMETER("off_save", bSaveOffMsg?1:0));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_A_MSG, EB_CALL_NAME_A_MSG, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CEBCAppCenter::SendExecJob(bool bCheckLimitTime,mycp::bigint nJobId,const tstring& sJobExecParams,const tstring& sFromString,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("job_id", nJobId));
	sotp()->doAddParameter(CGC_PARAMETER("exec_params", sJobExecParams),false);
	sotp()->doAddParameter(CGC_PARAMETER("from_desc", sFromString),false);
	if (bCheckLimitTime)
		sotp()->doAddParameter(CGC_PARAMETER("check_limit_time", 1));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_EXE_JOB, EB_CALL_NAME_EXE_JOB, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBCAppCenter::SendAUMsg(const tstring& sCallKey,const EB_APMsgInfo& pAPMsgInfo,const tstring& sToEntIds,const tstring& sToDepIds,int nInclude_sub_group,const tstring& sToAccount,
							 mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("call_key", sCallKey),false);
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("to_ent_id", sToEntIds),false);
	sotp()->doAddParameter(CGC_PARAMETER("to_dep_id", sToDepIds),false);
	if (nInclude_sub_group==1 && !sToDepIds.empty())
		sotp()->doAddParameter(CGC_PARAMETER("include_sub_group", 1));

	//if (nToEntId!=0)
	//	sotp()->doAddParameter(CGC_PARAMETER("to_ent_id", nToEntId));
	//if (nToDepId!=0)
	//{
	//	sotp()->doAddParameter(CGC_PARAMETER("to_dep_id", nToDepId));
	//	if (nInclude_sub_group==1)
	//		sotp()->doAddParameter(CGC_PARAMETER("include_sub_group", 1));
	//}
	sotp()->doAddParameter(CGC_PARAMETER("to_account", sToAccount),false);
	if (pAPMsgInfo.m_nMsgType!=0)
		sotp()->doAddParameter(CGC_PARAMETER("type", pAPMsgInfo.m_nMsgType));
	sotp()->doAddParameter(CGC_PARAMETER("title", pAPMsgInfo.m_sMsgName),false);
	sotp()->doAddParameter(CGC_PARAMETER("content", pAPMsgInfo.m_sMsgContent),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_AU_MSG, EB_CALL_NAME_AU_MSG, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CEBCAppCenter::SendAUMail(const tstring& sCallKey,const EB_APMsgInfo& pAPMsgInfo,mycp::bigint nToEntId,mycp::bigint nToDepId,const tstring& sToAccount,
							 mycp::bigint nFromAddress,int nOffSave, int nAuthAccount,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("call_key", sCallKey),false);
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	if (pAPMsgInfo.m_sFromUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_uid", pAPMsgInfo.m_sFromUserId));
	if (nToEntId!=0)
		sotp()->doAddParameter(CGC_PARAMETER("to_ent_id", nToEntId));
	if (nToDepId!=0)
		sotp()->doAddParameter(CGC_PARAMETER("to_dep_id", nToDepId));
	sotp()->doAddParameter(CGC_PARAMETER("to_account", sToAccount),false);
	if (pAPMsgInfo.m_nMsgType!=0)
		sotp()->doAddParameter(CGC_PARAMETER("type", pAPMsgInfo.m_nMsgType));
	sotp()->doAddParameter(CGC_PARAMETER("name", pAPMsgInfo.m_sMsgName),false);
	sotp()->doAddParameter(CGC_PARAMETER("content", pAPMsgInfo.m_sMsgContent),false);
	sotp()->doAddParameter(CGC_PARAMETER("off_save", nOffSave));
	sotp()->doAddParameter(CGC_PARAMETER("auth_account", nAuthAccount));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_AU_MAIL, EB_CALL_NAME_AU_MAIL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CEBCAppCenter::SendAMAck(mycp::bigint nMsgId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (m_sAppId==0) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_online_key", m_sAppOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("msg_id", nMsgId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_A_MACK, EB_CALL_NAME_A_MACK, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBCAppCenter::SendSyncEditPwd(mycp::bigint nUserId,const tstring& sOldPwd,const tstring& sNewPwd,const tstring& sUserSalt,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (nUserId==0 || sOldPwd.empty() || sNewPwd.empty()) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("new_pwd", sNewPwd));
	sotp()->doAddParameter(CGC_PARAMETER("old_pwd", sOldPwd),false);
	sotp()->doAddParameter(CGC_PARAMETER("salt", sUserSalt),false);
	//if (nRebuildAccountPrefix==1)
	//	sotp()->doAddParameter(CGC_PARAMETER("rebuild_account_prefix", 1));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_SYNC_EDITPWD, EB_CALL_NAME_SYNC_EDITPWD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBCAppCenter::SendSyncUserInfo(mycp::bigint nGroupId, mycp::bigint nUserId,const CPopParameterList& pInfoList,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (nUserId==0) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	if (nGroupId>0)
		sotp()->doAddParameter(CGC_PARAMETER("group_id", nGroupId));
	sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	sotp()->doAddParameter(pInfoList.getParameter("sync_flag"),false);
	sotp()->doAddParameter(pInfoList.getParameter("username"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_SYNC_USERINFO, EB_CALL_NAME_SYNC_USERINFO, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CEBCAppCenter::SendSysEdit(mycp::bigint nFromAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromAddress!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", m_sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", m_sAppOnlineKey));
	sotp()->doAddParameter(pInfoList.getParameter("user-pwd-callback-url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("forget-pwd-url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("register-url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("ent-logo-url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("open-register"),false);
	sotp()->doAddParameter(pInfoList.getParameter("open-visitor"),false);
	sotp()->doAddParameter(pInfoList.getParameter("callback-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("callback-enc-mode"));
	//sotp()->doAddParameter(pInfoList.getParameter("callback-enc-data"),false);
	sotp()->doAddParameter(pInfoList.getParameter("resend-error-cb-data"),false);
	//sotp()->doAddParameter(pInfoList.getParameter("init-my-ip"),false);
	sotp()->doAddParameter(pInfoList.getParameter("product-name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("account-prefix"),false);
	sotp()->doAddParameter(pInfoList.getParameter("default-pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("passwd-auth-mode"),false);
	sotp()->doAddParameter(pInfoList.getParameter("build-all-default-pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("build-mgr-default-pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("save-conversations"),false);
	sotp()->doAddParameter(pInfoList.getParameter("invite-add2group-auth"),false);
	sotp()->doAddParameter(pInfoList.getParameter("member-change-event"),false);
	sotp()->doAddParameter(pInfoList.getParameter("offfile-space-size"),false);
	sotp()->doAddParameter(pInfoList.getParameter("onlinemsg-save-days"),false);
	sotp()->doAddParameter(pInfoList.getParameter("offlinemsg-save-days"),false);
	sotp()->doAddParameter(pInfoList.getParameter("offlinefile-save-days"),false);
	sotp()->doAddParameter(pInfoList.getParameter("user-cd-space-size"),false);
	sotp()->doAddParameter(pInfoList.getParameter("group-cd-space-size"),false);
	sotp()->doAddParameter(pInfoList.getParameter("department-cd-space-size"),false);
	sotp()->doAddParameter(pInfoList.getParameter("default-allow-out-chat"),false);
	sotp()->doAddParameter(pInfoList.getParameter("default-accept-out-chat"),false);
	sotp()->doAddParameter(pInfoList.getParameter("max-retry-count"),false);
	sotp()->doAddParameter(pInfoList.getParameter("temp-lock-minute"),false);
	sotp()->doAddParameter(pInfoList.getParameter("delete-ent-member-op"),false);
	sotp()->doAddParameter(pInfoList.getParameter("add-contact-auth"),false);
	sotp()->doAddParameter(pInfoList.getParameter("max-ug-count"),false);
	sotp()->doAddParameter(pInfoList.getParameter("max-contact-count"),false);
	sotp()->doAddParameter(pInfoList.getParameter("uses-uid-list"),false);
	sotp()->doAddParameter(pInfoList.getParameter("empty-uid-list-auto"),false);
	sotp()->doAddParameter(pInfoList.getParameter("auto-open-subid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("autohide-mainframe-pc"),false);
	sotp()->doAddParameter(pInfoList.getParameter("hide-mainframe-pc"),false);
	sotp()->doAddParameter(pInfoList.getParameter("open-ent-mgr"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-user-cloud-drive"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-group-shared-cloud"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-modify-passwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-video"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-remote-desktop"),false);
	// *** 以下属于 业务相关数据
	//sotp()->doAddParameter(pInfoList.getParameter("attend-start-date"),false);	// *** 由IM系统设置，不开放接口配置
	sotp()->doAddParameter(pInfoList.getParameter("attend-daily-job-date"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetResponseTime(0);
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_SYS_EDIT, EB_CALL_NAME_SYS_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

} // namespace entboost
