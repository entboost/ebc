// chatroom.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UserManagerApp.h"
#include "usermanager.h"
//#include "md5.h"



Cusermanager::Cusermanager(void)
: m_handle(NULL)
{

}
Cusermanager::~Cusermanager(void)
{
	Stop();
}

int Cusermanager::Start(const char * address)
{
	int result = 0;
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager == NULL)
	{
		pManager = new CUserManagerApp();
		pManager->SetServerAddress(address);
		m_handle = pManager;
	}else
	{
		pManager->SetServerAddress(address);
		result = 1;
	}
	return result;
}

std::string Cusermanager::GetAddress(void) const
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

bool Cusermanager::IsStart(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return true;
	}
	return false;
}
//void Cusermanager::SetCallBack(CUserManagerCallBack * pCallBack)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		//pManager->m_callback = pCallBack;
//	}
//}
void Cusermanager::SetMsgHwnd(HWND pHwnd)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->m_pHwnd = pHwnd;
	}
}
void Cusermanager::Stop(void)
{
	Logout();
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	m_handle = NULL;
	if (pManager != NULL)
	{
		//pManager->m_pLogonCenter->Stop();
		delete pManager;
	}
}
tstring Cusermanager::GetAppPath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppPath();
	}
	return "";
}
tstring Cusermanager::GetResourcePath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetResourcePath();
	}
	return "";
}

tstring Cusermanager::GetAppDatePath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppDatePath();
	}
	return "";
}
tstring Cusermanager::GetAppDataTempPath(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAppDataTempPath();
	}
	return "";
}

int Cusermanager::SetDevAppId(const char* sAppId, const char* sAppKey,bool bReLoadAppOnlineKey)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->SetDevAppId(sAppId,sAppKey,bReLoadAppOnlineKey);
	}
	return -1;
}
void Cusermanager::SetDevAppOnlineKey(const char* sAppId, const char* sAppOnlineKey)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->SetDevAppOnlineKey(sAppId,sAppOnlineKey);
	}
}
void Cusermanager::GetDevAppOnlineKey(std::string& sOutAppId, std::string& sOutAppOnlineKey)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->GetDevAppOnlineKey(sOutAppId,sOutAppOnlineKey);
	}
}

int Cusermanager::Register(const char* sAccount, const char* sPwd, const char* sUserName, const char* sEntName)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->Register(sAccount,sPwd,sUserName,sEntName);
	}
	return -1;
}
int Cusermanager::LogonVisitor(int nLogonType)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_VISITOR|nLogonType);
		pLogonInfo->m_nLineState = EB_LINE_STATE_ONLINE;
		return pManager->Logon(pLogonInfo);
	}
	return -1;
}
int Cusermanager::LogonAccount(const char * sAccount, const char * sPassword, int nLogonType, EB_USER_LINE_STATE nNewLineState)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(nLogonType, sAccount, sPassword);
		pLogonInfo->m_nLineState = nNewLineState;
		return pManager->Logon(pLogonInfo);
	}
	return -1;
}
bool Cusermanager::IsLogoned(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetUserStatus() == CUserManagerApp::US_Logoned;
	}
	return false;
}
void Cusermanager::LoadInfo(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadInfo();
	}
}
void Cusermanager::Logout(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->Logout();
	}
}

int Cusermanager::SetDepHead(const char* sDepCode,const char* sFilePath)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		unsigned int nFileSize = 0;
		FILE * f = fopen(sFilePath, "rb");
		if (f == NULL)
		{
			return -1;
		}
		fseek(f, 0, SEEK_END);
		nFileSize = ftell(f);
		fclose(f);
		if (nFileSize>1*1024*1024)
		{
			return 1;
		}
		return pManager->SetDepHead(sDepCode,sFilePath);
	}
	return -1;
}

//int Cusermanager::LoadRes(const char* sDepCode,const char* sEntCode)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->LoadRes(sDepCode,sEntCode);
//	}
//	return -1;
//}
//int Cusermanager::AddDirRes(const char* sDirName,const char* sParentResId,const char* sDepCode,const char* sEntCode)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->AddDirRes(sDirName,sParentResId,sDepCode,sEntCode);
//	}
//	return -1;
//}
//int Cusermanager::AddFileRes(const char* sFilePath,const char* sFileName,const char* sDescription,const char* sParentResId,const char* sDepCode,const char* sEntCode)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		FILE * f = fopen(sFilePath, "rb");
//		if (f == NULL)
//		{
//			return -1;
//		}
//		fclose(f);
//		tstring sName=sFileName;
//		if (sName=="")
//		{
//			sName = sFilePath;
//			sName = sName.substr(sName.rfind('\\')+1);
//		}
//		return pManager->AddFileRes(sName,sFilePath,sDescription,sParentResId,sDepCode,sEntCode);
//	}
//	return -1;
//}
//int Cusermanager::AddNoteRes(const char* sNoteName,const char* sDescription,const char* sParentResId,const char* sDepCode,const char* sEntCode)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->AddNoteRes(sNoteName,sDescription,sParentResId,sDepCode,sEntCode);
//	}
//	return -1;
//}
//int Cusermanager::EditRes(const char* sResourceId,const char* sName,const char* sDescription)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->EditRes(sResourceId,sName,sDescription);
//	}
//	return -1;
//}
//int Cusermanager::DeleteRes(const char* sResourceId)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->DeleteRes(sResourceId);
//	}
//	return -1;
//}

CEBAccountInfo::pointer Cusermanager::GetMyAccountInfo(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->m_pMyAccountInfo;
	}
	return EB_Null_AccountInfo;//CEBAccountInfo::create("");
}

std::string Cusermanager::GetLogonAccount(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->m_pMyAccountInfo->GetAccount();
	}
	return "";
}

std::string Cusermanager::GetDescription(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->GetDescription();
	}
	return "";
}

void Cusermanager::GetLogonInfo(std::string& sOutUserName, std::string& sOutPassword) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		sOutUserName = pManager->m_pMyAccountInfo->GetUserName();
		if (sOutUserName.empty())
			sOutUserName = pManager->m_pMyAccountInfo->GetAccount();
		sOutPassword = pManager->m_pMyAccountInfo->GetPasswd();
	}
}
bool Cusermanager::ChangeLineState(EB_USER_LINE_STATE nNewLineState)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ChangeLineState(nNewLineState);
	}
	return false;
}
bool Cusermanager::UpdateInfo(const CPopParameterList& pInfoList)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->UpdateInfo(pInfoList);
	}
	return false;
}

EB_USER_LINE_STATE Cusermanager::GetLineState(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->GetLineState();
	}
	return EB_LINE_STATE_UNKNOWN;
}

bool Cusermanager::IsLogonVisitor(void) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL && pManager->m_pMyAccountInfo.get() != NULL)
	{
		return pManager->m_pMyAccountInfo->IsLogonVisitor();
	}
	return false;
}
bool Cusermanager::EnterChat(const char* sCallId, const char* sAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EnterChat(sCallId, sAccount);
	}
	return false;
}
bool Cusermanager::ExitChat(const char* sCallId, const char* sAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ExitChat(sCallId, sAccount);
	}
	return false;
}
/*
int Cusermanager::CallUser(const char * sCallee,const char* sEmpCode,const char* sExistCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallUser(sCallee,sEmpCode,sExistCallId,"");
	}
	return -1;
}
int Cusermanager::Call2Dep(const char* sCallId,const char* sToAccount,const char* sEmpCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->Call2Dep(sCallId,sToAccount,sEmpCode);
	}
	return -1;
}
int Cusermanager::CallDep(const char* sDepCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallDep(sDepCode);
	}
	return -1;
}
//EB_GroupInfo::pointer Cusermanager::Call2Dep(const CEBCallInfo::pointer& pCallInfo)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->Call2Dep(pCallInfo);
//	}
//	return NullPopDepartmentInfo;
//}

int Cusermanager::InviteCall(const char * sExistCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->InviteCall(sExistCallId, "");
	}
	return -1;
}
//int Cusermanager::InviteUser(cosnt char* sUserAccount, const char* sExistCallId)
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->InviteUser(sUserAccount, sExistCallId);
//	}
//	return -1;
//}

int Cusermanager::CallResponse(const char * sCallId,bool bAccept,const char* sEmpCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallAck(sCallId,bAccept?EB_CAT_ACCEPT:EB_CAT_REJECT,sEmpCode);
	}
	return -1;
}
int Cusermanager::CallCancel(const char * sCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->CallCancel(sCallId);
	}
	return -1;
}
void Cusermanager::ClearCallInfo(const char * sCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->ClearCallInfo(sCallId);
	}
}
void Cusermanager::GetCallUserList(const std::string & sCallId, std::vector<std::string> & pOutUserList)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->GetCallUserList(sCallId, pOutUserList);
	}
}
CEBCallInfo::pointer Cusermanager::GetCallInfo(const std::string & sCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallInfo(sCallId);
	}
	return NullCallInfo;
}
CEBAccountInfo::pointer Cusermanager::GetCallInfo(const std::string & sCallId, const std::string & sFromAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetCallInfo(sCallId, sFromAccount);
	}
	return NullAccountInfo;
}
CEbECardInfo::pointer Cusermanager::GetMyECardInfo(const char* sEmpCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetMyECardInfo(sEmpCode);
	}
	return NullEbECardInfo;
}
std::string Cusermanager::GetMyECardFromInfo(const char* sEmpCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		CEbECardInfo::pointer pECardInfo = pManager->GetMyECardInfo(sEmpCode);
		return pManager->GetECardFromInfo(pECardInfo);
	}
	return "";
}
CEbECardInfo::pointer Cusermanager::GetECardByFromInfo(const char * sFromInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetECardByFromInfo(sFromInfo);
	}
	return NullEbECardInfo;
}
bool Cusermanager::GetAccountInfoByFromInfo(const char * sFromInfo, CEBAccountInfo::pointer & pAccountInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetAccountInfoByFromInfo(sFromInfo, pAccountInfo);
	}
	return false;
}

int Cusermanager::ContactLoad(void)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ContactQuery();
	}
	return -1;
}
int Cusermanager::ContactEdit(const EB_ContactInfo::pointer& pPopContactInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ContactEdit(pPopContactInfo);
	}
	return -1;
}
int Cusermanager::ContactDelete(const char* sContactAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->ContactDelete(sContactAccount);
	}
	return -1;
}
EB_ContactInfo::pointer Cusermanager::GetContactInfo(const char* sContactAccount) const
{
	EB_ContactInfo::pointer pPopContactInfo;
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->theContactList.find(sContactAccount, pPopContactInfo);
	}
	return pPopContactInfo;
}
EB_ContactInfo::pointer Cusermanager::GetContactInfo(const EB_ContactInfo* pInPopContactInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pInPopContactInfo == NULL) return NullPopContactInfo;
		boost::mutex::scoped_lock lock(pManager->theContactList.mutex());
		CLockMap<std::string, EB_ContactInfo::pointer>::iterator pIter;
		for (pIter=pManager->theContactList.begin(); pIter!=pManager->theContactList.end(); pIter++)
		{
			EB_ContactInfo::pointer pPopContactInfo = pIter->second;
			if (pPopContactInfo.get() == pInPopContactInfo)
			{
				return pPopContactInfo;
			}
		}
	}
	return NullPopContactInfo;
}
void Cusermanager::GetContactList(std::vector<EB_ContactInfo::pointer> & pOutContactList) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		boost::mutex::scoped_lock lock(pManager->theContactList.mutex());
		CLockMap<std::string, EB_ContactInfo::pointer>::iterator pIter;
		for (pIter=pManager->theContactList.begin(); pIter!=pManager->theContactList.end(); pIter++)
		{
			EB_ContactInfo::pointer pPopContactInfo = pIter->second;
			pOutContactList.push_back(pPopContactInfo);
		}
	}
}

int Cusermanager::EntEdit(const EB_EnterpriseInfo::pointer& pEnterpriseInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EntEdit(pEnterpriseInfo);
	}
	return -1;
}
int Cusermanager::DepEdit(const EB_GroupInfo::pointer& pDepartmentInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DepEdit(pDepartmentInfo);
	}
	return -1;
}
int Cusermanager::DepDelete(const char* sDepCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DepDelete(sDepCode);
	}
	return -1;
}
int Cusermanager::DepExit(const char* sDepCode)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->DepExit(sDepCode);
	}
	return -1;
}
int Cusermanager::EmpEdit(const EB_MemberInfo::pointer& pEmployeeInfo,int nNeedEmpInfo)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EmpEdit(pEmployeeInfo,nNeedEmpInfo);
	}
	return -1;
}
int Cusermanager::EmpDelete(const char* sEmpCode, bool bDeleteAccount)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EmpDelete(sEmpCode,bDeleteAccount);
	}
	return -1;
}
int Cusermanager::EnterpriseLoad(const char* sDepCode,int nLoadEmp)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->EnterpriseLoad(sDepCode,nLoadEmp);
	}
	return -1;
}
EB_EnterpriseInfo::pointer Cusermanager::GetEnterpriseInfo(const char* sEntCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sEntCode==NULL || strlen(sEntCode)==0)
			return pManager->m_pDefaultEnterpriseInfo;
		EB_EnterpriseInfo::pointer pEnterpriseInfo;
		pManager->theEnterpriseList.find(sEntCode,pEnterpriseInfo);
		return pEnterpriseInfo;
	}
	return NullPopEnterpriseInfo;
}
void Cusermanager::LoadDepartmentInfo(const char* sEntCode, CEBSearchCallback * pCallback, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadDepartmentInfo(sEntCode, pCallback, dwParam);
	}
}
void Cusermanager::LoadContactInfo(CEBSearchCallback * pCallback, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->LoadContactInfo(pCallback, dwParam);
	}
}
void Cusermanager::SearchUserInfo(const char* sSearchKey, CEBSearchCallback * pCallback, unsigned long dwParam)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->SearchUserInfo(sSearchKey, pCallback, dwParam);
	}
}

EB_GroupInfo::pointer Cusermanager::GetDepartmentInfo(const char* sDepCode) const
{
	EB_GroupInfo::pointer pPopDepartmentInfo;
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->theDepartmentList.find(sDepCode, pPopDepartmentInfo);
	}
	return pPopDepartmentInfo;
}
EB_MemberInfo::pointer Cusermanager::GetEmployeeInfo(const EB_MemberInfo* pInPopEmployeeInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pInPopEmployeeInfo == NULL) return PopNullEmployeeInfo;
		boost::mutex::scoped_lock lock1(pManager->theDepartmentList.mutex());
		CLockMap<std::string, EB_GroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		{
			EB_GroupInfo::pointer pPopDepartmentInfo = pIter1->second;
			boost::mutex::scoped_lock lock2(pPopDepartmentInfo->m_pEmployeeList.mutex());
			CLockMap<tstring, EB_MemberInfo::pointer>::iterator pIter2 = pPopDepartmentInfo->m_pEmployeeList.begin();
			for (; pIter2!=pPopDepartmentInfo->m_pEmployeeList.end(); pIter2++)
			{
				EB_MemberInfo::pointer pPopEmployeeInfo = pIter2->second;
				if (pPopEmployeeInfo.get() == pInPopEmployeeInfo)
					return pPopEmployeeInfo;
			}
		}
	}
	return PopNullEmployeeInfo;
}
EB_MemberInfo::pointer Cusermanager::GetEmployeeInfo(const char* sDepCode, const char * sEmpAccount) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		EB_GroupInfo::pointer pDepartmentInfo;
		if (pManager->theDepartmentList.find(sDepCode, pDepartmentInfo))
		{
			EB_MemberInfo::pointer pPopEmployeeInfo;
			pDepartmentInfo->m_pEmployeeList.find(sEmpAccount, pPopEmployeeInfo);
			return pPopEmployeeInfo;
		}

		//boost::mutex::scoped_lock lock1(pManager->theDepartmentList.mutex());
		//CLockMap<std::string, EB_GroupInfo::pointer>::iterator pIter1 = pManager->theDepartmentList.begin();
		//for (; pIter1!=pManager->theDepartmentList.end(); pIter1++)
		//{
		//	EB_GroupInfo::pointer pPopDepartmentInfo = pIter1->second;
		//	EB_MemberInfo::pointer pPopEmployeeInfo;
		//	if ((pPopDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || pPopDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_PROJECT) &&
		//		pPopDepartmentInfo->m_pEmployeeList.find(sEmpAccount, pPopEmployeeInfo))
		//	{
		//		return pPopEmployeeInfo;
		//	}
		//}
	}
	return PopNullEmployeeInfo;
}
EB_MemberInfo::pointer Cusermanager::GetEmployeeInfo(const char* sEmpCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		EB_MemberInfo::pointer pPopEmployeeInfo;
		pManager->theEmployeeList.find(sEmpCode, pPopEmployeeInfo);
		return pPopEmployeeInfo;
	}
	return PopNullEmployeeInfo;
}

EB_MemberInfo::pointer Cusermanager::GetMyEmployeeInfo(const char* sDepCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (sDepCode == NULL || strlen(sDepCode) == 0)
			return pManager->m_pDefaultEntEmployeeInfo;
		else
		{
			EB_MemberInfo::pointer pMyEmployeeInfo;
			if (pManager->theMyEmployeeList.find(sDepCode,pMyEmployeeInfo))
			{
				return pMyEmployeeInfo;
			}else
			{
				return pManager->m_pDefaultEntEmployeeInfo;
			}
		}
	}
	return PopNullEmployeeInfo;
}
void Cusermanager::GetMyEmployeeInfo(std::vector<EB_MemberInfo::pointer>& pOutEmployeeInfo) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->GetMyEmployeeInfo(pOutEmployeeInfo);
	}
}

bool Cusermanager::ExistEmployee(const char* sDepCode, const char* sEmpAccount) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		EB_GroupInfo::pointer pPopDepartmentInfo;
		if (pManager->theDepartmentList.find(sDepCode, pPopDepartmentInfo))
		{
			return pPopDepartmentInfo->m_pEmployeeList.exist(sEmpAccount);
		}
	}
	return false;
}
bool Cusermanager::IsMyDepartment(const char* sDepCode) const
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->theMyEmployeeList.exist(sDepCode);
	}
	return false;
}
//
//void Cusermanager::GetMyDepartment(std::vector<EB_GroupInfo::pointer>& pOutDepartment) const
//{
//	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
//	if (pManager != NULL)
//	{
//		pManager->IsMyDepartment(pOutDepartment);
//	}
//}

void Cusermanager::SetVideoHwnd(const char* sCallId,HWND pHwnd)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		if (pHwnd==NULL)
		{
			pManager->m_pVideoHwnd.remove(sCallId);
		}else
		{
			pManager->m_pVideoHwnd.insert(sCallId, pHwnd);
		}
	}
}
void Cusermanager::DelVideoHwnd(const char* sCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		pManager->m_pVideoHwnd.remove(sCallId);
	}
}

int Cusermanager::VideoRequest(const char* sCallId,int nVideoType)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoRequest(sCallId,nVideoType);
	}
	return -1;
}
int Cusermanager::VideoAck(const char* sCallId,bool bAccept)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoAck(sCallId,bAccept);
	}
	return -1;
}
int Cusermanager::VideoEnd(const char* sCallId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->VideoEnd(sCallId);
	}
	return -1;
}
int Cusermanager::GetVideoUserId(const char* sCallId, int & pOutMyVideoUserId, std::vector<int>& pOutFromVideoUserId, std::vector<int>& pOutOnVideoUserId)
{
	CUserManagerApp * pManager = (CUserManagerApp*)m_handle;
	if (pManager != NULL)
	{
		return pManager->GetVideoUserId(sCallId,pOutMyVideoUserId,pOutFromVideoUserId,pOutOnVideoUserId);
	}
	return -1;
}
*/

