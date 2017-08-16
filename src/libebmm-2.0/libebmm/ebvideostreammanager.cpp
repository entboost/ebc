#include "ebvideostreammanager.h"
#include "ebtimegettime.h"
#include "ebvideodefine.h"

namespace entboost {

EbVideoStreamManager::EbVideoStreamManager(int nWidth, int nHeight, QObject *parent)
    : QObject(parent)
    , m_bReady(false)
    , m_isStoped(true)
    , m_callback(0)
    , m_cbParam(0)
    , m_nWidth(nWidth)
    , m_nHeight(nHeight)

{

}

EbVideoStreamManager::~EbVideoStreamManager()
{
//    m_cachelst.clear();
    m_videoBufferPool.clear();
    m_videoFramePool.clear();
    DelAllUser();
    Stop();
}

void EbVideoStreamManager::AddUser(mycp::bigint userId, int delaytime, int codeType)
{
    if (m_userList.exist(userId)) {
        return;
    }
    if (!m_bReady) {
        return;
    }
    EbVideoStreamItem::pointer videoStreamItem = EbVideoStreamItem::create(userId, codeType, m_nWidth , m_nHeight);
    videoStreamItem->setDelay(delaytime);
    videoStreamItem->run();
    m_userList.insert(userId, videoStreamItem);
}

void EbVideoStreamManager::DelUser(mycp::bigint nUserId)
{
    if (!m_bReady) {
        return;
    }
    EbVideoStreamItem::pointer videoStreamItem;
    if (m_userList.find(nUserId, videoStreamItem, true)) {
        videoStreamItem->stop();
    }
}

void EbVideoStreamManager::DelAllUser()
{
    m_userList.clear();
}

bool EbVideoStreamManager::SetUserData(mycp::bigint nUserId, unsigned long userData)
{
    EbVideoStreamItem::pointer videoStreamItem;
    if (m_userList.find(nUserId, videoStreamItem)) {
        videoStreamItem->setItemData(userData);
        return true;
    }
    return false;
}

unsigned long EbVideoStreamManager::GetUserData(mycp::bigint nUserId)
{
    EbVideoStreamItem::pointer videoStreamItem;
    if (m_userList.find(nUserId, videoStreamItem)) {
        return videoStreamItem->itemData();
    }
    return 0;
}

bool EbVideoStreamManager::Write(
        mycp::bigint userId,
        unsigned long size,
        const unsigned char *data,
        unsigned short seq,
        unsigned long ts,
        unsigned char pdtype,
        int pt)
{
    if (!m_bReady) {
        return false;
    }
#ifdef _EB_USES_VIDEO_STREAM_CACHE
    if (!m_userList.exist(userId)) {
        return false;
    }
#endif
    EbVideoBuffer::pointer videoBuffer = getVideoBufferPool();
    if (videoBuffer->data==0)
        videoBuffer->allocBuffer(size+2048);
    else
        videoBuffer->allocBuffer(size+32);
    if (videoBuffer->data==0) {
        return false;
    }
    videoBuffer->userId = userId;
    videoBuffer->seq = (unsigned short)seq;
    videoBuffer->ts = ts;
    videoBuffer->expire_ts = 0;
    videoBuffer->payload_type = pdtype;
    videoBuffer->pt = pt;
    memcpy( videoBuffer->data, data, size);
    videoBuffer->size = size;
#ifdef _EB_USES_VIDEO_STREAM_CACHE
    m_videoBufferCache.add(videoBuffer);
#else
    EbVideoStreamItem::pointer videoStreamItem;
    if (m_userList.find(userId, videoStreamItem)) {
        videoStreamItem->write(videoBuffer);
        return true;
    }
    setVideoBufferPool(videoBuffer);
    return false;
#endif
}

#ifdef _EB_USES_VIDEO_STREAM_CACHE
void EbVideoStreamManager::readCache()
{
    EbVideoBuffer::pointer videoBuffer;
    EbVideoStreamItem::pointer videoItem;
    int loadCount = 0;
    while (!m_isStoped && m_videoBufferCache.front(videoBuffer)) {
        if (!m_userList.find(videoBuffer->userId, videoItem)) {
            this->setVideoBufferPool(videoBuffer);
            continue;
        }
        if (!videoItem->write(videoBuffer)) {
            this->setVideoBufferPool(videoBuffer);
            continue;
        }

        if ( ++loadCount > 50) {
            loadCount = 0;
            break;
        }
    }
}
#endif /// _EB_USES_VIDEO_STREAM_CACHE

void EbVideoStreamManager::ProcessData(void)
{
    bool     hRet = false;
    unsigned long       dwSizeread = 0;
    EbVideoFrame::pointer pVideoFrame = getVideoFramePool();
    /// new a video frame.
    pVideoFrame->allocBuffer(EB_MAX_VIDEO_FRAME_SIZE);
    pVideoFrame->height  = 0;
    pVideoFrame->width   = 0;
    pVideoFrame->type    = 0;///BI_RGB;
    pVideoFrame->usedSize = 0;
    pVideoFrame->timastamp = 0;

    while (!m_isStoped) {
        /// process video stream.
#ifdef _EB_USES_VIDEO_STREAM_CACHE
        readCache();
#endif  /// _EB_USES_VIDEO_STREAM_CACHE
        if (m_userList.empty()) {
#ifdef WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
            continue;
        }
        const unsigned long currentPlayTime = timeGetTime();
        AUTO_RLOCK(m_userList);
        CLockMap<mycp::bigint, EbVideoStreamItem::pointer>::iterator iter = m_userList.begin();
        for (; iter!=m_userList.end(); iter++) {
            const EbVideoStreamItem::pointer & videoStreamItem = iter->second;
            const int nStreamSize = videoStreamItem->size();
            int fps = 0;
            if (videoStreamItem->codeType() == EB_CODEC_TYPE_NOCPMPRESS)
                fps = 9+2;
            else if (nStreamSize < 5)
                fps = 8;
            else if (nStreamSize < 8)
                fps = 10;
            else if (nStreamSize < 15)
                fps = 12;
            else
                fps = 15;
            if (currentPlayTime < videoStreamItem->lastPlayTime() + (1000/fps)) {
                continue;
            }

            hRet = false;
            dwSizeread = 0;
            /// TODO: process video stream.
            EbVideoBuffer::pointer pOutBuffer;
            hRet = videoStreamItem->read(pVideoFrame.get(), &dwSizeread, pOutBuffer);
            this->setVideoBufferPool(pOutBuffer);
            if (hRet) {
                if (m_callback!=0)
                    m_callback(videoStreamItem->userId(),
                               pVideoFrame.get(),
                               m_cbParam,
                               videoStreamItem->itemData());
            }
//            else if (hRet != E_PENDING)
//            {
//                // ???
//            }
        }
    }
}

void EbVideoStreamManager::Run()
{
    if (m_thread.get()!=0) {
        return;
    }
    m_bReady = true;
    m_isStoped = false;
    m_thread = boost::shared_ptr<boost::thread>(
                new boost::thread(boost::bind(&EbVideoStreamManager::ProcessData, this)));
}

void EbVideoStreamManager::Stop()
{
    m_isStoped = true;
    m_bReady = false;
    /// 等待线程退出
    if (m_thread.get()!=0) {
        m_thread->join();
        m_thread.reset();
    }
}

void EbVideoStreamManager::clearVideoLst()
{
    m_userList.clear();
}

void EbVideoStreamManager::clearSelf()
{
    m_bReady = false;
    clearVideoLst();
//    clearCacheLst();
}

void EbVideoStreamManager::SetVideoStreamCallBack(VideoStreamCallBack cb, void *param)
{
    m_callback = cb;
    m_cbParam = param;
}

void EbVideoStreamManager::onRunProcessData()
{
    try
    {
        ProcessData();
    }catch(...)
    {}
}

EbVideoBuffer::pointer EbVideoStreamManager::getVideoBufferPool()
{
    EbVideoBuffer::pointer pResult;
    if (!m_videoBufferPool.front(pResult)) {
        pResult = EbVideoBuffer::create();
    }
    return pResult;
}

void EbVideoStreamManager::setVideoBufferPool(const EbVideoBuffer::pointer &pVideoBuffer)
{
    if (pVideoBuffer.get()!=NULL && m_videoBufferPool.size()<100) {
        m_videoBufferPool.add(pVideoBuffer);
    }
}

EbVideoFrame::pointer EbVideoStreamManager::getVideoFramePool()
{
    EbVideoFrame::pointer pResult;
    if (!m_videoFramePool.front(pResult)) {
        pResult = EbVideoFrame::create();
    }
    return pResult;
}

void EbVideoStreamManager::setVideoFramePool(const EbVideoFrame::pointer &pVideoFrame)
{
    if (pVideoFrame.get()!=NULL && m_videoFramePool.size()<100) {
        m_videoFramePool.add(pVideoFrame);
    }
}

}   /// namespace entboost
