// chatroom.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../include/POPCChatManager.h"
#include "chatroom.h"
#include "md5.h"
#ifdef WIN32
#include "ShlObj.h"
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
#endif

#ifdef _QT_MAKE_
#include <QCoreApplication>
#include <QEvent>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#endif  /// _QT_MAKE_

#ifndef WIN32
#include "../include/boost_ini.h"
#endif

namespace entboost {

const int const_send_data_size = 1100;//1000
const unsigned int theOneMB = 1024*1024;

bool GetFileMd5(const char* sFilePath,mycp::bigint& pOutFileSize,tstring& pOutFileMd5)
{
#ifdef _QT_MAKE_
    QFile file(sFilePath);
    if ( !file.open(QFile::ReadOnly) ) {
        return false;
    }
    pOutFileSize = file.size();
    file.seek(0);
    MD5 md5;
    const unsigned int nPackSize = (unsigned int)(pOutFileSize>theOneMB?theOneMB:pOutFileSize);
    char * lpszBuffer = new char[nPackSize+1];
    while (true) {
        const size_t nReadSize = file.read(lpszBuffer,nPackSize);
        if (nReadSize==0) {
            break;
        }
        md5.update(lpszBuffer, nReadSize);
    }
    pOutFileMd5 = md5.toString();
    file.close();
    delete[] lpszBuffer;
#else
    FILE * f = fopen(sFilePath, "rb");
	if (f == NULL)
	{
		return false;
	}
#ifdef WIN32
	_fseeki64(f, 0, SEEK_END);
	pOutFileSize = _ftelli64(f);
#else
	fseeko(f, 0, SEEK_END);
	pOutFileSize = ftello(f);
#endif
	// 获取文件MD5
	MD5 md5;
	const unsigned int nPackSize = (unsigned int)(pOutFileSize>theOneMB?theOneMB:pOutFileSize);
	char * lpszBuffer = new char[nPackSize+1];
#ifdef WIN32
	_fseeki64(f, 0, SEEK_SET);
#else
	fseeko(f, 0, SEEK_SET);
#endif
	while (true)
	{
		const size_t nReadSize = fread(lpszBuffer,1,nPackSize,f);
		if (nReadSize==0)
			break;
		md5.update(lpszBuffer, nReadSize);
	}
	fclose(f);
	pOutFileMd5 = md5.toString();
	delete[] lpszBuffer;
#endif
	return true;
}

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<tstring> & pOut)
{
	const tstring sIn(lpszString);
	const size_t nInLen = sIn.size();
	const size_t nIntervalLen = strlen(lpszInterval);
	pOut.clear();
	std::string::size_type nFindBegin = 0;
	while (nFindBegin<nInLen)
	{
		std::string::size_type find = sIn.find(lpszInterval,nFindBegin);
		if (find == std::string::npos)
		{
			pOut.push_back(sIn.substr(nFindBegin));
			break;
		}
		if (find==nFindBegin)
		{
			pOut.push_back("");	// 空
		}else
		{
			pOut.push_back(sIn.substr(nFindBegin, (find-nFindBegin)));
		}
		nFindBegin = find+nIntervalLen;
	}
	return (int)pOut.size();
	//tstring sIn(lpszString);
	//pOut.clear();
	//while (!sIn.empty())
	//{
	//	std::string::size_type find = sIn.find(lpszInterval);
	//	if (find == std::string::npos)
	//	{
	//		pOut.push_back(sIn);
	//		break;
	//	}
	//	if (find==0)
	//		pOut.push_back("");	// 空
	//	else
	//		pOut.push_back(sIn.substr(0, find));
	//	sIn = sIn.substr(find+strlen(lpszInterval));
	//}
	//return (int)pOut.size();
}
tstring GetHostIp(const char * lpszHostName,const char* lpszDefaultName)
{
	if (lpszHostName==NULL || strlen(lpszHostName)==0)
		return lpszDefaultName;
	try
	{
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

tstring theAppDataTempPath;
tstring GetAddressList(const tstring& sFullAddress)
{
	std::vector<tstring> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)!=2)
		return "";

	const tstring sAddress = pList[0];
	char sTempIniFile[260];
    sprintf(sTempIniFile,"%s\\temp.ini",theAppDataTempPath.c_str());
	char lpszIp[101];
	memset(lpszIp,0,101);
#ifdef WIN32
	GetPrivateProfileStringA(_T("address_list"),sAddress.c_str(),"",lpszIp,100,sTempIniFile);
#else
	GetPrivateProfileStringABoost(_T("address_list"),sAddress.c_str(),"",lpszIp,100,sTempIniFile);
#endif
	if (strlen(lpszIp)==0)
	{
		return "";
	}
	tstring result = lpszIp;
	result.append(":");
	result.append(pList[1]);
	return result;
}
void SaveAddressList(const tstring& sFullAddress)
{
	std::vector<tstring> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)==2)
	{
		const tstring sAddress = pList[0];
		const tstring sIp = GetHostIp(sAddress.c_str(),sAddress.c_str());
		if (sAddress!=sIp)
		{
			char sTempIniFile[260];
            sprintf(sTempIniFile,"%s\\temp.ini",theAppDataTempPath.c_str());
#ifdef WIN32
			WritePrivateProfileStringA(_T("address_list"),sAddress.c_str(),sIp.c_str(),sTempIniFile);
#else
			WritePrivateProfileStringABoost(_T("address_list"),sAddress.c_str(),sIp.c_str(),sTempIniFile);
#endif
		}
	}
}


// 使用线程是为了不会挂住主线程
class CThreadRequestReceiveFileInfo
{
public:
	mycp::bigint m_sCallId;
	mycp::bigint m_sChatId;
	mycp::bigint m_sSendFrom;
	mycp::bigint m_sSendTo;
	bool m_bPrivate;
	mycp::bigint m_sResId;
	tstring m_sResCmServer;
	tstring m_sResCmAppName;
	mycp::bigint m_nMsgId;
    EBFileString m_sFileName;
	mycp::bigint m_nFileSize;
	tstring m_sFileTime;
	tstring m_sMD5;

	//static void thread_request_receive_file(void* lparam)
	//{
	//	CThreadRequestReceiveFileInfo * pThreadInfo = (CThreadRequestReceiveFileInfo*)lparam;
	//	pThreadInfo->SendThreadCallBack();
	//	delete pThreadInfo;
	//}

	void SendThreadCallBack(void)
	{
        CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_sCallId,m_sChatId,EB_STATE_OK);
#ifdef _QT_MAKE_
        pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_RECEIVING_FILE);
#endif
        pCrFileInfo->m_sResId = m_sResId;
        pCrFileInfo->m_sResCmServer = m_sResCmServer;
        pCrFileInfo->m_sResCmAppName = m_sResCmAppName;
        pCrFileInfo->m_nMsgId = m_nMsgId;
        pCrFileInfo->m_sFileName = m_sFileName;
        pCrFileInfo->m_nFileSize = m_nFileSize;
        pCrFileInfo->m_sFileTime = m_sFileTime;
        pCrFileInfo->m_bOffFile = !m_sFileTime.empty();
        pCrFileInfo->m_sSendFrom = m_sSendFrom;
        pCrFileInfo->m_sSendTo = m_sSendTo;
        pCrFileInfo->m_bPrivate = m_bPrivate;
        pCrFileInfo->m_sMD5 = m_sMD5;
		if (m_callback)
            m_callback->OnReceivingFile(*pCrFileInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd,pCrFileInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_RECEIVING_FILE, (WPARAM)pCrFileInfo, 0);
            delete pCrFileInfo;
#endif
        }
        else {
            delete pCrFileInfo;
        }
	}
	void SetThreadCallBack(void)
	{
		if (m_pProcessThread.get() == NULL)
		{
            m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CThreadRequestReceiveFileInfo::SendThreadCallBack, this)));
//			boost::thread_attributes attrs;
//            attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//			m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CThreadRequestReceiveFileInfo::SendThreadCallBack, this)));
		}
	}
#ifdef _QT_MAKE_
    CThreadRequestReceiveFileInfo(CChatRoomCallBack * pCallBack, QObject* pHwnd,DWORD dwParam)
#else
    CThreadRequestReceiveFileInfo(CChatRoomCallBack * pCallBack, HWND pHwnd,DWORD dwParam)
#endif
        : m_sCallId(0),m_sChatId(0)
		, m_sSendFrom(0), m_sSendTo(0)
		, m_sResId(0)
		, m_callback(pCallBack), m_pHwnd(pHwnd),m_nParam(dwParam),m_nFileSize(0), m_bPrivate(false)
	{
	}
	virtual  ~CThreadRequestReceiveFileInfo(void)
	{
		m_pProcessThread.reset();
		//if (m_pProcessThread)
		//	delete m_pProcessThread;
	}
private:
	CChatRoomCallBack * m_callback;
#ifdef _QT_MAKE_
    QObject* m_pHwnd;
#else
    HWND m_pHwnd;
#endif
	DWORD m_nParam;
	boost::shared_ptr<boost::thread> m_pProcessThread;
};

class CChatManager
	: public CPOPCMHandler
{
public:
	CChatManager(void)
		: m_callback(NULL)
		, m_pHwnd(NULL),m_pVideoHwnd(NULL)
		, m_nParam(0)
	{
		m_chatManager = CPOPCChatManager::create(this);
        if (theAppDataTempPath.empty())
		{
			// app data path
#ifdef _QT_MAKE_
            /// C:/Users/pc/AppData/Roaming/ebcd
            const QString sStringTemp = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            std::string m_sAppDataPath = sStringTemp.toStdString();
//            m_sAppDataPath += _T("\\entboost");
            //if (!QFileInfo::isDir(m_sAppDataPath))
            QDir pDir1(m_sAppDataPath.c_str());
            if (!pDir1.exists()) {
                pDir1.mkdir(m_sAppDataPath.c_str());
            }
            theAppDataTempPath = m_sAppDataPath+_T("/temp");
            QDir pDir2(theAppDataTempPath.c_str());
            if (!pDir2.exists()) {
                pDir2.mkdir(theAppDataTempPath.c_str());
            }
#else
			char lpszBuffer[MAX_PATH];
            SHGetSpecialFolderPathA(::GetDesktopWindow(),lpszBuffer,CSIDL_APPDATA,FALSE);
            tstring m_sAppDataPath = lpszBuffer;
			m_sAppDataPath += _T("\\entboost");
            if (!::PathFileExistsA(m_sAppDataPath.c_str()))
			{
                ::CreateDirectoryA(m_sAppDataPath.c_str(), NULL);
			}
            theAppDataTempPath = m_sAppDataPath+_T("\\temp");
            if (!::PathFileExistsA(theAppDataTempPath.c_str()))
			{
                ::CreateDirectoryA(theAppDataTempPath.c_str(), NULL);
			}
#endif
        }
	}
	virtual ~CChatManager(void)
	{
		m_chatManager.reset();
        m_callback = 0;
	}

	//int SetResource(const char* sResourceId,const char* sAccount,const char* sResourceKey,const char* sFilePath)
	//{
	//	unsigned int nFileSize = 0;
	//	FILE * f = fopen(sFilePath, "rb");
	//	if (f == NULL)
	//	{
	//		return -1;
	//	}
	//	_fseeki64(f, 0, SEEK_END);
	//	nFileSize = _ftelli64(f);
	//	fclose(f);

	//	return 0;
	//}
	EB_ChatRoomRichMsg m_pRichBuffer;

	CPOPCChatManager::pointer m_chatManager;
	CChatRoomCallBack * m_callback;
#ifdef _QT_MAKE_
    QObject* m_pHwnd;
    QObject* m_pVideoHwnd;
#else
    HWND m_pHwnd;
	HWND m_pVideoHwnd;
#endif
	DWORD m_nParam;
	//tstring m_sParam;
private:
    virtual void OnTimeout(const CPOPSotpRequestInfo::pointer& pRequestInfo,const CPOPCChatManager* /*pCMOwner*/)
	{
		if (m_chatManager.get()==NULL || pRequestInfo.get()==NULL) return;
		const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pRequestInfo->m_pRequestList.getParameterValue("cm-msg-type", (int)EB_MSG_UNKNOWN);
		if (nMsgType==EB_MSG_FILE)
		{
			const tstring sFileName(pRequestInfo->m_pRequestList.getParameterValue("cm-file-name", ""));
			const mycp::bigint nFileSize = pRequestInfo->m_pRequestList.getParameterValue("cm-size", (mycp::bigint)0);
			const int nOffFile = pRequestInfo->m_pRequestList.getParameterValue("cm-off-file", (int)0);
			const mycp::bigint nToUserId = pRequestInfo->m_pRequestList.getParameterValue("cm-to", (mycp::bigint)0);
            const int nPrivate = pRequestInfo->m_pRequestList.getParameterValue("cm-private", (int)0);
			const mycp::bigint nResourceId = pRequestInfo->m_pRequestList.getParameterValue("cm-res-id", (mycp::bigint)0);
            CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),EB_STATE_TIMEOUT_ERROR);
#ifdef _QT_MAKE_
             pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_SENDING_FILE);
#endif
			//pCrFileInfo.m_sResId = pChatMsgInfo->m_sResourceId;
			//pCrFileInfo.m_nMsgId = nMsgId;
            pCrFileInfo->m_sFileName = sFileName;
            pCrFileInfo->m_nFileSize = nFileSize;
            pCrFileInfo->m_bOffFile = nOffFile==1?true:false;
            pCrFileInfo->m_sSendFrom = m_chatManager->GetOwnerAccount();
            pCrFileInfo->m_sSendTo = nToUserId;
            pCrFileInfo->m_bPrivate = nPrivate==1?true:false;
            pCrFileInfo->m_sResId = nResourceId;
            //pCrFileInfo->m_sFromAccount = sReceivedAccount;

			if (m_callback)
                m_callback->OnSendingFile(*pCrFileInfo);
            if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                QCoreApplication::postEvent(m_pHwnd,pCrFileInfo);
#else
                ::SendMessage(m_pHwnd, CR_WM_SENDING_FILE, (WPARAM)pCrFileInfo, 0);
                delete pCrFileInfo;
#endif
            }
            else {
                delete pCrFileInfo;
            }
		}else if (nMsgType==EB_MSG_RICH)
		{
			const mycp::bigint nToUserId = pRequestInfo->m_pRequestList.getParameterValue("cm-to", (mycp::bigint)0);
			const int nPrivate = pRequestInfo->m_pRequestList.getParameterValue("cm-private", (int)0);
            CCrRichInfo * pCrMsgInfo = new CCrRichInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),EB_STATE_TIMEOUT_ERROR);
#ifdef _QT_MAKE_
             pCrMsgInfo->SetQEventType((QEvent::Type)CR_WM_SEND_RICH);
#endif
			//pCrMsgInfo.m_pRichMsg = pRichMsg.get();
            pCrMsgInfo->m_sSendFrom = m_chatManager->GetOwnerAccount();;
            pCrMsgInfo->m_sSendTo = nToUserId;
            pCrMsgInfo->m_bPrivate = nPrivate==1?true:false;
			if (m_callback)
                m_callback->OnSendRich(*pCrMsgInfo);
            if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                QCoreApplication::postEvent(m_pHwnd, pCrMsgInfo);
#else
                ::SendMessage(m_pHwnd, CR_WM_SEND_RICH, (WPARAM)pCrMsgInfo, 0);
                delete pCrMsgInfo;
#endif
            }
            else {
                delete pCrMsgInfo;
            }
		}
	
	}
	virtual void OnInvalidateSession(int nResultCode,const CPOPCChatManager* pCMOwner)
	{
		if (m_chatManager->GetChatInfo().get()==NULL) return;
        CCrInfo * pParameter = new CCrInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultCode);
#ifdef _QT_MAKE_
        pParameter->SetQEventType((QEvent::Type)CR_WM_INVALIDATE_SESSION);
#endif
        if (m_callback)
            m_callback->OnInvalidateSession(*pParameter);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pParameter);
#else
            ::SendMessage(m_pHwnd, CR_WM_INVALIDATE_SESSION, (WPARAM)pParameter, 0);
            delete pParameter;
#endif
        }
        else {
            delete pParameter;
        }
	}
	virtual void OnSave2CloudDriveResponse(const CPOPSotpRequestInfo::pointer & pRequestInfo,mycp::bigint sResourceId, EB_STATE_CODE nResultCode,const CPOPCChatManager* pCMOwner)
	{
		if (m_chatManager.get()==NULL || pRequestInfo.get()==NULL) return;
		const mycp::bigint nMsgId = pRequestInfo->m_pRequestList.getParameterValue("msg-id", (mycp::bigint)0);
		CChatMsgInfo::pointer pChatMsgInfo = m_chatManager->GetChatMsgInfo(nMsgId, false);
		if (pChatMsgInfo.get() == NULL)
		{
			return;
		}

        CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),nResultCode);
#ifdef _QT_MAKE_
        pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_SAVE2CLOUD_DRIVE);
#endif
        pCrFileInfo->m_nMsgId = nMsgId;
        pCrFileInfo->m_sResId = sResourceId;
        pCrFileInfo->m_sFileName = pChatMsgInfo->GetContent();
        pCrFileInfo->m_nFileSize = pChatMsgInfo->GetSize();
        //pCrFileInfo->m_sSendFrom = m_chatManager->GetOwnerAccount();
        //pCrFileInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
        //pCrFileInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;

		if (m_callback)
            m_callback->OnSave2CloudDrive(*pCrFileInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrFileInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_SAVE2CLOUD_DRIVE, (WPARAM)pCrFileInfo, 0);
            delete pCrFileInfo;
#endif
        }
        else {
            delete pCrFileInfo;
        }
	}
	virtual void OnSetResourceResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo,mycp::bigint sResourceId, bool bError,const CPOPCChatManager* pCMOwner){}

	virtual void OnMsgReceipt(EB_STATE_CODE nResultValue,int nAckType,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CEBChatInfo::pointer& pChatInfo,mycp::bigint nMsgId,mycp::bigint sFromAccount, const CPOPCChatManager* pCMOwner)
	{
		EB_ChatRoomRichMsg pRichMsg(nMsgId);
        CCrRichInfo * pCrMsgInfo = new CCrRichInfo(m_nParam,pChatInfo->GetCallId(),pChatInfo->GetChatId(),nResultValue);
#ifdef _QT_MAKE_
        pCrMsgInfo->SetQEventType((QEvent::Type)CR_WM_MSG_RECEIPT);
        pCrMsgInfo->m_nAckType = nAckType;
#endif
        pCrMsgInfo->m_pRichMsg = &pRichMsg;
        pCrMsgInfo->m_sSendFrom = sFromAccount;
		if (m_callback)
            m_callback->OnMsgReceipt(*pCrMsgInfo, nAckType);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrMsgInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_MSG_RECEIPT, (WPARAM)pCrMsgInfo, (LPARAM)nAckType);
            delete pCrMsgInfo;
#endif
        }
        else {
            delete pCrMsgInfo;
        }
	}
	virtual void OnSendingRich(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CEBChatRoomRichMsg::pointer& pRichMsg,mycp::bigint sFromAccount, mycp::bigint sToAccount, int nPrivate,const CPOPCChatManager* pCMOwner)
	{
        CCrRichInfo * pCrMsgInfo = new CCrRichInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),nResultValue);
#ifdef _QT_MAKE_
        pCrMsgInfo->SetQEventType((QEvent::Type)CR_WM_SEND_RICH);
#endif
        pCrMsgInfo->m_pRichMsg = pRichMsg.get();
        pCrMsgInfo->m_sSendFrom = sFromAccount;
        pCrMsgInfo->m_sSendTo = sToAccount;
        pCrMsgInfo->m_bPrivate = nPrivate==1?true:false;
		if (m_callback)
            m_callback->OnSendRich(*pCrMsgInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrMsgInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_SEND_RICH, (WPARAM)pCrMsgInfo, 0);
            delete pCrMsgInfo;
#endif
        }
        else {
            delete pCrMsgInfo;
        }
	}
	virtual void OnSendingFile(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPCChatManager* pCMOwner)
	{
		if (pChatMsgInfo.get()==NULL) return;
		const mycp::bigint nMsgId = pChatMsgInfo->GetMsgId();
		const tstring sFileName(pChatMsgInfo->GetContent());
		mycp::bigint nFileSize = pChatMsgInfo->GetSize();

        CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),nResultValue);
#ifdef _QT_MAKE_
        pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_SENDING_FILE);
#endif
        pCrFileInfo->m_sResId = pChatMsgInfo->m_sResourceId;
        pCrFileInfo->m_nMsgId = nMsgId;
        pCrFileInfo->m_sFileName = sFileName;
        pCrFileInfo->m_nFileSize = nFileSize;
        pCrFileInfo->m_bOffFile = (bool)(pChatMsgInfo->m_nOffFile==1);
        pCrFileInfo->m_sSendFrom = m_chatManager->GetOwnerAccount();
        pCrFileInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
        pCrFileInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
        //pCrFileInfo->m_sFromAccount = sReceivedAccount;

		//CChatRoomFileInfo pChatRoomFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
		//pChatRoomFileInfo.m_sResId = pChatMsgInfo->m_sResourceId;
		//pChatRoomFileInfo.m_nMsgId = nMsgId;
		//pChatRoomFileInfo.m_sFileName = sFileName;
		//pChatRoomFileInfo.m_nFileSize = nFileSize;
		//pChatRoomFileInfo.m_bOffFile = (bool)(pChatMsgInfo->m_nOffFile==1);
		if (m_callback)
            m_callback->OnSendingFile(*pCrFileInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrFileInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_SENDING_FILE, (WPARAM)pCrFileInfo, 0);
            delete pCrFileInfo;
#endif
        }
        else {
            delete pCrFileInfo;
        }
	}
	virtual void OnSentFile(EB_STATE_CODE nResultValue,const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo, mycp::bigint sReceivedAccount,const CPOPCChatManager* pCMOwner)
	{
		if (pChatMsgInfo.get()==NULL) return;
		const mycp::bigint nMsgId = pChatMsgInfo->GetMsgId();
		const tstring sFileName(pChatMsgInfo->GetContent());
		mycp::bigint nFileSize = pChatMsgInfo->GetSize();

        CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),nResultValue);
#ifdef _QT_MAKE_
        pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_SENT_FILE);
#endif
        pCrFileInfo->m_sResId = pChatMsgInfo->m_sResourceId;
        pCrFileInfo->m_nMsgId = nMsgId;
        pCrFileInfo->m_sFileName = sFileName;
        pCrFileInfo->m_nFileSize = nFileSize;
        pCrFileInfo->m_bOffFile = (bool)(pChatMsgInfo->m_nOffFile==1);
        pCrFileInfo->m_sSendFrom = m_chatManager->GetOwnerAccount();
        pCrFileInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
        pCrFileInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
        pCrFileInfo->m_sReceiveAccount = sReceivedAccount;
		if (m_callback)
            m_callback->OnSentFile(*pCrFileInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrFileInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_SENT_FILE, (WPARAM)pCrFileInfo, 0);
            delete pCrFileInfo;
#endif
        }
        else {
            delete pCrFileInfo;
        }
	}
	virtual void OnCancelFile(EB_STATE_CODE nResultValue, const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo, mycp::bigint sCancelAccount,const CPOPCChatManager* pCMOwner)
	{
		if (pChatMsgInfo.get()==NULL) return;
		const mycp::bigint nMsgId = pChatMsgInfo->GetMsgId();
		const bool bIsMsgSender = m_chatManager->IsMsgSender(pChatMsgInfo);
		const mycp::bigint sMsgSender = m_chatManager->GetMsgSender(pChatMsgInfo);
        const EBFileString sFileName(bIsMsgSender?pChatMsgInfo->GetContent().c_str():pChatMsgInfo->m_sFileName);
		//mycp::bigint nFileSize = pChatMsgInfo->GetSize();

        CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),nResultValue);
#ifdef _QT_MAKE_
        pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_CANCEL_FILE);
#endif
        pCrFileInfo->m_sResId = pChatMsgInfo->m_sResourceId;
        pCrFileInfo->m_nMsgId = nMsgId;
        pCrFileInfo->m_sFileName = sFileName;
        //pCrFileInfo->m_bIsMsgSender = bIsMsgSender;
		if (bIsMsgSender)
            pCrFileInfo->m_bOffFile = (bool)(pChatMsgInfo->m_nOffFile==1);
		else
            pCrFileInfo->m_bOffFile = !pChatMsgInfo->m_sMsgTime.empty();

        pCrFileInfo->m_sSendFrom = sMsgSender;
        pCrFileInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
        pCrFileInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
        pCrFileInfo->m_sReceiveAccount = sCancelAccount;
		if (m_callback)
            m_callback->OnCancelFile(*pCrFileInfo);
        if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrFileInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_CANCEL_FILE, (WPARAM)pCrFileInfo, 0);
            delete pCrFileInfo;
#endif
        }
        else {
            delete pCrFileInfo;
        }

#ifdef _QT_MAKE_
        if (!bIsMsgSender) {
            //if (sCancelAccount>0 && !bIsMsgSender && !sFileName.empty()) {	// * 避免删除本端文件风险；
            QFile::remove(sFileName);
        }
        else if (pChatMsgInfo->m_sResourceId>0 && pChatMsgInfo->m_sFileName.lastIndexOf(".ebtemp")>0) {
            QFile::remove(pChatMsgInfo->m_sFileName);
        }
#else
        if (!bIsMsgSender) {
        //if (sCancelAccount>0 && !bIsMsgSender && !sFileName.empty()) {	// * 避免删除本端文件风险；
            DeleteFileA(sFileName.c_str());
        }
        else if (pChatMsgInfo->m_sResourceId>0 && pChatMsgInfo->m_sFileName.find(".ebtemp")!=std::string::npos) {
            DeleteFileA(pChatMsgInfo->m_sFileName.c_str());
        }
#endif
	}
	//virtual void OnReceivedFile(const CPOPSotpRequestInfo::pointer & pReqeustInfo,const tstring & sAccount, const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPCChatManager* pCMOwner)
	//{
	//	if (m_callback)
	//	{
	//		//CChatRoomSendInfo pSendInfo(sFromAccount,pChatMsgInfo->m_nToUserId,pChatMsgInfo->m_nPrivate);
	//		//m_callback->OnReceivedFile(m_chatManager->GetChatInfo()->GetChatId(), pSendInfo, pChatMsgInfo->m_sFileName);
	//	}
	//	if (m_pHwnd)
	//	{
	//		//CCrFileInfo * pCrFileInfo = new CCrFileInfo();
	//		//pCrFileInfo->m_pFileInfo.m_nMsgId = pChatMsgInfo->GetMsgId();
	//		//pCrFileInfo->m_pFileInfo.m_sFileName = pChatMsgInfo->m_sFileName;
	//		//pCrFileInfo->m_pFileInfo.m_nFileSize = pChatMsgInfo->GetSize();
	//		//pCrFileInfo->m_pSendInfo.m_sFrom = sFromAccount;
	//		//pCrFileInfo->m_pSendInfo.m_sTo = pChatMsgInfo->m_nToUserId;
	//		//pCrFileInfo->m_pSendInfo.m_nPrivate = pChatMsgInfo->m_nPrivate;
	//		//LPARAM lParam = m_sParam.empty()?m_nParam:(LPARAM)m_sParam.c_str();
	//		//::SendMessage(m_pHwnd, CR_WM_RECEIVED_FILE, (WPARAM)pCrFileInfo, lParam);
	//		//delete pCrFileInfo;
	//	}
	//}
	virtual void OnFilePercent(const CPOPSotpRequestInfo::pointer & pReqeustInfo,mycp::bigint sResId,mycp::bigint nMsgId, double percent, int nSeconds1, int nSeconds2, int nSpeech1, int nSpeech2,const CPOPCChatManager* pCMOwner)
	{
        CChatRoomFilePercent * pChatRoomFilePercent = new CChatRoomFilePercent(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),EB_STATE_OK);
#ifdef _QT_MAKE_
        pChatRoomFilePercent->SetQEventType((QEvent::Type)CR_WM_FILE_PERCENT);
#endif
        pChatRoomFilePercent->m_sResId = sResId;
        pChatRoomFilePercent->m_nMsgId = nMsgId;
        pChatRoomFilePercent->m_percent = percent;
        pChatRoomFilePercent->m_nTranSeconds = nSeconds1;
        pChatRoomFilePercent->m_nLeftSeconds = nSeconds2;
        pChatRoomFilePercent->m_nCurSpeed = nSpeech1;
        pChatRoomFilePercent->m_nAvrSpeed = nSpeech2;
		// ?这里要改用postmessage才行
		if (m_callback)
            m_callback->OnFilePercent(*pChatRoomFilePercent);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pChatRoomFilePercent);
#else
            ::SendMessage(m_pHwnd, CR_WM_FILE_PERCENT, (WPARAM)pChatRoomFilePercent, 0);
            delete pChatRoomFilePercent;
#endif
        }
        else {
            delete pChatRoomFilePercent;
        }
	}
	virtual void OnReceivingRes(const CPOPSotpRequestInfo::pointer & pReqeustInfo,const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPCChatManager* pCMOwner)
	{
		// 开始接收文件
		mycp::bigint nFileSize = pChatMsgInfo->GetSize();
		CThreadRequestReceiveFileInfo * pThreadInfo = new CThreadRequestReceiveFileInfo(m_callback, m_pHwnd,m_nParam);
		pThreadInfo->m_sCallId = m_chatManager->GetChatInfo()->GetCallId();
		pThreadInfo->m_sChatId = m_chatManager->GetChatInfo()->GetChatId();
		//pThreadInfo->m_sFromAccount = sFromAccount;
		pThreadInfo->m_sResId = pChatMsgInfo->m_sResourceId;
		pThreadInfo->m_nMsgId = pChatMsgInfo->GetMsgId();
		pThreadInfo->m_sFileName = pChatMsgInfo->m_sFileName;
		pThreadInfo->m_nFileSize = nFileSize;
		//pThreadInfo->m_sFileTime = pChatMsgInfo->m_sMsgTime;
		pThreadInfo->m_sMD5 = pChatMsgInfo->m_sMD5String;
		pThreadInfo->SetThreadCallBack();
	}
	virtual int OnFCMMsg(const CChatMsgInfo::pointer& pChatMsgInfo,const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		//if (m_callback)
		{
			const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
			const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id",(mycp::bigint)0);
			//const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pResponseInfo->m_pResponseList.getParameterValue("msg-type", (int)EB_MSG_UNKNOWN);

			//CChatMsgInfo::pointer pChatMsgInfo = m_chatManager->GetChatMsgInfo(nMsgId, false);
			if (pChatMsgInfo.get() == NULL)
			{
				return -1;
			}
			if (pChatMsgInfo->GetMsgType() == EB_MSG_RICH)
			{
				if (pChatMsgInfo->m_pRecvAttachList.empty())
					return -1;
				const mycp::bigint ntotal = pChatMsgInfo->GetSize();
				if (ntotal < 7)		// 最小一个字符，7位
					return 0;
				char * lpBuffer = new char[(unsigned int)ntotal+1];
				memset(lpBuffer, 0, (unsigned int)ntotal+1);
				//cgcAttachment::pointer pAttachment;
				{
					BoostReadLock rdlock(pChatMsgInfo->m_pRecvAttachList.mutex());
					CLockMap<mycp::bigint, cgcAttachment::pointer>::iterator pIter;
					for (pIter=pChatMsgInfo->m_pRecvAttachList.begin(); pIter!=pChatMsgInfo->m_pRecvAttachList.end(); pIter++)
					{
						const cgcAttachment::pointer& pAttachment = pIter->second;
						if (pAttachment->getIndex()+pAttachment->getAttachSize()>ntotal)
						{
							// 系统协议错误
							delete[] lpBuffer;
							return 0;
						}
						char * buffer = lpBuffer+(mycp::bigint)pAttachment->getIndex();
						memcpy(buffer, pAttachment->getAttachData(), pAttachment->getAttachSize());
					}
				}

				CEBChatRoomRichMsg::pointer pRichMsg = CEBChatRoomRichMsg::create(nMsgId);
				//pRichMsg->m_bOffLineMsg = pChatMsgInfo->m_pOffLineAccount[m_chatManager->GetOwnerAccount()];//m_bOffLineMsg;
				pRichMsg->SetSubType(pChatMsgInfo->GetSubType());
				pRichMsg->m_sMsgTime = pChatMsgInfo->m_sMsgTime;
				unsigned int ncharformatsize = 0;
				unsigned int nrichmsgsize = 0;
				unsigned int nprovsize = 0;
				if (lpBuffer[0] == 'f')
				{
					ncharformatsize = atoi(lpBuffer+1);	// 前面有一个f
					if (ncharformatsize+4 >= ntotal)
					{
						delete []lpBuffer;
						return 0;
					}
					char * lpCharFormat = new char[ncharformatsize+1];
					memcpy(lpCharFormat, lpBuffer+4, ncharformatsize);
					pRichMsg->SetCharFormat(EB_ChatRoomMsgItem(lpCharFormat, ncharformatsize, EB_ChatRoomMsgItem::MIT_CHARFORMAT));
					//pRichMsg->m_charFormat = EB_ChatRoomMsgItem::create(lpCharFormat, ncharformatsize, EB_ChatRoomMsgItem::MIT_CHARFORMAT);
					nrichmsgsize = atoi(lpBuffer+ncharformatsize+4);
					//nprovsize = 4 + ncharformatsize + 7;
					const char * find = strchr(lpBuffer+4+ncharformatsize,',');
					nprovsize = find-lpBuffer+1;
				}else
				{
					nrichmsgsize = atoi(lpBuffer);
					//nprovsize = 7;
					const char * find = strchr(lpBuffer,',');
					nprovsize = find-lpBuffer+1;
				}
				tstring sRichMsg(lpBuffer+nprovsize, nrichmsgsize);
				pRichMsg->SetFormatMsg(sRichMsg);
				const std::vector<EB_ChatRoomMsgItem*> & pRichMsgList = pRichMsg->GetList();
				unsigned long nobjectoffset = 0;
				for (size_t i=0; i<pRichMsgList.size(); i++)
				{
					EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
					if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
					{
						unsigned long nObjectSize = pMsgItem->GetSize();
						char * lpObjectData = new char[nObjectSize+1];
						memcpy(lpObjectData, lpBuffer+nprovsize+nrichmsgsize+nobjectoffset, nObjectSize);
						pMsgItem->UpdateData(lpObjectData, nObjectSize);
						nobjectoffset += nObjectSize;
					}
				}
				delete []lpBuffer;
                CCrRichInfo * pCrMsgInfo = new CCrRichInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),EB_STATE_OK);
#ifdef _QT_MAKE_
                pCrMsgInfo->SetQEventType((QEvent::Type)CR_WM_RECEIVE_RICH);
#endif
                pCrMsgInfo->m_pRichMsg = pRichMsg.get();
                pCrMsgInfo->m_sSendFrom = sFromAccount;
                pCrMsgInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
                pCrMsgInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
				int ret = -1;	// -1错误，外面不自动响应
				if (m_callback)
                    ret = m_callback->OnReceiveRich(*pCrMsgInfo);
                if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                    ret = QCoreApplication::sendEvent(m_pHwnd, pCrMsgInfo);
//                    QCoreApplication::postEvent(m_pHwnd, pCrMsgInfo);
#else
                    ret = ::SendMessage(m_pHwnd, CR_WM_RECEIVE_RICH, (WPARAM)pCrMsgInfo, 0);
                    delete pCrMsgInfo;
#endif
                }
                else {
                    delete pCrMsgInfo;
                }
				return ret;
			}else if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
			{
#ifdef _QT_MAKE_
                if (pChatMsgInfo->m_sFileName.isEmpty()) {
#else
                if (pChatMsgInfo->m_sFileName.empty()) {
#endif
                    /// 开始接收文件
					const tstring & sFileName = pChatMsgInfo->GetContent();
					mycp::bigint nFileSize = pChatMsgInfo->GetSize();
					CThreadRequestReceiveFileInfo * pThreadInfo = new CThreadRequestReceiveFileInfo(m_callback, m_pHwnd,m_nParam);
					pThreadInfo->m_sCallId = m_chatManager->GetChatInfo()->GetCallId();
					pThreadInfo->m_sChatId = m_chatManager->GetChatInfo()->GetChatId();
					pThreadInfo->m_sSendFrom = sFromAccount;
					pThreadInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
					pThreadInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
					pThreadInfo->m_nMsgId = nMsgId;
					pThreadInfo->m_sFileName = sFileName;
					pThreadInfo->m_nFileSize = nFileSize;
					pThreadInfo->m_sFileTime = pChatMsgInfo->m_sMsgTime;
					pThreadInfo->m_sResId = pChatMsgInfo->m_sResourceId;
					pThreadInfo->m_sResCmServer = pChatMsgInfo->m_sResCmServer;
					pThreadInfo->m_sResCmAppName = pChatMsgInfo->m_sResCmAppName;
					pThreadInfo->SetThreadCallBack();
					//m_callback->OnReceivingFile(m_chatManager->GetChatInfo()->GetChatId(), sFromAccount, sFileName, nFileSize);
				}else
				{
					// 文件接收完成
                    CCrFileInfo * pCrFileInfo = new CCrFileInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),EB_STATE_OK);
#ifdef _QT_MAKE_
                    pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_RECEIVED_FILE);
#endif
                    pCrFileInfo->m_sResId = pChatMsgInfo->m_sResourceId;
                    pCrFileInfo->m_nMsgId = pChatMsgInfo->GetMsgId();
                    pCrFileInfo->m_sFileName = pChatMsgInfo->m_sFileName;
                    pCrFileInfo->m_nFileSize = pChatMsgInfo->GetSize();
                    pCrFileInfo->m_bOffFile = !pChatMsgInfo->m_sMsgTime.empty();
                    pCrFileInfo->m_sSendFrom = sFromAccount;
                    pCrFileInfo->m_sSendTo = pChatMsgInfo->m_nToUserId;
                    pCrFileInfo->m_bPrivate = pChatMsgInfo->m_nPrivate==1?true:false;
                    //pCrFileInfo->m_sFromAccount = sFromAccount;
					if (m_callback)
                        m_callback->OnReceivedFile(*pCrFileInfo);
                    if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                        QCoreApplication::postEvent(m_pHwnd, pCrFileInfo);
#else
                        ::SendMessage(m_pHwnd, CR_WM_RECEIVED_FILE, (WPARAM)pCrFileInfo, 0);
                        delete pCrFileInfo;
#endif
                    }
                    else {
                        delete pCrFileInfo;
                    }
				}
			}
		}
		return 1;
	}
	virtual void OnFCMExit(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
		const int nHangup = pResponseInfo->m_pResponseList.getParameterValue("hangup",(int)0);
		const int nResultValue = pResponseInfo->GetResultValue();
        CCrAccountInfo * pCrAccountInfo = new CCrAccountInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pCrAccountInfo->SetQEventType((QEvent::Type)CR_WM_USER_EXIT_ROOM);
#endif
        pCrAccountInfo->m_sAccount = sFromAccount;
        //pCrAccountInfo->m_sFromInfo = sFromInfo;
        pCrAccountInfo->m_bOffLine = true;
		if (m_callback)
            m_callback->OnUserExitRoom(*pCrAccountInfo,nHangup==1?true:false);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrAccountInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_USER_EXIT_ROOM, (WPARAM)pCrAccountInfo,nHangup);
            delete pCrAccountInfo;
#endif
        }
        else {
            delete pCrAccountInfo;
        }
	}
	virtual void OnCMExitResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
        CCrInfo * pParameter = new CCrInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pParameter->SetQEventType((QEvent::Type)CR_WM_EXIT_ROOM);
#endif
		if (m_callback)
            m_callback->OnExitRoom(*pParameter);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pParameter);
#else
            ::SendMessage(m_pHwnd, CR_WM_EXIT_ROOM, (WPARAM)pParameter, 0);
            delete pParameter;
#endif
        }
        else {
            delete pParameter;
        }
	}
	virtual void OnFCMEnterResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
		const int nOffLine = pResponseInfo->m_pResponseList.getParameterValue("off-line", (int)0);
		const tstring sFromInfo = pResponseInfo->m_pResponseList.getParameterValue("f-info");
		//const tstring sDepCode = pResponseInfo->m_pResponseList.getParameterValue("dep-code");
        CCrAccountInfo * pCrAccountInfo = new CCrAccountInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pCrAccountInfo->SetQEventType((QEvent::Type)CR_WM_USER_ENTER_ROOM);
#endif
        pCrAccountInfo->m_sAccount = sFromAccount;
        pCrAccountInfo->m_sFromInfo = sFromInfo;
        pCrAccountInfo->m_bOffLine = nOffLine==1?true:false;
		if (m_callback)
            m_callback->OnUserEnterRoom(*pCrAccountInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pCrAccountInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_USER_ENTER_ROOM, (WPARAM)pCrAccountInfo, 0);
            delete pCrAccountInfo;
#endif
		}
        else {
            delete pCrAccountInfo;
        }
	}
	virtual void OnCMEnterResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
        CCrInfo * pParameter = new CCrInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pParameter->SetQEventType((QEvent::Type)CR_WM_ENTER_ROOM);
#endif
		if (m_callback)
            m_callback->OnEnterRoom(*pParameter);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pParameter);
#else
            ::SendMessage(m_pHwnd, CR_WM_ENTER_ROOM, (WPARAM)pParameter, 0);
            delete pParameter;
#endif
        }
        else {
            delete pParameter;
        }
	}
	virtual void OnEnterAnother(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
        CCrInfo * pParameter = new CCrInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pParameter->SetQEventType((QEvent::Type)CR_WM_ENTER_ANOTHER);
#endif
		if (m_callback)
            m_callback->OnEnterAnother(*pParameter);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pParameter);
#else
            ::SendMessage(m_pHwnd, CR_WM_ENTER_ANOTHER, (WPARAM)pParameter, 0);
            delete pParameter;
#endif
        }
        else {
            delete pParameter;
        }
	}
	virtual void OnFCMNotify(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner) 
	{
		const mycp::bigint nChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id",(mycp::bigint)0);
		if (m_chatManager->GetChatInfo()->GetChatId() != nChatId)
			return;
		const int nResultValue = pResponseInfo->GetResultValue();
		const mycp::bigint nFromUserId = pResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
		const EB_NOTIFY_TYPE nNotifyType = (EB_NOTIFY_TYPE)pResponseInfo->m_pResponseList.getParameterValue("notify-type", (int)EB_NOTIFY_UNKNOWN);
		const mycp::bigint nNotifyId = pResponseInfo->m_pResponseList.getParameterValue("notify-id",(mycp::bigint)0);
		const tstring sNotifyData(pResponseInfo->m_pResponseList.getParameterValue("notify-data"));

        CCrNotifyInfo * pNotifyInfo = new CCrNotifyInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pNotifyInfo->SetQEventType((QEvent::Type)CR_WM_USER_NOTIFY);
#endif
        pNotifyInfo->m_nFromAccount = nFromUserId;
        pNotifyInfo->m_nNotifyType = nNotifyType;
        pNotifyInfo->m_nNotityId = nNotifyId;
        pNotifyInfo->m_sNotifyData = sNotifyData;
		if (m_callback)
            m_callback->OnUserNotify(*pNotifyInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pNotifyInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_USER_NOTIFY, (WPARAM)pNotifyInfo, 0);
            delete pNotifyInfo;
#endif
        }
        else {
            delete pNotifyInfo;
        }
	}

	virtual void OnP2POk(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		// 已经打通P2P通道
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const mycp::bigint sCallId = pResponseInfo->m_pResponseList.getParameterValue("call-id", (mycp::bigint)0);
		const int nP2PData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
		const int nResultValue = pResponseInfo->GetResultValue();
        CCrP2PInfo * pP2PInfo = new CCrP2PInfo(m_nParam,sCallId,sChatId,(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pP2PInfo->SetQEventType((QEvent::Type)CR_WM_P2P_OK);
#endif
        pP2PInfo->m_sFromAccount = sFromAccount;
        pP2PInfo->m_nP2PData = nP2PData;
		if (m_callback)
            m_callback->OnP2POk(*pP2PInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pP2PInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_P2P_OK, (WPARAM)pP2PInfo, 0);
            delete pP2PInfo;
#endif
        }
        else {
            delete pP2PInfo;
        }
	}
	virtual void OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		//const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
		const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
		const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
        CCrP2PInfo * pP2PInfo = new CCrP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pP2PInfo->SetQEventType((QEvent::Type)CR_WM_P2P_REQUEST);
#endif
        pP2PInfo->m_sFromAccount = sFromAccount;
        pP2PInfo->m_sFromLocalAddress = sLocalIp;
        pP2PInfo->m_sFromRemoteAddress = sRemoteIp;
        pP2PInfo->m_nP2PData = nRequestData;
		if (m_callback)
            m_callback->OnP2PRequest(*pP2PInfo);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pP2PInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_P2P_REQUEST, (WPARAM)pP2PInfo, 0);
            delete pP2PInfo;
#endif
        }
        else {
            delete pP2PInfo;
        }
	}
	virtual void OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		//const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
		const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
		const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
        CCrP2PInfo * pP2PInfo = new CCrP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pP2PInfo->SetQEventType((QEvent::Type)CR_WM_P2P_RESPONSE);
        pP2PInfo->m_nFlag = 0;
#endif
        pP2PInfo->m_sFromAccount = sFromAccount;
        pP2PInfo->m_sFromLocalAddress = sLocalIp;
        pP2PInfo->m_sFromRemoteAddress = sRemoteIp;
        pP2PInfo->m_nP2PData = nRequestData;
		if (m_callback)
            m_callback->OnP2PResponse(*pP2PInfo,0);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pP2PInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_P2P_RESPONSE, (WPARAM)pP2PInfo, 0);
            delete pP2PInfo;
#endif
        }
        else {
            delete pP2PInfo;
        }
	}
	virtual void OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCChatManager* pCMOwner)
	{
		if (pRequestInfo.get()==NULL) return;
		const int nResultValue = pResponseInfo->GetResultValue();
		const mycp::bigint sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("to-uid", (mycp::bigint)0);
        CCrP2PInfo * pP2PInfo = new CCrP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId(),(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
        pP2PInfo->SetQEventType((QEvent::Type)CR_WM_P2P_RESPONSE);
        pP2PInfo->m_nFlag = 1;
#endif
        pP2PInfo->m_sFromAccount = sFromAccount;
		if (m_callback)
            m_callback->OnP2PResponse(*pP2PInfo,1);
        if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pP2PInfo);
#else
            ::SendMessage(m_pHwnd, CR_WM_P2P_RESPONSE, (WPARAM)pP2PInfo, 1);
            delete pP2PInfo;
#endif
        }
        else {
            delete pP2PInfo;
        }
	}
};

Cchatroom::Cchatroom(void)
: m_handle(NULL)
{
}
Cchatroom::~Cchatroom(void)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_callback = NULL;
		pChatManager->m_pHwnd = NULL;
	}
	Stop();
}

tstring theSslPublicKey;
tstring theSslPrivateKey;
tstring theSslPrivatePwd;
void Cchatroom::SetSslKey(const char* sSslPublicKey,const char* sSslPrivateKey,const char* sSslPrivatePwd)
{
	theSslPublicKey = sSslPublicKey;
	theSslPrivateKey = sSslPrivateKey;
	theSslPrivatePwd = sSslPrivatePwd;
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
	}
}

int Cchatroom::Start(const char * address, const char * sAppName, unsigned int bindPort,bool bBuildResponseThread,int nUsesSsl,int nMaxWaitSecons)
{
	int result = 0;
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager == NULL)
	{
		bool bRetry = false;
		pChatManager = new CChatManager();
		pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
		result = pChatManager->m_chatManager->Start(CCgcAddress(address, CCgcAddress::ST_UDP), sAppName, bindPort,bBuildResponseThread,true,nUsesSsl,nMaxWaitSecons);
		if (result==-1)
		{
			// 有可能域名解析问题，再试一次；
			const tstring sServerAddress = GetAddressList(address);
			if (!sServerAddress.empty())
			{
				delete pChatManager;
				pChatManager = new CChatManager();
				pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
				result = pChatManager->m_chatManager->Start(CCgcAddress(sServerAddress, CCgcAddress::ST_UDP), sAppName, bindPort,bBuildResponseThread,true,nUsesSsl,nMaxWaitSecons);
				bRetry = true;
			}
		}
		if (!bRetry)
		{
			SaveAddressList(address);
		}
		m_handle = pChatManager;
	}else
	{
		result = 1;
	}
	return result;
}
tstring Cchatroom::GetAddress(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->GetAddress().address();
	}
	return "";
}
//tstring Cchatroom::GetLocalIp(void) const
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL && pChatManager->m_chatManager.get() != NULL && pChatManager->m_chatManager->sotp().get()!=NULL)
//	{
//		return pChatManager->m_chatManager->sotp()->doGetLocalIp();
//	}
//	return "";
//}
unsigned short Cchatroom::GetLocalPort(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL && pChatManager->m_chatManager.get() != NULL && pChatManager->m_chatManager->sotp().get()!=NULL)
	{
		return pChatManager->m_chatManager->sotp()->doGetLocalPort();
	}
	return 0;
}

bool Cchatroom::IsStart(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->IsStart();
	}
	return false;
}
void Cchatroom::SetCallBack(CChatRoomCallBack * pCallBack)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_callback = pCallBack;
	}
}
#ifdef _QT_MAKE_
void Cchatroom::SetMsgReceiver(QObject* pHwnd)
#else
void Cchatroom::SetMsgHwnd(HWND pHwnd)
#endif
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pHwnd = pHwnd;
	}
}
void Cchatroom::SetParam(unsigned long nParam)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_nParam = nParam;
		//pChatManager->m_sParam = "";
	}
}
//void Cchatroom::SetParam(const tstring& sParam)
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		pChatManager->m_nParam = 0;
//		pChatManager->m_sParam = sParam;
//	}
//}

void Cchatroom::Stop(void)
{
	ExitRoom(0);
	CChatManager * pChatManager = (CChatManager*)m_handle;
	m_handle = NULL;
	if (pChatManager != NULL)
	{
		pChatManager->m_chatManager->Stop();
		delete pChatManager;
	}
}
//int Cchatroom::SetResource(const char* sResourceId,const char* sAccount,const char* sResourceKey,const char* sFilePath)
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		unsigned int nFileSize = 0;
//		FILE * f = fopen(sFilePath, "rb");
//		if (f == NULL)
//		{
//			return -1;
//		}
//		_fseeki64(f, 0, SEEK_END);
//		nFileSize = _ftelli64(f);
//		fclose(f);
//		return pChatManager->m_chatManager->SendCRSet(sResourceId,sAccount,sResourceKey,sFilePath,nFileSize);
//		//return pChatManager->SetResource(sResourceId,sAccount,sResourceKey,sFilePath);
//	}
//	return -1;
//}

int Cchatroom::EnterRoom(cr::bigint sFromAccount, cr::bigint sChatId, const char* sCmKey, int nType,cr::bigint sCallId,cr::bigint sDepCode)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		//tstring strtemp(sChatId);
		//strtemp.append(sFromAccount);
		//MD5 md5key;
		//md5key.update(strtemp);
		//const tstring sChatKey = md5key.toString();
		return pChatManager->m_chatManager->SendCMEnter(0,0,sFromAccount,nType,sDepCode, sCallId, sChatId, sCmKey);
	}
	return -1;
}
bool Cchatroom::IsEnterOk(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->IsEnterOk();
	}
	return false;
}

int Cchatroom::GetAccount(cr::bigint & pOutAccount) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pOutAccount = pChatManager->m_chatManager->GetOwnerAccount();
		return 0;
	}
	return -1;
}
int Cchatroom::GetChatId(cr::bigint& pOutChatId) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CEBChatInfo::pointer pChatInfo = pChatManager->m_chatManager->GetChatInfo();
		if (pChatInfo.get() != NULL)
		{
			pOutChatId = pChatInfo->GetChatId();
			return 0;
		}
	}
	return -1;
}
int Cchatroom::SetCallId(cr::bigint nCallId)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CEBChatInfo::pointer pChatInfo = pChatManager->m_chatManager->GetChatInfo();
		if (pChatInfo.get() != NULL)
		{
			pChatInfo->SetCallId(nCallId);
			return 0;
		}
	}
	return -1;
}
int Cchatroom::GetCallId(cr::bigint & pOutCallId) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CEBChatInfo::pointer pChatInfo = pChatManager->m_chatManager->GetChatInfo();
		if (pChatInfo.get() != NULL)
		{
			pOutCallId = pChatInfo->GetCallId();
			return 0;
		}
	}
	return -1;
}

int Cchatroom::ExitRoom(int nExitSes)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMExit(nExitSes);
	}
	return -1;
}
int Cchatroom::Notify(cr::bigint nToUserId, int nNotifyType, cr::bigint nNotifyId, const char* sNotifyData)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMNotify(nToUserId, nNotifyType, nNotifyId, sNotifyData);
	}
	return -1;
}

size_t Cchatroom::GetUserSize(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CEBChatInfo::pointer pChatInfo = pChatManager->m_chatManager->GetChatInfo();
		if (pChatInfo.get() != NULL)
		{
			return pChatInfo->m_pChatUserList.size();
		}
	}
	return 0;
}
void Cchatroom::GetUserList(std::vector<cr::bigint> & pOutUserList) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CEBChatInfo::pointer pChatInfo = pChatManager->m_chatManager->GetChatInfo();
		if (pChatInfo.get() != NULL)
		{
			BoostReadLock rdlock(pChatInfo->m_pChatUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::const_iterator pIter = pChatInfo->m_pChatUserList.begin();
			for (;pIter!=pChatInfo->m_pChatUserList.end(); pIter++)
			{
				pOutUserList.push_back(pIter->first);
			}
		}
	}
}

void Cchatroom::RichBufferAddText(const char* sTextMsg)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pRichBuffer.AddText(sTextMsg);
	}
}
void Cchatroom::RichBufferAddObject(const char* pData, unsigned long nSize)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		if (nSize > 0 && pData != NULL)
		{
			char * pNewData = new char[nSize+1];
			memcpy(pNewData,pData,nSize);
			pChatManager->m_pRichBuffer.AddObject(pNewData,nSize);
		}
	}
}
void Cchatroom::RichBufferAddResource(const char* sResource)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pRichBuffer.AddResource(sResource);
	}
}
void Cchatroom::RichBufferClear(void)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pRichBuffer.clear();
	}
}
int Cchatroom::SendRichBuffser(cr::bigint sTo,bool bPrivate)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		if (pChatManager->m_pRichBuffer.IsEmpty()) return -1;
		int nPrivate = bPrivate?1:0;
		int ret = pChatManager->m_chatManager->SendCMMsg(CEBChatRoomRichMsg::create(pChatManager->m_pRichBuffer),const_send_data_size,0,sTo,nPrivate);
		pChatManager->m_pRichBuffer.clear();
		return ret;
	}
	return -1;
}

//int Cchatroom::SendUserData(const char * sUserData)
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		if (sUserData==NULL || strlen(sUserData)>1024) return -1;
//		return pChatManager->m_chatManager->SendCMMsg(0, EB_RICH_SUB_TYPE_USER_DATA, sUserData);
//	}
//	return -1;
//}
//int Cchatroom::SendMapPos(const char * sPosData)
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		if (sPosData==NULL || strlen(sPosData)>1024) return -1;
//		return pChatManager->m_chatManager->SendCMMsg(0, sPosData);
//	}
//	return -1;
//}

int Cchatroom::SendRich(const EB_ChatRoomRichMsg* pRichMsg,cr::bigint sTo,bool bPrivate)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		if (pRichMsg==NULL) return -1;
		int nPrivate = bPrivate?1:0;
		return pChatManager->m_chatManager->SendCMMsg(CEBChatRoomRichMsg::create(*pRichMsg),const_send_data_size,0,sTo,nPrivate);
	}
	return -1;
}
int Cchatroom::SendFile(const EBFileString &sFilePath,cr::bigint sTo,bool bPrivate,bool bSendOffFile)
{
#ifdef _QT_MAKE_
    return SendFile(sFilePath.toStdString().c_str(), sTo, bPrivate, bSendOffFile);
#else
    return SendFile(sFilePath.c_str(), sTo, bPrivate, bSendOffFile);
#endif
}
int Cchatroom::SendFile(const char * sFilePath,cr::bigint sTo, bool bPrivate,bool bSendOffFile)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		mycp::bigint nFileSize = 0;
		tstring sFileMd5String;
        if (pChatManager->m_chatManager->GetP2POwnerAccount()==0) {
            if (!GetFileMd5(sFilePath,nFileSize,sFileMd5String)) {
				return -1;
			}
        }
        else {

#ifdef _QT_MAKE_
            QFile file(sFilePath);
            if ( !file.open(QFile::ReadOnly) ) {
                return -1;
            }
            nFileSize = file.size();
            file.close();
#else
			FILE * f = fopen(sFilePath, "rb");
			if (f == NULL)
			{
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
#endif    // _QT_MAKE_
		}

		int nPrivate = bPrivate?1:0;
		int nSendOffFile = bSendOffFile?1:0;
		return pChatManager->m_chatManager->SendCMMsg(sFilePath,"",sFileMd5String,nFileSize,const_send_data_size,0,0,sTo,nPrivate,nSendOffFile);
	}
	return -1;
}
int Cchatroom::SendResource(cr::bigint sResourceId,const EBFileString &sFilePath,bool bNewUpload)
{
#ifdef _QT_MAKE_
    return SendResource(sResourceId, sFilePath.toStdString().c_str(), bNewUpload);
#else
    return SendResource(sResourceId, sFilePath.c_str(), bNewUpload);
#endif
}
int Cchatroom::SendResource(cr::bigint sResourceId,const char * sFilePath,bool bNewUpload)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		mycp::bigint nFileSize = 0;
		tstring sFileMd5String;
        /// sFilePath为空，表示通知上传文件而已
        if (bNewUpload && sFilePath!=0 && strlen(sFilePath)>0 && !GetFileMd5(sFilePath,nFileSize,sFileMd5String))
		{
			return -1;
		}
		for (int i=0;i<30;i++)
		{
			if (!pChatManager->m_chatManager->IsEnterOk())
			{
#ifdef WIN32
				Sleep(100);
#else
                usleep(100000);
#endif
				continue;
			}else
			{
				break;
			}
		}
		return pChatManager->m_chatManager->SendCMMsg(sFilePath,"",sFileMd5String,nFileSize,const_send_data_size,sResourceId,0,0,0,0);
	}
	return -1;
}

int Cchatroom::CancelSendingFile(mycp::bigint nMsgId)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMAck(nMsgId,0,EB_MAT_CANCEL);
	}
	return -1;
}

int Cchatroom::ReceiveFile(mycp::bigint nMsgId, const char * sSaveTo)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMReceiveFile(nMsgId, sSaveTo);
	}
	return -1;
}
int Cchatroom::SaveFile2CloudDrive(cr::bigint nMsgId)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMSave2CloudDrive(nMsgId);
	}
	return -1;
}
int Cchatroom::ResendMsg(cr::bigint nMsgId)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMAck(nMsgId,0,EB_MAT_RESEND);
	}
	return -1;
}

int Cchatroom::MsgAck(cr::bigint nMsgId,int nAckType)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendCMAck(nMsgId,0,(EB_MSG_ACK_TYPE)nAckType);
	}
	return -1;
}
int Cchatroom::RecviveResource(cr::bigint sResourceId,const EBFileString &sSaveTo)
{
#ifdef _QT_MAKE_
    return RecviveResource(sResourceId, sSaveTo.toStdString().c_str());
#else
    return RecviveResource(sResourceId, sSaveTo.c_str());
#endif
}
int Cchatroom::RecviveResource(mycp::bigint sResourceId,const char* sSaveTo)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		for (int i=0;i<30;i++)
		{
            if (!pChatManager->m_chatManager->IsEnterOk()) {
#ifdef WIN32
				Sleep(100);
#else
                usleep(100000);
#endif
				continue;
			}
		}
		return pChatManager->m_chatManager->SendCMReceiveResource(sResourceId, sSaveTo);
	}
	return -1;
}

int Cchatroom::SetDestAddress(int nUserIndex,const tstring& sToAddress)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_chatManager->SetDestAddress(nUserIndex,sToAddress);
	}
	return -1;
}
int Cchatroom::SendP2PRequest(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocalIp,int nRequestData)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendP2PRequest(nFromUid,nChatId,nToAccount,sLocalIp,nRequestData);
	}
	return -1;
}
int Cchatroom::SendP2PResponse(cr::bigint nFromUid,cr::bigint nChatId,cr::bigint nToAccount,const tstring& sLocalIp,int nResponseData)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
		pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nToAccount));
		return pChatManager->m_chatManager->SendP2PResponse(nFromUid,nChatId,nToAccount,sLocalIp,nResponseData,pRequestInfo);
	}
	return -1;
}
int Cchatroom::SendP2PTry(cr::bigint nCallId,cr::bigint nChatId,cr::bigint nFromAccount,int nP2PData)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->SendP2PTry(nCallId,nChatId,nFromAccount,0,nP2PData);
	}
	return -1;
}
void Cchatroom::SendP2PTest(unsigned short nTestCount)
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_chatManager->SendP2PTest(nTestCount);
	}
}
cr::bigint Cchatroom::GetP2POwnerAccount(void) const
{
	CChatManager * pChatManager = (CChatManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_chatManager->GetP2POwnerAccount();
	}
	return 0;
}

//void Cchatroom::SetVideoHwnd(HWND pHwnd)
//{
//	CChatManager * pManager = (CChatManager*)m_handle;
//	if (pManager != NULL)
//	{
//		pManager->m_pVideoHwnd = pHwnd;
//	}
//}
//
//int Cchatroom::VideoRequest(int nVideoType)
//{
//	CChatManager * pManager = (CChatManager*)m_handle;
//	if (pManager != NULL)
//	{
//		return pManager->m_chatManager->SendVRequest(nVideoType);
//	}
//	return -1;
//}
//int Cchatroom::VideoAck(bool bAccept)
//{
//	CChatManager * pManager = (CChatManager*)m_handle;
//	if (pManager != NULL)
//	{
//		//return pManager->VideoAck(sChatId,bAccept);
//	}
//	return -1;
//}
//int Cchatroom::VideoEnd(void)
//{
//	CChatManager * pManager = (CChatManager*)m_handle;
//	if (pManager != NULL)
//	{
//		//return pManager->VideoEnd(sChatId);
//	}
//	return -1;
//}
//int Cchatroom::GetVideoUserId(int & pOutMyVideoUserId, std::vector<int>& pOutFromVideoUserId)
//{
//	CChatManager * pManager = (CChatManager*)m_handle;
//	if (pManager != NULL)
//	{
//		//return pManager->GetVideoUserId(sChatId,pOutMyVideoUserId,pOutFromVideoUserId);
//	}
//	return -1;
//}
//
//

} // namespace entboost
