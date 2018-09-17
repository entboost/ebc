#ifndef __UserManagerApp_h__
#define __UserManagerApp_h__
#include "../include/POPCLogonCenter.h"
#include "../include/POPCUserManager.h"
#include "../include/EBCAppCenter.h"
#include "chatroom.h"
#include "../include/EBCallbackInterface.h"
#include "../include/chatroomobjects.h"
#include "../include/rsa.h"
#include "../include/SqliteExt.h"
//#include "bodb2.h"

#ifdef _QT_MAKE_
#include <QObject>
#endif

namespace entboost {

inline bool IsFullNumber(const char* pString, size_t nLen)
{
	for (size_t i=0;i<nLen; i++)
	{
		const char pChar = pString[i];
		if (pChar<'0' || pChar>'9')
			return false;
	}
	return true;
}

class CToSendInfo
{
public:
	typedef boost::shared_ptr<CToSendInfo> pointer;
    static CToSendInfo::pointer create(const EBFileString& sFilePath,mycp::bigint nResourceId)
	{
		return CToSendInfo::pointer(new CToSendInfo(sFilePath,nResourceId));
	}
    static CToSendInfo::pointer create(const EBFileString& sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile)
	{
		return CToSendInfo::pointer(new CToSendInfo(sFilePath,sTo,bPrivate,bOffFile));
	}
	static CToSendInfo::pointer create(const CEBChatRoomRichMsg::pointer& pRich,mycp::bigint sTo,bool bPrivate)
	{
		return CToSendInfo::pointer(new CToSendInfo(pRich,sTo,bPrivate));
	}
	bool IsRich(void) const {return m_rich.get()!=NULL?true:false;}

	CEBChatRoomRichMsg::pointer m_rich;
    EBFileString m_sFilePath;
	mycp::bigint m_sTo;
	bool m_bPrivate;
	bool m_bOffFile;
	mycp::bigint m_nMsgId;	// for send file
	mycp::bigint m_nResourceId;
public:
    CToSendInfo(const EBFileString& sFilePath,mycp::bigint nResourceId)
		: m_sFilePath(sFilePath), m_sTo(0),m_bPrivate(false),m_bOffFile(false)
		, m_nMsgId(0), m_nResourceId(nResourceId)
	{
	}
    CToSendInfo(const EBFileString& sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile)
		: m_sFilePath(sFilePath), m_sTo(sTo),m_bPrivate(bPrivate),m_bOffFile(bOffFile)
		, m_nMsgId(0), m_nResourceId(0)
	{
	}
	CToSendInfo(const CEBChatRoomRichMsg::pointer& pRich,mycp::bigint sTo,bool bPrivate)
		: m_rich(pRich), m_sTo(sTo),m_bPrivate(bPrivate),m_bOffFile(false)
		, m_nMsgId(0), m_nResourceId(0)
	{
	}
	CToSendInfo(void)
		: m_sTo(0),m_bPrivate(false),m_bOffFile(false)
		, m_nMsgId(0),m_nResourceId(0)
	{}
	virtual ~CToSendInfo(void)
	{
		m_rich.reset();
	}
};

class CToSendList
{
public:
	typedef boost::shared_ptr<CToSendList> pointer;
	static CToSendList::pointer create(mycp::bigint nCallId,mycp::bigint nGroupCode)
	{
		return CToSendList::pointer(new CToSendList(nCallId,nGroupCode));
	}
    void AddFileResource(const EBFileString& sFilePath,mycp::bigint nResourceId)
	{
		{
			BoostReadLock rdlock(m_list.mutex());
			CLockList<CToSendInfo::pointer>::iterator pIter = m_list.begin();
			for (; pIter!=m_list.end(); pIter++)
			{
				const CToSendInfo::pointer& pToSendInfo = *pIter;
				if (pToSendInfo->m_sFilePath==sFilePath && pToSendInfo->m_nResourceId==nResourceId)
				{
					return;
				}
			}
		}
		m_list.add(CToSendInfo::create(sFilePath,nResourceId));
	}
    void AddFilePath(const EBFileString& sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile)
	{
		{
			BoostReadLock rdlock(m_list.mutex());
			CLockList<CToSendInfo::pointer>::iterator pIter = m_list.begin();
			for (; pIter!=m_list.end(); pIter++)
			{
				const CToSendInfo::pointer& pToSendInfo = *pIter;
				if (pToSendInfo->m_sFilePath==sFilePath)
				{
					pToSendInfo->m_sTo = sTo;
					pToSendInfo->m_bPrivate = bPrivate;
					pToSendInfo->m_bOffFile = bOffFile;
					return;
				}
			}
		}
		m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
	}
	mycp::bigint m_nCallId;
	mycp::bigint m_nGroupCode;
	CLockList<CToSendInfo::pointer> m_list;

	CToSendList(mycp::bigint nCallId,mycp::bigint nGroupCode)
		: m_nCallId(nCallId),m_nGroupCode(nGroupCode)
	{}
	CToSendList(void)
		: m_nCallId(0),m_nGroupCode(0)
	{}
};

class CP2PTaskInfo
{
public:
	typedef boost::shared_ptr<CP2PTaskInfo> pointer;
	static CP2PTaskInfo::pointer create(void)
	{
		return CP2PTaskInfo::pointer(new CP2PTaskInfo());
	}
	CCrP2PInfo m_pP2PInfo;
	time_t m_tP2PTry;
	bool m_bP2PResponse;
	CP2PTaskInfo(void)
		: m_tP2PTry(0),m_bP2PResponse(false)
	{}
};

class CFilePathInfo
{
public:
	typedef boost::shared_ptr<CFilePathInfo> pointer;
    enum FILE_PATH_TYPE
	{
		FILE_PAHT_HEAD
		, FILE_PAHT_RESOURCE
		, FILE_ENT_IMAGE		// resource too
		, FILE_FUNC_IMAGE		// resource too
	};
    static CFilePathInfo::pointer create(FILE_PATH_TYPE nType,const EBFileString &sFilePath,bool bDownload)
	{
		return CFilePathInfo::pointer(new CFilePathInfo(nType,sFilePath,bDownload));
	}
	CFilePathInfo(void)
		: m_nType(FILE_PAHT_HEAD)
		, m_bDownload(false)
		, m_nBigData(0), m_nMsgId(0)
	{
	}
    CFilePathInfo(FILE_PATH_TYPE nType,const EBFileString &sFilePath,bool bDownload)
		: m_nType(nType)
		, m_sFilePath(sFilePath)
		, m_bDownload(bDownload)
		, m_nBigData(0), m_nMsgId(0)
	{
	}
	CFilePathInfo(const CFilePathInfo& pObj)
		: m_nType(FILE_PAHT_HEAD)
		, m_bDownload(false)
		, m_nBigData(0), m_nMsgId(0)
	{
		CFilePathInfo::operator=(pObj);
	}
	const CFilePathInfo& operator =(const CFilePathInfo& pObj)
	{
		m_nType = pObj.m_nType;
		m_sFilePath = pObj.m_sFilePath;
		m_bDownload = pObj.m_bDownload;
		m_sParameter = pObj.m_sParameter;
		m_nBigData = pObj.m_nBigData;
		m_nMsgId = pObj.m_nMsgId;
		return *this;
	}
public:
	FILE_PATH_TYPE m_nType;
    EBFileString m_sFilePath;
	bool m_bDownload;
	tstring m_sParameter;
	mycp::bigint m_nBigData;
	mycp::bigint m_nMsgId;
	CEBContactInfo::pointer m_pContactInfo;	// for contact head info
	CEBAccountInfo::pointer m_pAccountInfo;	// for account head info
	CEBMemberInfo::pointer m_pEmployeeInfo;	// for memeber head info
};

#define USES_RESEND_FILE_2

class CProcessMsgInfo
{
public:
	typedef boost::shared_ptr<CProcessMsgInfo> pointer;
    enum PROCESS_MSG_TYPE {
		PROCESS_MSG_TYPE_RESET_DEVAPPID
		, PROCESS_MSG_TYPE_F_GROUP_INFO
		, PROCESS_MSG_TYPE_F_MEMBER_INFO
		, PROCESS_MSG_TYPE_LOAD_GROUP_OLS
		, PROCESS_MSG_TYPE_INVITE_CALL
		, PROCESS_MSG_TYPE_CALL_ACCEPT
		, PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE
		, PROCESS_MSG_TYPE_WAIT_ONCANCEL_FILE
		, PROCESS_MSG_TYPE_TO_SEND_LIST
		, PROCESS_MSG_TYPE_LOAD_RESOURCE_INFO
		, PROCESS_MSG_TYPE_SEND_GROUP_FILE
		, PROCESS_MSG_TYPE_WAIT_LOCAL_P2P_OK
#ifdef USES_RESEND_FILE_2
		, PROCESS_MSG_TYPE_RESEND_FILE
#endif
		, PROCESS_MSG_TYPE_P2P_REQUEST
		, PROCESS_MSG_TYPE_CHECK_RESOURCE_REFRESH
		, PROCESS_MSG_TYPE_DELETE_HEAD_RESFILE
		//, PROCESS_MSG_TYPE_CHECK_MEMBER_HEAD
#ifdef Q_OS_ANDROID
        , PROCESS_MSG_TYPE_GET_PUSH_TOKEN
#endif
        , PROCESS_MSG_TYPE_LOG_MESSAGE				= 200
	};
	static CProcessMsgInfo::pointer create(PROCESS_MSG_TYPE nMsg)
	{
		return CProcessMsgInfo::pointer(new CProcessMsgInfo(nMsg));
	}
	PROCESS_MSG_TYPE GetProcessMsgType(void ) const {return m_nProcessMsgType;}
	CEBGroupInfo::pointer m_pDepartmentInfo;
	CEBMemberInfo::pointer m_pEmployeeInfo;	// for memeber head info
	bool m_bNewMemberInfo;
	mycp::bigint m_nReturnVer;
	mycp::bigint m_nCallGroupId;
	CEBCallInfo::pointer m_pCallInfo;
	CEBAccountInfo::pointer m_pFromAccount;
	mycp::bigint m_nFromUserId;
	Cchatroom::pointer m_pChatRoom;
    mycp::bigint m_nBigInt1;	// resource_id,msgid,userid
	mycp::bigint m_nBigInt2;	// callid
    void *m_param;
    EBFileString m_sString1;	// resource_file
	time_t m_tProcessTime;
    CPOPSotpRequestInfo::pointer m_pSotpRequestInfo;
    CPOPSotpResponseInfo::pointer m_pSotpResponseInfo;

	CProcessMsgInfo(PROCESS_MSG_TYPE nMsg)
		: m_nProcessMsgType(nMsg)
		, m_bNewMemberInfo(false)
		, m_nReturnVer(0)
		, m_nCallGroupId(0)
		, m_nFromUserId(0)
        , m_nBigInt1(0), m_nBigInt2(0), m_param(0)
		, m_tProcessTime(0)
	{}
	virtual ~CProcessMsgInfo(void)
	{}	
private:
	PROCESS_MSG_TYPE m_nProcessMsgType;
};

class CUserManagerApp
	: public CPOPLCHandler
	, public CPOPUMHandler
	, public CEBAPHandler
	, public CChatRoomCallBack
#ifdef _QT_MAKE_
    , QObject
#endif
{
public:
    enum UserStatus {
		US_Logout
		, US_DevAppIdLogoning
		, US_Logging
		, US_LogonError
		, US_OAuthForward
		, US_Logoned
		, US_Invalidate
		, US_OnlineAnother	// 在其他地方登录
	};
	CUserManagerApp(void);
	virtual ~CUserManagerApp(void);
//#ifdef _DEBUG
//	tstring abctest3(int a) const {return "abc";}
//	int abctest1(int a) const {return 22+a;}
//#endif

	int LogonAppCenter(void);
	int LogoutAppCenter(void);
	int SendAPMsg(const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg);
	int APMsgAck(mycp::bigint nMsgId);
	int UmMsgAck(mycp::bigint nMsgId,EB_CALL_ACK_TYPE nAckType=EB_CAT_UNKNOWN);

	void SetInitParameter(const tstring& sInitParameter) {m_sInitParameter = sInitParameter;}
	void SetLogonUserData(const tstring& sLogonUserData) {m_sLogonUserData = sLogonUserData;}
	void SetServerAddress(const tstring& sAddress,const tstring& sOrgAddress);
	const tstring& GetServerAddress(void) const {return m_sServerAddress;}
	UserStatus GetUserStatus(void) const {return m_userStatus;}
	EB_STATE_CODE GetLastStateCode(void) const {return m_nLastStateCode;}

	int SetDevAppId(mycp::bigint sAppId, const tstring& sAppKey,bool bReLoadAppOnlineKey);
	void SetDevAppOnlineKey(mycp::bigint sAppId, const tstring& sAppOnlineKey);
	mycp::bigint GetDevAppId(void) const {return m_sDevAppId;}
	tstring GetDevAppOnlineKey(void) const {return m_sDevAppOnlineKey;}

	int Register(const tstring& sAccount, const tstring& sPwd, const tstring& sUserName="", const tstring& sEntName="");
	void SaveLCAddressIndex(int nIndex);
	void SaveLCAddressList(const tstring& sFullAddress);
	tstring GetLCAddressList(int& nOutLCIndex, time_t& pOutLastTime);
	void SaveAddressList(const tstring& sFullAddress);
	tstring GetAddressList(const tstring& sFullAddress);
	void GetLocalIp(void);
	int Logon(const CPOPLogonInfo::pointer & pLogonInfo, bool bReLogon = false);
	int ReLogon(void);
	int m_nLoadSubFunc;
	void LoadInfo(int nLoadSubFunc=1,int nLoadMsg=1,int nLoadGroupVer=0,mycp::bigint nLoadOLSGroupId=0, int nLoadEntGroupOLS=0,int nLoadUserGroupOLS=0, mycp::bigint nLoadGroupOLSGid=0, const tstring& sLoadGroupOLSGid="");
    void Logout(bool acceptPush);

	int GetSubGroupMemberSize(eb::bigint nParentGroupId) const;
	bool GetSubGroupMemberSize(eb::bigint nParentGroupId, int& pOutMemberSize, int& pOutOnlineSize) const;
	// 参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	int GetGroupMemberSize(eb::bigint nGroupId, int nGetType) const;
	bool GetGroupMemberSize(eb::bigint nGroupId, int nGetType,int& pOutMemberSize, int& pOutOnlineSize) const;
	int GetSubGroupOnlineSize(eb::bigint nParentGroupId) const;
	// 参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	int GetGroupOnlineSize(eb::bigint nGroupId, int nGetType) const;

	int GetAreaInfo(int nParent, unsigned int nParameter=0);
	EB_VersionInfo m_pVersionInfo;
	int CheckVersion(const tstring& sClientVersion);
	int SetDepHead(mycp::bigint nGroupId,const tstring& sFilePath);
	int SetDepResHead(mycp::bigint nGroupId,mycp::bigint sResourceId);

	int LoadRes(int nVerifyValid,mycp::bigint nResourceId,int nType,mycp::bigint sFromId,EB_RESOURCE_FROM_TYPE nFromType,int nOffset=-1,int nLimit=1);
	int AddDirRes(const tstring& sDirName,mycp::bigint sParentResId=0,mycp::bigint nFromId=0, EB_RESOURCE_FROM_TYPE nFromType=EB_RESOURCE_FROM_TYPE_UNKNOWN);
	//int AddDirRes(const tstring& sDirName,mycp::bigint sParentResId=0,mycp::bigint nGroupId=0,mycp::bigint sEntCode=0);
	int AddFileRes(const tstring& sFileName,const tstring& sFilePath,const tstring& sDescription="",mycp::bigint sParentResId=0,EB_RESOURCE_SHARE_TYPE nShare=EB_RESOURCE_SHARE_UNKNOWN, mycp::bigint nFromId=0, EB_RESOURCE_FROM_TYPE nFromType=EB_RESOURCE_FROM_TYPE_UNKNOWN);
	//int AddFileRes(const tstring& sFileName,const tstring& sFilePath,const tstring& sDescription="",mycp::bigint sParentResId=0,mycp::bigint nGroupId=0,mycp::bigint sEntCode=0,int nShare=0);
	int AddNoteRes(const tstring& sNoteName,const tstring& sDescription="",mycp::bigint sParentResId=0,mycp::bigint nFromId=0, EB_RESOURCE_FROM_TYPE nFromType=EB_RESOURCE_FROM_TYPE_UNKNOWN);
	//int AddNoteRes(const tstring& sNoteName,const tstring& sDescription="",mycp::bigint sParentResId=0,mycp::bigint nGroupId=0,mycp::bigint sEntCode=0);
	int EditRes(mycp::bigint sResourceId,const tstring& sName,const tstring& sDescription);
	int MoveRes2Dir(mycp::bigint sResourceId,mycp::bigint sParentResId);
	bool HasSubRes(mycp::bigint sParentResId) const;
	int DeleteRes(mycp::bigint sResourceId);
    int DownloadFileRes(mycp::bigint sResourceId,const EBFileString& sSaveTo);
	int CancelFileRes(mycp::bigint sResourceId,mycp::bigint nMsgId);

	mycp::bigint GetUserId(void) const;
	tstring GetOnlineKey(void) const;
	void SendMyLineState(EB_USER_LINE_STATE nNewLineState);
	bool ChangeLineState(EB_USER_LINE_STATE nNewLineState);
	bool UpdateInfo(const CPopParameterList& pInfoList);
	bool SetDefaltMemberCode(mycp::bigint sNewDefaultMemberCode);
	void LocalChatRoomP2PRequest(const CEBCallInfo::pointer& pCallInfo,const CEBAccountInfo::pointer& pAccountInfo, int nRequestData);
	bool EnterChat(mycp::bigint sCallId, mycp::bigint sAccount, bool bOffLine);
	bool ExitChat(mycp::bigint sCallId, mycp::bigint sAccount, bool bHangup,EB_USER_LINE_STATE& pOutLineState);
	CEBCallInfo::pointer GetExistCallInfo(mycp::bigint sFromAccount, mycp::bigint nGroupId);	// 查找现存会话

	int RDRequest(mycp::bigint sCallId,EB_REMOTE_DESKTOP_TYPE nRDType);
	int RDAck(mycp::bigint sCallId,bool bAccept,mycp::bigint nToUserId);
	int RDEnd(mycp::bigint sCallId);
	void CreateDesktop(const CEBCallInfo::pointer& pCallInfo);
	//void DoRDConnected(const CEBCallInfo::pointer& pCallInfo);
	//void DoRDDisonnecte(const CEBCallInfo::pointer& pCallInfo);

	int OpenLocalDesktop(eb::bigint sCallId, int quant, int fps);
	void EnableControlLocalDesktop(eb::bigint sCallId, eb::bigint nUserId);
	eb::bigint EnableControlLocalDesktop(eb::bigint sCallId) const;
	void CloseLocalDesktop(eb::bigint sCallId);
#ifdef _QT_MAKE_
	bool OpenRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, QObject* hWndParent);
#else
	bool OpenRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, HWND hWndParent);
#endif
	bool ControlRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, int nMouseMoveFPS);
	int ControlRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId) const;
	void MoveRemoteDesktopWindow(eb::bigint sCallId,mycp::bigint nUserId, int x, int y, int width, int height, bool bRepaint) const;
	void CloseRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId);
	void CloseAllRemoteDesktop(eb::bigint sCallId);
	void CloseAllDesktop(eb::bigint sCallId);

	bool GetCallRDUserInfo(mycp::bigint sCallId, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;
	void GetCallRDUserInfo(const CEBCallInfo::pointer& pCallInfo, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;

	int VideoRequest(mycp::bigint sCallId,EB_VIDEO_TYPE nVideoType);
	int VideoAck(mycp::bigint sCallId,bool bAccept,mycp::bigint nToUserId);
	int VideoEnd(mycp::bigint sCallId);
	bool GetCallVideoUserInfo(mycp::bigint sCallId, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;
    bool IsExistCallVideoUserInfo(mycp::bigint sCallId) const;
	void GetCallVideoUserInfo(const CEBCallInfo::pointer& pCallInfo, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const;
	void ProcessAudioStream(mycp::bigint nCallId, mycp::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam);
	void SetVideoCallback(eb::bigint sCallId, PEBVideoCallBack cbAudio);

	//PVideoDataCallBack m_pVideoStreamCallback;
	//DWORD m_dwVideoCallbackData;
#ifdef _QT_MAKE_
    std::vector<QString> m_pVideoDevices;
#else
	std::vector<tstring> m_pVideoDevices;
#endif
	int m_nLocalVideoIndex;
	void CreateVideoRoom(const CEBCallInfo::pointer& pCallInfo);
#ifdef _QT_MAKE_
    void OpenUserVideo(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bLocalUser,QWidget* hVideoWndParent, void* pAudioParam, bool& pOutVideoError);
#else
	void OpenUserVideo(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bLocalUser,HWND hVideoWndParent, void* pAudioParam, bool& pOutVideoError);
#endif
	bool SetVideoOpenClose(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bOpen);
	bool SetAudioOpenClose(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bOpen);
	void DoVideoConnected(const CEBCallInfo::pointer& pCallInfo);
	void DoVideoDisonnecte(eb::bigint sCallId, bool bSendVAck = false);
	void DoVideoDisonnecte(const CEBCallInfo::pointer& pCallInfo, bool bSendVAck = false);
#ifdef _QT_MAKE_
    int OpenLocalVideo(eb::bigint sCallId, QWidget *hVideoWndParent,void* pAudioParam);
    bool OpenUserVideo(eb::bigint sCallId,mycp::bigint sAccount, QWidget* hVideoWndParent,void* pAudioParam);
#else
	int OpenLocalVideo(eb::bigint sCallId,HWND hVideoWndParent,void* pAudioParam);
	bool OpenUserVideo(eb::bigint sCallId,mycp::bigint sAccount,HWND hVideoWndParent,void* pAudioParam);
#endif
	//bool OpenUserVideo(eb::bigint sCallId,mycp::bigint sAccount,PAudioDataCallBack cbAudio,void* pData);
	bool SetVideoOpenClose(eb::bigint sCallId,mycp::bigint sAccount,bool bOpen);
	bool SetAudioOpenClose(eb::bigint sCallId,mycp::bigint sAccount,bool bOpen);
	//bool OpenUserVideo(eb::bigint sCallId,int nUserVideoId,PAudioDataCallBack cbAudio,void* pData);

	bool m_bLogDebug;
	FILE * m_pLogDebug;
	void LogMessage(const char * format,...);
	void RichBufferAddText(eb::bigint sCallId,const char* sText);
	void RichBufferAddObject(eb::bigint sCallId,const char* pData,unsigned long nSize);
	void RichBufferAddResource(eb::bigint sCallId,const char* sResource);
	void RichBufferClear(eb::bigint sCallId);
	int SendCrRichBuffer(eb::bigint sCallId,mycp::bigint sTo,bool bPrivate);
	int SendCrRich(eb::bigint sCallId,const EB_ChatRoomRichMsg* pRichMsg,mycp::bigint sTo,bool bPrivate, bool bFromToSendList=false);
	int SendUserCard(eb::bigint sCallId,const tstring& sCardUserAccount,mycp::bigint sTo,bool bPrivate);
	int RequestWithdrawMsg(eb::bigint sCallId,mycp::bigint nMsgId);
	int RequestCollectMsg(eb::bigint sCallId,mycp::bigint nMsgId, bool bGroupCollection);
	bool DeleteOnlineFileList(eb::bigint sCallId,eb::bigint nMsgId);
	bool DeleteP2PFileList(eb::bigint sCallId,eb::bigint nMsgId);
    CLockMap<EBFileString,unsigned long> m_pWaitingProcessFileList;
    bool SendWaitingProcessCallback(eb::bigint sCallId,const EBFileString &sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile);
    int SendCrFile(eb::bigint sCallId,const EBFileString &sFilePath,mycp::bigint nResourceId);
    int SendCrFile(eb::bigint sCallId,const EBFileString &sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile, bool bFromToSendList=false,bool* pOutInviteCall=NULL,bool bNeedWaitingCallback=false);
    int SendCrFile(eb::bigint sCallId,const char* sFilePath,mycp::bigint nResourceId);
	int SendCrFile(eb::bigint sCallId,const char* sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile, bool bFromToSendList=false,bool* pOutInviteCall=NULL,bool bNeedWaitingCallback=false);
    int AcceptCrFile(eb::bigint sCallId,mycp::bigint nMsgId, const EBFileString &sSaveTo);
    int AcceptCrFile(eb::bigint sCallId,mycp::bigint nMsgId, const char* sSaveTo);
	int Save2CloudDrive(eb::bigint sCallId,mycp::bigint nMsgId);
	//int AcceptCrFileRes(eb::bigint sCallId,const char* sResId,const char* sSaveTo);
	int CancelCrFile(eb::bigint sCallId,mycp::bigint nMsgId);
	std::vector<tstring> m_pLocalHostIp;

	//int SendMsg(const tstring& sToAccount, const tstring& sEmpCode, EB_MSG_TYPE nMsgType, const tstring& sMsgContent, mycp::bigint nGroupId, bool bOffSave);//, const tstring& sFromInfo="");
	//int CEnter( const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int CallMember(mycp::bigint sMemberCode,mycp::bigint sExistCallId,mycp::bigint sOldCallId,bool bOwnerCall=false);
	int CallAccount(const tstring & sToAccount,mycp::bigint sExistCallId,mycp::bigint sOldCallId,bool bOwnerCall=false);
	int CallUserId(mycp::bigint nToUserId,mycp::bigint sExistCallId,mycp::bigint sOldCallId,bool bOwnerCall=false);
	int QueryUser(EB_REQEUST_TYPE nReqType,int nSubType,mycp::bigint nUserId,mycp::bigint nMemberId,mycp::bigint nGroupId=0, const tstring & sToAccount="",const CPOPSotpRequestInfo::pointer& pRequestInfo=NullPOPSotpRequestInfo);
	int Call2Dep(mycp::bigint sCallId,const tstring& sToAccount);
	int CallGroup(mycp::bigint nGroupId,bool bOwnerCall=false,bool bQueryForce=false);
	//CEBGroupInfo::pointer Call2Dep(const CEBCallInfo::pointer& pCallInfo);
	int GetOffLineCount(const CEBCallInfo::pointer & pCallInfo) const;
	int InviteCall(mycp::bigint sExistCallId, mycp::bigint sOldCallId, mycp::bigint sOnlyAccount=0,bool bOffReCall=true);
	int InviteCall(const CEBCallInfo::pointer & pCallInfo, mycp::bigint sOldCallId, mycp::bigint sOnlyAccount=0,bool bOffReCall=true, bool bSendFile=false,bool* pOutCanSendFileCall=NULL);
	int CallAck(mycp::bigint sCallId,EB_CALL_ACK_TYPE nAckType);
	int CallAck(const CEBCallInfo::pointer & pCallInfo, EB_CALL_ACK_TYPE nAckType);
	int CallHangup(mycp::bigint sCallId);
	int CallNotify(eb::bigint nCallId, eb::bigint nToUserId, int nNotifyType, eb::bigint nNotifyId, const char* sNotifyData);
	void ClearAllCallInfo(void);
	bool ClearCallInfo(mycp::bigint sCallId,bool bHangup, bool bSendHangup, bool bExitSes=false);
	bool GetCallUserList(mycp::bigint sCallId, std::vector<tstring> & pOutUserList) const;
	bool GetCallUserList(mycp::bigint sCallId, std::vector<mycp::bigint> & pOutUserList) const;
	bool GetCallUserList(mycp::bigint sCallId, std::vector<EB_AccountInfo> & pOutUserList) const;
	int GetCallOnlineSize(mycp::bigint sCallId) const;
	CEBCallInfo::pointer GetCallInfo(mycp::bigint sCallId) const;
	CEBAccountInfo::pointer GetCallInfo(mycp::bigint sCallId, mycp::bigint nFromUserId) const;
	// 电子名片
	//CEBECardInfo::pointer GetMyECardInfo(const tstring& sEmpCode, mycp::bigint nGroupId=0) const;	// 根据对方，获取电子名片
	//tstring GetECardFromInfo(const CEBECardInfo::pointer& pECardInfo) const;	// 根据对方，获取电子名片
	//tstring GetDepFromInfo(EB_ACCOUNT_TYPE nAccountType,const CEBGroupInfo::pointer& pDepartmentInfo) const;	// 根据群组，获取电子名片
	//EB_ACCOUNT_TYPE GetAccountTypeFromAccount(const tstring& sFromAccount) const;	// 根据对方，获取帐号类型
    static bool GetECardByFromInfo(const tstring& sFromInfo,EB_ECardInfo* pOutECardInfo);		// 根据对方，获取电子名片
	//bool GetAccountInfoByFromInfo(const tstring& sFromInfo, CEBAccountInfo::pointer & pAccountInfo) const;

	CEBCallInfo::pointer GetCallInfoByUser(mycp::bigint sAccount, tstring& pOutAccount, EB_USER_LINE_STATE& pOutLineState);
	CEBCallInfo::pointer GetCallInfoByUser(const tstring& sAccount, mycp::bigint& pOutUserId, EB_USER_LINE_STATE& pOutLineState);
	bool IsInCallUser(mycp::bigint sAccount, const CEBCallInfo::pointer & pCallInfo, tstring& pOutAccount,EB_USER_LINE_STATE& pOutLineState) const;
	bool IsInCallUser(const tstring& sAccount, const CEBCallInfo::pointer & pCallInfo,mycp::bigint& pOutUserId, EB_USER_LINE_STATE& pOutLineState) const;

	int UGInfoEit(mycp::bigint nUGId, const tstring& sGroupName);
	int UGInfoDelete(mycp::bigint nUGId);
    bool HasUGContact(eb::bigint nUGId) const;
    int GetUGContactSize(eb::bigint nUGId, int& pOutContactSize, int& pOutOnlineSize) const;
    int GetUGContactList(eb::bigint nUGId, std::vector<EB_ContactInfo> &pOutUGContactList) const;
	int ContactEdit(const EB_ContactInfo* pPopContactInfo);
	int ContactDelete(mycp::bigint nContactId,bool bDeleteDest);
	int ContactQuery(mycp::bigint nOnlyContactId = 0,mycp::bigint nOnlyContactUserId=0,int nLoadFlag=0,bool bAcceptContact=false);

	int EntEdit(const EB_EnterpriseInfo* pEnterpriseInfo);
	int DepEdit(const EB_GroupInfo* pDepartmentInfo);
	int SetGroupForbid(mycp::bigint nGroupId, bool bForbidSpeech, int nForbidMinutes);
	int DepDelete(mycp::bigint nGroupId);
	int JoinGroup(mycp::bigint nGroupId,const tstring& sDescription);
	int DepExit(mycp::bigint nGroupId);
	bool HasSubDepartment(eb::bigint sParentCode) const;
	int EmpEdit(const CEBMemberInfo* pEmployeeInfo,int nForbidMinutes,int nNeedEmpInfo);
	int SetMemberForbidSpeech(mycp::bigint nGroupId, mycp::bigint nMemberUserId,int nForbidMinutes);
	int EmpDelete(mycp::bigint sEmpCode, bool bDeleteAccount);
    int EmpDelete(mycp::bigint groupId, mycp::bigint userId, bool bDeleteAccount);
	bool m_bExistLocalImage;
	bool m_bExistLocalContact;
	int EnterpriseLoad(mycp::bigint nGroupId,int nLoadEntDep=1,int nLoadMyGroup=1,int nLoadEmp=1,mycp::bigint nMemberCode=0,mycp::bigint nMemberUid=0,bool bLoadImage=false,const tstring& sSearchKey="",bool bLoadFromEBC=false,const CPOPSotpRequestInfo::pointer& pRequestInfo = NullPOPSotpRequestInfo);

	// 组织架构
	void LoadDepartmentInfo(mycp::bigint sEntCode, CEBSearchCallback * pCallback, unsigned long dwParam);
	bool IsMyContact(mycp::bigint nContactUserId);
	bool GetContactInfo(mycp::bigint nContactUserId,EB_ContactInfo* pOutContactInfo) const;
    bool GetContactName(mycp::bigint nContactUserId, tstring &pOutContactName) const;
	CEBContactInfo::pointer GetContactInfo(mycp::bigint nContactUserId) const;
	void LoadContactInfo(CEBSearchCallback * pCallback, unsigned long dwParam=0);
	void SearchUserInfo(const tstring& sSearchKey, CEBSearchCallback * pCallback, unsigned long dwParam=0);
    void SearchGroupInfo(const tstring& sSearchKey, CEBSearchCallback * pCallback, unsigned long dwParam=0);
	CEBMemberInfo::pointer GetEmployeeInfo(mycp::bigint sAccount) const;
	CEBMemberInfo::pointer GetEmployeeInfo(const tstring& sAccount) const;
	CEBMemberInfo::pointer GetEmployeeInfo2(mycp::bigint sResourceId) const;
	CEBMemberInfo::pointer GetMyEntEmployeeInfo(void) const;	// 获取第一个企业部门成员名片
	bool GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<EB_MemberInfo>& pOutEmployeeInfo) const;
	bool GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<mycp::bigint>& pOutEmployeeInfo) const;
	bool GetGroupMemberCode(mycp::bigint sGroupCode,std::vector<mycp::bigint>& pOutEmployeeInfo) const;
	bool GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<tstring>& pOutEmployeeInfo) const;
	void GetMyEmployeeInfo(std::vector<EB_MemberInfo>& pOutEmployeeInfo) const;
	void GetMyDepartment(std::vector<CEBGroupInfo::pointer>& pOutDepartment) const;

	void GetEntManagerLevel(mycp::bigint sEnterpriseCode,std::vector<int>& pOutManagerLevelList) const;
	bool IsEnterpriseCreator(mycp::bigint sEnterpriseCode) const;
	bool IsEnterpriseCreator(mycp::bigint sEnterpriseCode, mycp::bigint nMemberUserId) const;
	bool GetFuncInfo(eb::bigint nSubId,EB_SubscribeFuncInfo* pOutFuncInfo) const;
    bool IsExistFuncInfo(eb::bigint nSubId) const;
	int GetFuncInfo(EB_FUNC_LOCATION nFuncLocation,std::vector<EB_SubscribeFuncInfo>& pOutFuncInfo) const;
	int GetFuncSize(EB_FUNC_LOCATION nFuncLocation) const;
	tstring GetSubscribeFuncUrl(eb::bigint nSubscribeId,eb::bigint nCallId,eb::bigint nFromUserId,const tstring& sFromAccount,eb::bigint nGroupId) const;
	tstring GetConversationsUrl(eb::bigint nFromUserId,eb::bigint nGroupId) const;
	tstring GetEntManagerUrl(void) const;
	tstring GetHomeIndexUrl(void) const;
	tstring GetLogonHttpReqUrl(void) const;

	mycp::bigint m_sDevAppId;
	tstring m_sDevAppKey;
	tstring m_sDevAppOnlineKey;
	std::vector<tstring> m_pLCList;
	int m_nCurrentIndex;
	time_t m_tBeginOfStart;
	tstring m_sAccountPrefix;
	int m_nOpenRegister;
	int m_nSystemSetting;
	bool m_bOpenVisitor;
	bool m_bLicenseUser;
	tstring m_sForgetPwdUrl;
	tstring m_sRegisterUrl;
	tstring m_sEntLogoUrl;
	tstring m_sProductName;
	//int m_nSendRegMail;
	PASSWD_AUTH_MODE m_nPasswdAuthMode;	// 密码验证方式，0：恩布默认
	tstring m_sEntManagerUrl;
	int m_nOnlineExtData;
	mycp::bigint m_nDeployId;
	int m_nLicenstType;	// 许可类型；0=未授权；1=终身授权；2=时间授权
	int m_nEBServerVersion;

	mycp::bigint m_nAutoOpenSubId;
	mycp::bigint m_nGroupMsgSubId;
	mycp::bigint m_nFindAppSubId;
	mycp::bigint m_nMyCollectionSubId;
	int m_nSaveConversations;
	tstring m_sConversationsUrl;
	tstring m_sDefaultUrl;
	tstring m_sAPServer;
	tstring m_sAPAppName;
	tstring m_sHttpServer;
	tstring m_sCallKey;
	int m_nSDKVersion;

	boost::shared_mutex m_mutexEBDatas; 
	CSqliteCdbc::pointer m_pEBDatas;
	CEBAccountInfo::pointer m_pMyAccountInfo;
	//const tstring& GetUserPassword(void) const {return m_sUserPassword;}

	CPOPCUserManager::pointer m_pUserManager;
	//EB_CALLBACK_INTERFACE * m_cbInterface;
	CEBCallbackInterface * m_callback;
#ifdef _QT_MAKE_
	QObject* m_pHwnd;
#else
	HWND m_pHwnd;
#endif
	CLockList<CEBContactInfo::pointer> m_pLogonToSendContactLineState;
	CLockList<CEBMemberInfo::pointer> m_pLogonToSendMemberLineState;
#ifdef _QT_MAKE_
	CLockMap<mycp::bigint,QObject*> m_pVideoHwnd;	// for video; callid->
	CLockMap<mycp::bigint,QObject*> m_pDesktopHwnd;	// for RD; callid->
#else
	CLockMap<mycp::bigint,HWND> m_pVideoHwnd;	// for video; callid->
	CLockMap<mycp::bigint,HWND> m_pDesktopHwnd;	// for RD; callid->
#endif

	CLockMap<mycp::bigint,bool> theSendFileCallTime;	// call-id->
	CLockMap<mycp::bigint, CPOPCUserManager::pointer> theUserManagerList;		// account/callid->
	CLockMap<mycp::bigint, CEBCallInfo::pointer>		 theCallInfoList;			// call-id->
	CLockMap<mycp::bigint, bool>						 theOwnerCallUser;			// uid->
	CLockMap<mycp::bigint, bool>						 theOwnerCallGroup;			// group_id->
	//CLockMap<mycp::bigint, bool>						 theOffLineUser;			// account->
	//CLockMap<mycp::bigint, bool>						 theAcceptUser;			// uid->
	CLockMap<mycp::bigint, CEBUGInfo::pointer>			theUGInfoList;			// ugid->
	CLockMap<mycp::bigint, CEBUGInfo::pointer>			theEitUGInfoList;		// ugid->
	CLockMap<mycp::bigint, CEBContactInfo::pointer>		theContactList1;			// contact_id->
	CLockMap<mycp::bigint, CEBContactInfo::pointer>		theContactList2;			// uid->
	CLockMap<mycp::bigint, CEBContactInfo::pointer>		theEditContactList;		// contact_id->
	// 在线资源
	CLockMap<mycp::bigint, CEBResourceInfo::pointer>	theResourceList;		// resid->
	CLockMultiMap<mycp::bigint, CEBResourceInfo::pointer>	theTempResourceList;	// resid->
	tstring m_sSearchNullKey;

	// 组织架构
	CEBEnterpriseInfo::pointer m_pDefaultEnterpriseInfo;
	CLockMap<mycp::bigint, CEBEnterpriseInfo::pointer>	theEnterpriseList;		// ent_code->
	CEBMemberInfo::pointer m_pDefaultEntEmployeeInfo;					// 默认企业部门员工（??如果在二个部门中，怎么办?）
	CEBEnterpriseInfo::pointer m_pEditEnterpriseInfo;
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>	theDepartmentList;		// depcode->
	CLockList<mycp::bigint>							theNeedLoadGroupMemberList;	// group-id->
	CLockList<mycp::bigint>							theNeedLoadGroupInfoList;	// group-id->
	CLockMap<mycp::bigint,bool>						theCheckGroupInfoList;		// group-id->
	CLockMap<mycp::bigint,time_t>					theCheckGroupMemberList;		// group-id->
	CLockMap<mycp::bigint,time_t>					theGroupMemberDataTime;		// group-id->
	CLockList<mycp::bigint>							theNeedLoadOLSList;		// group-id->
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>	theEmployeeList;		// emp_code->
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>	theMyEmployeeList;		// dep_code->
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>	theEditDepartmentList;		// depcode->
	CLockMultiMap<mycp::bigint, CEBGroupInfo::pointer>	theTempDepartmentList;	// depcode->
	CLockMultiMap<mycp::bigint, CEBMemberInfo::pointer>	theTempMemberList;		// depcode->
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>	theEditEmployeeList;		// emp_uid-> ??emp_account->
	CLockMap<tstring,Cchatroom::pointer> theChatRoomList;					// cmserver->
	CLockMap<mycp::bigint,Cchatroom::pointer> theResourceChatRoom;				// resourceid->
	CLockMap<mycp::bigint,mycp::bigint> theResource2CallId;				// resid->
	CLockMap<mycp::bigint,mycp::bigint> theResource2MsgId;				// resid->
	CLockMap<mycp::bigint,CFilePathInfo::pointer> theResourceFilePath;								// resourceid->
	CLockMap<tstring,CEBEmotionInfo::pointer> theEmotionList;	// resourceid+entcode->
	tstring m_pFuncUrl;	// um func url
	CLockMap<mycp::bigint,CEBSubscribeFuncInfo::pointer> theSubscribeFuncInfolist;	// sub_id->
	CLockMap<mycp::bigint,CToSendList::pointer> theCallToSendList;				// callid->
	CLockMap<mycp::bigint,CToSendList::pointer> theOnlineFileList;				// callid->*；在线发送文件列表
	CLockMap<mycp::bigint,CToSendList::pointer> theP2PFileList;					// callid->*；P2P发送文件列表
	CLockMap<mycp::bigint,bool> theChangeP2PMsgList;								// msgid->*
#ifdef USES_RESEND_FILE_2
	CLockMap<mycp::bigint,CToSendInfo::pointer> theREsendIdList;			// msgid->*
#endif
	CLockMap<int,CEBAreaInfo::pointer> theAreaInfoList;

	CLockList<Cchatroom::pointer> m_pClearChatRoom;
	CLockList<CPOPCUserManager::pointer> m_pHangupUm;
    const EBFileString& GetAppPath(void) const {return m_sAppPath;}
    const EBFileString& GetResourcePath(void) const {return m_sEbResourcePath;}
    const EBFileString& GetAppDataPath(void) const {return m_sAppDataPath;}
    const EBFileString& GetAppDataTempPath(void) const {return m_sAppDataTempPath;}

private:
    EBFileString m_sAppPath;
    EBFileString m_sImgPath;
    EBFileString m_sEbResourcePath;
    EBFileString m_sEbDataPath;
    EBFileString m_sAppDataPath;
    EBFileString m_sAppDataTempPath;
	tstring m_sServerAddress;
	tstring m_sLastErrorServerAddress;
	tstring m_sOrgServerAddress;
	std::vector<tstring> m_pOrgServerList;
	time_t m_tOrgBeginOfStart;
	int m_nOrgServerIndex;
	tstring m_sInitParameter;
	tstring m_sLogonUserData;
	time_t m_tLogonCenter;
	CPOPCLogonCenter::pointer m_pLogonCenter;
	CEBCAppCenter::pointer m_pAppCenter;
	CEBCAppCenter::pointer m_pAppCenterTemp;	// 用于临时清空
	UserStatus	m_userStatus;
	EB_SERVER_STATE m_nLcServerState;
	EB_SERVER_STATE m_nUmServerState;
	EB_SERVER_STATE m_nApServerState;
	//ServerStatus m_serverStatus;
	tstring m_sUserPassword;
	EB_STATE_CODE m_nLastStateCode;
	time_t m_tReLogonTime;
	time_t m_tReLoadInfo;
	CEBSysInfo m_pSysInfo;
	//CLockMap<tstring, CPOPCLogonCenter::pointer>	theAccountLogonCenterList;	// account->
	CLockMap<mycp::bigint,CP2PTaskInfo::pointer> m_pP2PTaskInfo;		// callid->
	CLockList<mycp::bigint> m_pP2POkList;		// callid->
	//mycp::bigint m_nP2POKToSendListCallId;
	bool m_bNeedLoadVerInfo;
	mycp::bigint m_nServerEntDepInfoVer;
	int theServerEntDepInfoSize;
	//int m_nEntDepInfoSize;
	mycp::bigint m_nServerMyDepInfoVer;
	mycp::bigint m_nServerContactInfoVer;
	//time_t		m_tReturnMyDepInfo;
	int m_nMyDepInfoSize;
	int m_nReturnMyGroupInfoSize;
	//time_t		m_tReturnEntDepInfo;
	int m_nEntDepInfoSize;
	int m_nReturnEntGroupInfoSize;

	CRSA m_pRsa;
	bool			m_bKilled;
	boost::shared_ptr<boost::thread> m_pProcessThread;
	void process_thread_svr(void);
	CLockList<CProcessMsgInfo::pointer> m_pProcessMsgList;
	CProcessMsgInfo::pointer m_pProcessCheckResourceRefresh;	// *有2个地方使用，所以不用担心不加锁会异常问题
	// 0=default
	// 1=取消群组共享文件，不需要处理
	CLockMap<mycp::bigint,int> m_pWaitList1;					// msgid/callid->
	CLockMap<mycp::bigint,int> m_pWaitList2;					// group_id/->
#ifdef Q_OS_ANDROID
    CLockMap<mycp::bigint,int> m_pSslIdList;					// ssl-id->[0/1 1表示，支持其他品牌消息推送]
    /// manufacturer	"Xiaomi"	QString
    /// HUAWEI
    QString m_manufacturer;
    eb::bigint m_nPushSslId;
    QString m_sPushToken;
    bool m_sentSPush;   /// 是否已经调用  eb_u_spush
//    int m_timerIdGetPushToken;
#endif
	void DoProcess(void);

#ifdef _QT_MAKE_
protected:
    long postWaitEventResult(QObject *receiver, EB_Event *event);
    long postWaitEventResult(QObject *receiver, CCrInfo *event);
    long waitEventResult(unsigned long resultKey,int waitMaxSecond,long defaultResult=0);
    CLockMap<unsigned long,long> m_eventResult;
    virtual void customEvent(QEvent *e);
    void doEBUMEvent(QEvent *e);

#ifdef Q_OS_ANDROID
    virtual void timerEvent(QTimerEvent *e);
    void getPushToken();
    void sendUMSPush();
#endif

#endif
private:
	static eb::bigint GetNextBigId(void)	// index++ 16
	{
		static unsigned short static_id_index = 0;
		char lpszNextId[24];
		sprintf(lpszNextId, "%d%04d%04d", 10000000+((int)time(0)%89999999),(++static_id_index)%10000,rand()%10000);
		return cgc_atoi64(lpszNextId);
	}
	static int GetNextId(void)
	{
		// <2100000000
		static unsigned short static_id_index = 0;
		char lpszNextId[24];
		sprintf(lpszNextId, "%04d%03d%03d", (int)time(0)%2000,rand()%1000,(++static_id_index)%1000);
		return atoi(lpszNextId);
	}
	virtual void OnProcessTimer(const CPOPCUserManager* pUMOwner);
	tstring GetNextLCAddress(void);
	void ProcessTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo, const void * p);
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCLogonCenter* pLCOwner);
	virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CEBCAppCenter* pAPOwner);
	virtual void OnInvalidateSession(int nResultCode,const CPOPCUserManager* pUMOwner);
	virtual void OnInvalidateSession(int nResultCode,const CPOPCLogonCenter* pLCOwner);
	virtual void OnInvalidateSession(int nResultCode,const CEBCAppCenter* pAPOwner);

	void ProcessResultValue(int nResultValue);
	virtual void OnResultValue(int nResultValue,const CPOPCUserManager* pUMOwner);
	virtual void OnResultValue(int nResultValue,const CPOPCLogonCenter* pLCOwner);
	virtual void OnResultValue(int nResultValue,const CEBCAppCenter* pAPOwner);

    std::vector<EBFileString> m_pExit2DeleteFileList;
	void DeleteOldHeadFile(const CEBMemberInfo::pointer& pEmployeeInfo);
	void DeleteOldHeadFile(mycp::bigint nHeadResourceId);
	CEBEmotionInfo::pointer GetEmotionInfo(mycp::bigint sResourceId) const;
	bool BuildGroupCallUserList(const CEBCallInfo::pointer& pCallInfo, mycp::bigint sGroupCode) const;
	void SaveLocalEmotionInfo(const CEBEmotionInfo::pointer& pEmotionInfo);
	void SetLocalEmoVer(mycp::bigint nEmoId,int nVersion);
	int GetLocalEmoVer(mycp::bigint nEmoId) const;
	void ProcessP2PMsgInfo(const CEBAppMsgInfo::pointer& pMsgInfo,bool & pOutAutoAck,EB_CALL_ACK_TYPE& pOutAckType,const CPOPSotpRequestInfo::pointer & pReqeustInfo);
	bool ProcUm(eb::bigint sCallId, mycp::bigint nFromUserId);

	void EnterRoom(const CEBCallInfo::pointer& pCallInfo);
	void SendConnectMsg(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nFromUserId,const tstring& sFromAccount, int nConnectFlag = 0, bool bSetCallStateConnection=true);
	size_t GetEntGroupSize(void) const;		// 获取企业部门数
	size_t GetUnEntGroupSize(void) const;	// 获取个人部门数
	void LoadLocalDepInfo(mycp::bigint nGroupId,mycp::bigint nServerGroupVer, bool bNeedLoadGroupInfo = true);

	/////////////////////////////////////
	// CChatRoomCallBack
	void ProcessToSendList(mycp::bigint nCallId, bool bGroupCall, bool bCheckCallTime);
	//void ProcessToSendList(mycp::bigint nCallId, bool bGroupCall, mycp::bigint nWaitFromUidP2PState=0);
	virtual void OnEnterRoom(const CCrInfo& pParameter);
	virtual void OnUserEnterRoom(const CCrAccountInfo& pAccountInfo);
	virtual void OnUserExitRoom(const CCrAccountInfo& pAccountInfo, bool bHangup);
	virtual void OnUserNotify(const CCrNotifyInfo& pNotifyInfo);
	virtual void OnSendRich(const CCrRichInfo& pCrMsgInfo);
	virtual int OnReceiveRich(const CCrRichInfo& pCrMsgInfo);
	virtual void OnMsgReceipt(const CCrRichInfo& pRichInfo, int nAckType);

	virtual void OnSendingFile(const CCrFileInfo& pFileInfo);
	virtual void OnSentFile(const CCrFileInfo& pFileInfo);
	virtual void OnCancelFile(const CCrFileInfo& pFileInfo);
	virtual int OnReceivingFile(const CCrFileInfo& pFileInfo);
	virtual void OnReceivedFile(const CCrFileInfo& pFileInfo);
	virtual void OnFilePercent(const CChatRoomFilePercent& pFilePercent);
	virtual void OnSave2CloudDrive(const CCrFileInfo& pFileInfo);
	virtual void OnInvalidateSession(const CCrInfo& pParameter);
	virtual void OnEnterAnother(const CCrInfo& pParameter);
	virtual void OnP2PRequest(const CCrP2PInfo& pP2PInfo);
	bool DoCheckP2PState(const CCrP2PInfo& pP2PInfo,const CEBAccountInfo::pointer& pFromAccount);
	bool DoP2PTask(const CCrP2PInfo& pP2PInfo,bool& pOutP2POk);
	virtual void OnP2PResponse(const CCrP2PInfo& pP2PInfo, int nFlag);
	virtual void OnP2POk(const CCrP2PInfo& pP2PInfo);

	//////////////////////////////
	// CEBAPHangler
	virtual void OnAPAOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner);
	virtual void OnAPAOffResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner);
	virtual void OnAPMsgResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner);
	virtual void OnAPFAMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner);
	virtual void OnAPAMackResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBCAppCenter* pAPOwner);

	/////////////////////////////////////
	// logon center
	virtual void OnLCULQueryResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCLogonCenter* pLCOwner);
	virtual void OnLCULLogonResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner);

	/////////////////////////////////////////
	// ****** user manager
	// other
	virtual void OnFuncReqResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFuncAuthResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnVersionCheckResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);

	// res
	virtual void OnResDeleteResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnResEditResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void ProcessSubResourceInfo(mycp::bigint sResId);
	virtual void OnResInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);

	// remote desktop
	virtual void OnRDRequestResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnRDAckResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFRDRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFRDAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFRDEnd(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);

    /// video
    void OnVRequestResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnVRequestResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
    void OnVAckResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnVAckResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
    void OnFVRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnFVRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
    void OnFVAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnFVAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
    void OnFVEnd(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo);
	virtual void OnFVEnd(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);

	void UpdateLocalGroupVer(mycp::bigint nGroupId,mycp::bigint nNewGroupVer) const;
	virtual void OnUMEMEmpDeleteResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void DeleteMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo, const CEBMemberInfo::pointer& pEmployeeInfo);
	virtual void OnUMEMEmpEditResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void UpdateLocalGroupInfoVer(mycp::bigint nEntId,mycp::bigint nNewGroupInfoVer, bool bSaveForce=false) const;
	virtual void OnUMEMDepDeleteResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void ClearGroupAllData(const CEBGroupInfo::pointer& pDepartmentInfo);
	virtual void OnUMEMDepEditResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMEMDepOpResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMEMEntEditResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFUMEMEntInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void SaveLocalMemberInfo(const CEBMemberInfo::pointer& pEmployeeInfo);
	void SaveLocalGroupInfo(const CEBGroupInfo::pointer& pDepartmentInfo);
	void SaveLocalContactInfo(const CEBContactInfo::pointer& pContactInfo);
	void ProcessGroupInfo(const CEBGroupInfo::pointer& pDepartmentInfo);
	void ProcessSubGroupInfo(mycp::bigint sGroupCode);
	mycp::bigint GetLocalEntGroupInfoVer(mycp::bigint nEntId) const;
	mycp::bigint GetLocalMyGroupInfoVer(mycp::bigint nUserId) const;
	mycp::bigint GetLocalMyContactInfoVer(mycp::bigint nUserId) const;
	void SetLocalMyContactInfoVer(mycp::bigint nUserId, mycp::bigint nNewContactInfoVer) const;
	//void SaveEntGroupInfo(mycp::bigint nEntId,mycp::bigint nNewGroupInfoVer);
	//void SaveMyGroupInfo(mycp::bigint nNewGroupInfoVer);
	//FILE * m_file;
	static tstring GetCmServerKey(const tstring& sCmServer);
	void OnFUMEMDepInfo(const CEBGroupInfo::pointer& pDepartmentInfo,mycp::bigint nGroupInfoVer,mycp::bigint nCallGroupId);
	virtual void OnFUMEMDepInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void ProcessContactHeadInfo(const CEBContactInfo::pointer& pContactInfo,bool bUserHeadChange);
	void ProcessAccountHeadInfo(const CEBAccountInfo::pointer& pAccountInfo);
	void ProcessMemberHeadInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bUserHeadChange=false);
	//void ProcessMemberHeadInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bUserHeadChange=false, bool bAddToCheckMemberHead=true);
	void ProcessMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo,const CEBMemberInfo::pointer& pEmployeeInfo,bool bSendGroupInfoEvent = false);
	mycp::bigint GetLocalGroupVer(mycp::bigint nGroupId) const;
	void SetLocalGroupEmpCount(mycp::bigint nGroupId,int nEmpCount) const;
	void SetLocalGroupVer(mycp::bigint nGroupId,mycp::bigint nNewGroupVer) const;
	void SaveGroupMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo);
	int GetLocalGroupMemberSize(mycp::bigint nGroupId) const;
	void OnFUMEMEmpInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bNewMemberInfo,mycp::bigint nGroupVer,bool bUserHeadChange);
	virtual void OnFUMEMEmpInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);

	virtual void OnUGEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void DeleteUG(mycp::bigint nUGId);
	virtual void OnUGDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void LoadLocalContactData(void);
	void ProcLoadContactData(void);
	int m_nLoadUGCount;
	int m_nLoadContactCount;
	virtual void OnUGInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMCMEdit(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMCMDelete(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMCMQuery(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFUMIUHangup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMIUHangup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFUMIUAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMIUAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMIUEnter(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMIUUser(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFUMIUEnter(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnFUMIUUser(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	void ProcessEmotionCount(int nEmoCount, const tstring& sEmoVerString);
	virtual void OnFUMIUMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMSMOfflineResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMSMLoadResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	//virtual void OnUMSMLoad2Response(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMSMOnlineResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMReg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMSInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMSHead(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
	virtual void OnUMQuery(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner);
};

} // namespace entboost

#endif // __UserManagerApp_h__
