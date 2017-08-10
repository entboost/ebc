#ifndef EBVIDEOSTREAMITEM_H
#define EBVIDEOSTREAMITEM_H

#include <CGCLib/CGCLib.h>
#include <boost/thread/mutex.hpp>
#include <list>
#include "../include/ebffmpegproxy.h"
#include "ebvideobuffer.h"
#include "ebvideoframe.h"

#define EB_CODEC_TYPE_NOCPMPRESS 0
#define EB_CODEC_TYPE_XVID       1
#define EB_CODEC_TYPE_H264       2

namespace entboost {

class EbVideoStreamItem
{
public:
    typedef boost::shared_ptr<EbVideoStreamItem> pointer;
    EbVideoStreamItem(mycp::bigint userId, int codeType, int width, int height);
    virtual ~EbVideoStreamItem(void);
    static EbVideoStreamItem::pointer create(mycp::bigint userId, int codeType, int width, int height);

    mycp::bigint userId(void) const {return m_userId;}
    int codeType(void) const {return m_codeType;}
    void setDelay(int v) {m_delay=v;}
    int delay(void) const {return m_delay;}
    void setItemData(unsigned long v) {m_itemData=v;}
    unsigned long itemData(void) const {return m_itemData;}
    size_t size(void) const {return m_bufferList.size();}
    void clearBuffer(void) {m_bufferList.clear();}

    void run(void) {m_isReady = true;}
    void stop(void) {m_isReady = false;}
    bool write(const EbVideoBuffer::pointer &pVideoBuf);
    bool read(VIDEOFRAME *pOutFrame, unsigned long *pdwSizeRead, EbVideoBuffer::pointer &pOutBuffer);

    unsigned long lastPlayTime(void) const {return m_lastPlayTime;}
protected:
    bool decode(const unsigned char *data, int size, VIDEOFRAME *pOutFrame);
private:
    mycp::bigint m_userId;
    int m_codeType;
    int m_width;
    int m_height;
    int m_delay;
    unsigned long m_itemData;
    boost::mutex m_mutex;
    std::list<EbVideoBuffer::pointer> m_bufferList;
    bool m_isReady;
    bool m_waitForKeyFrame;
    unsigned long m_lastPlayTime;
    unsigned short m_lastSequenceOfUpperFrame;
    unsigned int m_timestampOfLastFrame;
    EbFfmpegProxy m_h264Decoder;
};

}   /// namespace entboost

#endif // EBVIDEOSTREAMITEM_H
