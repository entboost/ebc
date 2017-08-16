#include "ebaudioinputiodevice.h"
#include <ebaacencoder.h>
#include "ebtimegettime.h"

EbAudioInputIODevice::EbAudioInputIODevice(QObject *parent)
    : QIODevice(parent)
    , m_detectSilenceData(true)
{
    m_buffer = new short[5*1024];
}

EbAudioInputIODevice::~EbAudioInputIODevice()
{
    close();
    delete[] m_buffer;
}

qint64 EbAudioInputIODevice::readData(char *data, qint64 maxlen)
{
    return 0;
}


qint64 EbAudioInputIODevice::writeData(const char *data, qint64 len)
{
    if (m_detectSilenceData && EbAacEncoder::DetectSilence((const unsigned char*)data, (unsigned long)len)==0) {
        /// 判断为静音数据，不发送
        return len;
    }
//    {
//        /// ?44100 -> 16000
//        const short * tmp = (const short*)data;
//        int destIndex = 0;
//        for (int i=0; i<len; i++) {
//            /// 做二次i++是为了跳过其中一个采样点;（声音会失真）
//            m_buffer[destIndex++] = tmp[i];
//            /// 1024也会失真
//            /// 2048有点声音，声音小，不会失真
//            /// 3076会有异常
//            if (destIndex==2048) {
//                emit captureAudioData((const char*)m_buffer, destIndex, timeGetTime());
//                destIndex=0;
//            }
//        }
//        if (destIndex>0) {
//            emit captureAudioData((const char*)m_buffer, destIndex, timeGetTime());
//        }
//        return len;
//    }

//    const qint64 size = MIN(2048, len);
//    emit captureAudioData(data, size, timeGetTime());
//    return size;
//    const int const_frame_size = 3072;
    const int const_frame_size = 2048;
//    const int const_frame_size = 1228;
    const int nCount = (len+const_frame_size-1)/const_frame_size;
    for (int i=0; i<nCount; i++) {
        int size = MIN((len%const_frame_size), const_frame_size);
        if (size==0)
            size = const_frame_size;
        emit captureAudioData(data+(i*const_frame_size*2), size, timeGetTime());
    }
    return len;
}
