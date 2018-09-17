#ifdef WIN32
#pragma warning(disable:4819 4267)
#endif // WIN32

#ifdef _QT_MAKE_
#include <QFile>
#endif

#ifdef __MACH__
#include "mach_uses.h"
#endif
//#include "StdAfx.h"
#include "POPCChatManager.h"
#include "eb_defines.h"
#ifdef WIN32
#include <Mmsystem.h>
#else
#include <time.h>   
inline unsigned long timeGetTime()  
{  
    unsigned long uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
        uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    return uptime;
} 
#define _FILE_OFFSET_BITS 64
#endif

namespace entboost {

class CPOPChatRoomRichMsg
        : public cgcObject
{
public:
    typedef boost::shared_ptr<CPOPChatRoomRichMsg> pointer;
    static CPOPChatRoomRichMsg::pointer create(const CEBChatRoomRichMsg::pointer& pRichMsg)
    {
        return CPOPChatRoomRichMsg::pointer(new CPOPChatRoomRichMsg(pRichMsg));
    }
    CPOPChatRoomRichMsg(const CEBChatRoomRichMsg::pointer& pRichMsg)
        : m_pRichMsg(pRichMsg)
    {
    }
    virtual cgcObject::pointer copyNew(void) const
    {
        CPOPChatRoomRichMsg::pointer result = CPOPChatRoomRichMsg::create(m_pRichMsg);
        return result;
    }
    CEBChatRoomRichMsg::pointer m_pRichMsg;
};

class CSendInfo
{
public:
    typedef boost::shared_ptr<CSendInfo> pointer;
    static CSendInfo::pointer create(const char * data, unsigned long size)
    {
        return CSendInfo::pointer(new CSendInfo(data, size));
    }

    CSendInfo(const char * data, unsigned long size)
        : m_data(data)
        , m_size(size)
        , m_sended(0)
    {
    }
    const char * GetData(void) const {return m_data+m_sended;}
    unsigned long GetSize(void) const {return m_size;}
    unsigned long GetUnSendSize(void) const {return m_size-m_sended;}
    void SetSendSize(unsigned long sended) {m_sended += sended;}
private:
    const char *	m_data;
    unsigned long	m_size;
    unsigned long	m_sended;
};

CPOPCChatManager::CPOPCChatManager(CPOPCMHandler * pPOPHandler,int nProcessTimer)
    : m_nProcessTimer(nProcessTimer)
    , m_bKilled(false)
    , m_pPOPHandler(pPOPHandler)
    , m_bEnterOk(false),m_sOwnerAccount(0),m_nLogonType(0)
    , m_bAutoAckSuccess(true)

{
    m_pChatInfo = CEBChatInfo::create();
    m_nBigData = 0;
    m_nActiveTimeoutCount = 0;
    m_bLocalIpAddress = false;
    m_nP2PTryOwnerAccount = 0;
    m_nP2PData = 0;
    m_nOnP2POk = -1;
}

CPOPCChatManager::~CPOPCChatManager(void)
{
    Stop();
    m_pChatInfo.reset();
}
void CPOPCChatManager::cm_response_thread_svr(void)
{
    //CPOPCChatManager * pSvr = (CPOPCChatManager*)lparam;
    while (true)
    {
        try
        {
            DoResponse();
        }catch(std::exception&)
        {}catch(...)
        {}
        if (IsKilled())
        {
            break;
        }
#ifdef WIN32
        Sleep(1000);
#else
        usleep(1000000);
#endif
    }
}
void CPOPCChatManager::cm_process_thread_svr(void)
{
    while (true)
    {
        try
        {
            DoProcess();
        }catch(std::exception&)
        {}catch(...)
        {}
        if (IsKilled())
        {
            break;
        }
#ifdef WIN32
        Sleep(1000);
#else
        usleep(1000000);
#endif
    }
}

int CPOPCChatManager::Start(const CCgcAddress & address, const tstring & sAppName, unsigned int bindPort,bool bBuildResponseThread,bool bBuildProcessThread,int nUsesSsl,int nMaxWaitSecons)
{
    if (!StartClient(address, bindPort, 500))
        return -1;
    m_bLocalIpAddress = address.getip()=="127.0.0.1"?true:false;
    sotp()->doSetResponseHandler((CgcClientHandler*)this);
    if (sAppName.empty())
        sotp()->doSetAppName(POP_APP_NAME_CHAT_MANAGER);
    else
        sotp()->doSetAppName(sAppName);
    sotp()->doSetConfig(SOTP_CLIENT_CONFIG_SOTP_VERSION,SOTP_PROTO_VERSION_21);
    sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,nUsesSsl);
    //sotp()->doSetConfig(SOTP_CLIENT_CONFIG_ACCEPT_ENCODING,SOTP_DATA_ENCODING_GZIP|SOTP_DATA_ENCODING_DEFLATE);
    sotp()->doSendOpenSession();
#ifndef _QT_MAKE_
    sotp()->doSetEncoding(_T("UTF8"));
#endif
    sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND);

    m_bKilled = false;
    if (bBuildResponseThread && m_pResponseThread.get() == NULL)
    {
        m_pResponseThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CPOPCChatManager::cm_response_thread_svr, this)));
//        boost::thread_attributes attrs;
//        attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//        m_pResponseThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CPOPCChatManager::cm_response_thread_svr, this)));
    }
    if (bBuildProcessThread && m_pProcessThread.get() == NULL)
    {
        m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CPOPCChatManager::cm_process_thread_svr, this)));
//        boost::thread_attributes attrs;
//        attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//        m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CPOPCChatManager::cm_process_thread_svr, this)));
    }
    return 0;
}
bool CPOPCChatManager::IsStart(void) const
{
    return IsClientStarted();
}
void CPOPCChatManager::Stop(void)
{
    m_bLocalIpAddress = false;
    m_nP2PTryOwnerAccount = 0;
    m_pSendFileThreadList.clear();
    m_pChatInfo->reset();
    m_bKilled = true;
#ifdef WIN32
    Sleep(500);
#else
    usleep(500000);
#endif
    if (m_pResponseThread.get() != NULL)
    {
        m_pResponseThread->join();
        m_pResponseThread.reset();
    }
    if (m_pProcessThread.get() != NULL)
    {
        m_pProcessThread->join();
        m_pProcessThread.reset();
    }
    StopClient();
}
void CPOPCChatManager::SetEncoding(const std::string& v)
{
    if (IsClientStarted())
    {
        sotp()->doSetEncoding(v);
    }
}
CEBChatInfo::pointer CPOPCChatManager::GetChatInfo(void) const
{
    return m_pChatInfo;
}

CChatMsgInfo::pointer CPOPCChatManager::GetChatMsgInfo(mycp::bigint nMsgId, bool erase) const
{
    //if (m_pChatInfo.get() == NULL)
    //	return NullChatMsgInfo;
    CChatMsgInfo::pointer pChatMsgInfo;
    m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo, erase);
    return pChatMsgInfo;
}
void CPOPCChatManager::DelChatMsgInfo(mycp::bigint nMsgId)
{
    //if (m_pChatInfo.get() != NULL)
    m_pChatInfo->m_pChatMsgList.remove(nMsgId);
}

bool CPOPCChatManager::doSendDSAck(const CChatMsgInfo::pointer& pChatMsgInfo)
{
    // m_nDSAckCount 重新请求多少次数据补偿，收到数据会清空
    // 4*250=1S
    // 40S
    // *** P2P不判断超时
    if (m_nP2PTryOwnerAccount==0)
    {
        if (pChatMsgInfo->m_nDSAckCount>4*40 ||							// 40S
                (pChatMsgInfo->m_nDSAckCount>4*30 && pChatMsgInfo->GetSize()<512))	// 小数据：20S
        {
            // 太久没有收到数据，直接超时处理；
            //this->SendCMAck(pChatMsgInfo->GetMsgId(),0,EB_MAT_ERROR);
            return false;
        }
    }

    bool bOffset = false;
    if ((pChatMsgInfo->m_dwMsgData&EB_MSG_DATA_OFFSET)==EB_MSG_DATA_OFFSET)
    {
        bOffset = true;
        pChatMsgInfo->m_dwMsgData |= EB_MSG_DATA_OFFSET;
    }else
    {
        pChatMsgInfo->m_dwMsgData &= ~EB_MSG_DATA_OFFSET;
    }

    tstring sIS;
    {
        const int const_max_os_count = (int)(m_bLocalIpAddress||(m_nP2PTryOwnerAccount>0&&((m_nP2PData&1)==1))?(EB_MAX_REQUEST_OS_COUNT*3):EB_MAX_REQUEST_OS_COUNT);
        char lpszBuffer[14];
        int noffset = 0;	// 跳过前面一些数据，保证不会重复发送
        int oscount = 0;
        BoostReadLock rdlock(pChatMsgInfo->m_pWaitStreamList.mutex());
        CLockMap<int, bool>::iterator pIterRecvStreamList = pChatMsgInfo->m_pWaitStreamList.begin();
        for (; pIterRecvStreamList!=pChatMsgInfo->m_pWaitStreamList.end(); pIterRecvStreamList++)
        {
            if (bOffset && (++noffset) <EB_MAX_REQUEST_OS_COUNT && pChatMsgInfo->m_pWaitStreamList.size()>EB_MAX_REQUEST_OS_COUNT*2)
            {
                continue;
            }
            const int nindex = pIterRecvStreamList->first;
            sprintf(lpszBuffer,"%d,",nindex);
            //const mycp::bigint nindex = pIterRecvStreamList->first;
            //sprintf(lpszBuffer,"%d,",(int)(nindex/pChatMsgInfo->m_nDataStreamSize));
            sIS.append(lpszBuffer);
            if ((++oscount)== const_max_os_count)
                break;
        }
    }
    if (!sIS.empty())
    {
        SendDSAck(pChatMsgInfo->GetMsgId(),sIS,false);	// 不需要NACK
        //SendDSAck(pChatMsgInfo->GetMsgId(),sIS,true);
        pChatMsgInfo->m_tRecvTime = timeGetTime();
        pChatMsgInfo->m_nDSAckCount++;
    }

    return true;
}

void CPOPCChatManager::DoProcess(void)
{
    unsigned int theIndex = 0;
    while (!m_bKilled)
    {
#ifdef WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
        theIndex++;
        DoProcess2(theIndex);
    }
}
void CPOPCChatManager::DoProcess2(unsigned int theTenMSIndex)
{
    if ((theTenMSIndex % 200)==1 && !m_pReqResponseList.empty())	// 200=2秒，处理一次
    {
        // 处理有已经返回数据，超过10S没有新数据，删除requestlist，用于提高效率
        BoostWriteLock wtlock(m_pReqResponseList.mutex());
        CLockList<CPOPSotpRequestInfo::pointer>::iterator pIterReqResponstList = m_pReqResponseList.begin();
        for (; pIterReqResponstList!=m_pReqResponseList.end(); )
        {
            const CPOPSotpRequestInfo::pointer& pRequestInfo = *pIterReqResponstList;
            if (pRequestInfo->GetResponseTime()>0 && pRequestInfo->GetResponseTime() + 10 < time(0))
            {
                m_pRequestList.remove(pRequestInfo->GetCallId());
                m_pReqResponseList.erase(pIterReqResponstList++);
            }else
            {
                pIterReqResponstList++;
            }
        }
    }
    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    if (pChatInfo.get() != NULL && !pChatInfo->m_pChatMsgList.empty())
    {
        // ** m_nP2PTryOwnerAccount>0 是P2P，P2P控制1000=1秒请求一次；
        const unsigned long tNowTime = timeGetTime();
        const int theWaitTime = pChatInfo->m_sGroupCode==0?((m_bLocalIpAddress || m_nP2PTryOwnerAccount>0)?1000:580):800;
        //const int theWaitTime = pChatInfo->m_sGroupCode==0?((m_bLocalIpAddress || m_nP2PTryOwnerAccount>0)?100:580):800;
        //unsigned long tCheckFile = 0;
        std::vector<mycp::bigint> pDeleteMsgId;
        {
            BoostReadLock rdlock(pChatInfo->m_pChatMsgList.mutex());
            CLockMap<mycp::bigint, CChatMsgInfo::pointer>::iterator pIterChatMsgList = pChatInfo->m_pChatMsgList.begin();
            for (; pIterChatMsgList!=pChatInfo->m_pChatMsgList.end(); pIterChatMsgList++)
            {
                const CChatMsgInfo::pointer& pChatMsgInfo = pIterChatMsgList->second;
                //检查未接收完成文件（或数据）
                const int nwaitms = (int)pChatInfo->m_pChatMsgList.size(false)*50;
                if (pChatMsgInfo->m_tRecvTime == 0 || pChatMsgInfo->m_nDataStreamSize==0 || (pChatMsgInfo->m_tRecvTime+(time_t)theWaitTime+(time_t)nwaitms)>=(time_t)tNowTime)	// 250
                {
                    continue;
                }

                // 需要补偿数据
                if (m_nP2PTryOwnerAccount>0)
                {
                    // 这是P2P数据；不主动请求ds_ack，由P2P对方，发送ds_check，这边处理即可；
                    // 这是主要主动处理接收完成；
                    //if (pChatMsgInfo->m_tRecvTime>0 && (pChatMsgInfo->m_tRecvTime+10*1000) < timeGetTime())	// 超过10秒钟，主动请求数据
                    {
                        // 超过1分钟没有收到新数据，当超时处理
                        //m_pChatInfo->m_pChatMsgList.erase(pIterChatMsgList);	// 不能删除，删除界面事件不好处理；
                        if (!doSendDSAck(pChatMsgInfo))
                        {
                            pDeleteMsgId.push_back(pChatMsgInfo->GetMsgId());
                        }
                    }
                }else
                {
                    if (!doSendDSAck(pChatMsgInfo))
                    {
                        pDeleteMsgId.push_back(pChatMsgInfo->GetMsgId());
                    }
                }
            }
        }
        for (size_t i=0;i<pDeleteMsgId.size();i++)
        {
            pChatInfo->m_pChatMsgList.remove(pDeleteMsgId[i]);
        }
    }
    // process timeout
    try
    {
        if (m_nProcessTimer>0&&(theTenMSIndex%(m_nProcessTimer*100)==1))
            m_pPOPHandler->OnProcessTimer(this);
    }catch(std::exception&)
    {}catch(...)
    {}

    // process timeout
    const int const_time_flag = (m_bLocalIpAddress||m_nP2PTryOwnerAccount>0)?100:200;	// P2P 100=1秒，处理一次；其他200=2秒，处理一次；
    const int const_time_second = (m_bLocalIpAddress||m_nP2PTryOwnerAccount>0)?8:15;		// P2P 8秒超时，其他15秒超时；
    if ((theTenMSIndex % const_time_flag)==1 && !m_pRequestList.empty())
    {
        BoostReadLock rdlock(m_pRequestList.mutex());
        CLockMap<unsigned long, CPOPSotpRequestInfo::pointer>::iterator pIter;
        for (pIter=m_pRequestList.begin(); pIter!=m_pRequestList.end(); pIter++)
        {
            const CPOPSotpRequestInfo::pointer pRequestInfo = pIter->second;
            if (pRequestInfo->GetResponseTime()==0 && (pRequestInfo->GetRequestTime() + const_time_second) <= time(0))	// 28
            {
                // timeout
                const unsigned long nCid = pIter->first;
                const CPOPSotpRequestInfo::pointer pRequestInfoTemp = pRequestInfo;
                rdlock.unlock();
                m_pRequestList.remove(nCid);
                m_pPOPHandler->OnTimeout(pRequestInfoTemp,this);
                break;
            }
        }
    }
}
inline bool IsDisableZipFile(const tstring& sFileName)
{
    if (sFileName.size()<3 || sFileName.rfind(".")==std::string::npos) return true;	// false
    const size_t nSize = sFileName.size();
    tstring sExt(sFileName.substr(nSize-3));
    std::transform(sExt.begin(), sExt.end(), sExt.begin(), ::tolower);
    return (sExt=="zip" ||
            sExt=="rar" ||
            sExt=="tar" ||
            sExt=="iso" ||
            sExt=="exe" ||
            sExt=="bz2" ||
            sExt=="gz" ||
            sExt.find("7z")!=std::string::npos)?true:false ;
}
void CPOPCChatManager::ProcFCMAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    BOOST_ASSERT (pResponseInfo.get() != NULL);
    //if (m_pChatInfo.get() == NULL) return;
    const int nResultValue = pResponseInfo->GetResultValue();
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    if (m_pChatInfo->GetChatId() != sChatId) return;

    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid", (mycp::bigint)0);
    //const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pResponseInfo->m_pResponseList.getParameterValue("msg-type", (int)EB_MSG_UNKNOWN);
    const EB_MSG_ACK_TYPE acktype = (EB_MSG_ACK_TYPE)pResponseInfo->m_pResponseList.getParameterValue("ack-type", (int)EB_MAT_SUCCESS);
    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        if (acktype==EB_MAT_SUCCESS && pChatInfo->m_sGroupCode==0)	// 消息回执
        {
            m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)nResultValue,acktype,pRequestInfo,pChatInfo,nMsgId,sFromAccount,this);
        }else if (acktype==EB_MAT_WITHDRAW)
        {
            m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)nResultValue,acktype,pRequestInfo,pChatInfo,nMsgId,sFromAccount,this);
        }
        return;
    }
    const int nAcceptEncoding = pResponseInfo->m_pResponseList.getParameterValue("accept-encoding", (int)0);
    switch (acktype)
    {
    case EB_MAT_REQUEST:
    {
        if (pChatMsgInfo->m_bTranOk)
            break;	// 已经传输完成
        if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
        {
            boost::mutex::scoped_lock lock(pChatMsgInfo->m_mutexMsg);
            if (pChatMsgInfo->m_pFileHandle != NULL)
            {
                /// 已经处理，传输文件，只需要处理一次。
                break;
            }
            const tstring sFileName(pChatMsgInfo->GetContent());
            const mycp::bigint ntotalsize = pChatMsgInfo->GetSize();

//#ifdef Q_OS_ANDROID
//            FILE * logf = fopen("/entboost_log.txt", "w");
//            if (logf!=NULL) {
//                fwrite(sFileName.c_str(), 1, sFileName.size(), logf);
//                fclose(logf);
//                logf = 0;
//            }
//#endif

#ifdef _QT_MAKE_
            const QString filePathTemp(sFileName.c_str());
            FILE * f = fopen(filePathTemp.toLocal8Bit().constData(),"rb");
#else
            FILE * f = fopen(sFileName.c_str(), "rb");
#endif
            if (f == NULL)
            {
                break;
            }
            pChatMsgInfo->m_pFileHandle = f;
            pChatMsgInfo->m_tBeginTranTime = timeGetTime();
            CSendFileThread::pointer pThreadSendFile = CSendFileThread::create(this, ntotalsize, nMsgId, f, pChatMsgInfo->m_nDataStreamSize);
            pThreadSendFile->SetAcceptEncoding(nAcceptEncoding);
            pThreadSendFile->SetFileName(sFileName);
#ifdef Q_OS_ANDROID
            /// for test
            pThreadSendFile->SetDisableZip(true);
#else
            pThreadSendFile->SetDisableZip(IsDisableZipFile(sFileName));
#endif
            pThreadSendFile->SetP2P(m_nP2PTryOwnerAccount>0);
            pThreadSendFile->SetLocalIpAddress(m_bLocalIpAddress);

            m_pSendFileThreadList.insert(nMsgId, pThreadSendFile);
            pThreadSendFile->SetThread();
        }
    }break;
    case EB_MAT_SUCCESS:
    case EB_MAT_ERROR:
    case EB_MAT_CANCEL:
    case EB_MAT_WITHDRAW:
    {
        pChatMsgInfo->m_pRecvUserList.remove(sFromAccount);
        if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
        {
            CEBAccountInfo::pointer pMsgFromAccount = CGC_OBJECT_CAST<CEBAccountInfo>(pChatMsgInfo->GetFromAccount());
            if (pMsgFromAccount->GetUserId() == m_sOwnerAccount)
            {
                if (acktype != EB_MAT_SUCCESS && pChatMsgInfo->m_pRecvUserList.empty())
                {
                    // 取消或错误
                    DelSendFileThread(nMsgId);
                    boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
                    if (pChatMsgInfo->m_pFileHandle != NULL)
                    {
                        fclose(pChatMsgInfo->m_pFileHandle);
                        pChatMsgInfo->m_pFileHandle = NULL;
                    }
                }
            }else
            {
                boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
                if (pChatMsgInfo->m_pFileHandle != NULL)
                {
                    fclose(pChatMsgInfo->m_pFileHandle);
                    pChatMsgInfo->m_pFileHandle = NULL;
                }
            }
            if (acktype==EB_MAT_SUCCESS)
                m_pPOPHandler->OnSentFile((EB_STATE_CODE)nResultValue,pRequestInfo,pChatMsgInfo, sFromAccount,this);
            else
            {
                m_pPOPHandler->OnCancelFile((EB_STATE_CODE)nResultValue,pRequestInfo,pChatMsgInfo,sFromAccount,this);
                if (pChatMsgInfo->m_pRecvUserList.empty())
                {
                    // 取消或错误
                    m_pPOPHandler->OnSentFile((EB_STATE_CODE)nResultValue,pRequestInfo,pChatMsgInfo, 0,this);	// 最后删除文件
                    m_pFilePercent.remove(nMsgId);
                    pChatInfo->m_pChatMsgList.remove(nMsgId);
                    if (acktype==EB_MAT_WITHDRAW)
                        m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)nResultValue,acktype,pRequestInfo,pChatInfo,nMsgId,sFromAccount,this);
                }
            }
        }else if (acktype==EB_MAT_SUCCESS && pChatInfo->m_sGroupCode==0)	// 消息回执
        {
            //ProcessMsgSendOk(pRequestInfo,pChatMsgInfo,EB_STATE_OK);	// ** 加了这个，会导致客户端不检查 eb_check ，然后服务端不会保存漫游消息
            m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)nResultValue,acktype,pRequestInfo,pChatInfo,nMsgId,sFromAccount,this);
        }else if (acktype==EB_MAT_WITHDRAW)
        {
            m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)nResultValue,acktype,pRequestInfo,pChatInfo,nMsgId,sFromAccount,this);
            pChatInfo->m_pChatMsgList.remove(nMsgId);
        }
    }break;
    default:
        break;
    }
}

void CPOPCChatManager::GetFileSpeechInfo(mycp::bigint nMsgId,CSpeechInfo & pOutSpeechInfo,int &pOutFiles)
{
    pOutFiles = (int)m_pSendFileThreadList.size();
    pOutSpeechInfo = m_pFilePercent[nMsgId];
}
inline bool FileIsExist(const char* lpszFile)
{
#ifdef _QT_MAKE_
    return QFile::exists(lpszFile);
#else
    FILE * f = fopen(lpszFile,"r");
    if (f==NULL)
        return false;
    fclose(f);
    return true;
#endif
}
inline void GetFileExt(const std::string &sFileName, tstring & sOutExt, tstring* sOutName=NULL)
{
    const std::string::size_type find = sFileName.rfind(".");
    if (find != std::string::npos && find > 0) {
        sOutExt = sFileName.substr(find);
        if (sOutExt.find("/")!=std::string::npos || sOutExt.find("\\")!=std::string::npos) {
            if (sOutName!=NULL)
                *sOutName = sFileName;
            sOutExt.clear();
            return;
        }
        if (sOutName!=NULL)
            *sOutName = sFileName.substr(0, find);
        //sOutExt = sFileName.substr(find+1);
    }
    else if (sOutName!=NULL) {
        *sOutName = sFileName;
    }
}
void CPOPCChatManager::ProcFDSSend(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,bool bP2PMsg)
{
    BOOST_ASSERT (pResponseInfo->m_pAttachMent.get() != 0);
    //if (m_pChatInfo.get() == NULL) return;
    const cgcAttachment::pointer & pAttachment = pResponseInfo->m_pAttachMent;
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    if (m_pChatInfo->GetChatId() != sChatId) return;

    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        return;
    }

    if (pChatMsgInfo->m_nAttachEncoding==0 &&				// ** 不可能会收到无用数据，所以必须判断 pChatMsgInfo->m_nAttachEncoding==0，避免m_nAttachEncoding被清空
            pResponseInfo->m_pAttachMent->getIndex()==0)
    {
        pChatMsgInfo->m_nAttachEncoding = pResponseInfo->m_pResponseList.getParameterValue("attach-encoding", (int)0);
    }
    if (pChatMsgInfo->m_nAttachEncoding!=0 && pChatMsgInfo->m_nAttachZipSize==0)
    {
        pChatMsgInfo->m_nAttachZipSize = pResponseInfo->m_pResponseList.getParameterValue("attach-zipsize", (mycp::bigint)0);
        if (pChatMsgInfo->m_nAttachZipSize>0 && !pChatMsgInfo->m_pWaitStreamList.empty(false))
        {
            // ** 清空后面无用 index
            const int nOrgCount = (int)((pChatMsgInfo->GetRealSize()+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);			// *压缩前，有多少个数据包
            const int nZipCount = (int)((pChatMsgInfo->m_nAttachZipSize+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);	// *压缩后，有多少个数据包
            //printf("**** eb_ds_send RealSize=%lld(%d), nAttachZipSize=%lld(%d)\n",pChatMsgInfo->GetRealSize(),nOrgCount,nAttachZipSize,nZipCount);
            for (int i=nZipCount; i<nOrgCount; i++)
            {
                pChatMsgInfo->m_pWaitStreamList.remove(i);
            }
        }
    }

    // 这段代码也有用，因为在某些网络情况下，有可能fds_send的数据会先到
    if (pChatMsgInfo->m_tBeginTranTime==0 &&	// ** 避免添加多次
            (pChatMsgInfo->m_pWaitStreamList.empty(false) || pChatMsgInfo->m_nDataStreamSize==0))
    {
        boost::mutex::scoped_lock lock(pChatMsgInfo->m_mutexMsg);
        if (pChatMsgInfo->GetMsgType() == EB_MSG_RICH)
        {
            if (!pChatMsgInfo->m_pRecvAttachList.empty())
            {
                // 该数据包已经接收
                return;
            }
        }else if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
        {
#ifdef _QT_MAKE_
            if (pChatMsgInfo->m_pFileHandle!=0 || pChatMsgInfo->m_sFileName.isEmpty()) {
#else
            if (pChatMsgInfo->m_pFileHandle != NULL || pChatMsgInfo->m_sFileName.empty()) {
#endif
                /// 该数据包已经接收
                return;
            }
        }

        if (pChatMsgInfo->GetSize()==0)
            pChatMsgInfo->SetSize(pAttachment->getTotal());
        if (pChatMsgInfo->m_nDataStreamSize == 0)							// 必须判断，
            pChatMsgInfo->m_nDataStreamSize = pAttachment->getAttachSize();
        const mycp::bigint nSizeTemp = pChatMsgInfo->m_nAttachZipSize>0?pChatMsgInfo->m_nAttachZipSize:pAttachment->getTotal();
        const int ncount = (int)((nSizeTemp+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);
        //const int ncount = (int)((pAttachment->getTotal()+pAttachment->getAttachSize()-1)/pAttachment->getAttachSize());
        for (int i=0; i<ncount; i++)
        {
            pChatMsgInfo->m_pWaitStreamList.insert(i,false,false);
        }
    }

    //if (pChatMsgInfo->m_pWaitStreamList.size() <= 2)
    //{
    //	char lpszBuffer[24];
    //	sprintf(lpszBuffer,"111 recv,size=%d;index=%lld",pChatMsgInfo->m_pWaitStreamList.size(),pAttachment->getIndex());
    //	MessageBox(NULL,lpszBuffer,"",MB_OK);
    //}
    const int nIndex = (int)(pAttachment->getIndex()/pChatMsgInfo->m_nDataStreamSize);
    if (!pChatMsgInfo->m_pWaitStreamList.remove(nIndex,false))
    {
        return;
    }
    //if (pChatMsgInfo->m_pWaitStreamList.size() <= 2)
    //{
    //	int i=0;
    //	//char lpszBuffer[24];
    //	//sprintf(lpszBuffer,"222 recv,size=%d;index=%lld",pChatMsgInfo->m_pWaitStreamList.size(),pAttachment->getIndex());
    //	//MessageBox(NULL,lpszBuffer,"",MB_OK);
    //}
    if (pChatMsgInfo->m_tBeginTranTime==0)
    {
        /// ** 这里先创建好；
        if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE) {
            boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
            if (pChatMsgInfo->m_pFileHandle == NULL) {
                //pChatMsgInfo->m_pFileHandle = fopen(pChatMsgInfo->m_sFileName.c_str(), "wb");	// w+b
                //if (pChatMsgInfo->m_pFileHandle == NULL)
                {
                    EBFileString sFilePathTemp = pChatMsgInfo->m_sFileName + ".ebtemp";
                    tstring sFileNameTemp;
                    tstring sFileExtTemp;
                    int nFileIndexTemp = 0;
#ifdef _QT_MAKE_
                    while (FileIsExist(sFilePathTemp.toStdString().c_str()) && nFileIndexTemp<100) {
#else
                    char lpszBuffer[260];
                    while (FileIsExist(sFilePathTemp.c_str()) && nFileIndexTemp<100) {
#endif
                        if ( sFileNameTemp.empty() ) {
#ifdef _QT_MAKE_
                            GetFileExt(pChatMsgInfo->m_sFileName.toStdString(),sFileExtTemp,&sFileNameTemp);
#else
                            GetFileExt(pChatMsgInfo->m_sFileName,sFileExtTemp,&sFileNameTemp);
#endif
                        }
                        if (sFileExtTemp.empty()) {
                            /// 没有扩展名，可能是资源文件，尝试直接使用临时文件打开
#ifdef _QT_MAKE_
                            pChatMsgInfo->m_pFileHandle = fopen(sFilePathTemp.toLocal8Bit().constData(), "wb");	// w+b
#else
                            pChatMsgInfo->m_pFileHandle = fopen(sFilePathTemp.c_str(), "wb");	// w+b
#endif
                            if (pChatMsgInfo->m_pFileHandle != 0) {
                                /// 可以创建打开，直接使用
                                break;
                            }
                        }
#ifdef _QT_MAKE_
                        sFilePathTemp = QString("%1(%2)%3.ebtemp").arg(sFileNameTemp.c_str()).arg(++nFileIndexTemp).arg(sFileExtTemp.c_str());
#else
                        sprintf(lpszBuffer,"%s(%d)%s.ebtemp",sFileNameTemp.c_str(),(++nFileIndexTemp),sFileExtTemp.c_str());
                        sFilePathTemp = lpszBuffer;
#endif
                    }
                    if (pChatMsgInfo->m_pFileHandle==0) {
#ifdef _QT_MAKE_
                        pChatMsgInfo->m_pFileHandle = fopen(sFilePathTemp.toLocal8Bit().constData(), "wb");	// w+b
#else
                        pChatMsgInfo->m_pFileHandle = fopen(sFilePathTemp.c_str(), "wb");	// w+b
#endif
                        if (pChatMsgInfo->m_pFileHandle == NULL)
                            return;
                    }
                    pChatMsgInfo->m_sFileName = sFilePathTemp;
                }

                /// 启动一个保存线程（放里面安全一次，避免启动多次）
                CSendFileThread::pointer pThreadSendFile = CSendFileThread::create(this,pChatMsgInfo);
                if (m_pSendFileThreadList.insert(nMsgId, pThreadSendFile, false)) {
                    pThreadSendFile->SetThread();
                }
            }
            //// 启动一个保存线程
            //CSendFileThread::pointer pThreadSendFile = CSendFileThread::create(this,pChatMsgInfo);
            //if (m_pSendFileThreadList.insert(nMsgId, pThreadSendFile, false))
            //{
            //	pThreadSendFile->SetThread();
            //}
        }
        pChatMsgInfo->m_tBeginTranTime = timeGetTime();
    }
    pChatMsgInfo->m_tRecvTime = timeGetTime();
    pChatMsgInfo->m_nDSAckCount = 0;
    if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
    {
        pChatMsgInfo->m_pRecvAttachList.insert(pAttachment->getIndex(),pAttachment,false);
    }else if (pChatMsgInfo->GetMsgType() == EB_MSG_RICH)
    {
        pChatMsgInfo->m_pRecvAttachList.insert(pAttachment->getIndex(),pAttachment,false);
    }

    if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE && pChatMsgInfo->GetSize() > 0 && pChatMsgInfo->m_nDataStreamSize > 0 &&
            (pChatMsgInfo->m_pWaitStreamList.empty(false) || pChatMsgInfo->m_tPercentTime == 0 || (pChatMsgInfo->m_tRecvTime-pChatMsgInfo->m_tPercentTime) >= 2000))
    {
        CSpeechInfo preFilePercent = m_pFilePercent[nMsgId];
        const time_t prePercentTime = pChatMsgInfo->m_tPercentTime > 0 ? pChatMsgInfo->m_tPercentTime : pChatMsgInfo->m_tRecvTime;
        pChatMsgInfo->m_tPercentTime = pChatMsgInfo->m_tRecvTime;
        const mycp::bigint nSizeTemp = pChatMsgInfo->m_nAttachZipSize>0?pChatMsgInfo->m_nAttachZipSize:pChatMsgInfo->GetRealSize();
        const int ncount = (int)((nSizeTemp+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);
        const double percent = (ncount-pChatMsgInfo->m_pWaitStreamList.size())*100.0/ncount;
        int ncursecond = (int)((pChatMsgInfo->m_tPercentTime-prePercentTime)/1000);
        int nsecond1 = (int)((timeGetTime() - pChatMsgInfo->m_tBeginTranTime)/1000);
        int nsecond2 = 0;
        int nspeech1 = 0;
        int nspeech2 = 0;
        if (ncursecond > 0)
        {
            nspeech1 = max(0,(int)(((double)nSizeTemp*((percent-preFilePercent.m_percent)/100.0))/ncursecond));
        }
        if (nsecond1 > 0)
        {
            nspeech2 = max(0,(int)(((double)nSizeTemp*(percent/100.0))/nsecond1));
            nsecond2 = (int)(((double)nSizeTemp*(1.0-(percent/100.0)))/nspeech1);
        }else if (nspeech2 > 0)
            nsecond2 = (int)(((double)nSizeTemp*(1.0-(percent/100.0)))/nspeech2);
        preFilePercent.m_percent = percent;
        preFilePercent.m_nCurSpeed = nspeech1;
        preFilePercent.m_nAvrSpeed = nspeech2;
        preFilePercent.m_time = time(0);
        m_pFilePercent[nMsgId] = preFilePercent;
        m_pPOPHandler->OnFilePercent(pRequestInfo,pChatMsgInfo->m_sResourceId,nMsgId, percent, nsecond1, nsecond2, nspeech1, nspeech2,this);

        if (bP2PMsg)
        {
            // 这是P2P消息，需要返回百分比信息；
            sotp()->doBeginCallLock();
            sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
            sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
            sotp()->doAddParameter(CGC_PARAMETER("percent", percent));
            const bool bNeedAck = percent>=100.0?true:false;
            //if (bNeedAck)
            //{
            //	int i=0;
            //}
            sotp()->doSendCallResult(EB_STATE_OK,pResponseInfo->GetCallId(),pResponseInfo->GetCallSign(),bNeedAck);
        }
    }

    if (pChatMsgInfo->m_pWaitStreamList.empty(false))
    {
        /// 信息接收完成
        if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
        {
            while (!pChatMsgInfo->m_pRecvAttachList.empty())
            {
#ifdef WIN32
                Sleep(10);
#else
                usleep(10000);
#endif
            }
            DelSendFileThread(nMsgId);

            boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
            if (pChatMsgInfo->m_pFileHandle != NULL)
            {
                fclose(pChatMsgInfo->m_pFileHandle);
                pChatMsgInfo->m_pFileHandle = NULL;
            }
            m_pFilePercent.remove(nMsgId);
            const std::string::size_type nFileNameSize = pChatMsgInfo->m_sFileName.size();
#ifdef _QT_MAKE_
            if (nFileNameSize>7 && pChatMsgInfo->m_sFileName.right(7)==".ebtemp")
#else
            if (nFileNameSize>7 && pChatMsgInfo->m_sFileName.substr(nFileNameSize-7)==".ebtemp")
#endif
            {
#ifdef _QT_MAKE_
                const EBFileString sOldTempFile(pChatMsgInfo->m_sFileName);
                pChatMsgInfo->m_sFileName = pChatMsgInfo->m_sFileName.left(nFileNameSize-7);
#else
                const tstring sOldTempFile(pChatMsgInfo->m_sFileName);
                pChatMsgInfo->m_sFileName = pChatMsgInfo->m_sFileName.substr(0,nFileNameSize-7);
#endif
                try
                {
                    if (pChatMsgInfo->m_nAttachEncoding==2) {
                        /// *2=deflate
#ifdef _QT_MAKE_
                        FILE * source = fopen(sOldTempFile.toLocal8Bit().constData(),"rb");
                        FILE * dest = source==NULL?NULL:fopen(pChatMsgInfo->m_sFileName.toLocal8Bit().constData(),"wb");
#else
                        FILE * source = fopen(sOldTempFile.c_str(),"rb");
                        FILE * dest = source==NULL?NULL:fopen(pChatMsgInfo->m_sFileName.c_str(),"wb");
#endif
                        bool bOk = false;
                        if (source!= NULL && dest!=NULL)
                        {
                            bOk = UnZipFile2File(source, 0, dest, 0, NULL)==Z_OK?true:false;
                        }
                        if (source!= NULL)
                            fclose(source);
                        if (dest!=NULL)
                            fclose(dest);
                        if (bOk) {
#ifdef _QT_MAKE_
                            QFile::remove(sOldTempFile);
#else
                            namespace fs = boost::filesystem;
                            fs::path pathOld(sOldTempFile.string());
                            fs::remove(pathOld);
#endif
                        }
                    }
                    else {
#ifdef _QT_MAKE_
                        const QString oldFile(sOldTempFile);
                        const QString newFile(pChatMsgInfo->m_sFileName);
                        QFile::remove(newFile);
                        QFile::rename(oldFile, newFile);
#else
                        namespace fs = boost::filesystem;
                        fs::path pathNew( pChatMsgInfo->m_sFileName.string() );
                        fs::path pathOld( sOldTempFile.string());
                        fs::remove(pathNew);
                        boost::system::error_code ec;
                        fs::rename(pathOld,pathNew,ec);
//                        fs::copy_file(pathOld,pathNew,fs::copy_option::overwrite_if_exists);
//                        fs::remove(pathOld);
#endif
                    }
                }
                catch(const boost::exception&) {
                }
                catch(const std::exception&) {
                }
                catch(...) {
                }
                //}else if (pChatMsgInfo->m_nAttachEncoding==2)	// *2=deflate
                //{
                //	FILE * source = fopen(sOldTempFile.c_str(),"rb");
                //	FILE * dest = fopen(pChatMsgInfo->m_sFileName.c_str(),"wb");
                //	bool bOk = false;
                //	if (source!= NULL && dest!=NULL)
                //	{
                //		bOk = UnZipFile2File(source, 0, dest, 0, NULL)==Z_OK?true:false;
                //	}
                //	if (source!= NULL)
                //		fclose(source);
                //	if (dest!=NULL)
                //		fclose(dest);
                //	if (bOk)
                //	{
                //		namespace fs = boost::filesystem;
                //		fs::path pathOld( sOldTempFile.string());
                //		fs::remove(pathOld);
                //	}
            }
        }
        //SendDSAck(pChatMsgInfo->GetMsgId());	/// 这个可以不要
        const int ret = m_pPOPHandler->OnFCMMsg(pChatMsgInfo,NullPOPSotpRequestInfo, pResponseInfo,this);
        if (m_bAutoAckSuccess && ret!=-1)	/// -1错误不响应
            SendCMAck(pChatMsgInfo->GetMsgId(),0,EB_MAT_SUCCESS);
        m_pChatInfo->m_pChatMsgList.remove(nMsgId);
    }
}

void CPOPCChatManager::ProcessMsgSendOk(const CPOPSotpRequestInfo::pointer & pRequestInfo,const CChatMsgInfo::pointer& pChatMsgInfo,int nResultValue)
{
    //if (m_pChatInfo.get() == NULL) return;
    if (pChatMsgInfo.get() == NULL) return;
    const mycp::bigint nMsgId = pChatMsgInfo->GetMsgId();
    DelSendFileThread(nMsgId);	// 这个必须放在前面
    if (pChatMsgInfo->m_bTranOk) return;	// 避免多次响应
    pChatMsgInfo->m_bTranOk = true;
    //if (nResultValue==EB_STATE_MSG_NOT_EXIST)
    //	nResultValue = EB_STATE_OK;
    if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE)
    {
        {
            boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
            if (pChatMsgInfo->m_pFileHandle != NULL)
            {
                fclose(pChatMsgInfo->m_pFileHandle);
                pChatMsgInfo->m_pFileHandle = NULL;
            }
        }
        //if (nResultValue != EB_STATE_FILE_ALREADY_EXIST && pChatMsgInfo->m_pRecvUserList.empty())
        //{
        //	// 避免多次响应
        //	m_pFilePercent.remove(nMsgId);
        //	return;
        //}
        m_pPOPHandler->OnSentFile((EB_STATE_CODE)nResultValue,pRequestInfo,pChatMsgInfo, m_sOwnerAccount,this);
        //m_pPOPHandler->OnSentFile((EB_STATE_CODE)nResultValue,NullPOPSotpRequestInfo,pChatMsgInfo, m_sOwnerAccount,this);
        m_pFilePercent.remove(nMsgId);
        if (pChatMsgInfo->m_pRecvUserList.empty())
        {
            m_pChatInfo->m_pChatMsgList.remove(nMsgId);
        }
    }else if (pChatMsgInfo->GetMsgType() == EB_MSG_RICH)
    {
        //if (pChatMsgInfo->GetSubType()>=EB_RICH_SUB_TYPE_MAP_POS)
        //{
        //	CChatMsgInfo::pointer pChatMsgInfo;
        //	if (m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo,true))
        //	{
        //		CPOPChatRoomRichMsg::pointer pPOPRichMsg = CGC_OBJECT_CAST<CPOPChatRoomRichMsg>(pChatMsgInfo->m_pRichMsg);
        //		BOOST_ASSERT (pPOPRichMsg.get() != 0);
        //		const CEBChatRoomRichMsg::pointer& pRichMsg = pPOPRichMsg->m_pRichMsg;
        //		m_pPOPHandler->OnSendingRich((EB_STATE_CODE)nResultValue,pRequestInfo,pRichMsg,m_sOwnerAccount,0,0,this);
        //	}
        //}else
        if (nResultValue==EB_STATE_OK || nResultValue==EB_STATE_MSG_NOT_EXIST)
        {
            m_pChatInfo->m_pChatMsgList.remove(nMsgId);
        }else
        {
            CChatMsgInfo::pointer pChatMsgInfo;
            if (m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo,true))
            {
                CPOPChatRoomRichMsg::pointer pPOPRichMsg = CGC_OBJECT_CAST<CPOPChatRoomRichMsg>(pChatMsgInfo->m_pRichMsg);
                BOOST_ASSERT (pPOPRichMsg.get() != 0);
                const CEBChatRoomRichMsg::pointer& pRichMsg = pPOPRichMsg->m_pRichMsg;
                m_pPOPHandler->OnSendingRich((EB_STATE_CODE)nResultValue,pRequestInfo,pRichMsg,m_sOwnerAccount,0,0,this);
            }
        }
    }

}
int CPOPCChatManager::ParseString(const char * lpszString, const char * lpszInterval, std::vector<int> & pOut)
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
            pOut.push_back(atoi(sIn.substr(nFindBegin).c_str()));
            break;
        }
        if (find==nFindBegin)
        {
            //pOut.push_back("");	// 空
        }else
        {
            pOut.push_back(atoi(sIn.substr(nFindBegin, (find-nFindBegin)).c_str()));
        }
        nFindBegin = find+nIntervalLen;
    }
    return (int)pOut.size();
    //tstring sIn(lpszString);
    //const size_t nIntervalLen = strlen(lpszInterval);
    //pOut.clear();
    //while (!sIn.empty())
    //{
    //	std::string::size_type find = sIn.find(lpszInterval);
    //	if (find == std::string::npos)
    //	{
    //		pOut.push_back(atoi(sIn.c_str()));
    //		break;
    //	}
    //	if (find==0)
    //	{
    //		//pOut.push_back("");	// 空
    //	}else
    //		pOut.push_back(atoi(sIn.substr(0, find).c_str()));
    //	sIn = sIn.substr(find+nIntervalLen);
    //}
    //return (int)pOut.size();
}
void CPOPCChatManager::ProcDSCheck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    if (pRequestInfo.get() == NULL)
        return;
    //if (m_pChatInfo.get() == NULL)
    //	return;

    const mycp::bigint nMsgId = pRequestInfo->m_pRequestList.getParameterValue("cm-msg-id", (mycp::bigint)0);
    //const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    //const tstring sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("cm-from", "");
    //const tstring sChatId = pRequestInfo->m_pRequestList.getParameterValue("cm-chat-id", "");

    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        DelSendFileThread(nMsgId);	// 这个必须放在前面
        m_pFilePercent.remove(nMsgId);
        return;
    }
    const int nResultValue = pResponseInfo->GetResultValue();
    if (nResultValue == EB_STATE_NEED_RESEND)
    {
        // 需要补偿数据
        CSendFileThread::pointer pSendDataThread;
        if (m_pSendFileThreadList.find(pChatMsgInfo->GetMsgId(), pSendDataThread))
        {
            const tstring sOS = pResponseInfo->m_pResponseList.getParameterValue("is","");
            std::vector<int> pList;
            const int nCount = ParseString(sOS.c_str(),",",pList);
            for (int i=0; i<nCount; i++)
            {
                const mycp::bigint nindex = pList[i]*pChatMsgInfo->m_nDataStreamSize;
                if (pChatMsgInfo->GetMsgType() == EB_MSG_FILE ||
                        pChatMsgInfo->GetMsgType() == EB_MSG_RICH)
                {
                    pSendDataThread->m_pResendIndex.insert(nindex,true,false);
                    //pSendDataThread->m_pResendIndex.add(nindex);
                }
            }
        }
    }else// if (nResultValue == EB_STATE_OK)	// **有可能对方先接收完成，导致本端收到EB_STATE_MSG_NOT_EXIST返回，同样完成。
    {
        // 传输完成
        CSendFileThread::pointer pSendDataThread;
        if (nResultValue == EB_STATE_OK || !m_pSendFileThreadList.find(pChatMsgInfo->GetMsgId(), pSendDataThread) || pSendDataThread->m_pResendIndex.empty())
        {
            ProcessMsgSendOk(pRequestInfo,pChatMsgInfo,nResultValue);
        }
    }
}
void CPOPCChatManager::ProcDSSend(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    //if (m_pChatInfo.get() == NULL)
    //	return;
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    if (m_pChatInfo->GetChatId() != sChatId)
        return;
    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        return;
    }
    const time_t prePercentTime = pChatMsgInfo->m_tPercentTime > 0 ? pChatMsgInfo->m_tPercentTime : timeGetTime();
    pChatMsgInfo->m_tPercentTime = timeGetTime();
    CSpeechInfo preFilePercent = m_pFilePercent[nMsgId];
    const mycp::bigint nSizeTemp = pChatMsgInfo->m_nAttachZipSize>0?pChatMsgInfo->m_nAttachZipSize:pChatMsgInfo->GetRealSize();
    const double percent = pResponseInfo->m_pResponseList.getParameterValue("percent", 0.0);
    int ncursecond = (int)((pChatMsgInfo->m_tPercentTime-prePercentTime)/1000);
    int nsecond1 = (int)((timeGetTime() - pChatMsgInfo->m_tBeginTranTime)/1000);	// 传输时间S
    int nsecond2 = 0;														// 剩余时间S
    int nspeech1 = 0;														// 当前速度 1=1Byte
    int nspeech2 = 0;														// 平均速度
    if (ncursecond > 0)
    {
        nspeech1 = max(0,(int)(((double)nSizeTemp*((percent-preFilePercent.m_percent)/100.0))/ncursecond));
    }
    if (nsecond1 > 0)
    {
        nspeech2 = max(0,(int)(((double)nSizeTemp*(percent/100.0))/nsecond1));
        nsecond2 = (int)(((double)nSizeTemp*(1.0-(percent/100.0)))/nspeech1);
    }else if (nspeech2 > 0)
        nsecond2 = (int)(((double)nSizeTemp*(1.0-(percent/100.0)))/nspeech2);
    preFilePercent.m_percent = percent;
    preFilePercent.m_nCurSpeed = nspeech1;
    preFilePercent.m_nAvrSpeed = nspeech2;
    preFilePercent.m_time = time(0);
    m_pFilePercent[nMsgId] = preFilePercent;
    m_pPOPHandler->OnFilePercent(pRequestInfo,pChatMsgInfo->m_sResourceId,nMsgId, percent, nsecond1, nsecond2, nspeech1, nspeech2,this);
}
//
//void CPOPCChatManager::ProcCRSet(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
//{
//	if (pRequestInfo.get() == NULL) return;
//	const tstring sResourceId = pRequestInfo->m_pRequestList.getParameterValue("r-id", "");
//	const tstring sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("from", "");
//	const tstring sFilePath = pRequestInfo->m_pRequestList.getParameterValue("file-path", "");
//	const int nFileSize = pRequestInfo->m_pRequestList.getParameterValue("file-size", (int)0);
//	const tstring sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", "");
//	const int nResultValue = pResponseInfo->GetResultValue();
//	if (nResultValue == EB_STATE_OK)
//	{
//		m_sOwnerAccount = sFromAccount;
//		if (m_pChatInfo.get() == NULL)
//		{
//			m_pChatInfo = CEBChatInfo::create(sChatId, "");
//		}
//		m_pChatInfo->m_bAccountChat = true;
//		if (!m_pChatInfo->m_pChatUserList.exist(sFromAccount))
//		{
//			m_pChatInfo->m_pChatUserList.insert(sFromAccount, CEBAccountInfo::create(sFromAccount));
//		}
//		SendCMMsg(sFilePath,nFileSize,"",0);
//	}else
//	{
//		m_pPOPHandler->OnSetResourceResponse(sResourceId);
//	}
//}
void CPOPCChatManager::ProcDSAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    if (pRequestInfo.get() == NULL) return;
    const mycp::bigint nMsgId = pRequestInfo->m_pRequestList.getParameterValue("cm-msg-id", (mycp::bigint)0);
    //const mycp::bigint sChatId = pRequestInfo->m_pRequestList.getParameterValue("cm-chat-id", (mycp::bigint)0);
    const int nResultValue = pResponseInfo->GetResultValue();
    if (nResultValue == EB_STATE_EXCESS_QUOTA_ERROR)
        return;
    if (nResultValue != EB_STATE_OK)
    {
        // 响应失败 error
        m_pChatInfo->m_pChatMsgList.remove(nMsgId);
        SendCMAck(nMsgId,0,EB_MAT_ERROR);

        //if (nMsgType == EB_MSG_FILE)
        //{
        //	// 记录下文件信息
        //	CChatMsgInfo::pointer pChatMsgInfo;
        //	if (!m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
        //	{
        //		pChatMsgInfo = CChatMsgInfo::create(nMsgId, pChatInfo, pFromAccountInfo, nMsgType);
        //		pChatInfo->m_pChatMsgList.insert(nMsgId, pChatMsgInfo);
        //	}
    }
}
void CPOPCChatManager::ProcFUMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo)
{
    //const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
    const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("msg-type", (int)EB_MSG_UNKNOWN);
    //const tstring sMsgContent(pSotpResponseInfo->m_pResponseList.getParameterValue("msg-content"));
    //const mycp::bigint sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code",(mycp::bigint)0);
    //const tstring sFromInfo(pSotpResponseInfo->m_pResponseList.getParameterValue("f-info"));

    //CEBAppMsgInfo::pointer pMsgInfo = CEBAppMsgInfo::create(nMsgType, sMsgContent);
    ////pMsgInfo->m_sFromAccount = sFromAccount;
    //pMsgInfo->m_nFromUserId = nFromUserId;
    //pMsgInfo->m_sGroupCode = sDepCode;
    //pMsgInfo->m_sFromInfo = sFromInfo;
    //ProcessP2PMsgInfo(pMsgInfo);
    switch (nMsgType)
    {
    case EB_MSG_ONLINE_ANOTHER:
    {
        m_pPOPHandler->OnEnterAnother(NullPOPSotpRequestInfo, pSotpResponseInfo,this);
        m_pChatInfo->reset();
        m_bEnterOk = false;
        m_sOwnerAccount = 0;
        m_nLogonType = 0;
    }break;
    default:
        break;
    }
}
//void CPOPCChatManager::ProcFCMNotify(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo)
//{
//	const mycp::bigint nChatId = pSotpResponseInfo->m_pResponseList.getParameterValue("chat-id",(mycp::bigint)0);
//	if (m_pChatInfo->GetChatId() != nChatId)
//		return;
//	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
//	const EB_NOTIFY_TYPE nNotifyType = (EB_NOTIFY_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("notify-type", (int)EB_NOTIFY_UNKNOWN);
//	const mycp::bigint nNotifyId = pSotpResponseInfo->m_pResponseList.getParameterValue("notify-id",(mycp::bigint)0);
//	const tstring sNotifyData(pSotpResponseInfo->m_pResponseList.getParameterValue("notify-data"));
//
//	m_pPOPHandler->OnFCMNotify(NullPOPSotpRequestInfo, pSotpResponseInfo,this);
//}

void CPOPCChatManager::ProcCMMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    if (pRequestInfo.get() == NULL) return;
    //if (m_pChatInfo.get() == NULL) return;
    const int nResultValue = pResponseInfo->GetResultValue();
    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pRequestInfo->m_pRequestList.getParameterValue("cm-msg-type", (int)EB_MSG_UNKNOWN);
    const int nSubType = pRequestInfo->m_pRequestList.getParameterValue("cm-sub-type", (int)0);
    const mycp::bigint sToAccount = pRequestInfo->m_pRequestList.getParameterValue("cm-to", (mycp::bigint)0);
    const int nPrivate = pRequestInfo->m_pRequestList.getParameterValue("cm-private", (int)0);
    const int nSendOffFile = pRequestInfo->m_pRequestList.getParameterValue("cm-off-file", (int)0);
    const mycp::bigint sResourceId = pRequestInfo->m_pRequestList.getParameterValue("cm-res-id", (mycp::bigint)0);
    //const tstring sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("cm-from", "");
    //const tstring sChatId = pRequestInfo->m_pRequestList.getParameterValue("cm-chat-id", "");
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-id", nMsgId));

    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pChatInfo->m_pChatUserList.find(m_sOwnerAccount, pFromAccountInfo))
    {
        return;
    }
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        pChatMsgInfo = CChatMsgInfo::create(nMsgId, pChatInfo, pFromAccountInfo, nMsgType,nSubType);
        pChatInfo->m_pChatMsgList.insert(nMsgId, pChatMsgInfo);
    }
    pChatMsgInfo->m_sResourceId = sResourceId;
    pChatMsgInfo->m_nOffFile = nSendOffFile;
    if (pChatInfo->m_bAccountChat)
    {
    }else
    {
        if (sToAccount>0 && nPrivate)
        {
            pChatMsgInfo->m_pRecvUserList.insert(sToAccount, CEBLogonTypeList::create(),false);
        }else
        {
            BoostReadLock rdlock(pChatInfo->m_pChatUserList.mutex());
            CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
            for (pIter=pChatInfo->m_pChatUserList.begin(); pIter!=pChatInfo->m_pChatUserList.end(); pIter++)
            {
                const CEBAccountInfo::pointer& pToAccountInfo = pIter->second;
                if (pToAccountInfo->GetUserId() == m_sOwnerAccount)
                {
                    continue;
                }
                pChatMsgInfo->m_pRecvUserList.insert(pToAccountInfo->GetUserId(), CEBLogonTypeList::create(),false);
            }
        }
    }
    const mycp::bigint ntotalsize = pRequestInfo->m_pRequestList.getParameterValue("cm-size", (mycp::bigint)0);
    const int nPackLen = pRequestInfo->m_pRequestList.getParameterValue("cm-pack-len", (int)0);
    pChatMsgInfo->SetSize(ntotalsize);
    pChatMsgInfo->m_nDataStreamSize = nPackLen;
    if (nMsgType == EB_MSG_FILE)
    {
        // 记录下文件信息
        const tstring sFileName = pRequestInfo->m_pRequestList.getParameterValue("cm-file-name", "");
        pChatMsgInfo->SetContent(sFileName);
        m_pPOPHandler->OnSendingFile((EB_STATE_CODE)nResultValue,pRequestInfo,pChatMsgInfo,this);
        if (nResultValue==EB_STATE_FILE_ALREADY_EXIST)
        {
            // 文件已经存在
            ProcessMsgSendOk(pRequestInfo,pChatMsgInfo,nResultValue);
        }
        return;	// 对方接收才发送文件
    }else if (nMsgType == EB_MSG_RICH)
    {

        const cgcParameter::pointer richmsgtemp = pRequestInfo->m_pRequestList.getParameter("cm-rich-msg");
        //if (pRequestInfo->m_pRichMsg.get() == NULL)
        if (richmsgtemp.get() == NULL || richmsgtemp->getObject().get() == NULL)
        {
            pChatInfo->m_pChatMsgList.remove(nMsgId);
            return;
        }

        CPOPChatRoomRichMsg::pointer pPOPRichMsg = CGC_OBJECT_CAST<CPOPChatRoomRichMsg>(richmsgtemp->getObject());
        BOOST_ASSERT (pPOPRichMsg.get() != 0);
        pChatMsgInfo->m_pRichMsg = pPOPRichMsg;
        const CEBChatRoomRichMsg::pointer& pRichMsg = pPOPRichMsg->m_pRichMsg;
        pRichMsg->SetMsgId(nMsgId);
        //BOOST_ASSERT (pRichMsg->m_charFormat.get() != 0);
        //if (pChatMsgInfo->GetSubType()>=EB_RICH_SUB_TYPE_MAP_POS)
        //{
        //	// 发送返回
        //	ProcessMsgSendOk(pRequestInfo,pChatMsgInfo,nResultValue);
        //	return;
        //}
        tstring sSendFormat;
        std::vector<size_t> pObjectIndex;
        if (!pRichMsg->GetFormatMsg(sSendFormat, pObjectIndex))
        {
            // ?? error
            pChatInfo->m_pChatMsgList.remove(nMsgId);
            return;
        }
        if (nResultValue == (int)EB_STATE_OK)
        {
            // build send list
            std::vector<CSendInfo::pointer> pSendInfoList;
            for (size_t i=0; i<pObjectIndex.size(); i++)
            {
                size_t index = pObjectIndex[i];
                const EB_ChatRoomMsgItem* pMsgItem = pRichMsg->GetMsgItem(index);
                BOOST_ASSERT (pMsgItem != 0);
                pSendInfoList.push_back(CSendInfo::create(pMsgItem->GetData(), pMsgItem->GetSize()));
            }
            // send data
            const int nsendcount = (int)((ntotalsize+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);
            char unsigned * lpBuffer = new unsigned char[pChatMsgInfo->m_nDataStreamSize+1];
            bool bAddCharFormat = false;
            for (int i=0; i<nsendcount; i++)
            {
                cgcAttachment::pointer pAttach = cgcAttachment::create();
                pAttach->setTotal(ntotalsize);
                mycp::bigint nindex = i*pChatMsgInfo->m_nDataStreamSize;
                pAttach->setIndex(nindex);

                size_t nBufferSize = 0;
                memset(lpBuffer, 0, sizeof(lpBuffer));
                if (!bAddCharFormat && pRichMsg->IsInvalidateCharFormat())
                {
                    bAddCharFormat = true;
                    char lpszCharFormatSize[10];
                    sprintf(lpszCharFormatSize, "f%02d,", (int)pRichMsg->GetCharFormat().GetSize());
                    //sprintf(lpszCharFormatSize, "%03d,", pRichMsg->m_charFormat->GetSize());
                    memcpy(lpBuffer, lpszCharFormatSize, 4);
                    memcpy(lpBuffer+4, pRichMsg->GetCharFormat().GetData(), pRichMsg->GetCharFormat().GetSize());
                    nBufferSize = pRichMsg->GetCharFormat().GetSize() + 4;	// char format size;
                }
                if (sSendFormat.size() >= pChatMsgInfo->m_nDataStreamSize-nBufferSize)
                {
                    //pAttach->setAttach((const unsigned char*)sSendFormat.c_str(), pChatMsgInfo->m_nDataStreamSize);
                    //sSendFormat = sSendFormat.substr(pChatMsgInfo->m_nDataStreamSize);
                    memcpy(lpBuffer+nBufferSize, sSendFormat.c_str(), pChatMsgInfo->m_nDataStreamSize-nBufferSize);
                    pAttach->setAttach(lpBuffer, pChatMsgInfo->m_nDataStreamSize);
                    sSendFormat = sSendFormat.substr(pChatMsgInfo->m_nDataStreamSize-nBufferSize);
                }else
                {
                    if (!sSendFormat.empty())
                    {
                        memcpy(lpBuffer+nBufferSize, sSendFormat.c_str(), sSendFormat.size());
                        nBufferSize += sSendFormat.size();
                        sSendFormat.clear();
                    }
                    while (!pSendInfoList.empty())
                    {
                        size_t sended = pChatMsgInfo->m_nDataStreamSize-nBufferSize;
                        CSendInfo::pointer pSendInfo = pSendInfoList[0];
                        if (pSendInfo->GetUnSendSize() == 0)
                        {
                            pSendInfoList.erase(pSendInfoList.begin());
                            if (pSendInfoList.empty())
                            {
                                break;
                            }
                            continue;
                        }else if (pSendInfo->GetUnSendSize() >= sended)
                        {
                            memcpy(lpBuffer+nBufferSize, pSendInfo->GetData(), sended);
                            pSendInfo->SetSendSize((unsigned long)sended);
                            pAttach->setAttach(lpBuffer, pChatMsgInfo->m_nDataStreamSize);
                            nBufferSize = 0;	// 清空，外面不会重新添加
                            break;
                        }else
                        {
                            memcpy(lpBuffer+nBufferSize, pSendInfo->GetData(), pSendInfo->GetUnSendSize());
                            nBufferSize += pSendInfo->GetUnSendSize();
                            pSendInfo->SetSendSize(pSendInfo->GetUnSendSize());
                        }
                    }
                    if (nBufferSize > 0)
                    {
                        pAttach->setAttach(lpBuffer, (unsigned int)nBufferSize);
                    }
                }
                pRequestInfo->m_pAttachList.insert(pAttach->getIndex(), pAttach);
            }
            delete []lpBuffer;

            // 启动一个线程，用于定期检查，保证数据一定能发送成功
            CSendFileThread::pointer pThreadSendFile = CSendFileThread::create(this,ntotalsize,pChatMsgInfo->m_nDataStreamSize, nMsgId, pRequestInfo);
            pThreadSendFile->SetP2P(m_nP2PTryOwnerAccount>0);
            pThreadSendFile->SetLocalIpAddress(m_bLocalIpAddress);
            m_pSendFileThreadList.insert(nMsgId, pThreadSendFile);
            pThreadSendFile->SetThread();
        }
        m_pPOPHandler->OnSendingRich((EB_STATE_CODE)nResultValue,pRequestInfo,pRichMsg,m_sOwnerAccount,sToAccount,nPrivate,this);
    }else
    {
        // error
        pChatInfo->m_pChatMsgList.remove(nMsgId);
        return;
    }
    //SendDSCheck(nMsgId, pRequestInfo);
    //m_pPOPHandler->OnCMMsgResponse(pRequestInfo, pResponseInfo);
}
void CPOPCChatManager::ProcP2PTry(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    // 收到对方P2P请求
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
    const mycp::bigint sCallId = pResponseInfo->m_pResponseList.getParameterValue("call-id", (mycp::bigint)0);
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    const int nTryCount = pResponseInfo->m_pResponseList.getParameterValue("try-count", (int)0);
    const int nP2PData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
    if (nTryCount < 5)
    {
        // 回复P2PTRY
        // nFromAccount=0：表示使用当前from用户；
        if (m_nOnP2POk==1 && nTryCount<=1)	// 1 重新请求；
            m_nOnP2POk = -1;
        SendP2PTry(sCallId,sChatId,0,nTryCount+1,nP2PData);
    }else if (m_nOnP2POk!=1)
    {
        // 已经打通P2P
        //m_nOnP2POk = 0;
        if (!m_sSslPublicKey.empty() && !m_sSslPrivateKey.empty() && !m_sSslPrivatePwd.empty())
        {
            unsigned long nHasSslPassword = 0;
            sotp()->doGetConfig(SOTP_CLIENT_CONFIG_HAS_SSL_PASSWORD,&nHasSslPassword);
            if (nHasSslPassword==0)
            {
                sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PUBLIC_KEY,(unsigned long)m_sSslPublicKey.c_str());
                sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_KEY,(unsigned long)m_sSslPrivateKey.c_str());
                sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_PWD,(unsigned long)m_sSslPrivatePwd.c_str());
                sotp()->doSetConfig(SOTP_CLIENT_CONFIG_USES_SSL,1);
                if (sFromAccount>m_sOwnerAccount)
                {
                    // 发送SSL证书，获取通讯密码sslpassword，对方也会生成相同密码
                    m_nOnP2POk = 0;
                    sotp()->doSendOpenSession(0);
                }
                return;
            }
        }
        m_nOnP2POk = 1;
        for (int i=0;i<3;i++)
        {
            // 发送多次，确保对方能收到，保证二边互通；
            SendP2PTry(sCallId,sChatId,m_nP2PTryOwnerAccount,nTryCount+1,nP2PData);
#ifdef WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
        }

        sotp()->doStartActiveThread(15,10);
        //sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,10);
        m_pPOPHandler->OnP2POk(pRequestInfo, pResponseInfo,this);
    }
}

//void CPOPCChatManager::ProcFCMP2P(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
//{
//	//const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
//	//const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
//	//const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
//	//if (m_pChatInfo->GetChatId() != sChatId)
//	//	return;
//
//	//CEBAccountInfo::pointer pFromAccountInfo;
//	//if (!pChatInfo->m_pChatUserList.find(sFromAccount, pFromAccountInfo, bExitRoom))
//	//{
//	//	return;
//	//}
//	m_pPOPHandler->OnFCMP2P(pRequestInfo, pResponseInfo,this);
//}

void CPOPCChatManager::ProcFCMExit(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    //if (m_pChatInfo.get() == NULL) return;
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid", (mycp::bigint)0);
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    //const int nHangup = pResponseInfo->m_pResponseList.getParameterValue("hangup", (int)0);
    if (m_pChatInfo->GetChatId() != sChatId)
        return;

    bool bExitRoom = m_pChatInfo->m_sGroupCode==0?true:false;
    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pChatInfo->m_pChatUserList.find(sFromAccount, pFromAccountInfo, bExitRoom))
    {
        return;
    }
    m_pPOPHandler->OnFCMExit(pRequestInfo, pResponseInfo,this);
    if (pChatInfo->m_pChatUserList.empty())
    {
        pChatInfo->reset();
    }
}
void CPOPCChatManager::ProcCMMAck(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    // 这里主要用于，接收资源文件，第二次请求
    // 或者接收离线文件，存入云盘返回
    //if (m_pChatInfo.get() == NULL) return;
    if (pRequestInfo.get()==NULL) return;

    const int nSave2CloudDrive = pRequestInfo->m_pRequestList.getParameterValue("save-2-cloud", (int)0);
    if (nSave2CloudDrive==1)
    {
        // 存入云盘返回。
        const mycp::bigint sResourceId = pResponseInfo->m_pResponseList.getParameterValue("r-id", (mycp::bigint)0);
        m_pPOPHandler->OnSave2CloudDriveResponse(pRequestInfo,sResourceId,(EB_STATE_CODE)pResponseInfo->GetResultValue(),this);
        return;
    }
    const mycp::bigint nRequestMsgId = pRequestInfo->m_pRequestList.getParameterValue("request-msg-id", (mycp::bigint)0);
    if (nRequestMsgId>0)
    {
        const EB_MSG_ACK_TYPE nRequestAckType = (EB_MSG_ACK_TYPE)pRequestInfo->m_pRequestList.getParameterValue("request-mack-type", (int)EB_MAT_SUCCESS);
        if (nRequestAckType == EB_MAT_WITHDRAW || nRequestAckType == EB_MAT_SELF_COLLECT || nRequestAckType == EB_MAT_GROUP_COLLECT)
            m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)pResponseInfo->GetResultValue(),nRequestAckType,pRequestInfo,m_pChatInfo,nRequestMsgId,m_sOwnerAccount,this);
        return;
    }
    //const mycp::bigint nWithdrawMsgId = pRequestInfo->m_pRequestList.getParameterValue("withdraw-msg-id", (mycp::bigint)0);
    //if (nWithdrawMsgId>0)
    //{
    //	m_pPOPHandler->OnMsgReceipt((EB_STATE_CODE)pResponseInfo->GetResultValue(),EB_MAT_WITHDRAW,pRequestInfo,m_pChatInfo,nWithdrawMsgId,m_sOwnerAccount,this);
    //	return;
    //}

    const tstring sSaveTo(pRequestInfo->m_pRequestList.getParameterValue("save-to", ""));
    const mycp::bigint sResourceId = pResponseInfo->m_pResponseList.getParameterValue("r-id", (mycp::bigint)0);
    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    const mycp::bigint nSize = pResponseInfo->m_pResponseList.getParameterValue("size", (mycp::bigint)0);
    const tstring sExt(pResponseInfo->m_pResponseList.getParameterValue("ext", ""));
    const tstring sMD5(pResponseInfo->m_pResponseList.getParameterValue("md5", ""));
    const int nPackLen = pResponseInfo->m_pResponseList.getParameterValue("pack-len", (int)0);
    if (nSize==0 || nPackLen==0 || sResourceId==0 || nMsgId==0)
        return;
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!m_pChatInfo->m_pChatUserList.find(m_sOwnerAccount, pFromAccountInfo))
    {
        return;
    }
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        pChatMsgInfo = CChatMsgInfo::create(nMsgId, m_pChatInfo, pFromAccountInfo, EB_MSG_FILE,0);
        m_pChatInfo->m_pChatMsgList.insert(nMsgId, pChatMsgInfo);
    }
    pChatMsgInfo->SetSize(nSize);
    pChatMsgInfo->m_nDataStreamSize = nPackLen;
    pChatMsgInfo->m_sResourceId = sResourceId;
    pChatMsgInfo->m_sMD5String = sMD5;
    pChatMsgInfo->m_nAttachEncoding = pResponseInfo->m_pResponseList.getParameterValue("attach-encoding", (int)0);
    pChatMsgInfo->m_nAttachZipSize = pResponseInfo->m_pResponseList.getParameterValue("attach-zipsize", (mycp::bigint)0);

    //if (!pChatMsgInfo->m_pWaitStreamList.empty())
    //{
    //	if (pChatMsgInfo->m_nAttachEncoding==2 && pChatMsgInfo->m_nAttachZipSize>0)
    //	{

    //	}
    //}else
    if (pChatMsgInfo->GetSize()>0 && pChatMsgInfo->m_nDataStreamSize>0)
    {
        const mycp::bigint nSizeTemp = pChatMsgInfo->m_nAttachZipSize>0?pChatMsgInfo->m_nAttachZipSize:pChatMsgInfo->GetRealSize();
        const int ncount = (int)((nSizeTemp+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);
        for (int i=0; i<ncount; i++)
        {
            pChatMsgInfo->m_pWaitStreamList.insert(i,false,false);
            //const mycp::bigint index = i*pChatMsgInfo->m_nDataStreamSize;
            //pChatMsgInfo->m_pWaitStreamList[index] = false;
        }
    }
    /// 开始接收资源文件
    SendCMReceiveFile(nMsgId,sSaveTo);
}

void CPOPCChatManager::ProcFCMMsg(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo)
{
    //if (m_pChatInfo.get() == NULL) return;
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    const bool bOffChat = pResponseInfo->m_pResponseList.getParameterValue("off-chat", false);
    const tstring sMsgTime = pResponseInfo->m_pResponseList.getParameterValue("msg-time", "");
    const mycp::bigint sResourceId = pResponseInfo->m_pResponseList.getParameterValue("res-id", (mycp::bigint)0);
    const tstring sResCmServer = pResponseInfo->m_pResponseList.getParameterValue("res-cm-server", "");
    const tstring sResCmAppName = pResponseInfo->m_pResponseList.getParameterValue("res-cm-appname", "");
    if (m_pChatInfo->GetChatId() != sChatId)
        return;
    CEBChatInfo::pointer pChatInfo = m_pChatInfo;
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid", (mycp::bigint)0);
    const mycp::bigint sToAccount = pResponseInfo->m_pResponseList.getParameterValue("to_uid", (mycp::bigint)0);
    const int nPrivate = pResponseInfo->m_pResponseList.getParameterValue("private", (int)0);
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pChatInfo->m_pChatUserList.find(sFromAccount, pFromAccountInfo))
    {
        // ?注释这段代码，避免一方异常退出时，重新登录，有时会没有成员信息
        //if (bOffChat)
        {
            pFromAccountInfo = CEBAccountInfo::create(sFromAccount,"");
            pChatInfo->m_pChatUserList.insert(sFromAccount, pFromAccountInfo);
            //}else
            //{
            //	return;
        }
    }
    const mycp::bigint nMsgId = pResponseInfo->m_pResponseList.getParameterValue("msg-id", (mycp::bigint)0);
    const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pResponseInfo->m_pResponseList.getParameterValue("msg-type", (int)EB_MSG_UNKNOWN);
    const int nSubType = pResponseInfo->m_pResponseList.getParameterValue("sub-type", (int)0);
    CChatMsgInfo::pointer pChatMsgInfo;
    if (!pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
    {
        pChatMsgInfo = CChatMsgInfo::create(nMsgId, pChatInfo, pFromAccountInfo, nMsgType,nSubType);
        pChatMsgInfo->m_pOffLineAccount[m_sOwnerAccount] = bOffChat;
        //pChatMsgInfo->m_bOffLineMsg = bOffChat;
        pChatMsgInfo->m_sMsgTime = sMsgTime;
        pChatMsgInfo->m_nToUserId = sToAccount;
        pChatMsgInfo->m_nPrivate = nPrivate;
        pChatMsgInfo->m_sResourceId = sResourceId;
        pChatMsgInfo->m_sResCmServer = sResCmServer;
        pChatMsgInfo->m_sResCmAppName = sResCmAppName;
        pChatInfo->m_pChatMsgList.insert(nMsgId, pChatMsgInfo);
    }
    const mycp::bigint nSize = pResponseInfo->m_pResponseList.getParameterValue("size", (mycp::bigint)0);
    const size_t nPackLen = pResponseInfo->m_pResponseList.getParameterValue("pack-len", (int)0);
    const tstring sContent(pResponseInfo->m_pResponseList.getParameterValue("content", ""));
    pChatMsgInfo->SetContent(sContent);
    pChatMsgInfo->SetSize(nSize);
    pChatMsgInfo->m_nDataStreamSize = (unsigned int)nPackLen;
    //pChatMsgInfo->m_nAttachEncoding = pResponseInfo->m_pResponseList.getParameterValue("attach-encoding", (int)0);
    //pChatMsgInfo->m_nAttachZipSize = pResponseInfo->m_pResponseList.getParameterValue("attach-zipsize", (mycp::bigint)0);

    // ???兼容旧版本，旧版本，不会发送齐全的size和pack-len，所以需要判断
    //if (pChatMsgInfo->GetSize()>0 && pChatMsgInfo->m_nDataStreamSize>0)
    if (pChatMsgInfo->m_tBeginTranTime==0)	// ** 避免添加多次
    {
        const mycp::bigint nSizeTemp = pChatMsgInfo->m_nAttachZipSize>0?pChatMsgInfo->m_nAttachZipSize:pChatMsgInfo->GetRealSize();
        const int ncount = (int)((nSizeTemp+pChatMsgInfo->m_nDataStreamSize-1)/pChatMsgInfo->m_nDataStreamSize);
        for (int i=0; i<ncount; i++)
        {
            pChatMsgInfo->m_pWaitStreamList.insert(i,false,false);
            //const mycp::bigint index = i*pChatMsgInfo->m_nDataStreamSize;
            //pChatMsgInfo->m_pWaitStreamList[index] = false;
        }
    }
    if (nMsgType == EB_MSG_RICH)
    {
        // 这里设置，实现在1S后，如果网络问题，没有转发第一个数据包，客户端可以发起请求
        pChatMsgInfo->m_tRecvTime = timeGetTime()+1000;
    }else if (nMsgType == EB_MSG_FILE)
    {
        // ?? 通知接收文件
        m_pPOPHandler->OnFCMMsg(pChatMsgInfo,NullPOPSotpRequestInfo, pResponseInfo,this);
    }
}

void CPOPCChatManager::DoResponse(void)
{
    while (!m_bKilled)
    {
        CPOPSotpResponseInfo::pointer pResponseInfo;
        if (!m_pResponseList.front(pResponseInfo))
        {
#ifdef WIN32
            Sleep(3);
#else
            usleep(5000);
#endif
            continue;
        }

        CPOPSotpRequestInfo::pointer pRequestInfo;
        //m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo, true);
        if (m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo))
        {
            if (pRequestInfo->GetResponseTime()==0)
            {
                m_pReqResponseList.add(pRequestInfo);
            }
            pRequestInfo->SetResponseTime();
        }
        //const int nResultValue = pResponseInfo->GetResultValue();
        switch (pResponseInfo->GetCallSign())
        {
        case EB_SIGN_FCM_NOTIFY:
            m_pPOPHandler->OnFCMNotify(pRequestInfo, pResponseInfo,this);
            //ProcFCMNotify(pRequestInfo, pResponseInfo);
            break;
            //case EB_SIGN_CR_SET:
            //	{
            //		ProcCRSet(pRequestInfo, pResponseInfo);
            //	}break;
        case EB_SIGN_P2P_RESPONSE:
        {
            m_pPOPHandler->OnCMP2PResponseRes(pRequestInfo, pResponseInfo,this);
        }break;
        case EB_SIGN_FU_MSG:
            ProcFUMsg(pRequestInfo, pResponseInfo);
            break;
        case EB_SIGN_DS_ACK:
        {
            ProcDSAck(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_FCM_MACK:
        {
            // 开始传输文件
            ProcFCMAck(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_CM_MACK:
        {
            ProcCMMAck(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_FDS_SEND:
        {
            ProcFDSSend(pRequestInfo, pResponseInfo,false);
        }break;
        case EB_SIGN_FCM_MSG:
        {
            ProcFCMMsg(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_FP2P_REQUEST:
            m_pPOPHandler->OnFCMP2PRequest(pRequestInfo, pResponseInfo,this);
            break;
        case EB_SIGN_FP2P_RESPONSE:
            m_pPOPHandler->OnFCMP2PResponse(pRequestInfo, pResponseInfo,this);
            break;
        case EB_SIGN_DS_SEND:
        {
            ProcDSSend(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_DS_CHECK:
        {
            ProcDSCheck(pRequestInfo, pResponseInfo);
        }break;
        case EB_SIGN_CM_MSG:
        {
            ProcCMMsg(pRequestInfo,pResponseInfo);
        }break;
        case EB_SIGN_FCM_EXIT:
        {
            ProcFCMExit(pRequestInfo,pResponseInfo);
        }break;
        case EB_SIGN_CM_EXIT:
        {
            if (pRequestInfo.get() != NULL)
            {
                //if (m_pChatInfo.get() != NULL)
                {
                    pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from", m_sOwnerAccount));
                    pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
                    m_pPOPHandler->OnCMExitResponse(pRequestInfo, pResponseInfo,this);
                }
                m_pChatInfo->reset();
                m_bEnterOk = false;
                m_sOwnerAccount = 0;
                m_nLogonType = 0;
            }
        }break;
        case EB_SIGN_FCM_ENTER:
            m_pPOPHandler->OnFCMEnterResponse(pRequestInfo, pResponseInfo,this);
            break;
        case EB_SIGN_CM_ENTER:
            m_pPOPHandler->OnCMEnterResponse(pRequestInfo, pResponseInfo,this);
            break;
        default:
            break;
        }
    }
}

int CPOPCChatManager::SendDSSend(mycp::bigint nMsgId, const cgcAttachment::pointer& pAttach, bool bDisableZip, int nAttachEncoding, mycp::bigint nZipSize)
{
    if (!IsClientStarted()) return -1;
    if (pAttach.get() == NULL) return 1;
    //if (m_pChatInfo.get() == NULL) return -1;
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
    if (pAttach->getIndex()==0 && nAttachEncoding!=0)
    {
        sotp()->doAddParameter(CGC_PARAMETER("attach-encoding", nAttachEncoding));
    }
    if (nZipSize>0)
    {
        sotp()->doAddParameter(CGC_PARAMETER("attach-zipsize", nZipSize));
    }
    return sotp()->doSendAppCall2(0,EB_SIGN_DS_SEND,EB_CALL_NAME_DS_SEND,false,pAttach,bDisableZip);
    //return sotp()->doSendAppCall(EB_SIGN_DS_SEND,EB_CALL_NAME_DS_SEND,false,pAttach);
}
int CPOPCChatManager::SendDSCheck(mycp::bigint nMsgId,bool bNeedAck,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    //if (m_nP2PTryOwnerAccount>0) return 1;	// P2P由对方请求ack，这边不主动发送check
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return -1;
    if (pRequestInfo.get() == NULL)
    {
        // 传输文件
        pRequestInfo = CPOPSotpRequestInfo::create(0);
        //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-from", sFromAccount));
        //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", sChatId));
        pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-id", nMsgId));
        //return 0;
    }else
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
    }
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DS_CHECK, EB_CALL_NAME_DS_CHECK, bNeedAck))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return -2;
}
int CPOPCChatManager::SendDSAck(mycp::bigint nMsgId, const tstring& sIS, bool bNeedAck)
{
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return -1;
    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", m_pChatInfo->GetChatId()));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-id", nMsgId));
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
    sotp()->doAddParameter(CGC_PARAMETER("is", sIS),false);
    sotp()->doAddParameter(CGC_PARAMETER("ack-type", (int)EB_DSAT_REQUEST));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DS_ACK, EB_CALL_NAME_DS_ACK, bNeedAck))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return 1;
}
int CPOPCChatManager::SendDSAck(mycp::bigint nMsgId)
{
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return -1;
    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", m_pChatInfo->GetChatId()));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-id", nMsgId));
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
    sotp()->doAddParameter(CGC_PARAMETER("ack-type", (int)EB_DSAT_OK));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_DS_ACK, EB_CALL_NAME_DS_ACK,true))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return 0;
}

//int CPOPCChatManager::SendCRSet(const tstring & sResourceId,const tstring& sAccount,const tstring& sResourceKey,const tstring& sFilePath,size_t nFileSize)
//{
//	if (!IsClientStarted()) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", sAccount),false);
//	sotp()->doAddParameter(CGC_PARAMETER("r-id", sResourceId),false);
//	sotp()->doAddParameter(CGC_PARAMETER("r-key", sResourceKey),false);
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_CR_SET, EB_CALL_NAME_CR_SET, true, constNullAttchment, &nCallId))
//	{
//		CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(nCallId);
//		pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("r-id", sResourceId));
//		pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from", sAccount));
//		pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("file-path", sFilePath));
//		pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("file-size", (int)nFileSize));
//		m_pRequestList.insert(nCallId, pRequestInfo);
//		return 0;
//	}
//	return -2;
//}

//
//int CPOPCChatManager::SendVRequest(int nVideoType)
//{
//	if (!IsClientStarted()) return -1;
//	if (m_pChatInfo.get() == NULL) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", m_sOwnerAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
//	sotp()->doAddParameter(CGC_PARAMETER("v-type", nVideoType));
//
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_V_REQUEST, EB_CALL_NAME_V_REQUEST, true,constNullAttchment, &nCallId))
//	{
//		CPOPSotpRequestInfo::pointer pRequestInfo;
//		if (pRequestInfo.get() == NULL)
//		{
//			pRequestInfo = CPOPSotpRequestInfo::create(nCallId);
//			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("v-type", nVideoType));
//		}else
//		{
//			pRequestInfo->SetResponseTime(0);
//			pRequestInfo->SetRequestTime();
//			pRequestInfo->SetCallId(nCallId);
//		}
//		m_pRequestList.insert(nCallId, pRequestInfo);
//		return 0;
//	}
//	return 1;
//}
//int CPOPCChatManager::SendVAck(bool bAccept)
//{
//	if (!IsClientStarted()) return -1;
//	if (m_pChatInfo.get() == NULL) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", m_sOwnerAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
//	int nAckType = bAccept?1:2;
//	sotp()->doAddParameter(CGC_PARAMETER("ack-type", nAckType));
//
//	unsigned long nCallId = 0;
//	if (sotp()->doSendAppCall(EB_SIGN_V_ACK, EB_CALL_NAME_V_ACK, true,constNullAttchment, &nCallId))
//	{
//		CPOPSotpRequestInfo::pointer pRequestInfo;
//		if (pRequestInfo.get() == NULL)
//		{
//			pRequestInfo = CPOPSotpRequestInfo::create(nCallId);
//			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ack-type", nAckType));
//		}else
//		{
//			pRequestInfo->SetResponseTime(0);
//			pRequestInfo->SetRequestTime();
//			pRequestInfo->SetCallId(nCallId);
//		}
//		m_pRequestList.insert(nCallId, pRequestInfo);
//		return 0;
//	}
//	return 1;
//}
//int CPOPCChatManager::SendVEnd(void)
//{
//	if (!IsClientStarted()) return -1;
//	if (m_pChatInfo.get() == NULL) return -1;
//	sotp()->doBeginCallLock();
//	sotp()->doAddParameter(CGC_PARAMETER("from", m_sOwnerAccount));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
//	return sotp()->doSendAppCall(EB_SIGN_V_END, EB_CALL_NAME_V_END);
//}

void CPOPCChatManager::SetDestAddress(int nUserIndex,const std::string& sToAddress)
{
    sotp()->doSetConfig(SOTP_CLIENT_CONFIG_CURRENT_INDEX,nUserIndex);
    sotp()->doSetRemoteAddr(sToAddress);
    if (m_nP2PTryOwnerAccount>0)
        SendP2PTest(3);
}
#define DEFAULT_BUFFER_SIZE 128	// 128
int CPOPCChatManager::SendP2PRequest(cr::bigint nFromUid,cr::bigint sChatId,cr::bigint nToAccount,const tstring& sLocapIp,int nRequestData)
{
    m_pChatInfo->SetChatId(sChatId);
    m_pChatInfo->m_bAccountChat = (bool)(nFromUid==sChatId);
    if (!m_pChatInfo->m_pChatUserList.exist(nFromUid))
    {
        m_pChatInfo->m_pChatUserList.insert(nFromUid, CEBAccountInfo::create(nFromUid,""));
    }
    if (!m_pChatInfo->m_pChatUserList.exist(nToAccount))
    {
        m_pChatInfo->m_pChatUserList.insert(nToAccount, CEBAccountInfo::create(nToAccount,""));
    }
    if (m_nP2PTryOwnerAccount==0)
    {
        m_nP2PTryOwnerAccount = nFromUid;
        //sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,EB_MAX_HEART_BEAT_SECOND);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,24*1024);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
    }
    if (m_sOwnerAccount==0)
        m_sOwnerAccount = m_nP2PTryOwnerAccount;

    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUid));
    sotp()->doAddParameter(CGC_PARAMETER("to-uid", nToAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
    sotp()->doAddParameter(CGC_PARAMETER("local-ip", sLocapIp),false);
    sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nRequestData));
    if (sotp()->doSendAppCall(EB_SIGN_P2P_REQUEST, EB_CALL_NAME_P2P_REQ,true))
    {
        //if (pRequestInfo.get() != NULL)
        //{
		//	pRequestInfo->SetResponseTime(0);
        //	pRequestInfo->SetRequestTime();
        //	pRequestInfo->SetCallId(nCallId);
        //	m_pRequestList.insert(nCallId, pRequestInfo);
        //}
        return 0;
    }
    return -2;
}
int CPOPCChatManager::SendP2PResponse(cr::bigint nFromUid,cr::bigint sChatId,cr::bigint nToAccount,const tstring& sLocapIp,int nResponseData,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    m_pChatInfo->SetChatId(sChatId);
    m_pChatInfo->m_bAccountChat = (bool)(nFromUid==sChatId);
    if (!m_pChatInfo->m_pChatUserList.exist(nFromUid))
    {
        m_pChatInfo->m_pChatUserList.insert(nFromUid, CEBAccountInfo::create(nFromUid,""));
    }
    if (!m_pChatInfo->m_pChatUserList.exist(nToAccount))
    {
        m_pChatInfo->m_pChatUserList.insert(nToAccount, CEBAccountInfo::create(nToAccount,""));
    }
    if (m_nP2PTryOwnerAccount==0)
    {
        m_nP2PTryOwnerAccount = nFromUid;
        //sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND,EB_MAX_HEART_BEAT_SECOND);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,24*1024);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
    }
    if (m_sOwnerAccount==0)
        m_sOwnerAccount = m_nP2PTryOwnerAccount;

    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from-uid", nFromUid));
    sotp()->doAddParameter(CGC_PARAMETER("to-uid", nToAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
    sotp()->doAddParameter(CGC_PARAMETER("local-ip", sLocapIp),false);
    sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nResponseData));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    if (pRequestInfo.get() != NULL)
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
        pRequestInfo->SetCallId(nCallId);
        m_pRequestList.insert(nCallId, pRequestInfo);
    }
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_P2P_RESPONSE, EB_CALL_NAME_P2P_RES,true))
    {
        return 0;
    }
    if (pRequestInfo.get() != NULL)
        m_pRequestList.remove(nCallId);
    return -2;
}

int CPOPCChatManager::SendP2PTry(cr::bigint sCallId,cr::bigint sChatId,cr::bigint nFromAccount,int nTryCount,int nP2PData)
{
    m_pChatInfo->SetChatId(sChatId);
    m_pChatInfo->SetCallId(sCallId);
    if (m_nP2PTryOwnerAccount==0)
    {
        m_nP2PTryOwnerAccount = nFromAccount;
        //sotp()->doStartActiveThread(EB_MAX_HEART_BEAT_SECOND/2,EB_MAX_HEART_BEAT_SECOND/2);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_MAX_RECEIVE_BUFFER_SIZE,24*1024);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_SEND_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
        sotp()->doSetConfig(SOTP_CLIENT_CONFIG_IO_RECEIVE_BUFFER_SIZE,DEFAULT_BUFFER_SIZE);
    }
    if (m_sOwnerAccount==0)
        m_sOwnerAccount = m_nP2PTryOwnerAccount;
    m_nP2PData = nP2PData;

    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from-uid", m_nP2PTryOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
    sotp()->doAddParameter(CGC_PARAMETER("try-count", nTryCount));
    sotp()->doAddParameter(CGC_PARAMETER("p2p-data", nP2PData));
    if (sotp()->doSendAppCall(EB_SIGN_P2P_TRY, EB_CALL_NAME_P2P_TRY,true))
    {
        //if (pRequestInfo.get() != NULL)
        //{
		//	pRequestInfo->SetResponseTime(0);
        //	pRequestInfo->SetRequestTime();
        //	pRequestInfo->SetCallId(nCallId);
        //	m_pRequestList.insert(nCallId, pRequestInfo);
        //}
        return 0;
    }
    return -2;
}
void CPOPCChatManager::SendP2PTest(unsigned short nTestCount)
{
    sotp()->doSendP2PTry(nTestCount);
}

int CPOPCChatManager::SendCMReceiveResource(mycp::bigint sResourceId, const tstring & sSaveTo)
{
    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("save-to", sSaveTo));
    return SendCMAck(0,sResourceId,EB_MAT_REQUEST,pRequestInfo);
}
int CPOPCChatManager::SendCMReceiveFile(mycp::bigint nMsgId, const tstring & sSaveTo)
{
    //if (m_pChatInfo.get() == NULL) return -1;
    CChatMsgInfo::pointer pChatMsgInfo = GetChatMsgInfo(nMsgId, false);
    if (pChatMsgInfo.get() == NULL)
    {
        return -1;
    }
#ifdef _QT_MAKE_
    if (!pChatMsgInfo->m_sFileName.isEmpty()) {
#else
    if (!pChatMsgInfo->m_sFileName.empty()) {
#endif
        /// 已经接收
        return 1;
    }
    pChatMsgInfo->m_sFileName = sSaveTo;
    if (m_pPOPHandler != NULL && pChatMsgInfo->m_sResourceId>0)
        m_pPOPHandler->OnReceivingRes(NullPOPSotpRequestInfo,pChatMsgInfo,this);
    // ?这里设置，实现在2S后，如果网络问题，没有转发第一个数据包，客户端可以发起请求
    const int nwaitms = (int)m_pChatInfo->m_pChatMsgList.size()*1000;	// 限制多个文档，同时请求下载；
    pChatMsgInfo->m_tRecvTime = timeGetTime()+1500+nwaitms;
    return SendCMAck(nMsgId, 0, EB_MAT_REQUEST);
}
int CPOPCChatManager::SendCMSave2CloudDrive(mycp::bigint nMsgId)
{
    CChatMsgInfo::pointer pChatMsgInfo = GetChatMsgInfo(nMsgId, false);
    if (pChatMsgInfo.get() == NULL)
    {
        return -1;
    }
    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("save-2-cloud", 1));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("msg-id", nMsgId));
    return SendCMAck(nMsgId,0, EB_MAT_SAVE2CLOUDDRIVE,pRequestInfo);
}

int CPOPCChatManager::SendCMAck(mycp::bigint nMsgId,mycp::bigint sResourceId,EB_MSG_ACK_TYPE ackType,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return -1;
    const mycp::bigint sChatId(m_pChatInfo->GetChatId());
    if (ackType == EB_MAT_CANCEL ||
            ackType == EB_MAT_ERROR ||
            ackType == EB_MAT_WITHDRAW)
    {
        // kill send file thread
        DelSendFileThread(nMsgId);

        CChatMsgInfo::pointer pChatMsgInfo;
        if (m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo, true))
        {
            boost::mutex::scoped_lock lockFile(pChatMsgInfo->m_mutexFile);
            FILE * pfile = pChatMsgInfo->m_pFileHandle;
            pChatMsgInfo->m_pFileHandle = NULL;
            if (pfile)
            {
                fclose(pfile);
            }
            lockFile.unlock();

            if (m_pPOPHandler!=NULL && pChatMsgInfo->GetMsgType()==EB_MSG_FILE)
                m_pPOPHandler->OnCancelFile(EB_STATE_OK,NullPOPSotpRequestInfo,pChatMsgInfo, 0,this);
            m_pFilePercent.remove(nMsgId);
        }
    }
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
    sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
    if (sResourceId >0)
        sotp()->doAddParameter(CGC_PARAMETER("r-id", sResourceId));
    //sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)ackType));
    sotp()->doAddParameter(CGC_PARAMETER("ack-type", (int)ackType));
    sotp()->doAddParameter(CGC_PARAMETER("accept-encoding", (int)2));	// *2=deflate
    const unsigned long nCallId = sotp()->doGetNextCallId();
    if (ackType == EB_MAT_WITHDRAW || ackType == EB_MAT_SELF_COLLECT || ackType == EB_MAT_GROUP_COLLECT)
    {
        if (pRequestInfo.get()==NULL)
            pRequestInfo = CPOPSotpRequestInfo::create(nCallId);
        pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("request-mack-type", (int)ackType));
        pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("request-msg-id", nMsgId));
        //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("withdraw-msg-id", nMsgId));
    }
    if (pRequestInfo.get() != NULL)
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
        pRequestInfo->SetCallId(nCallId);
        m_pRequestList.insert(nCallId, pRequestInfo);
    }
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_MACK, EB_CALL_NAME_CM_MACK,true))
    {
        return 0;
    }
    if (pRequestInfo.get() != NULL)
        m_pRequestList.remove(nCallId);
    return -2;

}

bool CPOPCChatManager::IsMsgSender(const CChatMsgInfo::pointer & pChatMsgInfo) const
{
    if (pChatMsgInfo.get() == NULL || pChatMsgInfo->GetFromAccount().get() == NULL)
        return false;
    CEBAccountInfo::pointer pMsgFromAccount = CGC_OBJECT_CAST<CEBAccountInfo>(pChatMsgInfo->GetFromAccount());
    return pMsgFromAccount->GetUserId() == m_sOwnerAccount;
}
mycp::bigint CPOPCChatManager::GetMsgSender(const CChatMsgInfo::pointer & pChatMsgInfo) const
{
    if (pChatMsgInfo.get() == NULL || pChatMsgInfo->GetFromAccount().get() == NULL)
        return 0;
    CEBAccountInfo::pointer pMsgFromAccount = CGC_OBJECT_CAST<CEBAccountInfo>(pChatMsgInfo->GetFromAccount());
    return pMsgFromAccount->GetUserId();
}

bool CPOPCChatManager::DelSendFileThread(mycp::bigint nMsgId)
{
    CSendFileThread::pointer pSendFileThread;
    if (m_pSendFileThreadList.find(nMsgId, pSendFileThread, true))
    {
        pSendFileThread->KillThread();
        return true;
    }
    return false;
}

void CPOPCChatManager::OnActiveTimeout(void)
{
    // -200：timeout
    m_nActiveTimeoutCount++;
    if (m_nActiveTimeoutCount >= 2)
    {
        m_pPOPHandler->OnInvalidateSession(-200,this);
        m_nActiveTimeoutCount = 0;
    }
}

void CPOPCChatManager::OnCgcResponse(const cgcParserSotp & response)
{
    const long nSotpCallId = response.getCallid();
    //const long nSotpSign = response.getSign();
    const long nResultValue = response.getResultValue();
    // -102: 错误模块代码
    if (nResultValue < 0)
        //if (nResultValue == -102 || nResultValue == -103)
    {
        m_pPOPHandler->OnInvalidateSession(nResultValue,this);
        return;
    }
    m_nActiveTimeoutCount = 0;
    if (m_nOnP2POk==0 && m_pChatInfo.get()!=NULL && response.isResulted() && response.isOpenType())
    {
        for (int i=0;i<3;i++)
        {
            // 发送多次，确保对方能收到，保证二边互通；
            SendP2PTry(m_pChatInfo->GetCallId(),m_pChatInfo->GetChatId(),0,5,m_nP2PData);
#ifdef WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
        }
    }

    CPOPSotpRequestInfo::pointer pRequestInfo;
    //m_pRequestList.find(pResponseInfo->GetCallId(), pRequestInfo, true);
    if (m_pRequestList.find(nSotpCallId, pRequestInfo))
    {
        if (pRequestInfo->GetResponseTime()==0)
        {
            m_pReqResponseList.add(pRequestInfo);
        }
        pRequestInfo->SetResponseTime();
    }
    switch (response.getSign())
    {
    case EB_SIGN_FCM_NOTIFY:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("notify-type", response.getRecvParameterValue("notify-type", (int)EB_NOTIFY_UNKNOWN)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("notify-id", response.getRecvParameterValue("notify-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("notify-data", response.getRecvParameterValue("notify-data", "")));
        if (m_pResponseThread.get()==NULL)
            m_pPOPHandler->OnFCMNotify(pRequestInfo, pResponseInfo,this);
        //ProcFCMNotify(pRequestInfo, pResponseInfo);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_P2P_RESPONSE:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        if (m_pResponseThread.get()==NULL)
            m_pPOPHandler->OnCMP2PResponseRes(pRequestInfo, pResponseInfo,this);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_FU_MSG:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from", response.getRecvParameterValue("from", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-content", response.getRecvParameterValue("msg-content", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("dep-code", response.getRecvParameterValue("dep-code", (mycp::bigint)0)));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("f-info", response.getRecvParameterValue("f-info", "")));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("emo-count", response.getRecvParameterValue("emo-count", (int)-1)));
        //m_pPOPHandler->OnFUMIUMsg(pRequestInfo, pResponseInfo, this);
        //m_pResponseList.add(pResponseInfo);
        if (m_pResponseThread.get()==NULL)
            ProcFUMsg(pRequestInfo, pResponseInfo);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
        //case EB_SIGN_CR_SET:
    case EB_SIGN_DS_ACK:
    {
        if (m_nP2PTryOwnerAccount!=0 && response.getFunctionName()==EB_CALL_NAME_DS_ACK)
        {
            // 这是P2P消息，对方请求数据补偿
            const mycp::bigint nMsgId = response.getRecvParameterValue("msg-id", (mycp::bigint)0);
            const EB_DATASTREAM_ACK_TYPE nAckType = (EB_DATASTREAM_ACK_TYPE)response.getRecvParameterValue("ack-type", (int)EB_DSAT_UNKNOWN);
            CChatMsgInfo::pointer pChatMsgInfo;
            if (!m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
            {
                sotp()->doSendCallResult(EB_STATE_MSG_NOT_EXIST,response.getCallid(),response.getSign());
                break;
            }
            if (nAckType == EB_DSAT_REQUEST)
            {
                // 需要补偿数据
                CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), EB_STATE_NEED_RESEND);
                pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", nMsgId));
                pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("is", response.getRecvParameterValue("is", "")));
                if (pRequestInfo.get()==NULL)
                    pRequestInfo = CPOPSotpRequestInfo::create(response.getCallid());
                pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-id", nMsgId));
                ProcDSCheck(pRequestInfo, pResponseInfo);
            }
        }else
        {
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            ProcDSAck(pRequestInfo, pResponseInfo);
        }
    }break;
    case EB_SIGN_FCM_MACK:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", response.getRecvParameterValue("ack-type", (int)EB_MAT_SUCCESS)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("accept-encoding", response.getRecvParameterValue("accept-encoding", (int)0)));
        if (m_pResponseThread.get()==NULL)
            ProcFCMAck(pRequestInfo, pResponseInfo);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_CM_MACK:
    {
        if (m_nP2PTryOwnerAccount!=0 && response.getFunctionName()==EB_CALL_NAME_CM_MACK)
        {
            // 这是P2P消息，对方响应消息；
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", response.getRecvParameterValue("ack-type", (int)EB_MAT_SUCCESS)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("accept-encoding", response.getRecvParameterValue("accept-encoding", (int)0)));
            ProcFCMAck(pRequestInfo, pResponseInfo);

            //// 响应；
            //sotp()->doSendCallResult(EB_STATE_OK,response.getCallid(),response.getSign());
        }else
        {
            // resourceid有效
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("r-id", response.getRecvParameterValue("r-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("size", response.getRecvParameterValue("size", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ext", response.getRecvParameterValue("ext", "")));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("md5", response.getRecvParameterValue("md5", "")));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("pack-len", response.getRecvParameterValue("pack-len", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-encoding", response.getRecvParameterValue("attach-encoding", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-zipsize", response.getRecvParameterValue("attach-zipsize", (mycp::bigint)0)));
            ProcCMMAck(pRequestInfo, pResponseInfo);
            //m_pResponseList.add(pResponseInfo);
        }
    }break;
    case EB_SIGN_DS_CHECK:
    {
        if (m_nP2PTryOwnerAccount!=0 && response.getFunctionName()==EB_CALL_NAME_DS_CHECK)
        {
            // 这是对方发送的P2P消息；
            //const mycp::bigint sChatId = response.getRecvParameterValue("chat-id", (mycp::bigint)0);
            const mycp::bigint nMsgId = response.getRecvParameterValue("msg-id", (mycp::bigint)0);
            CChatMsgInfo::pointer pChatMsgInfo;
            if (!m_pChatInfo->m_pChatMsgList.find(nMsgId, pChatMsgInfo))
            {
                //sotp()->doBeginCallLock();
                //sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
                sotp()->doSendCallResult(EB_STATE_OK,response.getCallid(),response.getSign());
                break;
            }else if (pChatMsgInfo->m_pWaitStreamList.empty())
            {
                // 传输完成；
                //pChatMsgInfo->m_bTranOk = true;
                //sotp()->doBeginCallLock();
                //sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
                sotp()->doSendCallResult(EB_STATE_OK,response.getCallid(),response.getSign());
                break;
            }

            //if (pChatMsgInfo->m_tRecvTime>0 && (pChatMsgInfo->m_tRecvTime+60*1000) < timeGetTime())
            //{
            //	// 超过1分钟没有收到新数据，当超时处理
            //	//m_pChatInfo->m_pChatMsgList.remove(nMsgId);
            //	//sotp()->doBeginCallLock();
            //	//sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
            //	sotp()->doSendCallResult(EB_STATE_TIMEOUT_ERROR,response.getCallid(),response.getSign());
            //	break;
            //}

            // 本端会主动请求ds_ack，这里是为了预防本端丢包，对方也会主动请求检查；
            tstring sIS;
            {
                int oscount = 0;
                char lpszParamName[14];
                BoostReadLock rdlock(pChatMsgInfo->m_pWaitStreamList.mutex());
                CLockMap<int, bool>::iterator pIter = pChatMsgInfo->m_pWaitStreamList.begin();
                for (; pIter!=pChatMsgInfo->m_pWaitStreamList.end(); pIter++)
                {
                    const int nIndex = pIter->first;
                    sprintf(lpszParamName, "%d,", nIndex);
                    //mycp::bigint index = pIter->first;
                    //sprintf(lpszParamName, "%d,", (int)(index/pChatMsgInfo->m_nDataStreamSize));
                    sIS.append(lpszParamName);
                    if ((oscount++) == EB_MAX_REQUEST_OS_COUNT)	// P2P
                        break;
                }
            }
            if (!sIS.empty())
            {
                sotp()->doBeginCallLock();
                //sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
                sotp()->doAddParameter(CGC_PARAMETER("is", sIS));
                sotp()->doSendCallResult(EB_STATE_NEED_RESEND,response.getCallid(),response.getSign(),false);	// 不需要NeedAck
            }
            break;
        }else
        {
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            if (nResultValue == EB_STATE_NEED_RESEND)
            {
                // 需要补偿数据
                pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("is", response.getRecvParameterValue("is", "")));
            }
            ProcDSCheck(pRequestInfo, pResponseInfo);
        }
    }break;
    case EB_SIGN_FDS_SEND:
    {
        const cgcAttachment::pointer pAttachMent = response.getRecvAttachment();
        if (pAttachMent.get() == NULL)
            break;
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
        if (pAttachMent->getIndex()==0)
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-encoding", response.getRecvParameterValue("attach-encoding", (int)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-zipsize", response.getRecvParameterValue("attach-zipsize", (mycp::bigint)0)));

        pResponseInfo->m_pAttachMent = pAttachMent;
        //pResponseInfo->m_pAttachMent = cgcAttachment::create();
        //pResponseInfo->m_pAttachMent->setName(pAttachMent->getName());
        //pResponseInfo->m_pAttachMent->setTotal(pAttachMent->getTotal());
        //pResponseInfo->m_pAttachMent->setIndex(pAttachMent->getIndex());
        //pResponseInfo->m_pAttachMent->setAttach(pAttachMent->getAttachData(), pAttachMent->getAttachSize());
        if (m_pResponseThread.get()==NULL)
            ProcFDSSend(pRequestInfo, pResponseInfo,false);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_DS_SEND:
    {
        if (m_nP2PTryOwnerAccount!=0 && response.getFunctionName()==EB_CALL_NAME_DS_SEND)
        {
            // 这是对方发送的P2P消息；
            const cgcAttachment::pointer pAttachMent = response.getRecvAttachment();
            if (pAttachMent.get() == NULL)
                break;
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            pResponseInfo->m_pAttachMent = pAttachMent;
            if (pAttachMent->getIndex()==0)
                pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-encoding", response.getRecvParameterValue("attach-encoding", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-zipsize", response.getRecvParameterValue("attach-zipsize", (mycp::bigint)0)));
            //pResponseInfo->m_pAttachMent = cgcAttachment::create();
            //pResponseInfo->m_pAttachMent->setName(pAttachMent->getName());
            //pResponseInfo->m_pAttachMent->setTotal(pAttachMent->getTotal());
            //pResponseInfo->m_pAttachMent->setIndex(pAttachMent->getIndex());
            //pResponseInfo->m_pAttachMent->setAttach(pAttachMent->getAttachData(), pAttachMent->getAttachSize());
            ProcFDSSend(pRequestInfo, pResponseInfo,true);
        }else if (nResultValue == EB_STATE_OK)
        {
            // 这是服务端返回事件；
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("percent", response.getRecvParameterValue("percent", 0.0)));
            ProcDSSend(pRequestInfo, pResponseInfo);
            //m_pResponseList.add(pResponseInfo);
        }
    }break;
    case EB_SIGN_FCM_MSG:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("to_uid", response.getRecvParameterValue("to_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("private", response.getRecvParameterValue("private", (int)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-type", response.getRecvParameterValue("sub-type", (int)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("content", response.getRecvParameterValue("content", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("size", response.getRecvParameterValue("size", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("pack-len", response.getRecvParameterValue("pack-len", (int)0)));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("attach-zipsize", response.getRecvParameterValue("attach-zipsize", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("off-chat", response.getRecvParameterValue("off-chat", false)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-time", response.getRecvParameterValue("msg-time", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-id", response.getRecvParameterValue("res-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-cm-server", response.getRecvParameterValue("res-cm-server", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-cm-appname", response.getRecvParameterValue("res-cm-appname", "")));
        if (m_pResponseThread.get()==NULL)
            ProcFCMMsg(pRequestInfo, pResponseInfo);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_P2P_TRY:
    {
        // 收到对方P2P请求
        if (response.getFunctionName()==EB_CALL_NAME_P2P_TRY)
        {
            //MessageBox(NULL,"EB_SIGN_P2P_TRY","EB_SIGN_P2P_TRY",MB_OK);
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("try-count", response.getRecvParameterValue("try-count", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
            ProcP2PTry(pRequestInfo,pResponseInfo);
        }
    }break;
    case EB_SIGN_FP2P_REQUEST:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("local-ip", response.getRecvParameterValue("local-ip", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("remote-ip", response.getRecvParameterValue("remote-ip", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
        if (m_pResponseThread.get()==NULL)
            m_pPOPHandler->OnFCMP2PRequest(pRequestInfo, pResponseInfo,this);
        else
            m_pResponseList.add(pResponseInfo);
        //ProcFCMP2P(pRequestInfo,pResponseInfo);
    }break;
    case EB_SIGN_FP2P_RESPONSE:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from-uid", response.getRecvParameterValue("from-uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("local-ip", response.getRecvParameterValue("local-ip", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("remote-ip", response.getRecvParameterValue("remote-ip", "")));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("p2p-data", response.getRecvParameterValue("p2p-data", (int)0)));
        if (m_pResponseThread.get()==NULL)
            m_pPOPHandler->OnFCMP2PResponse(pRequestInfo, pResponseInfo,this);
        else
            m_pResponseList.add(pResponseInfo);
        //ProcFCMP2P(pRequestInfo,pResponseInfo);
    }break;
    case EB_SIGN_CM_MSG:
    {
        if (m_nP2PTryOwnerAccount!=0 && response.getFunctionName()==EB_CALL_NAME_CM_MSG)
        {
            // 这是对方发送的P2P消息；
            const mycp::bigint nMsgId = this->GetNextBigId();
            //const mycp::bigint nFromUserId = response.getRecvParameterValue("from_uid", (mycp::bigint)0);
            //const mycp::bigint nToUserId = response.getRecvParameterValue("to_uid", (mycp::bigint)0);
            //const int nPrivate = response.getRecvParameterValue("private", (int)0);
            //const mycp::bigint nChatId = response.getRecvParameterValue("chat-id", (mycp::bigint)0);
            //const int nMsgType = response.getRecvParameterValue("msg-type", (int)EB_MSG_RICH);
            //const int nSubType = response.getRecvParameterValue("sub-type", (int)0);
            //const tstring sContent = response.getRecvParameterValue("content", "");
            //const int nSize = response.getRecvParameterValue("size", (int)0);
            //const int sPackLen = response.getRecvParameterValue("pack-len", (int)0);

            //
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("to_uid", response.getRecvParameterValue("to_uid", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("private", response.getRecvParameterValue("private", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", nMsgId));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-type", response.getRecvParameterValue("msg-type", (int)EB_MSG_UNKNOWN)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("sub-type", response.getRecvParameterValue("sub-type", (int)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("content", response.getRecvParameterValue("content", "")));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("size", response.getRecvParameterValue("size", (mycp::bigint)0)));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("pack-len", response.getRecvParameterValue("pack-len", (int)0)));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("off-chat", response.getRecvParameterValue("off-chat", false)));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-time", response.getRecvParameterValue("msg-time", "")));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-id", response.getRecvParameterValue("res-id", (mycp::bigint)0)));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-cm-server", response.getRecvParameterValue("res-cm-server", "")));
            //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("res-cm-appname", response.getRecvParameterValue("res-cm-appname", "")));
            ProcFCMMsg(pRequestInfo, pResponseInfo);

            // 返回给对方P2P事件；
            sotp()->doBeginCallLock();
            sotp()->doAddParameter(CGC_PARAMETER("msg-id", nMsgId));
            //sotp()->doAddParameter(CGC_PARAMETER("accept-encoding", 2));	// 2=deflate（P2P临时不需要支持）
            sotp()->doSendCallResult(EB_STATE_OK,response.getCallid(),response.getSign());	// 响应对方请求；
        }else
        {
            // 服务端返回，或P2P对方返回
            CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("msg-id", response.getRecvParameterValue("msg-id", (mycp::bigint)0)));
            ProcCMMsg(pRequestInfo,pResponseInfo);
            //m_pResponseList.add(pResponseInfo);
        }
    }break;
    case EB_SIGN_FCM_EXIT:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("hangup", response.getRecvParameterValue("hangup", (int)0)));
        //if (m_pResponseThread.get()==NULL)
        ProcFCMExit(pRequestInfo,pResponseInfo);
        //else
        //	m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_CM_EXIT:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        if (pRequestInfo.get() != NULL)
        {
            //if (m_pResponseThread.get()==NULL)
            //{
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
            m_pPOPHandler->OnCMExitResponse(pRequestInfo, pResponseInfo,this);
            m_pChatInfo->reset();
            m_bEnterOk = false;
            m_sOwnerAccount = 0;
            m_nLogonType = 0;
            //}else
            //{
            //	m_pResponseList.add(pResponseInfo);
            //}
        }
    }break;
    case EB_SIGN_FCM_ENTER:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", response.getRecvParameterValue("chat-id", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", response.getRecvParameterValue("from_uid", (mycp::bigint)0)));
        pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("off-line", response.getRecvParameterValue("off-line", (int)0)));
        //if (m_pChatInfo.get() == NULL) break;
        const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from_uid", (mycp::bigint)0);
        const int nOffLine = pResponseInfo->m_pResponseList.getParameterValue("off-line", (int)0);
        const tstring sFromInfo(pResponseInfo->m_pResponseList.getParameterValue("f-info", ""));
        const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
        //const tstring sDepCode = pResponseInfo->m_pResponseList.getParameterValue("dep-code", "");
        if (m_pChatInfo->GetChatId() != sChatId)
            break;
        CEBAccountInfo::pointer pFromAccountInfo;
        if (!m_pChatInfo->m_pChatUserList.find(sFromAccount, pFromAccountInfo))
        {
            pFromAccountInfo = CEBAccountInfo::create(sFromAccount,"");
            pFromAccountInfo->m_sFromInfo = sFromInfo;
            m_pChatInfo->m_pChatUserList.insert(sFromAccount, pFromAccountInfo);
        }
        pFromAccountInfo->SetLineState(nOffLine==1?EB_LINE_STATE_OFFLINE:EB_LINE_STATE_ONLINE_NEW);
        if (m_pResponseThread.get()==NULL)
            m_pPOPHandler->OnFCMEnterResponse(pRequestInfo, pResponseInfo,this);
        else
            m_pResponseList.add(pResponseInfo);
    }break;
    case EB_SIGN_CM_ENTER:
    {
        CPOPSotpResponseInfo::pointer pResponseInfo = CPOPSotpResponseInfo::create(response.getCallid(), response.getSign(), nResultValue);
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from", response.getRecvParameterValue("from", "")));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("call-id", response.getRecvParameterValue("call-id", "")));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("multi", response.getRecvParameterValue("multi", (int)0)));
        //pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("address", response.getRecvParameterValue("address", "")));
        //m_pResponseList.add(pResponseInfo);
        //if (m_pChatInfo.get() == NULL) break;
        if (pRequestInfo.get() != NULL)
        {
            m_bEnterOk = true;
            m_sOwnerAccount = pRequestInfo->m_pRequestList.getParameterValue("cm-from", (mycp::bigint)0);
            m_nLogonType = pRequestInfo->m_pRequestList.getParameterValue("cm-type", (int)0);
            //m_sChatId = pRequestInfo->m_pRequestList.getParameterValue("cm-chat-id", "");
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
            pResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("chat-key", pRequestInfo->m_pRequestList.getParameterValue("cm-chat-key", "")));
            if (m_pResponseThread.get()==NULL)
                m_pPOPHandler->OnCMEnterResponse(pRequestInfo, pResponseInfo,this);
            else
                m_pResponseList.add(pResponseInfo);
        }
    }break;
    default:
        break;
    }
}
eb::bigint CPOPCChatManager::GetNextBigId(void)
{
    static unsigned short static_id_index = 0;
    char lpszNextId[24];
    sprintf(lpszNextId, "%d%04d%04d", 10000000+((int)time(0)%89999999),(++static_id_index)%10000,rand()%10000);
    return cgc_atoi64(lpszNextId);
}
//int CPOPCChatManager::SendCMMsg(mycp::bigint nFrom, EB_RICH_SUB_TYPE subtype, const tstring& sUserData,CPOPSotpRequestInfo::pointer pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	////if (m_pChatInfo.get() == NULL) return -1;
//	//if (pRichMsg->GetMsgSize(false) >= EB_MAX_RICH_FORMAT_SIZE)
//	//{
//	//	m_pPOPHandler->OnSendingRich(EB_STATE_MAX_CAPACITY_ERROR,pRequestInfo,pRichMsg,m_sOwnerAccount,sTo,nPrivate,this);
//	//	return -3;
//	//}
//
//	const EB_MSG_TYPE msgtype = EB_MSG_RICH;
//	if (pRequestInfo.get() == NULL)
//		pRequestInfo = CPOPSotpRequestInfo::create(0);
//	else
//	{
//		pRequestInfo->SetResponseTime(0);
//		pRequestInfo->SetRequestTime();
//	}
//	EB_ChatRoomRichMsg pRichMsg1;
//	pRichMsg1.SetSubType(subtype);
//	pRichMsg1.AddText(sUserData);
//	CEBChatRoomRichMsg::pointer pRichMsg2 = CEBChatRoomRichMsg::create(pRichMsg1);
//	const CPOPChatRoomRichMsg::pointer pChatRoomRichMsg = CPOPChatRoomRichMsg::create(pRichMsg2);
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-type", (int)msgtype));
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-sub-type", (int)subtype));
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-rich-msg", (mycp::cgcObject::pointer)pChatRoomRichMsg));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-to", sTo));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-private", nPrivate));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-size", nSize));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-pack-len", (int)nPackLen));
//
//	sotp()->doBeginCallLock();
//	if (m_pChatInfo.get()!=NULL && m_pChatInfo->m_bAppPushChat && nFrom>0)
//		sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFrom));
//	else
//		sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
//	//if (sTo > 0)
//	//	sotp()->doAddParameter(CGC_PARAMETER("to_uid", sTo));
//	//if (nPrivate == 1)
//	//	sotp()->doAddParameter(CGC_PARAMETER("private", nPrivate));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
//	sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)msgtype));
//	sotp()->doAddParameter(CGC_PARAMETER("sub-type", (int)subtype));
//	sotp()->doAddParameter(CGC_PARAMETER("content", sUserData),false);
//
//	const unsigned long nCallId = sotp()->doGetNextCallId();
//	pRequestInfo->SetCallId(nCallId);
//	m_pRequestList.insert(nCallId, pRequestInfo);
//	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_MSG, EB_CALL_NAME_CM_MSG, true))
//	{
//		return 0;
//	}
//	m_pRequestList.remove(nCallId);
//	return -2;
//}
//int CPOPCChatManager::SendCMMsg(mycp::bigint nFrom, const tstring& sMapPosData,CPOPSotpRequestInfo::pointer pRequestInfo)
//{
//	if (!IsClientStarted()) return -1;
//	////if (m_pChatInfo.get() == NULL) return -1;
//	//if (pRichMsg->GetMsgSize(false) >= EB_MAX_RICH_FORMAT_SIZE)
//	//{
//	//	m_pPOPHandler->OnSendingRich(EB_STATE_MAX_CAPACITY_ERROR,pRequestInfo,pRichMsg,m_sOwnerAccount,sTo,nPrivate,this);
//	//	return -3;
//	//}
//
//	const EB_MSG_TYPE msgtype = EB_MSG_RICH;
//	const EB_RICH_SUB_TYPE subtype = EB_RICH_SUB_TYPE_MAP_POS;
//	if (pRequestInfo.get() == NULL)
//		pRequestInfo = CPOPSotpRequestInfo::create(0);
//	else
//	{
//		pRequestInfo->SetResponseTime(0);
//		pRequestInfo->SetRequestTime();
//	}
//	EB_ChatRoomRichMsg pRichMsg1;
//	pRichMsg1.SetSubType(subtype);
//	pRichMsg1.AddText(sMapPosData);
//	CEBChatRoomRichMsg::pointer pRichMsg2 = CEBChatRoomRichMsg::create(pRichMsg1);
//	const CPOPChatRoomRichMsg::pointer pChatRoomRichMsg = CPOPChatRoomRichMsg::create(pRichMsg2);
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-type", (int)msgtype));
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-sub-type", (int)subtype));
//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-rich-msg", (mycp::cgcObject::pointer)pChatRoomRichMsg));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-to", sTo));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-private", nPrivate));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-size", nSize));
//	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-pack-len", (int)nPackLen));
//
//	sotp()->doBeginCallLock();
//	if (m_pChatInfo.get()!=NULL && m_pChatInfo->m_bAppPushChat && nFrom>0)
//		sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFrom));
//	else
//		sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
//	//if (sTo > 0)
//	//	sotp()->doAddParameter(CGC_PARAMETER("to_uid", sTo));
//	//if (nPrivate == 1)
//	//	sotp()->doAddParameter(CGC_PARAMETER("private", nPrivate));
//	sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
//	sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)msgtype));
//	sotp()->doAddParameter(CGC_PARAMETER("sub-type", (int)subtype));
//	sotp()->doAddParameter(CGC_PARAMETER("content", sMapPosData),false);
//
//	const unsigned long nCallId = sotp()->doGetNextCallId();
//	pRequestInfo->SetCallId(nCallId);
//	m_pRequestList.insert(nCallId, pRequestInfo);
//	if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_MSG, EB_CALL_NAME_CM_MSG, true))
//	{
//		return 0;
//	}
//	m_pRequestList.remove(nCallId);
//	return -2;
//}
int CPOPCChatManager::SendCMMsg(const tstring & sPathName,const tstring & sInFileName,const tstring& sMD5,mycp::bigint nFileSize,unsigned int nPackLen,
                                mycp::bigint sResourceId,mycp::bigint nFrom,mycp::bigint sTo,int nPrivate,int nSendOffFile,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;
    if (m_nP2PTryOwnerAccount>0 && (m_nP2PData&1)==1)
        nPackLen = 16*1024;	// 16ok 内网12K成功，外网12K失败；
    //if (m_pChatInfo.get() == NULL) return -1;
    tstring sFileName(sInFileName);
    if (sFileName.empty() && !sPathName.empty())
    {
        sFileName = sPathName;
#ifdef WIN32
        std::string::size_type find = sPathName.rfind("\\");
#else
        std::string::size_type find = sPathName.rfind("/");
#endif
        if (find == std::string::npos)
        {
#ifdef WIN32
            find = sPathName.rfind("/");
#else
            find = sPathName.rfind("\\");
#endif
        }
        if (find != std::string::npos)
            sFileName = sPathName.substr(find+1);
    }
    const EB_MSG_TYPE msgtype = EB_MSG_FILE;
    if (pRequestInfo.get() == NULL)
        pRequestInfo = CPOPSotpRequestInfo::create(0);
    else
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
    }
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-type", (int)msgtype));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-file-name", sPathName));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-size", nFileSize));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-pack-len", (int)nPackLen));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-to", sTo));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-private", nPrivate));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-off-file", nSendOffFile));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-res-id", sResourceId));
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-from", sFromAccount));
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", sChatId));

    sotp()->doBeginCallLock();
    if (m_pChatInfo.get()!=NULL && m_pChatInfo->m_bAppPushChat && nFrom>0)
        sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFrom));
    else
        sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    if (sTo > 0)
        sotp()->doAddParameter(CGC_PARAMETER("to_uid", sTo));
    if (nPrivate == 1)
        sotp()->doAddParameter(CGC_PARAMETER("private", 1));
    if (nSendOffFile == 1)
        sotp()->doAddParameter(CGC_PARAMETER("off-file", 1));
    if (sResourceId>0)
        sotp()->doAddParameter(CGC_PARAMETER("r-id", sResourceId));
    sotp()->doAddParameter(CGC_PARAMETER("md5", sMD5),false);
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)msgtype));
    sotp()->doAddParameter(CGC_PARAMETER("content", sFileName),false);
    if ((m_nLogonType&(EB_LOGON_TYPE_WEB|EB_LOGON_TYPE_SERVER))!=0)
    {
        sotp()->doAddParameter(CGC_PARAMETER("file-path", sPathName),false);
    }
    sotp()->doAddParameter(CGC_PARAMETER("size", nFileSize));
    sotp()->doAddParameter(CGC_PARAMETER("pack-len", (int)nPackLen));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_MSG, EB_CALL_NAME_CM_MSG, true))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return -2;
}

int CPOPCChatManager::SendCMMsg(const CEBChatRoomRichMsg::pointer& pRichMsg,unsigned int nPackLen,mycp::bigint nFrom,mycp::bigint sTo,int nPrivate,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return -1;
    if (pRichMsg->GetMsgSize(false) >= (unsigned long)EB_MAX_RICH_FORMAT_SIZE)
    {
        m_pPOPHandler->OnSendingRich(EB_STATE_MAX_CAPACITY_ERROR,pRequestInfo,pRichMsg,m_sOwnerAccount,sTo,nPrivate,this);
        return -3;
    }
    const mycp::bigint nSize = pRichMsg->GetMsgSize();
    if (nSize>EB_MAX_RICH_TOTAL_SIZE)
    {
        m_pPOPHandler->OnSendingRich(EB_STATE_MAX_CAPACITY_ERROR,pRequestInfo,pRichMsg,m_sOwnerAccount,sTo,nPrivate,this);
        return -3;
    }else if (nSize==0)
    {
        return -2;
    }
    if (m_nP2PTryOwnerAccount>0 && (m_nP2PData&1)==1)
        nPackLen = 8*1024;	// P2P

    const EB_MSG_TYPE msgtype = EB_MSG_RICH;
    if (pRequestInfo.get() == NULL)
        pRequestInfo = CPOPSotpRequestInfo::create(0);
    else
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
    }
    //pRequestInfo->m_pRichMsg = pRichMsg;
    const CPOPChatRoomRichMsg::pointer pChatRoomRichMsg = CPOPChatRoomRichMsg::create(pRichMsg);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-msg-type", (int)msgtype));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-sub-type", (int)pRichMsg->GetSubType()));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-rich-msg", (mycp::cgcObject::pointer)pChatRoomRichMsg));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-to", sTo));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-private", nPrivate));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-size", nSize));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-pack-len", (int)nPackLen));
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-from", sFromAccount));
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", sChatId));

    sotp()->doBeginCallLock();
    if (m_pChatInfo.get()!=NULL && m_pChatInfo->m_bAppPushChat && nFrom>0)
        sotp()->doAddParameter(CGC_PARAMETER("from_uid", nFrom));
    else
        sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    if (sTo > 0)
        sotp()->doAddParameter(CGC_PARAMETER("to_uid", sTo));
    if (nPrivate == 1)
        sotp()->doAddParameter(CGC_PARAMETER("private", nPrivate));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("msg-type", (int)msgtype));
    sotp()->doAddParameter(CGC_PARAMETER("sub-type", (int)pRichMsg->GetSubType()));
    //if (pRichMsg->GetSubType()>=EB_RICH_SUB_TYPE_MAP_POS)
    //{
    //	sotp()->doAddParameter(CGC_PARAMETER("content", pRichMsg->GetFormatMsg()),false);
    //}else
    {
        sotp()->doAddParameter(CGC_PARAMETER("size", nSize));
        sotp()->doAddParameter(CGC_PARAMETER("pack-len", (int)nPackLen));
    }
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_MSG, EB_CALL_NAME_CM_MSG, true))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return -2;
}

int CPOPCChatManager::SendCMExit(int nExitSes,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;
    //if (m_pChatInfo.get() == NULL) return 1;
    if (m_sOwnerAccount==0) return 1;
    if (pRequestInfo.get() == NULL)
        pRequestInfo = CPOPSotpRequestInfo::create(0);
    else
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
    }
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-from", sFromAccount));
    //pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", sChatId));
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("chat-key", m_pChatInfo->GetChatKey()));
    if (nExitSes==1)
        sotp()->doAddParameter(CGC_PARAMETER("exit-ses", nExitSes));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_EXIT, EB_CALL_NAME_CM_EXIT, true))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return -2;
}
mycp::bigint CPOPCChatManager::GetOwnerAccount(void) const
{
    return m_sOwnerAccount;
}
int CPOPCChatManager::SendCMEnter(mycp::bigint nAppId,mycp::bigint nFromIpAddress,mycp::bigint sFromAccount, int nType, mycp::bigint sDepCode,
                                  mycp::bigint sCallId, mycp::bigint sChatId, const tstring & sChatKey,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;

    //if (m_pChatInfo.get() == NULL)
    //{
    //	m_pChatInfo = CEBChatInfo::create(sChatId, sChatKey);
    //}else
    {
        m_pChatInfo->SetChatId(sChatId);
        m_pChatInfo->SetChatKey(sChatKey);
    }
    m_pChatInfo->SetCallId(sCallId);
    m_pChatInfo->m_sGroupCode = sDepCode;
    m_pChatInfo->m_bAppPushChat = nAppId>0 && nAppId==sChatId && nAppId==sFromAccount;
    m_pChatInfo->m_bAccountChat = (bool)(sFromAccount==sChatId);
    if (!m_pChatInfo->m_pChatUserList.exist(sFromAccount))
    {
        m_pChatInfo->m_pChatUserList.insert(sFromAccount, CEBAccountInfo::create(sFromAccount,""));
    }
    if (m_sOwnerAccount==0)
        m_sOwnerAccount = sFromAccount;

    if (pRequestInfo.get() == NULL)
        pRequestInfo = CPOPSotpRequestInfo::create(0);
    else
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
    }
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-from", sFromAccount));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-type", nType));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-id", sChatId));
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("cm-chat-key", sChatKey));
    sotp()->doBeginCallLock();
    if (nAppId>0)
        sotp()->doAddParameter(CGC_PARAMETER("app_id", nAppId));
    if (nFromIpAddress>0)
        sotp()->doAddParameter(CGC_PARAMETER("from_ip_address", nFromIpAddress));
    sotp()->doAddParameter(CGC_PARAMETER("type", nType));
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", sFromAccount));
    //if (sOffUser>0)
    //	sotp()->doAddParameter(CGC_PARAMETER("off_uid", sOffUser));
    if (sDepCode>0)
        sotp()->doAddParameter(CGC_PARAMETER("dep-code", sDepCode));
    if (sCallId>0)
        sotp()->doAddParameter(CGC_PARAMETER("call-id", sCallId));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", sChatId));
    sotp()->doAddParameter(CGC_PARAMETER("chat-key", sChatKey));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    pRequestInfo->SetCallId(nCallId);
    m_pRequestList.insert(nCallId, pRequestInfo);
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_ENTER, EB_CALL_NAME_CM_ENTER, true))
    {
        return 0;
    }
    m_pRequestList.remove(nCallId);
    return -2;
}

int CPOPCChatManager::SendCMNotify(cr::bigint nToUserId, int nNotifyType, cr::bigint nNotifyId, const tstring& sNotifyData,CPOPSotpRequestInfo::pointer pRequestInfo)
{
    if (!IsClientStarted()) return -1;
    if (m_sOwnerAccount==0) return 1;
    //if (pRequestInfo.get() == NULL)
    //	pRequestInfo = CPOPSotpRequestInfo::create(0);
    //else
    //{
	//	pRequestInfo->SetResponseTime(0);
    //	pRequestInfo->SetRequestTime();
    //}
    sotp()->doBeginCallLock();
    sotp()->doAddParameter(CGC_PARAMETER("from_uid", m_sOwnerAccount));
    if (nToUserId>0)
        sotp()->doAddParameter(CGC_PARAMETER("to_uid", nToUserId));
    sotp()->doAddParameter(CGC_PARAMETER("chat-id", m_pChatInfo->GetChatId()));
    sotp()->doAddParameter(CGC_PARAMETER("notify-type", nNotifyType));
    if (nNotifyId!=0)
        sotp()->doAddParameter(CGC_PARAMETER("notify-id", nNotifyId));
    if (!sNotifyData.empty())
        sotp()->doAddParameter(CGC_PARAMETER("notify-data", sNotifyData));
    const unsigned long nCallId = sotp()->doGetNextCallId();
    if (pRequestInfo.get() != NULL)
    {
		pRequestInfo->SetResponseTime(0);
        pRequestInfo->SetRequestTime();
        pRequestInfo->SetCallId(nCallId);
        m_pRequestList.insert(nCallId, pRequestInfo);
    }
    if (sotp()->doSendAppCall2(nCallId,EB_SIGN_CM_NOTIFY, EB_CALL_NAME_CM_NOTIFY, true))
    {
        return 0;
    }
    if (pRequestInfo.get() != NULL)
        m_pRequestList.remove(nCallId);
    return -2;
}

} // namespace entboost
