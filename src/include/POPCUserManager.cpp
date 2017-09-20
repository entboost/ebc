#ifdef WIN32
#pragma warning(disable:4819 4267)
#endif // WIN32
//#include "StdAfx.h"
#include "POPCUserManager.h"
#include "eb_defines.h"
//#ifdef WIN32
//#include <Mmsystem.h>
//#else
//#include <time.h>   
//inline unsigned long timeGetTime()  
//{  
//	unsigned long uptime = 0;  
//	struct timespec on;  
//	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)  
//		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;  
//	return uptime;  
//} 
//#endif
namespace entboost {

CPOPCUserManager::CPOPCUserManager(CPOPUMHandler * pPOPHandler, int bProcessTimer)
: m_bProcessTimer(bProcessTimer)
, m_bKilled(false)
, m_pPOPHandler(pPOPHandler)
, m_nUserId(0)
, m_nBigData(0)
{
	m_sAppId = 0;
	m_nActiveTimeoutCount = 0;
}

CPOPCUserManager::~CPOPCUserManager(void)
{
	Stop();
}
void CPOPCUserManager::response_thread_svr(void)
{
	while (true)
	{
		try
		{
			DoResponse();
		}catch(std::exception&)
		{}catch(...)
		{}
		if (m_bKilled)
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
void CPOPCUserManager::process_thread_svr(void)
{
	while (true)
	{
		try
		{
			DoProcess();
		}catch(std::exception&)
		{}catch(...)
		{}
		if (m_bKilled)
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
int CPOPCUserManager::Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort,bool bBuildResponseThread,bool bBuildProcessThread,int nUsesSsl)
{
	if (!StartClient(address, bindPort, 600))
		return -1;
	sotp()->doSetResponseHandler((CgcClientHandler*)this);
	if (sAppName.empty())
		sotp()->doSetAppName(POP_APP_NAME_USER_MANAGER);
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
//	boost::thread_attributes attrs;
//    attrs.set_stack_size(CGC_THREAD_STACK_MIN);
	if (bBuildResponseThread)
	{
        if (m_pResponseThread.get() == 0) {
            m_pResponseThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CPOPCUserManager::response_thread_svr, this)));
//			m_pResponseThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CPOPCUserManager::response_thread_svr, this)));
        }
	}
	if (bBuildProcessThread)
	{
        if (m_pProcessThread.get() == 0) {
            m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CPOPCUserManager::process_thread_svr, this)));
//			m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CPOPCUserManager::process_thread_svr, this)));
        }
	}
	return 0;
}
void CPOPCUserManager::SetEncoding(const std::string& v)
{
	if (IsClientStarted())
	{
		sotp()->doSetEncoding(v);
	}
}
void CPOPCUserManager::Stop(void)
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
	m_pPOPHandler = NULL;
}
void CPOPCUserManager::DoProcess(void)
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
		if (((nIndex++)%100)==99)
		{
			DoProcess2(theSecond++);
		}
	}
}
void CPOPCUserManager::DoProcess2(unsigned int nIndex)
{
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

	// process timeout
	try
	{
		if (m_bProcessTimer>0 && (nIndex%m_bProcessTimer)==0)
			m_pPOPHandler->OnProcessTimer(this);
	}catch(std::exception&)
	{}catch(...)
	{}

	bool bContinue = m_pRequestList.empty()?false:true;
	while (bContinue)
	{
		bContinue = false;
		BoostReadLock rdlock(m_pRequestList.mutex());
		CLockMap<unsigned long, CPOPSotpRequestInfo::pointer>::iterator pIter=m_pRequestList.begin();
		for (; pIter!=m_pRequestList.end(); pIter++)
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
				bContinue = m_pRequestList.empty()?false:true;
				break;
			}
		}
	}
}
void CPOPCUserManager::DoResponse(void)
{
	while (!m_bKilled)
	{
		CPOPSotpResponseInfo::pointer pResponseInfo;
		if (!m_pResponseList.front(pResponseInfo))
		{
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
			continue;
		}

		CPOPSotpRequestInfo::pointer pRequestInfo;
		if (m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo))
		{
			if (pRequestInfo->GetResponseTime()==0)
			{
				m_pReqResponseList.add(pRequestInfo);
			}
			pRequestInfo->SetResponseTime();
		}
		//const int nResultValue = pResponseInfo->GetResultValue();
		//// '-103': invalidate session handle
		//// -117 ： 服务器退出，更新
		//if (nResultValue < 0)
		//{
		//	m_pPOPHandler->OnInvalidateSession(this);
		//	continue;
		//}
		//m_pPOPHandler->OnResultValue(nResultValue,this);

		switch (pResponseInfo->GetCallSign())
		{
		//case EB_SIGN_DICT_LOAD:
		//	m_pPOPHandler->OnUGEditResponse(pRequestInfo, pResponseInfo, this);
		//	break;
		case EB_SIGN_UG_EDIT:
			m_pPOPHandler->OnUGEditResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_UG_DEL:
			m_pPOPHandler->OnUGDeleteResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_UG_LOAD:
			m_pPOPHandler->OnUGInfo(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_CS_DEL:
			m_pPOPHandler->OnCSDelResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_CS_ADD:
			m_pPOPHandler->OnCSAddResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_MACK:
			m_pPOPHandler->OnUMackResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_R_INFO:
			{
				m_pPOPHandler->OnResInfo(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_R_DEL:
			{
				m_pPOPHandler->OnResDeleteResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_R_EDIT:
			{
				m_pPOPHandler->OnResEditResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_VER_CHECK:
			{
				m_pPOPHandler->OnVersionCheckResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_V_REQUEST:
			m_pPOPHandler->OnVRequestResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_V_ACK:
			m_pPOPHandler->OnVAckResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FV_REQUEST:
			m_pPOPHandler->OnFVRequest(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FV_ACK:
			m_pPOPHandler->OnFVAck(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FV_END:
			m_pPOPHandler->OnFVEnd(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_RD_REQUEST:
			m_pPOPHandler->OnRDRequestResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_RD_ACK:
			m_pPOPHandler->OnRDAckResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FRD_REQUEST:
			m_pPOPHandler->OnFRDRequest(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FRD_ACK:
			m_pPOPHandler->OnFRDAck(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FRD_END:
			m_pPOPHandler->OnFRDEnd(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_AUTH:
			m_pPOPHandler->OnFuncAuthResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_REQ:
			m_pPOPHandler->OnFuncReqResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_LOAD:
			m_pPOPHandler->OnFuncLoadResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_EDIT:
			m_pPOPHandler->OnFuncEditResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_SETICON:
			m_pPOPHandler->OnFuncSetIconResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_DEL:
			m_pPOPHandler->OnFuncDelResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FUNC_SUB:
			m_pPOPHandler->OnFuncSubResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FNAV_SET:
			m_pPOPHandler->OnFuncNavSetResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FNAV_DEL:
			m_pPOPHandler->OnFuncNavDelResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_FNAV_LOAD:
			m_pPOPHandler->OnFuncNavLoadResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_ENT_LOAD:
			m_pPOPHandler->OnEntLoadResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_ENT_EDIT:
			{
				m_pPOPHandler->OnUMEMEntEditResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_EMP_DEL:
			{
				m_pPOPHandler->OnUMEMEmpDeleteResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_EMP_EDIT:
			{
				m_pPOPHandler->OnUMEMEmpEditResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_DEP_DEL:
			{
				m_pPOPHandler->OnUMEMDepDeleteResponse(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_DEP_EDIT:
			{
				m_pPOPHandler->OnUMEMDepEditResponse(pRequestInfo, pResponseInfo, this);
			}break;
		//case EB_SIGN_DEP_OP:
		//	{
		//		m_pPOPHandler->OnUMEMDepOpResponse(pRequestInfo, pResponseInfo, this);
		//	}break;
		case EB_SIGN_FEMP_INFO:
			{
				m_pPOPHandler->OnFUMEMEmpInfo(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_FDEP_INFO:
			{
				m_pPOPHandler->OnFUMEMDepInfo(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_FENT_INFO:
			{
				m_pPOPHandler->OnFUMEMEntInfo(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_AB_EDIT:
			{
				m_pPOPHandler->OnUMCMEdit(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_AB_DEL:
			{
				m_pPOPHandler->OnUMCMDelete(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_AB_LOAD:
			{
				m_pPOPHandler->OnUMCMQuery(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_FC_HANGUP:
			{
				m_pPOPHandler->OnFUMIUHangup(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_U_QUERY:
			m_pPOPHandler->OnUMQuery(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_REG:
			{
				m_pPOPHandler->OnUMReg(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_U_SINFO:
			{
				m_pPOPHandler->OnUMSInfo(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_U_GINFO:
				m_pPOPHandler->OnUMGInfo(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_MGR:
				m_pPOPHandler->OnUMReset(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_SHEAD:
			{
				m_pPOPHandler->OnUMSHead(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_C_HANGUP:
			{
				m_pPOPHandler->OnUMIUHangup(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_FC_ACK:
			{
				m_pPOPHandler->OnFUMIUAck(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_C_ACK:
			{
				m_pPOPHandler->OnUMIUAck(pRequestInfo, pResponseInfo, this);
			}break;
		//case POP_CS_UMIU_GROUP:
		//	{
		//		m_pPOPHandler->OnUMIUGroup(pRequestInfo, pResponseInfo, this);
		//	}break;
		case EB_SIGN_C_ENTER:
			{
				m_pPOPHandler->OnUMIUEnter(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_C_CALL:
			{
				m_pPOPHandler->OnUMIUUser(pRequestInfo, pResponseInfo, this);
			}break;
		//case EB_SIGN_FUM_GROUP:
		//	{
		//		m_pPOPHandler->OnFUMIUGroup(pRequestInfo, pResponseInfo, this);
		//	}break;
		case EB_SIGN_FC_CALL:
			{
				m_pPOPHandler->OnFUMIUUser(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_FU_MSG:
			{
				m_pPOPHandler->OnFUMIUMsg(pRequestInfo, pResponseInfo, this);
			}break;
		case EB_SIGN_U_OFFLINE:
			{
				m_pPOPHandler->OnUMSMOfflineResponse(pRequestInfo, pResponseInfo, this);
				m_nUserId = 0;
				//m_sAccount = "";
				m_sOnlineKey = "";
			}break;
		case EB_SIGN_U_LOAD2:
			m_pPOPHandler->OnUMSMLoad2Response(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_LOAD:
			m_pPOPHandler->OnUMSMLoadResponse(pRequestInfo, pResponseInfo, this);
			break;
		case EB_SIGN_U_ONLINE:
			{
				//if (nResultValue == EB_STATE_OK && pRequestInfo.get() != NULL)
				//{
				//	m_sAccount = pRequestInfo->m_pRequestList.getParameterValue("um-account", "");
				//	m_sOnlineKey = pRequestInfo->m_pRequestList.getParameterValue("um-online-key", "");
				//}
				m_pPOPHandler->OnUMSMOnlineResponse(pRequestInfo, pResponseInfo, this);
			}break;
		default:
			break;
		}
	}
}

void CPOPCUserManager::OnActiveTimeout(void)
{
	// -200：stoped
	m_nActiveTimeoutCount++;
	if (m_nActiveTimeoutCount >= 2)
	{
		if (m_pPOPHandler!=NULL)
			m_pPOPHandler->OnInvalidateSession(-200,this);
		m_nActiveTimeoutCount = 0;
	}
}
void CPOPCUserManager::OnCidTimeout(unsigned long callid, unsigned long sign, bool canResendAgain)
{
	m_nActiveTimeoutCount++;
	if (m_nActiveTimeoutCount >= 4)
	{
		if (m_pPOPHandler!=NULL)
			m_pPOPHandler->OnInvalidateSession(-200,this);
		m_nActiveTimeoutCount = 0;
	}
}

void CPOPCUserManager::OnCgcResponse(const cgcParserSotp & response)
{
	const long nSotpCallId = response.getCallid();
	//const long nSotpSign = response.getSign();
	const long nResultValue = response.getResultValue();
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
	// '-103': invalidate session handle
	// -117 ： 服务器退出，更新
	if (nResultValue < 0)
	//if (nResultValue == -102 || nResultValue == -103)
	{
		if (m_pPOPHandler!=NULL)
			m_pPOPHandler->OnInvalidateSession(nResultValue,this);
		return;
	}
	m_nActiveTimeoutCount = 0;
	if (m_pPOPHandler==NULL) return;
	m_pPOPHandler->OnResultValue(nResultValue,this);
	switch (response.getSign())
	{
	case EB_SIGN_UG_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ugid", response.getRecvParameterValue("ugid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_contact_info_ver", response.getRecvParameterValue("new_contact_info_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUGEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_UG_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_contact_info_ver", response.getRecvParameterValue("new_contact_info_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUGDeleteResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_UG_LOAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ug-count", response.getRecvParameterValue("ug-count", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ug-offset", response.getRecvParameterValue("ug-offset", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ugid", response.getRecvParameterValue("ugid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("groupname", response.getRecvParameterValue("groupname", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUGInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_CS_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnCSDelResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_CS_ADD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cs-id", response.getRecvParameterValue("cs-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cs-ext", response.getRecvParameterValue("cs-ext", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cs-type", response.getRecvParameterValue("cs-type", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnCSAddResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_MACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMackResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_R_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("r-count", response.getRecvParameterValue("r-count", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("r-offset", response.getRecvParameterValue("r-offset", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("r_id", response.getRecvParameterValue("r_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("pr_id", response.getRecvParameterValue("pr_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("name", response.getRecvParameterValue("name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("type", response.getRecvParameterValue("type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("share", response.getRecvParameterValue("share", (int)EB_RESOURCE_SHARE_UNKNOWN)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-service-id", response.getRecvParameterValue("cm-service-id", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-http-server", response.getRecvParameterValue("cm-http-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-id", response.getRecvParameterValue("from-id", (mycp::bigint)0)));

			const EB_RESOURCE_FROM_TYPE nFromType = (EB_RESOURCE_FROM_TYPE)response.getRecvParameterValue("from-type", (int)EB_RESOURCE_FROM_TYPE_UNKNOWN);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-type", (int)nFromType));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("share-id", response.getRecvParameterValue("share-id", (mycp::bigint)0)));
			if (nFromType==EB_RESOURCE_FROM_TYPE_MCON_ID_ATT)
			{
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email-content-flag", response.getRecvParameterValue("email-content-flag", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email-from-user-id", response.getRecvParameterValue("email-from-user-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email-from-account", response.getRecvParameterValue("email-from-account", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email-from-name", response.getRecvParameterValue("email-from-name", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email-subject", response.getRecvParameterValue("email-subject", "")));
			}
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("online-view-url", response.getRecvParameterValue("online-view-url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("view-ext-type", response.getRecvParameterValue("view-ext-type", (int)EB_FILE_EXT_TYPE_OTHER)));
			// ??? 兼容旧服务端
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-code", response.getRecvParameterValue("ent-code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_uid", response.getRecvParameterValue("create_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_username", response.getRecvParameterValue("create_username", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("time", response.getRecvParameterValue("time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("delete_time", response.getRecvParameterValue("delete_time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dlc", response.getRecvParameterValue("dlc", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("index", response.getRecvParameterValue("index", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("size", response.getRecvParameterValue("size", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnResInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_R_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnResDeleteResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_R_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("r_id", response.getRecvParameterValue("r_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-http-server", response.getRecvParameterValue("cm-http-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnResEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_AUTH:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auth-id", response.getRecvParameterValue("auth-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-id", response.getRecvParameterValue("ent-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-id", response.getRecvParameterValue("sub-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("username", response.getRecvParameterValue("username", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-level", response.getRecvParameterValue("sub-level", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-type", response.getRecvParameterValue("func-type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("extension", response.getRecvParameterValue("extension", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("post-data", response.getRecvParameterValue("post-data", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("online-key", response.getRecvParameterValue("online-key", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("acm-key", response.getRecvParameterValue("acm-key", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("logon-type", response.getRecvParameterValue("logon-type", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncAuthResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_REQ:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub_level", response.getRecvParameterValue("sub_level", (int)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub_key", response.getRecvParameterValue("sub_key", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-url", response.getRecvParameterValue("func-url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-param", response.getRecvParameterValue("func-param", (int)1)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-ext", response.getRecvParameterValue("func-ext", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auth-id", response.getRecvParameterValue("auth-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("fk", response.getRecvParameterValue("fk", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("color", response.getRecvParameterValue("color", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncReqResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_LOAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			// get-summary=1
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-id", response.getRecvParameterValue("ent-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-count", response.getRecvParameterValue("func-count", (int)-1)));
			const int nGetSummary = response.getRecvParameterValue("get-summary", (int)0);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("get-summary", nGetSummary));
			if (nGetSummary==0)
			{
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-offset", response.getRecvParameterValue("func-offset", (int)0)));
				//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-id", response.getRecvParameterValue("ent-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("app-id", response.getRecvParameterValue("app-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create-time", response.getRecvParameterValue("create-time", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("location", response.getRecvParameterValue("location", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-name", response.getRecvParameterValue("func-name", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-mode", response.getRecvParameterValue("func-mode", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-param", response.getRecvParameterValue("func-param", (int)1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-url", response.getRecvParameterValue("func-url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-type", response.getRecvParameterValue("func-type", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("width", response.getRecvParameterValue("width", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("height", response.getRecvParameterValue("height", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dis-contextmenu", response.getRecvParameterValue("dis-contextmenu", (int)1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dis-scrollbar", response.getRecvParameterValue("dis-scrollbar", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("close-prompt", response.getRecvParameterValue("close-prompt", (int)1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("win-resizable", response.getRecvParameterValue("win-resizable", (int)1)));
				// ent-id>0
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-id", response.getRecvParameterValue("sub-id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-level", response.getRecvParameterValue("sub-level", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("start-time", response.getRecvParameterValue("start-time", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("end-time", response.getRecvParameterValue("end-time", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("display-index", response.getRecvParameterValue("display-index", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-desc", response.getRecvParameterValue("sub-desc", "")));
			}
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncLoadResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_SETICON:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-id", response.getRecvParameterValue("res-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncSetIconResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncDelResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FUNC_SUB:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-id", response.getRecvParameterValue("sub-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncSubResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FNAV_SET:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("nav-id", response.getRecvParameterValue("nav-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncNavSetResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FNAV_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("nav-id", response.getRecvParameterValue("nav-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncNavDelResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FNAV_LOAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("func-id", response.getRecvParameterValue("func-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("nav-id", response.getRecvParameterValue("nav-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("parent-navid", response.getRecvParameterValue("parent-navid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("name", response.getRecvParameterValue("name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("url", response.getRecvParameterValue("url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("index", response.getRecvParameterValue("index", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("type", response.getRecvParameterValue("type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create-time", response.getRecvParameterValue("create-time", "")));
			// 
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("nav-count", response.getRecvParameterValue("nav-count", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFuncNavLoadResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_VER_CHECK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (nResultValue == EB_STATE_NEW_VERSION)
			{
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("server_ver", response.getRecvParameterValue("server_ver", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("download_url", response.getRecvParameterValue("download_url", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("update_type", response.getRecvParameterValue("update_type", (int)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("update_time", response.getRecvParameterValue("update_time", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm_server", response.getRecvParameterValue("cm_server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("http_server", response.getRecvParameterValue("http_server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm_appname", response.getRecvParameterValue("cm_appname", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res_id", response.getRecvParameterValue("res_id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ext", response.getRecvParameterValue("ext", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("md5", response.getRecvParameterValue("md5", "")));
			}
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnVersionCheckResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_V_REQUEST:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-server", response.getRecvParameterValue("v-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-param", response.getRecvParameterValue("v-param", (mycp::bigint)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("vu-id", response.getRecvParameterValue("vu-id", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnVRequestResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_V_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-server", response.getRecvParameterValue("v-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-param", response.getRecvParameterValue("v-param", (mycp::bigint)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("vu-id", response.getRecvParameterValue("vu-id", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnVAckResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FV_REQUEST:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-type", response.getRecvParameterValue("v-type", (int)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("vu-id", response.getRecvParameterValue("vu-id", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFVRequest(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FV_END:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("vu-id", response.getRecvParameterValue("vu-id", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFVEnd(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FV_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", response.getRecvParameterValue("ack-type", (int)1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-server", response.getRecvParameterValue("v-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("v-param", response.getRecvParameterValue("v-param", (mycp::bigint)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("vu-id", response.getRecvParameterValue("vu-id", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFVAck(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_RD_REQUEST:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-server", response.getRecvParameterValue("rd-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-param", response.getRecvParameterValue("rd-param", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnRDRequestResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_RD_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-server", response.getRecvParameterValue("rd-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-param", response.getRecvParameterValue("rd-param", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnRDAckResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FRD_REQUEST:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-type", response.getRecvParameterValue("rd-type", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFRDRequest(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FRD_END:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFRDEnd(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FRD_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", response.getRecvParameterValue("ack-type", (int)1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-server", response.getRecvParameterValue("rd-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("rd-param", response.getRecvParameterValue("rd-param", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFRDAck(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_ENT_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMEMEntEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_DEP_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_group_info_ver", response.getRecvParameterValue("new_group_info_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMEMDepDeleteResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_EMP_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_code", response.getRecvParameterValue("emp_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_group_ver", response.getRecvParameterValue("new_group_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMEMEmpDeleteResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_EMP_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_code", response.getRecvParameterValue("emp_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_uid", response.getRecvParameterValue("emp_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_group_ver", response.getRecvParameterValue("new_group_ver", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("line_state", response.getRecvParameterValue("line_state", (int)EB_LINE_STATE_UNKNOWN)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMEMEmpEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_DEP_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_code", response.getRecvParameterValue("dep_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_group_info_ver", response.getRecvParameterValue("new_group_info_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMEMDepEditResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	//case EB_SIGN_DEP_OP:
	//	{
	//		//CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
	//		//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_code", response.getRecvParameterValue("dep_code", "")));
	//		//m_pResponseList.add(pResponseInfo);
	//	}break;
	case EB_SIGN_ENT_LOAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnEntLoadResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FEMP_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_code", response.getRecvParameterValue("emp_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_code", response.getRecvParameterValue("dep_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_uid", response.getRecvParameterValue("emp_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_account", response.getRecvParameterValue("emp_account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("username", response.getRecvParameterValue("username", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("job_title", response.getRecvParameterValue("job_title", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("job_position", response.getRecvParameterValue("job_position", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cell_phone", response.getRecvParameterValue("cell_phone", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("fax", response.getRecvParameterValue("fax", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("work_phone", response.getRecvParameterValue("work_phone", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("manager_level", response.getRecvParameterValue("manager_level", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("gender", response.getRecvParameterValue("gender", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("birthday", response.getRecvParameterValue("birthday", (int)0)));
			// account_t
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("state", response.getRecvParameterValue("state", (int)EB_LINE_STATE_UNKNOWN)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um_server", response.getRecvParameterValue("um_server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um_app_name", response.getRecvParameterValue("um_app_name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h_cm_server", response.getRecvParameterValue("h_cm_server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h_cm_httpserver", response.getRecvParameterValue("h_cm_httpserver", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h_cm_app_name", response.getRecvParameterValue("h_cm_app_name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h_md5", response.getRecvParameterValue("h_md5", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h-r-id", response.getRecvParameterValue("h-r-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cs_id", response.getRecvParameterValue("cs_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cs_ext", response.getRecvParameterValue("cs_ext", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("display_index", response.getRecvParameterValue("display_index", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("forbid_minutes", response.getRecvParameterValue("forbid_minutes", (int)0)));
			//
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp-count", response.getRecvParameterValue("emp-count", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp-offset", response.getRecvParameterValue("emp-offset", (int)-1)));

			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("group_ver", response.getRecvParameterValue("group_ver", (mycp::bigint)-1)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMEMEmpInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FDEP_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_code", response.getRecvParameterValue("ent_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_code", response.getRecvParameterValue("dep_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_name", response.getRecvParameterValue("dep_name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("parent_code", response.getRecvParameterValue("parent_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("phone", response.getRecvParameterValue("phone", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("fax", response.getRecvParameterValue("fax", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("url", response.getRecvParameterValue("url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_time", response.getRecvParameterValue("create_time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my_emp_id", response.getRecvParameterValue("my_emp_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp_count", response.getRecvParameterValue("emp_count", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep_ver", response.getRecvParameterValue("dep_ver", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ext_data", response.getRecvParameterValue("ext_data", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("creator", response.getRecvParameterValue("creator", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_uid", response.getRecvParameterValue("create_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("manager_uid", response.getRecvParameterValue("manager_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("display_index", response.getRecvParameterValue("display_index", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("type", response.getRecvParameterValue("type", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-dep-offset", response.getRecvParameterValue("ent-dep-offset", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-dep-count", response.getRecvParameterValue("ent-dep-count", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my-dep-offset", response.getRecvParameterValue("my-dep-offset", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my-dep-count", response.getRecvParameterValue("my-dep-count", (int)-1)));

			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("group_info_ver", response.getRecvParameterValue("group_info_ver", (mycp::bigint)-1)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMEMDepInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FENT_INFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_code", response.getRecvParameterValue("ent_code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_name", response.getRecvParameterValue("ent_name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("phone", response.getRecvParameterValue("phone", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("fax", response.getRecvParameterValue("fax", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("url", response.getRecvParameterValue("url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_time", response.getRecvParameterValue("create_time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("creator", response.getRecvParameterValue("creator", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("create_uid", response.getRecvParameterValue("create_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_ext", response.getRecvParameterValue("ent_ext", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent-count", response.getRecvParameterValue("ent-count", (int)-1)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMEMEntInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_AB_DEL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_contact_info_ver", response.getRecvParameterValue("new_contact_info_ver", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMCMDelete(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_AB_EDIT:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("con_id", response.getRecvParameterValue("con_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("add_contact", response.getRecvParameterValue("add_contact", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("new_contact_info_ver", response.getRecvParameterValue("new_contact_info_ver", (mycp::bigint)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("contact_uid", response.getRecvParameterValue("contact_uid", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMCMEdit(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_AB_LOAD:
		{
			if (nResultValue == EB_STATE_OK)
			{
				CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("con_id", response.getRecvParameterValue("con_id", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("con_uid", response.getRecvParameterValue("con_uid", (mycp::bigint)0)));
				const int nLoadFlag = response.getRecvParameterValue("load_flag", (int)0);
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("load_flag", nLoadFlag));
				const bool bLoadOnlyHeadInfo = (nLoadFlag&0x4)==0x4?true:false;
				if (!bLoadOnlyHeadInfo)
				{
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ugid", response.getRecvParameterValue("ugid", (mycp::bigint)0)));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("contact", response.getRecvParameterValue("contact", "")));
					//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("groupname", response.getRecvParameterValue("groupname", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("name", response.getRecvParameterValue("name", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("phone", response.getRecvParameterValue("phone", "")));
					//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("phone", response.getRecvParameterValue("phone", (mycp::bigint)0)));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("company", response.getRecvParameterValue("company", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("job_title", response.getRecvParameterValue("job_title", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("tel", response.getRecvParameterValue("tel", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("fax", response.getRecvParameterValue("fax", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("url", response.getRecvParameterValue("url", "")));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("type", response.getRecvParameterValue("type", (int)0)));
					pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("state", response.getRecvParameterValue("state", (int)EB_LINE_STATE_UNKNOWN)));
					//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("contact_uid", response.getRecvParameterValue("contact_uid", (mycp::bigint)0)));
				}
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ab-offset", response.getRecvParameterValue("ab-offset", (int)-1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ab-count", response.getRecvParameterValue("ab-count", (int)-1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my_contact_info_ver", response.getRecvParameterValue("my_contact_info_ver", (mycp::bigint)-1)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-ver", response.getRecvParameterValue("head-ver", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-rid", response.getRecvParameterValue("head-rid", (mycp::bigint)0)));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-cm-server", response.getRecvParameterValue("head-cm-server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-http-server", response.getRecvParameterValue("head-http-server", "")));
				pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-md5", response.getRecvParameterValue("head-md5", "")));
				if (m_pResponseThread.get()==NULL)
					m_pPOPHandler->OnUMCMQuery(pRequestInfo, pResponseInfo, this);
				else
					m_pResponseList.add(pResponseInfo);
			}
		}break;
	case EB_SIGN_FC_HANGUP:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("hangup", response.getRecvParameterValue("hangup", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMIUHangup(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_MGR:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMReset(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_SHEAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h-r-id", response.getRecvParameterValue("h-r-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("h-r-md5", response.getRecvParameterValue("h-r-md5", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMSHead(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_QUERY:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-uid", response.getRecvParameterValue("f-uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-account", response.getRecvParameterValue("f-account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMQuery(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_REG:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("reg_code", response.getRecvParameterValue("reg_code", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMReg(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_SINFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMSInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_GINFO:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("uid", response.getRecvParameterValue("uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("account", response.getRecvParameterValue("account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("username", response.getRecvParameterValue("username", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("description", response.getRecvParameterValue("description", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("setting", response.getRecvParameterValue("setting", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("default_emp", response.getRecvParameterValue("default_emp", (mycp::bigint)0)));
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
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("gender", response.getRecvParameterValue("gender", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("tel", response.getRecvParameterValue("tel", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("mobile", response.getRecvParameterValue("mobile", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("email", response.getRecvParameterValue("email", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("birthday", response.getRecvParameterValue("birthday", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("zipcode", response.getRecvParameterValue("zipcode", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-rid", response.getRecvParameterValue("head-rid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-cm-server", response.getRecvParameterValue("head-cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-http-server", response.getRecvParameterValue("head-http-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("head-md5", response.getRecvParameterValue("head-md5", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("phone", response.getRecvParameterValue("phone", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMGInfo(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_C_HANGUP:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMIUHangup(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FC_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_account", response.getRecvParameterValue("from_account", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", response.getRecvParameterValue("ack-type", (int)EB_CAT_UNKNOWN)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-key", response.getRecvParameterValue("um-key", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-type", response.getRecvParameterValue("f-type", (int)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-emp", response.getRecvParameterValue("f-emp", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-name", response.getRecvParameterValue("f-name", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-phone", response.getRecvParameterValue("f-phone", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-tel", response.getRecvParameterValue("f-tel", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-email", response.getRecvParameterValue("f-email", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-title", response.getRecvParameterValue("f-title", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-dep", response.getRecvParameterValue("f-dep", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-ent", response.getRecvParameterValue("f-ent", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMIUAck(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_C_ACK:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-appname", response.getRecvParameterValue("um-appname", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-server", response.getRecvParameterValue("um-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("um-key", response.getRecvParameterValue("um-key", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			////pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-app-name", response.getRecvParameterValue("chat-app-name", "")));
			////pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-server", response.getRecvParameterValue("chat-server", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-key", response.getRecvParameterValue("chat-key", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", "")));
			////pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emp-code", response.getRecvParameterValue("emp-code", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMIUAck(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	//case POP_CS_UMIU_GROUP:
	//	{
	//		CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", "")));
	//		// 群聊天，查询结果
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-app-name", response.getRecvParameterValue("chat-app-name", "")));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-server", response.getRecvParameterValue("chat-server", "")));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-key", response.getRecvParameterValue("chat-key", "")));
	//		pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", "")));
	//		m_pResponseList.add(pResponseInfo);
	//	}break;
	case EB_SIGN_C_ENTER:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-appname", response.getRecvParameterValue("cm-appname", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-server", response.getRecvParameterValue("cm-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-http-server", response.getRecvParameterValue("cm-http-server", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-key", response.getRecvParameterValue("cm-key", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("cm-id", response.getRecvParameterValue("cm-id", (mycp::bigint)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMIUEnter(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_C_CALL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-type", response.getRecvParameterValue("f-type", (int)EB_ACCOUNT_TYPE_USER)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auto-accept", response.getRecvParameterValue("auto-accept", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMIUUser(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	//case EB_SIGN_FUM_GROUP:
	case EB_SIGN_FC_CALL:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_account", response.getRecvParameterValue("from_account", "")));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("old-cid", response.getRecvParameterValue("old-cid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
			// 群
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
			// 电子名片
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auto-accept", response.getRecvParameterValue("auto-accept", (int)0)));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMIUUser(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_FU_MSG:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg_id", response.getRecvParameterValue("msg_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from", response.getRecvParameterValue("from", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-subtype", response.getRecvParameterValue("msg-subtype", (int)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-name", response.getRecvParameterValue("msg-name", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-content", response.getRecvParameterValue("msg-content", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-time", response.getRecvParameterValue("msg-time", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emo-count", response.getRecvParameterValue("emo-count", (int)-1)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emo_ver", response.getRecvParameterValue("emo_ver", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnFUMIUMsg(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_LOAD2:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auth-msg-url", response.getRecvParameterValue("auth-msg-url", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMSMLoad2Response(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_LOAD:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("auth-msg-url", response.getRecvParameterValue("auth-msg-url", "")));
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMSMLoadResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	case EB_SIGN_U_OFFLINE:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			if (m_pResponseThread.get()==NULL)
			{
				m_pPOPHandler->OnUMSMOfflineResponse(pRequestInfo, pResponseInfo, this);
				m_nUserId = 0;
				//m_sAccount = "";
				m_sOnlineKey = "";
			}else
			{
				m_pResponseList.add(pResponseInfo);
			}
		}break;
	case EB_SIGN_U_ONLINE:
		{
			CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("access_token", response.getRecvParameterValue("access_token", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("us_id", response.getRecvParameterValue("us_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_id", response.getRecvParameterValue("ent_id", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emo_ver", response.getRecvParameterValue("emo_ver", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my_group_info_ver", response.getRecvParameterValue("my_group_info_ver", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("my_contact_info_ver", response.getRecvParameterValue("my_contact_info_ver", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_dep_info_ver", response.getRecvParameterValue("ent_dep_info_ver", (mycp::bigint)0)));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_manager_url", response.getRecvParameterValue("ent_manager_url", "")));
			pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ext_data", response.getRecvParameterValue("ext_data", (int)0)));
			//pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ent_ext", response.getRecvParameterValue("ent_ext", (int)0)));
			//m_nUserId = response.getRecvParameterValue("uid", (mycp::bigint)0);
			//m_sAccount = response.getRecvParameterValue("account", "");
			//m_pResponseList.add(pResponseInfo);
			if (nResultValue == EB_STATE_OK && pRequestInfo.get() != NULL)
			{
				m_nUserId = pRequestInfo->m_pRequestList.getParameterValue("um-uid", (mycp::bigint)0);
				//m_sAccount = pRequestInfo->m_pRequestList.getParameterValue("um-account", "");
				m_sOnlineKey = pRequestInfo->m_pRequestList.getParameterValue("um-online-key", "");
			}
			if (m_pResponseThread.get()==NULL)
				m_pPOPHandler->OnUMSMOnlineResponse(pRequestInfo, pResponseInfo, this);
			else
				m_pResponseList.add(pResponseInfo);
		}break;
	default:
		break;
	}
}

int CPOPCUserManager::SendUQuery(mycp::bigint nFromIpAddress, const tstring& pQueryAccount, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("q-account-info", pQueryAccount));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_QUERY, EB_CALL_NAME_UM_QUERY, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendDictLoad(EB_DICT_TYPE nDictType,mycp::bigint nValue,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	//if (nFromIpAddress>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)nDictType));
	if (nValue!=0)
		sotp()->doAddParameter(CGC_PARAMETER("value", nValue));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DICT_LOAD, EB_CALL_NAME_DICT_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUGEdit(mycp::bigint nUGId,const tstring& sGroupName,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (sGroupName.empty()) return -1;
	sotp()->doBeginCallLock();
	//if (nFromIpAddress>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("ugid", nUGId));
	sotp()->doAddParameter(CGC_PARAMETER("groupname", sGroupName));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId, EB_SIGN_UG_EDIT, EB_CALL_NAME_UG_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUGDelete(mycp::bigint nUGId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	//if (nFromIpAddress>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("ugid", nUGId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_UG_DEL, EB_CALL_NAME_UG_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUGLoad(int nOffset,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	//if (nFromIpAddress>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	if (nOffset>=0)
		sotp()->doAddParameter(CGC_PARAMETER("offset", nOffset));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_UG_LOAD, EB_CALL_NAME_UG_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendCSAdd(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("ent-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("emp-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-ext"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-type"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-order"),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CS_ADD, EB_CALL_NAME_CS_ADD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendCSDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cs-ext"),false);
	sotp()->doAddParameter(pInfoList.getParameter("disable"),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CS_DEL, EB_CALL_NAME_CS_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncNavSet(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("nav-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("parent-navid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("index"),false);
	sotp()->doAddParameter(pInfoList.getParameter("type"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FNAV_SET, EB_CALL_NAME_FNAV_SET, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncNavDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("nav-id"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FNAV_DEL, EB_CALL_NAME_FNAV_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncNavLoad(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("sub-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FNAV_LOAD, EB_CALL_NAME_FNAV_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendFuncSub(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("ent-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cancel"),false);
	sotp()->doAddParameter(pInfoList.getParameter("sub-level"),false);
	sotp()->doAddParameter(pInfoList.getParameter("display-index"),false);
	sotp()->doAddParameter(pInfoList.getParameter("start-time"),false);
	sotp()->doAddParameter(pInfoList.getParameter("end-time"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_SUB, EB_CALL_NAME_FUNC_SUB, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendFuncDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_DEL, EB_CALL_NAME_FUNC_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("location"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-mode"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-param"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-type"),false);
	sotp()->doAddParameter(pInfoList.getParameter("width"),false);
	sotp()->doAddParameter(pInfoList.getParameter("height"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-contextmenu"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dis-scrollbar"),false);
	sotp()->doAddParameter(pInfoList.getParameter("close-prompt"),false);
	sotp()->doAddParameter(pInfoList.getParameter("win-resizable"),false);
	//sotp()->doAddParameter(pInfoList.getParameter("relative-path"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_EDIT, EB_CALL_NAME_FUNC_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncSetIcon(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("func-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_SETICON, EB_CALL_NAME_FUNC_SETICON, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncLoad(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app-ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("ent-id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("owner-appid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("get-summary"),false);
	sotp()->doAddParameter(pInfoList.getParameter("offset"),false);
	sotp()->doAddParameter(pInfoList.getParameter("limit"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_LOAD, EB_CALL_NAME_FUNC_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncReq(mycp::bigint nUserId,const tstring& sOnlineKey,mycp::bigint nSubscribeId,const tstring& sFunctionKey,mycp::bigint nFromIpAddress,
								  mycp::bigint sCallId,mycp::bigint nFromUserId,const tstring& sFromAccount,mycp::bigint nGroupId,const tstring& sPostData,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", sOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("sub-id", nSubscribeId));
	sotp()->doAddParameter(CGC_PARAMETER("func-key", sFunctionKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("post-data", sPostData),false);
	if (sCallId!=0)
		sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	if (nFromUserId!=0)
		sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUserId));
	sotp()->doAddParameter(CGC_PARAMETER("from-account", sFromAccount),false);
	if (nGroupId!=0)
		sotp()->doAddParameter(CGC_PARAMETER("group-id", nGroupId));
	//sotp()->doAddParameter(CGC_PARAMETER("color", sColor),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_REQ, EB_CALL_NAME_FUNC_REQ, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendFuncAuth(mycp::bigint nAppId,const tstring& sAppOnlineKey,mycp::bigint nAuthId,mycp::bigint nFromIpAddress,const tstring& sFromIp,int nLogonWeb,int nLogonType,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("from-ip", sFromIp),false);
	sotp()->doAddParameter(CGC_PARAMETER("app-id", nAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app-ok", sAppOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("auth-id", nAuthId));
	if (nLogonWeb==1)
	{
		sotp()->doAddParameter(CGC_PARAMETER("logon-web", 1));
		if (nLogonType>0)
			sotp()->doAddParameter(CGC_PARAMETER("logon-type", nLogonType));
	}
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_FUNC_AUTH, EB_CALL_NAME_FUNC_AUTH, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendVersinoCheck(mycp::bigint sAppId,const tstring& sClientVersion,int nLogonType)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("client_ver", sClientVersion));
	sotp()->doAddParameter(CGC_PARAMETER("type", nLogonType));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_VER_CHECK, EB_CALL_NAME_VER_CHECK, true))
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

int CPOPCUserManager::SendRDRequest(mycp::bigint sCallId,int nRDType, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("rd-type", nRDType));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_RD_REQUEST, EB_CALL_NAME_RD_REQUEST, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendRDAck(mycp::bigint sCallId,int nAckType,mycp::bigint nToUserId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	if (nToUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("to_uid", nToUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("ack-type", nAckType));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_RD_ACK, EB_CALL_NAME_RD_ACK, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendRDEnd(mycp::bigint sCallId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_RD_END, EB_CALL_NAME_RD_END, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

void CPOPCUserManager::SetDevAppId(mycp::bigint sAppId, const tstring& sAppOnlineKey)
{
	m_sAppId = sAppId;
	m_sAppOnlineKey = sAppOnlineKey;
}

int CPOPCUserManager::SendVRequest(mycp::bigint sCallId,int nVideoType, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("v-type", nVideoType));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_V_REQUEST, EB_CALL_NAME_V_REQUEST, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendVAck(mycp::bigint sCallId,int nAckType,mycp::bigint nToUserId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	if (nToUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("to_uid", nToUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("ack-type", nAckType));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_V_ACK, EB_CALL_NAME_V_ACK, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendVEnd(mycp::bigint sCallId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_V_END, EB_CALL_NAME_V_END, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendRNew(const tstring& sName,EB_RESOURCE_TYPE nResType,mycp::bigint sParentResId,const tstring& sDescription,
							   EB_RESOURCE_SHARE_TYPE nShare,mycp::bigint nFromId, EB_RESOURCE_FROM_TYPE nFromType,const CPOPSotpRequestInfo::pointer& pRequestInfo)
//int CPOPCUserManager::SendRNew(const tstring& sName,EB_RESOURCE_TYPE nResType,mycp::bigint sParentResId,const tstring& sDescription,mycp::bigint sDepCode,
//							   mycp::bigint sEntCode,int nShare,mycp::bigint nFromId, int nFromType,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("name", sName),false);
	if (sParentResId>0)
		sotp()->doAddParameter(CGC_PARAMETER("pr_id", sParentResId));
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)nResType));
	// ??? 1.19 兼容旧服务端
	if (nFromType==EB_RESOURCE_FROM_TYPE_GROUP && nFromId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", nFromId));
	else if (nFromType==EB_RESOURCE_FROM_TYPE_ENT && nFromId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("ent_code", nFromId));
	//if (sDepCode > 0)
	//	sotp()->doAddParameter(CGC_PARAMETER("dep_code", sDepCode));
	//if (sEntCode > 0)
	//	sotp()->doAddParameter(CGC_PARAMETER("ent_code", sEntCode));
	if (nShare!=EB_RESOURCE_SHARE_UNKNOWN)
		sotp()->doAddParameter(CGC_PARAMETER("share", (int)nShare));
	if (nFromId>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_id", nFromId));
	if (nFromType>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_type", (int)nFromType));
	sotp()->doAddParameter(CGC_PARAMETER("description", sDescription),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_R_EDIT, EB_CALL_NAME_R_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendREdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(pInfoList.getParameter("r_id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("pr_id"),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_R_EDIT, EB_CALL_NAME_R_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendREdit(mycp::bigint sResourceId,const tstring& sName,const tstring& sDescription,mycp::bigint sParentResId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	if (sResourceId>0)
		sotp()->doAddParameter(CGC_PARAMETER("r_id", sResourceId));
	sotp()->doAddParameter(CGC_PARAMETER("name", sName),false);
	sotp()->doAddParameter(CGC_PARAMETER("description", sDescription),false);
	if (sParentResId>0)
		sotp()->doAddParameter(CGC_PARAMETER("pr_id", sParentResId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_R_EDIT, EB_CALL_NAME_R_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendRDelete(mycp::bigint nFromIpAddress,mycp::bigint sResourceId,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	if (sResourceId>0)
		sotp()->doAddParameter(CGC_PARAMETER("r_id", sResourceId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_R_DEL, EB_CALL_NAME_R_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendRLoad(int nVerifyValid,mycp::bigint sResourceId, int nType,mycp::bigint nFromId,EB_RESOURCE_FROM_TYPE nFromType,int nOffset,int nLimit,
								mycp::bigint nToShareUserId,int nShareType,int nShareFlag,int nGetSummary,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("verify_valid", nVerifyValid));
	if (sResourceId>0)
		sotp()->doAddParameter(CGC_PARAMETER("r_id", sResourceId));
	if (nType!=0)
		sotp()->doAddParameter(CGC_PARAMETER("type", nType));
	// ??? 1.19 兼容旧服务端
	if (nFromType==EB_RESOURCE_FROM_TYPE_GROUP && nFromId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", nFromId));
	else if (nFromType==EB_RESOURCE_FROM_TYPE_ENT && nFromId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("ent_code", nFromId));
	if (nFromId>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_id", nFromId));
	if (nFromType>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_type", (int)nFromType));
	if (nOffset>=0)
		sotp()->doAddParameter(CGC_PARAMETER("offset", nOffset));
	if (nOffset!=1)
		sotp()->doAddParameter(CGC_PARAMETER("limit", nLimit));
	if (nToShareUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("to_share_uid", nToShareUserId));
	if (nShareType>0)
		sotp()->doAddParameter(CGC_PARAMETER("share_type", nShareType));
	if (nShareFlag>=EB_RESOURCE_SHARE_UNKNOWN)
		sotp()->doAddParameter(CGC_PARAMETER("flag", nShareFlag));
	if (nGetSummary==1)
		sotp()->doAddParameter(CGC_PARAMETER("get_summary", 1));

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_R_LOAD, EB_CALL_NAME_R_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMReset(mycp::bigint nFromIpAddress,mycp::bigint nResetUserId, int nResetType,const tstring& sResetValue, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("muid", nResetUserId));
	sotp()->doAddParameter(CGC_PARAMETER("mt", nResetType));
	sotp()->doAddParameter(CGC_PARAMETER("mv", sResetValue),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_MGR, EB_CALL_NAME_UM_MGR, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMSHead(mycp::bigint nSetUserId, mycp::bigint sDepCode, mycp::bigint sResId, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	if (nSetUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("set-uid", nSetUserId));
	if (sDepCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep-code", sDepCode));
	if (sResId>0)
		sotp()->doAddParameter(CGC_PARAMETER("res-id", sResId));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_SHEAD, EB_CALL_NAME_UM_SHEAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMSInfo(mycp::bigint nFromIpAddress, mycp::bigint nSetUserId, const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	if (nSetUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("set-uid", nSetUserId));
	sotp()->doAddParameter(pInfoList.getParameter("user-name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("desc"),true);
	sotp()->doAddParameter(pInfoList.getParameter("old_pwd"),false);	// **新版本，加密好密码；
	//sotp()->doAddParameter(pInfoList.getParameter("pwd"),false);		// 未加密密码
	sotp()->doAddParameter(pInfoList.getParameter("passwd"),false);		// **加密好密码
	sotp()->doAddParameter(pInfoList.getParameter("setting"),false);
	sotp()->doAddParameter(pInfoList.getParameter("default_emp"),false);
	sotp()->doAddParameter(pInfoList.getParameter("phone"),false);
	sotp()->doAddParameter(pInfoList.getParameter("area1"),false);
	sotp()->doAddParameter(pInfoList.getParameter("area2"),false);
	sotp()->doAddParameter(pInfoList.getParameter("area3"),false);
	sotp()->doAddParameter(pInfoList.getParameter("area4"),false);
	sotp()->doAddParameter(pInfoList.getParameter("add"),false);
	sotp()->doAddParameter(pInfoList.getParameter("url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("gender"),false);
	sotp()->doAddParameter(pInfoList.getParameter("tel"),false);
	sotp()->doAddParameter(pInfoList.getParameter("mobile"),false);
	sotp()->doAddParameter(pInfoList.getParameter("email"),false);
	sotp()->doAddParameter(pInfoList.getParameter("birthday"),false);
	sotp()->doAddParameter(pInfoList.getParameter("zipcode"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_SINFO, EB_CALL_NAME_UM_SINFO, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMGInfo(mycp::bigint nFromIpAddress, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_GINFO, EB_CALL_NAME_UM_GINFO, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMReg(mycp::bigint nFromIpAddress,const tstring& sAccount, 
								int nResendRegMail,const tstring& sPwd, const tstring& sUserName, const tstring& sEntName,const tstring& sUserExt, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	return SendUMReg2(nFromIpAddress,m_sAppId,m_sAppOnlineKey,sAccount,nResendRegMail,sPwd,0,sUserName,sEntName,sUserExt,pRequestInfo);
}
int CPOPCUserManager::SendUMReg2(mycp::bigint nFromIpAddress,mycp::bigint sAppId,const tstring& sAppOK,const tstring& sAccount,
								 int nResendRegMail,const tstring& sPwd,int nEncPwd, const tstring& sUserName, const tstring& sEntName,const tstring& sUserExt, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("app_id", sAppId));
	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOK),false);
	sotp()->doAddParameter(CGC_PARAMETER("account", sAccount),false);
	if (nResendRegMail==1)
		sotp()->doAddParameter(CGC_PARAMETER("resend-reg-mail", 1));
	sotp()->doAddParameter(CGC_PARAMETER("pwd", sPwd),false);
	if (nEncPwd==1)
		sotp()->doAddParameter(CGC_PARAMETER("enc-pwd", 1));
	sotp()->doAddParameter(CGC_PARAMETER("user-name", sUserName),false);
	sotp()->doAddParameter(CGC_PARAMETER("ent-name", sEntName),false);
	sotp()->doAddParameter(CGC_PARAMETER("user-ext", sUserExt),false);
	
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_REG, EB_CALL_NAME_UM_REG, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMReg3(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));

	sotp()->doAddParameter(pInfoList.getParameter("app_id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app_ok"),false);
	sotp()->doAddParameter(pInfoList.getParameter("account"),false);
	sotp()->doAddParameter(pInfoList.getParameter("resend-reg-mail"),false);
	sotp()->doAddParameter(pInfoList.getParameter("pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("salt"),false);
	sotp()->doAddParameter(pInfoList.getParameter("enc-pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("user-name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("ent-name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("user-ext"),false);
	sotp()->doAddParameter(pInfoList.getParameter("gender"),false);
	sotp()->doAddParameter(pInfoList.getParameter("birthday"),false);
	sotp()->doAddParameter(pInfoList.getParameter("address"),false);
	sotp()->doAddParameter(pInfoList.getParameter("auto-add-group"),false);
	
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_REG, EB_CALL_NAME_UM_REG, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMIUHangup(mycp::bigint sCallId, bool bHangup, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("hangup", bHangup?1:0));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_C_HANGUP, EB_CALL_NAME_UM_HANGUP, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMIUAck(mycp::bigint sCallId, EB_CALL_ACK_TYPE nCallAckType,
								  const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	sotp()->doAddParameter(CGC_PARAMETER("ack-type", (int)nCallAckType));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_C_ACK, EB_CALL_NAME_UM_CACK, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
//int CPOPCUserManager::SendUMIUGroup(const tstring & sFromAccount, const tstring & sToAccount, mycp::bigint sDepCode,const tstring& sFromInfo,
//								   const CPOPSotpRequestInfo::pointer& pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", sFromAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("to", sToAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("dep-code", sDepCode));
//	sotp()->doAddParameter(CGC_PARAMETER("call-id", sDepCode));
//	sotp()->doAddParameter(CGC_PARAMETER("f-info", sFromInfo));
//
//	//sotp()->doAddParameter(CGC_PARAMETER("f-type", pECardInfo->m_nAccountType));
//	//if (pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_IN_ENT)
//	//{
//	//	//BOOST_ASSERT(pECardInfo->m_pDepartmentInfo.get() != NULL);
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-dep", pECardInfo->m_pDepartmentInfo->m_sGroupName));	// ?
//	//}else if (pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_OUT_ENT)
//	//{
//	//	//BOOST_ASSERT(pECardInfo->m_pEnterpriseInfo.get() != NULL);
//	//	BOOST_ASSERT(pECardInfo->m_pDepartmentInfo.get() != NULL);
//	//	BOOST_ASSERT(pECardInfo->m_pEmployeeInfo.get() != NULL);
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-name", pECardInfo->m_pEmployeeInfo->m_sUserName));
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-phone", pECardInfo->m_pEmployeeInfo->m_sCellPhone));
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-tel", pECardInfo->m_pEmployeeInfo->m_sWorkPhone));
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-email", pECardInfo->m_pEmployeeInfo->m_sEmail));
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-title", pECardInfo->m_pEmployeeInfo->m_sJobTitle));
//	//	sotp()->doAddParameter(CGC_PARAMETER("f-dep", pECardInfo->m_pDepartmentInfo->m_sGroupName));
//	//	if (pECardInfo->m_pEnterpriseInfo.get() != NULL)
//	//		sotp()->doAddParameter(CGC_PARAMETER("f-ent", pECardInfo->m_pEnterpriseInfo->m_sEnterpriseName));
//	//}else if (pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_USER)
//	//{
//	//	//ASSERT_BOOST(pECardInfo->m_pContactInfo.get() != NULL);
//	//}
//
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(POP_CS_UMIU_GROUP, EB_CALL_NAME_UMIU_GROUP, true, constNullAttchment, &nCallId))
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

int CPOPCUserManager::SendUMIUEnter(mycp::bigint sCallId, const tstring& sUmKey,
									mycp::bigint sDepCode, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
	if (sDepCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep-code", sDepCode));
	sotp()->doAddParameter(CGC_PARAMETER("um-key", sUmKey),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_C_ENTER, EB_CALL_NAME_C_ENTER, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMIUUser(mycp::bigint sToMemberCode,mycp::bigint nToUserId,
								   const CEBCallInfo::pointer & pExistCallInfo, int nCall2Dep, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("from", sFromAccount),false);
	if (sToMemberCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("member-code", sToMemberCode));
	if (nToUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("to_uid", nToUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("to", sToAccount),false);
	//sotp()->doAddParameter(CGC_PARAMETER("f-info", sFromInfo),false);
	if (nCall2Dep == 1)
		sotp()->doAddParameter(CGC_PARAMETER("c2d", nCall2Dep));

	//sotp()->doAddParameter(CGC_PARAMETER("f-type", pECardInfo->m_nAccountType));
	//if (pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_IN_ENT || pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_OUT_ENT)
	//{
	//	BOOST_ASSERT(pECardInfo->m_pEmployeeInfo.get() != NULL);
	//	sotp()->doAddParameter(CGC_PARAMETER("f-name", pECardInfo->m_pEmployeeInfo->m_sUserName));
	//	sotp()->doAddParameter(CGC_PARAMETER("f-phone", pECardInfo->m_pEmployeeInfo->m_sCellPhone));
	//	sotp()->doAddParameter(CGC_PARAMETER("f-tel", pECardInfo->m_pEmployeeInfo->m_sWorkPhone));
	//	sotp()->doAddParameter(CGC_PARAMETER("f-email", pECardInfo->m_pEmployeeInfo->m_sEmail));
	//	sotp()->doAddParameter(CGC_PARAMETER("f-title", pECardInfo->m_pEmployeeInfo->m_sJobTitle));
	//	if (pECardInfo->m_pDepartmentInfo.get() != NULL)
	//		sotp()->doAddParameter(CGC_PARAMETER("f-dep", pECardInfo->m_pDepartmentInfo->m_sGroupName));
	//	if (pECardInfo->m_pEnterpriseInfo.get() != NULL)
	//		sotp()->doAddParameter(CGC_PARAMETER("f-ent", pECardInfo->m_pEnterpriseInfo->m_sEnterpriseName));
	//}else if (pECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_USER)
	//{
	//	//ASSERT_BOOST(pECardInfo->m_pContactInfo.get() != NULL);
	//}

	// 已经存在呼叫，要动态生成群组ID
	if (pExistCallInfo.get() != NULL)
	{
		//sotp()->doAddParameter(CGC_PARAMETER("ent-code", pExistCallInfo->m_sEnterpriseCode));
		if (pExistCallInfo->m_sGroupCode>0)
			sotp()->doAddParameter(CGC_PARAMETER("dep-code", pExistCallInfo->m_sGroupCode));
		//if (pExistCallInfo->m_sOldCallId>0)
		//	sotp()->doAddParameter(CGC_PARAMETER("old-cid", pExistCallInfo->m_sOldCallId));
		if (pExistCallInfo->GetCallId()>0)
			sotp()->doAddParameter(CGC_PARAMETER("call-id", pExistCallInfo->GetCallId()));
		//sotp()->doAddParameter(CGC_PARAMETER("chat-id", pExistCallInfo->m_sChatId));
		//sotp()->doAddParameter(CGC_PARAMETER("chat-server", pExistCallInfo->m_sChatServer));
		//sotp()->doAddParameter(CGC_PARAMETER("chat-app-name", pExistCallInfo->m_sChatAppName));
	}

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_C_CALL, EB_CALL_NAME_UM_CALL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
//int CPOPCUserManager::SendUMIUMsg(mycp::bigint nFromUserId, mycp::bigint nToUserId, const CEBAppMsgInfo::pointer & pMsgInfo,//const CEbECardInfo::pointer & pECardInfo,
//								  const CPOPSotpRequestInfo::pointer& pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFromUserId));
//	sotp()->doAddParameter(CGC_PARAMETER("to_uid", nToUserId));
//	sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)pMsgInfo->GetMsgType()));
//	sotp()->doAddParameter(CGC_PARAMETER("msg-content", pMsgInfo->GetMsgContent()));
//	if (pMsgInfo->m_sGroupCode>0)
//		sotp()->doAddParameter(CGC_PARAMETER("dep-code", pMsgInfo->m_sGroupCode));
//	sotp()->doAddParameter(CGC_PARAMETER("f-info", pMsgInfo->m_sFromInfo));
//	if (pMsgInfo->m_bOffSave)
//		sotp()->doAddParameter(CGC_PARAMETER("off-save", 1));
//
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_U_MSG, EB_CALL_NAME_UM_MSG, true, constNullAttchment, &nCallId))
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
int CPOPCUserManager::SendUMMack(mycp::bigint nFromIpAddress,mycp::bigint nMsgId, EB_CALL_ACK_TYPE nAckType, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	if (nMsgId==0) return 1;	// 消息0不需要处理；
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey),false);
	sotp()->doAddParameter(CGC_PARAMETER("msg_id", nMsgId));
	if (nAckType!=EB_CAT_UNKNOWN)
		sotp()->doAddParameter(CGC_PARAMETER("ack-type", (int)nAckType));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_MACK, EB_CALL_NAME_UM_MACK, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

//int CPOPCUserManager::SendUMIUUser(const tstring & sFromAccount, const tstring & sToAccount, const tstring& sExistCallId,
//								   const tstring& sExistChatId, const tstring& sChatServer, const tstring& sChatAppName,
//								   const CEbECardInfo::pointer & pECardInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", sFromAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("to", sToAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("call-id", sExistCallId));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", sExistChatId));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-server", sChatServer));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-app-name", sChatAppName));
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_C_CALL, EB_CALL_NAME_UMIU_USER, true, constNullAttchment, &nCallId))
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

int CPOPCUserManager::SendUMSMOnline(int nSDKVersion, int nLogonType, mycp::bigint nUserId, const tstring & sOnlineKey,int nNewLineState,mycp::bigint nFromIpAddress,
									 mycp::bigint nUserSignId,const tstring& sUserData,const tstring& sEbSid,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	CPOPSotpRequestInfo::pointer pRequestInfoTemp = pRequestInfo;
	if (pRequestInfoTemp.get() == NULL)
		pRequestInfoTemp = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_LOGON);
	else
	{
		pRequestInfoTemp->SetRequestTime();
	}
	pRequestInfoTemp->m_pRequestList.SetParameter(CGC_PARAMETER("um-uid", nUserId));
	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("um-account", sAccount));
	pRequestInfoTemp->m_pRequestList.SetParameter(CGC_PARAMETER("um-online-key", sOnlineKey));
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)nLogonType));
	sotp()->doAddParameter(CGC_PARAMETER("uid", nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", sOnlineKey));
    sotp()->doAddParameter(CGC_PARAMETER("sdk-ver", nSDKVersion));
	sotp()->doAddParameter(CGC_PARAMETER("state", nNewLineState));
	//sotp()->doAddParameter(CGC_PARAMETER("desc", sNewDesc));
	if (nUserSignId>0)
		sotp()->doAddParameter(CGC_PARAMETER("us_id", nUserSignId));
	if (!sUserData.empty())
		sotp()->doAddParameter(CGC_PARAMETER("us_data", sUserData));
	if (!sEbSid.empty())
		sotp()->doAddParameter(CGC_PARAMETER("from_eb_sid", sEbSid));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	pRequestInfoTemp->SetCallId(nCallId);
	m_pRequestList.insert(nCallId, pRequestInfoTemp);
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_ONLINE, EB_CALL_NAME_UM_ONLINE, true))
	{
		return 0;
	}
	m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMSMLoad2(EB_UM_LOAD2_TYPE nLoadType,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("load-type", (int)nLoadType));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_LOAD2, EB_CALL_NAME_UM_LOAD2, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMSMLoad(int nLoadSubFunc,int nLoadMsg,int nLoadGroupVer,mycp::bigint nLoadOLSGroupId,const tstring& sLoadOLSAccounts,
								   int nLoadEntGroupOLS,int nLoadUserGroupOLS, const tstring& sLoadGroupOLSGid,int nLoadOLSContact,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("load-subfunc", nLoadSubFunc));
	sotp()->doAddParameter(CGC_PARAMETER("load-msg", nLoadMsg));
	sotp()->doAddParameter(CGC_PARAMETER("load-groupver", nLoadGroupVer));
	if (nLoadOLSGroupId>0)
		sotp()->doAddParameter(CGC_PARAMETER("load-ols-gid", nLoadOLSGroupId));
	sotp()->doAddParameter(CGC_PARAMETER("load-ols-accounts", sLoadOLSAccounts),false);
	if (nLoadEntGroupOLS==1)
		sotp()->doAddParameter(CGC_PARAMETER("load-entgroup-ols", nLoadEntGroupOLS));
	if (nLoadUserGroupOLS==1)
		sotp()->doAddParameter(CGC_PARAMETER("load-mygroup-ols", nLoadUserGroupOLS));
	sotp()->doAddParameter(CGC_PARAMETER("load-group-ols-gid", sLoadGroupOLSGid),false);
	if (nLoadOLSContact==1)
	sotp()->doAddParameter(CGC_PARAMETER("load-ols-con", 1));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_LOAD, EB_CALL_NAME_UM_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMSMOffline(int nOfflineState, const tstring& sEbSid, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("offline-state", nOfflineState));
	sotp()->doAddParameter(CGC_PARAMETER("from_eb_sid", sEbSid),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_U_OFFLINE, EB_CALL_NAME_UM_OFFLINE, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMCMEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(pInfoList.getParameter("con_id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("ugid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("con_uid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("phone"),false);
	sotp()->doAddParameter(pInfoList.getParameter("email"),false);
	sotp()->doAddParameter(pInfoList.getParameter("address"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("company"),false);
	sotp()->doAddParameter(pInfoList.getParameter("job_title"),false);
	sotp()->doAddParameter(pInfoList.getParameter("tel"),false);
	sotp()->doAddParameter(pInfoList.getParameter("fax"),false);
	sotp()->doAddParameter(pInfoList.getParameter("url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("need_add_msg"),false);

	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_AB_EDIT, EB_CALL_NAME_AB_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

//int CPOPCUserManager::SendUMCMEdit(const EB_ContactInfo* pPopContactInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
//	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
//	sotp()->doAddParameter(CGC_PARAMETER("contact", pPopContactInfo->m_sContact));
//	sotp()->doAddParameter(CGC_PARAMETER("groupname", pPopContactInfo->m_sGroup));
//	sotp()->doAddParameter(CGC_PARAMETER("name", pPopContactInfo->m_sName));
//	sotp()->doAddParameter(CGC_PARAMETER("phone", pPopContactInfo->m_sPhone));
//	sotp()->doAddParameter(CGC_PARAMETER("email", pPopContactInfo->m_sEmail));
//	sotp()->doAddParameter(CGC_PARAMETER("address", pPopContactInfo->m_sAddress));
//	sotp()->doAddParameter(CGC_PARAMETER("description", pPopContactInfo->m_sDescription));
//	sotp()->doAddParameter(CGC_PARAMETER("company", pPopContactInfo->m_sCompany));
//	sotp()->doAddParameter(CGC_PARAMETER("job_title", pPopContactInfo->m_sJobTitle));
//	sotp()->doAddParameter(CGC_PARAMETER("tel", pPopContactInfo->m_sTel));
//	sotp()->doAddParameter(CGC_PARAMETER("fax", pPopContactInfo->m_sFax));
//	sotp()->doAddParameter(CGC_PARAMETER("url", pPopContactInfo->m_sUrl));
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_AB_EDIT, EB_CALL_NAME_AB_EDIT, true, constNullAttchment, &nCallId))
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
int CPOPCUserManager::SendUMCMDelete(mycp::bigint nContactId,mycp::bigint nContactUserId,bool bDeleteDest, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	if (nContactId>0)
		sotp()->doAddParameter(CGC_PARAMETER("con_id", nContactId));
	if (nContactUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("con_uid", nContactUserId));
	if (bDeleteDest)
		sotp()->doAddParameter(CGC_PARAMETER("del_dest", 1));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_AB_DEL, EB_CALL_NAME_AB_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMCMQuery(mycp::bigint nOnlyContactId,mycp::bigint nOnlyContactUserId,int nLoadFlag,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	if (nOnlyContactId>0)
		sotp()->doAddParameter(CGC_PARAMETER("only_con_id", nOnlyContactId));
	else if (nOnlyContactUserId>0)
		sotp()->doAddParameter(CGC_PARAMETER("only_con_uid", nOnlyContactUserId));
	if (nLoadFlag>0)
		sotp()->doAddParameter(CGC_PARAMETER("load_flag", nLoadFlag));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_AB_LOAD, EB_CALL_NAME_AB_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

int CPOPCUserManager::SendUMEMEntEdit(const EB_EnterpriseInfo* pEnterpriseInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(CGC_PARAMETER("ent_code", pEnterpriseInfo->m_sEnterpriseCode));
	//sotp()->doAddParameter(CGC_PARAMETER("ent_name", pEnterpriseInfo->m_sEnterpriseName));
	sotp()->doAddParameter(CGC_PARAMETER("phone", pEnterpriseInfo->m_sPhone));
	sotp()->doAddParameter(CGC_PARAMETER("fax", pEnterpriseInfo->m_sFax));
	sotp()->doAddParameter(CGC_PARAMETER("email", pEnterpriseInfo->m_sEmail));
	sotp()->doAddParameter(CGC_PARAMETER("url", pEnterpriseInfo->m_sUrl));
	sotp()->doAddParameter(CGC_PARAMETER("address", pEnterpriseInfo->m_sAddress));
	sotp()->doAddParameter(CGC_PARAMETER("description", pEnterpriseInfo->m_sDescription));
	sotp()->doAddParameter(CGC_PARAMETER("ent_ext", pEnterpriseInfo->m_nEntExt));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_ENT_EDIT, EB_CALL_NAME_ENT_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMDepEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	sotp()->doAddParameter(pInfoList.getParameter("ent_code"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dep_code"),false);
	sotp()->doAddParameter(pInfoList.getParameter("dep_name"),false);
	sotp()->doAddParameter(pInfoList.getParameter("parent_code"),false);
	sotp()->doAddParameter(pInfoList.getParameter("manager_uid"),false);
	sotp()->doAddParameter(pInfoList.getParameter("phone"),false);
	sotp()->doAddParameter(pInfoList.getParameter("fax"),false);
	sotp()->doAddParameter(pInfoList.getParameter("email"),false);
	sotp()->doAddParameter(pInfoList.getParameter("url"),false);
	sotp()->doAddParameter(pInfoList.getParameter("address"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("type"),false);
	sotp()->doAddParameter(pInfoList.getParameter("display_index"),false);
	sotp()->doAddParameter(pInfoList.getParameter("forbid_minutes"),false);
	//sotp()->doAddParameter(pInfoList.getParameter("ext_data"),false);

	//if (pDepartmentInfo->m_sEnterpriseCode>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("ent_code", pDepartmentInfo->m_sEnterpriseCode));
	//if (pDepartmentInfo->m_sGroupCode>0)
	//	sotp()->doAddParameter(CGC_PARAMETER("dep_code", pDepartmentInfo->m_sGroupCode));
	//sotp()->doAddParameter(CGC_PARAMETER("dep_name", pDepartmentInfo->m_sGroupName),false);
	//if (pDepartmentInfo->m_sParentCode > 0)
	//	sotp()->doAddParameter(CGC_PARAMETER("parent_code", pDepartmentInfo->m_sParentCode));
	////if (pDepartmentInfo->m_nManagerUserId > 0)
	//	sotp()->doAddParameter(CGC_PARAMETER("manager_uid", pDepartmentInfo->m_nManagerUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("phone", pDepartmentInfo->m_sPhone),false);
	//sotp()->doAddParameter(CGC_PARAMETER("fax", pDepartmentInfo->m_sFax),false);
	//sotp()->doAddParameter(CGC_PARAMETER("email", pDepartmentInfo->m_sEmail),false);
	//sotp()->doAddParameter(CGC_PARAMETER("url", pDepartmentInfo->m_sUrl),false);
	//sotp()->doAddParameter(CGC_PARAMETER("address", pDepartmentInfo->m_sAddress),false);
	//sotp()->doAddParameter(CGC_PARAMETER("description", pDepartmentInfo->m_sDescription),false);
	//sotp()->doAddParameter(CGC_PARAMETER("type", (int)pDepartmentInfo->m_nGroupType));
	//sotp()->doAddParameter(CGC_PARAMETER("display_index", (int)pDepartmentInfo->m_nDisplayIndex));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DEP_EDIT, EB_CALL_NAME_DEP_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMDepEdit(mycp::bigint nFromIpAddress,const EB_GroupInfo* pDepartmentInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	if (pDepartmentInfo->m_sEnterpriseCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("ent_code", pDepartmentInfo->m_sEnterpriseCode));
	if (pDepartmentInfo->m_sGroupCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", pDepartmentInfo->m_sGroupCode));
	sotp()->doAddParameter(CGC_PARAMETER("dep_name", pDepartmentInfo->m_sGroupName),false);
	if (pDepartmentInfo->m_sParentCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("parent_code", pDepartmentInfo->m_sParentCode));
	//if (pDepartmentInfo->m_nManagerUserId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("manager_uid", pDepartmentInfo->m_nManagerUserId));
	sotp()->doAddParameter(CGC_PARAMETER("phone", pDepartmentInfo->m_sPhone),false);
	sotp()->doAddParameter(CGC_PARAMETER("fax", pDepartmentInfo->m_sFax),false);
	sotp()->doAddParameter(CGC_PARAMETER("email", pDepartmentInfo->m_sEmail),false);
	sotp()->doAddParameter(CGC_PARAMETER("url", pDepartmentInfo->m_sUrl),false);
	sotp()->doAddParameter(CGC_PARAMETER("address", pDepartmentInfo->m_sAddress),false);
	sotp()->doAddParameter(CGC_PARAMETER("description", pDepartmentInfo->m_sDescription),false);
	sotp()->doAddParameter(CGC_PARAMETER("type", (int)pDepartmentInfo->m_nGroupType));
	sotp()->doAddParameter(CGC_PARAMETER("display_index", (int)pDepartmentInfo->m_nDisplayIndex));
	//sotp()->doAddParameter(CGC_PARAMETER("ext_data", (int)pDepartmentInfo->m_nGroupExtData));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DEP_EDIT, EB_CALL_NAME_DEP_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
//int CPOPCUserManager::SendUMEMDepOp(mycp::bigint sDepCode, int nOpType, const tstring& sOpText, const CPOPSotpRequestInfo::pointer& pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
//	sotp()->doAddParameter(CGC_PARAMETER("dep_code", sDepCode),false);
//	sotp()->doAddParameter(CGC_PARAMETER("op_type", nOpType));
//	sotp()->doAddParameter(CGC_PARAMETER("op_text", sOpText),false);
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_DEP_OP, EB_CALL_NAME_DEP_OP, true, constNullAttchment, &nCallId))
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
int CPOPCUserManager::SendUMEMDepDelete(mycp::bigint nFromIpAddress,mycp::bigint sDepCode, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	//sotp()->doAddParameter(CGC_PARAMETER("ent_code", sEntCode));
	if (sDepCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", sDepCode));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DEP_DEL, EB_CALL_NAME_DEP_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMEmpEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(pInfoList.getParameter("app_id"),false);
	sotp()->doAddParameter(pInfoList.getParameter("app_ok"),false);
	//if (nAppId>0 && !sAppOk.empty())
	//{
	//	sotp()->doAddParameter(CGC_PARAMETER("app_id", nAppId));
	//	sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk));
	//}
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	
	sotp()->doAddParameter(pInfoList.getParameter("dep_code"),false);
	sotp()->doAddParameter(pInfoList.getParameter("emp_code"),false);
	sotp()->doAddParameter(pInfoList.getParameter("emp_uid"),false);	// ** PC客户端这个参数有用
	sotp()->doAddParameter(pInfoList.getParameter("emp_account"),false);
	sotp()->doAddParameter(pInfoList.getParameter("username"),false);
	sotp()->doAddParameter(pInfoList.getParameter("job_title"),false);
	sotp()->doAddParameter(pInfoList.getParameter("job_position"),false);
	sotp()->doAddParameter(pInfoList.getParameter("cell_phone"),false);
	sotp()->doAddParameter(pInfoList.getParameter("fax"),false);
	sotp()->doAddParameter(pInfoList.getParameter("work_phone"),false);
	sotp()->doAddParameter(pInfoList.getParameter("email"),false);
	sotp()->doAddParameter(pInfoList.getParameter("address"),false);
	sotp()->doAddParameter(pInfoList.getParameter("description"),false);
	sotp()->doAddParameter(pInfoList.getParameter("manager_level"),false);
	sotp()->doAddParameter(pInfoList.getParameter("forbid_minutes"),false);
	sotp()->doAddParameter(pInfoList.getParameter("gender"),false);
	sotp()->doAddParameter(pInfoList.getParameter("birthday"),false);
	sotp()->doAddParameter(pInfoList.getParameter("need-emp-info"),false);
	sotp()->doAddParameter(pInfoList.getParameter("pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("enc-pwd"),false);
	sotp()->doAddParameter(pInfoList.getParameter("display_index"),false);

	//sotp()->doAddParameter(CGC_PARAMETER("dep_code", pEmployeeInfo->m_sGroupCode));
	//if (pEmployeeInfo->m_sMemberCode > 0)
	//	sotp()->doAddParameter(CGC_PARAMETER("emp_code", pEmployeeInfo->m_sMemberCode));
	//sotp()->doAddParameter(CGC_PARAMETER("emp_account", pEmployeeInfo->m_sMemberAccount));
	//sotp()->doAddParameter(CGC_PARAMETER("username", pEmployeeInfo->m_sUserName));
	//sotp()->doAddParameter(CGC_PARAMETER("job_title", pEmployeeInfo->m_sJobTitle));
    //sotp()->doAddParameter(CGC_PARAMETER("job_position", pEmployeeInfo->m_nJobPosition));
	//sotp()->doAddParameter(CGC_PARAMETER("cell_phone", pEmployeeInfo->m_sCellPhone));
	//sotp()->doAddParameter(CGC_PARAMETER("fax", pEmployeeInfo->m_sFax));
	//sotp()->doAddParameter(CGC_PARAMETER("work_phone", pEmployeeInfo->m_sWorkPhone));
	//sotp()->doAddParameter(CGC_PARAMETER("email", pEmployeeInfo->m_sEmail));
	//sotp()->doAddParameter(CGC_PARAMETER("address", pEmployeeInfo->m_sAddress));
	//sotp()->doAddParameter(CGC_PARAMETER("description", pEmployeeInfo->m_sDescription));
	//if (pEmployeeInfo->m_nManagerLevel>=0)
	//	sotp()->doAddParameter(CGC_PARAMETER("manager_level", (int)pEmployeeInfo->m_nManagerLevel));
	//sotp()->doAddParameter(CGC_PARAMETER("gender", (int)pEmployeeInfo->m_nGender));
	//sotp()->doAddParameter(CGC_PARAMETER("birthday", (int)pEmployeeInfo->m_nBirthday));
	//if (nNeedEmpInfo==1)
	//	sotp()->doAddParameter(CGC_PARAMETER("need-emp-info",nNeedEmpInfo));
	//sotp()->doAddParameter(CGC_PARAMETER("pwd", sPwd),false);
	//if (nEncPwd==1)
	//	sotp()->doAddParameter(CGC_PARAMETER("enc-pwd", 1));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_EMP_EDIT, EB_CALL_NAME_EMP_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMEmpEdit(const EB_MemberInfo* pEmployeeInfo,int nForbidMinutes,int nNeedEmpInfo,const tstring& sPwd,int nEncPwd,
									  mycp::bigint nFromIpAddress,mycp::bigint nAppId,const tstring& sAppOk,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	if (nAppId>0 && !sAppOk.empty())
	{
		sotp()->doAddParameter(CGC_PARAMETER("app_id", nAppId));
		sotp()->doAddParameter(CGC_PARAMETER("app_ok", sAppOk));
	}
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	//sotp()->doAddParameter(CGC_PARAMETER("ent_code", pEmployeeInfo->m_sEnterpriseCode));
	sotp()->doAddParameter(CGC_PARAMETER("dep_code", pEmployeeInfo->m_sGroupCode));
	if (pEmployeeInfo->m_sMemberCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("emp_code", pEmployeeInfo->m_sMemberCode));
	if (pEmployeeInfo->m_nMemberUserId > 0)
		sotp()->doAddParameter(CGC_PARAMETER("emp_uid", pEmployeeInfo->m_nMemberUserId));
	sotp()->doAddParameter(CGC_PARAMETER("emp_account", pEmployeeInfo->m_sMemberAccount));
	sotp()->doAddParameter(CGC_PARAMETER("username", pEmployeeInfo->m_sUserName));
	sotp()->doAddParameter(CGC_PARAMETER("job_title", pEmployeeInfo->m_sJobTitle));
    sotp()->doAddParameter(CGC_PARAMETER("job_position", pEmployeeInfo->m_nJobPosition));
	sotp()->doAddParameter(CGC_PARAMETER("cell_phone", pEmployeeInfo->m_sCellPhone));
	sotp()->doAddParameter(CGC_PARAMETER("fax", pEmployeeInfo->m_sFax));
	sotp()->doAddParameter(CGC_PARAMETER("work_phone", pEmployeeInfo->m_sWorkPhone));
	sotp()->doAddParameter(CGC_PARAMETER("email", pEmployeeInfo->m_sEmail));
	sotp()->doAddParameter(CGC_PARAMETER("address", pEmployeeInfo->m_sAddress));
	sotp()->doAddParameter(CGC_PARAMETER("description", pEmployeeInfo->m_sDescription));
	if (pEmployeeInfo->m_nManagerLevel>=0)
	{
		sotp()->doAddParameter(CGC_PARAMETER("manager_level", (int)pEmployeeInfo->m_nManagerLevel));
		//if ((pEmployeeInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)!=0 && nForbidMinutes>=0)	// -1=不处理，0=永久禁言
		////if ((pEmployeeInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)!=0 && nForbidMinutes>0)
		//{
		//	sotp()->doAddParameter(CGC_PARAMETER("forbid_minutes", nForbidMinutes));
		//}
	}
	// -1: 解决禁言 0:永久禁言 >0 禁言分钟；
	if (nForbidMinutes>=-1)
	{
		sotp()->doAddParameter(CGC_PARAMETER("forbid_minutes", nForbidMinutes));
	}
	sotp()->doAddParameter(CGC_PARAMETER("gender", (int)pEmployeeInfo->m_nGender));
	sotp()->doAddParameter(CGC_PARAMETER("birthday", (int)pEmployeeInfo->m_nBirthday));
	if (nNeedEmpInfo==1)
		sotp()->doAddParameter(CGC_PARAMETER("need-emp-info",nNeedEmpInfo));
	sotp()->doAddParameter(CGC_PARAMETER("pwd", sPwd),false);
	if (nEncPwd==1)
		sotp()->doAddParameter(CGC_PARAMETER("enc-pwd", 1));
	sotp()->doAddParameter(CGC_PARAMETER("display_index", (int)pEmployeeInfo->m_nDisplayIndex));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_EMP_EDIT, EB_CALL_NAME_EMP_EDIT, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMEmpDelete(mycp::bigint sEmpCode, mycp::bigint nGroupId, const tstring& sMemberAccount,mycp::bigint nFromIpAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	if (nFromIpAddress>0)
		sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	if (sEmpCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("emp_code", sEmpCode));
	if (nGroupId>0)
		sotp()->doAddParameter(CGC_PARAMETER("dep_code", nGroupId));
	sotp()->doAddParameter(CGC_PARAMETER("emp_account", sMemberAccount),false);
	//sotp()->doAddParameter(CGC_PARAMETER("delete_account", (int)(bDeleteAccount?1:0)));
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_EMP_DEL, EB_CALL_NAME_EMP_DEL, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}
int CPOPCUserManager::SendUMEMLoad(mycp::bigint sDepCode,int nLoadEntDep,int nLoadMyGroup,int nLoadEmp,bool bLoadImage,mycp::bigint nEmpCode,mycp::bigint nEmpUid,const tstring& sSearchKey,const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (!IsClientStarted()) return -1;
	sotp()->doBeginCallLock();
	sotp()->doAddParameter(CGC_PARAMETER("uid", m_nUserId));
	//sotp()->doAddParameter(CGC_PARAMETER("account", m_sAccount));
	sotp()->doAddParameter(CGC_PARAMETER("online-key", m_sOnlineKey));
	if (nEmpUid>0)
		sotp()->doAddParameter(CGC_PARAMETER("emp-uid", nEmpCode));
	if (nEmpCode>0)
		sotp()->doAddParameter(CGC_PARAMETER("emp-code", nEmpCode));
	if (sDepCode > 0)
		sotp()->doAddParameter(CGC_PARAMETER("dep-code", sDepCode));
	sotp()->doAddParameter(CGC_PARAMETER("load-ent-dep", nLoadEntDep));
	sotp()->doAddParameter(CGC_PARAMETER("load-my-group", nLoadMyGroup));
	//sotp()->doAddParameter(CGC_PARAMETER("send-dep-info", nSendDepInfo));
	sotp()->doAddParameter(CGC_PARAMETER("load-emp", nLoadEmp));
	sotp()->doAddParameter(CGC_PARAMETER("load-image", (int)(bLoadImage?1:0)));
	sotp()->doAddParameter(CGC_PARAMETER("search-key", sSearchKey),false);
	const unsigned long nCallId = sotp()->doGetNextCallId();
	if (pRequestInfo.get() != NULL)
	{
		pRequestInfo->SetRequestTime();
		pRequestInfo->SetCallId(nCallId);
		m_pRequestList.insert(nCallId, pRequestInfo);
	}
	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_ENT_LOAD, EB_CALL_NAME_ENT_LOAD, true))
	{
		return 0;
	}
	if (pRequestInfo.get() != NULL)
		m_pRequestList.remove(nCallId);
	return -2;
}

} // namespace entboost
