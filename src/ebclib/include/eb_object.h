// eb_object.h file here
#ifndef __eb_object_h__
#define __eb_object_h__

//#include <string>
#include <CGCBase/cgcSmartString.h>
//#include "ebstring.h"
#include "time.h"
#include "eb_define.h"
#ifndef CEBString
#define CEBString tstring
#endif

namespace entboost {

/*==========================================================
APP应用信息
===========================================================*/
class EB_AreaInfo
{
public:
	int m_nAreaCode;
	int m_nParentCode;
	CEBString m_sAreaName;
	CEBString m_sAreaDigit;

	EB_AreaInfo(void)
		: m_nAreaCode(0),m_nParentCode(0)
	{}
	EB_AreaInfo(int nParent)
		: m_nAreaCode(0),m_nParentCode(nParent)
	{}
	EB_AreaInfo(const EB_AreaInfo* pObj)
		: m_nAreaCode(0),m_nParentCode(0)
	{
		if (pObj!=NULL)
			EB_AreaInfo::operator =(*pObj);
	}
#ifdef USES_EBCOM_TEST
	EB_AreaInfo(IEB_AreaInfo* pObj)
		: m_nAreaCode(0),m_nParentCode(0)
	{
		EB_AreaInfo::operator =(pObj);
	}
#endif
	EB_AreaInfo(const EB_AreaInfo& pObj)
		: m_nAreaCode(0),m_nParentCode(0)
	{
		EB_AreaInfo::operator =(pObj);
	}

	const EB_AreaInfo& operator=(const EB_AreaInfo& pObj)
	{
		m_nAreaCode = pObj.m_nAreaCode;
		m_nParentCode = pObj.m_nParentCode;
		m_sAreaName = pObj.m_sAreaName;
		m_sAreaDigit = pObj.m_sAreaDigit;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_AreaInfo& operator=(IEB_AreaInfo* pAreaInfo)
	{
		if (pAreaInfo!=NULL)
		{
			m_nAreaCode = pAreaInfo->AreaCode;
			m_nParentCode = pAreaInfo->ParentCode;
			m_sAreaName = pAreaInfo->AreaName.GetBSTR();
			m_sAreaDigit = pAreaInfo->AreaDigit.GetBSTR();
		}
		return *this;
	}
#endif
};

/*==========================================================
APP应用信息
===========================================================*/
class EB_APMsgInfo
{
public:
	eb::bigint m_sFromAppId;
	eb::bigint m_nGroupId;
	eb::bigint m_nUserId;
	CEBString m_sAccount;
	eb::bigint m_nMsgId;
	eb::bigint m_nMsgType;
	CEBString m_sMsgName;
	CEBString m_sMsgContent;
	CEBString m_sMsgTime;		// 空为当前时间
	eb::bigint m_nExtData;

	EB_APMsgInfo(void)
		: m_sFromAppId(0),m_nGroupId(0),m_nUserId(0)
		, m_nMsgId(0),m_nMsgType(0),m_nExtData(0)
	{
	}
	EB_APMsgInfo(const EB_APMsgInfo* pObj)
		: m_sFromAppId(0),m_nGroupId(0),m_nUserId(0)
		, m_nMsgId(0),m_nMsgType(0),m_nExtData(0)
	{
		if (pObj!=NULL)
			EB_APMsgInfo::operator =(*pObj);
	}
	EB_APMsgInfo(const EB_APMsgInfo& pObj)
		: m_sFromAppId(0),m_nGroupId(0),m_nUserId(0)
		, m_nMsgId(0),m_nMsgType(0)
	{
		EB_APMsgInfo::operator =(pObj);
	}
#ifdef USES_EBCOM_TEST
	EB_APMsgInfo(IEB_APMsgInfo* pObj)
		: m_sFromAppId(0),m_nGroupId(0),m_nUserId(0)
		, m_nMsgId(0),m_nMsgType(0)
	{
		EB_APMsgInfo::operator =(pObj);
	}
#endif
	const EB_APMsgInfo& operator=(const EB_APMsgInfo& pObj)
	{
		m_sFromAppId = pObj.m_sFromAppId;
		m_nGroupId = pObj.m_nGroupId;
		m_nUserId = pObj.m_nUserId;
		m_sAccount = pObj.m_sAccount;
		m_nMsgId = pObj.m_nMsgId;
		m_nMsgType = pObj.m_nMsgType;
		m_sMsgName = pObj.m_sMsgName;
		m_sMsgContent = pObj.m_sMsgContent;
		m_sMsgTime = pObj.m_sMsgTime;
		m_nExtData = pObj.m_nExtData;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_APMsgInfo& operator=(IEB_APMsgInfo* pObj)
	{
		if (pObj!=NULL)
		{
			m_sFromAppId = pObj->FromAppId;
			m_nGroupId = pObj->GroupId;
			m_nUserId = pObj->UserId;
			m_sAccount = pObj->Account.GetBSTR();
			m_nMsgId = pObj->MsgId;
			m_nMsgType = pObj->MsgType;
			m_sMsgName = pObj->MsgName.GetBSTR();
			m_sMsgContent = pObj->MsgContent.GetBSTR();
			m_sMsgTime = pObj->MsgTime.GetBSTR();
			m_nExtData = pObj->ExtData;
		}
		return *this;
	}
#endif
};

/*==========================================================
在线资源信息
===========================================================*/
class EB_ResourceInfo
{
public:
	eb::bigint m_sResId;
	eb::bigint m_sParentResId;
	CEBString m_sName;
	EB_RESOURCE_TYPE m_nResType;
	CEBString m_sDescription;
	eb::bigint m_nFromId;
	EB_RESOURCE_FROM_TYPE m_nFromType;
	//eb::bigint m_sEnterpriseCode;
	//eb::bigint m_sGroupCode;
	//CEBString m_sCreator;
	eb::bigint m_nCreateUserId;
	CEBString m_sTime;
	unsigned int m_nDownloads;
	eb::bigint m_nSize;
	EB_RESOURCE_SHARE_TYPE m_nShare;
	CEBString m_sOnlineViewUrl;
	EB_FILE_EXT_TYPE m_nViewExtType;

	EB_ResourceInfo(void)
		: m_sResId(0),m_sParentResId(0),m_nResType(EB_RESOURCE_UNKNOWN)
		, m_nFromId(0), m_nFromType(EB_RESOURCE_FROM_TYPE_UNKNOWN)
		//, m_sEnterpriseCode(0)
		//, m_sGroupCode(0)
		, m_nCreateUserId(0)
		, m_nDownloads(0)
		, m_nSize(0)
		, m_nShare(EB_RESOURCE_SHARE_UNKNOWN)
		, m_nViewExtType(EB_FILE_EXT_TYPE_OTHER)
	{
	}
	EB_ResourceInfo(const EB_ResourceInfo* pObj)
		: m_sResId(0),m_sParentResId(0),m_nResType(EB_RESOURCE_UNKNOWN)
		, m_nFromId(0), m_nFromType(EB_RESOURCE_FROM_TYPE_UNKNOWN)
		//, m_sEnterpriseCode(0)
		//, m_sGroupCode(0)
		, m_nCreateUserId(0)
		, m_nDownloads(0)
		, m_nSize(0)
		, m_nShare(EB_RESOURCE_SHARE_UNKNOWN)
		, m_nViewExtType(EB_FILE_EXT_TYPE_OTHER)
	{
		if (pObj!=NULL)
			EB_ResourceInfo::operator =(*pObj);
	}
	EB_ResourceInfo(const EB_ResourceInfo& pObj)
		: m_sResId(0),m_sParentResId(0),m_nResType(EB_RESOURCE_UNKNOWN)
	{
		EB_ResourceInfo::operator =(pObj);
	}
	const EB_ResourceInfo& operator=(const EB_ResourceInfo& pObj)
	{
		m_sResId = pObj.m_sResId;
		m_sParentResId = pObj.m_sParentResId;
		m_sName = pObj.m_sName;
		m_nResType = pObj.m_nResType;
		m_sDescription = pObj.m_sDescription;
		m_nFromId = pObj.m_nFromId;
		m_nFromType = pObj.m_nFromType;
		//m_sEnterpriseCode = pObj.m_sEnterpriseCode;
		//m_sGroupCode = pObj.m_sGroupCode;
		//m_sCreator = pObj.m_sCreator;
		m_nCreateUserId = pObj.m_nCreateUserId;
		m_sTime = pObj.m_sTime;
		m_nDownloads = pObj.m_nDownloads;
		m_nSize = pObj.m_nSize;
		m_nShare = pObj.m_nShare;
		m_sOnlineViewUrl = pObj.m_sOnlineViewUrl;
		m_nViewExtType = pObj.m_nViewExtType;
		return *this;
	}
};

/*==========================================================
订购功能应用信息
===========================================================*/
class EB_SubscribeFuncInfo
{
public:
	eb::bigint m_nSubscribeId;
	eb::bigint m_nFromSubscribeId;
	//eb::bigint m_nFunctionId;
	//int m_nSubscribeLevel;
	int m_nLocation;
	eb::bigint m_nIconResId;
	CEBString m_sResFile;
	CEBString m_sFunctionName;
	CEBString m_sFunctionUrl;
	EB_FUNC_MODE m_nFunctionMode;
	int m_nWindowWidth;
	int m_nWindowHeight;
	bool m_bDisableContextMenu;
	bool m_bDisableScrollBar;
	bool m_bClosePrompt;
	bool m_bWinResizable;
	int m_nDisplayIndex;
	EB_FUNC_BROWSER_TYPE m_nBrowserType;
	CEBString m_sDescription;

	EB_SubscribeFuncInfo(void)
		: m_nSubscribeId(0), m_nFromSubscribeId(0)
		//, m_nFunctionId(0)
		//, m_nSubscribeLevel(0)
		, m_nLocation(0)
		, m_nIconResId(0)
		, m_nFunctionMode(EB_FUNC_MODE_BROWSER),m_nWindowWidth(0),m_nWindowHeight(0),m_bDisableContextMenu(true),m_bDisableScrollBar(false),m_bClosePrompt(true),m_bWinResizable(true)
		, m_nDisplayIndex(0)
		, m_nBrowserType(EB_FUNC_BROWSER_TYPE_DEFAULT)
	{
	}
	EB_SubscribeFuncInfo(eb::bigint nSubscribeId)
		: m_nSubscribeId(nSubscribeId), m_nFromSubscribeId(0)
		//, m_nFunctionId(nFunctionId)
		//, m_nSubscribeLevel(0)
		, m_nLocation(0)
		, m_nIconResId(0)
		, m_nFunctionMode(EB_FUNC_MODE_BROWSER),m_nWindowWidth(0),m_nWindowHeight(0),m_bDisableContextMenu(true),m_bDisableScrollBar(false),m_bClosePrompt(true),m_bWinResizable(true)
		, m_nDisplayIndex(0)
		, m_nBrowserType(EB_FUNC_BROWSER_TYPE_DEFAULT)
	{
	}
	EB_SubscribeFuncInfo(const EB_SubscribeFuncInfo& pObj)
	{
		EB_SubscribeFuncInfo::operator =(pObj);
	}
	EB_SubscribeFuncInfo(const EB_SubscribeFuncInfo* pObj)
		: m_nSubscribeId(0), m_nFromSubscribeId(0)
		//, m_nFunctionId(0)
		//, m_nSubscribeLevel(0)
		, m_nLocation(0)
		, m_nIconResId(0)
		, m_nFunctionMode(EB_FUNC_MODE_BROWSER),m_nWindowWidth(0),m_nWindowHeight(0),m_bDisableContextMenu(true),m_bDisableScrollBar(false),m_bClosePrompt(true),m_bWinResizable(true)
		, m_nDisplayIndex(0)
		, m_nBrowserType(EB_FUNC_BROWSER_TYPE_DEFAULT)
	{
		EB_SubscribeFuncInfo::operator =(pObj);
	}
#ifdef USES_EBCOM_TEST
	EB_SubscribeFuncInfo(IEB_SubscribeFuncInfo* pObj)
		: m_nSubscribeId(0), m_nFromSubscribeId(0)
		//, m_nFunctionId(0)
		//, m_nSubscribeLevel(0)
		, m_nLocation(0)
		, m_nIconResId(0)
		, m_nFunctionMode(EB_FUNC_MODE_BROWSER),m_nWindowWidth(0),m_nWindowHeight(0),m_bDisableContextMenu(true),m_bDisableScrollBar(false),m_bClosePrompt(true),m_bWinResizable(true)
		, m_nDisplayIndex(0)
	{
		EB_SubscribeFuncInfo::operator =(pObj);
	}
#endif
	const EB_SubscribeFuncInfo& operator =(const EB_SubscribeFuncInfo* pObj)
	{
		if (pObj)
			EB_SubscribeFuncInfo::operator =(*pObj);
		return *this;
	}
	const EB_SubscribeFuncInfo& operator =(const EB_SubscribeFuncInfo& pObj)
	{
		m_nSubscribeId = pObj.m_nSubscribeId;
		m_nFromSubscribeId = pObj.m_nFromSubscribeId;
		//m_nFunctionId = pObj.m_nFunctionId;
		//m_nSubscribeLevel = pObj.m_nSubscribeLevel;
		m_nLocation = pObj.m_nLocation;
		m_nIconResId = pObj.m_nIconResId;
		m_sResFile = pObj.m_sResFile;
		m_sFunctionName = pObj.m_sFunctionName;
		m_sFunctionUrl = pObj.m_sFunctionUrl;
		m_nFunctionMode = pObj.m_nFunctionMode;
		m_nWindowWidth = pObj.m_nWindowWidth;
		m_nWindowHeight = pObj.m_nWindowHeight;
		m_bDisableContextMenu = pObj.m_bDisableContextMenu;
		m_bDisableScrollBar = pObj.m_bDisableScrollBar;
		m_bClosePrompt = pObj.m_bClosePrompt;
		m_bWinResizable = pObj.m_bWinResizable;
		m_nDisplayIndex = pObj.m_nDisplayIndex;
		m_nBrowserType = pObj.m_nBrowserType;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_SubscribeFuncInfo& operator =(IEB_SubscribeFuncInfo* pObj)
	{
		if (pObj != NULL)
		{
			m_nSubscribeId = pObj->SubscribeId;
			m_nFromSubscribeId = pObj->FromSubscribeId;
			m_nLocation = pObj->Location;
			m_nIconResId = pObj->IconResId;
			m_sResFile = pObj->ResFile.GetBSTR();
			m_sFunctionName = pObj->FuncName.GetBSTR();
			m_sFunctionUrl = pObj->FuncUrl.GetBSTR();
			m_nFunctionMode = (EB_FUNC_MODE)pObj->FuncMode;
			m_nWindowWidth = pObj->WindowWidth;
			m_nWindowHeight = pObj->WindowHeight;
			m_bDisableContextMenu = pObj->DisableContextMenu==VARIANT_TRUE?true:false;
			m_bDisableScrollBar = pObj->DisableScrollBar==VARIANT_TRUE?true:false;
			m_bClosePrompt = pObj->ClosePrompt==VARIANT_TRUE?true:false;
			m_bWinResizable = pObj->WinResizable==VARIANT_TRUE?true:false;
			m_nDisplayIndex = pObj->DisplayIndex;
			m_nBrowserType = pObj->BrowserType;
		}
		return *this;
	}
#endif
};

/*==========================================================
表情信息/头像信息
===========================================================*/
class EB_EmotionInfo
{
public:
	eb::bigint m_sResId;
	CEBString m_sResFile;
	int m_nIndex;
	int m_nType;
	eb::bigint m_nUserId;
	eb::bigint m_sEnterpriseCode;
	//CEBString m_sAccount;
	//CEBString m_sEnterpriseCode;	// 空为个人表情
	EB_RESOURCE_TYPE m_nResType;	
	CEBString m_sDescription;

	EB_EmotionInfo(void)
		: m_sResId(0),m_nIndex(0)
		, m_nType(0)
		, m_nUserId(0),m_sEnterpriseCode(0)
		, m_nResType(EB_RESOURCE_UNKNOWN)
	{
	}
	EB_EmotionInfo(eb::bigint sResId,int nIndex,int nType,eb::bigint sAccount,eb::bigint sEntCode,EB_RESOURCE_TYPE nResType)
		: m_sResId(sResId)
		, m_nIndex(nIndex)
		, m_nType(nType)
		, m_nUserId(sAccount), m_sEnterpriseCode(sEntCode)
		, m_nResType(nResType)
	{
	}
	EB_EmotionInfo(const EB_EmotionInfo& pObj)
		: m_sResId(0),m_nIndex(0)
		, m_nType(0)
		, m_nUserId(0),m_sEnterpriseCode(0)
		, m_nResType(EB_RESOURCE_UNKNOWN)
	{
		EB_EmotionInfo::operator =(pObj);
	}
	EB_EmotionInfo(const EB_EmotionInfo* pObj)
		: m_sResId(0),m_nIndex(0)
		, m_nType(0)
		, m_nUserId(0),m_sEnterpriseCode(0)
		, m_nResType(EB_RESOURCE_UNKNOWN)
	{
		EB_EmotionInfo::operator =(pObj);
	}
#ifdef USES_EBCOM_TEST
	EB_EmotionInfo(IEB_EmotionInfo* pObj)
		: m_sResId(0),m_nIndex(0)
		, m_nType(0)
		, m_nUserId(0),m_sEnterpriseCode(0)
		, m_nResType(EB_RESOURCE_UNKNOWN)
	{
		EB_EmotionInfo::operator =(pObj);
	}
#endif
	const EB_EmotionInfo& operator =(const EB_EmotionInfo* pObj)
	{
		if (pObj)
			EB_EmotionInfo::operator =(*pObj);
		return *this;
	}
	const EB_EmotionInfo& operator =(const EB_EmotionInfo& pObj)
	{
		m_sResId = pObj.m_sResId;
		m_sResFile = pObj.m_sResFile;
		m_nIndex = pObj.m_nIndex;
		m_nType = pObj.m_nType;
		m_nUserId = pObj.m_nUserId;
		m_sEnterpriseCode = pObj.m_sEnterpriseCode;
		m_nResType = pObj.m_nResType;
		m_sDescription = pObj.m_sDescription;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_EmotionInfo& operator =(IEB_EmotionInfo* pObj)
	{
		if (pObj != NULL)
		{
			m_sResId = pObj->ResId;
			m_sResFile = pObj->ResFile.GetBSTR();
			m_nIndex = pObj->Index;
			m_nType = pObj->Type;
			m_nUserId = pObj->UserId;
			m_sEnterpriseCode = pObj->EnterpriseCode;
			m_nResType = (EB_RESOURCE_TYPE)pObj->ResType;
			m_sDescription = pObj->Description.GetBSTR();
		}
		return *this;
	}
#endif
};

/*==========================================================
版本信息
===========================================================*/
class EB_VersionInfo
{
public:
	CEBString m_sVersion;			// 版本号: x.x.x.x
	CEBString m_sDescription;		// 更新说明
	CEBString m_sDownloadUrl;		// 下载链接地址
	CEBString m_sUpdateTime;		// 更新时间
	int m_nUpdateType;				// 更新类型 0：普通更新；1：强制更新（更新才能继续使用）
	CEBString m_sVersionFile;		// 打包文件
	eb::bigint m_sResId;
	CEBString m_sMD5;

	EB_VersionInfo(void)
		: m_nUpdateType(0)
		, m_sResId(0)
	{
	}
	EB_VersionInfo(const EB_VersionInfo& pVersionInfo)
		: m_nUpdateType(0)
		, m_sResId(0)
	{
		EB_VersionInfo::operator =(pVersionInfo);
	}
	const EB_VersionInfo& operator =(const EB_VersionInfo& pVersionInfo)
	{
		m_sVersion = pVersionInfo.m_sVersion;
		m_sDescription = pVersionInfo.m_sDescription;
		m_sDownloadUrl = pVersionInfo.m_sDownloadUrl;
		m_sUpdateTime = pVersionInfo.m_sUpdateTime;
		m_nUpdateType = pVersionInfo.m_nUpdateType;
		m_sVersionFile = pVersionInfo.m_sVersionFile;
		m_sResId = pVersionInfo.m_sResId;
		return *this;
	}
};

/*==========================================================
电子名片
===========================================================*/
class EB_ECardInfo
{
public:
	EB_ACCOUNT_TYPE m_nAccountType;
	eb::bigint	m_nMemberUserId;
	eb::bigint	m_sMemberCode;
	//CEBString	m_sMemberCode;
	CEBString	m_sName;
	CEBString	m_sPhone;
	CEBString	m_sTel;
	CEBString	m_sEmail;
	CEBString	m_sTitle;
	CEBString	m_sGroupName;
	eb::bigint	m_sGroupCode;
	//CEBString	m_sGroupCode;
	CEBString	m_sEnterprise;
	CEBString	m_sAddress;
	eb::bigint	m_nUserSignId;
	eb::bigint	m_nFromIp;
	eb::bigint	m_nHeadResId;
	CEBString	m_sHeadCmServer;
	CEBString	m_sHeadHttpServer;
	CEBString	m_sHeadMd5;
	CEBString	m_sHeadResourceFile;

	EB_ECardInfo(void)
		: m_nAccountType(EB_ACCOUNT_TYPE_VISITOR)
		, m_nMemberUserId(0)
		, m_sMemberCode(0)
		, m_sGroupCode(0)
		, m_nUserSignId(0),m_nFromIp(0)
		, m_nHeadResId(0)
	{
	}
	EB_ECardInfo(const EB_ECardInfo& pECardInfo)
		: m_nAccountType(EB_ACCOUNT_TYPE_VISITOR)
		, m_nMemberUserId(0)
		, m_sMemberCode(0)
		, m_sGroupCode(0)
		, m_nUserSignId(0),m_nFromIp(0)
		, m_nHeadResId(0)
	{
		EB_ECardInfo::operator = (pECardInfo);
	}
	const EB_ECardInfo& operator = (const EB_ECardInfo& pECardInfo)
	{
		EB_ECardInfo::operator =(&pECardInfo);
		return *this;
	}
	const EB_ECardInfo& operator = (const EB_ECardInfo* pECardInfo)
	{
		if (pECardInfo)
		{
			m_nAccountType = pECardInfo->m_nAccountType;
			m_nMemberUserId = pECardInfo->m_nMemberUserId;
			m_sMemberCode = pECardInfo->m_sMemberCode;
			m_sName = pECardInfo->m_sName;
			m_sPhone = pECardInfo->m_sPhone;
			m_sTel = pECardInfo->m_sTel;
			m_sEmail = pECardInfo->m_sEmail;
			m_sTitle = pECardInfo->m_sTitle;
			m_sGroupName = pECardInfo->m_sGroupName;
			m_sGroupCode = pECardInfo->m_sGroupCode;
			m_sEnterprise = pECardInfo->m_sEnterprise;
			m_sAddress = pECardInfo->m_sAddress;
			m_nUserSignId = pECardInfo->m_nUserSignId;
			m_nFromIp = pECardInfo->m_nFromIp;
			m_nHeadResId = pECardInfo->m_nHeadResId;
			m_sHeadCmServer = pECardInfo->m_sHeadCmServer;
			m_sHeadHttpServer = pECardInfo->m_sHeadHttpServer;
			m_sHeadMd5 = pECardInfo->m_sHeadMd5;
			m_sHeadResourceFile = pECardInfo->m_sHeadResourceFile;
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_ECardInfo& operator = (IEB_ECardInfo* pECardInfo)
	{
		if (pECardInfo)
		{
			m_nAccountType = (EB_ACCOUNT_TYPE)pECardInfo->AccountType;
			m_nMemberUserId = pECardInfo->MemberUserId;
			m_sMemberCode = pECardInfo->MemberCode;
			m_sName = pECardInfo->Name.GetBSTR();
			m_sPhone = pECardInfo->Phone.GetBSTR();
			m_sTel = pECardInfo->Tel.GetBSTR();
			m_sEmail = pECardInfo->Email.GetBSTR();
			m_sTitle = pECardInfo->Title.GetBSTR();
			m_sGroupName = pECardInfo->GroupName.GetBSTR();
			m_sGroupCode = pECardInfo->GroupCode;
			m_sEnterprise = pECardInfo->Enterprise.GetBSTR();
			m_sAddress = pECardInfo->Address.GetBSTR();
			m_nUserSignId = pECardInfo->UserSignId;
			m_nFromIp = pECardInfo->FromIp;
			m_nHeadResId = pECardInfo->HeadResId;
			m_sHeadCmServer = pECardInfo->HeadCmServer.GetBSTR();
			m_sHeadHttpServer = pECardInfo->HeadHttpServer.GetBSTR();
			m_sHeadMd5 = pECardInfo->HeadMd5.GetBSTR();
			m_sHeadResourceFile = pECardInfo->HeadResourceFile.GetBSTR();
		}
		return *this;
	}
#endif
};

/*==========================================================
群组（部门）成员信息
===========================================================*/
class EB_MemberInfo
{
public:
	EB_MemberInfo(void)
		: m_sMemberCode(0), m_sGroupCode(0), m_nMemberUserId(0), m_nJobPosition(0)
		, m_nManagerLevel(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_sHeadResourceId(0)
		, m_nCSId(0),m_nCSExt(0)
	{}
	EB_MemberInfo(eb::bigint nGroupId, eb::bigint nMemberUserId)
		: m_sMemberCode(0),m_sGroupCode(nGroupId)
		, m_nMemberUserId(nMemberUserId)
		, m_nJobPosition(0)
		, m_nManagerLevel(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_sHeadResourceId(0)
		, m_nCSId(0),m_nCSExt(0)
	{}
	EB_MemberInfo(const EB_MemberInfo* pMemberInfo)
		: m_sMemberCode(0), m_sGroupCode(0), m_nMemberUserId(0), m_nJobPosition(0)
		, m_nManagerLevel(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_sHeadResourceId(0)
		, m_nCSId(0),m_nCSExt(0)
	{
		EB_MemberInfo::operator =(pMemberInfo);
	}
	EB_MemberInfo(const EB_MemberInfo& pMemberInfo)
		: m_sMemberCode(0),m_sGroupCode(0),m_nMemberUserId(0), m_nJobPosition(0)
		, m_nManagerLevel(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_sHeadResourceId(0)
		, m_nCSId(0),m_nCSExt(0)
	{
		EB_MemberInfo::operator =(pMemberInfo);
	}
#ifdef USES_EBCOM_TEST
	EB_MemberInfo(IEB_MemberInfo* pMemberInfo)
		: m_sMemberCode(0),m_sGroupCode(0),m_nMemberUserId(0), m_nJobPosition(0)
		, m_nManagerLevel(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_sHeadResourceId(0)
		, m_nCSId(0),m_nCSExt(0)
	{
		EB_MemberInfo::operator =(pMemberInfo);
	}
#endif
	const EB_MemberInfo& operator =(const EB_MemberInfo& pMemberInfo)
	{
		EB_MemberInfo::operator =(&pMemberInfo);
		return *this;
	}
	const EB_MemberInfo& operator =(const EB_MemberInfo * pMemberInfo)
	{
		if (pMemberInfo)
		{
			m_sMemberCode = pMemberInfo->m_sMemberCode;
			m_sGroupCode = pMemberInfo->m_sGroupCode;
			m_nMemberUserId = pMemberInfo->m_nMemberUserId;
			m_sMemberAccount = pMemberInfo->m_sMemberAccount;
			m_sUserName = pMemberInfo->m_sUserName;
			m_sJobTitle = pMemberInfo->m_sJobTitle;
			m_nJobPosition = pMemberInfo->m_nJobPosition;
			m_sCellPhone = pMemberInfo->m_sCellPhone;
			m_sFax = pMemberInfo->m_sFax;
			m_sWorkPhone = pMemberInfo->m_sWorkPhone;
			m_sEmail = pMemberInfo->m_sEmail;
			m_sAddress = pMemberInfo->m_sAddress;
			m_sDescription = pMemberInfo->m_sDescription;
			m_nManagerLevel = pMemberInfo->m_nManagerLevel;
			m_nGender = pMemberInfo->m_nGender;
			m_nBirthday = pMemberInfo->m_nBirthday;
			m_nLineState = pMemberInfo->m_nLineState;
			m_sHeadResourceId = pMemberInfo->m_sHeadResourceId;
			m_sHeadResourceFile = pMemberInfo->m_sHeadResourceFile;
			m_nCSId = pMemberInfo->m_nCSId;
			m_nCSExt = pMemberInfo->m_nCSExt;
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_MemberInfo& operator =(IEB_MemberInfo * pMemberInfo)
	{
		if (pMemberInfo)
		{
			m_sMemberCode = pMemberInfo->MemberCode;
			m_sGroupCode = pMemberInfo->GroupCode;
			m_nMemberUserId = pMemberInfo->MemberUserId;
			m_sMemberAccount = pMemberInfo->MemberAccount.GetBSTR();
			m_sUserName = pMemberInfo->UserName.GetBSTR();
			m_sJobTitle = pMemberInfo->JobTitle.GetBSTR();
			m_nJobPosition = pMemberInfo->JobPosition;
			m_sCellPhone = pMemberInfo->CellPhone.GetBSTR();
			m_sFax = pMemberInfo->Fax.GetBSTR();
			m_sWorkPhone = pMemberInfo->WorkPhone.GetBSTR();
			m_sEmail = pMemberInfo->Email.GetBSTR();
			m_sAddress = pMemberInfo->Address.GetBSTR();
			m_sDescription = pMemberInfo->Description.GetBSTR();
			//m_nManagerLevel = pMemberInfo->ManagerLevel;
			m_nGender = (EB_GENDER_TYPE)pMemberInfo->Gender;
			m_nBirthday = pMemberInfo->Birthday;
			m_nLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
			m_sHeadResourceId = pMemberInfo->HeadResourceId;
			m_sHeadResourceFile = pMemberInfo->HeadResourceFile.GetBSTR();
			m_nCSId = pMemberInfo->CSId;
			m_nCSExt = pMemberInfo->CSExt;
		}
		return *this;
	}
#endif
	void GetECardInfo(EB_ECardInfo* pOutCardInfo) const
	{
		if (pOutCardInfo!=NULL)
		{
			pOutCardInfo->m_nMemberUserId = m_nMemberUserId;
			pOutCardInfo->m_sMemberCode = m_sMemberCode;
			pOutCardInfo->m_sPhone = m_sCellPhone;
			pOutCardInfo->m_sName = m_sUserName;
			pOutCardInfo->m_sTel = m_sWorkPhone;
			pOutCardInfo->m_sEmail = m_sEmail;
			pOutCardInfo->m_sTitle = m_sJobTitle;
		}
	}

	eb::bigint m_sMemberCode;
	eb::bigint m_sGroupCode;
	//CEBString m_sMemberCode;
	//CEBString m_sGroupCode;
	eb::bigint m_nMemberUserId;
	CEBString m_sMemberAccount;
	CEBString m_sUserName;
	CEBString m_sJobTitle;
	int m_nJobPosition;
	CEBString m_sCellPhone;
	CEBString m_sFax;
	CEBString m_sWorkPhone;
	CEBString m_sEmail;
	CEBString m_sAddress;
	CEBString m_sDescription;
	int m_nManagerLevel;
	EB_GENDER_TYPE m_nGender;		// 0:unknown 1:M 2:F
	int m_nBirthday;				// 19790101
	// account_t
	EB_USER_LINE_STATE m_nLineState;
	// 
	eb::bigint m_sHeadResourceId;
	CEBString m_sHeadResourceFile;
	eb::bigint m_nCSId;
	int m_nCSExt;
};

/*==========================================================
企业信息
===========================================================*/
class EB_EnterpriseInfo
{
public:
	EB_EnterpriseInfo(void)
		: m_sEnterpriseCode(0),m_nCreateUserId(0)
		, m_nEntExt(0)
	{}
	EB_EnterpriseInfo(eb::bigint nEnterpriseId, const CEBString& sEnterpriseName)
		: m_sEnterpriseCode(nEnterpriseId)
		, m_sEnterpriseName(sEnterpriseName), m_nCreateUserId(0)
		, m_nEntExt(0)
	{}
	EB_EnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo)
		: m_sEnterpriseCode(0),m_nCreateUserId(0)
		, m_nEntExt(0)
	{
		EB_EnterpriseInfo::operator =(pEnterpriseInfo);
	}
	EB_EnterpriseInfo(const EB_EnterpriseInfo& pEnterpriseInfo)
		: m_sEnterpriseCode(0),m_nCreateUserId(0)
		, m_nEntExt(0)
	{
		EB_EnterpriseInfo::operator =(pEnterpriseInfo);
	}
	const EB_EnterpriseInfo& operator =(const EB_EnterpriseInfo* pEnterpriseInfo)
	{
		if (pEnterpriseInfo!=NULL)
		{
			EB_EnterpriseInfo::operator =(*pEnterpriseInfo);
		}
		return *this;
	}
	const EB_EnterpriseInfo& operator =(const EB_EnterpriseInfo& pEnterpriseInfo)
	{
		m_sEnterpriseCode = pEnterpriseInfo.m_sEnterpriseCode;
		m_sEnterpriseName = pEnterpriseInfo.m_sEnterpriseName;
		m_sDescription = pEnterpriseInfo.m_sDescription;
		m_sPhone = pEnterpriseInfo.m_sPhone;
		m_sFax = pEnterpriseInfo.m_sFax;
		m_sEmail = pEnterpriseInfo.m_sEmail;
		m_sUrl = pEnterpriseInfo.m_sUrl;
		m_sAddress = pEnterpriseInfo.m_sAddress;
		m_sCreateTime = pEnterpriseInfo.m_sCreateTime;
		//m_sCreator = pEnterpriseInfo.m_sCreator;
		m_nCreateUserId = pEnterpriseInfo.m_nCreateUserId;
		m_nEntExt = pEnterpriseInfo.m_nEntExt;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_EnterpriseInfo& operator =(IEB_EnterpriseInfo* pEnterpriseInfo)
	{
		if (pEnterpriseInfo != NULL)
		{
			m_sEnterpriseCode = pEnterpriseInfo->EnterpriseCode;
			m_sEnterpriseName = pEnterpriseInfo->EnterpriseName.GetBSTR();
			m_sDescription = pEnterpriseInfo->Description.GetBSTR();
			m_sPhone = pEnterpriseInfo->Phone.GetBSTR();
			m_sFax = pEnterpriseInfo->Fax.GetBSTR();
			m_sEmail = pEnterpriseInfo->Email.GetBSTR();
			m_sUrl = pEnterpriseInfo->Url.GetBSTR();
			m_sAddress = pEnterpriseInfo->Address.GetBSTR();
			m_sCreateTime = pEnterpriseInfo->CreateTime.GetBSTR();
			//m_sCreator = pEnterpriseInfo->Creator.GetBSTR();
			m_nCreateUserId = pEnterpriseInfo->CreateUserId;
			m_nEntExt = pEnterpriseInfo->EntExt;
		}
		return *this;
	}
#endif
	eb::bigint m_sEnterpriseCode;
	//CEBString m_sEnterpriseCode;
	CEBString m_sEnterpriseName;
	CEBString m_sDescription;
	CEBString m_sPhone;
	CEBString m_sFax;
	CEBString m_sEmail;
	CEBString m_sUrl;
	CEBString m_sAddress;
	CEBString m_sCreateTime;
	//CEBString m_sCreator;
	eb::bigint m_nCreateUserId;
	int m_nEntExt;
};

/*==========================================================
群组（部门）信息
===========================================================*/
class EB_GroupInfo
{
public:
	EB_GroupInfo(void)
		: m_sEnterpriseCode(0), m_sGroupCode(0), m_sParentCode(0)
		, m_nCreateUserId(0),m_nManagerUserId(0),m_nDisplayIndex(0), m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
		, m_nMyEmpId(0),m_nEmpCount(0),m_nGroupVer(0)
	{}
	EB_GroupInfo(eb::bigint nEnterpriseId, eb::bigint nGroupId)
		: m_sEnterpriseCode(nEnterpriseId), m_sGroupCode(nGroupId), m_sParentCode(0)
		, m_nCreateUserId(0),m_nManagerUserId(0),m_nDisplayIndex(0),m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
		, m_nMyEmpId(0),m_nEmpCount(0),m_nGroupVer(0)
	{}
	EB_GroupInfo(const EB_GroupInfo* pGroupInfo)
		: m_sEnterpriseCode(0), m_sGroupCode(0), m_sParentCode(0)
		, m_nCreateUserId(0),m_nManagerUserId(0),m_nDisplayIndex(0),m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
		, m_nMyEmpId(0),m_nEmpCount(0),m_nGroupVer(0)
	{
		EB_GroupInfo::operator =(pGroupInfo);
	}
	EB_GroupInfo(const EB_GroupInfo& pGroupInfo)
		: m_sEnterpriseCode(0), m_sGroupCode(0), m_sParentCode(0)
		, m_nCreateUserId(0),m_nManagerUserId(0),m_nDisplayIndex(0),m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
		, m_nMyEmpId(0),m_nEmpCount(0),m_nGroupVer(0)
	{
		EB_GroupInfo::operator =(pGroupInfo);
	}
#ifdef USES_EBCOM_TEST
	EB_GroupInfo(IEB_GroupInfo* pObj)
		: m_sEnterpriseCode(0), m_sGroupCode(0), m_sParentCode(0)
		, m_nCreateUserId(0),m_nManagerUserId(0),m_nDisplayIndex(0),m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
		, m_nMyEmpId(0),m_nEmpCount(0),m_nGroupVer(0)
	{
		EB_GroupInfo::operator =(pObj);
	}
#endif
	const EB_GroupInfo& operator =(const EB_GroupInfo* pGroupInfo)
	{
		if (pGroupInfo!=NULL)
		{
			EB_GroupInfo::operator =(*pGroupInfo);
		}
		return *this;
	}
	const EB_GroupInfo& operator =(const EB_GroupInfo& pGroupInfo)
	{
		m_sEnterpriseCode = pGroupInfo.m_sEnterpriseCode;
		m_sGroupCode = pGroupInfo.m_sGroupCode;
		m_sGroupName = pGroupInfo.m_sGroupName;
		m_sParentCode = pGroupInfo.m_sParentCode;
		m_sDescription = pGroupInfo.m_sDescription;
		m_sPhone = pGroupInfo.m_sPhone;
		m_sFax = pGroupInfo.m_sFax;
		m_sEmail = pGroupInfo.m_sEmail;
		m_sUrl = pGroupInfo.m_sUrl;
		m_sAddress = pGroupInfo.m_sAddress;
		m_sCreateTime = pGroupInfo.m_sCreateTime;
		//m_sCreator = pGroupInfo.m_sCreator;
		m_nCreateUserId = pGroupInfo.m_nCreateUserId;
		m_nManagerUserId = pGroupInfo.m_nManagerUserId;
		m_nDisplayIndex = pGroupInfo.m_nDisplayIndex;
		m_nGroupType = pGroupInfo.m_nGroupType;
		m_nMyEmpId = pGroupInfo.m_nMyEmpId;
		m_nEmpCount = pGroupInfo.m_nEmpCount;
		m_nGroupVer = pGroupInfo.m_nGroupVer;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_GroupInfo& operator =(IEB_GroupInfo* pGroupInfo)
	{
		if (pGroupInfo != NULL)
		{
			m_sEnterpriseCode = pGroupInfo->EnterpriseCode;
			m_sGroupCode = pGroupInfo->GroupCode;
			m_sGroupName = pGroupInfo->GroupName.GetBSTR();
			m_sParentCode = pGroupInfo->ParentCode;
			m_sDescription = pGroupInfo->Description.GetBSTR();
			m_sPhone = pGroupInfo->Phone.GetBSTR();
			m_sFax = pGroupInfo->Fax.GetBSTR();
			m_sEmail = pGroupInfo->Email.GetBSTR();
			m_sUrl = pGroupInfo->Url.GetBSTR();
			m_sAddress = pGroupInfo->Address.GetBSTR();
			m_sCreateTime = pGroupInfo->CreateTime.GetBSTR();
			//m_sCreator = pGroupInfo->Creator.GetBSTR();
			m_nCreateUserId = pGroupInfo->CreateUserId;
			m_nManagerUserId = pGroupInfo->ManagerUserId;
			m_nDisplayIndex = pGroupInfo->DisplayIndex;
			m_nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
			m_nMyEmpId = pGroupInfo->MyEmpId;
			m_nEmpCount = pGroupInfo->EmpCount;
		}
		return *this;
	}
#endif
	eb::bigint m_sEnterpriseCode;
	eb::bigint m_sGroupCode;
	CEBString m_sGroupName;
	eb::bigint m_sParentCode;
	//CEBString m_sParentCode;
	CEBString m_sDescription;
	CEBString m_sPhone;
	CEBString m_sFax;
	CEBString m_sEmail;
	CEBString m_sUrl;
	CEBString m_sAddress;
	CEBString m_sCreateTime;
	//CEBString m_sCreator;
	eb::bigint m_nCreateUserId;
	eb::bigint m_nManagerUserId;
	int m_nDisplayIndex;
	EB_GROUP_TYPE m_nGroupType;
	eb::bigint m_nMyEmpId;
	int m_nEmpCount;
	eb::bigint m_nGroupVer;
};

/*==========================================================
用户分组信息
===========================================================*/
class EB_UGInfo
{
public:
	EB_UGInfo(void)
		: m_nUGId(0)
	{}
	EB_UGInfo(eb::bigint nUGId,const CEBString& sGroupName)
		: m_nUGId(nUGId),m_sGroupName(sGroupName)
	{}
	EB_UGInfo(const EB_UGInfo* pUGInfo)
		: m_nUGId(0)
	{
		EB_UGInfo::operator =(pUGInfo);
	}
	const EB_UGInfo& operator = (const EB_UGInfo& pUGInfo)
	{
		EB_UGInfo::operator =(&pUGInfo);
		return *this;
	}
	const EB_UGInfo& operator = (const EB_UGInfo* pUGInfo)
	{
		if (pUGInfo != NULL)
		{
			m_nUGId = pUGInfo->m_nUGId;
			m_sGroupName = pUGInfo->m_sGroupName;
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_UGInfo& operator = (IEB_UGInfo* pUGInfo)
	{
		if (pUGInfo != NULL)
		{
			m_nUGId = pUGInfo->UGId;
			m_sGroupName = pUGInfo->GroupName.GetBSTR();
		}
		return *this;
	}
#endif
	eb::bigint m_nUGId;
	CEBString m_sGroupName;
};

/*==========================================================
通讯录（联系人）信息
===========================================================*/
class EB_ContactInfo : public EB_UGInfo
{
public:
	EB_ContactInfo(void)
		: EB_UGInfo(),m_nContactId(0),m_nContactUserId(0),m_nContactType(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
	{}
	EB_ContactInfo(eb::bigint nContactId)
		: EB_UGInfo(),m_nContactId(nContactId),m_nContactType(0)//,m_sContact(sContact)
		, m_nContactUserId(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
	{}
	//EB_ContactInfo(const CEBString& sContact)
	//	: EB_UGInfo(),m_nContactId(0),m_sContact(sContact)
	//	, m_nContactUserId(0)
	//{}
	EB_ContactInfo(const EB_ContactInfo* pContactInfo)
		: EB_UGInfo(),m_nContactId(0),m_nContactUserId(0),m_nContactType(0)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
	{
		EB_ContactInfo::operator =(pContactInfo);
	}
	const EB_ContactInfo& operator = (const EB_ContactInfo& pContactInfo)
	{
		EB_ContactInfo::operator =(&pContactInfo);
		return *this;
	}
	const EB_ContactInfo& operator = (const EB_ContactInfo* pContactInfo)
	{
		if (pContactInfo != NULL)
		{
			EB_UGInfo::operator =((EB_UGInfo*)pContactInfo);
			m_nContactId = pContactInfo->m_nContactId;
			m_nContactType = pContactInfo->m_nContactType;
			m_sContact = pContactInfo->m_sContact;
			//m_sGroup = pContactInfo->m_sGroup;
			m_sName = pContactInfo->m_sName;
			m_sPhone = pContactInfo->m_sPhone;
			m_sEmail = pContactInfo->m_sEmail;
			m_sAddress = pContactInfo->m_sAddress;
			m_sDescription = pContactInfo->m_sDescription;
			m_sCompany = pContactInfo->m_sCompany;
			m_sJobTitle = pContactInfo->m_sJobTitle;
			m_sTel = pContactInfo->m_sTel;
			m_sFax = pContactInfo->m_sFax;
			m_sUrl = pContactInfo->m_sUrl;
			m_nContactUserId = pContactInfo->m_nContactUserId;
			m_nLineState = pContactInfo->m_nLineState;
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_ContactInfo& operator = (IEB_ContactInfo* pContactInfo)
	{
		if (pContactInfo != NULL)
		{
			m_nUGId = pContactInfo->UGId;
			m_sGroupName = pContactInfo->GroupName.GetBSTR();

			m_nContactId = pContactInfo->ContactId;
			m_nContactUserId = pContactInfo->ContactUserId;
			m_nContactType = pContactInfo->ContactType;
			m_sContact = pContactInfo->Contact.GetBSTR();
			//m_sGroup = pContactInfo->Group.GetBSTR();
			m_sName = pContactInfo->Name.GetBSTR();
			m_sPhone = pContactInfo->Phone.GetBSTR();
			m_sEmail = pContactInfo->Email.GetBSTR();
			m_sAddress = pContactInfo->Address.GetBSTR();
			m_sDescription = pContactInfo->Description.GetBSTR();
			m_sCompany = pContactInfo->Company.GetBSTR();
			m_sJobTitle = pContactInfo->JobTitle.GetBSTR();
			m_sTel = pContactInfo->Tel.GetBSTR();
			m_sFax = pContactInfo->Fax.GetBSTR();
			m_sUrl = pContactInfo->Url.GetBSTR();
			m_nLineState = (EB_USER_LINE_STATE)pContactInfo->LineState;
		}
		return *this;
	}
#endif
	eb::bigint m_nContactId;
	eb::bigint m_nContactUserId;
	int m_nContactType;
	CEBString m_sContact;
	//CEBString m_sGroup;
	CEBString m_sName;
	CEBString m_sPhone;
	//eb::bigint m_sPhone;
	CEBString m_sEmail;
	CEBString m_sAddress;
	CEBString m_sDescription;
	CEBString m_sCompany;
	CEBString m_sJobTitle;
	CEBString m_sTel;
	CEBString m_sFax;
	CEBString m_sUrl;
	EB_USER_LINE_STATE m_nLineState;
};


/*==========================================================
会话远程桌面信息
===========================================================*/
class EB_RemoteDesktopInfo
{
public:
	EB_RemoteDesktopInfo(void)
		: m_sCallId(0), m_nRDType(EB_RD_DESKTOP_SRC), m_nFromUserId(0), m_bOnDesktop(false)
	{
	}
	EB_RemoteDesktopInfo(const EB_RemoteDesktopInfo& pObj)
		: m_sCallId(0), m_nRDType(EB_RD_DESKTOP_SRC), m_nFromUserId(0), m_bOnDesktop(false)
	{
		EB_RemoteDesktopInfo::operator =(pObj);
	}
	const EB_RemoteDesktopInfo& operator = (const EB_RemoteDesktopInfo& pObj)
	{
		m_sCallId = pObj.m_sCallId;
		m_nRDType = pObj.m_nRDType;
		m_nFromUserId = pObj.m_nFromUserId;
		m_bOnDesktop = pObj.m_bOnDesktop;
		return *this;
	}
	eb::bigint m_sCallId;
	EB_REMOTE_DESKTOP_TYPE m_nRDType;
	eb::bigint m_nFromUserId;
	bool m_bOnDesktop;
};

/*==========================================================
会话视频信息
===========================================================*/
class EB_VideoInfo
{
public:
	EB_VideoInfo(void)
		: m_sCallId(0), m_nVideoType(EB_VIDEO_BOTH)
	{
	}
	EB_VideoInfo(const EB_VideoInfo& pObj)
		: m_sCallId(0), m_nVideoType(EB_VIDEO_BOTH)
	{
		EB_VideoInfo::operator =(pObj);
	}
	const EB_VideoInfo& operator = (const EB_VideoInfo& pObj)
	{
		m_sCallId = pObj.m_sCallId;
		m_nVideoType = pObj.m_nVideoType;
		return *this;
	}
	eb::bigint m_sCallId;
	EB_VIDEO_TYPE m_nVideoType;
};

/*==========================================================
用户视频信息
===========================================================*/
class EB_UserVideoInfo
{
public:
	EB_UserVideoInfo(void)
		: m_sUserAccount(0), m_bOnVideo(false)//, m_nUserVideoId(0)
	{
	}
	EB_UserVideoInfo(eb::bigint sUser, bool bOnVideo)
		: m_sUserAccount(sUser),m_bOnVideo(bOnVideo)//, m_nUserVideoId(nId)
	{}
	EB_UserVideoInfo(const EB_UserVideoInfo& pInfo)
		: m_sUserAccount(0),m_bOnVideo(false)//, m_nUserVideoId(0)
	{
		EB_UserVideoInfo::operator =(pInfo);
	}
#ifdef USES_EBCOM_TEST
	EB_UserVideoInfo(IEB_UserVideoInfo* pInfo)
		: m_sUserAccount(0),m_bOnVideo(false)//, m_nUserVideoId(0)
	{
		EB_UserVideoInfo::operator =(pInfo);
	}
#endif
	const EB_UserVideoInfo& operator =(const EB_UserVideoInfo& pInfo)
	{
		m_sUserAccount = pInfo.m_sUserAccount;
		m_bOnVideo = pInfo.m_bOnVideo;
		//m_nUserVideoId = pInfo.m_nUserVideoId;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_UserVideoInfo& operator =(IEB_UserVideoInfo* pInfo)
	{
		if (pInfo != NULL)
		{
			m_sUserAccount = pInfo->UserId;
			m_bOnVideo = pInfo->OnVideo==VARIANT_TRUE?true:false;
			//m_nUserVideoId = pInfo->UserVideoId;
		}
		return *this;
	}
	
#endif
	eb::bigint m_sUserAccount;
	//int m_nUserVideoId;
	bool m_bOnVideo;
};

/*==========================================================
帐号信息
===========================================================*/
class EB_AccountInfo
{
public:
	EB_AccountInfo(void)
		: m_nUserId(0),m_nPhone(0),m_nLogonType(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nArea1(0), m_nArea2(0), m_nArea3(0), m_nArea4(0)
		//, m_nLineState(EB_LINE_STATE_UNKNOWN)
		//, m_nSetting(0)
		//, m_tOnlineTime(0)
	{
	}
	EB_AccountInfo(eb::bigint nUserId, const CEBString& sAccount,int nLogonType)
		: m_nUserId(nUserId),m_sAccount(sAccount)
		, m_nPhone(0)
		, m_nLogonType(nLogonType)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nArea1(0), m_nArea2(0), m_nArea3(0), m_nArea4(0)
		//, m_nLineState(EB_LINE_STATE_UNKNOWN)
		//, m_nSetting(0)
	{
		//m_tOnlineTime = time(0);
	}
	EB_AccountInfo(const EB_AccountInfo* pAccountInfo)
		: m_nUserId(0),m_nPhone(0),m_nLogonType(0)
		, m_nGender(EB_GENDER_UNKNOWN)
		, m_nBirthday(0)
		, m_nArea1(0), m_nArea2(0), m_nArea3(0), m_nArea4(0)
		//, m_nLineState(EB_LINE_STATE_UNKNOWN)
		//, m_nSetting(0)
		//, m_tOnlineTime(0)
	{
		EB_AccountInfo::operator =(pAccountInfo);
	}
#ifdef USES_EBCOM_TEST
	EB_AccountInfo(const IEB_AccountInfo* pAccountInfo)
		: m_nLogonType(0)
		//, m_nLineState(EB_LINE_STATE_UNKNOWN)
		//, m_nSetting(0)
		//, m_tOnlineTime(0)
	{
		EB_AccountInfo::operator =(pAccountInfo);
	}
#endif
	const EB_AccountInfo& operator =(const EB_AccountInfo& pAccountInfo)
	{
		EB_AccountInfo::operator =(&pAccountInfo);
		return *this;
	}
	const EB_AccountInfo& operator =(const EB_AccountInfo* pAccountInfo)
	{
		if (pAccountInfo!=NULL)
		{
			m_nUserId = pAccountInfo->m_nUserId;
			m_sAccount = pAccountInfo->m_sAccount;
			m_sPasswd = pAccountInfo->m_sPasswd;
			m_sUserName = pAccountInfo->m_sUserName;
			m_nGender = pAccountInfo->m_nGender;
			m_nBirthday = pAccountInfo->m_nBirthday;
			m_sMobile = pAccountInfo->m_sMobile;
			m_sTel = pAccountInfo->m_sTel;
			m_sEmail = pAccountInfo->m_sEmail;
			m_nArea1 = pAccountInfo->m_nArea1;
			m_sArea1 = pAccountInfo->m_sArea1;
			m_nArea2 = pAccountInfo->m_nArea2;
			m_sArea2 = pAccountInfo->m_sArea2;
			m_nArea3 = pAccountInfo->m_nArea3;
			m_sArea3 = pAccountInfo->m_sArea3;
			m_nArea4 = pAccountInfo->m_nArea4;
			m_sArea4 = pAccountInfo->m_sArea4;
			m_sAddress = pAccountInfo->m_sAddress;
			m_sUrl = pAccountInfo->m_sUrl;
			m_sZipCode = pAccountInfo->m_sZipCode;
			m_nLogonType = pAccountInfo->m_nLogonType;
			//m_tOnlineTime = pAccountInfo->m_tOnlineTime;
			//m_nLineState = pAccountInfo->m_nLineState;
			m_sDescription = pAccountInfo->m_sDescription;
			//m_nSetting = pAccountInfo->m_nSetting;
			//m_sDefaultEmp = pAccountInfo->m_sDefaultEmp;
			m_pFromCardInfo.operator =(&pAccountInfo->m_pFromCardInfo);
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_AccountInfo& operator =(IEB_AccountInfo* pAccountInfo)
	{
		if (pAccountInfo!=NULL)
		{
			m_nUserId = pAccountInfo->UserId;
			m_sAccount = pAccountInfo->Account.GetBSTR();
			m_sPasswd = pAccountInfo->Passwd.GetBSTR();
			m_sUserName = pAccountInfo->UserName.GetBSTR();
			m_nGender = (EB_GENDER_TYPE)pAccountInfo->Gender;
			m_nBirthday = pAccountInfo->Birthday;
			m_sMobile = pAccountInfo->Mobile.GetBSTR();
			m_sTel = pAccountInfo->Tel.GetBSTR();
			m_sEmail = pAccountInfo->Email.GetBSTR();
			m_nArea1 = pAccountInfo->Area1;
			m_sArea1 = pAccountInfo->Area1String.GetBSTR();
			m_nArea2 = pAccountInfo->Area2;
			m_sArea2 = pAccountInfo->Area2String.GetBSTR();
			m_nArea3 = pAccountInfo->Area3;
			m_sArea3 = pAccountInfo->Area3String.GetBSTR();
			m_nArea4 = pAccountInfo->Area4;
			m_sArea4 = pAccountInfo->Area4String.GetBSTR();
			m_sAddress = pAccountInfo->Address.GetBSTR();
			m_sUrl = pAccountInfo->Url.GetBSTR();
			m_sZipCode = pAccountInfo->ZipCode.GetBSTR();
			m_nLogonType = pAccountInfo->LogonType;
			//m_tOnlineTime = pAccountInfo->m_tOnlineTime;
			//m_nLineState = pAccountInfo->m_nLineState;
			m_sDescription = pAccountInfo->Description.GetBSTR();
			//m_nSetting = pAccountInfo->m_nSetting;
			//m_sDefaultEmp = pAccountInfo->m_sDefaultEmp;
			CComPtr<IEB_ECardInfo> m_pECardInfo;
			pAccountInfo->get_ECardInfo(&m_pECardInfo);
			m_pFromCardInfo.operator =(m_pECardInfo);
		}
		return *this;
	}
#endif
	void SetUserId(eb::bigint v) {m_nUserId = v;}
	eb::bigint GetUserId(void) const {return m_nUserId;}
//	CEBString GetStringUserId(void) const
//	{
//		char lpszUserId[24];
//#ifdef WIN32
//		sprintf(lpszUserId,"%I64d",m_nUserId);
//#else
//		sprintf(lpszUserId,"%lld",m_nUserId);
//#endif
//		return lpszUserId;
//	}
	void SetPhone(eb::bigint v) {m_nPhone = v;}
	eb::bigint GetPhone(void) const {return m_nPhone;}
	void SetAccount(const CEBString& v) {m_sAccount=v;}
	const CEBString & GetAccount(void) const {return m_sAccount;}
	void SetPasswd(const CEBString& v) {m_sPasswd=v;}
	const CEBString& GetPasswd(void) const {return m_sPasswd;}
	void SetUserName(const CEBString& v) {m_sUserName=v;}
	const CEBString& GetUserName(void) const {return m_sUserName;}
	void SetLogonType(int value) {m_nLogonType = value;}
	int GetLogonType(void) const {return m_nLogonType;}
	//void SetOnlineTime(time_t value) {m_tOnlineTime = value;}
	//time_t GetOnlineTime(void) const {return m_tOnlineTime;}
	//void SetLineState(EB_USER_LINE_STATE value) {m_nLineState = value;}
	//EB_USER_LINE_STATE GetLineState(void) const {return m_nLineState;}
	//bool IsUnknownLineState(void) const {return m_nLineState == EB_LINE_STATE_UNKNOWN;}
	//bool IsOnLineState(void) const {return m_nLineState == EB_LINE_STATE_ONLINE;}
	//bool IsOffLineState(void) const {return m_nLineState == EB_LINE_STATE_OFFLINE;}
	void SetDescription(const CEBString& v) {m_sDescription = v;}
	const CEBString& GetDescription(void) const {return m_sDescription;}
	//void SetSetting(int v) {m_nSetting = v;}
	//int GetSetting(void) const {return m_nSetting;}
	//void SetDefaultEmp(const CEBString& v) {m_sDefaultEmp = v;}
	//const CEBString& GetDefaultEmp(void) const {return m_sDefaultEmp;}

	void SetGender(EB_GENDER_TYPE v) {m_nGender = v;}
	EB_GENDER_TYPE GetGender(void) const {return m_nGender;}
	void SetBirthday(int v) {m_nBirthday = v;}
	int GetBirthday(void) const {return m_nBirthday;}
	void SetMobile(const CEBString& v) {m_sMobile = v;}
	const CEBString& GetMobile(void) const {return m_sMobile;}
	void SetTel(const CEBString& v) {m_sTel = v;}
	const CEBString& GetTel(void) const {return m_sTel;}
	void SetEmail(const CEBString& v) {m_sEmail = v;}
	const CEBString& GetEmail(void) const {return m_sEmail;}
	void SetArea1(int v) {m_nArea1 = v;}
	int GetArea1(void) const {return m_nArea1;}
	void SetArea1String(const CEBString& v) {m_sArea1 = v;}
	const CEBString& GetArea1String(void) const {return m_sArea1;}
	void SetArea2(int v) {m_nArea2 = v;}
	int GetArea2(void) const {return m_nArea2;}
	void SetArea2String(const CEBString& v) {m_sArea2 = v;}
	const CEBString& GetArea2String(void) const {return m_sArea2;}
	void SetArea3(int v) {m_nArea3 = v;}
	int GetArea3(void) const {return m_nArea3;}
	void SetArea3String(const CEBString& v) {m_sArea3 = v;}
	const CEBString& GetArea3String(void) const {return m_sArea3;}
	void SetArea4(int v) {m_nArea4 = v;}
	int GetArea4(void) const {return m_nArea4;}
	void SetArea4String(const CEBString& v) {m_sArea4 = v;}
	const CEBString& GetArea4String(void) const {return m_sArea4;}
	void SetAddress(const CEBString& v) {m_sAddress = v;}
	const CEBString& GetAddress(void) const {return m_sAddress;}
	void SetUrl(const CEBString& v) {m_sUrl = v;}
	const CEBString& GetUrl(void) const {return m_sUrl;}
	void SetZipCode(const CEBString& v) {m_sZipCode = v;}
	const CEBString& GetZipCode(void) const {return m_sZipCode;}

	EB_ECardInfo m_pFromCardInfo;
protected:
	eb::bigint		m_nUserId;
	CEBString		m_sAccount;
	eb::bigint		m_nPhone;
	CEBString		m_sPasswd;
	CEBString		m_sUserName;
	unsigned int	m_nLogonType;
	//time_t			m_tOnlineTime;
	//EB_USER_LINE_STATE m_nLineState;
	CEBString		m_sDescription;
	//unsigned int	m_nSetting;
	//CEBString		m_sDefaultEmp;
	EB_GENDER_TYPE m_nGender;		// 0:unknown 1:M 2:F
	int m_nBirthday;				// 19790101
	CEBString m_sMobile;
	CEBString m_sTel;
	CEBString m_sEmail;
	int m_nArea1;
	CEBString m_sArea1;
	int m_nArea2;
	CEBString m_sArea2;
	int m_nArea3;
	CEBString m_sArea3;
	int m_nArea4;
	CEBString m_sArea4;
	CEBString m_sAddress;
	CEBString m_sUrl;
	CEBString m_sZipCode;
};

/*==========================================================
呼叫（会话）信息
===========================================================*/
class EB_CallInfo
{
public:
	EB_CallInfo(void)
		: m_sGroupCode(0),m_sOldCallId(0),m_sCallId(0),m_nFromUserId(0)
	{
	}
	EB_CallInfo(eb::bigint nFromUserId, const CEBString & sFromAccount, eb::bigint sCallId,eb::bigint sGroupCode)
		: m_nFromUserId(nFromUserId),m_sFromAccount(sFromAccount),m_sCallId(sCallId),m_sGroupCode(sGroupCode),m_sOldCallId(0)
		//, m_nStateCode(0)
		//, m_bOwner(false)
	{
	}
	EB_CallInfo(const EB_CallInfo* pCallInfo)
		: m_sGroupCode(0),m_sOldCallId(0),m_sCallId(0),m_nFromUserId(0)
	{
		EB_CallInfo::operator =(pCallInfo);
	}
	const EB_CallInfo& operator =(const EB_CallInfo& pCallInfo)
	{
		EB_CallInfo::operator =(&pCallInfo);
		return *this;
	}
	const EB_CallInfo& operator =(const EB_CallInfo* pCallInfo)
	{
		if (pCallInfo!=NULL)
		{
			m_sCallId = pCallInfo->m_sCallId;
			m_sGroupCode = pCallInfo->m_sGroupCode;
			m_sOldCallId = pCallInfo->m_sOldCallId;
			m_nFromUserId = pCallInfo->m_nFromUserId;
			m_sFromAccount = pCallInfo->m_sFromAccount;
		}
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const EB_CallInfo& operator =(IEB_CallInfo* pCallInfo)
	{
		if (pCallInfo != NULL)
		{
			m_sCallId = pCallInfo->CallId;
			m_sGroupCode = pCallInfo->GroupCode;
			m_sOldCallId = pCallInfo->OldCallId;
			m_nFromUserId = pCallInfo->FromUserId;
			m_sFromAccount = pCallInfo->FromAccount.GetBSTR();
		}
		return *this;
	}
#endif
	void SetCallId(eb::bigint v) {m_sCallId=v;}
	eb::bigint GetCallId(void) const {return m_sCallId;}
	void SetFromUserId(eb::bigint nFromUserId) {m_nFromUserId=nFromUserId;}
	eb::bigint GetFromUserId(void) const {return m_nFromUserId;}
	void SetFromAccount(const CEBString& sFromAccount) {m_sFromAccount = sFromAccount;}
	const CEBString& GetFromAccount(void) const {return m_sFromAccount;}

	//EB_CallInfo(const CEBString & sFromAccount, int nStateCode)
	//	: m_sFromAccount(sFromAccount), m_sCallId("")
	//	, m_nStateCode(nStateCode)
	//	, m_bOwner(false)
	//{
	//}
	eb::bigint m_sGroupCode;
	eb::bigint m_sOldCallId;
protected:
	eb::bigint m_sCallId;
	eb::bigint m_nFromUserId;
	CEBString m_sFromAccount;
	//int m_nStateCode;
	//bool m_bOwner;
};
//
//
///*==========================================================
//呼叫连接信息
//===========================================================*/
//class EB_ConnectInfo
//	: public EB_CallInfo
//{
//public:
//	enum CONNECTED_FLAG
//	{
//		CONNECTED_OFFLINE_USER		= 0x001
//		, CONNECTED_OWNER_CALL		= 0x002
//		, CONNECTED_AUTO_ACK			= 0x004
//		, CONNECTED_ONLINE_INCALL	= 0x008
//	};
//
//	EB_ConnectInfo(void)
//		: m_nConnectedFlag(0)
//	{
//	}
//	EB_ConnectInfo(const CEBString & sFromAccount, const CEBString & sCallId,const CEBString& sGroupCode)
//		: EB_CallInfo(sFromAccount,sCallId,sGroupCode)
//		, m_nConnectedFlag(0)
//	{
//	}
//	int m_nConnectedFlag;
//};

} // namespace entboost

#endif // __eb_object_h__
