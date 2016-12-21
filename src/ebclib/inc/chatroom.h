// chatroom.h file here
// 版权所有：恩布（ENTBOOST）互联网络科技有限公司 2012－2013
// 版    本：2.0.0.100

#ifndef __chatroom_h__
#define __chatroom_h__

#include <string>
#include <vector>
#include "../include/chatroomhandle.h"

namespace entboost {

class CHATROOM_API Cchatroom
{
public:
	typedef boost::shared_ptr<Cchatroom> pointer;
	static Cchatroom::pointer create(void)
	{
		return Cchatroom::pointer(new Cchatroom());
	}
	Cchatroom(void);
	virtual ~Cchatroom(void);

	/****
	功能：初始化应用环境
	==============================================================================================================================*/
	void SetSslKey(const char* sSslPublicKey,const char* sSslPrivateKey,const char* sSslPrivatePwd);
	int Start(const char * address, const char * sAppName, unsigned int bindPort,bool bBuildResponseThread, int nUsesSsl, int nMaxWaitSecons=2);
	tstring GetAddress(void) const;
	//tstring GetLocalIp(void) const;
	unsigned short GetLocalPort(void) const;
	bool IsStart(void) const;
	void SetCallBack(CChatRoomCallBack * pCallBack);
	void SetMsgHwnd(HWND pHwnd);
	void SetParam(unsigned long nParam);
	//void SetParam(const tstring& sParam);
	void Stop(void);

	// 
	//int SetResource(const char* sResourceId,const char* sAccount,const char* sResourceKey,const char* sFilePath);

	int EnterRoom(cr::bigint nFromUserId, cr::bigint sChatId, const char* sCmKey, int nType=1, cr::bigint sCallid=0, cr::bigint sDepCode=0);
	bool IsEnterOk(void) const;
	int GetAccount(cr::bigint & pOutUserId) const;
	int GetChatId(cr::bigint & pOutChatId) const;
	int SetCallId(cr::bigint nCallId);
	int GetCallId(cr::bigint & pOutCallId) const;
	int ExitRoom(int nExitSes); // 0/1
	int Notify(cr::bigint nToUserId, int nNotifyType, cr::bigint nNotifyId, const char* sNotifyData);

	size_t GetUserSize(void) const;
	void GetUserList(std::vector<cr::bigint> & pOutUserList) const;

	void RichBufferAddText(const char* sTextMsg);
	void RichBufferAddObject(const char* pData, unsigned long nSize);
	void RichBufferAddResource(const char* sResource);
	void RichBufferClear(void);
	int SendRichBuffser(cr::bigint sTo=0,bool nPrivate=false);
	int SendMapPos(const char * sPosData);
	int SendRich(const EB_ChatRoomRichMsg* pRichMsg,cr::bigint sTo=0,bool bPrivate=false);
	int SendFile(const char * sFilePath,cr::bigint sTo=0,bool bPrivate=false,bool bSendOffFile=false);
	int SendResource(cr::bigint sResourceId,const char * sFilePath,bool bNewUpload);
	int CancelSendingFile(cr::bigint nMsgId);
	int ReceiveFile(cr::bigint nMsgId, const char * sSaveTo);
	int SaveFile2CloudDrive(cr::bigint nMsgId);
	int ResendMsg(cr::bigint nMsgId);
	int MsgAck(cr::bigint nMsgId,int nAckType);
	int RecviveResource(cr::bigint sResourceId,const char* sSaveTo);

	int SetDestAddress(int nUserIndex,const tstring& sToAddress);
	int SendP2PRequest(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocalIp,int nRequestData);
	int SendP2PResponse(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocalIp,int nResponseData);
	int SendP2PTry(cr::bigint nCallId,cr::bigint nChatId,cr::bigint nFromAccount,int nP2PData);
	void SendP2PTest(unsigned short nTestCount=3);
	cr::bigint GetP2POwnerAccount(void) const;

	////**** video
	//// VideoType: 1:audio; 2:audio&video
	//void SetVideoHwnd(HWND pHwnd);
	//int VideoRequest(int nVideoType=2);
	//int VideoAck(bool bAccept);
	//int VideoEnd(void);
	//int GetVideoUserId(int & pOutMyVideoUserId, std::vector<int>& pOutFromVideoUserId);

private:
	void * m_handle;
};

} // namespace entboost

#endif //__chatroom_h__
