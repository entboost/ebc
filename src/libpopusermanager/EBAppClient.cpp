#ifdef _MSC_VER //WIN32
#pragma warning(disable:4819)
#endif // WIN32
#include "stdafx.h"
#include "UserManagerApp.h"
#include "EBAppClient.h"
#include "../include/md5.h"
#ifndef _QT_MAKE_
//#ifdef _MSC_VER
#include "WmiInfo.h"
#endif // _MSC_VER
#ifdef WIN32
#else
#define _FILE_OFFSET_BITS 64
#endif

#ifdef _QT_MAKE_
#include <QFileInfo>
//#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>
#else
#include <direct.h>
#include <io.h>
#endif

namespace entboost {

inline bool IsIpAddress(const char* pString, size_t nLen)
{
	for (size_t i=0;i<nLen; i++)
	{
		const char pChar = pString[i];
		if (pChar== '.' || (pChar>='0' && pChar<='9'))
			continue;
		return false;
	}
	return true;
}
mycp::tstring GetHostIp(const char * lpszHostName,const char* lpszDefaultName)
{
	if (lpszHostName==NULL || strlen(lpszHostName)==0)
		return lpszDefaultName;
	try
	{
		if (IsIpAddress(lpszHostName,strlen(lpszHostName))) return lpszHostName;
		struct hostent *host_entry;
		//struct sockaddr_in addr;
		/* 即要解析的域名或主机名 */
		host_entry=gethostbyname(lpszHostName);
		//printf("%s\n", dn_or_ip);
		if(host_entry!=0)
		{
			//printf("解析IP地址: ");
			char lpszIpAddress[50];
			memset(lpszIpAddress, 0, sizeof(lpszIpAddress));
			sprintf(lpszIpAddress, "%d.%d.%d.%d",
				(host_entry->h_addr_list[0][0]&0x00ff),
				(host_entry->h_addr_list[0][1]&0x00ff),
				(host_entry->h_addr_list[0][2]&0x00ff),
				(host_entry->h_addr_list[0][3]&0x00ff));
			return lpszIpAddress;
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
	return lpszDefaultName;
}

bool GetAddressPort(const char* sAddress, mycp::tstring & sOutAddress, int & nOutPort)
{
	const mycp::tstring sFullAddress(sAddress);
	std::string::size_type find = sFullAddress.find(":");
	if (find == std::string::npos)
		return false;
	sOutAddress = sFullAddress.substr(0, find);
	nOutPort = atoi(sFullAddress.substr(find+1).c_str());
	return true;
}
mycp::tstring theLocalHostOAuthKey;
void GetLocalHostOAuthKey(mycp::tstring& pOutLocalHostOAuthKey)
{
	if (theLocalHostOAuthKey.empty())
	{
#ifdef _QT_MAKE_
        ///
        QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface interfaceItem, interfaceList) {
            if(interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
                    &&interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
                    &&interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
                    &&interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
                    &&!interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack)
                    &&interfaceItem.hardwareAddress()!="00:50:56:C0:00:01"
                    &&interfaceItem.hardwareAddress()!="00:50:56:C0:00:08")
            {
                theLocalHostOAuthKey = interfaceItem.hardwareAddress().toStdString();
                break;
            }
        }

#else
		CWmiInfo pWmiInfo;
		pWmiInfo.InitWmi();
		tstring strCpuInfo; 
		tstring strBoardInfo; 
		tstring strCpuClassMem[] = {_T("Caption"),_T("ProcessorId")};
		tstring strBoardClassMem[] = {_T("Caption"),_T("SerialNumber")};
		pWmiInfo.GetGroupItemInfo(_T("Win32_Processor"),strCpuClassMem,2,strCpuInfo);
		pWmiInfo.GetGroupItemInfo(_T("Win32_BaseBoard"),strBoardClassMem,2,strBoardInfo);
        theLocalHostOAuthKey = strCpuInfo+strBoardInfo;
#endif
        MD5 md5;
        md5.update((const unsigned char *)theLocalHostOAuthKey.c_str(), theLocalHostOAuthKey.size());
        md5.finalize();
        theLocalHostOAuthKey = md5.hex_digest();
	}
	pOutLocalHostOAuthKey = theLocalHostOAuthKey;
}

//bool GetAccountAddress(const char* sAccount, mycp::tstring & sOutAddress)
////bool GetAccountAddress(const char* sAccount, mycp::tstring& sOutAccount, mycp::tstring & sOutAddress)
//{
//	const mycp::tstring sFullAccount(sAccount);
//	std::string::size_type find = sFullAccount.find("@");
//	if (find == std::string::npos)
//		return false;
//	//sOutAccount = sFullAccount.substr(0, find);
//	mycp::tstring sFullAddress = sFullAccount.substr(find+1);
//	int nPort = 0;
//	if (!GetAddressPort(sFullAddress.c_str(), sOutAddress, nPort))
//	{
//		sOutAddress = sFullAddress;
//	}
//	const mycp::tstring sPOPDomain = "entboost.entboost.com";
//	//const mycp::tstring sPOPDomain = "entboost." + sOutAddress;
//	const mycp::tstring sPOPIp = GetHostIp(sPOPDomain.c_str());
//	if (!sPOPIp.empty())
//	{
//		sOutAddress = sPOPIp;
//	}else
//	{
//		//sPOPIp = GetHostIp("entboost.entboost.com");
//		//if (sPOPIp.empty())
//		//	return -1;
//	}
//	if (nPort > 0)
//	{
//		char lpszBuffer[60];
//		sprintf(lpszBuffer, "%s:%d", sOutAddress.c_str(), nPort);
//		sOutAddress = lpszBuffer;
//	}else
//	{
//		sOutAddress.append(":18012");
//	}
//	return true;
//}
//mycp::tstring str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
//{
//	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strSource, -1, NULL, 0);
//	if (unicodeLen <= 0) return "";
//
//	wchar_t * pUnicode = new wchar_t[unicodeLen];
//	memset(pUnicode,0,(unicodeLen)*sizeof(wchar_t));
//
//	MultiByteToWideChar(sourceCodepage, 0, strSource, -1, (wchar_t*)pUnicode, unicodeLen);
//
//	char * pTargetData = 0;
//	int targetLen = WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);
//	if (targetLen <= 0)
//	{
//		delete[] pUnicode;
//		return "";
//	}
//
//	pTargetData = new char[targetLen];
//	memset(pTargetData, 0, targetLen);
//
//	WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);
//
//	mycp::tstring result(pTargetData);
//	//	mycp::tstring result(pTargetData, targetLen);
//	delete[] pUnicode;
//	delete[] pTargetData;
//	return   result;
//}
//mycp::tstring ACP2UTF8(const char* sString)
//{
//	return str_convert(sString,CP_ACP,CP_UTF8);
//}
//mycp::tstring UTF82ACP(const char* sString)
//{
//	return str_convert(sString,CP_UTF8,CP_ACP);
//}

CEBAppClient::CEBAppClient(void)
: m_handle(0)
{

}
CEBAppClient::~CEBAppClient(void)
{
	EB_UnInit();
}

void CEBAppClient::EB_Init(const char* address,const char* sInitParameter)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager == NULL)
	{
		pManager = new CUserManagerApp();
		m_handle = pManager;
	}
	pManager->SetInitParameter(sInitParameter);
	int nPort = 18012;	// 默认18012端口
	mycp::tstring sOutAddress = "entboost.entboost.com";
	if (strlen(address)>0 && !entboost::GetAddressPort(address,sOutAddress,nPort))
	{
		sOutAddress = address;
	}
	char lpszFullAddress[50];
	sprintf(lpszFullAddress,"%s:%d",sOutAddress.c_str(),nPort);
	pManager->SetServerAddress(lpszFullAddress,address);
}

mycp::tstring CEBAppClient::EB_GetAddress(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetServerAddress();
	}else
	{
		return "";
	}
}

bool CEBAppClient::EB_IsInited(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	return (bool)(pManager != NULL);
}
void CEBAppClient::EB_SetCallBack(CEBCallbackInterface * pCallBack)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->m_callback = pCallBack;
	}
}
#ifdef _QT_MAKE_
void CEBAppClient::EB_SetMsgReceiver(QObject* pHwnd)
#else
void CEBAppClient::EB_SetMsgHwnd(HWND pHwnd)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->m_pHwnd = pHwnd;
	}
}
void CEBAppClient::EB_UnInit(void)
{
	EB_Logout();
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	m_handle = NULL;
	if (pManager != NULL)
	{
		//pManager->m_pLogonCenter->Stop();
		delete pManager;
	}
}
EB_STATE_CODE CEBAppClient::EB_GetLastStateCode(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetLastStateCode();
	}
	return EB_STATE_ERROR;
}
EBFileString CEBAppClient::EB_GetAppPath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppPath();
	}
	return "";
}
EBFileString CEBAppClient::EB_GetResourcePath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetResourcePath();
	}
	return "";
}

EBFileString CEBAppClient::EB_GetAppDataPath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppDataPath();
	}
	return "";
}
EBFileString CEBAppClient::EB_GetAppDataTempPath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppDataTempPath();
	}
	return "";
}

int CEBAppClient::EB_SetDevAppId(eb::bigint sAppId, const char* sAppKey,bool bReLoadAppOnlineKey)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetDevAppId(sAppId,sAppKey,bReLoadAppOnlineKey);
	}
	return -1;
}
void CEBAppClient::EB_SetDevAppOnlineKey(eb::bigint sAppId, const char* sAppOnlineKey)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->SetDevAppOnlineKey(sAppId,sAppOnlineKey);
	}
}
eb::bigint CEBAppClient::EB_GetDevAppId(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetDevAppId();
	}
	return 0;
}
mycp::tstring CEBAppClient::EB_GetDevAppOnlineKey(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetDevAppOnlineKey();
	}
	return "";
}
bool CEBAppClient::EB_SetSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long nParameterValue)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		switch (nParameter)
		{
		case EB_SYSTEM_PARAMETER_LOGON_USER_DATA:
			{
				if (nParameterValue==0) return false;
				mycp::tstring sLogonUserData((const char*)nParameterValue);
				if (sLogonUserData.size()>256)
					sLogonUserData = sLogonUserData.substr(0,256);
				pManager->SetLogonUserData(sLogonUserData);
				return true;
			}break;
		default:
			break;
		}
	}
	return false;
}

void CEBAppClient::EB_GetSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long* pParameterValue) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pParameterValue==NULL) return;
		*pParameterValue = 0;
		switch (nParameter)
		{
		case EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID:
			{
				if (pManager->m_nMyCollectionSubId>0)
				{
					char * lpszBuffer = new char[24];
					sprintf(lpszBuffer,"%lld",pManager->m_nMyCollectionSubId);
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_STAT_SUB_GROUP_MEMBER:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_STATSUB_GROUP_MEMBER)==0?1:0;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_MSG_RECEIPT:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_MSG_RECEIPT)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_ACCOUNT_EDIT)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL:
			{
				// ??? V1.22版本，1.23以后使用访问令牌，完美解决网络问题，可以删除该代码；（不返回任务数据，外面不会处理）
				const mycp::tstring sLogonHttpReqUrl = pManager->GetLogonHttpReqUrl();
				if (!sLogonHttpReqUrl.empty())
				{
					char * lpszBuffer = new char[sLogonHttpReqUrl.size()+1];
					memset(lpszBuffer,0,sLogonHttpReqUrl.size()+1);
					strcpy(lpszBuffer,sLogonHttpReqUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}
			break;
		case EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DEFAULT_BROWSER_IE)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_VIDEO:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_VIDEO)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_REMOTE_DESKTOP)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_USER_CLOUD_DRIVE)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_GROUP_SHARED_CLOUD)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_MODIFY_PASSWD)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_AUTH_INVITEADD2GROUP:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_INVITEADD2GROUP)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_AUTH_CONTACT:
			{
				*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==0?0:1;
			}break;
		case EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX:
			{
				if (!pManager->m_sAccountPrefix.empty())
				{
					char * lpszBuffer = new char[pManager->m_sAccountPrefix.size()+1];
					memset(lpszBuffer,0,pManager->m_sAccountPrefix.size()+1);
					strcpy(lpszBuffer,pManager->m_sAccountPrefix.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_OPEN_REGISTER:
			*pParameterValue = pManager->m_nOpenRegister;
			break;
		case EB_SYSTEM_PARAMETER_OPEN_VISITOR:
			*pParameterValue = pManager->m_bOpenVisitor?1:0;
			break;
		case EB_SYSTEM_PARAMETER_LICENSE_USER:
			*pParameterValue = pManager->m_bLicenseUser?1:0;
			break;
		case EB_SYSTEM_PARAMETER_FORGET_PWD_URL:
			{
				if (!pManager->m_sForgetPwdUrl.empty())
				{
					char * lpszBuffer = new char[pManager->m_sForgetPwdUrl.size()+1];
					memset(lpszBuffer,0,pManager->m_sForgetPwdUrl.size()+1);
					strcpy(lpszBuffer,pManager->m_sForgetPwdUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_REGISTER_URL:
			{
				if (!pManager->m_sRegisterUrl.empty())
				{
					char * lpszBuffer = new char[pManager->m_sRegisterUrl.size()+1];
					memset(lpszBuffer,0,pManager->m_sRegisterUrl.size()+1);
					strcpy(lpszBuffer,pManager->m_sRegisterUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_ENT_LOGO_URL:
			{
				if (!pManager->m_sEntLogoUrl.empty())
				{
					char * lpszBuffer = new char[pManager->m_sEntLogoUrl.size()+1];
					memset(lpszBuffer,0,pManager->m_sEntLogoUrl.size()+1);
					strcpy(lpszBuffer,pManager->m_sEntLogoUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_SEND_REG_MAIL:
			{
				if (!pManager->m_sAccountPrefix.empty())
					*pParameterValue = 0;
				else
				{
					//*pParameterValue = pManager->m_nSendRegMail;
					*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_SEND_REG_MAIL)==0?0:1;
				}
			}break;
		case EB_SYSTEM_PARAMETER_EB_SERVER_VERSION:
				*pParameterValue = (unsigned long)pManager->m_nEBServerVersion;
			break;
		case EB_SYSTEM_PARAMETER_LICENSE_TYPE:
			{
				*pParameterValue = (unsigned long)pManager->m_nLicenstType;
			}break;
		case EB_SYSTEM_PARAMETER_DEFAULT_URL:
			{
				if (!pManager->m_sDefaultUrl.empty())
				{
					char * lpszBuffer = new char[pManager->m_sDefaultUrl.size()+1];
					memset(lpszBuffer,0,pManager->m_sDefaultUrl.size()+1);
					strcpy(lpszBuffer,pManager->m_sDefaultUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_DEPLOY_ID:
			{
				if (pManager->m_nDeployId>0)
				{
					char * lpszBuffer = new char[24];
					sprintf(lpszBuffer,"%lld",pManager->m_nDeployId);
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_PRODUCT_NAME:
			{
				if (!pManager->m_sProductName.empty())
				{
					char * lpszBuffer = new char[pManager->m_sProductName.size()+1];
					memset(lpszBuffer,0,pManager->m_sProductName.size()+1);
					strcpy(lpszBuffer,pManager->m_sProductName.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_ENT_MANAGER_URL:
			{
				const mycp::tstring sEntManagerUrl = pManager->GetEntManagerUrl();
				if (!sEntManagerUrl.empty())
				{
					char * lpszBuffer = new char[sEntManagerUrl.size()+1];
					memset(lpszBuffer,0,sEntManagerUrl.size()+1);
					strcpy(lpszBuffer,sEntManagerUrl.c_str());
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS:
			{
				// 1=保存本机聊天记录；
				// 2=保存在线聊天记录；
				// 3=BOTH
				*pParameterValue = pManager->m_nSaveConversations;//&1)==1?1:0;
			}break;
		case EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID:
			{
				if (pManager->m_nGroupMsgSubId>0)
				{
					char * lpszBuffer = new char[24];
					sprintf(lpszBuffer,"%lld",pManager->m_nGroupMsgSubId);
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_FIND_APP_SUBID:
			{
				if (pManager->m_nFindAppSubId>0)
				{
					char * lpszBuffer = new char[24];
					sprintf(lpszBuffer,"%lld",pManager->m_nFindAppSubId);
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID:
			{
				if (pManager->m_nAutoOpenSubId>0)
				{
					char * lpszBuffer = new char[24];
					sprintf(lpszBuffer,"%lld",pManager->m_nAutoOpenSubId);
					*pParameterValue = (unsigned long)lpszBuffer;
				}
			}break;
		case EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTOHIDE_MAINFRAME_PC)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_HIDE_MAINFRAME:
			*pParameterValue = (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_HIDE_MAINFRAME_PC)==0?0:1;
			break;
		case EB_SYSTEM_PARAMETER_SYSTEM_ACCOUNT_FLAG:
			{
				if ((pManager->m_nOnlineExtData&0x1)!=0)
					*pParameterValue = 1;
				else if (!pManager->m_sEntManagerUrl.empty())
					*pParameterValue = 2;
				else
					*pParameterValue = 0;
			}break;
		default:
			break;
		}
	}
}
void CEBAppClient::EB_FreeSystemParameter(EB_SYSTEM_PARAMETER nParameter, unsigned long nParameterValue) const
{
	switch (nParameter)
	{
	case EB_SYSTEM_PARAMETER_DEFAULT_URL:
	case EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL:
	case EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX:
	case EB_SYSTEM_PARAMETER_FORGET_PWD_URL:
	case EB_SYSTEM_PARAMETER_REGISTER_URL:
	case EB_SYSTEM_PARAMETER_ENT_LOGO_URL:
	case EB_SYSTEM_PARAMETER_DEPLOY_ID:
	case EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID:
	case EB_SYSTEM_PARAMETER_PRODUCT_NAME:
	case EB_SYSTEM_PARAMETER_ENT_MANAGER_URL:
	case EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID:
	case EB_SYSTEM_PARAMETER_FIND_APP_SUBID:
	case EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID:
		{
			char * lpszBuffer = (char*)nParameterValue;
			if (lpszBuffer!=NULL)
				delete[] lpszBuffer;
		}break;
	default:
		break;
	}
}

int CEBAppClient::EB_LogonAppCenter(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->LogonAppCenter();
	}
	return -1;
}
void CEBAppClient::EB_LogoutAppCenter(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LogoutAppCenter();
	}
}
int CEBAppClient::EB_SendAPMsg(const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SendAPMsg(pAPMsgInfo,bSaveOffMsg);
	}
	return -1;
}
void CEBAppClient::EB_APMsgAck(mycp::bigint nMsgId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->APMsgAck(nMsgId);
	}
}

int CEBAppClient::EB_Register(const char* sAccount, const char* sPwd, const char* sUserName, const char* sEntName)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->Register(sAccount,sPwd,sUserName,sEntName);
	}
	return -1;
}
int CEBAppClient::EB_LogonByVisitor(const char* sReqCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_PC|EB_LOGON_TYPE_VISITOR);
		pLogonInfo->m_sReqCode = sReqCode;
		pLogonInfo->m_nLineState = EB_LINE_STATE_ONLINE_NEW;
		return pManager->Logon(pLogonInfo);
	}
	return -1;
}
int CEBAppClient::EB_LogonByAccount(const char * sAccount, const char * sPassword,
                                    const char* sReqCode, const char * sOAuthKey,
                                    EB_USER_LINE_STATE nNewLineState, EB_LOGON_TYPE logonType)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		//CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_ANDROID|EB_LOGON_TYPE_EMAIL, sAccount, sPassword);		// 测试
        int nLogonType = logonType;
		mycp::tstring sAccountTemp(sAccount);
		const bool bIsFullNumber = entboost::IsFullNumber(sAccountTemp.c_str(),sAccountTemp.size());
		if (!pManager->m_sAccountPrefix.empty() && !bIsFullNumber && sAccountTemp.find(pManager->m_sAccountPrefix)==std::string::npos)
		{
			sAccountTemp = pManager->m_sAccountPrefix+sAccountTemp;
		}
		//const mycp::tstring sAccountTemp(sAccount);
		mycp::bigint nUserId = 0;
		//if (!pManager->m_sAccountPrefix.empty())
		//{
		//	nLogonType |= EB_LOGON_TYPE_EMAIL;
		//}else
		{
			const std::string::size_type find = sAccountTemp.find("@");
			if (find!=std::string::npos || !bIsFullNumber)
			{
				// 邮箱帐号
				nLogonType |= EB_LOGON_TYPE_EMAIL;
				//}else if ((find=sAccountTemp.find("*",2))!=std::string::npos || sAccount.substr(0,3)=="900")
				//{
				//	// 900客服号码
				//	// 900X，或者xxxx*xxx，或者xxxxx*
				//	//printf("**** %s;find=%d\n",sQueryAccount.c_str(),find);
				//	nCustomerServiceId = cgc_atoi64(sAccount.c_str());
				//	if (find!=std::string::npos && sAccount.size()>find)
				//	{
				//		// 取出分机号码
				//		nCustomerServiceExt = atoi(sAccount.substr(find+1).c_str());
				//	}
			}else if (sAccountTemp.size()==11 && sAccountTemp.substr(0,1)=="1")
			{
				// 手机号码
				nLogonType |= EB_LOGON_TYPE_PHONE;
			}else if (bIsFullNumber)
			{
				// 用户ID
				nUserId = cgc_atoi64(sAccount);
				if (nUserId==0) return -1;
				nLogonType |= EB_LOGON_TYPE_UID;
			}else
			{
				return -1;
			}
		}
		//const int nLogonType = EB_LOGON_TYPE_PC|EB_LOGON_TYPE_EMAIL;
		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(nLogonType, sAccount, sPassword);
		pLogonInfo->m_nUserId = nUserId;
		pLogonInfo->m_sReqCode = sReqCode;
		if (sOAuthKey!=NULL)
			pLogonInfo->m_sOAuthKey = sOAuthKey;
		pLogonInfo->m_nLineState = nNewLineState;
		return pManager->Logon(pLogonInfo);
	}
	return -1;
}
int CEBAppClient::EB_LogonOAuth(eb::bigint nUserId, const char * sOAuthKey,EB_USER_LINE_STATE nNewLineState)
//int CEBAppClient::EB_LogonOAuth(const char * sAccount,EB_USER_LINE_STATE nNewLineState)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		//CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_PC|EB_LOGON_TYPE_EMAIL|EB_LOGON_TYPE_OAUTH,sAccount,"");
		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_PC|EB_LOGON_TYPE_EMAIL|EB_LOGON_TYPE_OAUTH);
		pLogonInfo->m_nUserId = nUserId;
		if (sOAuthKey!=NULL)
			pLogonInfo->m_sOAuthKey = sOAuthKey;
		pLogonInfo->m_nLineState = nNewLineState;
		return pManager->Logon(pLogonInfo);
	}
	return -1;
}
int CEBAppClient::EB_ReLogon(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ReLogon();
		//if (EB_IsLogonVisitor())
		//{
		//	return EB_LogonByVisitor();
		//}else
		//{
		//	EB_USER_LINE_STATE nLineState = pManager->m_pMyAccountInfo->GetLineState();
		//	return EB_LogonByAccount(pManager->m_pMyAccountInfo->GetAccount().c_str(),pManager->GetUserPassword().c_str(),"",nLineState);
		//}
	}
	return -1;
}

bool CEBAppClient::EB_IsLogoned(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetUserStatus() == CUserManagerApp::US_Logoned;
	}
	return false;
}
void CEBAppClient::EB_LoadInfo(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadInfo();
	}
}
void CEBAppClient::EB_Logout(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->Logout();
	}
}

int CEBAppClient::EB_CheckVersion(const char* sClientVersion)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CheckVersion(sClientVersion);
	}
	return -1;
}

int CEBAppClient::EB_GetAreaInfo(int nParent,unsigned int nParameter)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAreaInfo(nParent, nParameter);
	}
	return -1;
}

mycp::bigint EBGetFileSize(const char* sFilePath)
{
#ifdef _QT_MAKE_
    QFile file(sFilePath);
    if ( !file.open(QFile::ReadOnly) ) {
        return -1;
    }
    const mycp::bigint nFileSize = file.size();
    file.close();
#else
	mycp::bigint nFileSize = 0;
	FILE * f = fopen(sFilePath, "rb");
    if (f==0) {
		return -1;
	}
#ifdef WIN32
	_fseeki64(f, 0, SEEK_END);
	nFileSize = _ftelli64(f);
#else
	fseeko(f, 0, SEEK_END);
	nFileSize = ftello(f);
#endif
	fclose(f);
#endif
	return nFileSize;
}

int CEBAppClient::EB_SetMyGroupHeadFile(eb::bigint nGroupId, const EBFileString &sImagePath)
{
#ifdef _QT_MAKE_
    return EB_SetMyGroupHeadFile(nGroupId, sImagePath.toStdString().c_str());
#else
    return EB_SetMyGroupHeadFile(nGroupId, sImagePath.c_str());
#endif
}

int CEBAppClient::EB_SetMyGroupHeadFile(eb::bigint nGroupId,const char* sFilePath)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		const mycp::bigint nFileSize = EBGetFileSize(sFilePath);
		if (nFileSize == -1)
		{
			return -1;
		}else 	if (nFileSize>500*1024)	// 500KB
		{
			return 1;
		}
		return pManager->SetDepHead(nGroupId,sFilePath);
	}
	return -1;
}
int CEBAppClient::EB_SetMyGroupHeadRes(eb::bigint nGroupId,eb::bigint sResId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetDepResHead(nGroupId,sResId);
	}
	return -1;
}

int CEBAppClient::EB_LoadRes(eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		const EB_RESOURCE_FROM_TYPE nFromType = nGroupId>0?EB_RESOURCE_FROM_TYPE_GROUP:EB_RESOURCE_FROM_TYPE_USER;
		if (nGroupId>0)
		{
			if (!pManager->theDepartmentList.exist(nGroupId)) return -1;
		}
		return pManager->LoadRes(1,0,0,nGroupId,nFromType,-1);
	}
	return -1;
}
bool CEBAppClient::EB_CanEditGroupRes(eb::bigint nGroupId, eb::bigint nResourceId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		if ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_RES_EDIT) == EB_LEVEL_DEP_RES_EDIT) return true;
		CEBResourceInfo::pointer pResourceInfo;
		if (nResourceId>0 && pManager->theResourceList.find(nResourceId,pResourceInfo) && pResourceInfo->m_nCreateUserId==pManager->m_pMyAccountInfo->GetUserId())
		{
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_CanDeleteGroupRes(eb::bigint nGroupId, eb::bigint nResourceId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		if ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_RES_DELETE) == EB_LEVEL_DEP_RES_DELETE) return true;
		CEBResourceInfo::pointer pResourceInfo;
		if (nResourceId>0 && pManager->theResourceList.find(nResourceId,pResourceInfo) && pResourceInfo->m_nCreateUserId==pManager->m_pMyAccountInfo->GetUserId())
		{
			return true;
		}
	}
	return false;
}
void CEBAppClient::EB_GetDirAllRes(eb::bigint sParentResId,std::vector<EB_ResourceInfo>& pOutResourceList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		//if (!pManager->theDepartmentList.exist(sInGroupCode)) return;
		BoostReadLock rdlock(pManager->theResourceList.mutex());
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = pManager->theResourceList.begin();
		for (; pIter!=pManager->theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if (pResourceInfo->m_sParentResId == sParentResId)
			{
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			}
		}
	}
}
void CEBAppClient::EB_GetNotDirAllRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		BoostReadLock rdlock(pManager->theResourceList.mutex());
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = pManager->theResourceList.begin();
		for (; pIter!=pManager->theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if (pResourceInfo->m_sParentResId>0 || pResourceInfo->m_nResType==EB_RESOURCE_DIR) continue;
			if (nGroupId==0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_USER)						// 个人
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			else if (nGroupId>0 && pResourceInfo->m_nFromId == nGroupId && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)	// 群组
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//if (nGroupId==0 && pResourceInfo->m_sGroupCode==0 && pResourceInfo->m_sEnterpriseCode==0)	// 个人
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//else if (nGroupId>0 && pResourceInfo->m_sGroupCode == nGroupId)									// 群组
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
		}
	}
}
bool CEBAppClient::EB_HasSubRes(eb::bigint sParentResId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->HasSubRes(sParentResId);
	}
	return false;
}
void CEBAppClient::EB_GetAllRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId>0)
		{
			if (!pManager->theDepartmentList.exist(nGroupId)) return;
		}
		BoostReadLock rdlock(pManager->theResourceList.mutex());
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = pManager->theResourceList.begin();
		for (; pIter!=pManager->theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if (nGroupId==0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_USER)						// 个人
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			else if (nGroupId>0 && pResourceInfo->m_nFromId == nGroupId && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)	// 群组
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));

			//if (nGroupId==0 && pResourceInfo->m_sGroupCode==0 && pResourceInfo->m_sEnterpriseCode==0)	// 个人
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//else if (nGroupId>0 && pResourceInfo->m_sGroupCode == nGroupId)									// 群组
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
		}
	}
}
void CEBAppClient::EB_GetAllFileRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId>0)
		{
			if (!pManager->theDepartmentList.exist(nGroupId)) return;
		}
		BoostReadLock rdlock(pManager->theResourceList.mutex());
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = pManager->theResourceList.begin();
		for (; pIter!=pManager->theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if (pResourceInfo->m_nResType != EB_RESOURCE_FILE) continue;
			if (nGroupId==0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_USER)						// 个人
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			else if (nGroupId>0 && pResourceInfo->m_nFromId == nGroupId && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)	// 群组
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//if (nGroupId==0 && pResourceInfo->m_sGroupCode==0 && pResourceInfo->m_sEnterpriseCode==0)	// 个人
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//else if (nGroupId>0 && pResourceInfo->m_sGroupCode == nGroupId)									// 群组
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
		}
	}
}
void CEBAppClient::EB_GetAllNoteRes(std::vector<EB_ResourceInfo>& pOutResourceList,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId>0)
			if (!pManager->theDepartmentList.exist(nGroupId)) return;
		BoostReadLock rdlock(pManager->theResourceList.mutex());
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = pManager->theResourceList.begin();
		for (; pIter!=pManager->theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if (pResourceInfo->m_nResType != EB_RESOURCE_NOTE) continue;
			if (nGroupId==0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_USER)						// 个人
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			else if (nGroupId>0 && pResourceInfo->m_nFromId == nGroupId && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)	// 群组
				pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//if (nGroupId==0 && pResourceInfo->m_sGroupCode==0 && pResourceInfo->m_sEnterpriseCode==0)	// 个人
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
			//else if (nGroupId>0 && pResourceInfo->m_sGroupCode == nGroupId)									// 群组
			//	pOutResourceList.push_back(EB_ResourceInfo(pResourceInfo.get()));
		}
	}
}
int CEBAppClient::EB_AddDirRes(const char* sDirName,eb::bigint sParentResId,eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sDirName==NULL || strlen(sDirName)==0) return -1;
		const EB_RESOURCE_FROM_TYPE nFromType = nGroupId>0?EB_RESOURCE_FROM_TYPE_GROUP:EB_RESOURCE_FROM_TYPE_USER;
		if (nGroupId > 0)
		{
			if (!EB_CanEditGroupRes(nGroupId,0)) return -1;
			if (!pManager->theDepartmentList.exist(nGroupId)) return -1;
		}
		return pManager->AddDirRes(sDirName,sParentResId,nGroupId,nFromType);
		//return pManager->AddDirRes(sDirName,sParentResId,nGroupId,0);
	}
	return -1;
}
int CEBAppClient::EB_AddFileRes(const char* sFilePath,const char* sFileName,const char* sDescription,eb::bigint sParentResId,eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		const EB_RESOURCE_FROM_TYPE nFromType = nGroupId>0?EB_RESOURCE_FROM_TYPE_GROUP:EB_RESOURCE_FROM_TYPE_USER;
		if (nGroupId > 0)
		{
			if (!EB_CanEditGroupRes(nGroupId,0)) return -1;
			if (!pManager->theDepartmentList.exist(nGroupId)) return -1;
		}
		// **后台会限制
//		FILE * f = fopen(sFilePath, "rb");
//		if (f == NULL)
//		{
//			return -1;
//		}
//#ifdef WIN32
//		_fseeki64(f, 0, SEEK_END);
//		const mycp::bigint nFileSize = _ftelli64(f);
//#else
//		fseeko(f, 0, SEEK_END);
//		const mycp::bigint nFileSize = ftello(f);
//#endif
//		fclose(f);
//		if (nFileSize >= 100*1024*1024)
//		{
//			// 暂时限制低于100MB
//			return 1;
//		}
		mycp::tstring sName(sFileName);
		if (sName=="")
		{
			sName = sFilePath;
			sName = sName.substr(sName.rfind('\\')+1);
		}
		return pManager->AddFileRes(sName,sFilePath,sDescription,sParentResId,EB_RESOURCE_SHARE_UNKNOWN,nGroupId,nFromType);
		//return pManager->AddFileRes(sName,sFilePath,sDescription,sParentResId,nGroupId,0,EB_RESOURCE_SHARE_UNKNOWN);
	}
	return -1;
}
int CEBAppClient::EB_AddTempGroupFileRes(const char* sFilePath,const char* sFileName,eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId == 0)
		{
			return -1;
		}
//		FILE * f = fopen(sFilePath, "rb");
//		if (f == NULL)
//		{
//			return -1;
//		}
//#ifdef WIN32
//		_fseeki64(f, 0, SEEK_END);
//		const mycp::bigint nFileSize = _ftelli64(f);
//#else
//		fseeko(f, 0, SEEK_END);
//		const mycp::bigint nFileSize = ftello(f);
//#endif
//		fclose(f);
//		if (nFileSize >= 100*1024*1024)
//		{
//			// 暂时限制低于100MB
//			return 1;
//		}
		mycp::tstring sName(sFileName);
        if (sName.empty()) {
			sName = sFilePath;
            std::string::size_type find = sName.rfind('\\');
            if (find==std::string::npos)
                find = sName.rfind('/');
            if (find!=std::string::npos)
                sName = sName.substr(find+1);
		}
		return pManager->AddFileRes(sName,sFilePath,"",0,EB_RESOURCE_SHARE_TEMP,nGroupId,EB_RESOURCE_FROM_TYPE_GROUP);
		//return pManager->AddFileRes(sName,sFilePath,"",0,nGroupId,0,EB_RESOURCE_SHARE_TEMP);
	}
	return -1;
}

int CEBAppClient::EB_AddNoteRes(const char* sNoteName,const char* sDescription,eb::bigint sParentResId,eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sNoteName==NULL || strlen(sNoteName)==0) return -1;
		if (nGroupId > 0)
		{
			if (!EB_CanEditGroupRes(nGroupId,0)) return -1;
			if (!pManager->theDepartmentList.exist(nGroupId)) return -1;
		}
		return pManager->AddNoteRes(sNoteName,sDescription,sParentResId,nGroupId,EB_RESOURCE_FROM_TYPE_GROUP);
		//return pManager->AddNoteRes(sNoteName,sDescription,sParentResId,nGroupId,0);
	}
	return -1;
}
int CEBAppClient::EB_EditRes(eb::bigint sResId,const char* sName,const char* sDescription)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (!pManager->theResourceList.find(sResId,pResourceInfo)) return -1;
		if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
		{
			if (!EB_CanEditGroupRes(pResourceInfo->m_nFromId,sResId)) return -1;
		//}else
		//{
		//	// ?
		//	return -1;
		}
		return pManager->EditRes(sResId,sName,sDescription);
	}
	return -1;
}
int CEBAppClient::EB_MoveRes2Dir(eb::bigint sResId,eb::bigint sParentResId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (!pManager->theResourceList.find(sResId,pResourceInfo)) return -1;
		if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
		{
			if (!EB_CanEditGroupRes(pResourceInfo->m_nFromId,sResId)) return -1;
		//}else
		//{
		//	// ?
		//	return -1;
		}
		return pManager->MoveRes2Dir(sResId,sParentResId);
	}
	return -1;
}

int CEBAppClient::EB_DeleteRes(eb::bigint sResId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (!pManager->theResourceList.find(sResId,pResourceInfo)) return -1;
		if (pResourceInfo->m_nResType == EB_RESOURCE_DIR)
		{
			if (EB_HasSubRes(sResId)) return 1;	// 不能删除非空目录
		}
		if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
		{
			if (!EB_CanDeleteGroupRes(pResourceInfo->m_nFromId,sResId)) return -1;
		//}else
		//{
		//	// ?
		//	return -1;
		}
		return pManager->DeleteRes(sResId);
	}
	return -1;
}
bool CEBAppClient::EB_GetResourceInfo(eb::bigint sResId,EB_ResourceInfo* pOutResourceInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (pManager->theResourceList.find(sResId,pResourceInfo))
		{
			pOutResourceInfo->operator =(pResourceInfo.get());
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetResourceCmInfo(eb::bigint sResId,mycp::tstring& pOutResourceInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		const mycp::bigint sResourceId(sResId);
		CEBResourceInfo::pointer pResourceInfo;
		if (pManager->theResourceList.find(sResourceId,pResourceInfo))
		{
			char lpszResourceInfo[200];
			sprintf(lpszResourceInfo,"%lld;%s;%s;%s;%d",pResourceInfo->m_sResId,pResourceInfo->m_sCmServer.c_str(),pResourceInfo->m_sCmAppName.c_str(),pResourceInfo->m_sDescription.substr(0,50).c_str(),pResourceInfo->m_nCmServiceId);
			pOutResourceInfo = lpszResourceInfo;
			return true;
		}
		//
		BoostReadLock rdlock(pManager->theEmotionList.mutex());
		CLockMap<mycp::tstring,CEBEmotionInfo::pointer>::iterator pIter = pManager->theEmotionList.begin();
		for (;pIter!=pManager->theEmotionList.end();pIter++)
		{
			const CEBEmotionInfo::pointer& pEmotionInfo = pIter->second;
			if (pEmotionInfo->m_sResId == sResourceId)
			{
				char lpszResourceInfo[200];
				sprintf(lpszResourceInfo,"%lld;%s;%s;%s;%d",pEmotionInfo->m_sResId,pEmotionInfo->m_sCmServer.c_str(),pEmotionInfo->m_sCmAppName.c_str(),pEmotionInfo->m_sDescription.substr(0,50).c_str(),pEmotionInfo->m_nCmServiceId);
				pOutResourceInfo = lpszResourceInfo;
				return true;
			}
		}
	}
    return false;
}

int CEBAppClient::EB_DownloadFileRes(eb::bigint sResId, const EBFileString &sSaveTo)
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        return pManager->DownloadFileRes(sResId,sSaveTo);
    }
    return -1;
}
int CEBAppClient::EB_DownloadFileRes(eb::bigint sResId,const char *sSaveTo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DownloadFileRes(sResId,sSaveTo);
	}
	return -1;
}
int CEBAppClient::EB_CancelFileRes(eb::bigint sResId,mycp::bigint nMsgId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CancelFileRes(sResId,nMsgId);
	}
	return -1;
}

bool CEBAppClient::EB_GetMyAccountInfo(EB_AccountInfo* pOutAccountInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->m_pMyAccountInfo.get()==NULL) return false;
		pOutAccountInfo->operator =(pManager->m_pMyAccountInfo.get());
		return true;
	}
	return false;
}

bool CEBAppClient::EB_SetMyAccountInfo(const EB_AccountInfo* pInAccountInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPopParameterList pParameter;
		pParameter.SetParameter(CGC_PARAMETER("user-name",pInAccountInfo->GetUserName()));
		pParameter.SetParameter(CGC_PARAMETER("desc",pInAccountInfo->GetDescription()));
		pParameter.SetParameter(CGC_PARAMETER("area1",pInAccountInfo->GetArea1()));
		pParameter.SetParameter(CGC_PARAMETER("area2",pInAccountInfo->GetArea2()));
		pParameter.SetParameter(CGC_PARAMETER("area3",pInAccountInfo->GetArea3()));
		pParameter.SetParameter(CGC_PARAMETER("area4",pInAccountInfo->GetArea4()));
		pParameter.SetParameter(CGC_PARAMETER("add",pInAccountInfo->GetAddress()));
		pParameter.SetParameter(CGC_PARAMETER("url",pInAccountInfo->GetUrl()));
		pParameter.SetParameter(CGC_PARAMETER("gender",(int)pInAccountInfo->GetGender()));
		pParameter.SetParameter(CGC_PARAMETER("tel",pInAccountInfo->GetTel()));
		pParameter.SetParameter(CGC_PARAMETER("mobile",pInAccountInfo->GetMobile()));
		pParameter.SetParameter(CGC_PARAMETER("email",pInAccountInfo->GetEmail()));
		pParameter.SetParameter(CGC_PARAMETER("birthday",pInAccountInfo->GetBirthday()));
		pParameter.SetParameter(CGC_PARAMETER("zipcode",pInAccountInfo->GetZipCode()));
		return pManager->UpdateInfo(pParameter);
	}
	return false;
}

bool CEBAppClient::EB_GetMyDefaultMemberCode(eb::bigint& pOutDefaultMemberCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->m_pMyAccountInfo.get()==NULL) return false;
		pOutDefaultMemberCode = pManager->m_pMyAccountInfo->GetDefaultEmpId();
		return true;
	}
	return false;
}
bool CEBAppClient::EB_GetMyDefaultMemberInfo(EB_MemberInfo* pOutmemberInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->m_pDefaultEntEmployeeInfo.get()==NULL) return false;
		pOutmemberInfo->operator = (pManager->m_pDefaultEntEmployeeInfo.get());
		return true;
	}
	return false;
}
EBFileString CEBAppClient::EB_GetMyDefaultMemberHeadFile(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->m_pDefaultEntEmployeeInfo.get()==NULL) return "";
		return pManager->m_pDefaultEntEmployeeInfo->m_sHeadResourceFile;
	}
	return "";
}
bool CEBAppClient::EB_SetMyDefaultMemberCode(eb::bigint sNewDefaultMemberCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetDefaltMemberCode(sNewDefaultMemberCode);
	}
	return false;
}
bool CEBAppClient::EB_CanEditEnterprise(eb::bigint nEnterpriseCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->IsEnterpriseCreator(nEnterpriseCode)) return true;	// 企业创建者
		//std::vector<int> pManagerLevel;
		//pManager->GetEntManagerLevel(sEnterpriseCode,pManagerLevel);
		//bool bAllow = false;
		//for (size_t i=0;i<pManagerLevel.size();i++)
		//{
		//	int nManagerLevel = pManagerLevel[i];
		//	if ((nManagerLevel & (int)EB_LEVEL_ENT_EDIT) == (int)EB_LEVEL_ENT_EDIT)
		//	{
		//		bAllow = true;
		//		break;
		//	}
		//}
		//return bAllow;
	}
	return false;
}
bool CEBAppClient::EB_CanEditGroupInfo(eb::bigint nEnterpriseCode,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId==0)
		{
			if (nEnterpriseCode==0) return true;				// 个人新建群组权限
			if (pManager->IsEnterpriseCreator(nEnterpriseCode)) return true;					// 企业创建者，有新建部门权限
		}
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_EDIT) == EB_LEVEL_DEP_EDIT);
	}
	return false;
}
bool CEBAppClient::EB_CanDeleteGroupInfo(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE);
	}
	return false;
}
bool CEBAppClient::EB_CanDeleteGroupInfo(eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (nMemberUserId==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode,nMemberUserId)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE);
	}
	return false;
}
bool CEBAppClient::EB_IsGroupCreator(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode,pManager->m_pMyAccountInfo->GetUserId())) return true;	// 企业创建者
		}
		//CEBMemberInfo::pointer pMyEmployeeInfo;
		//if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		//return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE);
	}
	return false;
}
bool CEBAppClient::EB_IsGroupCreator(eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (nMemberUserId==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode,nMemberUserId)) return true;	// 企业创建者
		}
		//CEBMemberInfo::pointer pMyEmployeeInfo;
		//if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		//return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE);
	}
	return false;
}
bool CEBAppClient::EB_IsGroupManager(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nManagerUserId) return true;	// 部门创建者
		//if (pDepartmentInfo->m_sEnterpriseCode>0)
		//{
		//	// 企业部门
		//	if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode,pManager->m_pMyAccountInfo->GetUserId())) return true;	// 企业创建者
		//}
	}
	return false;
}
bool CEBAppClient::EB_IsGroupManager(eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (nMemberUserId==pDepartmentInfo->m_nManagerUserId) return true;	// 部门创建者
		//if (pDepartmentInfo->m_sEnterpriseCode>0)
		//{
		//	// 企业部门
		//	if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode,nMemberUserId)) return true;	// 企业创建者
		//}
	}
	return false;
}

//bool CEBAppClient::EB_CanDeleteGroupInfo(int nManagerLevel) const
//{
//	return ((nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE);
//}

bool CEBAppClient::EB_CanEditMemberInfo(eb::bigint nGroupId,eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (pManager->m_pMyAccountInfo->GetUserId()==nMemberUserId) return true;			// 修改自己帐号
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_EMP_EDIT) == EB_LEVEL_EMP_EDIT);
	}
	return false;
}
bool CEBAppClient::EB_CanEditMyBaseMemberInfo(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (EB_IsGroupAdminLevel(nGroupId)) return true;
		const mycp::bigint nMeMemberUserId = pManager->m_pMyAccountInfo->GetUserId();
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (nMeMemberUserId==pDepartmentInfo->m_nCreateUserId) return true;					// 部门创建者
		if (pDepartmentInfo->m_sEnterpriseCode==0) return true;								// 个人群组
		if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者

		if (pManager->m_pDefaultEnterpriseInfo.get()==NULL ||
			pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode!=pDepartmentInfo->m_sEnterpriseCode) return false;
		const int nEntExt = pManager->m_pDefaultEnterpriseInfo->m_nEntExt;
		return (nEntExt&EB_ENT_EXT_DATA_ENABLE_MODIFY_MEMBER_INFO)==0?false:true;			// 判断企业是否配置了允许修改部门成员名片
	}
	return false;
}
bool CEBAppClient::EB_CanEditMyBaseAccountInfo(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		//if (EB_IsGroupAdminLevel(nGroupId)) return true;
		//const mycp::bigint nMeMemberUserId = pManager->m_pMyAccountInfo->GetUserId();
		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		//if (nMeMemberUserId==pDepartmentInfo->m_nCreateUserId) return true;					// 部门创建者
		//if (pDepartmentInfo->m_sEnterpriseCode==0) return true;								// 个人群组
		//if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者

		//if (pManager->m_pDefaultEnterpriseInfo.get()==NULL ||
		//	pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode!=pDepartmentInfo->m_sEnterpriseCode) return false;
		if (pManager->m_pDefaultEnterpriseInfo.get()==NULL || pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode==0)
			return (pManager->m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_ACCOUNT_EDIT)==0?true:false;
		const int nEntExt = pManager->m_pDefaultEnterpriseInfo->m_nEntExt;
		return (nEntExt&EB_ENT_EXT_DATA_DISABLE_MODIFY_ACCOUNT_INFO)==0?true:false;			// 判断企业是否配置了允许修改个人资料
	}
	return false;
}
bool CEBAppClient::EB_CanDeleteMemberInfo(eb::bigint nGroupId,eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_GROUP || pDepartmentInfo->m_nGroupType==EB_GROUP_TYPE_TEMP)
		{
			// 个人群组，临时讨论组，可以删除自己（等于退出群组）
			if (pManager->m_pMyAccountInfo->GetUserId()==nMemberUserId) return true;	// 修改自己帐号
		}
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_EMP_DELETE) == EB_LEVEL_EMP_DELETE);
	}
	return false;
}
bool CEBAppClient::EB_CanDeleteMemberInfo(eb::bigint nMemberId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBMemberInfo::pointer pEmployeeInfo;
		if (!pManager->theEmployeeList.find(nMemberId, pEmployeeInfo)) return false;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo)) return false;
		if (pManager->m_pMyAccountInfo->GetUserId()==pDepartmentInfo->m_nCreateUserId) return true;	// 部门创建者
		if (pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_GROUP || pDepartmentInfo->m_nGroupType==EB_GROUP_TYPE_TEMP)
		{
			// 个人群组，临时讨论组，可以删除自己（等于退出群组）
			if (pManager->m_pMyAccountInfo->GetUserId()==pEmployeeInfo->m_nMemberUserId) return true;	// 删除自己帐号
		}
		if (pDepartmentInfo->m_sEnterpriseCode>0)
		{
			// 企业部门
			if (pManager->IsEnterpriseCreator(pDepartmentInfo->m_sEnterpriseCode)) return true;	// 企业创建者
		}
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(pManager->m_pMyAccountInfo->GetUserId(), pMyEmployeeInfo)) return false;
		return ((pMyEmployeeInfo->m_nManagerLevel & (int)EB_LEVEL_EMP_DELETE) == EB_LEVEL_EMP_DELETE);
	}
	return false;
}
int CEBAppClient::EB_GetMyAccountSetting(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->m_pMyAccountInfo.get()==NULL?0:pManager->m_pMyAccountInfo->GetSetting();
	}
	return 0;
}
bool CEBAppClient::EB_SetMyAccountSetting(int nNewSetting)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPopParameterList pParameter;
		pParameter.SetParameter(CGC_PARAMETER("setting",nNewSetting));
		return pManager->UpdateInfo(pParameter);
	}
	return false;
}

mycp::tstring CEBAppClient::EB_GetLogonAccount(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->m_pMyAccountInfo->GetAccount();
	}
	return "";
}
//bool CEBAppClient::EB_GetLogonAccount(mycp::tstring& pOutAccount) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		pOutAccount = pManager->m_pMyAccountInfo->GetAccount();
//		return true;
//	}
//	return false;
//}

eb::bigint CEBAppClient::EB_GetLogonUserId(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetUserId();
	}
	return 0;
}

mycp::tstring CEBAppClient::EB_GetDescription(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->GetDescription();
	}
	return "";
}
bool CEBAppClient::EB_SetDescription(const char* sNewDescription)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPopParameterList pParameter;
		if (sNewDescription == NULL)
			pParameter.SetParameter(CGC_PARAMETER("desc",(mycp::tstring)""));
		else
			pParameter.SetParameter(CGC_PARAMETER("desc",(mycp::tstring)sNewDescription));
		return pManager->UpdateInfo(pParameter);
	}
	return false;
}
//
//void CEBAppClient::EB_GetLogonInfo(mycp::tstring& sOutUserName, mycp::tstring& sOutPassword) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
//	{
//		sOutUserName = pManager->m_pMyAccountInfo->GetUserName();
//		if (sOutUserName.empty())
//			sOutUserName = pManager->m_pMyAccountInfo->GetAccount();
//		sOutPassword = pManager->m_pMyAccountInfo->GetPasswd();
//	}
//}
mycp::tstring CEBAppClient::EB_GetUserName(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		mycp::tstring sOutUserName = pManager->m_pMyAccountInfo->GetUserName();
		if (sOutUserName.empty())
			sOutUserName = pManager->m_pMyAccountInfo->GetAccount();
		return sOutUserName;
	}
	return "";
}
bool CEBAppClient::EB_SetUserName(const char* sNewUserName)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sNewUserName == NULL || strlen(sNewUserName) == 0) return false;
		CPopParameterList pParameter;
		pParameter.SetParameter(CGC_PARAMETER("user-name",(mycp::tstring)sNewUserName));
		return pManager->UpdateInfo(pParameter);
	}
	return false;
}

mycp::tstring CEBAppClient::EB_GetPassword(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		//return pManager->GetUserPassword();
		return pManager->m_pMyAccountInfo->GetPasswd();
	}
	return "";
}
bool CEBAppClient::EB_SetPassword(const char* sOldPassword, const char* sNewPassword)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sOldPassword == NULL || strlen(sOldPassword) == 0) return false;
		if (sNewPassword == NULL || strlen(sNewPassword) == 0) return false;
		if (strstr(sNewPassword," ") != NULL) return false;	// 不能输入空格密码；
		CPopParameterList pParameter;
		pParameter.SetParameter(CGC_PARAMETER("old_pwd",(mycp::tstring)sOldPassword));
		//pParameter.SetParameter(CGC_PARAMETER("pwd",(mycp::tstring)sNewPassword));		// 不加密密码
		pParameter.SetParameter(CGC_PARAMETER("passwd",(mycp::tstring)sNewPassword));		// 要加密密码；
		return pManager->UpdateInfo(pParameter);
	}
	return false;
}

EB_USER_LINE_STATE CEBAppClient::EB_GetLineState(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->GetLineState();
	}
	return EB_LINE_STATE_UNKNOWN;
}

bool CEBAppClient::EB_IsLogonVisitor(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->IsLogonVisitor();
	}
	return false;
}

bool CEBAppClient::EB_SetLineState(EB_USER_LINE_STATE nNewLineState)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ChangeLineState(nNewLineState);
	}
	return false;
}
//bool CEBAppClient::EB_UpdateInfo(const CPopParameterList& pInfoList)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->UpdateInfo(pInfoList);
//	}
//	return false;
//}
//
//bool CEBAppClient::EB_EnterChat(eb::bigint nCallId, const char* sAccount)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->EnterChat(nCallId, sAccount);
//	}
//	return false;
//}
//bool CEBAppClient::EB_ExitChat(eb::bigint nCallId, const char* sAccount)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->ExitChat(nCallId, sAccount);
//	}
//	return false;
//}

int CEBAppClient::EB_CallMember(eb::bigint nMemberCode,eb::bigint nExistCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallMember(nMemberCode,nExistCallId,0,true);
	}
	return -1;
}
int CEBAppClient::EB_CallAccount(const char * sCallee,eb::bigint nExistCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallAccount(sCallee,nExistCallId,0,true);
	}
	return -1;
}
int CEBAppClient::EB_CallUserId(eb::bigint nToUserId,eb::bigint nExistCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallUserId(nToUserId,nExistCallId,0,true);
	}
	return -1;
}

int CEBAppClient::EB_Call2Group(eb::bigint nCallId,const char* sToAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->Call2Dep(nCallId,sToAccount);
	}
	return -1;
}
int CEBAppClient::EB_CallGroup(eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallGroup(nGroupId,true);
	}
	return -1;
}

int CEBAppClient::EB_CallAnswer(eb::bigint nCallId,bool bAccept)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallAck(nCallId,bAccept?EB_CAT_ACCEPT:EB_CAT_REJECT);
	}
	return -1;
}

int CEBAppClient::EB_CallHangup(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallHangup(nCallId);
	}
	return -1;
}
void CEBAppClient::EB_CallExit(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->ClearCallInfo(nCallId,false,false);
	}
}
void CEBAppClient::EB_CallNotify(eb::bigint nCallId, eb::bigint nToUserId, int nNotifyType, eb::bigint nNotifyId, const char* sNotifyData)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->CallNotify(nCallId, nToUserId, nNotifyType, nNotifyId, sNotifyData);
	}
}

//int CEBAppClient::EB_CheckReCall(eb::bigint nCallId,bool bOffReCall)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		//return pManager->InviteCall(nCallId,0,0,bOffReCall);
//		return 0;
//	}
//	return -1;
//}
void CEBAppClient::EB_RichBufferAddText(eb::bigint nCallId,const char* sText)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->RichBufferAddText(nCallId,sText);
	}
}
void CEBAppClient::EB_RichBufferAddObject(eb::bigint nCallId,const char* pData,unsigned long nSize)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->RichBufferAddObject(nCallId,pData,nSize);
	}
}
void CEBAppClient::EB_RichBufferAddResource(eb::bigint nCallId,const char* sResource)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->RichBufferAddResource(nCallId,sResource);
	}
}
void CEBAppClient::EB_RichBufferClear(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->RichBufferClear(nCallId);
	}
}
int CEBAppClient::EB_SendRichBuffer(eb::bigint nCallId,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SendCrRichBuffer(nCallId,nToUserId,bPrivate);
	}
	return -1;
}

int CEBAppClient::EB_SendText(eb::bigint nCallId,const char* sTextMsg,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		EB_ChatRoomRichMsg pRichMsg;
		pRichMsg.AddText(sTextMsg);
		return pManager->SendCrRich(nCallId,&pRichMsg,nToUserId,bPrivate);
	}
	return -1;
}
int CEBAppClient::EB_SendRich(eb::bigint nCallId,const EB_ChatRoomRichMsg* pRichMsg,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SendCrRich(nCallId,pRichMsg,nToUserId,bPrivate);
	}
	return -1;
}
int CEBAppClient::EB_SendUserCard(eb::bigint nCallId,const tstring& sCardUserAccount,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sCardUserAccount.empty()) return -2;
		return pManager->SendUserCard(nCallId,sCardUserAccount,nToUserId,bPrivate);
	}
	return -1;
}
int CEBAppClient::EB_SendUserCard(eb::bigint nCallId,eb::bigint nCardUserId,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nCardUserId==0) return -2;
		char lpszUserId[24];
		sprintf(lpszUserId,"%lld",nCardUserId);
		return pManager->SendUserCard(nCallId,lpszUserId,nToUserId,bPrivate);
	}
	return -1;
}
bool CEBAppClient::EB_ParseCardInfo(const tstring& sInCardInfoString,int& pOutCardType,tstring& pOutCardData)
{
	// [CARD_TYPE],[CARD_DATA]
	const std::string::size_type find = sInCardInfoString.find(",");
	if (find==std::string::npos) return false;
	pOutCardType = atoi(sInCardInfoString.substr(0,find).c_str());
	pOutCardData = sInCardInfoString.substr(find+1);
	return true;
}
bool CEBAppClient::EB_GetUserECardByFromInfo(const tstring& sInUserECardString, EB_ECardInfo* pOutUserECard)
{
	return pOutUserECard==NULL?false:CUserManagerApp::GetECardByFromInfo(sInUserECardString,pOutUserECard);
}
bool CEBAppClient::EB_GetUserECardByCardInfo(const tstring& sInUserECardString, EB_ECardInfo* pOutUserECard)
{
	int nCardType = 0;
	tstring sCardData;
	if (!EB_ParseCardInfo(sInUserECardString,nCardType,sCardData) || nCardType!=1) return false;
	return EB_GetUserECardByFromInfo(sCardData,pOutUserECard);
}

//int CEBAppClient::EB_SendCard(eb::bigint nCallId,int nCardType,const char* sCardData,eb::bigint nToUserId,bool bPrivate)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		if (sCardData==NULL) return -1;
//		EB_ChatRoomRichMsg pRichMsg;
//		pRichMsg.SetSubType(EB_RICH_SUB_TYPE_CARD_INFO);
//		const unsigned long nDataSize = strlen(sCardData);
//		char * pObjectData = new char[nDataSize+16];
//		const int n = sprintf(pObjectData,"%d,",nCardType);
//		memcpy(pObjectData+n,sCardData,nDataSize);
//		pRichMsg.AddObject(pObjectData,n+nDataSize);
//		return pManager->SendCrRich(nCallId,&pRichMsg,nToUserId,bPrivate);
//	}
//	return -1;
//}
int CEBAppClient::EB_SendMapPos(eb::bigint nCallId,const char* sMapPosData,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sMapPosData==NULL) return -1;
		EB_ChatRoomRichMsg pRichMsg;
		pRichMsg.SetSubType(EB_RICH_SUB_TYPE_MAP_POS);
		const unsigned long nDataSize = strlen(sMapPosData);
		char * pObjectData = new char[nDataSize+1];
		memcpy(pObjectData,sMapPosData,nDataSize);
		pRichMsg.AddObject(pObjectData,nDataSize);
		return pManager->SendCrRich(nCallId,&pRichMsg,nToUserId,bPrivate);
	}
	return -1;
}
int CEBAppClient::EB_SendUserData(eb::bigint nCallId,const char* sUserData,unsigned long nDataSize,eb::bigint nToUserId,bool bPrivate)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sUserData==NULL || nDataSize==0) return -1;
		EB_ChatRoomRichMsg pRichMsg;
		pRichMsg.SetSubType(EB_RICH_SUB_TYPE_USER_DATA);
		char * pObjectData = new char[nDataSize+1];
		memcpy(pObjectData,sUserData,nDataSize);
		pRichMsg.AddObject(pObjectData,nDataSize);
		return pManager->SendCrRich(nCallId,&pRichMsg,nToUserId,bPrivate);
	}
	return -1;
}
int CEBAppClient::EB_RequestWithdrawMsg(eb::bigint nCallId,eb::bigint nMsgId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->RequestWithdrawMsg(nCallId,nMsgId);
	}
	return -1;
}
int CEBAppClient::EB_RequestCollectMsg(eb::bigint nCallId,eb::bigint nMsgId, bool bGroupCollection)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->RequestCollectMsg(nCallId,nMsgId,bGroupCollection);
	}
    return -1;
}

int CEBAppClient::EB_SendFile(eb::bigint nCallId, const EBFileString &sFilePath, eb::bigint nToUserId, bool bPrivate, bool bOffFile)
{
#ifdef _QT_MAKE_
    return EB_SendFile(nCallId, sFilePath.toStdString().c_str(), nToUserId, bPrivate, bOffFile);
#else
    return EB_SendFile(nCallId, sFilePath.c_str(), nToUserId, bPrivate, bOffFile);
#endif
}

int CEBAppClient::EB_SendFile(eb::bigint nCallId,const char* sFilePath,eb::bigint nToUserId,bool bPrivate,bool bOffFile)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
#ifdef _QT_MAKE_
        QFileInfo pFileInfo(sFilePath);
		if (!pFileInfo.exists()) return -1;
		if (pFileInfo.isDir()) return -2;
#else
		long hFile = 0;
		_finddata_t fileinfo;
		if ((hFile = _findfirst(sFilePath,&fileinfo)) == -1)
			return -1;
		_findclose(hFile);
		if (fileinfo.attrib & _A_SUBDIR)
			return -2;
#endif
		return pManager->SendCrFile(nCallId,sFilePath,nToUserId,bPrivate,bOffFile,false,NULL,true);
	}
    return -1;
}

int CEBAppClient::EB_AcceptFile(eb::bigint nCallId, eb::bigint nMsgId, const EBFileString &sSaveTo)
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        return pManager->AcceptCrFile(nCallId,nMsgId,sSaveTo);
    }
    return -1;
}
int CEBAppClient::EB_AcceptFile(eb::bigint nCallId,mycp::bigint nMsgId,const char * sSaveTo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->AcceptCrFile(nCallId,nMsgId,sSaveTo);
	}
	return -1;
}
int CEBAppClient::EB_Save2CloudDrive(eb::bigint nCallId,eb::bigint nMsgId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->Save2CloudDrive(nCallId,nMsgId);
	}
	return -1;
}

//int CEBAppClient::EB_AcceptFileRes(eb::bigint nCallId,const char * sResId,const char * sSaveTo)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->AcceptCrFileRes(nCallId,sResId,sSaveTo);
//	}
//	return -1;
//}

int CEBAppClient::EB_CancelFile(eb::bigint nCallId,mycp::bigint nMsgId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CancelCrFile(nCallId,nMsgId);
	}
	return -1;
}

bool CEBAppClient::EB_GetCallUserIdList(eb::bigint nCallId, std::vector<eb::bigint> & pOutUserList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallUserList(nCallId, pOutUserList);
	}
	return false;
}
bool CEBAppClient::EB_GetCallAccountList(eb::bigint nCallId, std::vector<mycp::tstring> & pOutUserList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallUserList(nCallId, pOutUserList);
	}
	return false;
}

//int CEBAppClient::EB_GetCallOnlineSize(eb::bigint nCallId) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->GetCallOnlineSize(nCallId);
//	}
//	return -1;
//}
bool CEBAppClient::EB_GetCallAccountInfoList(eb::bigint nCallId,std::vector<EB_AccountInfo>& pOutUserList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallUserList(nCallId, pOutUserList);
	}
	return false;
}

//CEBCallInfo::pointer CEBAppClient::EB_GetCallInfo(const mycp::tstring & nCallId)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->GetCallInfo(nCallId);
//	}
//	return NullCallInfo;
//}

bool CEBAppClient::EB_GetCallAccountInfo(eb::bigint nCallId, eb::bigint nUserId,EB_AccountInfo* pOutAccountInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBAccountInfo::pointer result = pManager->GetCallInfo(nCallId, nUserId);
		if (result.get() == NULL) return false;
		pOutAccountInfo->operator =(result.get());
		return true;
	}
	return false;
}
//
//CEbECardInfo::pointer CEBAppClient::EB_GetMyECardInfo(eb::bigint nMemberCode) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->GetMyECardInfo(nMemberCode);
//	}
//	return NullEbECardInfo;
//}
//mycp::tstring CEBAppClient::EB_GetMyECardFromInfo(eb::bigint nMemberCode) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		CEbECardInfo::pointer pECardInfo = pManager->GetMyECardInfo(nMemberCode);
//		return pManager->GetECardFromInfo(pECardInfo);
//	}
//	return "";
//}
//CEbECardInfo::pointer CEBAppClient::EB_GetECardByFromInfo(const char * sFromInfo) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->GetECardByFromInfo(sFromInfo);
//	}
//	return NullEbECardInfo;
//}
//bool CEBAppClient::EB_GetAccountInfoByFromInfo(const char * sFromInfo, CEBAccountInfo::pointer & pAccountInfo) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->GetAccountInfoByFromInfo(sFromInfo, pAccountInfo);
//	}
//	return false;
//}

int CEBAppClient::EB_LoadContact(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		// *** 先加载联系人分组
		// *** 然后再根据本地是否有联系人缓存数据，再决定是否加载联系人数据
		return pManager->ContactQuery();
	}
	return -1;
}

int CEBAppClient::EB_EditUGInfo(eb::bigint nUGId, const char* sGroupName)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sGroupName==NULL || strlen(sGroupName)==0) return -1;
		return pManager->UGInfoEit(nUGId, sGroupName);
	}
	return -1;
}

int CEBAppClient::EB_DeleteUGInfo(eb::bigint nUGId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->UGInfoDelete(nUGId);
	}
	return -1;
}
int CEBAppClient::EB_GetUGContactSize(eb::bigint nUGId, int& pOutContactSize, int& pOutOnlineSize)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetUGContactSize(nUGId, pOutContactSize, pOutOnlineSize);
	}
	return -1;
}
void CEBAppClient::EB_GetUGInfoList(std::vector<EB_UGInfo>& pOutUGInfoList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		BoostReadLock rdlock(pManager->theUGInfoList.mutex());
		CLockMap<mycp::bigint, CEBUGInfo::pointer>::iterator pIter;
		for (pIter=pManager->theUGInfoList.begin(); pIter!=pManager->theUGInfoList.end(); pIter++)
		{
			const CEBUGInfo::pointer& pUGInfo = pIter->second;
			pOutUGInfoList.push_back(EB_UGInfo(pUGInfo.get()));
		}
	}
}

int CEBAppClient::EB_ReqAddContact(eb::bigint nContactUserId,const char * sDescription)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		EB_ContactInfo pContactInfo;
		pContactInfo.m_nContactUserId = nContactUserId;
		pContactInfo.m_sDescription = sDescription;
		return pManager->ContactEdit(&pContactInfo);
	}
	return -1;
}

int CEBAppClient::EB_EditContact(const EB_ContactInfo* pPopContactInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ContactEdit(pPopContactInfo);
	}
    return -1;
}

int CEBAppClient::EB_MoveContactTo1(eb::bigint contactId, eb::bigint ugId)
{
    EB_ContactInfo contactInfo;
    if (!EB_GetContactInfo1(contactId, &contactInfo)) {
        return -1;
    }
    contactInfo.m_nUGId = ugId;
    return EB_EditContact(&contactInfo);
}

int CEBAppClient::EB_MoveContactTo2(eb::bigint userId, eb::bigint ugId)
{
    EB_ContactInfo contactInfo;
    if (!EB_GetContactInfo2(userId, &contactInfo)) {
        return -1;
    }
    contactInfo.m_nUGId = ugId;
    return EB_EditContact(&contactInfo);
}
int CEBAppClient::EB_DeleteContact(eb::bigint nContactId,bool bDeleteDest)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ContactDelete(nContactId,bDeleteDest);
	}
	return -1;
}

bool CEBAppClient::EB_GetContactInfo1(eb::bigint nContactId,EB_ContactInfo* pOutContactInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBContactInfo::pointer pPopContactInfo;
		if (pManager->theContactList1.find(nContactId, pPopContactInfo))
		{
			pOutContactInfo->operator =(pPopContactInfo.get());
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetContactInfo2(eb::bigint nContactUserId,EB_ContactInfo* pOutContactInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pManager->GetContactInfo(nContactUserId,pOutContactInfo))
		{
			return true;
		}
	}
	return false;
}

bool CEBAppClient::EB_GetContactHeadInfoByContactId(eb::bigint nContactId, EBFileString &pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBContactInfo::pointer pPopContactInfo;
		if (pManager->theContactList1.find(nContactId, pPopContactInfo))
		{
			pOutHeadFile = pPopContactInfo->m_sHeadResourceFile;
			if (pManager->theResourceFilePath.exist(pPopContactInfo->m_sHeadResourceId))
				pOutHeadMd5 = "";
			else
				pOutHeadMd5 = pPopContactInfo->m_sHeadMd5;
			pOutLineState = pPopContactInfo->m_nLineState;
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetContactHeadInfoByUserId(eb::bigint nUserId, EBFileString &pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBContactInfo::pointer pPopContactInfo;
		if (pManager->theContactList2.find(nUserId, pPopContactInfo))
		{
			pOutHeadFile = pPopContactInfo->m_sHeadResourceFile;
			if (pManager->theResourceFilePath.exist(pPopContactInfo->m_sHeadResourceId))
				pOutHeadMd5 = "";
			else
				pOutHeadMd5 = pPopContactInfo->m_sHeadMd5;
			pOutLineState = pPopContactInfo->m_nLineState;
			return true;
		}
	}
	return false;
}

bool CEBAppClient::EB_IsMyContactAccount1(mycp::bigint nContactId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->theContactList1.exist(nContactId);
	}
	return false;
}
bool CEBAppClient::EB_IsMyContactAccount2(mycp::bigint nContactUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->IsMyContact(nContactUserId);
	}
	return false;
}

//
//EB_ContactInfo::pointer CEBAppClient::EB_GetContactInfo(const EB_ContactInfo* pInPopContactInfo) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		if (pInPopContactInfo == NULL) return NullPopContactInfo;
//		boost::mutex::scoped_lock lock(pManager->theContactList.mutex());
//		CLockMap<mycp::tstring, EB_ContactInfo::pointer>::iterator pIter;
//		for (pIter=pManager->theContactList.begin(); pIter!=pManager->theContactList.end(); pIter++)
//		{
//			EB_ContactInfo::pointer pPopContactInfo = pIter->second;
//			if (pPopContactInfo.get() == pInPopContactInfo)
//			{
//				return pPopContactInfo;
//			}
//		}
//	}
//	return NullPopContactInfo;
//}

void CEBAppClient::EB_GetContactList(std::vector<EB_ContactInfo> & pOutContactList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		BoostReadLock rdlock(pManager->theContactList1.mutex());
		CLockMap<mycp::bigint, CEBContactInfo::pointer>::iterator pIter;
		for (pIter=pManager->theContactList1.begin(); pIter!=pManager->theContactList1.end(); pIter++)
		{
			const CEBContactInfo::pointer& pPopContactInfo = pIter->second;
			pOutContactList.push_back(EB_ContactInfo(pPopContactInfo.get()));
		}
	}
}

int CEBAppClient::EB_LoadOrg(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		// **会加载企业信息和加载表情；
		return pManager->EnterpriseLoad(0,0,0,0,0,0,true,"",true);
		//return pManager->EnterpriseLoad(0,1,1,0);
	}
	return -1;
}
int CEBAppClient::EB_LoadGroup(eb::bigint nGroupId,bool bLoadMember)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId <= 0)
			return -1;
		const int nLoadEmp = bLoadMember?1:0;
		if (nLoadEmp==1 && pManager->theCheckGroupMemberList.exist(nGroupId))
		{
			return 1;
		}
		return pManager->EnterpriseLoad(nGroupId,1,1,nLoadEmp,0,0,false,"",true);
	}
	return -1;
}

int CEBAppClient::EB_EditEnterprise(const EB_EnterpriseInfo* pEnterpriseInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (!this->EB_CanEditEnterprise(pEnterpriseInfo->m_sEnterpriseCode)) return -1;
		return pManager->EntEdit(pEnterpriseInfo);
	}
	return -1;
}
int CEBAppClient::EB_EditGroup(const EB_GroupInfo* pDepartmentInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (!this->EB_CanEditGroupInfo(pDepartmentInfo->m_sEnterpriseCode,pDepartmentInfo->m_sGroupCode)) return -1;
		return pManager->DepEdit(pDepartmentInfo);
	}
	return -1;
}
bool CEBAppClient::EB_IsGroupForbidSpeech(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		return ((pDepartmentInfo->m_nGroupExtData&EB_GROUP_EXT_DATA_FORBID_SPEECH)==0)?false:true;
	}
	return false;
}

int CEBAppClient::EB_SetGroupForbidSpeech(eb::bigint nGroupId, bool bForbidSpeech)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return 0;
		if (!this->EB_CanEditGroupInfo(pDepartmentInfo->m_sEnterpriseCode,pDepartmentInfo->m_sGroupCode)) return -1;
		// -1: 解决禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
		return pManager->SetGroupForbid(nGroupId, bForbidSpeech, 0);	// 
		//EB_GroupInfo pEditGroupInfo(pDepartmentInfo.get());
		//if (bForbidSpeech)
		//	pEditGroupInfo.m_nGroupExtData |= EB_GROUP_EXT_DATA_FORBID_SPEECH;
		//else
		//	pEditGroupInfo.m_nGroupExtData &= ~EB_GROUP_EXT_DATA_FORBID_SPEECH;
		//return pManager->DepEdit(&pEditGroupInfo);
	}
	return -1;
}

int CEBAppClient::EB_DeleteGroup(eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (!this->EB_CanDeleteGroupInfo(nGroupId)) return -1;
		return pManager->DepDelete(nGroupId);
	}
	return -1;
}
int CEBAppClient::EB_JoinGroup(eb::bigint nGroupId,const char* sDescription)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->JoinGroup(nGroupId,sDescription);
	}
	return -1;
}

int CEBAppClient::EB_ExitGroup(eb::bigint nGroupId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DepExit(nGroupId);
	}
    return -1;
}

bool CEBAppClient::EB_GetGroupInfo(eb::bigint nGroupId,EB_GroupInfo* pOutGroupInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			pOutGroupInfo->operator =(*(EB_GroupInfo*)pDepartmentInfo.get());
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetGroupName(eb::bigint nGroupId, mycp::tstring& pOutGroupName) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			pOutGroupName = pDepartmentInfo->m_sGroupName;
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetGroupCreator(eb::bigint nGroupId,eb::bigint& pOutGroupCreator) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			pOutGroupCreator = pDepartmentInfo->m_nCreateUserId;
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetGroupType(eb::bigint nGroupId,EB_GROUP_TYPE& pOutGroupType) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			pOutGroupType = pDepartmentInfo->m_nGroupType;
			return true;
		}
	}
	return false;
}

bool CEBAppClient::EB_GetGroupMemberInfoList(eb::bigint nGroupId,std::vector<EB_MemberInfo>& pOutMemberInfoList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupEmployeeInfo(nGroupId,pOutMemberInfoList);
	}
	return false;
}
bool CEBAppClient::EB_GetGroupMemberUserIdList(eb::bigint nGroupId,std::vector<eb::bigint>& pOutMemberInfoList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupEmployeeInfo(nGroupId,pOutMemberInfoList);
	}
	return false;
}
bool CEBAppClient::EB_GetGroupMemberCodeList(eb::bigint nGroupId,std::vector<eb::bigint>& pOutMemberCodeList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupMemberCode(nGroupId,pOutMemberCodeList);
	}
	return false;
}
bool CEBAppClient::EB_GetGroupMemberAccountList(eb::bigint nGroupId,std::vector<mycp::tstring>& pOutMemberInfoList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupEmployeeInfo(nGroupId,pOutMemberInfoList);
	}
	return false;
}
int CEBAppClient::EB_GetGroupMemberSize(eb::bigint nGroupId, int nGetType) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupMemberSize(nGroupId,nGetType);
		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		//{
		//	//if (!pDepartmentInfo->m_pMemberList.empty())
		//	//	return pDepartmentInfo->m_pMemberList.size();
		//	//else
		//	//	return pDepartmentInfo->m_nEmpCount;

		//	return pDepartmentInfo->m_nEmpCount;
		//	//if (pDepartmentInfo->m_pMemberList.empty())
		//	//	return pDepartmentInfo->m_nEmpCount;

		//	//int result = 0;
		//	//BoostReadLock rdlock(pDepartmentInfo->m_pMemberList.mutex());
		//	//CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		//	//for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		//	//{
		//	//	CEBMemberInfo::pointer pEmployeeInfo = pIter->second;
		//	//	if (!pEmployeeInfo->m_bDeleteFlag)
		//	//	{
		//	//		result++;
		//	//	}
		//	//}
		//	////return (int)pDepartmentInfo->m_pMemberList.size();
		//	//return result;
		//}
	}
	return -1;
}
bool CEBAppClient::EB_GetGroupMemberSize(eb::bigint nGroupId, int nGetType, int& pOutMemberSize, int& pOutOnlineSize) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupMemberSize(nGroupId,nGetType,pOutMemberSize,pOutOnlineSize);
	}
	return false;
}

int CEBAppClient::EB_GetGroupOnlineSize(eb::bigint nGroupId, int nGetType) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetGroupOnlineSize(nGroupId,nGetType);
		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		//{
		//	return pDepartmentInfo->m_nOnlineSize;
		//	//if (pDepartmentInfo->m_pMemberList.empty())
		//	//{
		//	//	return -1;
		//	//}
		//	//int result = 0;
		//	//BoostReadLock rdlock(pDepartmentInfo->m_pMemberList.mutex());
		//	//CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		//	//for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		//	//{
		//	//	CEBMemberInfo::pointer pEmployeeInfo = pIter->second;
		//	//	if (!pEmployeeInfo->m_bDeleteFlag && !pEmployeeInfo->IsOfflineState())
		//	//	{
		//	//		result++;
		//	//	}
		//	//}
		//	//return result;
		//}
	}
	return -1;
}

bool CEBAppClient::EB_IsMyGroup(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->theMyEmployeeList.exist(nGroupId);
	}
    return false;
}

bool CEBAppClient::EB_HasSubGroup(eb::bigint nGroupId) const
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        if (nGroupId==0) return false;
        BoostReadLock rdlock(pManager->theDepartmentList.mutex());
        CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
        for (; pIter1!=pManager->theDepartmentList.end(); pIter1++) {
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
            if (pDepartmentInfo->m_sParentCode==nGroupId) {
                return true;
            }
        }
    }
    return false;
}

int CEBAppClient::EB_EditMember(const EB_MemberInfo* pEmployeeInfo)//,int nNeedEmpInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (!this->EB_CanEditMemberInfo(pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_nMemberUserId)) return 0;
		if (pEmployeeInfo->m_sMemberCode>0 && pEmployeeInfo->m_nMemberUserId==pManager->m_pMyAccountInfo->GetUserId())
		{
			const bool bCalEditMyBasememberinfo = this->EB_CanEditMyBaseMemberInfo(pEmployeeInfo->m_sGroupCode);
			if (!bCalEditMyBasememberinfo)
			{
				// 只能修改description
				CEBGroupInfo::pointer pDepartmentInfo;
				if (!pManager->theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo)) return 0;
				CEBMemberInfo::pointer pMyEmployeeInfo;
				if (!pDepartmentInfo->m_pMemberList.find(pEmployeeInfo->m_nMemberUserId, pMyEmployeeInfo)) return 0;
				if (pMyEmployeeInfo->m_sDescription==pEmployeeInfo->m_sDescription)
					return 0;	// 不需要修改，直接返回成功

                /// 只允许修改个人签名（备注信息）
				pMyEmployeeInfo->m_sDescription = pEmployeeInfo->m_sDescription;
				// **
				const int nForbidMinutes = -2;	// ** －2表示不处理
				const int nNeedEmpInfo = 0;
				return pManager->EmpEdit(pMyEmployeeInfo.get(),nForbidMinutes,nNeedEmpInfo);
			}
		}
		CEBMemberInfo pEditMemberInfo(pEmployeeInfo);
		const int nForbidMinutes = -2;	// ** －2表示不处理
		const int nNeedEmpInfo = 0;
		return pManager->EmpEdit(&pEditMemberInfo,nForbidMinutes,nNeedEmpInfo);
	}
	return -1;
}
bool CEBAppClient::EB_IsGroupAdminLevel(eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		const mycp::bigint nMemberUserId = pManager->m_pMyAccountInfo->GetUserId();
		if (this->EB_IsGroupCreator(nGroupId,nMemberUserId)) return true;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		const int nGroupAdminLevel = EB_LEVEL_DEP_ADMIN;
		return ((pMyEmployeeInfo->m_nManagerLevel&nGroupAdminLevel)==nGroupAdminLevel)?true:false;
	}
	return false;
}
bool CEBAppClient::EB_IsGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (this->EB_IsGroupCreator(nGroupId,nMemberUserId)) return true;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		const int nGroupAdminLevel = EB_LEVEL_DEP_ADMIN;
		return ((pMyEmployeeInfo->m_nManagerLevel&nGroupAdminLevel)==nGroupAdminLevel)?true:false;
	}
	return false;
}
int CEBAppClient::EB_AddGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (!this->EB_IsGroupCreator(nGroupId,pManager->m_pMyAccountInfo->GetUserId())) return 0;
		if (this->EB_IsGroupCreator(nGroupId,nMemberUserId)) return 1;

		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return 0;
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return 0;

		const int nGroupAdminLevel = EB_LEVEL_DEP_ADMIN;
		if ((pMyEmployeeInfo->m_nManagerLevel&nGroupAdminLevel)==nGroupAdminLevel)
			return 1;

		CEBMemberInfo pEditMemberInfo(pMyEmployeeInfo.get());
		pEditMemberInfo.m_nManagerLevel |= nGroupAdminLevel;
		const int nForbidMinutes = -2;	// ** －2表示不处理
		const int nNeedEmpInfo = 0;
		return pManager->EmpEdit(&pEditMemberInfo,nForbidMinutes,nNeedEmpInfo);
	}
	return -1;
}

int CEBAppClient::EB_DelGroupAdminLevel(eb::bigint nGroupId, eb::bigint nMemberUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (!this->EB_IsGroupCreator(nGroupId,pManager->m_pMyAccountInfo->GetUserId())) return 0;
		if (this->EB_IsGroupCreator(nGroupId,nMemberUserId)) return 0;

		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return 0;
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return 0;

		const int nGroupAdminLevel = EB_LEVEL_DEP_ADMIN;
		if ((pMyEmployeeInfo->m_nManagerLevel&nGroupAdminLevel)==0)
			return 1;

		CEBMemberInfo pEditMemberInfo(pMyEmployeeInfo.get());
		pEditMemberInfo.m_nManagerLevel &= ~nGroupAdminLevel;
		//pEditMemberInfo.m_nManagerLevel = 0;		// 
		const int nForbidMinutes = -2;	// ** －2表示不处理
		const int nNeedEmpInfo = 0;
		return pManager->EmpEdit(&pEditMemberInfo,nForbidMinutes,nNeedEmpInfo);
	}
	return -1;
}
bool CEBAppClient::EB_IsMemberForbidSpeech(eb::bigint nGroupId, eb::bigint nMemberUserId,int& pOutForbidMinutes)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return false;
		CEBMemberInfo::pointer pMyEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return false;
		pOutForbidMinutes = pMyEmployeeInfo->m_nForbidMinutes;
		return ((pMyEmployeeInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)?false:true;
	}
	return false;
}

int CEBAppClient::EB_SetMemberForbidSpeech(eb::bigint nGroupId, eb::bigint nMemberUserId, bool bForbidSpeech, int nForbidMinutes)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get()!=NULL)
	{
		if (bForbidSpeech && this->EB_IsGroupCreator(nGroupId,nMemberUserId)) return 0;	// 群主不能被禁言
		if (!this->EB_CanEditMemberInfo(nGroupId,nMemberUserId)) return 0;
		//if (!this->EB_IsGroupCreator(nGroupId,pManager->m_pMyAccountInfo->GetUserId())) return 0;

		// -1: 解除禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
		if (bForbidSpeech)
		{
			if (nForbidMinutes<0)
				nForbidMinutes = 0;
		}
		else
		{
			nForbidMinutes = -1;
		}
		return pManager->SetMemberForbidSpeech(nGroupId, nMemberUserId, nForbidMinutes);

		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (!pManager->theDepartmentList.find(nGroupId, pDepartmentInfo)) return 0;
		//CEBMemberInfo::pointer pMyEmployeeInfo;
		//if (!pDepartmentInfo->m_pMemberList.find(nMemberUserId, pMyEmployeeInfo)) return 0;

		//CEBMemberInfo pEditMemberInfo(pMyEmployeeInfo.get());
		//if (bForbidSpeech)
		//{
		//	pEditMemberInfo.m_nManagerLevel |= EB_LEVEL_FORBID_SPEECH;
		//}
		//else
		//{
		//	nForbidMinutes = -1;
		//	pEditMemberInfo.m_nManagerLevel &= ~EB_LEVEL_FORBID_SPEECH;
		//}
		//const int nNeedEmpInfo = 0;
		//return pManager->EmpEdit(&pEditMemberInfo,nForbidMinutes,nNeedEmpInfo);
	}
	return -1;
}

int CEBAppClient::EB_DeleteMember(eb::bigint nMemberCode, bool bDeleteAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (!this->EB_CanDeleteMemberInfo(nMemberCode)) return -1;
		return pManager->EmpDelete(nMemberCode,bDeleteAccount);
	}
	return -1;
}

//EB_MemberInfo::pointer CEBAppClient::EB_GetEmployeeInfo(const EB_MemberInfo* pInPopEmployeeInfo) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		if (pInPopEmployeeInfo == NULL) return PopNullEmployeeInfo;
//		boost::mutex::scoped_lock lock1(pManager->theDepartmentList.mutex());
//		CLockMap<mycp::tstring, EB_GroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
//		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
//		{
//			EB_GroupInfo::pointer pDepartmentInfo = pIter1->second;
//			boost::mutex::scoped_lock lock2(pDepartmentInfo->m_pEmployeeList.mutex());
//			CLockMap<mycp::tstring, EB_MemberInfo::pointer>::iterator pIter2 = pDepartmentInfo->m_pEmployeeList.begin();
//			for (; pIter2!=pDepartmentInfo->m_pEmployeeList.end(); pIter2++)
//			{
//				EB_MemberInfo::pointer pEmployeeInfo = pIter2->second;
//				if (pEmployeeInfo.get() == pInPopEmployeeInfo)
//					return pEmployeeInfo;
//			}
//		}
//	}
//	return PopNullEmployeeInfo;
//}
bool CEBAppClient::EB_GetMemberInfoByUserId(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId==0) return false;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutMemberInfo->operator =(pEmployeeInfo.get());
				return true;
			}
		}

		//boost::mutex::scoped_lock lock1(pManager->theDepartmentList.mutex());
		//CLockMap<mycp::tstring, EB_GroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		//for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		//{
		//	CEBGroupInfo::pointer pDepartmentInfo = pIter1->second;
		//	CEBMemberInfo::pointer pEmployeeInfo;
		//	if ((pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_PROJECT) &&
		//		pDepartmentInfo->m_pMemberList.find(sEmpAccount, pEmployeeInfo))
		//	{
		//		return pEmployeeInfo;
		//	}
		//}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberInfoByAccount(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId,const char* sMemberAccount) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId==0) return false;
		if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
		mycp::tstring sInMemberAccount(sMemberAccount);
		if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
		{
			sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
		}

		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			CEBMemberInfo::pointer pEmployeeInfo;
			BoostReadLock rdlock(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
				CEBMemberInfo::pointer pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					pOutMemberInfo->operator =(pEmployeeInfo.get());
					return true;
				}
			}
		}
	}
	return false;
}

bool CEBAppClient::EB_GetMemberInfoByUserId2(EB_MemberInfo* pOutMemberInfo,eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberUserId==0) return false;
		BoostReadLock rdlock(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutMemberInfo->operator =(pEmployeeInfo.get());
				return true;
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberInfoByUserId2(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberUserId==0) return false;
		BoostReadLock rdlock(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutMemberInfo->operator =(pEmployeeInfo.get());
				if (pOutGroupInfo!=NULL)
					pOutGroupInfo->operator =(*(EB_GroupInfo*)pDepartmentInfo.get());
				return true;
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberInfoByAccount2(EB_MemberInfo* pOutMemberInfo,const char* sMemberAccount) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
		mycp::tstring sInMemberAccount(sMemberAccount);
		if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
		{
			sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
		}
		BoostReadLock rdlock1(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
			BoostReadLock rdlock2(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
                const CEBMemberInfo::pointer &pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					pOutMemberInfo->operator =(pEmployeeInfo.get());
					return true;
				}
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberInfoByAccount2(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,const char* sMemberAccount) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
		mycp::tstring sInMemberAccount(sMemberAccount);
		if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
		{
			sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
		}
		BoostReadLock rdlock1(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
			BoostReadLock rdlock2(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
                const CEBMemberInfo::pointer &pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					pOutMemberInfo->operator =(pEmployeeInfo.get());
					if (pOutGroupInfo!=NULL)
						pOutGroupInfo->operator =(*(EB_GroupInfo*)pDepartmentInfo.get());
					return true;
				}
			}
		}
	}
    return false;
}

bool CEBAppClient::EB_IsExistLocalMemberInfo(eb::bigint nMemberId) const
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        if (nMemberId==0) return false;
        return pManager->theEmployeeList.exist(nMemberId);
    }
    return false;
}

bool CEBAppClient::EB_GetMemberInfoByMemberCode(EB_MemberInfo* pOutMemberInfo,eb::bigint nMemberCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberCode==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberCode, pEmployeeInfo))
		{
			pOutMemberInfo->operator =(pEmployeeInfo.get());
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberInfoByMemberCode(EB_MemberInfo* pOutMemberInfo,EB_GroupInfo* pOutGroupInfo,eb::bigint nMemberCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberCode==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberCode, pEmployeeInfo))
		{
			pOutMemberInfo->operator =(pEmployeeInfo.get());
			if (pOutGroupInfo!=NULL)
			{
				CEBGroupInfo::pointer pDepartmentInfo;
				if (pManager->theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo))
				{
					pOutGroupInfo->operator =(*(EB_GroupInfo*)pDepartmentInfo.get());
				}else
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberHeadInfoByMemberCode(eb::bigint nMemberCode, EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberCode==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberCode, pEmployeeInfo))
		{
			pOutHeadFile = pEmployeeInfo->m_sHeadResourceFile;
			if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
				pOutHeadMd5 = "";
			else
				pOutHeadMd5 = pEmployeeInfo->m_sHeadMd5;
			pOutLineState = pEmployeeInfo->m_nLineState;
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberHeadInfoByUserId(eb::bigint nGroupId,eb::bigint nMemberUserId, EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId==0) return false;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutHeadFile = pEmployeeInfo->m_sHeadResourceFile;
				if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
					pOutHeadMd5 = "";
				else
					pOutHeadMd5 = pEmployeeInfo->m_sHeadMd5;
				pOutLineState = pEmployeeInfo->m_nLineState;
				return true;
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberHeadInfoByAccount(const char* sMemberAccount, EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
		mycp::tstring sInMemberAccount(sMemberAccount);
		if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
		{
			sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
		}
		BoostReadLock rdlock1(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
			const CEBGroupInfo::pointer& pDepartmentInfo = pIter1->second;
			BoostReadLock rdlock2(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
				const CEBMemberInfo::pointer& pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					pOutHeadFile = pEmployeeInfo->m_sHeadResourceFile;
					if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
						pOutHeadMd5 = "";
					else
						pOutHeadMd5 = pEmployeeInfo->m_sHeadMd5;
					pOutLineState = pEmployeeInfo->m_nLineState;
					return true;
				}
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberHeadInfoByUserId(eb::bigint nMemberUserId, EBFileString& pOutHeadFile,tstring& pOutHeadMd5,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberUserId==0) return false;
		BoostReadLock rdlock(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
			const CEBGroupInfo::pointer& pDepartmentInfo = pIter1->second;
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutHeadFile = pEmployeeInfo->m_sHeadResourceFile;
				if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
					pOutHeadMd5 = "";
				else
					pOutHeadMd5 = pEmployeeInfo->m_sHeadMd5;
				pOutLineState = pEmployeeInfo->m_nLineState;
				return true;
			}
		}
	}
	return false;
}

bool CEBAppClient::EB_GetMemberNameByUserId(eb::bigint nGroupId,eb::bigint nMemberUserId,mycp::tstring& pOutMemberName) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId==0 || nMemberUserId==0) return false;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
			{
				pOutMemberName = pEmployeeInfo->m_sUserName;
				return true;
			}
		}
	}
    return false;
}

bool CEBAppClient::EB_GetMemberNameByUserId2(eb::bigint nMemberUserId, cgcSmartString &pOutMemberName) const
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        if (nMemberUserId==0) return false;
        BoostReadLock rdlock(pManager->theDepartmentList.mutex());
        CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
        for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
        {
            const CEBGroupInfo::pointer &pDepartmentInfo = pIter1->second;
            CEBMemberInfo::pointer pEmployeeInfo;
            if (pDepartmentInfo->m_pMemberList.find(nMemberUserId, pEmployeeInfo))
            {
                pOutMemberName = pEmployeeInfo->m_sUserName;
                return true;
            }
        }
    }
    return false;
}

bool CEBAppClient::EB_GetMemberNameByMemberCode(eb::bigint nMemberId,mycp::tstring& pOutMemberName) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberId==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberId, pEmployeeInfo))
		{
			pOutMemberName = pEmployeeInfo->m_sUserName;
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberLineState(eb::bigint nMemberId,EB_USER_LINE_STATE& pOutLineState) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberId==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberId, pEmployeeInfo))
		{
			pOutLineState = pEmployeeInfo->m_nLineState;
			return true;
		}
	}
	return false;
}

bool CEBAppClient::EB_GetMemberHeadFile(eb::bigint nMemberId,eb::bigint& pOutResourceId, EBFileString& pOutHeadPath,mycp::tstring& pOutFileMd5)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberId==0) return false;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pManager->theEmployeeList.find(nMemberId, pEmployeeInfo))
		{
			pOutResourceId = pEmployeeInfo->m_sHeadResourceId;
			pOutHeadPath = pEmployeeInfo->m_sHeadResourceFile;
			if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
				pOutFileMd5 = "";
			else
				pOutFileMd5 = pEmployeeInfo->m_sHeadMd5;
			//pOutFileSize = (int)EBGetFileSize(pOutHeadPath.c_str());
			return true;
		}
	}
	return false;
}
bool CEBAppClient::EB_GetMemberHeadFile(eb::bigint nGroupId,eb::bigint nUserId,eb::bigint& pOutResourceId, EBFileString& pOutHeadPath,mycp::tstring& pOutFileMd5)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			CEBMemberInfo::pointer pEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(nUserId,pEmployeeInfo))
			{
				pOutResourceId = pEmployeeInfo->m_sHeadResourceId;
				pOutHeadPath = pEmployeeInfo->m_sHeadResourceFile;
				if (pManager->theResourceFilePath.exist(pEmployeeInfo->m_sHeadResourceId))
					pOutFileMd5 = "";
				else
					pOutFileMd5 = pEmployeeInfo->m_sHeadMd5;
				//pOutFileSize = (int)EBGetFileSize(pOutHeadPath.c_str());
				return true;
			}
		}
	}
	return false;
}

bool CEBAppClient::EB_GetMyMemberInfo(EB_MemberInfo* pOutMemberInfo,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nGroupId == 0)
		{
			if (pManager->m_pDefaultEntEmployeeInfo.get()==NULL) return false;
			pOutMemberInfo->operator =(pManager->m_pDefaultEntEmployeeInfo.get());
			return true;
		}else
		{
			CEBMemberInfo::pointer pMyEmployeeInfo;
			if (pManager->theMyEmployeeList.find(nGroupId,pMyEmployeeInfo))
			{
				pOutMemberInfo->operator =(pMyEmployeeInfo.get());
				return true;
			//}else
			//{
			//	return pManager->m_pDefaultEntEmployeeInfo;
			}
		}
	}
	return false;
}

void CEBAppClient::EB_GetMyMemberList(std::vector<EB_MemberInfo>& pOutEmployeeInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->GetMyEmployeeInfo(pOutEmployeeInfo);
	}
}

bool CEBAppClient::EB_IsExistMemberByUserId(eb::bigint nGroupId, eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			return pDepartmentInfo->m_pMemberList.exist(nMemberUserId);
		}
	}
	return false;
}
bool CEBAppClient::EB_IsExistMemberByUserId(eb::bigint nMemberUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nMemberUserId==0) return false;
		BoostReadLock rdlock(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
			const CEBGroupInfo::pointer pDepartmentInfo = pIter1->second;
			if (pDepartmentInfo->m_pMemberList.exist(nMemberUserId))
			{
				return true;
			}
		}
	}
	return false;
}

bool CEBAppClient::EB_IsExistMemberByAccount(eb::bigint nGroupId,const char* sMemberAccount) const
{
	if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(nGroupId, pDepartmentInfo))
		{
			mycp::tstring sInMemberAccount(sMemberAccount);
			if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
			{
				sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
			}

			BoostReadLock rdlock2(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
				CEBMemberInfo::pointer pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					return true;
				}
			}
		}
	}
	return false;
}
bool CEBAppClient::EB_IsExistMemberByAccount(const char* sMemberAccount) const
{
	if (sMemberAccount==NULL || strlen(sMemberAccount)==0) return false;
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		BoostReadLock rdlock(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
			const CEBGroupInfo::pointer pDepartmentInfo = pIter1->second;

			mycp::tstring sInMemberAccount(sMemberAccount);
			if (!pManager->m_sAccountPrefix.empty() && sInMemberAccount.find(pManager->m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
			{
				sInMemberAccount = pManager->m_sAccountPrefix+sInMemberAccount;
			}

			BoostReadLock rdlock2(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
			{
				CEBMemberInfo::pointer pEmployeeInfo = pIter2->second;
				if (pEmployeeInfo->m_sMemberAccount == sInMemberAccount)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CEBAppClient::EB_IsExistMemberByMemberCode(eb::bigint nMemberId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->theEmployeeList.exist(nMemberId);
	}
	return false;
}

bool CEBAppClient::EB_GetEnterpriseInfo(EB_EnterpriseInfo* pOutEnterpriseInfo,eb::bigint sEntCode) const
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        if (sEntCode==0)
        {
            if (pManager->m_pDefaultEnterpriseInfo.get()==NULL) return false;
            pOutEnterpriseInfo->operator =(*(EB_EnterpriseInfo*)pManager->m_pDefaultEnterpriseInfo.get());
            return true;
        }
        CEBEnterpriseInfo::pointer pEnterpriseInfo;
        if (pManager->theEnterpriseList.find(sEntCode,pEnterpriseInfo))
        {
            pOutEnterpriseInfo->operator =(*(EB_EnterpriseInfo*)pEnterpriseInfo.get());
            return true;
        }
    }
    return false;
}

bool CEBAppClient::EB_IsEnterpriseUser(eb::bigint nEnterpriseCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nEnterpriseCode==0)
		{
			return (pManager->m_pDefaultEnterpriseInfo.get()==NULL || pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode==0)?false:true;
		}
		return pManager->theEnterpriseList.exist(nEnterpriseCode);
	}
	return false;
}

bool compare_emotion_index(const EB_EmotionInfo& t1,const EB_EmotionInfo& t2){  
	return t1.m_nIndex < t2.m_nIndex;  
}
bool CEBAppClient::EB_GetMyEmotionList(std::vector<EB_EmotionInfo>& pOutEmotionList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		//mycp::tstring sInEnterpriseCode;
		//if (pManager->m_pDefaultEnterpriseInfo.get()!=NULL)
		//	sInEnterpriseCode = pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode;
		{
			const mycp::bigint sMyAccount = pManager->m_pMyAccountInfo->GetUserId();
			BoostReadLock rdlock(pManager->theEmotionList.mutex());
			CLockMap<mycp::tstring,CEBEmotionInfo::pointer>::iterator pIter = pManager->theEmotionList.begin();
			for (;pIter!=pManager->theEmotionList.end();pIter++)
			{
				const CEBEmotionInfo::pointer& pEmotionInfo = pIter->second;
				if (pEmotionInfo->m_nResType == EB_RESOURCE_EMOTION && pEmotionInfo->m_nUserId == sMyAccount)
				{
					pOutEmotionList.push_back(EB_EmotionInfo(pEmotionInfo.get()));
				}
			}
		}
		// 这里要排序
		std::sort(pOutEmotionList.begin(),pOutEmotionList.end(),compare_emotion_index);
		return true;
	}
	return false;
}
bool CEBAppClient::EB_GetDefaultHeadList(std::vector<EB_EmotionInfo>& pOutEmotionList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		//mycp::tstring sInEnterpriseCode;
		//if (pManager->m_pDefaultEnterpriseInfo.get()!=NULL)
		//	sInEnterpriseCode = pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseCode;
		const mycp::bigint sMyAccount = pManager->m_pMyAccountInfo->GetUserId();
		{
			BoostReadLock rdlock(pManager->theEmotionList.mutex());
			CLockMap<mycp::tstring,CEBEmotionInfo::pointer>::iterator pIter = pManager->theEmotionList.begin();
			for (;pIter!=pManager->theEmotionList.end();pIter++)
			{
				const CEBEmotionInfo::pointer& pEmotionInfo = pIter->second;
				if (pEmotionInfo->m_nResType == EB_RESOURCE_HEAD && pEmotionInfo->m_nUserId == sMyAccount)
				{
					pOutEmotionList.push_back(EB_EmotionInfo(pEmotionInfo.get()));
				}
			}
		}
		// 这里要排序
		std::sort(pOutEmotionList.begin(),pOutEmotionList.end(),compare_emotion_index);
		return true;
	}
	return false;
}
bool CEBAppClient::EB_GetEnterpriseName(mycp::tstring& pOutEnterpriseName,eb::bigint nEnterpriseCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (nEnterpriseCode==0)
		{
			if (pManager->m_pDefaultEnterpriseInfo.get()==NULL) return false;
			pOutEnterpriseName = pManager->m_pDefaultEnterpriseInfo->m_sEnterpriseName;
			return true;
		}
		CEBEnterpriseInfo::pointer pEnterpriseInfo;
		if (pManager->theEnterpriseList.find(nEnterpriseCode,pEnterpriseInfo))
		{
			pOutEnterpriseName = pEnterpriseInfo->m_sEnterpriseName;
			return true;
		}
	}
	return false;
}

void CEBAppClient::EB_GetEnterpriseMemberSize(eb::bigint nEnterpriseCode,int& pOutMemberSize,int& pOutOnlineSize) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pOutMemberSize = 0;
		pOutOnlineSize = 0;
		BoostReadLock lockDepartment(pManager->theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = pManager->theDepartmentList.begin();
		for (; pIterDepartment!=pManager->theDepartmentList.end(); pIterDepartment++)
		{
			CEBGroupInfo::pointer pDepartmentInfo = pIterDepartment->second;
			if (pDepartmentInfo->m_sEnterpriseCode != nEnterpriseCode ||
				(pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_DELETE)==EB_GROUP_DATA_DELETE)
			{
				continue;
			}

			pOutMemberSize += pDepartmentInfo->m_nEmpCount;
			pOutOnlineSize += pDepartmentInfo->m_nOnlineSize;

			//if (pDepartmentInfo->m_pMemberList.empty())
			//	pOutMemberSize += pDepartmentInfo->m_nEmpCount;
			//else
			//	pOutMemberSize += pDepartmentInfo->m_pMemberList.size();
			//BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
			//CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
			//for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
			//{
			//	CEBMemberInfo::pointer pEmployeeInfo = pIter->second;
			//	if (pEmployeeInfo->m_bDeleteFlag)
			//		pOutMemberSize--;
			//	else if (pEmployeeInfo->IsFullOnineState())
			//	{
			//		pOutOnlineSize++;
			//	}
			//}
		}
	}
}

void CEBAppClient::EB_FindAllGroupInfo(CEBSearchCallback * pCallback, eb::bigint sEntCode, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadDepartmentInfo(sEntCode, pCallback, dwParam);
	}
}
void CEBAppClient::EB_FindAllContactInfo(CEBSearchCallback * pCallback, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadContactInfo(pCallback, dwParam);
	}
}
void CEBAppClient::EB_SearchUserInfo(CEBSearchCallback * pCallback, const char* sSearchKey, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->SearchUserInfo(sSearchKey, pCallback, dwParam);
	}
}

int CEBAppClient::EB_RDRequest(eb::bigint nCallId,EB_REMOTE_DESKTOP_TYPE nRDType)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->RDRequest(nCallId,nRDType);
	}
	return -1;
}
int CEBAppClient::EB_RDAck(eb::bigint nCallId,bool bAccept,eb::bigint nToUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->RDAck(nCallId,bAccept,nToUserId);
	}
	return -1;
}
int CEBAppClient::EB_RDEnd(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->RDEnd(nCallId);
	}
	return -1;
}

#ifdef _QT_MAKE_
void CEBAppClient::EB_SetDesktopReceiver(eb::bigint nCallId,QObject* pHwnd)
#else
void CEBAppClient::EB_SetDesktopHwnd(eb::bigint nCallId,HWND pHwnd)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pHwnd==NULL)
		{
			pManager->m_pDesktopHwnd.remove(nCallId);
		}else
		{
			pManager->m_pDesktopHwnd.insert(nCallId, pHwnd);
		}
	}
}

int CEBAppClient::EB_OpenLocalDesktop(eb::bigint nCallId, int quant, int fps)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->OpenLocalDesktop(nCallId,quant,fps);
	}
	return -1;
}
void CEBAppClient::EB_EnableControlLocalDesktop(eb::bigint nCallId, eb::bigint nUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->EnableControlLocalDesktop(nCallId, nUserId);
	}
}
eb::bigint CEBAppClient::EB_EnableControlLocalDesktop(eb::bigint nCallId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EnableControlLocalDesktop(nCallId);
	}
	return 0;
}
void CEBAppClient::EB_CloseLocalDesktop(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->CloseLocalDesktop(nCallId);
	}
}

#ifdef _QT_MAKE_
bool CEBAppClient::EB_OpenRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, QObject* hWndParent)
#else
bool CEBAppClient::EB_OpenRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, HWND hWndParent)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->OpenRemoteDesktop(nCallId, nUserId, hWndParent);
	}
	return false;
}
// int nMouseMoveFPS: 0: disable
bool CEBAppClient::EB_ControlRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId, int nMouseMoveFPS)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ControlRemoteDesktop(nCallId, nUserId, nMouseMoveFPS);
	}
	return false;
}
int CEBAppClient::EB_ControlRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ControlRemoteDesktop(nCallId, nUserId);
	}
	return 0;
}
void CEBAppClient::EB_MoveRemoteDesktopWindow(eb::bigint nCallId, eb::bigint nUserId, int x, int y, int width, int height, bool bRepaint)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->MoveRemoteDesktopWindow(nCallId, nUserId, x, y, width, height, bRepaint);
	}
}
void CEBAppClient::EB_CloseRemoteDesktop(eb::bigint nCallId,eb::bigint nUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->CloseRemoteDesktop(nCallId, nUserId);
	}
}

void CEBAppClient::EB_CloseAllDesktop(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->CloseAllDesktop(nCallId);
	}
}
bool CEBAppClient::EB_GetUserRDInfo(eb::bigint nCallId,std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallRDUserInfo(nCallId,pOutVideoUserInfo);
	}
	return false;
}


//
//void CEBAppClient::EB_GetMyDepartment(std::vector<EB_GroupInfo::pointer>& pOutDepartment) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		pManager->IsMyDepartment(pOutDepartment);
//	}
//}

void CEBAppClient::EB_SetVideoCallback(eb::bigint nCallId,PEBVideoCallBack cbAudio)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->SetVideoCallback(nCallId,cbAudio);
	}
}
#ifdef _QT_MAKE_
void CEBAppClient::EB_GetVideoDeviceList(std::vector<QString>& pOutVideoDeviceList)
#else
void CEBAppClient::EB_GetVideoDeviceList(std::vector<mycp::tstring>& pOutVideoDeviceList)
#endif
{
	Cebmm::GetVideoDevices(pOutVideoDeviceList);
}
//int CEBAppClient::EB_GetDefaultVideoMediaType(void)
//{
//	return vr::VIDEO_RGB24;
//}

#ifdef _QT_MAKE_
int CEBAppClient::EB_OpenLocalVideo(eb::bigint nCallId,int nLocalVideoIndex, QWidget* hVideoWndParent,void* pUserParam)
#else
int CEBAppClient::EB_OpenLocalVideo(eb::bigint nCallId,int nLocalVideoIndex,HWND hVideoWndParent,void* pUserParam)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
#ifdef _QT_MAKE_
		// ???
#else
		if (pManager->m_pVideoDevices.empty())
			Cebmm::GetVideoDevices(pManager->m_pVideoDevices);
#endif
		if (pManager->m_pVideoDevices.empty()) return -1;
		pManager->m_nLocalVideoIndex = min(nLocalVideoIndex,(int)pManager->m_pVideoDevices.size()-1);
		return pManager->OpenLocalVideo(nCallId,hVideoWndParent,pUserParam);
	}
	return -1;
}

#ifdef _QT_MAKE_
bool CEBAppClient::EB_OpenUserVideo(eb::bigint nCallId, eb::bigint nUserId,
                                    QWidget *hVideoWndParent, void* pUserParam)
#else
bool CEBAppClient::EB_OpenUserVideo(eb::bigint nCallId,eb::bigint nUserId,HWND hVideoWndParent,void* pUserParam)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->OpenUserVideo(nCallId,nUserId,hVideoWndParent,pUserParam);
	}
	return false;
}
//bool CEBAppClient::EB_OpenVideoByUserVideoId(eb::bigint nCallId,int nUserVideoId,PAudioDataCallBack cbAudio,void* pParam)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->OpenUserVideo(nCallId,nUserVideoId,cbAudio,pParam);
//	}
//	return false;
//}

bool CEBAppClient::EB_SetVideoOpenClose(eb::bigint nCallId,eb::bigint nUserId,bool bOpen)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetVideoOpenClose(nCallId,nUserId,bOpen);
	}
	return false;
}
bool CEBAppClient::EB_SetAudioOpenClose(eb::bigint nCallId,eb::bigint nUserId,bool bOpen)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetAudioOpenClose(nCallId,nUserId,bOpen);
	}
	return false;
}

void CEBAppClient::EB_CloseAllVideo(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DoVideoDisonnecte(nCallId, false);
	}
}

#ifdef _QT_MAKE_
void CEBAppClient::EB_SetVideoReceiver(eb::bigint nCallId,QObject* pHwnd)
#else
void CEBAppClient::EB_SetVideoHwnd(eb::bigint nCallId,HWND pHwnd)
#endif
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pHwnd==NULL)
		{
			pManager->m_pVideoHwnd.remove(nCallId);
		}else
		{
			pManager->m_pVideoHwnd.insert(nCallId, pHwnd);
		}
	}
}
//void CEBAppClient::EB_DelVideoHwnd(eb::bigint nCallId)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		pManager->m_pVideoHwnd.remove(nCallId);
//	}
//}

int CEBAppClient::EB_VideoRequest(eb::bigint nCallId,EB_VIDEO_TYPE nVideoType)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoRequest(nCallId,nVideoType);
	}
	return -1;
}
int CEBAppClient::EB_VideoAck(eb::bigint nCallId,bool bAccept,eb::bigint nToUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoAck(nCallId,bAccept,nToUserId);
	}
	return -1;
}
int CEBAppClient::EB_VideoEnd(eb::bigint nCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoEnd(nCallId);
	}
	return -1;
}
bool CEBAppClient::EB_GetUserVideoInfo(eb::bigint nCallId, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallVideoUserInfo(nCallId,pOutVideoUserInfo);
	}
    return false;
}

bool CEBAppClient::EB_IsExistUserVideoInfo(eb::bigint nCallId) const
{
    CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
    if (pManager != NULL)
    {
        return pManager->IsExistCallVideoUserInfo(nCallId);
    }
    return false;
}

int CEBAppClient::EB_GetSubscribeFuncList(EB_FUNC_LOCATION nFuncLocation,std::vector<EB_SubscribeFuncInfo>& pOutFuncInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetFuncInfo(nFuncLocation,pOutFuncInfo);
	}
	return -1;
}
int CEBAppClient::EB_GetSubscribeFuncSize(EB_FUNC_LOCATION nFuncLocation) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetFuncSize(nFuncLocation);
	}
	return -1;
}
bool CEBAppClient::EB_GetSubscribeFuncInfo(eb::bigint nSubId, EB_SubscribeFuncInfo* pOutFuncInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetFuncInfo(nSubId,pOutFuncInfo);
	}
	return false;
}

mycp::tstring CEBAppClient::EB_GetSubscribeFuncUrl(eb::bigint nSubscribeId,eb::bigint nCallId,eb::bigint nFromUserId,const mycp::tstring& sFromAccount,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetSubscribeFuncUrl(nSubscribeId,nCallId,nFromUserId,sFromAccount,nGroupId);
	}
	return "";
}

mycp::tstring CEBAppClient::EB_GetConversationsUrl(eb::bigint nFromUserId,eb::bigint nGroupId) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetConversationsUrl(nFromUserId,nGroupId);
	}
	return "";
}

void CEBAppClient::EB_AckMsg(eb::bigint nMsgId,bool bAccept) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->UmMsgAck(nMsgId,bAccept?EB_CAT_ACCEPT:EB_CAT_REJECT);
	}
}

void CEBAppClient::EB_AckMsg(eb::bigint nMsgId,int nAckType) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->UmMsgAck(nMsgId,(EB_CALL_ACK_TYPE)nAckType);
	}
}

} // namespace entboost
