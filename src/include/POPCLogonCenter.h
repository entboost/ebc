#pragma once
#include <string>
#include "pop_sotpclient.h"
#include "eb_objects.h"

namespace entboost {

class CPOPCLogonCenter;
class CPOPLCHandler
{
public:
	virtual void OnProcessTimer(const CPOPCLogonCenter* pLCOwner) {}	// 1秒定时事件
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnInvalidateSession(int nResultCode,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnSessionOpen(int nResultCode,const tstring& sSessionId, const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnResultValue(int nResultValue,const CPOPCLogonCenter* pLCOwner) {}

	virtual void OnLCFindPwdResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnLCResetPwdResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnLCRegAuthResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnLCULQueryResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnLCULLogonResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}

	virtual void OnLCULQInfoResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}
	virtual void OnLCULPInfoResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner) {}

};

class CPOPCLogonCenter
	: public CPOPSotpClient
{
public:
	typedef boost::shared_ptr<CPOPCLogonCenter> pointer;
	static CPOPCLogonCenter::pointer create(CPOPLCHandler * pPOPHandler,bool bProcessTimer=false)
	{
		return CPOPCLogonCenter::pointer(new CPOPCLogonCenter(pPOPHandler,bProcessTimer));
	}
	CPOPCLogonCenter(CPOPLCHandler * pPOPHandler,bool bProcessTimer);
	virtual ~CPOPCLogonCenter(void);

public:
	int Start(const CCgcAddress & address, unsigned int bindPort, int nUsesSsl, short nMaxWaitSecons=3, bool bBuildProcessThread = true);
	bool IsStart(void) const;
	void Stop(void);
	void SetEncoding(const std::string& v);

	int SendLCQueryInfo(const tstring& sSystemKey,int nQueryLicenseInfo,int nQueryServiceId,CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCPutInfo(mycp::bigint nPutKey,EB_PUT_INFO_TYPE nPutType,const tstring& sPutValue="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);

	//int SendLCQuery(mycp::bigint nPhone,const tstring & sAccount,int nReg=0, CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	//int SendLCReq(const tstring & sAccount="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCQuery(mycp::bigint nFromIpAddress,mycp::bigint sAppId,const tstring& sAppOk,EB_REQEUST_TYPE nReqType,int nSubType,mycp::bigint nFromUid,const tstring& sCallKey,mycp::bigint nMemberId,mycp::bigint sDepCode, const tstring & sAccount="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCLogon(const tstring& sUserData, int nSDKVersion,mycp::bigint nFromIpAddress,const CPOPLogonInfo::pointer & pLogonInfo,mycp::bigint sAppId=0,const tstring& sAppOk="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCAuthAppId(int nSDKVersion,mycp::bigint nFromIpAddress,const CPOPLogonInfo::pointer & pLogonInfo,CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCRegAuth(const tstring& sAuthKey, mycp::bigint nUserId,int nRegCode, const tstring& sResetPwd,int nAuthOnly,mycp::bigint sAppId=0,const tstring& sAppOk="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCFindPwd(mycp::bigint nFromIpAddress,const tstring& sAccount,mycp::bigint sAppId=0,const tstring& sAppOk="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);
	int SendLCResetPwd(mycp::bigint nFromIpAddress,const tstring& sFindKey,mycp::bigint nUserId,const tstring& sAccount,const tstring& sCurrentPwd,const tstring& sResetPwd,const tstring& sSalt,mycp::bigint sAppId=0,const tstring& sAppOk="",CPOPSotpRequestInfo::pointer pRequestInfo=NullPOPSotpRequestInfo);

	void SetParameter(const std::string& v) {m_sParameter = v;}
	const std::string& GetParameter(void) const {return m_sParameter;}
	void SetIntParameter(unsigned int v) {m_nParameter = v;}
	unsigned int GetIntParameter(void) const {return m_nParameter;}

	unsigned int GetLogonType(void) const {return m_nLogonType;}
	const tstring & GetLogonAccount(void) const {return m_nLogonAccount;}
	mycp::bigint GetAppId(void) const {return m_sAppId;}
	//void SetLogonPassword(const tstring& v) {m_nLogonPassword=v;}
	const tstring & GetLogonPassword(void) const {return m_nLogonPassword;}
	//void SetUserName(const tstring& v) {m_nUserName=v;}
	const tstring & GetUserName(void) const {return m_nUserName;}
	//void SetLineState(EB_USER_LINE_STATE v) {m_nLineState = v;}
	EB_USER_LINE_STATE GetLineState(void) const {return m_nLineState;}
	const tstring& GetReqCode(void) const {return m_sReqCode;}

	//static void response_thread_svr(void* lparam);
	void process_thread_svr(void);
	void DoProcess(void);
	//void DoResponse(void);
private:
	virtual void OnCgcResponse(const cgcParserSotp & response);
	virtual void OnCidTimeout(unsigned long callid, unsigned long sign, bool canResendAgain){}

	CLockMap<unsigned long, CPOPSotpRequestInfo::pointer> m_pRequestList;			// callid->
	CLockList<CPOPSotpRequestInfo::pointer> m_pReqResponseList;
	//CLockList<CPOPSotpResponseInfo::pointer> m_pResponseList;

	bool			m_bProcessTimer;
	bool			m_bKilled;
	//boost::thread * m_pResponseThread;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	CPOPLCHandler *	m_pPOPHandler;
	// info
	unsigned int	m_nLogonType;
	tstring			m_nLogonAccount;
	mycp::bigint		m_sAppId;
	tstring			m_sReqCode;
	tstring			m_nLogonPassword;
	tstring			m_nUserName;
	EB_USER_LINE_STATE m_nLineState;
	std::string		m_sParameter;
	unsigned int	m_nParameter;
};

} // namespace entboost
