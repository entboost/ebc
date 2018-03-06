// EBClientCore.h : Declaration of the CEBClientCore

#pragma once
#include "resource.h"       // main symbols

#include "ebcore_i.h"
#include "_IEBClientCoreEvents_CP.h"
#include "EBAppClient.h"
#include "IEB_APMsgInfo.h"
#include "IEB_AreaInfo.h"
#include "IEB_AccountInfo.h"
#include "IEB_ECardInfo.h"
#include "IEB_MemberInfo.h"
#include "IEB_ResourceInfo.h"
#include "IEB_ChatMsgItem.h"
#include "IEB_UGInfo.h"
#include "IEB_ContactInfo.h"
#include "IEB_EnterpriseInfo.h"
#include "IEB_GroupInfo.h"
#include "IEB_EmotionInfo.h"
#include "IEB_CallInfo.h"
#include "IEB_VideoInfo.h"
#include "IEB_UserVideoInfo.h"
#include "IEB_VersionInfo.h"
#include "IEB_ChatInfo.h"
#include "IEB_P2PInfo.h"
#include "IEB_ChatAccountInfo.h"
#include "IEB_ChatRichInfo.h"
#include "IEB_ChatFileInfo.h"
#include "IEB_ChatFilePercent.h"
#include "IEB_SubscribeFuncInfo.h"
#include "boost/shared_ptr.hpp"
#include "stl/lockmap.h"
#include "stl/locklist.h"
#include "../include/EBParseSetting.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

//  tlbexp SisCalLib.dll /out:SisCalLib.tlb

class CEBClientCore;
class CStreamData
{
public:
	typedef boost::shared_ptr<CStreamData> pointer;
	static CStreamData::pointer create(CEBClientCore* pOwner, ULONGLONG sCallId)
	{
		return CStreamData::pointer(new CStreamData(pOwner, sCallId));
	}
	CEBClientCore *	m_pOwner;
	ULONGLONG		m_sCallId;
	ULONG			m_nUserData;
	CComPtr<IDispatch> m_pVideoCallback;
	CComPtr<IDispatch> m_pAudioCallback;

	//CLockList<CStreamData::pointer> m_pAudioData;	// callid->
	//CLockMap<eb::bigint,DWORD> m_pVideoData;	// uid->
	CLockMap<eb::bigint,DWORD> m_pAudioData;	// uid->

	CStreamData(void)
		: m_pOwner(NULL)
		, m_sCallId(0)
		//, m_nUserData(0)
	{
	}
	CStreamData(CEBClientCore* pOwner, ULONGLONG sCallId)
		: m_pOwner(pOwner)
		, m_sCallId(sCallId)
		//, m_nUserData(nUserData)
	{
	}
	virtual ~CStreamData(void)
	{
		//m_pAudioData.clear();
		m_pVideoCallback.Release();
		m_pAudioCallback.Release();
	}
};


// CEBClientCore

class ATL_NO_VTABLE CEBClientCore :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEBClientCore, &CLSID_EBClientCore>,
	public IConnectionPointContainerImpl<CEBClientCore>,
	public CProxy_IEBClientCoreEvents<CEBClientCore>,
	public IDispatchImpl<IEBClientCore, &IID_IEBClientCore, &LIBID_ebcoreLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CEBCallbackInterface,
	public CEBSearchCallback,
	public IDispatchImpl<IEBParseSetting, &__uuidof(IEBParseSetting), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IProvideClassInfo2Impl<&CLSID_EBClientCore, &__uuidof(_IEBClientCoreEvents)>,
	public IDispatchImpl<_IEBClientCoreEvents, &__uuidof(_IEBClientCoreEvents), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>	// 支持脚本接收事件
	//public IDispatchImpl<IEBCoreDev, &__uuidof(IEBCoreDev), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreAp, &__uuidof(IEBCoreAp), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreAuth, &__uuidof(IEBCoreAuth), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreMgr, &__uuidof(IEBCoreMgr), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreLevel, &__uuidof(IEBCoreLevel), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreRes, &__uuidof(IEBCoreRes), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>
	//public IDispatchImpl<IEBCoreCall, &__uuidof(IEBCoreCall), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreContact, &__uuidof(IEBCoreContact), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreOrg, &__uuidof(IEBCoreOrg), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	//public IDispatchImpl<IEBCoreVideo, &__uuidof(IEBCoreVideo), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>
	//public IDispatchImpl<IEBLib, &__uuidof(IEBLib), &LIBID_ebcoreLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CLockMap<ULONGLONG,CStreamData::pointer> m_pVideoData;		// callid->

	CEBClientCore()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EBCLIENTCORE)


	BEGIN_COM_MAP(CEBClientCore)
		COM_INTERFACE_ENTRY(IEBClientCore)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IProvideClassInfo)	// 支持脚本接收事件
		COM_INTERFACE_ENTRY(IProvideClassInfo2)	// 支持脚本接收事件
		COM_INTERFACE_ENTRY(IEBParseSetting)
		//COM_INTERFACE_ENTRY(IEBCoreDev)
		//COM_INTERFACE_ENTRY(IEBCoreAp)
		//COM_INTERFACE_ENTRY(IEBCoreAuth)
		//COM_INTERFACE_ENTRY(IEBCoreMgr)
		//COM_INTERFACE_ENTRY(IEBCoreLevel)
		//COM_INTERFACE_ENTRY(IEBCoreRes)
		//COM_INTERFACE_ENTRY(IEBCoreCall)
		//COM_INTERFACE_ENTRY(IEBCoreContact)
		//COM_INTERFACE_ENTRY(IEBCoreOrg)
		//COM_INTERFACE_ENTRY(IEBCoreVideo)
		//COM_INTERFACE_ENTRY(IEBLib)
		COM_INTERFACE_ENTRY(_IEBClientCoreEvents)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CEBClientCore)
		CONNECTION_POINT_ENTRY(__uuidof(_IEBClientCoreEvents))
	END_CONNECTION_POINT_MAP()
	// 实现脚本安全
	BEGIN_CATEGORY_MAP(CEBClientCore)
		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
	END_CATEGORY_MAP()

	CComPtr<IDispatch> m_pSearchCallback;
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo, unsigned long dwParam){
		if (m_pSearchCallback==NULL) return;
		CIEB_GroupInfo* pIEBGroupInfo = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IEB_GroupInfo> pGroupInfoInterface = NULL;
		pIEBGroupInfo->QueryInterface(IID_IEB_GroupInfo,(void**)&pGroupInfoInterface);

		CIEB_EnterpriseInfo* pIEBEnterpriseInfo = new CIEB_EnterpriseInfo(*pEnterpriseInfo);
		CComPtr<IEB_EnterpriseInfo> pEnterpriseInfoInterface = NULL;
		pIEBEnterpriseInfo->QueryInterface(IID_IEB_EnterpriseInfo,(void**)&pEnterpriseInfoInterface);

		CComVariant avarParams[3];
		avarParams[2] = pGroupInfoInterface;
		avarParams[2].vt = VT_DISPATCH;
		avarParams[1] = pEnterpriseInfoInterface;
		avarParams[1].vt = VT_DISPATCH;
		avarParams[0] = dwParam;
		avarParams[0].vt = VT_UI4;
		DISPPARAMS params = { avarParams, NULL, 3, 0 };
		HRESULT hr = m_pSearchCallback->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam){
		if (m_pSearchCallback==NULL) return;
		CIEB_GroupInfo* pIEBGroupInfo = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IEB_GroupInfo> pGroupInfoInterface = NULL;
		pIEBGroupInfo->QueryInterface(IID_IEB_GroupInfo,(void**)&pGroupInfoInterface);

		CIEB_MemberInfo* pIEBMemberInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IEB_MemberInfo> pMemberInfoInterface = NULL;
		pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pMemberInfoInterface);

		CComVariant avarParams[3];
		avarParams[2] = pGroupInfoInterface;
		avarParams[2].vt = VT_DISPATCH;
		avarParams[1] = pMemberInfoInterface;
		avarParams[1].vt = VT_DISPATCH;
		avarParams[0] = dwParam;
		avarParams[0].vt = VT_UI4;
		DISPPARAMS params = { avarParams, NULL, 3, 0 };
		HRESULT hr = m_pSearchCallback->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam){
		if (m_pSearchCallback==NULL) return;
		CIEB_ContactInfo* pIEBContactInfo = new CIEB_ContactInfo(*pContactInfo);
		CComPtr<IEB_ContactInfo> pContactInfoInterface = NULL;
		pIEBContactInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pContactInfoInterface);

		CComVariant avarParams[2];
		avarParams[1] = pContactInfoInterface;
		avarParams[1].vt = VT_DISPATCH;
		avarParams[0] = dwParam;
		avarParams[0].vt = VT_UI4;
		DISPPARAMS params = { avarParams, NULL, 2, 0 };
		HRESULT hr = m_pSearchCallback->Invoke(3, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}

	// CM
	virtual void OnEnterRoom(const CCrInfo& pParameter, EB_STATE_CODE nState) {
		CIEB_ChatInfo* pInfo = new CIEB_ChatInfo(pParameter);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatInfo,(void**)&pInterface);
		Fire_OnEnterRoom(pInterface,(SHORT)nState);}
	virtual void OnExitRoom(const CCrInfo& pParameter) {
		CIEB_ChatInfo* pInfo = new CIEB_ChatInfo(pParameter);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatInfo,(void**)&pInterface);
		Fire_OnExitRoom(pInterface);}
	virtual void OnUserEnterRoom(const CCrAccountInfo& pAccountInfo) {
		CIEB_ChatAccountInfo* pInfo = new CIEB_ChatAccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatAccountInfo,(void**)&pInterface);
		Fire_OnUserEnterRoom(pInterface);}
	virtual void OnUserExitRoom(const CCrAccountInfo& pAccountInfo,bool bHangup) {
		CIEB_ChatAccountInfo* pInfo = new CIEB_ChatAccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatAccountInfo,(void**)&pInterface);
		Fire_OnUserExitRoom(pInterface,bHangup==true?VARIANT_TRUE:VARIANT_FALSE);}

	virtual void OnUserNotify(const CCrNotifyInfo& pNotifyInfo) {
		// ???
	}

	virtual void OnSendRich(const CCrRichInfo& pCrMsgInfo,EB_STATE_CODE nState) {
		CIEB_ChatRichInfo* pInfo = new CIEB_ChatRichInfo(pCrMsgInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatRichInfo,(void**)&pInterface);
		Fire_OnSendRich(pInterface,(SHORT)nState);}
	virtual int OnReceiveRich(const CCrRichInfo& pCrMsgInfo) {
		CIEB_ChatRichInfo* pInfo = new CIEB_ChatRichInfo(pCrMsgInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatRichInfo,(void**)&pInterface);
		Fire_OnReceiveRich(pInterface);
		return 0;	// ???
	}
	virtual void OnSendingFile(const CCrFileInfo& pFileInfo,EB_STATE_CODE nState) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnSendingFile(pInterface,(SHORT)nState);}
	virtual void OnSentFile(const CCrFileInfo& pFileInfo,EB_STATE_CODE nState) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnSentFile(pInterface,(SHORT)nState);}
	virtual void OnCancelFile(const CCrFileInfo& pFileInfo) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnCancelFile(pInterface);}
	virtual int OnReceivingFile(const CCrFileInfo& pFileInfo) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnReceivingFile(pInterface);
		return 0;	/// ???
	}
	virtual void OnReceivedFile(const CCrFileInfo& pFileInfo) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnReceivedFile(pInterface);}
	virtual void OnFilePercent(const CChatRoomFilePercent& pFilePercent) {
		CIEB_ChatFilePercent* pInfo = new CIEB_ChatFilePercent(pFilePercent);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFilePercent,(void**)&pInterface);
		Fire_OnFilePercent(pInterface);}
	virtual void OnSave2CloudDrive(const CCrFileInfo& pFileInfo, EB_STATE_CODE nState) {
		CIEB_ChatFileInfo* pInfo = new CIEB_ChatFileInfo(pFileInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatFileInfo,(void**)&pInterface);
		Fire_OnSave2CloudDrive(pInterface,nState);}

	virtual void OnInvalidateSession(const CCrInfo& pParameter) {
		/// ???
	}
	//virtual void OnInvalidateSession(const CCrInfo& pParameter,int nResultCode) {}
	virtual void OnEnterAnother(const CCrInfo& pParameter){
		CIEB_ChatInfo* pInfo = new CIEB_ChatInfo(pParameter);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_ChatInfo,(void**)&pInterface);
		Fire_OnEnterAnother(pInterface);}

	virtual void OnP2PRequest(const CCrP2PInfo& pP2PInfo){
		CIEB_P2PInfo* pInfo = new CIEB_P2PInfo(pP2PInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_P2PInfo,(void**)&pInterface);
		Fire_OnP2PRequest(pInterface);}
	virtual void OnP2PResponse(const CCrP2PInfo& pP2PInfo, int nFlag){
		CIEB_P2PInfo* pInfo = new CIEB_P2PInfo(pP2PInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_P2PInfo,(void**)&pInterface);
		Fire_OnP2PResponse(pInterface,(SHORT)nFlag);}
	virtual void OnP2POk(const CCrP2PInfo& pP2PInfo){
		CIEB_P2PInfo* pInfo = new CIEB_P2PInfo(pP2PInfo);
		CComPtr<IDispatch> pInterface;
		pInfo->QueryInterface(IID_IEB_P2PInfo,(void**)&pInterface);
		Fire_OnP2POk(pInterface);}

	// UM&LC
	virtual void onAppIdSuccess(eb::bigint appId,const char* appOnlineKey) {
		Fire_onAppIdSuccess(appId,_bstr_t(appOnlineKey));}
	virtual void onAppIdError(EB_STATE_CODE nState) {Fire_onAppIdError(nState);}
	virtual void onLogonAppCenter(EB_STATE_CODE nState) {Fire_onLogonAppCenter(nState);}
	virtual void onLogoutAppCenter(EB_STATE_CODE nState) {Fire_onLogoutAppCenter(nState);}

	virtual int onAPMsgInfo(const EB_APMsgInfo& pAPMsgInfo) {
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pAPMsgInfo);
		CComPtr<IDispatch> pInterface;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface);
		Fire_onAPMsgInfo(pInterface);
		return 0;}

	virtual void onRegisterResponse(EB_STATE_CODE nState,eb::bigint nUserId) {Fire_onRegisterResponse(nState,nUserId);}

	virtual void onOAuthForward(const std::string& sOAuthUrl) {
		Fire_onOAuthForward(_bstr_t(sOAuthUrl.c_str()));}
	virtual void onLogonSuccess(const EB_AccountInfo& pAccountInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface);
		Fire_onLogonSuccess(pInterface);
	}
	virtual void onLogonTimeout(const EB_AccountInfo& pAccountInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface);
		Fire_onLogonTimeout(pInterface);
	}
	virtual void onLogonError(const EB_AccountInfo& pAccountInfo,EB_STATE_CODE nState) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface);
		Fire_onLogonError(pInterface,nState);
	}
	virtual void onOnlineAnother(void) {Fire_onOnlineAnother();}	// 在其他地方登录
	virtual void onLogout(void) {Fire_onLogout();}

	virtual void onEditInfoResponse(EB_STATE_CODE nState) {Fire_onEditInfoResponse(nState);}										// 个人资料修改返回
	virtual void onUserStateChange(const EB_MemberInfo* pMemberInfo, bool bIsOwnerMember) {
		CIEB_MemberInfo* pIEBMemberInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IDispatch> pInterface;
		pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pInterface);
		Fire_onUserStateChange(pInterface,bIsOwnerMember==true?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onUserHeadChange(const EB_MemberInfo* pMemberInfo, bool bIsOwnerMember) {
		CIEB_MemberInfo* pIEBMemberInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IDispatch> pInterface;
		pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pInterface);
		Fire_onUserHeadChange(pInterface,bIsOwnerMember==true?VARIANT_TRUE:VARIANT_FALSE);
	}

	virtual void onCallIncoming(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccount) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pFromAccount);
		CComPtr<IDispatch> pFromAccountInfoInterface = NULL;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pFromAccountInfoInterface);
		Fire_onCallIncoming(pCallInfoInterface,pFromAccountInfoInterface);
	}
	virtual void onCallAlerting(const EB_CallInfo& pCallInfo) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		Fire_onCallAlerting(pCallInfoInterface);
	}
	virtual void onCallBusy(const EB_CallInfo& pCallInfo) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		Fire_onCallBusy(pCallInfoInterface);
	}
	virtual void onCallHangup(const EB_CallInfo& pCallInfo, bool bOwner) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		Fire_onCallHangup(pCallInfoInterface,bOwner==true?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onCallError(const EB_CallInfo& pCallInfo,EB_STATE_CODE nState) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		Fire_onCallError(pCallInfoInterface,nState);
	}
	virtual void onCallConnected(const EB_CallInfo& pCallInfo, int nConnectFlag) {
		CIEB_CallInfo* pIEBCallInfo = new CIEB_CallInfo(pCallInfo);
		CComPtr<IDispatch> pCallInfoInterface = NULL;
		pIEBCallInfo->QueryInterface(IID_IEB_CallInfo,(void**)&pCallInfoInterface);
		Fire_onCallConnected(pCallInfoInterface,nConnectFlag);
	}

	virtual void onVRequestResponse(const EB_VideoInfo& pVideoInfo, EB_STATE_CODE nState) {
		CIEB_VideoInfo* pIEBInfo = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface);
		Fire_onVRequestResponse(pInfoInterface,nState);
	}
	virtual void onVAckResponse(const EB_VideoInfo& pVideoInfo, EB_STATE_CODE nState) {
		CIEB_VideoInfo* pIEBInfo = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface);
		Fire_onVAckResponse(pInfoInterface,nState);
	}
	virtual void onVideoRequest(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {
		CIEB_VideoInfo* pIEBInfo1 = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface1);
		CIEB_UserVideoInfo* pIEBInfo2 = new CIEB_UserVideoInfo(sFromAccount);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pInfoInterface2);
		Fire_onVideoRequest(pInfoInterface1,pInfoInterface2);
	}
	virtual void onVideoAccept(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {
		CIEB_VideoInfo* pIEBInfo1 = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface1);
		CIEB_UserVideoInfo* pIEBInfo2 = new CIEB_UserVideoInfo(sFromAccount);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pInfoInterface2);
		Fire_onVideoAccept(pInfoInterface1,pInfoInterface2);
	}
	virtual void onVideoReject(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {
		CIEB_VideoInfo* pIEBInfo1 = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface1);
		CIEB_UserVideoInfo* pIEBInfo2 = new CIEB_UserVideoInfo(sFromAccount);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pInfoInterface2);
		Fire_onVideoReject(pInfoInterface1,pInfoInterface2);
	}
	virtual void onVideoClose(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {
		CIEB_VideoInfo* pIEBInfo1 = new CIEB_VideoInfo(pVideoInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_VideoInfo,(void**)&pInfoInterface1);
		CIEB_UserVideoInfo* pIEBInfo2 = new CIEB_UserVideoInfo(sFromAccount);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pInfoInterface2);
		Fire_onVideoClose(pInfoInterface1,pInfoInterface2);
	}

	virtual void onEnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo) {
		CIEB_EnterpriseInfo* pIEBInfo = new CIEB_EnterpriseInfo(pEnterpriseInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_EnterpriseInfo,(void**)&pInfoInterface);
		Fire_onEnterpriseInfo(pInfoInterface);
	}
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo, bool bIsMyGroup) {
		CIEB_GroupInfo* pIEBInfo = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IEB_GroupInfo> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_GroupInfo,(void**)&pInfoInterface);
		Fire_onGroupInfo(pInfoInterface,bIsMyGroup?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onGroupDelete(const EB_GroupInfo* pGroupInfo, bool bIsMyGroup) {
		CIEB_GroupInfo* pIEBInfo = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_GroupInfo,(void**)&pInfoInterface);
		Fire_onGroupDelete(pInfoInterface,bIsMyGroup?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onGroupEditError(const EB_GroupInfo* pGroupInfo, EB_STATE_CODE nState) {
		CIEB_GroupInfo* pIEBInfo = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_GroupInfo,(void**)&pInfoInterface);
		Fire_onGroupEditError(pInfoInterface,nState);
	}
	virtual void onRemoveGroup(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo) {
		CIEB_GroupInfo* pIEBInfo1 = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_GroupInfo,(void**)&pInfoInterface1);
		CIEB_MemberInfo* pIEBInfo2 = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_MemberInfo,(void**)&pInfoInterface2);
		Fire_onRemoveGroup(pInfoInterface1,pInfoInterface2);
	}
	virtual void onExitGroup(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo) {
		CIEB_GroupInfo* pIEBInfo1 = new CIEB_GroupInfo(*pGroupInfo);
		CComPtr<IDispatch> pInfoInterface1;
		pIEBInfo1->QueryInterface(IID_IEB_GroupInfo,(void**)&pInfoInterface1);
		CIEB_MemberInfo* pIEBInfo2 = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IDispatch> pInfoInterface2;
		pIEBInfo2->QueryInterface(IID_IEB_MemberInfo,(void**)&pInfoInterface2);
		Fire_onExitGroup(pInfoInterface1,pInfoInterface2);
	}
	virtual void onRequestAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pApMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onRequestAdd2Group(pInterface1,pInterface2);
	}
	virtual void onInviteAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pApMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onInviteAdd2Group(pInterface1,pInterface2);
	}
	virtual void onRejectAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pApMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onRejectAdd2Group(pInterface1,pInterface2);
	}

	virtual void onMemberInfo(const EB_MemberInfo* pMemberInfo, bool bIsMyDefaultMember) {
		CIEB_MemberInfo* pIEBInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pInfoInterface);
		Fire_onMemberInfo(pInfoInterface,bIsMyDefaultMember?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onMemberDelete(const EB_MemberInfo* pMemberInfo, bool bIsMyDefaultMember) {
		CIEB_MemberInfo* pIEBInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IEB_MemberInfo> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pInfoInterface);
		Fire_onMemberDelete(pInfoInterface,bIsMyDefaultMember?VARIANT_TRUE:VARIANT_FALSE);
	}
	virtual void onMemberEditError(const EB_MemberInfo* pMemberInfo, EB_STATE_CODE nState) {
		CIEB_MemberInfo* pIEBInfo = new CIEB_MemberInfo(*pMemberInfo);
		CComPtr<IEB_MemberInfo> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pInfoInterface);
		Fire_onMemberEditError(pInfoInterface,(SHORT)nState);
	}

	virtual void onContactInfo(const EB_ContactInfo* pContactInfo) {
		CIEB_ContactInfo* pIEBInfo = new CIEB_ContactInfo(*pContactInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pInfoInterface);
		Fire_onContactInfo(pInfoInterface);
	}
	virtual void onContactDelete(const EB_ContactInfo* pContactInfo) {
		CIEB_ContactInfo* pIEBInfo = new CIEB_ContactInfo(*pContactInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pInfoInterface);
		Fire_onContactDelete(pInfoInterface);
	}
	virtual void onUGInfo(const EB_UGInfo* pUGInfo) {
		CIEB_UGInfo* pIEBInfo = new CIEB_UGInfo(*pUGInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_UGInfo,(void**)&pInfoInterface);
		Fire_onUGInfo(pInfoInterface);
	}
	virtual void onUGDelete(const EB_UGInfo* pUGInfo) {
		CIEB_UGInfo* pIEBInfo = new CIEB_UGInfo(*pUGInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_UGInfo,(void**)&pInfoInterface);
		Fire_onUGDelete(pInfoInterface);
	}
	virtual void onRequestAddContact(const EB_AccountInfo* pAccountInfo, const EB_APMsgInfo* pAPMsgInfo) {
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pAPMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onRequestAddContact(pInterface1,pInterface2);
	}
	virtual void onRejectAddContact(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo)
	{
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pApMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onRejectAddContact(pInterface1,pInterface2);
	}
	virtual void onAcceptAddContact(const EB_ContactInfo* pContactInfo)
	{
		CIEB_ContactInfo* pIEBInfo = new CIEB_ContactInfo(*pContactInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pInfoInterface);
		Fire_onAcceptAddContact(pInfoInterface);
	}
	virtual void onContactStateChange(const EB_ContactInfo* pContactInfo)
	{
		CIEB_ContactInfo* pIEBInfo = new CIEB_ContactInfo(*pContactInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pInfoInterface);
		Fire_onContactStateChange(pInfoInterface);
	}

	virtual void onResourceInfo(const EB_ResourceInfo* pResourceInfo) {
		CIEB_ResourceInfo* pIEBInfo = new CIEB_ResourceInfo(*pResourceInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pInfoInterface);
		Fire_onResourceInfo(pInfoInterface);
	}
	virtual void onResourceDelete(const EB_ResourceInfo* pResourceInfo) {
		CIEB_ResourceInfo* pIEBInfo = new CIEB_ResourceInfo(*pResourceInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pInfoInterface);
		Fire_onResourceDelete(pInfoInterface);
	}
	virtual void onResourceMove(const EB_ResourceInfo* pResourceInfo,const std::string& sOldParentResId) {
		CIEB_ResourceInfo* pIEBInfo = new CIEB_ResourceInfo(*pResourceInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pInfoInterface);
		Fire_onResourceMove(pInfoInterface,_bstr_t(sOldParentResId.c_str()));
	}

	virtual void onBroadcastMsg(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo)
	{
		CIEB_AccountInfo* pIEBAccountInfo = new CIEB_AccountInfo(pAccountInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pInterface1);
		CIEB_APMsgInfo* pIEBAPMsgInfo = new CIEB_APMsgInfo(pApMsgInfo);
		CComPtr<IDispatch> pInterface2;
		pIEBAPMsgInfo->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface2);
		Fire_onBroadcastMsg(pInterface1,pInterface2);
	}
	virtual void onAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter)
	{
		CIEB_AreaInfo* pIEBAreaInfo = new CIEB_AreaInfo(pAreaInfo);
		CComPtr<IDispatch> pInterface1;
		pIEBAreaInfo->QueryInterface(IID_IEB_AreaInfo,(void**)&pInterface1);
		Fire_onAreaInfo(pInterface1,nParameter);
	}

	virtual void onNewVersion(const EB_VersionInfo& pVersionInfo) {
		CIEB_VersionInfo* pIEBInfo = new CIEB_VersionInfo(pVersionInfo);
		CComPtr<IDispatch> pInfoInterface;
		pIEBInfo->QueryInterface(IID_IEB_VersionInfo,(void**)&pInfoInterface);
		Fire_onNewVersion(pInfoInterface);
	}
	//virtual void onServerDisconnect(void) {Fire_onServerDisconnect();}
	virtual void onServerChange(SERVER_STATE nServerState) {Fire_onServerChange((SHORT)nServerState);}			// 服务器状态改变
	virtual void onStateCode(EB_STATE_CODE nStateCode, unsigned long nParam) {Fire_onStateCode((SHORT)nStateCode,nParam);}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		theEBAppClient.EB_UnInit();
		m_pVideoData.clear();
	}

public:
	CEBAppClient theEBAppClient;

	STDMETHOD(EB_Init)(BSTR sAddress, BSTR sInitParameter);
	STDMETHOD(get_EB_IsInited)(VARIANT_BOOL* pVal);
	//STDMETHOD(EB_SetCallBack)(IDispatch* pCallback);
	//STDMETHOD(EB_SetMsgHwnd)(ULONG hWnd);
	STDMETHOD(EB_UnInit)(void);
	STDMETHOD(get_EB_LastStateCode)(LONG* pVal);
	STDMETHOD(get_EB_Address)(BSTR* pVal);
	STDMETHOD(put_EB_Address)(BSTR newVal);
	STDMETHOD(get_EB_AppPath)(BSTR* pVal);
	STDMETHOD(get_EB_ResourcePath)(BSTR* pVal);
	STDMETHOD(get_EB_AppDataPath)(BSTR* pVal);
	STDMETHOD(get_EB_AppDataTempPath)(BSTR* pVal);
	STDMETHOD(EB_CheckVersion)(BSTR sClientVersion);
	STDMETHOD(EB_GetAreaInfo)(LONG nParent, ULONG nParameter);
	// IEBCoreDev Methods
	STDMETHOD(EB_SetDevAppId)(ULONGLONG sAppId, BSTR sAppKey, VARIANT_BOOL bReLoadAppOnlineKey);
	STDMETHOD(EB_SetDevAppOnlineKey)(ULONGLONG sAppId, BSTR sAppOnlineKey);
	STDMETHOD(EB_GetDevAppId)(ULONGLONG* pVal);
	STDMETHOD(EB_GetDevAppOnlineKey)(BSTR* pVal);
	STDMETHOD(EB_GetSystemParameter)(LONG nParameter, ULONG* pParameterVal);
	STDMETHOD(EB_FreeSystemParameter)(LONG nParameter, ULONG pParameterVal);
	// IEBCoreAp Methods
	STDMETHOD(EB_LogonAppCenter)(void);
	STDMETHOD(EB_LogoutAppCenter)(void);
	STDMETHOD(EB_SendAPMsg)(IDispatch* pAPMsgInfoInterface, VARIANT_BOOL bSaveOffMsg);
	STDMETHOD(EB_SendAPMsg2)(SEB_APMsgInfo* pAPMsgInfoStruct, VARIANT_BOOL bSaveOffMsg);
	STDMETHOD(EB_APMsgAck)(ULONGLONG nMsgId);
	// IEBCoreAuth Methods
	STDMETHOD(EB_Register)(BSTR sAccount, BSTR sPassword, BSTR sUserName, BSTR sEnterpriseName);
	STDMETHOD(EB_LogonByVisitor)(BSTR sReqCode);
	STDMETHOD(EB_LogonByAccount)(BSTR sAccount, BSTR sPassword, BSTR sReqCode, BSTR sOAuthKey, LONG nNewLineState, LONG nLogonType);
	STDMETHOD(EB_LogonOAuth)(ULONGLONG nUserId, BSTR sOAuthKey, LONG nNewLineState);
	STDMETHOD(EB_ReLogon)(void);
	STDMETHOD(get_EB_IsLogoned)(VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_IsLogonVisitor)(VARIANT_BOOL* pVal);
	STDMETHOD(EB_Logout)(void);
	// IEBCoreMgr Methods
	STDMETHOD(EB_LoadInfo)(void);
	STDMETHOD(EB_GetMyAccountInfo)(IEB_AccountInfo** pOutAccountInterface);
	STDMETHOD(EB_GetMyAccountInfo2)(SEB_AccountInfo* pOutAccountStruct);
	STDMETHOD(get_EB_MyDefaultMemberCode)(ULONGLONG* pVal);
	STDMETHOD(put_EB_MyDefaultMemberCode)(ULONGLONG newVal);
	STDMETHOD(EB_GetMyDefaultMemberInfo)(IEB_MemberInfo** pOutMemberInfoInterface);
	STDMETHOD(EB_GetMyDefaultMemberInfo2)(SEB_MemberInfo* pOutMemberInfoStruct);
	STDMETHOD(EB_GetMyDefaultMemberHeadFile)(BSTR* pVal);
	STDMETHOD(get_EB_MyAccountSetting)(LONG* pVal);
	STDMETHOD(put_EB_MyAccountSetting)(LONG newVal);
	STDMETHOD(get_EB_LogonAccount)(BSTR* pVal);
	STDMETHOD(get_EB_LogonUserId)(ULONGLONG* pVal);
	STDMETHOD(get_EB_Description)(BSTR* pVal);
	STDMETHOD(put_EB_Description)(BSTR newVal);
	STDMETHOD(get_EB_LineState)(LONG* pVal);
	STDMETHOD(put_EB_LineState)(LONG newVal);
	STDMETHOD(get_EB_UserName)(BSTR* pVal);
	STDMETHOD(put_EB_UserName)(BSTR newVal);
	STDMETHOD(get_EB_Password)(BSTR* pVal);
	//STDMETHOD(put_EB_Password)(BSTR newVal);
	STDMETHOD(EB_SetPassword)(BSTR sOldPassword,BSTR sNewPassword);
	// IEBCoreLevel Methods
	STDMETHOD(get_EB_CanEditEnterprise)(ULONGLONG sEnterpriseCode, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanEditGroupInfo)(ULONGLONG sEnterpriseCode, ULONGLONG sGroupCode, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanDeleteGroupInfo)(ULONGLONG sGroupCode, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanEditMemberInfo)(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanEditMyBaseMemberInfo)(ULONGLONG sGroupCode, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanDeleteMemberInfo)(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanDeleteMemberInfo2)(ULONGLONG sMemberCode, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanEditGroupRes)(ULONGLONG sGroupCode, ULONGLONG nResourceId, VARIANT_BOOL* pVal);
	STDMETHOD(get_EB_CanDeleteGroupRes)(ULONGLONG sGroupCode, ULONGLONG nResourceId, VARIANT_BOOL* pVal);
	// IEBCoreRes Methods
	STDMETHOD(EB_LoadRes)(ULONGLONG sGroupCode);
	STDMETHOD(EB_GetDirAllRes)(ULONGLONG sParentResId, VARIANT* pOutResourceList);
	STDMETHOD(EB_GetNotDirAllRes)(ULONGLONG sGroupCode, VARIANT* pOutResourceList);
	STDMETHOD(EB_HasSubRes)(ULONGLONG sParentResId, VARIANT_BOOL* pVal);
	STDMETHOD(EB_GetAllRes)(ULONGLONG sGroupCode, VARIANT* pOutResourceList);
	STDMETHOD(EB_GetAllFileRes)(ULONGLONG sGroupCode, VARIANT* pOutResourceList);
	STDMETHOD(EB_GetAllNoteRes)(ULONGLONG sGroupCode, VARIANT* pOutResourceList);
	STDMETHOD(EB_AddDirRes)(BSTR sDirName, ULONGLONG sParentResId, ULONGLONG sGroupCode);
	STDMETHOD(EB_AddFileRes)(BSTR sFilePath, BSTR sFileName, BSTR sDescription, ULONGLONG sParentResId, ULONGLONG sGroupCode);
	STDMETHOD(EB_AddTempGroupFileRes)(BSTR sFilePath, BSTR sFileName, ULONGLONG sGroupCode);
	STDMETHOD(EB_AddNoteRes)(BSTR sNoteName, BSTR sDescription, ULONGLONG sParentResId, ULONGLONG sGroupCode);
	STDMETHOD(EB_EditRes)(ULONGLONG sResId, BSTR sName, BSTR sDescription);
	STDMETHOD(EB_MoveRes2Dir)(ULONGLONG sResId, ULONGLONG sParentResId);
	STDMETHOD(EB_DeleteRes)(ULONGLONG sResId);
	STDMETHOD(EB_GetResourceInfo)(ULONGLONG sResId, IEB_ResourceInfo** pVal);
	STDMETHOD(EB_GetResourceCmInfo)(ULONGLONG sResId, BSTR* pVal);
	STDMETHOD(EB_DownloadFileRes)(ULONGLONG sResId, BSTR sSaveTo);
	STDMETHOD(EB_CancelFileRes)(ULONGLONG sResId, ULONGLONG nMsgId);
	// IEBCoreCall Methods
	STDMETHOD(EB_CallMember)(ULONGLONG sMemberCode, ULONGLONG sExistCallId);
	STDMETHOD(EB_CallAccount)(BSTR sToAccount, ULONGLONG sExistCallId);
	STDMETHOD(EB_CallUserId)(ULONGLONG nUserId, ULONGLONG sExistCallId);
	STDMETHOD(EB_Call2Group)(ULONGLONG sCallId, BSTR sToAccount);
	STDMETHOD(EB_CallGroup)(ULONGLONG sGroupCode);
	STDMETHOD(EB_CallAnswer)(ULONGLONG sCallId, VARIANT_BOOL bAccept);
	STDMETHOD(EB_CallHangup)(ULONGLONG sCallId);
	STDMETHOD(EB_CallExit)(ULONGLONG sCallId);
	//STDMETHOD(EB_CheckReCall)(ULONGLONG sCallId,VARIANT_BOOL bOffReCall);
	STDMETHOD(EB_RichBufferAddText)(ULONGLONG sCallId, BSTR sText);
	STDMETHOD(EB_RichBufferAddObject)(ULONGLONG sCallId, BYTE* pData, ULONG nSize);
	STDMETHOD(EB_RichBufferAddObject2)(ULONGLONG sCallId, VARIANT* pData, ULONG nSize);
	STDMETHOD(EB_RichBufferAddResource)(ULONGLONG sCallId, BSTR sResource);
	STDMETHOD(EB_RichBufferClear)(ULONGLONG sCallId);
	STDMETHOD(EB_SendRichBuffer)(ULONGLONG sCallId, ULONGLONG sToUserId, VARIANT_BOOL bPrivate);
	STDMETHOD(EB_SendText)(ULONGLONG sCallId, BSTR sTextMsg, ULONGLONG sToUserId, VARIANT_BOOL bPrivate);
	STDMETHOD(EB_SendRich)(ULONGLONG sCallId, VARIANT* pRichMsg, ULONGLONG sToUserId, VARIANT_BOOL bPrivate);
	STDMETHOD(EB_SendFile)(ULONGLONG sCallId, BSTR sFilePath, ULONGLONG sToUserId, VARIANT_BOOL bPrivate, VARIANT_BOOL bOffFile);
	STDMETHOD(EB_AcceptFile)(ULONGLONG sCallId, ULONGLONG nMsgId, BSTR sSaveTo);
	STDMETHOD(EB_Save2CloudDrive)(ULONGLONG sCallId, ULONGLONG nMsgId);
	//STDMETHOD(EB_AcceptFileRes)(ULONGLONG sCallId, ULONGLONG sResId, BSTR sSaveTo);
	STDMETHOD(EB_CancelFile)(ULONGLONG sCallId, ULONGLONG nMsgId);
	STDMETHOD(EB_GetCallAccountInfoList)(ULONGLONG sCallId, VARIANT* pVal);
	STDMETHOD(EB_GetCallUserIdList)(ULONGLONG sCallId, VARIANT* pVal);
	STDMETHOD(EB_GetCallAccountList)(ULONGLONG sCallId, VARIANT* pVal);
	STDMETHOD(EB_GetCallAccountInfo)(ULONGLONG sCallId, ULONGLONG nUserId, IEB_AccountInfo** pVal);
	STDMETHOD(EB_GetCallAccountInfo2)(ULONGLONG sCallId, ULONGLONG nUserId, SEB_AccountInfo* pVal);
	// IEBCoreContact Methods
	STDMETHOD(EB_LoadContact)(void);
	STDMETHOD(EB_EditUGInfo)(ULONGLONG nUGId, BSTR bstrGroupName);
	STDMETHOD(EB_DeleteUGInfo)(ULONGLONG nUGId);
	STDMETHOD(EB_GetUGInfoList)(VARIANT* pVal);
	STDMETHOD(EB_ReqAddContact)(ULONGLONG nContactUserId, BSTR bstrDescription);
	STDMETHOD(EB_EditContact)(IDispatch* pVal);
	STDMETHOD(EB_EditContact2)(SEB_ContactInfo* newVal);
	STDMETHOD(EB_DeleteContact)(ULONGLONG nContactId,VARIANT_BOOL bDeleteDest);
	STDMETHOD(EB_GetContactList)(VARIANT* pVal);
	STDMETHOD(EB_GetContactInfo1)(ULONGLONG nContactId, IEB_ContactInfo** pVal);
	STDMETHOD(EB_GetContactInfo2)(ULONGLONG nContactUserId, IEB_ContactInfo** pVal);
	//STDMETHOD(EB_GetContactInfo2)(BSTR sContactAccount, SEB_ContactInfo* pVal);
	STDMETHOD(EB_IsMyContactAccount1)(ULONGLONG nContactId, VARIANT_BOOL* pVal);
	STDMETHOD(EB_IsMyContactAccount2)(ULONGLONG nContactUserId, VARIANT_BOOL* pVal);
	// IEBCoreOrg Methods
	STDMETHOD(EB_LoadOrg)(void);
	STDMETHOD(EB_LoadGroup)(ULONGLONG sGroupCode, VARIANT_BOOL bLoadMember);
	STDMETHOD(EB_GetEnterpriseInfo)(ULONGLONG sEnterpriseCode, IEB_EnterpriseInfo** pVal);
	STDMETHOD(EB_GetEnterpriseInfo2)(ULONGLONG sEnterpriseCode, SEB_EnterpriseInfo* pVal);
	STDMETHOD(EB_IsEnterpriseUser)(ULONGLONG sEnterpriseCode, VARIANT_BOOL* pVal);
	STDMETHOD(EB_GetEnterpriseName)(ULONGLONG sEnterpriseCode, BSTR* pVal);
	STDMETHOD(EB_GetEnterpriseMemberSize)(ULONGLONG sEnterpriseCode, LONG* pOutMemberSize, LONG* pOutOnlineSize);
	STDMETHOD(EB_EditEnterprise)(IDispatch* newVal);
	STDMETHOD(EB_EditEnterprise2)(SEB_EnterpriseInfo* newVal);
	STDMETHOD(EB_EditGroup)(IDispatch* newVal);
	STDMETHOD(EB_EditGroup2)(SEB_GroupInfo* newVal);
	STDMETHOD(EB_DeleteGroup)(ULONGLONG sGroupCode);
	STDMETHOD(EB_ExitGroup)(ULONGLONG sGroupCode);
	STDMETHOD(EB_SetMyGroupHeadFile)(ULONGLONG sGroupCode, BSTR sImagePath);
	STDMETHOD(EB_SetMyGroupHeadRes)(ULONGLONG sGroupCode, ULONGLONG sResId);
	STDMETHOD(EB_GetGroupInfo)(ULONGLONG sGroupCode, IEB_GroupInfo** pVal);
	STDMETHOD(EB_GetGroupInfo2)(ULONGLONG sGroupCode, SEB_GroupInfo* pVal);
	STDMETHOD(EB_GetGroupName)(ULONGLONG sGroupCode, BSTR* pVal);
	STDMETHOD(EB_GetGroupCreator)(ULONGLONG sGroupCode, ULONGLONG* pVal);
	STDMETHOD(EB_GetGroupType)(ULONGLONG sGroupCode, SHORT* pVal);
	STDMETHOD(EB_GetGroupMemberInfoList)(ULONGLONG sGroupCode, VARIANT* pVal);
	STDMETHOD(EB_GetGroupMemberUserIdList)(ULONGLONG sGroupCode, VARIANT* pVal);
	STDMETHOD(EB_GetGroupMemberCodeList)(ULONGLONG sGroupCode, VARIANT* pVal);
	STDMETHOD(EB_GetGroupMemberAccountList)(ULONGLONG sGroupCode, VARIANT* pVal);
	STDMETHOD(EB_GetGroupMemberSize)(ULONGLONG sGroupCode, SHORT nGetType, ULONG* pVal);
	STDMETHOD(EB_GetGroupOnlineSize)(ULONGLONG sGroupCode, SHORT nGetType, ULONG* pVal);
	STDMETHOD(EB_IsMyGroup)(ULONGLONG sGroupCode, VARIANT_BOOL* pVal);
	STDMETHOD(EB_EditMember)(IDispatch* newVal);
	STDMETHOD(EB_EditMember2)(SEB_MemberInfo* newVal);
	STDMETHOD(EB_DeleteMember)(ULONGLONG sMemberCode, VARIANT_BOOL bDeleteAccount);
	STDMETHOD(EB_GetMemberInfoByUserId)(ULONGLONG sGroupCode, ULONGLONG nUserId, IEB_MemberInfo** pVal);
	STDMETHOD(EB_GetMemberInfoByUserId2)(ULONGLONG nUserId, IEB_MemberInfo** pVal);
	STDMETHOD(EB_GetMemberInfoByAccount)(ULONGLONG sGroupCode, BSTR sAccount, IEB_MemberInfo** pVal);
	STDMETHOD(EB_GetMemberInfoByAccount2)(BSTR sAccount, IEB_MemberInfo** pVal);
	//STDMETHOD(EB_GetMemberInfoByAccount2)(ULONGLONG sGroupCode, BSTR sAccount, SEB_MemberInfo* pVal);
	STDMETHOD(EB_GetMemberInfoByMemberCode)(ULONGLONG sMemberCode, IEB_MemberInfo** pVal);
	STDMETHOD(EB_GetMemberInfoByMemberCode2)(ULONGLONG sMemberCode, SEB_MemberInfo* pVal);
	STDMETHOD(EB_GetMemberNameByUserId)(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, BSTR* pVal);
	STDMETHOD(EB_GetMemberNameByMemberCode)(ULONGLONG sMemberCode, BSTR* pVal);
	STDMETHOD(EB_GetMemberLineState)(ULONGLONG sMemberCode, SHORT* pVal);
	STDMETHOD(EB_GetMyMemberInfo)(ULONGLONG sGroupCode, IEB_MemberInfo** pVal);
	STDMETHOD(EB_GetMyMemberInfo2)(ULONGLONG sGroupCode, SEB_MemberInfo* pVal);
	STDMETHOD(EB_GetMyMemberList)(VARIANT* pVal);
	STDMETHOD(EB_IsExistMemberByUserId)(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal);
	STDMETHOD(EB_IsExistMemberByAccount)(ULONGLONG sGroupCode, BSTR sMemberAccount, VARIANT_BOOL* pVal);
	STDMETHOD(EB_IsExistMemberByMemberCode)(ULONGLONG sMemberCode, VARIANT_BOOL* pVal);
	// 
	STDMETHOD(EB_GetMyEmotionList)(VARIANT* pVal);
	STDMETHOD(EB_GetDefaultHeadList)(VARIANT* pVal);
	STDMETHOD(EB_FindAllGroupInfo)(IDispatch* pConnection, ULONGLONG sEnterpriseCode, ULONG ulParam);
	STDMETHOD(EB_FindAllContactInfo)(IDispatch* pConnection, ULONG ulParam);
	STDMETHOD(EB_SearchUserInfo)(IDispatch* pConnection, BSTR sSearchKey, ULONG ulParam);
	// IEBCoreVideo Methods
	STDMETHOD(EB_GetVideoDeviceList)(VARIANT* pVal);
	//STDMETHOD(EB_GetDefaultVideoMediaType)(LONG* pVal);
	STDMETHOD(EB_SetVideoCallback)(ULONGLONG sCallId, IDispatch* pCallback, IDispatch* pAudioDataCallback);
	STDMETHOD(EB_VideoRequest)(ULONGLONG sCallId, SHORT nVideoType);
	STDMETHOD(EB_VideoAck)(ULONGLONG sCallId, VARIANT_BOOL bAccept, ULONGLONG nToUserId);
	STDMETHOD(EB_VideoEnd)(ULONGLONG sCallId);
	STDMETHOD(EB_GetUserVideoInfo)(ULONGLONG sCallId, VARIANT* pOutFromVideoUserId);
	STDMETHOD(EB_OpenLocalVideo)(ULONGLONG sCallId, SHORT nLocalVideoIndex, ULONG nVideoParentWnd, ULONG ulAudioParam);
	STDMETHOD(EB_OpenUserVideo)(ULONGLONG sCallId, ULONGLONG nUserId, ULONG nVideoParentWnd, ULONG ulAudioParam);
	STDMETHOD(EB_SetVideoOpenClose)(ULONGLONG sCallId, ULONGLONG nUserId, VARIANT_BOOL newVal);
	STDMETHOD(EB_SetAudioOpenClose)(ULONGLONG sCallId, ULONGLONG nUserId, VARIANT_BOOL newVal);
	//STDMETHOD(EB_OpenVideoByUserVideoId)(ULONGLONG sCallId, ULONG nUserVideoId, IDispatch* pAudioDataCallback, ULONG nUserData);
	STDMETHOD(EB_CloseAllVideo)(ULONGLONG sCallId);
	// IEBLib Methods
	//STDMETHOD(UTF82ACP)(BSTR sUtf8String, BSTR* pVal);
	//STDMETHOD(ACP2UTF8)(BSTR sCapString, BSTR* pVal);
	//STDMETHOD(GetAccountAddress)(BSTR sAccount, BSTR* pVal);
	STDMETHOD(EB_GetSubscribeFuncInfo)(ULONGLONG nSubId, IEB_SubscribeFuncInfo** pVal);
	STDMETHOD(EB_GetSubscribeFuncList)(ULONG nLocation, VARIANT* pVal);
	STDMETHOD(EB_GetSubscribeFuncSize)(ULONG nLocation, ULONG* pVal);
	STDMETHOD(EB_GetSubscribeFuncUrl)(ULONGLONG nSubscribeId,ULONGLONG nCallId,ULONGLONG nFromUserId,BSTR sFromAccount,ULONGLONG nGroupId, BSTR * pVal);
	STDMETHOD(EB_GetConversationsUrl)(ULONGLONG nFromUserId,ULONGLONG nGroupId, BSTR * pVal);

	STDMETHOD(EB_AckMsg)(ULONGLONG nMsgId, VARIANT_BOOL bAccept);

public:
	CEBParseSetting m_pSetting;
	STDMETHOD(Load)(BSTR sFileName);
	STDMETHOD(get_ServerAddress)(BSTR* pVal);
	STDMETHOD(get_DefaultColor)(ULONG* pVal);
	STDMETHOD(get_Version)(BSTR* pVal);
	STDMETHOD(get_Enterprise)(BSTR* pVal);

	// _IEBClientCoreEvents Methods
public:
	STDMETHOD_(void, onAppIdSuccess)( ULONGLONG sAppId,  BSTR sAppOnlineKey)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onAppIdError)( SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onLogonAppCenter)( SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onLogoutAppCenter)( SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onAPMsgInfo)( IDispatch * pAPMsgInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onRegisterResponse)( SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onOAuthForward)( BSTR sOAuthUrl)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onLogonSuccess)( IDispatch * pAccountInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onLogonTimeout)( IDispatch * pAccountInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onLogonError)( IDispatch * pAccountInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onEditInfoResponse)( SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onUserStateChange)( IDispatch * pAccountInfo,  VARIANT_BOOL bIsOwnerMember)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onUserHeadChange)( IDispatch * pAccountInfo,  VARIANT_BOOL bIsOwnerMember)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallIncoming)( IDispatch * pCallInfo,  IDispatch * pFromAccount)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallAlerting)( IDispatch * pCallInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallBusy)( IDispatch * pCallInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallHangup)( IDispatch * pCallInfo,  VARIANT_BOOL bIsOwnerMember)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallError)( IDispatch * pCallInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onCallConnected)( IDispatch * pCallInfo,  LONG nConnectFlag)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVRequestResponse)( IDispatch * pVideoInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVAckResponse)( IDispatch * pVideoInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVideoRequest)( IDispatch * pVideoInfo,  IDispatch * sFromAccount)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVideoAccept)( IDispatch * pVideoInfo,  IDispatch * sFromAccount)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVideoReject)( IDispatch * pVideoInfo,  IDispatch * sFromAccount)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onVideoClose)( IDispatch * pVideoInfo,  IDispatch * sFromAccount)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onEnterpriseInfo)( IDispatch * pEnterpriseInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onGroupInfo)( IDispatch * pGroupInfo,  VARIANT_BOOL bIsMyGroup)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onGroupDelete)( IDispatch * pGroupInfo,  VARIANT_BOOL bIsMyGroup)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onGroupEditError)( IDispatch * pGroupInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onRemoveGroup)( IDispatch * pGroupInfo,  IDispatch * pMemberInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onExitGroup)( IDispatch * pGroupInfo,  IDispatch * pMemberInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onMemberInfo)( IDispatch * pMemberInfo,  VARIANT_BOOL bIsMyDefaultMember)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onMemberDelete)( IDispatch * pMemberInfo,  VARIANT_BOOL bIsMyDefaultMember)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onMemberEditError)( IDispatch * pMemberInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onContactInfo)( IDispatch * pContactInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onContactDelete)( IDispatch * pContactInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onResourceInfo)( IDispatch * pResourceInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onResourceDelete)( IDispatch * pResourceInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onResourceMove)( IDispatch * pResourceInfo,  BSTR sOldParentResId)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onNewVersion)( IDispatch * pVersionInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onServerChange)( SHORT nServerState)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnEnterRoom)( IDispatch * pCrInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnExitRoom)( IDispatch * pCrInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnUserEnterRoom)( IDispatch * pAccountInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnUserExitRoom)( IDispatch * pAccountInfo,  VARIANT_BOOL bHangup)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnSendRich)( IDispatch * pRichInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnReceiveRich)( IDispatch * pRichInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnSendingFile)( IDispatch * pFileInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnSentFile)( IDispatch * pFileInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnCancelFile)( IDispatch * pFileInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnReceivingFile)( IDispatch * pFileInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnReceivedFile)( IDispatch * pFileInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnFilePercent)( IDispatch * pFilePercent)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnSave2CloudDrive)( IDispatch * pFileInfo,  SHORT nStateCode)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnEnterAnother)( IDispatch * pCrInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnP2PRequest)( IDispatch * pCrP2PInfo)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnP2PResponse)( IDispatch * pCrP2PInfo,  SHORT nFlag)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, OnP2POk)( IDispatch * pCrP2PInfo)
	{
		// Add your function implementation here.

	}
};

OBJECT_ENTRY_AUTO(__uuidof(EBClientCore), CEBClientCore)
