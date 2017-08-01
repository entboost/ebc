// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the USERMANAGER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// USERMANAGER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __usermanager_h__
#define __usermanager_h__

#include <string>
//#include <vector>
#include <boost/shared_ptr.hpp>
#include "usermanagerhandle.h"
#include "../include/eb_objects.h"

// This class is exported from the usermanager.dll
class USERMANAGER_API Cusermanager
{
public:
	typedef boost::shared_ptr<Cusermanager> pointer;
	static Cusermanager::pointer create(void)
	{
		return Cusermanager::pointer(new Cusermanager());
	}
	Cusermanager(void);
	virtual ~Cusermanager(void);

	int Start(const char * address);
	std::string GetAddress(void) const;
	bool IsStart(void) const;
	//void SetCallBack(CUserManagerCallBack * pCallBack);
	void SetMsgHwnd(HWND pHwnd);
	void Stop(void);

	tstring GetAppPath(void) const;
	tstring GetResourcePath(void) const;
	tstring GetAppDatePath(void) const;
	tstring GetAppDataTempPath(void) const;

	// load app online key
	int SetDevAppId(const char* sAppId,const char* sAppKey,bool bReLoadAppOnlineKey=true);
	void SetDevAppOnlineKey(const char* sAppId, const char* sAppOnlineKey);
	void GetDevAppOnlineKey(std::string& sOutAppId, std::string& sOutAppOnlineKey);

	int Register(const char* sAccount, const char* sPwd, const char* sUserName="", const char* sEntName="");
	int LogonVisitor(int nLogonType=EB_LOGON_TYPE_PC|EB_LOGON_TYPE_VISITOR);
	int LogonAccount(const char * sAccount, const char * sPassword, int nLogonType=EB_LOGON_TYPE_PC|EB_LOGON_TYPE_EMAIL, EB_USER_LINE_STATE nNewLineState=EB_LINE_STATE_ONLINE);
	bool IsLogoned(void) const;
	void LoadInfo(void);
	void Logout(void);

	// <= 1Mb
	int SetDepHead(const char* sDepCode,const char* sFilePath);

	// resource
	//int LoadRes(const char* sDepCode="",const char* sEntCode="");
	//int AddDirRes(const char* sDirName,const char* sParentResId="",const char* sDepCode="",const char* sEntCode="");
	//int AddFileRes(const char* sFilePath,const char* sFileName="",const char* sDescription="",const char* sParentResId="",const char* sDepCode="",const char* sEntCode="");
	//int AddNoteRes(const char* sNoteName,const char* sDescription="",const char* sParentResId="",const char* sDepCode="",const char* sEntCode="");
	//int EditRes(const char* sResourceId,const char* sName,const char* sDescription);
	//int DeleteRes(const char* sResourceId);

	CEBAccountInfo::pointer GetMyAccountInfo(void) const;
	std::string GetLogonAccount(void) const;
	std::string GetDescription(void) const;
	void GetLogonInfo(std::string& sOutUserName, std::string& sOutPassword) const;
	bool ChangeLineState(EB_USER_LINE_STATE nNewLineState);
	bool UpdateInfo(const CPopParameterList& pInfoList);
	EB_USER_LINE_STATE GetLineState(void)const;
	bool IsLogonVisitor(void) const;
	bool EnterChat(const char* sCallId, const char* sAccount);	// 记录在线状况
	bool ExitChat(const char* sCallId, const char* sAccount);	// 记录在线状况
/*
	// call
	int CallUser(const char * sCallee,const char* sEmpCode,const char* sExistCallId);
	int Call2Dep(const char* sCallId,const char* sToAccount,const char* sEmpCode);
	int CallDep(const char* sDepCode);
	//EB_GroupInfo::pointer Call2Dep(const CEBCallInfo::pointer& pCallInfo);
	int InviteCall(const char * sExistCallId);					// 邀请所有会话用户，用于刚接收离线信息，第一次发送信息处理
	//int InviteUser(cosnt char* sUserAccount, const char* sExistCallId);	// 邀请用户进入指定会话
	int CallResponse(const char * sCallId,bool bAccept,const char* sEmpCode);
	int CallCancel(const char * sCallId);
	void ClearCallInfo(const char * sCallId);
	void GetCallUserList(const std::string & sCallId, std::vector<std::string> & pOutUserList);
	CEBCallInfo::pointer GetCallInfo(const std::string & sCallId);
	CEBAccountInfo::pointer GetCallInfo(const std::string & sCallId, const std::string & sFromAccount);
	CEbECardInfo::pointer GetMyECardInfo(const char* sEmpCode) const;
	std::string GetMyECardFromInfo(const char* sEmpCode) const;
	CEbECardInfo::pointer GetECardByFromInfo(const char * sFromInfo) const;
	bool GetAccountInfoByFromInfo(const char * sFromInfo, CEBAccountInfo::pointer & pAccountInfo) const;

	// contact
	int ContactLoad(void);
	int ContactEdit(const EB_ContactInfo::pointer& pPopContactInfo);
	int ContactDelete(const char* sContactAccount);
	EB_ContactInfo::pointer GetContactInfo(const char* sContactAccount) const;
	EB_ContactInfo::pointer GetContactInfo(const EB_ContactInfo* pInPopContactInfo) const;
	void GetContactList(std::vector<EB_ContactInfo::pointer> & pOutContactList) const;

	// enterprise
	int EntEdit(const EB_EnterpriseInfo::pointer& pEnterpriseInfo);
	int DepEdit(const EB_GroupInfo::pointer& pDepartmentInfo);
	int DepDelete(const char* sDepCode);
	int DepExit(const char* sDepCode);
	int EmpEdit(const EB_MemberInfo::pointer& pEmployeeInfo,int nNeedEmpInfo=0);
	int EmpDelete(const char* sEmpCode, bool bDeleteAccount=true);
	int EnterpriseLoad(const char* sDepCode="", int nLoadEmp = 1);
	EB_EnterpriseInfo::pointer GetEnterpriseInfo(const char* sEntCode="") const;
	// sEntCode="": load all
	void LoadDepartmentInfo(const char* sEntCode, CEBSearchCallback * pCallback, unsigned long dwParam=0);
	void LoadContactInfo(CEBSearchCallback * pCallback, unsigned long dwParam=0);
	void SearchUserInfo(const char* sSearchKey, CEBSearchCallback * pCallback, unsigned long dwParam=0);
	EB_GroupInfo::pointer GetDepartmentInfo(const char* sDepCode) const;
	EB_MemberInfo::pointer GetEmployeeInfo(const EB_MemberInfo* pInPopEmployeeInfo) const;
	EB_MemberInfo::pointer GetEmployeeInfo(const char* sDepCode, const char * sEmpAccount) const;
	EB_MemberInfo::pointer GetEmployeeInfo(const char* sEmpCode) const;
	EB_MemberInfo::pointer GetMyEmployeeInfo(const char* sDepCode="") const;
	void GetMyEmployeeInfo(std::vector<EB_MemberInfo::pointer>& pOutEmployeeInfo) const;
	bool ExistEmployee(const char* sDepCode, const char* sEmpAccount)const;
	bool IsMyDepartment(const char* sDepCode) const;
	//void GetMyDepartment(std::vector<EB_GroupInfo::pointer>& pOutDepartment) const;

	//**** video
	// VideoType: 1:audio; 2:audio&video
	void SetVideoHwnd(const char* sCallId,HWND pHwnd);
	void DelVideoHwnd(const char* sCallId);
	int VideoRequest(const char* sCallId,int nVideoType=2);
	int VideoAck(const char* sCallId,bool bAccept);
	int VideoEnd(const char* sCallId);
	int GetVideoUserId(const char* sCallId, int & pOutMyVideoUserId, std::vector<int>& pOutFromVideoUserId, std::vector<int>& pOutOnVideoUserId);
*/
private:
	void * m_handle;
};

#endif // __usermanager_h__
