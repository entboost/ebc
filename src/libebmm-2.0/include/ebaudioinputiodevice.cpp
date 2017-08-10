#include "ebaudioinputiodevice.h"
#include <ebaacencoder.h>
#include "ebtimegettime.h"

EbAudioInputIODevice::EbAudioInputIODevice(QObject *parent)
    : QIODevice(parent)
    , m_detectSilenceData(true)
//    , m_aacEncoder(this)
//    , m_aacDecoder(this)
{
//    m_file.setFileName("./test_audio.pcm");
//    m_file.open( QIODevice::WriteOnly | QIODevice::Truncate );
//    m_aacEncoder.init(1, DEFAULT_SAMPLERATE, DEFAULT_AUDIO_BITRATE);

//    m_timerId = this->startTimer(1);
    m_buffer = new short[8*1024];
}

EbAudioInputIODevice::~EbAudioInputIODevice()
{
//    this->killTimer(m_timerId);
//    m_file.close();
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
//    memcpy(m_buffer, data, len*2);
//    emit captureAudioData((const char*)m_buffer, len*2, timeGetTime());
    emit captureAudioData(data, len, timeGetTime());
    return len;
//    /// 直接保存 PCM 文件是可以的
////    m_file.write(data, len);
//    /// 先压缩
//    m_list.add(EbData::create((unsigned char*)data, (unsigned int)len));
////    m_aacEncoder.encode((const short*)data, (int)len, this);

//    return len;
}

//void EbAudioInputIODevice::timerEvent(QTimerEvent *e)
//{
//    if (m_timerId!=0 && m_timerId==e->timerId()) {
//        EbData::pointer data;
//        if (m_list.front(data)) {
//            m_aacEncoder.encode((const short*)data->data(), (int)data->size());
//        }
//    }
//    QIODevice::timerEvent(e);
//}

//void EbAudioInputIODevice::onEncodeInitOk(const unsigned char *decodeSCData, int size)
//{
//    if (!m_aacDecoder.isInited()) {
//        m_aacDecoder.init(decodeSCData, size);
//    }
//}

//void EbAudioInputIODevice::onEncodeOk(const unsigned char *data, int size)
//{
//    m_aacDecoder.decode(data, size);
//}

//void EbAudioInputIODevice::onDecodeOk(const unsigned char *data, int size)
//{
//    m_file.write((const char*)data, (qint64)size);
//}
