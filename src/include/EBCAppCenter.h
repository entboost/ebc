#pragma once
#include <string>
#include "pop_sotpclient.h"
#include "eb_objects.h"
//#include "pophandler.h"

namespace entboost {

class CEBCAppCenter;
class CEBAPHandler
{
public:
	virtual void OnProcessTimer(const CEBCAppCenter* pAPOwner) {}	// 1秒定时事件
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CEBCAppCenter* pAPOwner) = 0;
	virtual void OnInvalidateSession(int nResultCode,const CEBCAppCenter* pAPOwner) = 0;
	virtual void OnResultValue(int nResultValue,const CEBCAppCenter* pAPOwner) = 0;

	virtual void OnAPAOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) = 0;
	virtual void OnAPAOffResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnAPMsgResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) = 0;
	virtual void OnAPFAMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) = 0;
	virtual void OnAPAMackResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnAPAUMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnAPAUMail(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnExeJobResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	
	virtual void OnSyncEditPwdResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnSyncUserInfoResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}
	virtual void OnSysEditResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner) {}

};

class CEBCAppCenter
	: public CPOPSotpClient
{
public:
	typedef boost::shared_ptr<CEBCAppCenter> pointer;
	static CEBCAppCenter::pointer create(CEBAPHandler * pPOPHandler,int bProcessTimer=0)
	{
		return CEBCAppCenter::pointer(new CEBCAppCenter(pPOPHandler,bProcessTimer));
	}
	CEBCAppCenter(CEBAPHandler * pPOPHandler,int bProcessTimer);
	virtual ~CEBCAppCenter(void);

public:
	int Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort, int nUsesSsl);
	bool IsStart(void) const;
	void Stop(void);
	void SetBigData(mycp::bigint v) {m_nBigData = v;}
	mycp::bigint GetBigData(void) const {return m_nBigData;}

	int SendAOn(mycp::bigint nFromAddress,mycp::bigint sAppId,const tstring& sAppOnlineKey, const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendAOff(mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendAMsg(mycp::bigint nFromAddress,const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendExecJob(bool bCheckLimitTime, mycp::bigint nJobId,const tstring& sJobParams,const tstring& sFromString,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendAUMsg(const tstring& sCallKey,const EB_APMsgInfo& pAPMsgInfo,const tstring& sToEntIds,const tstring& sToDepIds,int nInclude_sub_group,const tstring& sToAccount,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendAUMail(const tstring& sCallKey,const EB_APMsgInfo& pAPMsgInfo,mycp::bigint nToEntId,mycp::bigint nToDepId,const tstring& sToAccount,mycp::bigint nFromAddress,int nOffSave, int nAuthAccount,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendAMAck(mycp::bigint nMsgId,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);

	int SendSyncEditPwd(mycp::bigint nUserId,const tstring& sOldPwd,const tstring& sNewPwd,const tstring& sUserSalt,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendSyncUserInfo(mycp::bigint nGroupId, mycp::bigint nUserId,const CPopParameterList& pInfoList,mycp::bigint nFromAddress,const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int SendSysEdit(mycp::bigint nFromAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	void SetAppId(mycp::bigint v) {m_sAppId=v;}
	mycp::bigint GetAppId(void) const {return m_sAppId;}
	void SetAppOnlineKey(const tstring & v) {m_sAppOnlineKey=v;}
	const tstring & GetAppOnlineKey(void) const {return m_sAppOnlineKey;}

	//static void response_thread_svr(void* lparam);
	void process_thread_svr(void);
	void DoProcess(void);
	//void DoResponse(void);
private:
	virtual void OnCgcResponse(const cgcParserSotp & response);
	short m_nActiveTimeoutCount;
	virtual void OnActiveTimeout(void);

	CLockMap<unsigned long, CPOPSotpRequestInfo::pointer> m_pRequestList;			// callid->
	CLockList<CPOPSotpRequestInfo::pointer> m_pReqResponseList;
	CLockList<CPOPSotpResponseInfo::pointer> m_pResponseList;

	int			m_bProcessTimer;
	bool			m_bKilled;
	boost::shared_ptr<boost::thread> m_pResponseThread;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	CEBAPHandler *	m_pPOPHandler;
	// info
	mycp::bigint		m_sAppId;
	tstring			m_sAppOnlineKey;
	mycp::bigint		m_nBigData;
};

} // namespace entboost
