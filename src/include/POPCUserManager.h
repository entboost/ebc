#pragma once
#include <string>
#include "pop_sotpclient.h"
#include "eb_objects.h"

namespace entboost {

class CPOPCUserManager;
class CPOPUMHandler
{
public:
	virtual void OnProcessTimer(const CPOPCUserManager* pUMOwner) {}	// 1秒定时事件
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnInvalidateSession(int nResultCode,const CPOPCUserManager* pUMOwner) {}
	virtual void OnResultValue(int nResultValue,const CPOPCUserManager* pUMOwner) {}

	virtual void OnUMackResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnUGEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUGDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUGInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnCSAddResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnCSDelResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnFuncNavSetResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncNavDelResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncNavLoadResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnFuncReqResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncAuthResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncLoadResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncSetIconResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncDelResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFuncSubResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnVersionCheckResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	// res
	virtual void OnResDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnResEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnResInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	// remote desktop
	virtual void OnRDRequestResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnRDAckResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFRDRequest(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFRDAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFRDEnd(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	// video
	virtual void OnVRequestResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnVAckResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFVRequest(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFVAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFVEnd(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnUMEMEmpDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMEMEmpEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMEMDepDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMEMDepEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMEMDepOpResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMEMEntEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMEMEntInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMEMDepInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMEMEmpInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnEntLoadResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

	virtual void OnUMCMEdit(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMCMDelete(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMCMQuery(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMIUHangup(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMIUHangup(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMIUAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMIUAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	//virtual void OnUMIUGroup(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMIUEnter(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMIUUser(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	//virtual void OnFUMIUGroup(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMIUEnter(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMIUUser(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnFUMIUMsg(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSMOfflineResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSMLoadResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSMLoad2Response(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSMOnlineResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMReg(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMGInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMSHead(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMReset(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}
	virtual void OnUMQuery(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner) {}

};

class CPOPCUserManager
	: public CPOPSotpClient
{
public:
	typedef boost::shared_ptr<CPOPCUserManager> pointer;
	static CPOPCUserManager::pointer create(CPOPUMHandler * pPOPHandler,int bProcessTimer)
	{
		return CPOPCUserManager::pointer(new CPOPCUserManager(pPOPHandler,bProcessTimer));
	}
	CPOPCUserManager(CPOPUMHandler * pPOPHandler, int bProcessTimer);
	virtual ~CPOPCUserManager(void);

public:
	int Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort, bool bBuildResponseThread, bool bBuildProcessThread, int nUsesSsl);
	void Stop(void);
	void SetEncoding(const std::string& v);
	void SetParameter(const std::string& v) {m_sParameter = v;}
	const std::string& GetParameter(void) const {return m_sParameter;}
	void SetBigData(mycp::bigint data) {m_nBigData = data;}
	mycp::bigint GetBigData(void) const {return m_nBigData;}
	void SetUserId(mycp::bigint v) {m_nUserId = v;}
	mycp::bigint GetUserId(void) const {return m_nUserId;}
	void SetOnlineKey(const tstring& key) {m_sOnlineKey=key;}

	//const tstring& GetAccount(void) const {return m_sAccount;}

	int SendUQuery(mycp::bigint nFromIpAddress, const tstring& pQueryAccount, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendDictLoad(EB_DICT_TYPE nDictType,mycp::bigint nValue,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendUGEdit(mycp::bigint nUGId,const tstring& sGroupName,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUGDelete(mycp::bigint nUGId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUGLoad(int nOffset=-1,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendCSAdd(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendCSDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendFuncNavSet(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncNavDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncNavLoad(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendFuncSub(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncDel(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncSetIcon(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncLoad(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncReq(mycp::bigint nUserId,const tstring& sOnlineKey,mycp::bigint nSubscribeId,const tstring& sFunctionKey,mycp::bigint nFromIpAddress,mycp::bigint nCallId,mycp::bigint nFromUserId,const tstring& sFromAccount,mycp::bigint nGroupId,const tstring& sPostData,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendFuncAuth(mycp::bigint nAppId,const tstring& sAppOnlineKey,mycp::bigint nAuthId,mycp::bigint nFromIpAddress,const tstring& sFromIp,int nLogonWeb,int nLogonType,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendVersinoCheck(mycp::bigint sAppId,const tstring& sClientVersion,int nLogonType);

	int SendRDRequest(mycp::bigint sCallId,int nRDType, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendRDAck(mycp::bigint sCallId,int nAckType,mycp::bigint nToUserId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendRDEnd(mycp::bigint sCallId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	void SetDevAppId(mycp::bigint sAppId, const tstring& sAppOnlineKey);
	int SendVRequest(mycp::bigint sCallId,int nVideoType, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendVAck(mycp::bigint sCallId,int nAckType,mycp::bigint nToUserId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendVEnd(mycp::bigint sCallId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendRNew(const tstring& sName,EB_RESOURCE_TYPE nResType,mycp::bigint sParentResId,const tstring& sDescription,EB_RESOURCE_SHARE_TYPE nShare,mycp::bigint nFromId, EB_RESOURCE_FROM_TYPE nFromType,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendRNew(const tstring& sName,EB_RESOURCE_TYPE nResType,mycp::bigint sParentResId,const tstring& sDescription,mycp::bigint sDepCode,mycp::bigint sEntCode,int nShare,mycp::bigint nFromId, int nFromType,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendREdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendREdit(mycp::bigint sResourceId,const tstring& sName,const tstring& sDescription,mycp::bigint sParentResId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendRDelete(mycp::bigint nFromIpAddress,mycp::bigint sResourceId,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendRLoad(int nVerifyValid, mycp::bigint sResourceId, int nType,mycp::bigint sFromId,EB_RESOURCE_FROM_TYPE nFromType,int nOffset=-1,int nLimit=1,mycp::bigint nToShareUserId=0,int nShareType=0, int nShareFlag=-1, int nGetSummary=0, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

    int SendUMSPush(mycp::bigint nSslId, const tstring &sPushToken, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMReset(mycp::bigint nFromIpAddress,mycp::bigint nResetUserId, int nResetType,const tstring& sResetValue, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMSHead(mycp::bigint nSetUserId, mycp::bigint sDepCode, mycp::bigint sResId, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMSInfo(mycp::bigint nFromIpAddress, mycp::bigint nSetUserId, const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMGInfo(mycp::bigint nFromIpAddress, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMReg(mycp::bigint nFromIpAddress,const tstring& sAccount, int nResendRegMail,const tstring& sPwd, const tstring& sUserName, const tstring& sEntName,const tstring& sUserExt="", const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMReg2(mycp::bigint nFromIpAddress,mycp::bigint sAppId,const tstring& sAppOK,const tstring& sAccount,int nResendRegMail, const tstring& sPwd,int nEncPwd,const tstring& sUserName, const tstring& sEntName, const tstring& sUserExt="",const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMReg3(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMIUHangup(mycp::bigint sCallId, bool bHangup, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMIUAck(mycp::bigint sCallId, EB_CALL_ACK_TYPE nCallAckType,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendUMIUGroup(const tstring & sFromAccount, const tstring & sToAccount, mycp::bigint sDepCode, const tstring& sFromInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMIUEnter(mycp::bigint sCallId, const tstring& sUmKey, mycp::bigint sDepCode, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMIUUser(mycp::bigint sToMemberCode,mycp::bigint nToUserId, const CEBCallInfo::pointer & pExistCallInfo, int nCall2Dep, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendUMIUUser(const tstring & sFromAccount, const tstring & sToAccount, const CEbECardInfo::pointer & pECardInfo, const CEBCallInfo::pointer & pExistCallInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendUMIUMsg(mycp::bigint nFromUserId, mycp::bigint nToUserId, const CEBAppMsgInfo::pointer & pMsgInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMMack(mycp::bigint nFromIpAddress, mycp::bigint nMsgId, EB_CALL_ACK_TYPE nAckType=EB_CAT_UNKNOWN, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
    int SendUMSMOnline(int nSDKVersion, int nLogonType, mycp::bigint nUserId, const tstring & sOnlineKey, int nNewLineState,mycp::bigint nFromIpAddress=0,mycp::bigint nUserSignId=0,const tstring& sUserData="",const tstring& sEbSid="",const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMSMLoad2(EB_UM_LOAD2_TYPE nLoadType=EB_UM_LOAD2_TYPE_UNKNOWN,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMSMLoad(int nLoadSubFunc=1,int nLoadMsg=1,int nLoadGroupVer=0,mycp::bigint nLoadOLSGroupId=0,const tstring& sLoadOLSAccounts="",int nLoadEntGroupOLS=0,int nLoadUserGroupOLS=0,const tstring& sLoadGroupOLSGid="", int nLoadOLSContact=0,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
    int SendUMSMOffline(bool bAcceptPush=true,int nOfflineState = OFFLINE_USES_CLOSE,const tstring& sEbSid="",const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendUMCMEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendUMCMEdit(const EB_ContactInfo* pPopContactInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMCMDelete(mycp::bigint nContactId,mycp::bigint nContactUserId,bool bDeleteDest, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMCMQuery(mycp::bigint nOnlyContactId,mycp::bigint nOnlyContactUserId,int nLoadFlag, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	int SendUMEMEntEdit(const EB_EnterpriseInfo* pEnterpriseInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMDepEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMDepEdit(mycp::bigint nFromIpAddress,const EB_GroupInfo* pDepartmentInfo, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	//int SendUMEMDepOp(mycp::bigint sDepCode, int nOpType, const tstring& sOpText="", const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMDepDelete(mycp::bigint nFromIpAddress,mycp::bigint sDepCode, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMEmpEdit(mycp::bigint nFromIpAddress,const CPopParameterList& pInfoList, const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMEmpEdit(const EB_MemberInfo* pEmployeeInfo,int nForbidMinutes, int nNeedEmpInfo, const tstring& sPwd="",int nEncPwd=0,mycp::bigint nFromIpAddress=0,mycp::bigint nAppId=0,const tstring& sAppOk="", const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMEmpDelete(mycp::bigint sEmpCode, mycp::bigint nGroupId, const tstring& nMemberAccount,mycp::bigint nFromIpAddres=0,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);
	int SendUMEMLoad(mycp::bigint sDepCode,int nLoadEntDep,int nLoadMyGroup,int nLoadEmp,bool bLoadImage,mycp::bigint nEmpCode,mycp::bigint nEmpUid,const tstring& sSearchKey,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	void response_thread_svr(void);
	void process_thread_svr(void);
	void DoProcess(void);
	void DoProcess2(unsigned int nIndex);
	void DoResponse(void);
private:
	virtual void OnCgcResponse(const cgcParserSotp & response);
	short m_nActiveTimeoutCount;
	virtual void OnActiveTimeout(void);
	virtual void OnCidTimeout(unsigned long callid, unsigned long sign, bool canResendAgain);

	CLockMap<unsigned long, CPOPSotpRequestInfo::pointer> m_pRequestList;			// callid->
	CLockList<CPOPSotpRequestInfo::pointer> m_pReqResponseList;
	CLockList<CPOPSotpResponseInfo::pointer> m_pResponseList;

	int			m_bProcessTimer;
	bool			m_bKilled;
	boost::shared_ptr<boost::thread> m_pResponseThread;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	CPOPUMHandler *	m_pPOPHandler;
	// info
	mycp::bigint m_nUserId;
	//tstring m_sAccount;
	tstring m_sOnlineKey;
	// dev app id
	mycp::bigint m_sAppId;
	tstring m_sAppOnlineKey;
	std::string		m_sParameter;
	mycp::bigint		m_nBigData;
};

} // namespace entboost
