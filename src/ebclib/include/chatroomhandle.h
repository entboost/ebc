// charroomhandle.h file here
#ifndef __charroomhandle_h__
#define __charroomhandle_h__

#ifdef CHATROOM_EXPORTS
#define CHATROOM_API __declspec(dllexport)
#else
#define CHATROOM_API __declspec(dllimport)
#endif

#include <string>
#include "chatroomobject.h"

namespace entboost {

class CCrInfo
{
public:
	CCrInfo(void)
		: m_nParam(0)
		, m_sCallId(0), m_sChatId(0)
		, m_nState(EB_STATE_OK)
	{
	}
	CCrInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: m_nParam(nParam)
		, m_sCallId(sCallId), m_sChatId(sChatId)
		, m_nState(nState)
	{
	}
	CCrInfo(const CCrInfo& pObj)
	{
		CCrInfo::operator=(pObj);
	}
	const CCrInfo& operator =(const CCrInfo& pParameter)
	{
		CCrInfo::operator =(&pParameter);
		return *this;
	}
	const CCrInfo& operator =(const CCrInfo* pParameter)
	{
		if (pParameter!=NULL)
		{
			m_nParam = pParameter->GetParam();
			m_sCallId = pParameter->GetCallId();
			m_sChatId = pParameter->GetChatId();
			m_nState = pParameter->GetStateCode();
		}
		return *this;
	}
	unsigned long GetParam(void) const {return m_nParam;}
	void SetParam(unsigned long v) {m_nParam = v;}
	cr::bigint GetCallId(void) const {return m_sCallId;}
	void SetCallId(cr::bigint v) {m_sCallId = v;}
	cr::bigint GetChatId() const {return m_sChatId;}
	void SetChatId(cr::bigint v) {m_sChatId = v;}
	void SetStateCode(EB_STATE_CODE nState) {m_nState = nState;}
	EB_STATE_CODE GetStateCode(void) const {return m_nState;}

protected:
	unsigned long m_nParam;
	cr::bigint m_sCallId;
	cr::bigint m_sChatId;
	EB_STATE_CODE m_nState;
};

//class CChatRoomFileInfo
//	: public CCrInfo
//{
//public:
//	CChatRoomFileInfo(void)
//		: m_nFileSize(0)
//		//, m_bIsMsgSender(false)
//		, m_bOffFile(false)
//	{
//	}
//	CChatRoomFileInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId)
//		: CCrInfo(nParam,sCallId,sChatId)
//		, m_nFileSize(0)
//		, m_bOffFile(false)
//	{
//	}
//	const CChatRoomFileInfo & operator =(const CChatRoomFileInfo * pChatRoomFileInfo)
//	{
//		CCrInfo::operator =(pChatRoomFileInfo);
//		m_sResId = pChatRoomFileInfo->m_sResId;
//		m_nMsgId = pChatRoomFileInfo->m_nMsgId;
//		m_sFileName = pChatRoomFileInfo->m_sFileName;
//		m_nFileSize = pChatRoomFileInfo->m_nFileSize;
//		//m_bIsMsgSender = pChatRoomFileInfo->m_bIsMsgSender;
//		m_bOffFile = pChatRoomFileInfo->m_bOffFile;
//		return *this;
//	}
//
//	CEBString m_sResId;
//	CEBString m_sResCmServer;
//	CEBString m_sResCmAppName;
//	CEBString m_nMsgId;
//	CEBString m_sFileName;
//	unsigned int m_nFileSize;
//	//bool m_bIsMsgSender;
//	CEBString m_sFileTime;	// null is current time
//	bool m_bOffFile;
//};


class CChatRoomFilePercent
	: public CCrInfo
{
public:
	CChatRoomFilePercent(void)
		: m_sResId(0),m_nMsgId(0)
		, m_percent(0.0)
		, m_nTranSeconds(0)
		, m_nLeftSeconds(0)
		, m_nCurSpeed(0)
		, m_nAvrSpeed(0)
	{
	}
	CChatRoomFilePercent(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_sResId(0),m_nMsgId(0)
		, m_percent(0.0)
		, m_nTranSeconds(0)
		, m_nLeftSeconds(0)
		, m_nCurSpeed(0)
		, m_nAvrSpeed(0)
	{
	}
	CChatRoomFilePercent(cr::bigint nMsgId, double percent, int nseconds1, int nseconds2, int speech1, int speech2)
		: m_sResId(0),m_nMsgId(nMsgId)
		, m_percent(percent)
		, m_nTranSeconds(nseconds1)
		, m_nLeftSeconds(nseconds2)
		, m_nCurSpeed(speech1)
		, m_nAvrSpeed(speech2)
	{
	}
	CChatRoomFilePercent(const CChatRoomFilePercent& pObj)
	{
		CChatRoomFilePercent::operator=(pObj);
	}
	const CChatRoomFilePercent& operator=(const CChatRoomFilePercent& pObj)
	{
		CCrInfo::operator =(&pObj);
		m_sResId = pObj.m_sResId;
		m_nMsgId = pObj.m_nMsgId;
		m_percent = pObj.m_percent;
		m_nTranSeconds = pObj.m_nTranSeconds;
		m_nLeftSeconds = pObj.m_nLeftSeconds;
		m_nCurSpeed = pObj.m_nCurSpeed;
		m_nAvrSpeed = pObj.m_nAvrSpeed;
		return *this;
	}
	cr::bigint m_sResId;
	cr::bigint m_nMsgId;
	double		m_percent;
	int			m_nTranSeconds;		// 传输时间，单位秒
	int			m_nLeftSeconds;		// 剩余时间，单位秒
	int			m_nCurSpeed;		// 当前速度 1=1Byte
	int			m_nAvrSpeed;		// 平均速度
};

//class CChatRoomSendInfo
//{
//public:
//	CChatRoomSendInfo(void)
//		: m_nPrivate(0)
//	{
//	}
//	CChatRoomSendInfo(const CEBString& sFrom, const CEBString& sTo, int nPrivate)
//		: m_sFrom(sFrom)
//		, m_sTo(sTo)
//		, m_nPrivate(nPrivate)
//	{
//	}
//
//	CEBString m_sFrom;
//	CEBString m_sTo;
//	int m_nPrivate;
//};

class CCrFileInfo
	: public CCrInfo
{
public:
	CCrFileInfo(void)
		: m_sResId(0),m_nMsgId(0),m_nFileSize(0)
		, m_bOffFile(false)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
		, m_sReceiveAccount(0)
	{
	}
	CCrFileInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_sResId(0),m_nMsgId(0),m_nFileSize(0)
		, m_bOffFile(false)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
		, m_sReceiveAccount(0)
	{
	}
	CCrFileInfo(const CCrFileInfo& pObj)
		: m_sResId(0),m_nMsgId(0),m_nFileSize(0)
		, m_bOffFile(false)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
		, m_sReceiveAccount(0)
	{
		CCrFileInfo::operator=(pObj);
	}
#ifdef USES_EBCOM_TEST
	CCrFileInfo(IEB_ChatFileInfo* pObj)
		: m_sResId(0),m_nMsgId(0),m_nFileSize(0)
		, m_bOffFile(false)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
		, m_sReceiveAccount(0)
	{
		CCrFileInfo::operator=(pObj);
	}
#endif
	const CCrFileInfo& operator=(const CCrFileInfo* pObj)
	{
		if (pObj!=NULL)
		{
			CCrFileInfo::operator =(*pObj);
		}
		return *this;
	}
	const CCrFileInfo& operator=(const CCrFileInfo& pObj)
	{
		CCrInfo::operator =(&pObj);
		m_sResId = pObj.m_sResId;
		m_sResCmServer = pObj.m_sResCmServer;
		m_sResCmAppName = pObj.m_sResCmAppName;
		m_nMsgId = pObj.m_nMsgId;
		m_sFileName = pObj.m_sFileName;
		m_nFileSize = pObj.m_nFileSize;
		m_sFileTime = pObj.m_sFileTime;
		m_bOffFile = pObj.m_bOffFile;
		m_sSendFrom = pObj.m_sSendFrom;
		m_sSendTo = pObj.m_sSendTo;
		m_bPrivate = pObj.m_bPrivate;
		m_sReceiveAccount = pObj.m_sReceiveAccount;
		m_sMD5 = pObj.m_sMD5;
		return *this;
	}
#ifdef USES_EBCOM_TEST
	const CCrFileInfo& operator=(IEB_ChatFileInfo* pObj)
	{
		if (pObj != NULL)
		{
			m_nParam = pObj->Param;
			m_sCallId = pObj->CallId;
			m_sChatId = pObj->ChatId;
			m_sResId = pObj->ResId;
			m_sResCmServer = pObj->ResCmServer.GetBSTR();
			m_sResCmAppName = pObj->ResCmAppName.GetBSTR();
			m_nMsgId = pObj->MsgId;
			m_sFileName = pObj->FileName.GetBSTR();
			m_nFileSize = pObj->FileSize;
			m_sFileTime = pObj->FileTime.GetBSTR();
			m_bOffFile = pObj->OffFile?true:false;
			m_sSendFrom = pObj->SendFrom;
			m_sSendTo = pObj->SendTo;
			m_bPrivate = pObj->Private?true:false;
			m_sReceiveAccount = pObj->ReceiveAccount;
			m_sMD5 = pObj->MD5.GetBSTR();
		}
		return *this;
	}
#endif
	//CEBString m_sChatId;
	//CChatRoomFileInfo m_pFileInfo;
	cr::bigint m_sResId;
	CEBString m_sResCmServer;
	CEBString m_sResCmAppName;
	cr::bigint m_nMsgId;
	CEBString m_sFileName;
	cr::bigint m_nFileSize;
	//bool m_bIsMsgSender;
	CEBString m_sFileTime;	// null is current time
	bool m_bOffFile;
	//CChatRoomSendInfo m_pSendInfo;
	cr::bigint m_sSendFrom;
	cr::bigint m_sSendTo;
	bool m_bPrivate;
	cr::bigint m_sReceiveAccount;
	CEBString m_sMD5;
};

class CCrRichInfo
	: public CCrInfo
{
public:
	CCrRichInfo(void)
		: m_pRichMsg(NULL)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
	{
	}
	CCrRichInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_pRichMsg(NULL)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_bPrivate(false)
	{
	}
	CCrRichInfo(const CCrRichInfo& pObj)
	{
		CCrRichInfo::operator=(pObj);
	}
	const CCrRichInfo& operator=(const CCrRichInfo& pObj)
	{
		CCrInfo::operator=(&pObj);
		m_pRichMsg = pObj.m_pRichMsg;
		m_sSendFrom = pObj.m_sSendFrom;
		m_sSendTo = pObj.m_sSendTo;
		m_bPrivate = pObj.m_bPrivate;
		return *this;
	}
	EB_ChatRoomRichMsg* m_pRichMsg;
	//CChatRoomSendInfo m_pSendInfo;
	cr::bigint m_sSendFrom;
	cr::bigint m_sSendTo;
	bool m_bPrivate;
};

class CCrAccountInfo
	: public CCrInfo
{
public:
	CCrAccountInfo(void)
		: m_sAccount(0)
		, m_bOffLine(false)
	{
	}
	CCrAccountInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_sAccount(0)
		, m_bOffLine(false)
	{
	}
	CCrAccountInfo(const CCrAccountInfo& pObj)
		: m_sAccount(0)
		, m_bOffLine(false)
	{
		CCrAccountInfo::operator=(pObj);
	}
	const CCrAccountInfo& operator=(const CCrAccountInfo& pObj)
	{
		CCrInfo::operator=(&pObj);
		m_sAccount = pObj.m_sAccount;
		m_sFromInfo = pObj.m_sFromInfo;
		m_bOffLine = pObj.m_bOffLine;
		return *this;
	}
	cr::bigint m_sAccount;
	CEBString m_sFromInfo;
	bool m_bOffLine;
};

class CCrP2PInfo
	: public CCrInfo
{
public:
	CCrP2PInfo(void)
		: m_sFromAccount(0)
		, m_nP2PData(0)
	{
	}
	CCrP2PInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_sFromAccount(0)
		, m_nP2PData(0)
	{
	}
	CCrP2PInfo(const CCrP2PInfo& pObj)
		: m_sFromAccount(0)
		, m_nP2PData(0)
	{
		CCrP2PInfo::operator=(pObj);
	}
	const CCrP2PInfo& operator=(const CCrP2PInfo& pObj)
	{
		CCrInfo::operator=(&pObj);
		m_sFromAccount = pObj.m_sFromAccount;
		m_sFromLocalAddress = pObj.m_sFromLocalAddress;
		m_sFromRemoteAddress = pObj.m_sFromRemoteAddress;
		m_nP2PData = pObj.m_nP2PData;
		return *this;
	}
	cr::bigint m_sFromAccount;
	CEBString m_sFromLocalAddress;
	CEBString m_sFromRemoteAddress;
	int m_nP2PData;
};

class CCrNotifyInfo
	: public CCrInfo
{
public:
	CCrNotifyInfo(void)
		: m_nFromAccount(0)
		, m_nNotifyType(0)
		, m_nNotityId(0)
	{
	}
	CCrNotifyInfo(unsigned long nParam,cr::bigint sCallId,cr::bigint sChatId,EB_STATE_CODE nState)
		: CCrInfo(nParam,sCallId,sChatId,nState)
		, m_nFromAccount(0)
		, m_nNotifyType(0)
		, m_nNotityId(0)
	{
	}
	CCrNotifyInfo(const CCrNotifyInfo& pObj)
		: m_nFromAccount(0)
		, m_nNotifyType(0)
		, m_nNotityId(0)
	{
		CCrNotifyInfo::operator=(pObj);
	}
	const CCrNotifyInfo& operator=(const CCrNotifyInfo& pObj)
	{
		CCrInfo::operator=(&pObj);
		m_nFromAccount = pObj.m_nFromAccount;
		m_nNotifyType = pObj.m_nNotifyType;
		m_nNotityId = pObj.m_nNotityId;
		m_sNotifyData = pObj.m_sNotifyData;
		return *this;
	}
	cr::bigint m_nFromAccount;
	int m_nNotifyType;
	cr::bigint m_nNotityId;
	CEBString m_sNotifyData;
};

#ifdef EB_SERVER
#define WM_USER 0
#endif
#ifndef WM_USER
#define WM_USER 0
#endif
typedef enum CR_WINDOW_MESSAGE
{
	// const CCrInfo* pParameter = (const CCrInfo*)wp;
	CR_WM_ENTER_ROOM			= WM_USER+0x1101
	, CR_WM_EXIT_ROOM
	/*
	const CCrAccountInfo* pAccountInfo = (const CCrAccountInfo*)wParam;
	*/
	, CR_WM_USER_ENTER_ROOM
	/*
	const CCrAccountInfo* pAccountInfo = (const CCrAccountInfo*)wParam;
	const int nHangup = (const int)lParam;
	*/
	, CR_WM_USER_EXIT_ROOM
	// const CCrNotifyInfo* pNotifyInfo = (const CCrNotifyInfo*)wParam;
	, CR_WM_USER_NOTIFY
	/*
	const CCrRichInfo * pRichInfo = (const CCrRichInfo*)wParam;
	*/
	, CR_WM_SEND_RICH			= WM_USER+0x1111
	, CR_WM_RECEIVE_RICH
	/*
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	*/
	, CR_WM_SENDING_FILE
	, CR_WM_SENT_FILE
	, CR_WM_CANCEL_FILE
	, CR_WM_RECEIVING_FILE
	, CR_WM_RECEIVED_FILE
	/*
	const CChatRoomFilePercent * pChatRoomFilePercent = (const CChatRoomFilePercent*)wParam;
	*/
	, CR_WM_FILE_PERCENT
	/*
	const CCrRichInfo * pRichInfo = (const CCrRichInfo*)wParam;
	*/
	, CR_WM_MSG_RECEIPT
	/*
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	*/
	, CR_WM_SAVE2CLOUD_DRIVE			= WM_USER+0x1121

	///*
	//const char* sResourceId = (const char*)wParam;
	//lParam: nParam or sParam
	//*/
	//, CR_WM_SET_RESOURCE_OK			= WM_USER+0x1121
	//, CR_WM_SET_RESOURCE_ERROR
	// const CCrInfo* pParameter = (const CCrInfo*)wp;
	, CR_WM_INVALIDATE_SESSION			= WM_USER+0x1131
	, CR_WM_ENTER_ANOTHER

	// const CCrP2PInfo* pP2PInfo = (const CCrP2PInfo*)wp;
	, CR_WM_P2P_REQUEST						= WM_USER+0x1141
	, CR_WM_P2P_RESPONSE
	, CR_WM_P2P_OK
};

// If MFC error use CR_WINDOW_MESSAGE.
class CChatRoomCallBack
{
public:
	virtual void OnEnterRoom(const CCrInfo& pParameter) {}
	virtual void OnExitRoom(const CCrInfo& pParameter) {}
	virtual void OnUserEnterRoom(const CCrAccountInfo& pAccountInfo) {}
	virtual void OnUserExitRoom(const CCrAccountInfo& pAccountInfo, bool bHangup) =0;
	virtual void OnUserNotify(const CCrNotifyInfo& pNotifyInfo) =0;

	virtual void OnSendRich(const CCrRichInfo& pRichInfo) {}
	virtual int OnReceiveRich(const CCrRichInfo& pRichInfo) {return 0;}
	virtual void OnMsgReceipt(const CCrRichInfo& pRichInfo, int nAckType) {}
	virtual void OnSendingFile(const CCrFileInfo& pFileInfo) {}
	virtual void OnSentFile(const CCrFileInfo& pFileInfo) {}
	virtual void OnCancelFile(const CCrFileInfo& pFileInfo) {}
	virtual int OnReceivingFile(const CCrFileInfo& pFileInfo) {return 0;}
	virtual void OnReceivedFile(const CCrFileInfo& pFileInfo) {}
	virtual void OnFilePercent(const CChatRoomFilePercent& pFilePercent) {}
	virtual void OnSave2CloudDrive(const CCrFileInfo& pFileInfo) {}

	virtual void OnInvalidateSession(const CCrInfo& pParameter) =0;
	virtual void OnEnterAnother(const CCrInfo& pParameter) =0;

	virtual void OnP2PRequest(const CCrP2PInfo& pP2PInfo) =0;
	virtual void OnP2PResponse(const CCrP2PInfo& pP2PInfo,int nFlag) =0;
	virtual void OnP2POk(const CCrP2PInfo& pP2PInfo) =0;
};

} // namespace entboost

#endif // __charroomhandle_h__
