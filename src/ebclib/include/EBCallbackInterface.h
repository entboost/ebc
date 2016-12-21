// EBCallbackInterface.h file here

#ifndef __EBCallbackInterface_h__
#define __EBCallbackInterface_h__
#ifdef WIN32
#include "Windows.h"
#endif // WIN32
#include "eb_object.h"
#include "chatroomhandle.h"

namespace entboost {

//搜索回调函数
class CEBSearchCallback
{
public:
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo, unsigned long dwParam) = 0;
	virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam) = 0;
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam) = 0;
};

#define EB_CALLBACK_FIRST_VIDEO_FPS	1
#define EB_CALLBACK_VOICE_VOLUME	2
typedef void (*PEBVideoCallBack) (eb::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData);


// 应用回调函数
class CEBCallbackInterface
#ifdef USES_CHATROOM
	: public CChatRoomCallBack
#endif
{
public:
	// APP
	virtual void onAppIdSuccess(eb::bigint appId,const char* appOnlineKey) {}
	virtual void onAppIdError(EB_STATE_CODE nState) {}

	virtual void onLogonAppCenter(EB_STATE_CODE nState) {}
	virtual void onLogoutAppCenter(EB_STATE_CODE nState) {}
	virtual int onAPMsgInfo(const EB_APMsgInfo& pAPMsgInfo) {return 1;}	// 返回1，自动响应

	virtual void onRegisterResponse(EB_STATE_CODE nState,eb::bigint nUserId) {}

	virtual void onOAuthForward(const std::string& sOAuthUrl) {}
	virtual void onLogonSuccess(const EB_AccountInfo& pAccountInfo) {}
	virtual void onLogonTimeout(const EB_AccountInfo& pAccountInfo) {}
	virtual void onLogonError(const EB_AccountInfo& pAccountInfo,EB_STATE_CODE nState) {}
	virtual void onOnlineAnother(void) {}	// 在其他地方登录
	virtual void onLogout(void) {}
	virtual void onAccountInfoChange(const EB_AccountInfo& pAccountInfo) {}

	virtual void onEditInfoResponse(EB_STATE_CODE nState, int nFlag) {}								// 个人资料修改返回
	virtual void onUserStateChange(const EB_MemberInfo* pMemberInfo, bool bIsOwnerMember) {}		// 修改在线状态
	virtual void onUserHeadChange(const EB_MemberInfo* pMemberInfo, bool bIsOwnerMember) {}			// 修改群组头像

	// 呼叫（会话）
	virtual void onCallIncoming(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccount) {}
	virtual void onCallAlerting(const EB_CallInfo& pCallInfo) {}
	virtual void onCallBusy(const EB_CallInfo& pCallInfo) {}
	virtual void onCallHangup(const EB_CallInfo& pCallInfo, bool bOwner) {}
	virtual void onCallError(const EB_CallInfo& pCallInfo,EB_STATE_CODE nState) {}
	virtual void onCallConnected(const EB_CallInfo& pCallInfo, int nConnectFlag) {}

	// 远程桌面（远程协助）
	virtual void onRDRequestResponse(const EB_RemoteDesktopInfo& pRDInfo, EB_STATE_CODE nState) {}
	virtual void onRDAckResponse(const EB_RemoteDesktopInfo& pRDInfo, EB_STATE_CODE nState) {}
	virtual void onRDRequest(const EB_RemoteDesktopInfo& pRDInfo) {}									// 收到一个RD连接请求
	virtual void onRDAccept(const EB_RemoteDesktopInfo& pRDInfo) {}										// 对方接受RD
	virtual void onRDReject(const EB_RemoteDesktopInfo& pRDInfo) {}										// 对方拒绝RD
	virtual void onRDClose(const EB_RemoteDesktopInfo& pRDInfo) {}										// 对方关闭RD

	// 视频
	virtual void onVRequestResponse(const EB_VideoInfo& pVideoInfo, EB_STATE_CODE nState) {}
	virtual void onVAckResponse(const EB_VideoInfo& pVideoInfo, EB_STATE_CODE nState) {}
	virtual void onVideoRequest(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {}		// 收到一个视频连接请求
	virtual void onVideoAccept(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {}		// 对方接受视频
	virtual void onVideoReject(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {}		// 对方拒绝视频
	virtual void onVideoTimeout(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {}	// 视频请求超时
	virtual void onVideoClose(const EB_VideoInfo& pVideoInfo, const EB_UserVideoInfo& sFromAccount) {}		// 对方关闭视频
	
	// 组织结构
	virtual void onEnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo) {}						// 
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo, bool bIsMyGroup) {}			// 
	virtual void onGroupDelete(const EB_GroupInfo* pGroupInfo, bool bIsMyGroup) {}			//
	virtual void onGroupEditError(const EB_GroupInfo* pGroupInfo, EB_STATE_CODE nState) {}			//
	virtual void onRemoveGroup(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo) {}			// 成员被移出群组
	virtual void onExitGroup(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo) {}			// 用户退出群组
	virtual void onRequestAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 用户申请加入群组
	virtual void onInviteAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 用户邀请加入群组
	virtual void onRejectAdd2Group(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 拒绝加入群组
	virtual void onMemberInfo(const EB_MemberInfo* pMemberInfo, bool bIsMyDefaultMember) {}							// 
	virtual void onMemberDelete(const EB_MemberInfo* pMemberInfo, bool bIsMyDefaultMember) {}						// 
	virtual void onMemberEditError(const EB_MemberInfo* pMemberInfo, EB_STATE_CODE nState) {}						// 

	// 通讯录（联系人）
	virtual void onUGInfo(const EB_UGInfo* pUGInfo) {}							// 
	virtual void onUGDelete(const EB_UGInfo* pUGInfo) {}							// 
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo) {}							// 
	virtual void onContactDelete(const EB_ContactInfo* pContactInfo) {}							// 
	virtual void onRequestAddContact(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 用户申请加为好友
	virtual void onRejectAddContact(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 对方拒绝加为好友
	virtual void onAcceptAddContact(const EB_ContactInfo* pContactInfo) {}									// 加为好友成功
	virtual void onContactStateChange(const EB_ContactInfo* pContactInfo) {}								// 修改在线状态
	
	// 在线资源（云盘）
	virtual void onResourceInfo(const EB_ResourceInfo* pResourceInfo) {}							// 
	virtual void onResourceDelete(const EB_ResourceInfo* pResourceInfo) {}							// 
	virtual void onResourceMove(const EB_ResourceInfo* pResourceInfo,eb::bigint sOldParentResId) {}							// 

	virtual void onBroadcastMsg(const EB_AccountInfo* pAccountInfo,const EB_APMsgInfo* pApMsgInfo) {}	// 收到一条广播消息
	virtual void onAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter) {}					// 收到地区资料消息

	virtual void onNewVersion(const EB_VersionInfo& pVersionInfo) {}		// 有新版本需要更新
	typedef enum SERVER_STATE
	{
		SERVER_UNKNOWN		// 其他状态
		, SERVER_STOPED		// 服务器停止、维护状态中，提示用户
		, SERVER_MOVED		// 服务器业务已经转移，需要重新登录
		, SERVER_RESTART	// 服务器已经重启，需要重新登录
		, SERVER_TIMEOUT	// 服务器连接超时，有可能是本地网络问题
	};
	virtual void onServerChange(SERVER_STATE nServerState) {}			// 服务器状态改变
	virtual void onStateCode(EB_STATE_CODE nStateCode, unsigned long nParam) {}			// 错误状态码
};

#ifdef EB_SERVER
#define WM_USER 0
#endif
#ifndef WM_USER
#define WM_USER 0
#endif

typedef enum EB_WINDOW_MESSAGE_TYPE
{
	EB_WM_UNKNOWN				= 0
	, EB_WM_APPID_SUCCESS		= WM_USER+0x101
	
	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_APPID_ERROR

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	, EB_WM_LOGON_APPCENTER
	, EB_WM_LOGOUT_APPCENTER

	// const EB_APMsgInfo* pAPMsgInfo = (const EB_APMsgInfo*)wp;
	, EB_WM_AP_MSG

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	// const char* pUserId = (const chat*)lp;
	, EB_WM_REGISTER_RESPONSE

	// EB_STATE_CODE nState = (EB_STATE_CODE)wp;
	// int nFlag = (int)lp;
	, EB_WM_EDITINFO_RESPONSE

	// const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wp;
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
	// const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wp;
	, EB_WM_ACCOUNT_INFO_CHANGE

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
	, EB_WM_VIDEO_TIMEOUT
	, EB_WM_VIDEO_CLOSE

	/*==========================================================
	视频
	===========================================================*/
	// const EB_RemoteDesktopInfo* pRemoteDesktopInfo = (const EB_RemoteDesktopInfo*)wp;
	// EB_STATE_CODE nState = (EB_STATE_CODE)lp;
	, EB_WM_RD_REQUEST_RESPONSE	= WM_USER+0x221
	, EB_WM_RD_ACK_RESPONSE

	// const EB_RemoteDesktopInfo* pRemoteDesktopInfo = (const EB_RemoteDesktopInfo*)wp;
	, EB_WM_RD_REQUEST
	, EB_WM_RD_ACCEPT
	, EB_WM_RD_REJECT
	, EB_WM_RD_CLOSE

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
	, EB_WM_SUBSCRIBE_FUNC_INFO	= WM_USER+0x501

};

} // namespace entboost

#endif // __EBCallbackInterface_h__
