// EBAppClient.h file here

#ifndef __EBAppClient_h__
#define __EBAppClient_h__

#ifdef _QT_MAKE_
#include <QObject>
#endif
#include <vector>
#include <string>
#include "../include/EBCallbackInterface.h"
#include "../include/chatroomobject.h"
//using namespace entboost;

#ifdef EB_APPCLIENT_EXPORTS
#define EB_APPCLIENT_API __declspec(dllexport)
#else
#define EB_APPCLIENT_API __declspec(dllimport)
#endif

namespace entboost {

mycp::tstring EB_APPCLIENT_API GetHostIp(const char* lpszHostName,const char* lpszDefaultName);
bool EB_APPCLIENT_API GetAddressPort(const char* sFullAddress, mycp::tstring& sOutAddress, int & nOutPort);
//bool EB_APPCLIENT_API GetAccountAddress(const char* sFullAccount, mycp::tstring& sOutAddress);
void EB_APPCLIENT_API GetLocalHostOAuthKey(mycp::tstring& pOutLocalHostOAuthKey);

//
class EB_APPCLIENT_API CEBAppClient
{
public:
	CEBAppClient(void);
	virtual ~CEBAppClient(void);

	/**** 备注说明 ***********************************************************************/
	// 1.int函数返回，0/1为调用成功，负数为函数调用失败；
	// 2.调用成功表示指令发送成功，业务执行是否成功，看回调函数处理事件的返回数据；
	/*************************************************************************************/

	/**** 应用（APP）环境 ***********************************************************************/
	/****
	功能：初始化应用环境
	参数：sAddress 服务器地址，格式1:ip:port 如122.122.122.122:1234
	参数：sInitParameter 初始化参数，默认为空
	====================================================================*/
	void EB_Init(const char * sAddress,const char* sInitParameter="");

	/****
	功能：判断是否已经初始化应用环境
	====================================================================*/
	bool EB_IsInited(void) const;

	/****
	功能：注册事件回调监听类
	参数：pCallBack 回调监听类，NULL 空为注销
	====================================================================*/
	void EB_SetCallBack(CEBCallbackInterface * pCallBack);

	/****
	功能：注册事件回调监听窗口句柄
	参数：pHwnd 回调监听窗口句柄，NULL 空为注销
	====================================================================*/
#ifdef _QT_MAKE_
	void EB_SetMsgReceiver(QObject* pReceiver);
#else
	void EB_SetMsgHwnd(HWND pHwnd);
#endif

	/****
	功能：注销应用环境
	====================================================================*/
	void EB_UnInit(void);

	/****
	功能：返回最后操作状态码
	====================================================================*/
	EB_STATE_CODE EB_GetLastStateCode(void) const;

	/****
	功能：返回服务器地址
	====================================================================*/
	mycp::tstring EB_GetAddress(void) const;

	/****
	功能：返回应用程序APP路径
	====================================================================*/
    EBFileString EB_GetAppPath(void) const;

	/****
	功能：返回资源文件存放路径
	====================================================================*/
    EBFileString EB_GetResourcePath(void) const;

	/****
	功能：返回应用程序APP数据路径
	====================================================================*/
    EBFileString EB_GetAppDataPath(void) const;

	/****
	功能：返回应用程序APP临时数据路径
	====================================================================*/
    EBFileString EB_GetAppDataTempPath(void) const;

	/****
	功能：检查最新版本
	描述：如果有新版本，自动下载更新包文件，下载成功会收到onNewVersion()或EB_WM_NEW_VERSION通知
	参数：sClientVersion 当前版本
	====================================================================*/
	int EB_CheckVersion(const char* sClientVersion);

	/****
	功能：获取地区信息
	参数：nParent 上一级编码
	参数：nParameter 事件通知参数
	====================================================================*/
	int EB_GetAreaInfo(int nParent,unsigned int nParameter = 0);


	/**** 开发者验证 ***********************************************************************/
	/****
	功能：设置开发者appid和appkey
	参数：sAppId 开发者应用ID
	参数：sAppKey 应用KEY
	参数：bReLoadAppOnlineKey true:验证appid和appkey，获取最新app online key
	====================================================================*/
	int EB_SetDevAppId(eb::bigint sAppId,const char* sAppKey,bool bReLoadAppOnlineKey=true);

	/****
	功能：设置开发者appid和app online key
	====================================================================*/
	void EB_SetDevAppOnlineKey(eb::bigint sAppId, const char* sAppOnlineKey);

	/****
	功能：获取开发者appid和applinekey
	====================================================================*/
	eb::bigint EB_GetDevAppId(void) const;
	mycp::tstring EB_GetDevAppOnlineKey(void) const;

	bool EB_SetSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long nParameterValue);
	void EB_GetSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long* pParameterValue) const;
	void EB_FreeSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long nParameterValue) const;

	/**** APP应用中心管理 ***********************************************************************/
	/****
	功能：登录APP应用中心
	====================================================================*/
	int EB_LogonAppCenter(void);

	/****
	功能：登出APP应用中心
	====================================================================*/
	void EB_LogoutAppCenter(void);

	/****
	功能：发送信息给指定ＡＰＰ应用
	====================================================================*/
	int EB_SendAPMsg(const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg);

	/****
	功能：响应收到ＡＰＰ应用信息
	====================================================================*/
	void EB_APMsgAck(eb::bigint nMsgId);

	/**** 注册、登录，个人信息 ***********************************************************************/
	/****
	功能：注册一个新帐号
	参数：sAccount 注册帐号，电子邮箱格式；类似user@entboost.com
	参数：sPwd 帐号密码
	参数：sUserName 用户名称
	参数：sEnterpriseName 企业名称，不为空，注册一个企业帐号
	====================================================================*/
	int EB_Register(const char* sAccount, const char* sPassword, const char* sUserName="", const char* sEnterpriseName="");

	/****
	功能：游客登录
	====================================================================*/
	int EB_LogonByVisitor(const char* sReqCode="");

	/****
	功能：用户登录
	====================================================================*/
    int EB_LogonByAccount(const char * sAccount, const char * sPassword, const char* sReqCode="",
                          const char * sOAuthKey="", EB_USER_LINE_STATE nNewLineState=EB_LINE_STATE_ONLINE_NEW,
                          EB_LOGON_TYPE logonType=EB_LOGON_TYPE_PC);

	/****
	功能：OAUTH验证
	参数：sOAuthKey 用于自动登录
	====================================================================*/
	int EB_LogonOAuth(eb::bigint nUserId, const char * sOAuthKey="",EB_USER_LINE_STATE nNewLineState=EB_LINE_STATE_ONLINE_NEW);
	//int EB_LogonOAuth(const char * sAccount="",EB_USER_LINE_STATE nNewLineState=EB_LINE_STATE_ONLINE_NEW);

	/****
	功能：重新登录，主要用于电脑挂起，或网络断开重启时，直接重新登录
	====================================================================*/
	int EB_ReLogon(void);

	/****
	功能：返回是否登录成功
	====================================================================*/
	bool EB_IsLogoned(void) const;

	/****
	功能：返回是否游客
	====================================================================*/
	bool EB_IsLogonVisitor(void) const;

	/****
	功能：用户退出
	====================================================================*/
	void EB_Logout(void);

	/**** 基本信息管理 ***********************************************************************/
	/****
	功能：登录成功后，加载离线信息
	描述：先调用EB_LoadEnterprise()加载企业组织结构，个人群组和EB_LoadContact()加载通讯录，后再调用EB_LoadInfo()
	====================================================================*/
	void EB_LoadInfo(void);

	/******
	功能：返回登录帐号信息
	====================================================================*/
	bool EB_GetMyAccountInfo(EB_AccountInfo* pOutAccountInfo) const;

	/******
	功能：设置登录帐号个人信息
	====================================================================*/
	bool EB_SetMyAccountInfo(const EB_AccountInfo* pInAccountInfo);

	/******
	功能：获取默认群组（部门）成员编号
	====================================================================*/
	bool EB_GetMyDefaultMemberCode(eb::bigint& pOutDefaultMemberCode) const;

	/******
	功能：设置默认群组（部门）成员编号
	描述：默认群组（部门）成员编号，用于在外部呼叫中自动显示电子名片；
	====================================================================*/
	bool EB_SetMyDefaultMemberCode(eb::bigint nNewDefaultMemberCode);

	/******
	功能：获取默认群组（部门）成员信息
	====================================================================*/
	bool EB_GetMyDefaultMemberInfo(EB_MemberInfo* pOutMemberInfo) const;

	/******
	功能：获取默认群组（部门）成员头像文件
	====================================================================*/
    EBFileString EB_GetMyDefaultMemberHeadFile(void) const;

	/******
	功能：返回个人设置
	====================================================================*/
	int EB_GetMyAccountSetting(void) const;

	/******
	功能：更新个人设置
	====================================================================*/
	bool EB_SetMyAccountSetting(int nNewSetting);

	/****
	功能：返回登录帐号
	====================================================================*/
	mycp::tstring EB_GetLogonAccount(void) const;
	eb::bigint EB_GetLogonUserId(void) const;

	/****
	功能：返回帐号备注
	====================================================================*/
	mycp::tstring EB_GetDescription(void) const;

	/****
	功能：更新帐号备注
	====================================================================*/
	bool EB_SetDescription(const char* sNewDescription);

	/****
	功能：返回我的在线状态
	====================================================================*/
	EB_USER_LINE_STATE EB_GetLineState(void)const;

	/****
	功能：更改在线状态
	描述：所有在线群组成员（包括自己）会收到onUserStateChange()或EB_WM_USER_STATE_CHANGE通知
	====================================================================*/
	bool EB_SetLineState(EB_USER_LINE_STATE nNewLineState);

	/****
	功能：返回帐号名称和密码
	描述：帐号名称为空，名称=帐号
	====================================================================*/
	//void EB_GetLogonInfo(mycp::tstring& sOutUserName, mycp::tstring& sOutPassword) const;

	/****
	功能：返回帐号用户名称
	====================================================================*/
	mycp::tstring EB_GetUserName(void) const;

	/****
	功能：修改帐号用户名称
	====================================================================*/
	bool EB_SetUserName(const char* sNewUserName);


	/****
	功能：返回帐号密码
	====================================================================*/
	mycp::tstring EB_GetPassword(void) const;

	/****
	功能：修改密码
	====================================================================*/
	bool EB_SetPassword(const char* sOldPassword,const char* sNewPassword);

	/**** 权限管理 ***********************************************************************/
	/******
	功能：判断是否有编辑企业资料权限
	参数：sEnterpriseCode 企业代码，空为判断默认企业
	====================================================================*/
	bool EB_CanEditEnterprise(eb::bigint nEnterpriseCode=0) const;

	/******
	功能：判断是否有编辑群组（部门）资料权限
	参数：sEnterpriseCode 企业代码，空为判断默认企业
	参数：sGroupCode 群组代码，空为判断是否新建群组权限
	====================================================================*/
	bool EB_CanEditGroupInfo(eb::bigint nEnterpriseCode,eb::bigint nGroupId) const;

	/******
	功能：判断是否有删除群组（部门）资料权限
	参数：sGroupCode 群组代码
	====================================================================*/
	bool EB_CanDeleteGroupInfo(eb::bigint nGroupId) const;
	bool EB_CanDeleteGroupInfo(eb::bigint nGroupId, eb::bigint nMemberUserId) const;

	bool EB_IsGroupCreator(eb::bigint nGroupId) const;
	bool EB_IsGroupCreator(eb::bigint nGroupId, eb::bigint nMemberUserId) const;
	bool EB_IsGroupManager(eb::bigint nGroupId) const;
	bool EB_IsGroupManager(eb::bigint nGroupId, eb::bigint nMemberUserId) const;

	/******
	功能：判断是否有编辑群组（部门）成员资料权限
	参数：sGroupCode 群组代码
	参数：sMemberAccount 成员帐号
	====================================================================*/
	bool EB_CanEditMemberInfo(eb::bigint nGroupId,eb::bigint nMemberUserId) const;

	/******
	功能：判断是否有编辑我的群组（部门）基础成员资料权限
	参数：sGroupCode 群组代码
	====================================================================*/
	bool EB_CanEditMyBaseMemberInfo(eb::bigint nGroupId) const;

	bool EB_CanEditMyBaseAccountInfo(void) const;

	/******
	功能：判断是否有删除群组（部门）成员资料权限
	参数：sGroupCode 群组代码
	参数：sMemberAccount 成员帐号
	====================================================================*/
	bool EB_CanDeleteMemberInfo(eb::bigint nGroupId,eb::bigint nMemberUserId) const;

	/******
	功能：判断是否有删除群组（部门）成员资料权限
	参数：sMemberCode 群组成员代码
	====================================================================*/
	bool EB_CanDeleteMemberInfo(eb::bigint nMemberId) const;

	/******
	功能：判断是否有编辑（新建）群组（部门）云盘资源权限
	====================================================================*/
	bool EB_CanEditGroupRes(eb::bigint nGroupId, eb::bigint nResourceId) const;

	/******
	功能：判断是否有删除群组（部门）云盘资源权限
	====================================================================*/
	bool EB_CanDeleteGroupRes(eb::bigint nGroupId, eb::bigint nResourceId) const;

	/* 在线云盘管理 ***********************************************************************/
	/****
	功能：加载云盘资源（向服务器请求）
	参数：sGroupCode 群组（部门）代码；不为空，加载群组资源；空为加载个人云盘资源
	====================================================================*/
	int EB_LoadRes(eb::bigint nGroupId=0);

	/****
	功能：获取目录下所有云资源
	====================================================================*/
	void EB_GetDirAllRes(eb::bigint sParentResId,std::vector<EB_ResourceInfo>& pOutResourceList) const;

	/****
	功能：获取没有目录所有云资源
	参数：sGroupCode 群组（部门）代码；不为空，获取群组资源；空为获取个人云盘资源
	====================================================================*/
	void EB_GetNotDirAllRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId=0) const;

	/****
	功能：判断目录资源下，是否有资源信息
	====================================================================*/
	bool EB_HasSubRes(eb::bigint sParentResId) const;

	/****
	功能：获取所有云资源
	参数：sGroupCode 群组（部门）代码；不为空，获取群组资源；空为获取个人云盘资源
	====================================================================*/
	void EB_GetAllRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId=0) const;

	/****
	功能：获取所有云文件资源
	参数：sGroupCode 群组（部门）代码；不为空，获取群组资源；空为获取个人云盘资源
	====================================================================*/
	void EB_GetAllFileRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId=0) const;

	/****
	功能：获取所有云笔记资源
	参数：sGroupCode 群组（部门）代码；不为空，获取群组资源；空为获取个人云盘资源
	====================================================================*/
	void EB_GetAllNoteRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId=0) const;

	/****
	功能：增加目录资源
	参数：sGroupCode 群组（部门）代码；不为空，增加群组资源；空为增加个人云盘资源
	====================================================================*/
	int EB_AddDirRes(const char* sDirName,eb::bigint sParentResId=0,eb::bigint nGroupId=0);

	/****
	功能：增加文件资源
	参数：sGroupCode 群组（部门）代码；不为空，增加群组资源；空为增加个人云盘资源
	====================================================================*/
	int EB_AddFileRes(const char* sFilePath,const char* sFileName,const char* sDescription,eb::bigint sParentResId=0,eb::bigint nGroupId=0);

	/****
	功能：增加临时群组文件资源
	参数：nGroupId 群组（部门）代码；
	====================================================================*/
	int EB_AddTempGroupFileRes(const char* sFilePath,const char* sFileName,eb::bigint nGroupId);

	/****
	功能：增加在线笔记资源
	参数：sGroupCode 群组（部门）代码；不为空，增加群组资源；空为增加个人云盘资源
	====================================================================*/
	int EB_AddNoteRes(const char* sNoteName,const char* sDescription,eb::bigint sParentResId=0,eb::bigint nGroupId=0);

	/****
	功能：修改在线资源
	====================================================================*/
	int EB_EditRes(eb::bigint sResId,const char* sName,const char* sDescription);

	/****
	功能：移动资源到指定目录下
	====================================================================*/
	int EB_MoveRes2Dir(eb::bigint sResId,eb::bigint sParentResId);

	/****
	功能：删除在线资源
	====================================================================*/
	int EB_DeleteRes(eb::bigint sResId);

	/****
	功能：获取资源对象信息
	====================================================================*/
	bool EB_GetResourceInfo(eb::bigint sResId,EB_ResourceInfo* pOutResourceInfo);

	/****
	功能：获取资源对象信息
	====================================================================*/
	bool EB_GetResourceCmInfo(eb::bigint sResId,mycp::tstring& pOutResourceInfo);


	/****
	功能：下载云盘资源文件
	====================================================================*/
    int EB_DownloadFileRes(eb::bigint sResId, const EBFileString &sSaveTo);
    int EB_DownloadFileRes(eb::bigint sResId, const char *sSaveTo);

	/****
	功能：取消上传（或下载）云盘资源文件
	====================================================================*/
	int EB_CancelFileRes(eb::bigint sResId,eb::bigint nMsgId);

	//bool UpdateInfo(const CPopParameterList& pInfoList);
	//bool EnterChat(eb::bigint nCallId, const char* sAccount);	// 记录在线状况
	//bool ExitChat(eb::bigint nCallId, const char* sAccount);	// 记录在线状况

	/* 会话管理 ***********************************************************************/
	/****
	功能：呼叫群组成员
	参数：sMemberCode 呼叫用户帐号，群组（部门）成员编号
	参数：sExistCallId 不为空，表示邀请用户进多人会话
	====================================================================*/
	int EB_CallMember(eb::bigint nMemberId,eb::bigint nExistCallId=0);

	/****
	功能：呼叫用户
	参数：sToAccount 呼叫用户帐号
	参数：sExistCallId 不为空，表示邀请用户进多人会话
	====================================================================*/
	int EB_CallAccount(const char* sToAccount,eb::bigint nExistCallId=0);
	int EB_CallUserId(eb::bigint nToUserId,eb::bigint nExistCallId=0);

	/****
	功能：会话邀请成员，转临时讨论组
	参数：sCallId 会话ID
	参数：sToAccount 不为空，邀请第三方用户
	参数：sMemberCode 邀请第三方用户，群组（部门）成员编号
	====================================================================*/
	int EB_Call2Group(eb::bigint nCallId,const char* sToAccount);

	/****
	功能：发起群组（部门）会话
	参数：sGroupCode 群组（部门）编号
	====================================================================*/
	int EB_CallGroup(eb::bigint nGroupId);

	/****
	功能：应答一个会话请求
	参数：bAccept true:接受会话；false:拒绝会话
	====================================================================*/
	int EB_CallAnswer(eb::bigint nCallId,bool bAccept);

	/****
	功能：挂断会话
	描述：对方会收到挂断会话事件，如果不想挂断会话，使用EB_CallExit
	====================================================================*/
	int EB_CallHangup(eb::bigint nCallId);

	/****
	功能：退出会话
	描述：用于上层退出聊天窗口，清空会话
	====================================================================*/
	void EB_CallExit(eb::bigint nCallId);

	/****
	功能：发送会话通知
	描述：nNotifyType=1: 用户正在输入
	====================================================================*/
	void EB_CallNotify(eb::bigint nCallId, eb::bigint nToUserId, int nNotifyType, eb::bigint nNotifyId, const char* sNotifyData);

	/****
	功能：检查会话成员，成员离线，重新呼叫
	返回值：0=正常会话；1=正在重新呼叫离线成员；-1=会话不存在等其他错误
	====================================================================*/
	//int EB_CheckReCall(eb::bigint nCallId,bool bOffReCall);

	/****
	功能：会话聊天RICH格式缓存添加一个文本
	====================================================================*/
	void EB_RichBufferAddText(eb::bigint nCallId,const char* sText);

	/****
	功能：会话聊天RICH格式缓存添加一个图片对象（如JPG）
	====================================================================*/
	void EB_RichBufferAddObject(eb::bigint nCallId,const char* pData,unsigned long nSize);

	/****
	功能：会话聊天RICH格式缓存添加一个资源对象（如表情）
	====================================================================*/
	void EB_RichBufferAddResource(eb::bigint nCallId,const char* sResource);

	/****
	功能：清空会话聊天RICH格式缓存
	====================================================================*/
	void EB_RichBufferClear(eb::bigint nCallId);

	/****
	功能：发送会话聊天RICH格式缓存内容，发送完成自动清空缓存
	参数：sToAccount 发给会话中指定用户，（用于群组会话中）
	参数：bPrivate 悄悄话功能 true:以私聊方式发给指定用户，会话其他人看不到 false:会话其他成员也可以看到
	====================================================================*/
	int EB_SendRichBuffer(eb::bigint nCallId,eb::bigint nToUserId=0,bool bPrivate=false);

	/****
	功能：发送文本消息
	参数：sTextMsg 文本消息
	参数：sToAccount 发给会话中指定用户，（用于群组会话中）
	参数：bPrivate 悄悄话功能 true:以私聊方式发给指定用户，会话其他人看不到 false:会话其他成员也可以看到
	====================================================================*/
	int EB_SendText(eb::bigint nCallId,const char* sTextMsg,eb::bigint nToUserId=0,bool bPrivate=false);

	/****
	功能：发送RICH消息
	参数：pRichMsg RICH富格式，包括TEXT和JPG图片
	参数：sToAccount 发给会话中指定用户，（用于群组会话中）
	参数：bPrivate 悄悄话功能 true:以私聊方式发给指定用户，会话其他人看不到 false:会话其他成员也可以看到
	====================================================================*/
	int EB_SendRich(eb::bigint nCallId,const EB_ChatRoomRichMsg* pRichMsg,eb::bigint nToUserId=0,bool bPrivate=false);

	/****
	功能：发送用户名片
	参数：sCardUserAccount/nCardUserId 要发送名片用户帐号/用户ID
	参数：nToUserId 发给会话中指定用户，（用于群组会话中）
	参数：bPrivate 悄悄话功能 true:以私聊方式发给指定用户，会话其他人看不到 false:会话其他成员也可以看到
	====================================================================*/
	int EB_SendUserCard(eb::bigint nCallId,const tstring& sCardUserAccount,eb::bigint nToUserId=0,bool bPrivate=false);
	int EB_SendUserCard(eb::bigint nCallId,eb::bigint nCardUserId,eb::bigint nToUserId=0,bool bPrivate=false);

	/****
	功能：解析名片信息
	参数：[in] sInCardInfoString 要解析名片信息字符串
	参数：[out] pOutCartType 名片类型；1=用户名片
	参数：[out] pOutCardData 名片数据；配合 pOutCartType 使用，如果1=用户名片，调用 EB_ParseUserECard 解析用户名片
	====================================================================*/
	bool EB_ParseCardInfo(const tstring& sInCardInfoString,int& pOutCardType,tstring& pOutCardData);
	
	/****
	功能：解析用户电子名片
	参数：[in] sInUserECardString 要解析用户名片字符串
	参数：[out] pOutUserECard 用户电子名片类
	====================================================================*/
	bool EB_GetUserECardByFromInfo(const tstring& sInUserECardString, EB_ECardInfo* pOutUserECard);
	bool EB_GetUserECardByCardInfo(const tstring& sInUserECardString, EB_ECardInfo* pOutUserECard);
	
	//int EB_SendCard(eb::bigint nCallId,int nCardType,const char* sCardData,eb::bigint nToUserId=0,bool bPrivate=false);
	int EB_SendMapPos(eb::bigint nCallId,const char* sMapPosData,eb::bigint nToUserId=0,bool bPrivate=false);
	int EB_SendUserData(eb::bigint nCallId,const char* sUserData,unsigned long nDataSize,eb::bigint nToUserId=0,bool bPrivate=false);

	/****
	功能：请求撤回消息（2分钟内有效），不支持P2P点对点文件消息
	====================================================================*/
    int EB_RequestWithdrawMsg(eb::bigint nCallId,eb::bigint nMsgId);

	/****
	功能：请求收藏消息，不支持已经删除漫游消息
	参数：nMsgId 收藏消息ID，如果消息已经在漫游消息被删除，收藏失败；
	参数：bGroupCollection 是否“群收藏”；true=群收藏 false=个人收藏
	====================================================================*/
	int EB_RequestCollectMsg(eb::bigint nCallId,eb::bigint nMsgId, bool bGroupCollection);

	/****
	功能：发送文件
	参数：sFilePath 发送文件路径
	参数：sToAccount 发给会话中指定用户，（用于群组会话中）
	参数：bPrivate 悄悄话功能 true:以私聊方式发给指定用户，会话其他人看不到 false:会话其他成员也可以看到
	参数：bOffFile 保存离线文件 false/true
	====================================================================*/
    int EB_SendFile(eb::bigint nCallId,const EBFileString &sFilePath,eb::bigint nToUserId=0,bool bPrivate=false,bool bOffFile=false);
    int EB_SendFile(eb::bigint nCallId,const char* sFilePath,eb::bigint nToUserId=0,bool bPrivate=false,bool bOffFile=false);

	/****
	功能：接收文件
	参数：nMsgId 文件消息编号
	参数：sSaveTo 保存文件
	====================================================================*/
    int EB_AcceptFile(eb::bigint nCallId, eb::bigint nMsgId, const EBFileString &sSaveTo);
    int EB_AcceptFile(eb::bigint nCallId, eb::bigint nMsgId, const char *sSaveTo);

	/****
	功能：保存离线文件到我的云盘
	参数：nMsgId 文件消息编号
	====================================================================*/
	int EB_Save2CloudDrive(eb::bigint nCallId,eb::bigint nMsgId);

	/****
	功能：接收离线文件（资源文件）
	参数：sResId 离线文件资源编号
	参数：sSaveTo 保存路径
	====================================================================*/
	//int EB_AcceptFileRes(eb::bigint nCallId,const char * sResId,const char * sSaveTo);

	/****
	功能：取消或拒绝接收文件
	参数：nMsgId 文件消息编号
	====================================================================*/
	int EB_CancelFile(eb::bigint nCallId,eb::bigint nMsgId);

	/****
	功能：获取会话成员信息
	====================================================================*/
	bool EB_GetCallAccountInfoList(eb::bigint nCallId,std::vector<EB_AccountInfo>& pOutUserList) const;
	bool EB_GetCallUserIdList(eb::bigint nCallId,std::vector<eb::bigint>& pOutUserList) const;
	bool EB_GetCallAccountList(eb::bigint nCallId,std::vector<mycp::tstring>& pOutUserList) const;

	/****
	功能：获取会话当前在线用户数量
	====================================================================*/
	//int EB_GetCallOnlineSize(eb::bigint nCallId) const;

	/****
	功能：获取会话指定用户信息
	====================================================================*/
	bool EB_GetCallAccountInfo(eb::bigint nCallId,eb::bigint nUserId,EB_AccountInfo* pOutAccountInfo) const;

	/* 通讯录（联系人） ***********************************************************************/
	/****
	功能：加载我的在线通讯录（联系人），包括所有分组信息
	描述：登录成功后，调用加载一次
	====================================================================*/
	int EB_LoadContact(void);

	/****
	功能：修改（或新建）一条用户分组信息
	参数：nUGId 用户分组ID，0表示新建
	参数：sGroupName 分级名称，不能为空
	====================================================================*/
	int EB_EditUGInfo(eb::bigint nUGId, const char* sGroupName);

	/****
	功能：删除一条用户分组信息
	====================================================================*/
	int EB_DeleteUGInfo(eb::bigint nUGId);

	/****
	功能：删除一条用户分组信息
	====================================================================*/
	int EB_GetUGContactSize(eb::bigint nUGId, int& pOutContactSize, int& pOutOnlineSize);

	/****
	功能：获取联系人信息列表
	====================================================================*/
	void EB_GetUGInfoList(std::vector<EB_UGInfo>& pOutUGInfoList) const;

	/****
	功能：申请加了好友
	====================================================================*/
	int EB_ReqAddContact(eb::bigint nContactUserId,const char * sDescription);

	/****
	功能：修改（或新建）一条联系人资料
	====================================================================*/
	int EB_EditContact(const EB_ContactInfo* pContactInfo);

    /****
    功能：移动分组
    ====================================================================*/
    int EB_MoveContactTo1(eb::bigint contactId, eb::bigint ugId);
    int EB_MoveContactTo2(eb::bigint userId, eb::bigint ugId);

	/****
	功能：删除一条联系人资料
	====================================================================*/
	int EB_DeleteContact(eb::bigint nContactId,bool bDeleteDest);
	//int EB_DeleteContact(const char* sContactAccount);

	/****
	功能：获取联系人信息列表
	====================================================================*/
	void EB_GetContactList(std::vector<EB_ContactInfo>& pOutContactList) const;

	/****
	功能：获取联系人信息
	====================================================================*/
	bool EB_GetContactInfo1(eb::bigint nContactId,EB_ContactInfo* pOutContactInfo) const;
	bool EB_GetContactInfo2(eb::bigint nContactUserId,EB_ContactInfo* pOutContactInfo) const;
	//bool EB_GetContactInfo(const char* sContactAccount,EB_ContactInfo* pOutContactInfo) const;

	/****
	功能：获取联系人头像信息
	====================================================================*/
    bool EB_GetContactHeadInfoByContactId(eb::bigint nContactId,EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;
    bool EB_GetContactHeadInfoByUserId(eb::bigint nUserId,EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;

	/****
	功能：判断是否我的联系人帐号
	====================================================================*/
	bool EB_IsMyContactAccount1(eb::bigint nContactId) const;
	bool EB_IsMyContactAccount2(eb::bigint nContactUserId) const;
	//bool EB_IsMyContactAccount(const char* sContactAccount) const;

	/* 企业组织结构（个人群组信息） ***********************************************************************/
	/****
	功能：加载在线所有企业组织结构和个人群组
	描述：登录成功后，调用加载一次
	====================================================================*/
	int EB_LoadOrg(void);

	/****
	功能：加载在线指定群组（部门）信息
	参数：bLoadMember true:同时加载成员信息
	====================================================================*/
	int EB_LoadGroup(eb::bigint nGroupId,bool bLoadMember);

	/****
	功能：获取企业信息
	参数：sEnterpriseCode 企业代码；空为当前默认企业代码
    ====================================================================*/
    bool EB_GetEnterpriseInfo(EB_EnterpriseInfo* pOutEnterpriseInfo,eb::bigint nEnterpriseCode=0) const;

	bool EB_IsEnterpriseUser(eb::bigint nEnterpriseCode=0) const;

	/****
	功能：获取企业名称
	参数：sEnterpriseCode为空，获取当前默认企业名称
	====================================================================*/
	bool EB_GetEnterpriseName(mycp::tstring& pOutEnterpriseName,eb::bigint nEnterpriseCode=0) const;

	/****
	功能：获取企业在线用户信息
	参数：sEnterpriseCode：企业代码
	====================================================================*/
	void EB_GetEnterpriseMemberSize(eb::bigint nEnterpriseCode,int& pOutMemberSize,int& pOutOnlineSize) const;

	/****
	功能：修改企业资料
	====================================================================*/
	int EB_EditEnterprise(const EB_EnterpriseInfo* pEnterpriseInfo);

	/****
	功能：修改或新建群组（部门）资料信息
	描述：pGroupInfo->m_sGroupCode为空，表示新建
	====================================================================*/
	int EB_EditGroup(const EB_GroupInfo* pGroupInfo);

	bool EB_IsGroupForbidSpeech(eb::bigint nGroupId) const;	// 是否禁言群组
	int EB_SetGroupForbidSpeech(eb::bigint nGroupId, bool bForbidSpeech);	// 是否禁言群组

	/****
	功能：删除部门或解散群组
	====================================================================*/
	int EB_DeleteGroup(eb::bigint nGroupId);

	/****
	功能：申请加入群组（部门）
	====================================================================*/
	int EB_JoinGroup(eb::bigint nGroupId,const char* sDescription);

	/****
	功能：退出群组（部门）
	====================================================================*/
	int EB_ExitGroup(eb::bigint nGroupId);

	/****
	功能：设置个人群组（部门）头像
	描述：<= 1Mb
	====================================================================*/
    int EB_SetMyGroupHeadFile(eb::bigint nGroupId, const EBFileString &sImagePath);
    int EB_SetMyGroupHeadFile(eb::bigint nGroupId, const char* sImagePath);

	/****
	功能：设置个人群组（部门）头像
	====================================================================*/
	int EB_SetMyGroupHeadRes(eb::bigint nGroupId,eb::bigint sResId);

	/****
	功能：获取群组（部门）信息
	====================================================================*/
	bool EB_GetGroupInfo(eb::bigint nGroupId,EB_GroupInfo* pOutGroupInfo) const;

	/****
	功能：获取群组（部门）名称
	====================================================================*/
	bool EB_GetGroupName(eb::bigint nGroupId,mycp::tstring& pOutGroupName) const;

	/****
	功能：获取群组（部门）创建者
	====================================================================*/
	bool EB_GetGroupCreator(eb::bigint nGroupId,eb::bigint& pOutGroupCreator) const;

	/****
	功能：获取群组（部门）类型
	====================================================================*/
	bool EB_GetGroupType(eb::bigint nGroupId,EB_GROUP_TYPE& pOutGroupType) const;

	/****
	功能：获取群组（部门）成员信息
	====================================================================*/
	bool EB_GetGroupMemberInfoList(eb::bigint nGroupId,std::vector<EB_MemberInfo>& pOutMemberInfoList) const;

	/****
	功能：获取群组（部门）成员帐号
	====================================================================*/
	bool EB_GetGroupMemberUserIdList(eb::bigint nGroupId,std::vector<eb::bigint>& pOutMemberUserIdList) const;
	bool EB_GetGroupMemberCodeList(eb::bigint nGroupId,std::vector<eb::bigint>& pOutMemberCodeList) const;
	bool EB_GetGroupMemberAccountList(eb::bigint nGroupId,std::vector<mycp::tstring>& pOutMemberAccountList) const;

	/****
	功能：获取群组（部门）成员数量
	参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	====================================================================*/
	int EB_GetGroupMemberSize(eb::bigint nGroupId, int nGetType) const;
	bool EB_GetGroupMemberSize(eb::bigint nGroupId, int nGetType, int& pOutMemberSize, int& pOutOnlineSize) const;

	/****
	功能：获取群组（部门）在线成员数量
	参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	====================================================================*/
	int EB_GetGroupOnlineSize(eb::bigint nGroupId, int nGetType) const;

	/****
	功能：判断是否是我的群组（部门）
	参数：sGroupCode 群组（部门）代码
    ====================================================================*/
    bool EB_IsMyGroup(eb::bigint nGroupId) const;

    /****
    功能：判断是否有子部门
    参数：sGroupCode 群组（部门）代码
    ====================================================================*/
    bool EB_HasSubGroup(eb::bigint nGroupId) const;

	/****
	功能：修改或新建群组（部门）成员信息，主要用于添加部门员工或修改员工资料
	描述：pMemberInfo->m_sMemberCode成员编号为空，表示新建
	====================================================================*/
	int EB_EditMember(const EB_MemberInfo* pMemberInfo);

	bool EB_IsGroupAdminLevel(eb::bigint nGroupId) const;	// 是否群组管理员权限
	bool EB_IsGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId) const;	// 是否群组管理员权限
	int EB_AddGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId);	// 增加群组管理员权限
	int EB_DelGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId);	// 删除群组管理员权限

	bool EB_IsMemberForbidSpeech(eb::bigint nGroupId, eb::bigint nMemberUserId,int& pOutForbidMinutes);	// 判断群组成员是否禁言
	/****
	功能：设置或解除群组（部门）成员禁言
	描述：
	参数：bForbidSpeech : 禁言或解除禁言 true=禁言 false=解除禁言
	参数：nForbidMinutes: 禁言分钟数 -1=不处理 0=配合bForbidSpeech=true永久禁言 大于0=配合bForbidSpeech=true禁言多少分钟
	====================================================================*/
	int EB_SetMemberForbidSpeech(eb::bigint nGroupId, eb::bigint nMemberUserId, bool bForbidSpeech, int nForbidMinutes=0);	// 设置群组成员禁言

	/****
	功能：删除群组（部门）成员
	====================================================================*/
	int EB_DeleteMember(eb::bigint nMemberId, bool bDeleteAccount=true);

	/****
	功能：获取群组（部门）成员信息
	====================================================================*/
	bool EB_GetMemberInfoByUserId(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId,eb::bigint nMemberUserId) const;
	bool EB_GetMemberInfoByAccount(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId,const char* sMemberAccount) const;

	/****
	功能：获取群组（部门）成员信息
	====================================================================*/
	bool EB_GetMemberInfoByUserId2(EB_MemberInfo* pOutMemberInfo,eb::bigint nMemberUserId) const;
	bool EB_GetMemberInfoByUserId2(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,eb::bigint nMemberUserId) const;
	bool EB_GetMemberInfoByAccount2(EB_MemberInfo* pOutMemberInfo,const char* sMemberAccount) const;
	bool EB_GetMemberInfoByAccount2(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,const char* sMemberAccount) const;

    /****
    功能：判断成员是否存在（不存在不可能是本地没有缓存）
    ====================================================================*/
    bool EB_IsExistLocalMemberInfo(eb::bigint nMemberId) const;

	/****
	功能：获取群组（部门）成员信息
	====================================================================*/
	bool EB_GetMemberInfoByMemberCode(EB_MemberInfo* pOutMemberInfo,eb::bigint nMemberId) const;
	bool EB_GetMemberInfoByMemberCode(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,eb::bigint nMemberId) const;

	/****
	功能：获取群组（部门）成员头像信息
	====================================================================*/
    bool EB_GetMemberHeadInfoByMemberCode(eb::bigint nMemberId, EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;
    bool EB_GetMemberHeadInfoByUserId(eb::bigint nGroupId,eb::bigint nMemberUserId,EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;
    bool EB_GetMemberHeadInfoByUserId(eb::bigint nMemberUserId,EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;
    bool EB_GetMemberHeadInfoByAccount(const char* sMemberAccount,EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const;
	/****
	功能：获取群组（部门）成员名称
	====================================================================*/
	bool EB_GetMemberNameByUserId(eb::bigint nGroupId,eb::bigint nMemberUserId,mycp::tstring& pOutMemberName) const;
    bool EB_GetMemberNameByUserId2(eb::bigint nMemberUserId,mycp::tstring& pOutMemberName) const;

	/****
	功能：获取群组（部门）成员名称
	====================================================================*/
	bool EB_GetMemberNameByMemberCode(eb::bigint nMemberId,mycp::tstring& pOutMemberName) const;

	/****
	功能：获取群组（部门）成员在线状态
	====================================================================*/
	bool EB_GetMemberLineState(eb::bigint nMemberId,EB_USER_LINE_STATE& pOutLineState) const;

	/****
	功能：获取群组（部门）头像文件
	====================================================================*/
    bool EB_GetMemberHeadFile(eb::bigint nMemberId,eb::bigint& pOutResourceId, EBFileString &pOutHeadPath,mycp::tstring& pOutFileMd5);
    bool EB_GetMemberHeadFile(eb::bigint nGroupId,eb::bigint nUserId,eb::bigint& pOutResourceId, EBFileString &pOutHeadPath,mycp::tstring& pOutFileMd5);

	/****
	功能：获取我的群组（部门）成员信息
	参数：sGroupCode 群代码为空，获取默认群组成员信息
	====================================================================*/
	bool EB_GetMyMemberInfo(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId=0) const;

	/****
	功能：获取我的所有群组（部门）成员信息
	描述：包括企业部门员工信息和个人群组成员信息
	====================================================================*/
	void EB_GetMyMemberList(std::vector<EB_MemberInfo>& pOutMemberInfoList) const;

	/****
	功能：判断是否存在群组（部门）成员
	参数：sGroupCode 群组（部门）代码
	参数：sMemberAccount 成员帐号
	====================================================================*/
	bool EB_IsExistMemberByUserId(eb::bigint nGroupId,eb::bigint nMemberUserId) const;
	bool EB_IsExistMemberByUserId(eb::bigint nMemberUserId) const;
	bool EB_IsExistMemberByAccount(eb::bigint nGroupId,const char* sMemberAccount) const;
	bool EB_IsExistMemberByAccount(const char* sMemberAccount) const;

	/****
	功能：判断是否存在群组（部门）成员
	参数：sMemberCode 成员代码
	====================================================================*/
	bool EB_IsExistMemberByMemberCode(eb::bigint nMemberId) const;

	/****
	功能：获取表情库信息
	====================================================================*/
	bool EB_GetMyEmotionList(std::vector<EB_EmotionInfo>& pOutEmotionList) const;

	/****
	功能：获取默认头像信息
	====================================================================*/
	bool EB_GetDefaultHeadList(std::vector<EB_EmotionInfo>& pOutEmotionList) const;

	/**** 搜索查找 ***********************************************************************/
	/****
	功能：查找企业所有部门信息（组织结构）
	参数：sEnterpriseCode为空，查找默认企业
	参数：dwParam 由CEBSearchCallback回调函数返回；
	====================================================================*/
	void EB_FindAllGroupInfo(CEBSearchCallback * pCallback,eb::bigint nEnterpriseCode=0,unsigned long dwParam=0);

	/****
	功能：查找所有通讯录（联系人）信息
	参数：dwParam 由CEBSearchCallback回调函数返回；
	====================================================================*/
	void EB_FindAllContactInfo(CEBSearchCallback * pCallback,unsigned long dwParam=0);

	/****
	功能：搜索用户，包括企业部门员工，个人群组成员和通讯录用户
	参数：sSearchKey 搜索关键词，""空搜索所有
	参数：dwParam 由CEBSearchCallback回调函数返回；
	====================================================================*/
	void EB_SearchUserInfo(CEBSearchCallback * pCallback,const char* sSearchKey,unsigned long dwParam=0);

	/**** 远程桌面（远程协助） ***********************************************************************/
	/****
	功能：请求会话远程桌面（远程协助）
	====================================================================*/
	int EB_RDRequest(eb::bigint nCallId,EB_REMOTE_DESKTOP_TYPE nRDType=EB_RD_DESKTOP_SRC);

	/****
	功能： 接受（加入）或拒绝远程桌面（远程协助）
	参数：bAccept true:接受远程桌面 false:拒绝远程桌面
	====================================================================*/
	int EB_RDAck(eb::bigint nCallId,bool bAccept,eb::bigint nToUserId);

	/****
	功能： 结束或退出远程桌面
	====================================================================*/
	int EB_RDEnd(eb::bigint nCallId);

#ifdef _QT_MAKE_
	void EB_SetDesktopReceiver(eb::bigint nCallId,QObject* pReceiver);
#else
	void EB_SetDesktopHwnd(eb::bigint nCallId,HWND pHwnd);
#endif

	// quant : 1-20; default 12
	// fps : 0-10; 0=disable; default 2
	int EB_OpenLocalDesktop(eb::bigint nCallId, int quant=12, int fps = 2);
	// eb::bigint nUserId : 0 disable
	void EB_EnableControlLocalDesktop(eb::bigint nCallId, eb::bigint nUserId);
	eb::bigint EB_EnableControlLocalDesktop(eb::bigint nCallId) const;
	void EB_CloseLocalDesktop(eb::bigint nCallId);

#ifdef _QT_MAKE_
	bool EB_OpenRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, QObject* pParent);
#else
	bool EB_OpenRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, HWND hWndParent);
#endif
	// int nMouseMoveFPS: 0: disable
	bool EB_ControlRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, int nMouseMoveFPS = 3);
	int EB_ControlRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId) const;
	void EB_MoveRemoteDesktopWindow(eb::bigint nCallId, eb::bigint nUserId, int x, int y, int width, int height, bool bRepaint);
	void EB_CloseRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId);

	void EB_CloseAllDesktop(eb::bigint nCallId);

	bool EB_GetUserRDInfo(eb::bigint nCallId,std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;

	/**** 视频通讯 ***********************************************************************/
	/****
	功能：获取本地视频设备名称列表
	====================================================================*/
#ifdef _QT_MAKE_
    static void EB_GetVideoDeviceList(std::vector<QString>& pOutVideoDeviceList);
#else
    static void EB_GetVideoDeviceList(std::vector<mycp::tstring>& pOutVideoDeviceList);
#endif
	/****
	功能：获取默认视频媒体类型
	====================================================================*/
	//static int EB_GetDefaultVideoMediaType(void);

	/****
	功能：设置会话视频显示数据统一回调函数
	====================================================================*/
	void EB_SetVideoCallback(eb::bigint nCallId, PEBVideoCallBack cbAudio);
	//void EB_SetVideoCallback(eb::bigint nCallId,PVideoDataCallBack cbVideo,PAudioDataCallBack cbAudio);

	/****
	功能：设置会话视频业务回调窗口句柄，空为取消，使用默认句柄 EB_SetMsgHwnd
	====================================================================*/
#ifdef _QT_MAKE_
	void EB_SetVideoReceiver(eb::bigint nCallId,QObject* pReceiver);
#else
	void EB_SetVideoHwnd(eb::bigint nCallId,HWND pHwnd);
#endif

	/****
	功能：取消会话视频业务回调窗口句柄，使用默认句柄 EB_SetMsgHwnd
	====================================================================*/
	//void EB_DelVideoHwnd(eb::bigint nCallId);

	/****
	功能：请求会话视频通讯
	====================================================================*/
	int EB_VideoRequest(eb::bigint nCallId,EB_VIDEO_TYPE nVideoType=EB_VIDEO_BOTH);

	/****
	功能： 接受（加入）或拒绝会话视频通讯
	参数：bAccept true:接受视频通讯 false:拒绝视频通讯
	====================================================================*/
	int EB_VideoAck(eb::bigint nCallId,bool bAccept,eb::bigint nToUserId);

	/****
	功能： 结束或退出会话视频通讯
	====================================================================*/
	int EB_VideoEnd(eb::bigint nCallId);

	/****
	功能： 获取会话，所有用户视频编号；
	====================================================================*/
	bool EB_GetUserVideoInfo(eb::bigint nCallId,std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;

	/****
	功能：打开会话，本地视频设备
	描述：根据视频请求，打开视频设备（或声卡设备、或二个同时打开）
	参数：sCallId 会话编号
	参数：nLocalVideoIndex 打开指定索引位置视频设备，0开始（EB_VIDEO_BOTH有效）
	参数：[输出]返回本地视频媒体类型，用于视频显示用
	参数：PAudioDataCallBack cbAudio 音频数据回调函数
	参数：void* pParam 音频数据回调参数
	返回：返回本地用户视频编号 -1表示失败
	====================================================================*/
#ifdef _QT_MAKE_
    int EB_OpenLocalVideo(eb::bigint nCallId,int nLocalVideoIndex,QWidget* hVideoWndParent, void* pUserData=0);
#else
	int EB_OpenLocalVideo(eb::bigint nCallId,int nLocalVideoIndex,HWND hVideoWndParent, void* pUserData=0);
#endif
	//int EB_OpenLocalVideo(eb::bigint nCallId,int nLocalVideoIndex,int& pOutVideoMediaType,DWORD dwParamVideo,DWORD dwParamAudio);


	/****
	功能：打开会话，远程用户视频
	参数：sCallId 会话编号
	参数：sUserAccount 会话用户帐号
	参数：PAudioDataCallBack cbAudio 音频数据回调函数
	参数：void* pParam 音频数据回调参数
	====================================================================*/
#ifdef _QT_MAKE_
    bool EB_OpenUserVideo(eb::bigint nCallId,eb::bigint nUserId, QWidget *hVideoWndParent, void* pUserData=0);
#else
	bool EB_OpenUserVideo(eb::bigint nCallId,eb::bigint nUserId,HWND hVideoWndParent, void* pUserData=0);
#endif
	//bool EB_OpenVideo(eb::bigint nCallId,eb::bigint nUserId,DWORD dwParamVideo,DWORD dwParamAudio);

	/****
	功能：设置打开或屏蔽视频
	参数：sCallId 会话编号
	参数：nUserId 会话用户帐号
	参数：bOpen 打开或屏蔽视频
	====================================================================*/
	bool EB_SetVideoOpenClose(eb::bigint nCallId,eb::bigint nUserId,bool bOpen);

	/****
	功能：设置打开或屏蔽声音
	参数：sCallId 会话编号
	参数：nUserId 会话用户帐号
	参数：bOpen 打开或屏蔽声音
	====================================================================*/
	bool EB_SetAudioOpenClose(eb::bigint nCallId,eb::bigint nUserId,bool bOpen);
	//bool EB_GetAudioOpenClose(eb::bigint nCallId,eb::bigint nUserId) const;

	/****
	功能：关闭会话所有视频
	====================================================================*/
	void EB_CloseAllVideo(eb::bigint nCallId);

	/**** 应用功能 ***********************************************************************/
	/****
	功能：根据定位位置，获取订购应用功能信息
	参数：nFuncLocation，定位位置
	====================================================================*/
	int EB_GetSubscribeFuncList(EB_FUNC_LOCATION nFuncLocation,std::vector<EB_SubscribeFuncInfo>& pOutFuncInfo) const;

	/****
	功能：根据定位位置，获取订购应用功能数量
	参数：nFuncLocation，定位位置
	====================================================================*/
	int EB_GetSubscribeFuncSize(EB_FUNC_LOCATION nFuncLocation) const;

	/****
	功能：根据订购ID，获取订购应用功能信息
	参数：nSubId，订购ID
	====================================================================*/
	bool EB_GetSubscribeFuncInfo(eb::bigint nSubId, EB_SubscribeFuncInfo* pOutFuncInfo) const;

	/****
	功能：打开某个应用功能
	参数：nSubscribeId，订购ID
	====================================================================*/
	mycp::tstring EB_GetSubscribeFuncUrl(eb::bigint nSubscribeId,eb::bigint nCallId=0,eb::bigint nFromUserId=0,const mycp::tstring& sFromAccount="",eb::bigint nGroupId=0) const;

	/****
	功能：获取在线聊天记录URL
	参数：nSubscribeId，订购ID
	====================================================================*/
	mycp::tstring EB_GetConversationsUrl(eb::bigint nFromUserId,eb::bigint nGroupId) const;

	/****
	功能：接受或拒绝某个业务请求（如申请加入群组）
	参数：sMsgId，消息ID
	参数：bAccept，接受或拒绝 true/false
	====================================================================*/
	void EB_AckMsg(eb::bigint nMsgId,bool bAccept) const;
	// 1=接受（确认）; 2=拒绝; 3=离线接受; 4=超时; 5=删除
	void EB_AckMsg(eb::bigint nMsgId,int nAckType) const;

private:
	void* m_handle;
};

} // namespace entboost

#endif // __EBAppClient_h__
