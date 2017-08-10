#include "ebvideostreamitem.h"

#include "ebtimegettime.h"

namespace entboost {

EbVideoStreamItem::EbVideoStreamItem(mycp::bigint userId, int codeType, int width, int height)
    : m_userId(userId)
    , m_codeType(codeType)
    , m_width(width)
    , m_height(height)
    , m_delay(0)
    , m_itemData(0)
    , m_isReady(false)
    , m_waitForKeyFrame(true)
    , m_lastPlayTime(0)
    , m_lastSequenceOfUpperFrame(0)
    , m_timestampOfLastFrame(0)
{

}

EbVideoStreamItem::~EbVideoStreamItem()
{
    stop();
    m_h264Decoder.close();
    m_bufferList.clear();
}

EbVideoStreamItem::pointer EbVideoStreamItem::create(mycp::bigint userId, int codeType, int width, int height)
{
    return EbVideoStreamItem::pointer(new EbVideoStreamItem(userId, codeType, width, height));
}

//FILE * theFileLogWrite = 0;
bool EbVideoStreamItem::write(const EbVideoBuffer::pointer &videoBuffer)
{
    if (videoBuffer.get()==0) {
        return false;
    }
    if (!m_isReady) {
        return false;
    }
//    if (m_codeType==EB_CODEC_TYPE_H264) {
//        if (theFileLogWrite==0) {
//            theFileLogWrite = fopen("d:/h264_test.txt", "w");
//        }
//        if (theFileLogWrite!=0) {
//            char buf[64];
//            sprintf(buf, "seq=%d, ts=%d, size=%d\n", (int)videoBuffer->seq, (int)videoBuffer->ts, (int)videoBuffer->size);
//            fwrite(buf, 1, strlen(buf), theFileLogWrite);
//            fflush(theFileLogWrite);
//        }
//    }
    /// 直接处理
    boost::mutex::scoped_lock lock(m_mutex);
    m_bufferList.push_back( videoBuffer);
    return true;

//    /// check expired packet
//    if ((videoBuffer->ts <= m_timestampOfLastFrame)
//            && ((m_timestampOfLastFrame - videoBuffer->ts)<0xFFFF)) {
//        /// 过期视频帧，不处理
////        const unsigned long temp = m_timestampOfLastFrame - pVideoBuf->ts;
//        return false;
//    }

//    /// reset the expired time.
//    videoBuffer->expire_ts = timeGetTime() + m_delay;
//    /// write to pool.
//    boost::mutex::scoped_lock lock(m_mutex);
//    if (m_bufferList.empty()) {
//        m_bufferList.push_back( videoBuffer);
//    }
//    else {
//        /// sort ascending
//        bool bfound = false;
//        EbVideoBuffer::pointer bufferTemp;
//        std::list<EbVideoBuffer::pointer>::iterator iter = m_bufferList.begin();
//        for (iter; iter!=m_bufferList.end(); iter++) {
//            bufferTemp = (*iter);
//            if (bufferTemp->ts > videoBuffer->ts) {
//                bfound = true;
//                break;
//            }
//            else if ((videoBuffer->ts - bufferTemp->ts) > 0xFFFF0000) {
//                bfound = true;
//                break;
//            }
//        }
////        if (m_codeType==EB_CODEC_TYPE_H264) {
////            if (theFileLogWrite!=0) {
////                char buf[64];
////                sprintf(buf, "seq=%d, ts=%d, size=%d fount=%d\n", (int)videoBuffer->seq, (int)videoBuffer->ts, (int)videoBuffer->size, (int)(bfound?1:0));
////                fwrite(buf, 1, strlen(buf), theFileLogWrite);
////                fflush(theFileLogWrite);
////            }
////        }
//        /// insert postion.
//        if (bfound)
//            m_bufferList.insert(iter, videoBuffer);
//        else
//            m_bufferList.push_back( videoBuffer);
//    }
//    return true;
}

//FILE * theFileLogRead = 0;
bool EbVideoStreamItem::read(VIDEOFRAME *pOutFrame, unsigned long *pdwSizeRead, EbVideoBuffer::pointer &pOutBuffer)
{
    EbVideoBuffer::pointer videoBuffer;
    bool bfound = false;

    if (!m_isReady) {
        return false;
    }
    if (pOutFrame->bufferSize<=0 || pdwSizeRead==0) {
        return false;
    }
    *pdwSizeRead = 0;
//    if (m_codeType==EB_CODEC_TYPE_H264) {
//        if (theFileLogRead==0) {
//            theFileLogRead = fopen("d:/h264_test_read.txt", "w");
//        }
//    }
    boost::mutex::scoped_lock lock(m_mutex);
    if ( !m_bufferList.empty()) {
        videoBuffer = m_bufferList.front();
        /// 直接处理
        m_lastPlayTime = timeGetTime();
        m_lastSequenceOfUpperFrame = (unsigned short)videoBuffer->seq;
        m_timestampOfLastFrame = videoBuffer->ts;
        m_bufferList.pop_front();

//        if (m_lastSequenceOfUpperFrame==0 ||
//            (unsigned short)(m_lastSequenceOfUpperFrame + 1) == (unsigned short)videoBuffer->seq) {
//            bfound = true;
//        }
//        else if (videoBuffer->expire_ts < timeGetTime()) {
//            bfound = true;
//        }
//        else if (m_bufferList.size() > 10) {
//            /// 10 比 15，感觉更流畅，不会停顿太久
//            bfound = true;
//        }
//        if (bfound) {
//            m_lastPlayTime = timeGetTime();
//            m_lastSequenceOfUpperFrame = (unsigned short)videoBuffer->seq;
//            m_timestampOfLastFrame = videoBuffer->ts;
//            m_bufferList.pop_front();
//        }
//        else {
////            if (m_codeType==EB_CODEC_TYPE_H264) {
////                if (theFileLogRead!=0) {
////                    char buf[64];
////                    sprintf(buf, "seq=%d, ts=%d, size=%d, not found\n", (int)videoBuffer->seq, (int)videoBuffer->ts, (int)videoBuffer->size);
////                    fwrite(buf, 1, strlen(buf), theFileLogRead);
////                    fflush(theFileLogRead);
////                }
////            }
//            videoBuffer.reset();
//        }
    }
    lock.unlock();
    if (videoBuffer.get()!=0) {
        pOutBuffer = videoBuffer;
        const bool ret = decode(videoBuffer->data, videoBuffer->size, pOutFrame);
//        if (m_codeType==EB_CODEC_TYPE_H264) {
//            if (theFileLogRead!=0) {
//                char buf[64];
//                sprintf(buf, "seq=%d, ts=%d, size=%d, decode=%d\n", (int)videoBuffer->seq, (int)videoBuffer->ts, (int)videoBuffer->size, (int)(ret?1:0));
//                fwrite(buf, 1, strlen(buf), theFileLogRead);
//                fflush(theFileLogRead);
//            }
//        }
        return ret;
    }
    return false;
}

bool EbVideoStreamItem::decode(const unsigned char *data, int size, VIDEOFRAME *pOutFrame)
{
    if (m_codeType==EB_CODEC_TYPE_NOCPMPRESS) {
        memcpy(pOutFrame->data, data, size);
        pOutFrame->usedSize = size;
        return true;
    }
    else if (m_codeType!=EB_CODEC_TYPE_H264) {
        return false;
    }

    if (!m_h264Decoder.isOpened()) {
        m_h264Decoder.setFormat(AV_PIX_FMT_RGB24);
        if (!m_h264Decoder.open(AV_CODEC_ID_H264, false)) {
            return false;
        }
    }

    bool keyFrame = false;
    if (!m_h264Decoder.decodeVideo(data, size, pOutFrame->data,
                                   (int)pOutFrame->bufferSize, (int*)&pOutFrame->usedSize, &keyFrame)) {
        m_waitForKeyFrame = true;
        return false;
    }
    if (m_waitForKeyFrame) {
        if (keyFrame) {
            m_waitForKeyFrame = false;
        }
        else {
            /// 等待视频关键帧
            return false;
        }
    }
    return true;
}

}   /// namespace entboost
