#include "ebaudiostreammanager.h"
#include "ebtimegettime.h"

namespace entboost {

EbAudioStreamManager::EbAudioStreamManager(int srcSampleRate, int srcChannels, QObject *parent)
    : QObject(parent)
    , m_isStoped(true)
    , m_srcSampleRate(srcSampleRate)
    , m_srcChannels(srcChannels)
    , m_srcBitRate(0)

{

}

EbAudioStreamManager::~EbAudioStreamManager()
{
    Stop();
}

bool EbAudioStreamManager::AddStream(mycp::bigint nId, int srcSampleRate, int srcChannels)
{
    if (srcSampleRate == 0)
        srcSampleRate = m_srcSampleRate;
    if (srcChannels == 0)
        srcChannels = m_srcChannels;
    assert (srcChannels != 0 && srcSampleRate != 0);

    if (!m_streamlst.exist(nId)) {
        EbAudioStreamItem::pointer audioItem = EbAudioStreamItem::create(nId);
        if (!audioItem->open(srcSampleRate, srcChannels)) {
            return false;
        }
        m_streamlst.insert(nId, audioItem, false);
    }
    return true;
}

void EbAudioStreamManager::DelStream(mycp::bigint nId)
{
    EbAudioStreamItem::pointer audioItem;
    if (m_streamlst.find(nId, audioItem, true)) {
        audioItem->stopPlay();
    }
}

void EbAudioStreamManager::DelAllStream()
{
    BoostWriteLock wrlock(m_streamlst.mutex());
    CLockMap<mycp::bigint, EbAudioStreamItem::pointer>::iterator iter = m_streamlst.begin();
    for (; iter!=m_streamlst.end(); iter++) {
        const EbAudioStreamItem::pointer &audioItem = iter->second;
        audioItem->stopPlay();
    }
    m_streamlst.clear(false);
}

void EbAudioStreamManager::WritestreamData(mycp::bigint userId, unsigned char *buf, unsigned int len, unsigned short seq, unsigned int ts)
{
    if (m_isStoped) {
        return;
    }

    EbAudioStreamItem::pointer audioItem;
    if (!m_streamlst.find(userId, audioItem)) {
        return;
    }

//    if(!IsExistStream(nId))
//        return;

    EbAudioBuffer::pointer pAudioBuffer = getPool();
    pAudioBuffer->allocBuffer(len);
    if (pAudioBuffer->data == 0) {
        return;
    }
    pAudioBuffer->ssrc = userId;
    pAudioBuffer->ts = ts;
    pAudioBuffer->seq =(unsigned short)seq;
    memcpy(pAudioBuffer->data, buf, len);
    pAudioBuffer->len = len;
    audioItem->write(pAudioBuffer);
//    m_msgqlst.add(pAudioBuffer);
}

EbAudioBuffer::pointer EbAudioStreamManager::getPool()
{
    EbAudioBuffer::pointer pResult;
    if (!m_pPool.front(pResult)) {
        pResult = EbAudioBuffer::create();
    }
    return pResult;
}

void EbAudioStreamManager::setPool(const EbAudioBuffer::pointer &pBuffer)
{
    if (pBuffer.get()!=0 && m_pPool.size()<50) {
        m_pPool.add(pBuffer);
    }
}

void EbAudioStreamManager::SetStreamFFTCallback(mycp::bigint nId, FFTdataFn cb, void *userData)
{
    EbAudioStreamItem::pointer audioItem;
    if (m_streamlst.find(nId, audioItem)) {
        audioItem->setFFTCallback(cb, userData);
    }
}

void EbAudioStreamManager::Run()
{
    if (m_thread.get()!=0) {
        return;
    }
    m_isStoped = false;
    m_thread = boost::shared_ptr<boost::thread>(
                new boost::thread(boost::bind(&EbAudioStreamManager::onRunProcessData, this)));
}

void EbAudioStreamManager::Stop()
{
    m_isStoped = true;
    /// 等待线程退出
    if (m_thread.get()!=0) {
        m_thread->join();
        m_thread.reset();
    }
    ClearData();
}

void EbAudioStreamManager::StartPlay(mycp::bigint nId)
{
    EbAudioStreamItem::pointer audioItem;
    if (m_streamlst.find(nId, audioItem)) {
        audioItem->startPlay();
    }
}

void EbAudioStreamManager::StopPlay(mycp::bigint nId)
{
    EbAudioStreamItem::pointer audioItem;
    if (m_streamlst.find(nId, audioItem)) {
        audioItem->stopPlay();
    }
}

void EbAudioStreamManager::SetDefaultConfig(int srcSampleRate, int srcChannels)
{
    m_srcSampleRate = srcSampleRate;
    m_srcChannels = srcChannels;
}

void EbAudioStreamManager::onRunProcessData()
{
    try
    {
        ProcessData();
    }catch(const std::exception&)
    {
    }catch(...)
    {}
}

void EbAudioStreamManager::ProcessData()
{
    BOOL bRet;
    EbAudioBuffer::pointer pAudioBuffer;
    int load_cnt = 0;;

    while ( !m_isStoped) {
        load_cnt = 0;
        /// Read Cache.max read 100 buffer.
//        while (!m_isStoped && m_msgqlst.front(pAudioBuffer)) {
//            EbAudioStreamItem::pointer audioItem;
//            if (m_streamlst.find(pAudioBuffer->ssrc, audioItem)) {
//                bRet = audioItem->write(pAudioBuffer);
//                if ( !bRet) {
//                    this->setPool(pAudioBuffer);
//                }
//            }
//            else {
//                this->setPool(pAudioBuffer);
//            }
//            load_cnt++;
//            if (load_cnt > 100) {	///
//                load_cnt = 0;
//                break;
//            }
//        }
//        load_cnt = 0;

        /// process the buffer.
        /// 检测所有的stream是不是需要播放(缓冲完成)/继续写(收到标志)/缓冲(停止标志)。
        {
            const unsigned long currentPlayTime = timeGetTime();
            BoostReadLock rdlock(m_streamlst.mutex());
            CLockMap<mycp::bigint, EbAudioStreamItem::pointer>::iterator iter = m_streamlst.begin();
            for (; !m_isStoped && iter!=m_streamlst.end(); iter++) {
                const EbAudioStreamItem::pointer &audioItem = iter->second;
//                const int nStreamSize = audioItem->size();
//                int fps = 20;
//                if (nStreamSize < 5)
//                    fps = 40;
//                else if (nStreamSize < 8)
//                    fps = 50;
//                else if (nStreamSize < 15)
//                    fps = 60;
//                else
//                    fps = 70;
//                if (currentPlayTime < audioItem->lastPlayTime() + (1000/fps)) {
//                    continue;
//                }
//                EbAudioBuffer::pointer pOutBuffer;
//                audioItem->playBuffer(pOutBuffer);
//                this->setPool(pOutBuffer);

                EbAudioBuffer::pointer pOutBuffer;
                do
                {
                    pOutBuffer.reset();
                    audioItem->playBuffer(pOutBuffer);
                    if (pOutBuffer.get()==0) {
                        break;
                    }
                    this->setPool(pOutBuffer);
                }while (!m_isStoped && (++load_cnt)<100);
            }
        }

#ifdef WIN32
        Sleep(3);
#else
        usleep(3000);
#endif
    }
}

void EbAudioStreamManager::ClearData()
{
    m_msgqlst.clear();
    m_pPool.clear();
    m_streamlst.clear();
    m_srcSampleRate = 0;
    m_srcChannels = 0;
}

}   /// namespace entboost
