// EBCoreEventsSink.h file here
#ifndef __EBCoreEventsSink_h__
#define __EBCoreEventsSink_h__

#ifdef USES_EBCOM_TEST
//#define USES_AFX_MANAGE_STATE

class CEBEventSinkItem
{
public:
	typedef boost::shared_ptr<CEBEventSinkItem> pointer;
	static CEBEventSinkItem::pointer create(DISPID dispid)
	{
		return CEBEventSinkItem::pointer(new CEBEventSinkItem(dispid));
	}

	DISPID m_dispid;
	CComPtr<IEB_GroupInfo> m_pGroupInfo;
	CComPtr<IEB_MemberInfo> m_pMemberInfo;
	bool m_bIsMyGroup;
	bool m_bIsMyDefaultMember;

	CEBEventSinkItem(DISPID dispid)
		: m_dispid(dispid)
		, m_bIsMyGroup(false)
		, m_bIsMyDefaultMember(false)
	{
	}
	CEBEventSinkItem(void)
		: m_dispid(0)
		, m_bIsMyGroup(false)
		, m_bIsMyDefaultMember(false)
	{
	}
};
const CEBEventSinkItem::pointer CNullEBEventSinkItem;

typedef enum EB_WINDOW_MESSAGE_TYPE
{
	EB_WM_APPID_SUCCESS			= WM_USER+0x101
	
	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_APPID_ERROR

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_LOGON_APPCENTER
	, EB_WM_LOGOUT_APPCENTER

	// const EB_APMsgInfo* pAPMsgInfo = (const EB_APMsgInfo*)wp;
	, EB_WM_AP_MSG

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	// const char* pUserId = (const char*)lp;
	, EB_WM_REGISTER_RESPONSE

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_EDITINFO_RESPONSE

	// const EB_MemberInfo* pMemberInfo = (const char*)wp;
	// bool bIsOwnerMember = (bool)(lp==1);
	, EB_WM_USER_STATE_CHANGE
	, EB_WM_USER_HEAD_CHANGE

	// const char* sOAuthUrl = (const char*)wp;
	, EB_WM_OAUTH_FORWARD

	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	, EB_WM_LOGON_SUCCESS
	, EB_WM_LOGON_TIMEOUT

	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_LOGON_ERROR

	, EB_WM_ONLINE_ANOTHER
	, EB_WM_LOGOUT

	/*==========================================================
	会话
	===========================================================*/
	// const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wp;
	// const EB_AccountInfo* pFromAccount = (const EB_AccountInfo*)lp;
	, EB_WM_CALL_INCOMING	= WM_USER+0x201
	// const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wp;
	, EB_WM_CALL_ALERTING
	, EB_WM_CALL_BUSY
	// const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wp;
	// bool bOwner = (bool)(lp==1);
	, EB_WM_CALL_HANGUP
	// const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_CALL_ERROR
	// const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wp;
	// int nConnectFlag = (int)lp;
	, EB_WM_CALL_CONNECTED

	/*==========================================================
	视频
	===========================================================*/
	// const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_V_REQUEST_RESPONSE	= WM_USER+0x211
	, EB_WM_V_ACK_RESPONSE

	// const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wp;
	// const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lp;
	, EB_WM_VIDEO_REQUEST
	, EB_WM_VIDEO_ACCEPT
	, EB_WM_VIDEO_REJECT
	, EB_WM_VIDEO_CLOSE

	/*==========================================================
	组织结构
	===========================================================*/
	// const EB_EnterpriseInfo* pEnterpriseInfo = (const EB_EnterpriseInfo*)wp;
	, EB_WM_ENTERPRISE_INFO	= WM_USER+0x301

	// const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wp;
	// bool bIsMyGroup = (bool)(lp==1);
	, EB_WM_GROUP_INFO
	, EB_WM_GROUP_DELETE
	// const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_GROUP_EDIT_ERROR

	// const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wp;
	// const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)lp;
	, EB_WM_REMOVE_GROUP
	, EB_WM_EXIT_GROUP

	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	// const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lp;
	, EB_WM_REQUEST_ADD2GROUP
	, EB_WM_INVITE_ADD2GROUP
	, EB_WM_REJECT_ADD2GROUP

	// const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wp;
	// bool bIsMyDefaultMember = (bool)(lp==1);
	, EB_WM_MEMBER_INFO
	, EB_WM_MEMBER_DELETE
	// const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_MEMBER_EDIT_ERROR

	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	// const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lp;
	, EB_WM_REQUEST_ADDCONTACT
	, EB_WM_REJECT_ADDCONTACT
	// const EB_ContactInfo* pContactInfo = (const EB_ContactInfo*)wp;
	, EB_WM_ACCEPT_ADDCONTACT

	/*==========================================================
	通讯录（联系人）
	===========================================================*/
	// const EB_ContactInfo* pContactInfo = (const EB_ContactInfo*)wp;
	, EB_WM_CONTACT_INFO	= WM_USER+0x311
	, EB_WM_CONTACT_DELETE

	// const EB_ResourceInfo* pResourceInfo = (const EB_ResourceInfo*)wp;
	, EB_WM_RESOURCE_INFO
	, EB_WM_RESOURCE_DELETE
	// const EB_ResourceInfo* pResourceInfo = (const EB_ResourceInfo*)wp;
	// const char* sOldParentResId = (const char*)lp;
	, EB_WM_RESOURCE_MOVE

	// const EB_UGInfo* pUGInfo = (const EB_UGInfo*)wp;
	, EB_WM_UG_INFO
	, EB_WM_UG_DELETE

	// const EB_ContactInfo* pContactInfo = (const EB_ContactInfo*)wp;
	, EB_WM_CONTACT_STATE_CHANGE

	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	// const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lp;
	, EB_WM_BROADCAST_MSG		= WM_USER+0x321

	// const EB_AreaInfo* pAreaInfo = (const EB_AreaInfo*)wp;
	// unsigned int nParameter = (unsigned int)lp;
	, EB_WM_AREA_INFO

	// const EB_VersionInfo* pNewVersionInfo = (const EB_VersionInfo*)wp;
	, EB_WM_NEW_VERSION			= WM_USER+0x401

	// SERVER_STATE nServerState = (SERVER_STATE)wp;
	, EB_WM_SERVER_CHENGE		= WM_USER+0x411
	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_STATE_CODE

};

//typedef enum CR_WINDOW_MESSAGE
//{
//	// const CCrInfo* pParameter = (const CCrInfo*)wp;
//	CR_WM_ENTER_ROOM			= WM_USER+0x1101
//	, CR_WM_EXIT_ROOM
//	/*
//	const CCrAccountInfo* pAccountInfo = (const CCrAccountInfo*)wParam;
//	*/
//	, CR_WM_USER_ENTER_ROOM
//	, CR_WM_USER_EXIT_ROOM
//	/*
//	const CCrRichInfo * pRichInfo = (const CCrRichInfo*)wParam;
//	*/
//	, CR_WM_RECEIVE_RICH			= WM_USER+0x1111
//	, CR_WM_SEND_RICH
//	/*
//	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
//	*/
//	, CR_WM_SENDING_FILE
//	, CR_WM_SENT_FILE
//	, CR_WM_CANCEL_FILE
//	, CR_WM_RECEIVING_FILE
//	, CR_WM_RECEIVED_FILE
//	/*
//	const CChatRoomFilePercent * pChatRoomFilePercent = (const CChatRoomFilePercent*)wParam;
//	*/
//	, CR_WM_FILE_PERCENT
//	///*
//	//const char* sResourceId = (const char*)wParam;
//	//lParam: nParam or sParam
//	//*/
//	//, CR_WM_SET_RESOURCE_OK			= WM_USER+0x1121
//	//, CR_WM_SET_RESOURCE_ERROR
//};

const _ATL_FUNC_INFO onAppIdSuccessInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_I8,VT_BSTR } };
const _ATL_FUNC_INFO onAppIdErrorInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_I2 } };
const _ATL_FUNC_INFO onLogonAppCenterInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_I2 } };
const _ATL_FUNC_INFO onLogoutAppCenterInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_I2 } };
const _ATL_FUNC_INFO onAPMsgInfoInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onRegisterResponseInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_I2,VT_UI8 } };
const _ATL_FUNC_INFO onOAuthForwardInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
const _ATL_FUNC_INFO onLogonSuccessInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onLogonTimeoutInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onLogonErrorInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onOnlineAnotherInfo		= {CC_STDCALL, VT_EMPTY, 0, { } };
const _ATL_FUNC_INFO onLogoutInfo				= {CC_STDCALL, VT_EMPTY, 0, { } };
const _ATL_FUNC_INFO onEditInfoResponseInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_I2 } };
const _ATL_FUNC_INFO onUserStateChangeInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onUserHeadChangeInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onCallIncomingInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onCallAlertingInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onCallBusyInfo				= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onCallHangupInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onCallErrorInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onCallConnectedInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I4 } };
const _ATL_FUNC_INFO onVRequestResponseInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onVAckResponseInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onVideoRequestInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onVideoAcceptInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onVideoRejectInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onVideoCloseInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onEnterpriseInfoInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onGroupInfoInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onGroupDeleteInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onGroupEditErrorInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onRemoveGroupInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onAreaInfoInfo				= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_UI4 } };
const _ATL_FUNC_INFO onExitGroupInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onMemberInfoInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onMemberDeleteInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };
const _ATL_FUNC_INFO onMemberEditErrorInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO onContactInfoInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onContactDeleteInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onUGInfoInfo				= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onUGDeleteInfo				= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onRequestAddContactInfo	= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onRejectAddContactInfo		= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_DISPATCH } };
const _ATL_FUNC_INFO onAcceptAddContactInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onContactStateChangeInfo	= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onResourceInfoInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onResourceDeleteInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onResourceMoveInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BSTR } };
const _ATL_FUNC_INFO onNewVersionInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO onServerChangeInfo			= {CC_STDCALL, VT_EMPTY, 1, { VT_I2 } };
const _ATL_FUNC_INFO onStateCodeInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_I2,VT_I4 } };
const _ATL_FUNC_INFO OnEnterRoomInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_I2 } };
const _ATL_FUNC_INFO OnExitRoomInfo				= {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };
const _ATL_FUNC_INFO OnUserExitRoomInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH,VT_BOOL } };

//#define USES_AFX_MANAGE_STATE

class CEBCoreEventsSink
 : public IDispEventSimpleImpl<1, CEBCoreEventsSink,&__uuidof(_IEBClientCoreEvents)>
{
public:
	HWND m_hFireWnd;
	CEBCoreEventsSink(void)
		: m_hFireWnd(NULL)
	{}
	virtual ~CEBCoreEventsSink() {}

	virtual HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID /*lcid*/, _ATL_FUNC_INFO& info)
	{
		switch (dispidMember)
		{
		case 1:
			info = onAppIdSuccessInfo;
			return S_OK;
		case 2:	// onAppIdErrorInfo
		case 3:	// onLogonAppCenterInfo
		case 4:	// onLogoutAppCenterInfo
			info = onAppIdErrorInfo;
			return S_OK;
		case 5:
			info = onAPMsgInfoInfo;
			return S_OK;
		case 6:
			info = onRegisterResponseInfo;
			return S_OK;
		case 7:
			info = onOAuthForwardInfo;
			return S_OK;
		case 8:	// onLogonSuccessInfo
		case 9:	// onLogonTimeoutInfo
			info = onLogonSuccessInfo;
			return S_OK;
		case 10:
			info = onLogonErrorInfo;
			return S_OK;
		case 11:	// onOnlineAnotherInfo
		case 12:	// onLogoutInfo
			info = onOnlineAnotherInfo;
			return S_OK;
		case 13:
			info = onEditInfoResponseInfo;
			return S_OK;
		case 14:	// onUserStateChangeInfo
		case 15:	// onUserHeadChangeInfo
			info = onUserStateChangeInfo;
			return S_OK;
		case 20:
			info = onCallIncomingInfo;
			return S_OK;
		case 21:	// onCallAlertingInfo
		case 22:	// onCallBusyInfo
			info = onCallAlertingInfo;
			return S_OK;
		case 23:
			info = onCallHangupInfo;
			return S_OK;
		case 24:
			info = onCallErrorInfo;
			return S_OK;
		case 25:
			info = onCallConnectedInfo;
			return S_OK;
		case 30:	// onVRequestResponseInfo
		case 31:	// onVAckResponseInfo
			info = onVRequestResponseInfo;
			return S_OK;
		case 32:	// onVideoRequestInfo
		case 33:	// onVideoAcceptInfo
		case 34:	// onVideoRejectInfo
		case 35:	// onVideoCloseInfo
			info = onVideoRequestInfo;
			return S_OK;
		case 40:
			info = onEnterpriseInfoInfo;
			return S_OK;
		case 41:	// onGroupInfoInfo
		case 42:	// onGroupDeleteInfo
			info = onGroupInfoInfo;
			return S_OK;
		case 43:	// onGroupEditErrorInfo
			info = onGroupEditErrorInfo;
			return S_OK;
		case 44:	// onRemoveGroupInfo
		case 45:	// onExitGroup
		case 120:	// onRequestAdd2Group
		case 121:	// onInviteAdd2Group
		case 122:	// onRejectAdd2Group
		case 130:	// onBroadcastMsg
			info = onRemoveGroupInfo;
			return S_OK;
		case 131:	// onAreaInfo
			info = onAreaInfoInfo;
			return S_OK;
		case 46:	// onMemberInfoInfo
		case 47:	// onMemberDeleteInfo
			info = onMemberInfoInfo;
			return S_OK;
		case 48:	// onMemberEditErrorInfo
			info = onMemberEditErrorInfo;
			return S_OK;
		case 50:	// onContactInfoInfo
		case 51:	// onContactDeleteInfo
		case 53:	// onUGInfoInfo
		case 54:	// onUGDeleteInfo
		case 57:	// onAcceptAdContactInfo
		case 58:	// onContactStateChangeInfo
		case 60:	// onResourceInfoInfo
		case 61:	// onResourceDeleteInfo
			info = onContactInfoInfo;
			return S_OK;
		case 55:	// onRequestAdContactInfo
		case 56:	// onRejectAdContactInfo
			info = onRequestAddContactInfo;
			return S_OK;
		case 62:
			info = onResourceMoveInfo;
			return S_OK;
		case 70:
			info = onNewVersionInfo;
			return S_OK;
		case 71:
			info = onServerChangeInfo;
			return S_OK;
		case 75:
			info = onStateCodeInfo;
			return S_OK;
		case 80:	// 80-91 CM Event Sink
		case 84:
		case 86:
		case 87:
		case 92:	// OnSave2CloudDrive
			info = OnEnterRoomInfo;
			return S_OK;
		case 83:
			info = OnUserExitRoomInfo;
			return S_OK;
		case 81:
		case 82:
		case 85:
		case 88:
		case 89:
		case 90:
		case 91:
		case 100:	// OnEnterAnother
		case 110:	// OnP2PRequest
		case 111:	// OnP2PRequest
		case 112:	// OnP2PRequest
			info = OnExitRoomInfo;
			return S_OK;
		default:
			break;
		}
		return E_NOTIMPL;
	}
	//virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
	//	/* [in] */ DISPID dispIdMember,
	//	/* [in] */ REFIID riid,
	//	/* [in] */ LCID lcid,
	//	/* [in] */ WORD wFlags,
	//	/* [out][in] */ DISPPARAMS *pDispParams,
	//	/* [out] */ VARIANT *pVarResult,
	//	/* [out] */ EXCEPINFO *pExcepInfo,
	//	/* [out] */ UINT *puArgErr)
	//{
	//	switch (dispIdMember)
	//	{
	//	case 41:
	//		{
	//			if (pDispParams == NULL || pDispParams->cArgs != 2 || pDispParams->rgvarg == NULL)
	//				return E_INVALIDARG;
	//			IDispatch * pGroupInfo = pDispParams->rgvarg[1].pdispVal;
	//			VARIANT_BOOL bIsMyGroup = pDispParams->rgvarg[0].boolVal;
	//			return onGroupInfo(pGroupInfo, bIsMyGroup);
	//		}break;
	//	case 45:
	//		{
	//			if (pDispParams == NULL || pDispParams->cArgs != 2 || pDispParams->rgvarg == NULL)
	//				return E_INVALIDARG;
	//			IDispatch * pMemberInfo = pDispParams->rgvarg[1].pdispVal;
	//			VARIANT_BOOL bIsMyDefaultMember = pDispParams->rgvarg[0].boolVal;
	//			return onMemberInfo(pMemberInfo, bIsMyDefaultMember);
	//		}break;
	//	default:
	//		break;
	//	}
	//	return S_OK;
	//}

	BEGIN_SINK_MAP(CEBCoreEventsSink)
		// ATL宏映射事件函数
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 1, onAppIdSuccess)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 2, onAppIdError)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 3, onLogonAppCenter)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 4, onLogoutAppCenter)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 5, onAPMsgInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 6, onRegisterResponse)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 7, onOAuthForward)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 8, onLogonSuccess)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 9, onLogonTimeout)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 10, onLogonError)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 11, onOnlineAnother)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 12, onLogout)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 13, onEditInfoResponse)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 14, onUserStateChange)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 15, onUserHeadChange)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 20, onCallIncoming)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 21, onCallAlerting)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 22, onCallBusy)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 23, onCallHangup)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 24, onCallError)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 25, onCallConnected)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 30, onVRequestResponse)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 31, onVAckResponse)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 32, onVideoRequest)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 33, onVideoAccept)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 34, onVideoReject)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 35, onVideoClose)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 40, onEnterpriseInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 41, onGroupInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 42, onGroupDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 43, onGroupEditError)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 44, onRemoveGroup)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 45, onExitGroup)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 120, onRequestAdd2Group)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 121, onInviteAdd2Group)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 122, onRejectAdd2Group)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 46, onMemberInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 47, onMemberDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 48, onMemberEditError)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 50, onContactInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 51, onContactDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 53, onUGInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 54, onUGDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 55, onRequestAddContact)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 56, onRejectAddContact)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 57, onAcceptAddContact)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 58, onContactStateChange)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 60, onResourceInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 61, onResourceDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 62, onResourceMove)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 130, onBroadcastMsg)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 131, onAreaInfo)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 70, onNewVersion)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 71, onServerChange)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 75, onStateCode)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 80, OnEnterRoom)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 81, OnExitRoom)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 82, OnUserEnterRoom)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 83, OnUserExitRoom)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 84, OnSendRich)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 85, OnReceiveRich)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 86, OnSendingFile)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 87, OnSentFile)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 88, OnCancelFile)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 89, OnReceivingFile)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 90, OnReceivedFile)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 91, OnFilePercent)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 92, OnSave2CloudDrive)
		SINK_ENTRY_EX(1, __uuidof(_IEBClientCoreEvents), 100, OnEnterAnother)
	END_SINK_MAP()

	HRESULT _stdcall onAppIdSuccess(eb::bigint sAppId, BSTR sAppOnlineKey) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_APPID_SUCCESS,(WPARAM)sAppId,(LPARAM)sAppOnlineKey);
		else
			Fire_onAppIdSuccess(sAppId,CEBString(sAppOnlineKey));
		return S_OK;}
	HRESULT _stdcall onAppIdError(short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_APPID_ERROR,(WPARAM)nState,0);
		else
			Fire_onAppIdError((EB_STATE_CODE)nState);
		return S_OK;}
	HRESULT _stdcall onLogonAppCenter(short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_LOGON_APPCENTER,(WPARAM)nState,0);
		else
			Fire_onLogonAppCenter((EB_STATE_CODE)nState);
		return S_OK;}
	HRESULT _stdcall onLogoutAppCenter(short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_LOGOUT_APPCENTER,(WPARAM)nState,0);
		else
			Fire_onLogoutAppCenter((EB_STATE_CODE)nState);
		return S_OK;}
	HRESULT _stdcall onAPMsgInfo(IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_APMsgInfo> pInterface;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface);
		if (pInterface == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_AP_MSG,(WPARAM)(IEB_APMsgInfo*)pInterface,0);
		else
			Fire_onAPMsgInfo(pInterface);
		return S_OK; }
	HRESULT _stdcall onRegisterResponse(short nState,eb::bigint nUserId) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
		{
			char lpszUserId[24];
			sprintf(lpszUserId,"%lld",nUserId);
			::SendMessage(m_hFireWnd,EB_WM_REGISTER_RESPONSE,(WPARAM)nState,(LPARAM)&lpszUserId);
		}else
			Fire_onRegisterResponse((EB_STATE_CODE)nState,nUserId);
		return S_OK;}
	HRESULT _stdcall onOAuthForward(BSTR sOAuthUrl) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_OAUTH_FORWARD,(WPARAM)sOAuthUrl,0);
		else
			Fire_onOAuthForward(sOAuthUrl);
		return S_OK;}
	HRESULT _stdcall onLogonSuccess(IDispatch * pAccountInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_LOGON_SUCCESS,(WPARAM)(IEB_AccountInfo*)pInterface,0);
			else
				Fire_onLogonSuccess(pInterface);
		}
		return S_OK; }
	HRESULT _stdcall onLogonTimeout(IDispatch * pAccountInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_LOGON_TIMEOUT,(WPARAM)(IEB_AccountInfo*)pInterface,0);
			else
				Fire_onLogonTimeout(pInterface);
		}
		return S_OK; }
	HRESULT _stdcall onLogonError(IDispatch * pAccountInfo, short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_LOGON_ERROR,(WPARAM)(IEB_AccountInfo*)pInterface,0);
			else
				Fire_onLogonError(pInterface, (EB_STATE_CODE)nState);
		}
		return S_OK; }
	HRESULT _stdcall onOnlineAnother(void) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_ONLINE_ANOTHER,0,0);
		else
			Fire_onOnlineAnother();
		return S_OK;}
	HRESULT _stdcall onLogout(void) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_LOGOUT,0,0);
		else
			Fire_onLogout();
		return S_OK;}
	HRESULT _stdcall onEditInfoResponse(short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_EDITINFO_RESPONSE,(WPARAM)nState,0);
		else
			Fire_onEditInfoResponse((EB_STATE_CODE)nState);
		return S_OK;}
	HRESULT _stdcall onUserStateChange(IDispatch * pMemberInfo, VARIANT_BOOL bIsOwnerMember) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_MemberInfo> pInterface;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_USER_STATE_CHANGE,(WPARAM)(IEB_MemberInfo*)pInterface,bIsOwnerMember?1:0);
			else
				Fire_onUserStateChange(pInterface, bIsOwnerMember?true:false);
		} return S_OK; }
	HRESULT _stdcall onUserHeadChange(IDispatch * pMemberInfo, VARIANT_BOOL bIsOwnerMember) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_MemberInfo> pInterface;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_USER_HEAD_CHANGE,(WPARAM)(IEB_MemberInfo*)pInterface,bIsOwnerMember?1:0);
			else
				Fire_onUserHeadChange(pInterface, bIsOwnerMember?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onCallIncoming(IDispatch * pCallInfo, IDispatch* pFromAccount) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface1;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_AccountInfo> pInterface2;
		pFromAccount->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_INCOMING,(WPARAM)(IEB_CallInfo*)pInterface1,(LPARAM)(IEB_AccountInfo*)pInterface2);
			else
				Fire_onCallIncoming(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onCallAlerting(IDispatch * pCallInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_ALERTING,(WPARAM)(IEB_CallInfo*)pInterface,(LPARAM)0);
			else
				Fire_onCallAlerting(pInterface);
		}
		return S_OK; }
	HRESULT _stdcall onCallBusy(IDispatch * pCallInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_BUSY,(WPARAM)(IEB_CallInfo*)pInterface,(LPARAM)0);
			else
				Fire_onCallBusy(pInterface);
		}
		return S_OK; }
	HRESULT _stdcall onCallHangup(IDispatch * pCallInfo, VARIANT_BOOL bOwner) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_HANGUP,(WPARAM)(IEB_CallInfo*)pInterface,bOwner?1:0);
			else
				Fire_onCallHangup(pInterface, bOwner?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onCallError(IDispatch * pCallInfo, short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_ERROR,(WPARAM)(IEB_CallInfo*)pInterface,(LPARAM)nState);
			else
				Fire_onCallError(pInterface, (EB_STATE_CODE)nState);
		}
		return S_OK; }
	HRESULT _stdcall onCallConnected(IDispatch * pCallInfo, long nConnectFlag) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_CallInfo> pInterface;
		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CALL_CONNECTED,(WPARAM)(IEB_CallInfo*)pInterface,(LPARAM)nConnectFlag);
			else
				Fire_onCallConnected(pInterface, nConnectFlag);
		}
		return S_OK; }
	HRESULT _stdcall onVRequestResponse(IDispatch * pVideoInfo, short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_V_REQUEST_RESPONSE,(WPARAM)(IEB_VideoInfo*)pInterface,(LPARAM)nState);
			else
				Fire_onVRequestResponse(pInterface, (EB_STATE_CODE)nState);
		}
		return S_OK; }
	HRESULT _stdcall onVAckResponse(IDispatch * pVideoInfo, short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_V_ACK_RESPONSE,(WPARAM)(IEB_VideoInfo*)pInterface,(LPARAM)nState);
			else
				Fire_onVAckResponse(pInterface, (EB_STATE_CODE)nState);
		}
		return S_OK; }
	HRESULT _stdcall onVideoRequest(IDispatch * pVideoInfo, IDispatch* sFromAccount) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface1;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_UserVideoInfo> pInterface2;
		sFromAccount->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_VIDEO_REQUEST,(WPARAM)(IEB_VideoInfo*)pInterface1,(LPARAM)(IEB_UserVideoInfo*)pInterface2);
			else
				Fire_onVideoRequest(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onVideoAccept(IDispatch * pVideoInfo, IDispatch* sFromAccount) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface1;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_UserVideoInfo> pInterface2;
		sFromAccount->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_VIDEO_ACCEPT,(WPARAM)(IEB_VideoInfo*)pInterface1,(LPARAM)(IEB_UserVideoInfo*)pInterface2);
			else
				Fire_onVideoAccept(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onVideoReject(IDispatch * pVideoInfo, IDispatch* sFromAccount) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface1;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_UserVideoInfo> pInterface2;
		sFromAccount->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_VIDEO_REJECT,(WPARAM)(IEB_VideoInfo*)pInterface1,(LPARAM)(IEB_UserVideoInfo*)pInterface2);
			else
				Fire_onVideoReject(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onVideoClose(IDispatch * pVideoInfo, IDispatch* sFromAccount) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VideoInfo> pInterface1;
		pVideoInfo->QueryInterface(__uuidof(IEB_VideoInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_UserVideoInfo> pInterface2;
		sFromAccount->QueryInterface(__uuidof(IEB_UserVideoInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_VIDEO_CLOSE,(WPARAM)(IEB_VideoInfo*)pInterface1,(LPARAM)(IEB_UserVideoInfo*)pInterface2);
			else
				Fire_onVideoClose(pInterface1, pInterface2);} return S_OK; }
	HRESULT _stdcall onEnterpriseInfo(IDispatch * pEnterpriseInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_EnterpriseInfo> pInterface;
		pEnterpriseInfo->QueryInterface(__uuidof(IEB_EnterpriseInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_ENTERPRISE_INFO,(WPARAM)(IEB_EnterpriseInfo*)pInterface,(LPARAM)0);
			else
				Fire_onEnterpriseInfo(pInterface);
		}
		return S_OK; }
	HRESULT _stdcall onGroupInfo(IDispatch * pGroupInfo, VARIANT_BOOL bIsMyGroup) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		//return S_OK;
		CComPtr<IEB_GroupInfo> pInterface;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			//if (m_pThread != NULL)
			//{
			//	CEBEventSinkItem::pointer pEventSinkItem = CEBEventSinkItem::create(41);
			//	pEventSinkItem->m_pGroupInfo = pInterface;
			//	pEventSinkItem->m_bIsMyGroup = bIsMyGroup?true:false;
			//	m_list.add(pEventSinkItem);
			//	return S_OK;
			//}
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_GROUP_INFO,(WPARAM)(IEB_GroupInfo*)pInterface,bIsMyGroup?1:0);
			else
				Fire_onGroupInfo(pInterface, bIsMyGroup?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onGroupDelete(IDispatch * pGroupInfo, VARIANT_BOOL bIsMyGroup) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_GroupInfo> pInterface;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_GROUP_DELETE,(WPARAM)(IEB_GroupInfo*)pInterface,bIsMyGroup?1:0);
			else
				Fire_onGroupDelete(pInterface, bIsMyGroup?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onGroupEditError(IDispatch * pGroupInfo, EB_STATE_CODE bState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_GroupInfo> pInterface;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_GROUP_EDIT_ERROR,(WPARAM)(IEB_GroupInfo*)pInterface,(LPARAM)bState);
			else
				Fire_onGroupEditError(pInterface, bState);
		}
		return S_OK; }
	HRESULT _stdcall onRemoveGroup(IDispatch * pGroupInfo, IDispatch* pMemberInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_GroupInfo> pInterface1;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_MemberInfo> pInterface2;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_REMOVE_GROUP,(WPARAM)(IEB_GroupInfo*)pInterface1,(LPARAM)(IEB_MemberInfo*)pInterface2);
			else
				Fire_onRemoveGroup(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onExitGroup(IDispatch * pGroupInfo, IDispatch* pMemberInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_GroupInfo> pInterface1;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		CComPtr<IEB_MemberInfo> pInterface2;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface2);
		if (pInterface2 != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_EXIT_GROUP,(WPARAM)(IEB_GroupInfo*)pInterface1,(LPARAM)(IEB_MemberInfo*)pInterface2);
			else
				Fire_onExitGroup(pInterface1, pInterface2);
		}
		return S_OK; }
	HRESULT _stdcall onRequestAdd2Group(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_REQUEST_ADD2GROUP,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onRequestAdd2Group(pInterface1,pInterface2); return S_OK; }
	HRESULT _stdcall onInviteAdd2Group(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_INVITE_ADD2GROUP,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onInviteAdd2Group(pInterface1,pInterface2); return S_OK; }
	HRESULT _stdcall onRejectAdd2Group(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_REJECT_ADD2GROUP,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onRejectAdd2Group(pInterface1,pInterface2); return S_OK; }

	HRESULT _stdcall onMemberInfo(IDispatch * pMemberInfo, VARIANT_BOOL bIsMyDefaultMember) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_MemberInfo> pInterface;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			//if (m_pThread != NULL)
			//{
			//	CEBEventSinkItem::pointer pEventSinkItem = CEBEventSinkItem::create(45);
			//	pEventSinkItem->m_pMemberInfo = pInterface;
			//	pEventSinkItem->m_bIsMyDefaultMember = bIsMyDefaultMember?true:false;
			//	m_list.add(pEventSinkItem);
			//	return S_OK;
			//}
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_MEMBER_INFO,(WPARAM)(IEB_MemberInfo*)pInterface,(LPARAM)0);
			else
				Fire_onMemberInfo(pInterface, bIsMyDefaultMember?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onMemberDelete(IDispatch * pMemberInfo, VARIANT_BOOL bIsMyDefaultMember) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_MemberInfo> pInterface;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_MEMBER_DELETE,(WPARAM)(IEB_MemberInfo*)pInterface,(LPARAM)0);
			else
				Fire_onMemberDelete(pInterface, bIsMyDefaultMember?true:false);
		}
		return S_OK; }
	HRESULT _stdcall onMemberEditError(IDispatch * pMemberInfo, SHORT nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_MemberInfo> pInterface;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_MEMBER_EDIT_ERROR,(WPARAM)(IEB_MemberInfo*)pInterface,nState);
			else
				Fire_onMemberEditError(pInterface, (EB_STATE_CODE)nState);
		}
		return S_OK; }
	HRESULT _stdcall onContactInfo(IDispatch * pContactInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ContactInfo> pInterface;
		pContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CONTACT_INFO,(WPARAM)(IEB_ContactInfo*)pInterface,(LPARAM)0);
			else
				Fire_onContactInfo(pInterface);} return S_OK; }
	HRESULT _stdcall onContactDelete(IDispatch * pContactInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ContactInfo> pInterface;
		pContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CONTACT_DELETE,(WPARAM)(IEB_ContactInfo*)pInterface,(LPARAM)0);
			else
				Fire_onContactDelete(pInterface);} return S_OK; }
	HRESULT _stdcall onUGInfo(IDispatch * pUGInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_UGInfo> pInterface;
		pUGInfo->QueryInterface(__uuidof(IEB_UGInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_UG_INFO,(WPARAM)(IEB_UGInfo*)pInterface,(LPARAM)0);
			else
				Fire_onUGInfo(pInterface);} return S_OK; }
	HRESULT _stdcall onUGDelete(IDispatch * pUGInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_UGInfo> pInterface;
		pUGInfo->QueryInterface(__uuidof(IEB_UGInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_UG_DELETE,(WPARAM)(IEB_UGInfo*)pInterface,(LPARAM)0);
			else
				Fire_onUGDelete(pInterface);} return S_OK; }
	HRESULT _stdcall onRequestAddContact(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;

		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_REQUEST_ADDCONTACT,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onRequestAddContact(pInterface1,pInterface2); return S_OK; }
	HRESULT _stdcall onRejectAddContact(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;

		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_REJECT_ADDCONTACT,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onRejectAddContact(pInterface1,pInterface2); return S_OK; }
	HRESULT _stdcall onAcceptAddContact(IDispatch * pContactInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ContactInfo> pInterface;
		pContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_ACCEPT_ADDCONTACT,(WPARAM)(IEB_ContactInfo*)pInterface,(LPARAM)0);
			else
				Fire_onAcceptAddContact(pInterface);} return S_OK; }
	HRESULT _stdcall onContactStateChange(IDispatch * pContactInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ContactInfo> pInterface;
		pContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_CONTACT_STATE_CHANGE,(WPARAM)(IEB_ContactInfo*)pInterface,(LPARAM)0);
			else
				Fire_onContactStateChange(pInterface);} return S_OK; }

	HRESULT _stdcall onResourceInfo(IDispatch * pResourceInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ResourceInfo> pInterface;
		pResourceInfo->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_RESOURCE_INFO,(WPARAM)(IEB_ResourceInfo*)pInterface,(LPARAM)0);
			else
				Fire_onResourceInfo(pInterface);} return S_OK; }
	HRESULT _stdcall onResourceDelete(IDispatch * pResourceInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ResourceInfo> pInterface;
		pResourceInfo->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_RESOURCE_DELETE,(WPARAM)(IEB_ResourceInfo*)pInterface,(LPARAM)0);
			else
				Fire_onResourceDelete(pInterface);} return S_OK; }
	HRESULT _stdcall onResourceMove(IDispatch * pResourceInfo, BSTR sOldParentResId) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ResourceInfo> pInterface;
		pResourceInfo->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_RESOURCE_MOVE,(WPARAM)(IEB_ResourceInfo*)pInterface,(LPARAM)sOldParentResId);
			else
				Fire_onResourceMove(pInterface, CEBString(sOldParentResId));} return S_OK; }

	HRESULT _stdcall onBroadcastMsg(IDispatch * pAccountInfo,IDispatch * pAPMsgInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AccountInfo> pInterface1;
		pAccountInfo->QueryInterface(__uuidof(IEB_AccountInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		CComPtr<IEB_APMsgInfo> pInterface2;
		pAPMsgInfo->QueryInterface(__uuidof(IEB_APMsgInfo),(void**)&pInterface2);
		if (pInterface2 == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_BROADCAST_MSG,(WPARAM)(IEB_AccountInfo*)pInterface1,(LPARAM)(IEB_APMsgInfo*)pInterface2);
		else
			Fire_onBroadcastMsg(pInterface1,pInterface2); return S_OK; }

	HRESULT _stdcall onAreaInfo(IDispatch * pAreaInfo,ULONG nParameter) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_AreaInfo> pInterface1;
		pAreaInfo->QueryInterface(__uuidof(IEB_AreaInfo),(void**)&pInterface1);
		if (pInterface1 == NULL) return S_FALSE;
		if (m_hFireWnd)
			::SendMessage(m_hFireWnd,EB_WM_AREA_INFO,(WPARAM)(IEB_AreaInfo*)pInterface1,(LPARAM)nParameter);
		else
			Fire_onAreaInfo(pInterface1,nParameter); return S_OK; }


	HRESULT _stdcall onNewVersion(IDispatch * pVersionInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_VersionInfo> pInterface;
		pVersionInfo->QueryInterface(__uuidof(IEB_VersionInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,EB_WM_NEW_VERSION,(WPARAM)(IEB_VersionInfo*)pInterface,(LPARAM)0);
			else
				Fire_onNewVersion(pInterface);} return S_OK; }
	HRESULT _stdcall onServerChange(SHORT nServerState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_SERVER_CHENGE,(WPARAM)nServerState,(LPARAM)0);
		else
			Fire_onServerChange(nServerState); return S_OK;}
	HRESULT _stdcall onStateCode(SHORT nStateCode,LONG nParam) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hFireWnd)
			::PostMessage(m_hFireWnd,EB_WM_SERVER_CHENGE,(WPARAM)nStateCode,(LPARAM)nParam);
		else
			Fire_onStateCode((EB_STATE_CODE)nStateCode,nParam); return S_OK;}

	// CM Event Sink
	HRESULT _stdcall OnEnterRoom(IDispatch * pCrInfo, short nState) {
		CComPtr<IEB_ChatInfo> pInterface;
		pCrInfo->QueryInterface(__uuidof(IEB_ChatInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_ENTER_ROOM,(WPARAM)(IEB_ChatInfo*)pInterface,(LPARAM)nState);
			else
				Fire_OnEnterRoom(pInterface,(EB_STATE_CODE)nState);} return S_OK; }
	HRESULT _stdcall OnExitRoom(IDispatch * pCrInfo) {
		CComPtr<IEB_ChatInfo> pInterface;
		pCrInfo->QueryInterface(__uuidof(IEB_ChatInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_EXIT_ROOM,(WPARAM)(IEB_ChatInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnExitRoom(pInterface);} return S_OK; }
	HRESULT _stdcall OnUserEnterRoom(IDispatch * pAccountInfo) {
		CComPtr<IEB_ChatAccountInfo> pInterface;
		pAccountInfo->QueryInterface(__uuidof(IEB_ChatAccountInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_USER_ENTER_ROOM,(WPARAM)(IEB_ChatAccountInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnUserEnterRoom(pInterface);} return S_OK; }
	HRESULT _stdcall OnUserExitRoom(IDispatch * pAccountInfo,VARIANT_BOOL bHangup) {
		CComPtr<IEB_ChatAccountInfo> pInterface;
		pAccountInfo->QueryInterface(__uuidof(IEB_ChatAccountInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_USER_EXIT_ROOM,(WPARAM)(IEB_ChatAccountInfo*)pInterface,bHangup==VARIANT_TRUE?1:0);
			else
				Fire_OnUserExitRoom(pInterface,bHangup==VARIANT_TRUE?true:false);} return S_OK; }
	HRESULT _stdcall OnSendRich(IDispatch * pRichInfo,short nState) {
		CComPtr<IEB_ChatRichInfo> pInterface;
		pRichInfo->QueryInterface(__uuidof(IEB_ChatRichInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_SEND_RICH,(WPARAM)(IEB_ChatRichInfo*)pInterface,(LPARAM)nState);
			else
				Fire_OnSendRich(pInterface,(EB_STATE_CODE)nState);} return S_OK; }
	HRESULT _stdcall OnReceiveRich(IDispatch * pRichInfo) {
		CComPtr<IEB_ChatRichInfo> pInterface;
		pRichInfo->QueryInterface(__uuidof(IEB_ChatRichInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_RECEIVE_RICH,(WPARAM)(IEB_ChatRichInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnReceiveRich(pInterface);} return S_OK; }
	HRESULT _stdcall OnSendingFile(IDispatch * pFileInfo,short nState) {
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_SENDING_FILE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)nState);
			else
				Fire_OnSendingFile(pInterface,(EB_STATE_CODE)nState);} return S_OK; }
	HRESULT _stdcall OnSentFile(IDispatch * pFileInfo,short nState) {
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_SENT_FILE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)nState);
			else
				Fire_OnSentFile(pInterface,(EB_STATE_CODE)nState);} return S_OK; }
	HRESULT _stdcall OnCancelFile(IDispatch * pFileInfo) {
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_CANCEL_FILE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnCancelFile(pInterface);} return S_OK; }
	HRESULT _stdcall OnReceivingFile(IDispatch * pFileInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_RECEIVING_FILE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnReceivingFile(pInterface);} return S_OK; }
	HRESULT _stdcall OnReceivedFile(IDispatch * pFileInfo) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_RECEIVED_FILE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnReceivedFile(pInterface);} return S_OK; }
	HRESULT _stdcall OnFilePercent(IDispatch * pFilePercent) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ChatFilePercent> pInterface;
		pFilePercent->QueryInterface(__uuidof(IEB_ChatFilePercent),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_FILE_PERCENT,(WPARAM)(IEB_ChatFilePercent*)pInterface,(LPARAM)0);
			else
				Fire_OnFilePercent(pInterface);} return S_OK; }
	HRESULT _stdcall OnSave2CloudDrive(IDispatch * pFileInfo, short nState) {
#ifdef USES_AFX_MANAGE_STATE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CComPtr<IEB_ChatFileInfo> pInterface;
		pFileInfo->QueryInterface(__uuidof(IEB_ChatFileInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_SAVE2CLOUD_DRIVE,(WPARAM)(IEB_ChatFileInfo*)pInterface,(LPARAM)nState);
			else
				Fire_OnSave2CloudDrive(pInterface, (EB_STATE_CODE)nState);} return S_OK; }

	HRESULT _stdcall OnEnterAnother(IDispatch * pCrInfo) {
		CComPtr<IEB_ChatInfo> pInterface;
		pCrInfo->QueryInterface(__uuidof(IEB_ChatInfo),(void**)&pInterface);
		if (pInterface != NULL)
		{
			if (m_hFireWnd)
				::SendMessage(m_hFireWnd,CR_WM_ENTER_ANOTHER,(WPARAM)(IEB_ChatInfo*)pInterface,(LPARAM)0);
			else
				Fire_OnEnterAnother(pInterface);} return S_OK; }

private:
	virtual void Fire_onAppIdSuccess(eb::bigint sAppId, const CEBString& sAppOnlineKey) {}
	virtual void Fire_onAppIdError(EB_STATE_CODE nState) {}

	virtual void Fire_onLogonAppCenter(EB_STATE_CODE nState) {}
	virtual void Fire_onLogoutAppCenter(EB_STATE_CODE nState) {}
	virtual void Fire_onAPMsgInfo(IEB_APMsgInfo * pAPMsgInfo) {}

	virtual void Fire_onRegisterResponse(EB_STATE_CODE nState,eb::bigint nUserId) {}

	virtual void Fire_onOAuthForward(BSTR sOAuthUrl) {}
	virtual void Fire_onLogonSuccess(IEB_AccountInfo * pAccountInfo) {}
	virtual void Fire_onLogonTimeout(IEB_AccountInfo * pAccountInfo) {}
	virtual void Fire_onLogonError(IEB_AccountInfo * pAccountInfo, EB_STATE_CODE nState) {}
	virtual void Fire_onOnlineAnother(void) {}
	virtual void Fire_onLogout(void) {}

	virtual void Fire_onEditInfoResponse(EB_STATE_CODE nState) {}
	virtual void Fire_onUserStateChange(IEB_MemberInfo * pMemberInfo, bool bIsOwnerMember) {}
	virtual void Fire_onUserHeadChange(IEB_MemberInfo * pMemberInfo, bool bIsOwnerMember) {}

	virtual void Fire_onCallIncoming(IEB_CallInfo * pCallInfo, IEB_AccountInfo* pFromAccount) {}
	virtual void Fire_onCallAlerting(IEB_CallInfo * pCallInfo) {}
	virtual void Fire_onCallBusy(IEB_CallInfo * pCallInfo) {}
	virtual void Fire_onCallHangup(IEB_CallInfo * pCallInfo, bool bOwner) {}
	virtual void Fire_onCallError(IEB_CallInfo * pCallInfo, EB_STATE_CODE nState) {}
	virtual void Fire_onCallConnected(IEB_CallInfo * pCallInfo, long nConnectFlag) {}

	virtual void Fire_onVRequestResponse(IEB_VideoInfo * pVideoInfo, EB_STATE_CODE nState) {}
	virtual void Fire_onVAckResponse(IEB_VideoInfo * pVideoInfo, EB_STATE_CODE nState) {}
	virtual void Fire_onVideoRequest(IEB_VideoInfo * pVideoInfo, IEB_UserVideoInfo* pUserVideoInfo) {}
	virtual void Fire_onVideoAccept(IEB_VideoInfo * pVideoInfo, IEB_UserVideoInfo* pUserVideoInfo) {}
	virtual void Fire_onVideoReject(IEB_VideoInfo * pVideoInfo, IEB_UserVideoInfo* pUserVideoInfo) {}
	virtual void Fire_onVideoClose(IEB_VideoInfo * pVideoInfo, IEB_UserVideoInfo* pUserVideoInfo) {}

	virtual void Fire_onEnterpriseInfo(IEB_EnterpriseInfo * pEnterpriseInfo) {}
	virtual void Fire_onGroupInfo(IEB_GroupInfo * pGroupInfo, bool bIsMyGroup) {}
	virtual void Fire_onGroupDelete(IEB_GroupInfo * pGroupInfo, bool bIsMyGroup) {}
	virtual void Fire_onGroupEditError(IEB_GroupInfo * pGroupInfo, EB_STATE_CODE bState) {}
	virtual void Fire_onRemoveGroup(IEB_GroupInfo * pGroupInfo, IEB_MemberInfo* pMemberInfo) {}
	virtual void Fire_onExitGroup(IEB_GroupInfo * pGroupInfo, IEB_MemberInfo* pMemberInfo) {}
	virtual void Fire_onRequestAdd2Group(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onInviteAdd2Group(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onRejectAdd2Group(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onMemberInfo(IEB_MemberInfo * pMemberInfo, bool bIsMyDefaultMember) {}
	virtual void Fire_onMemberDelete(IEB_MemberInfo * pMemberInfo, bool bIsMyDefaultMember) {}
	virtual void Fire_onMemberEditError(IEB_MemberInfo * pMemberInfo, EB_STATE_CODE bState) {}

	virtual void Fire_onRequestAddContact(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onRejectAddContact(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onAcceptAddContact(IEB_ContactInfo* pContactInfo) {}
	virtual void Fire_onContactStateChange(IEB_ContactInfo* pContactInfo) {}

	virtual void Fire_onContactInfo(IEB_ContactInfo * pContactInfo) {}
	virtual void Fire_onContactDelete(IEB_ContactInfo * pContactInfo) {}

	virtual void Fire_onResourceInfo(IEB_ResourceInfo* pResourceInfo) {}
	virtual void Fire_onResourceDelete(IEB_ResourceInfo* pResourceInfo) {}
	virtual void Fire_onResourceMove(IEB_ResourceInfo* pResourceInfo, const CEBString& sOldParentResId) {}
	
	virtual void Fire_onUGInfo(IEB_UGInfo* pUGInfo) {}
	virtual void Fire_onUGDelete(IEB_UGInfo* pUGInfo) {}

	virtual void Fire_onBroadcastMsg(IEB_AccountInfo * pAccountInfo, IEB_APMsgInfo* pApMsgInfo) {}
	virtual void Fire_onAreaInfo(IEB_AreaInfo * pAreaInfo, unsigned int nParameter) {}

	virtual void Fire_onNewVersion(IEB_VersionInfo* pVersionInfo) {}
	virtual void Fire_onServerChange(SHORT nServerState) {}
	virtual void Fire_onStateCode(EB_STATE_CODE nState, LONG nParam) {}

	// CM Event Sink
	virtual void Fire_OnEnterRoom(IEB_ChatInfo* pChatInfo,EB_STATE_CODE nStateCode) {}
	virtual void Fire_OnExitRoom(IEB_ChatInfo* pChatInfo) {}
	virtual void Fire_OnUserEnterRoom(IEB_ChatAccountInfo* pChatAccountInfo) {}
	virtual void Fire_OnUserExitRoom(IEB_ChatAccountInfo* pChatAccountInfo,bool bHangup) {}
	virtual void Fire_OnSendRich(IEB_ChatRichInfo* pChatRichInfo,EB_STATE_CODE nState) {}
	virtual void Fire_OnReceiveRich(IEB_ChatRichInfo* pChatRichInfo) {}
	virtual void Fire_OnSendingFile(IEB_ChatFileInfo* pChatFileInfo,EB_STATE_CODE nState) {}
	virtual void Fire_OnSentFile(IEB_ChatFileInfo* pChatFileInfo,EB_STATE_CODE nState) {}
	virtual void Fire_OnCancelFile(IEB_ChatFileInfo* pChatFileInfo) {}
	virtual void Fire_OnReceivingFile(IEB_ChatFileInfo* pChatFileInfo) {}
	virtual void Fire_OnReceivedFile(IEB_ChatFileInfo* pChatFileInfo) {}
	virtual void Fire_OnFilePercent(IEB_ChatFilePercent* pChatFilePercent) {}
	virtual void Fire_OnSave2CloudDrive(IEB_ChatFileInfo* pChatFileInfo, EB_STATE_CODE nState) {}
	virtual void Fire_OnEnterAnother(IEB_ChatInfo* pChatInfo) {}
};

#endif

#endif // __EBCoreEventsSink_h__
