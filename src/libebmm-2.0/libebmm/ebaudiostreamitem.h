#ifndef EBAUDIOSTREAMITEM_H
#define EBAUDIOSTREAMITEM_H

#include <CGCLib/CGCLib.h>
#include <QAudioOutput>
#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "ebaudiobuffer.h"
#include "ebvoicebuffer.h"
#include "ebaacdecoder.h"
#include "ebaacencoder.h"

typedef void (*FFTdataFn) (mycp::bigint nId, const char *data, int size, void *userData);

namespace entboost {

class EbAudioStreamItem : QObject
        , public EbAacEncoderCallback
        , public EbAacDecoderCallback
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbAudioStreamItem> pointer;
    EbAudioStreamItem(mycp::bigint userId);
    virtual ~EbAudioStreamItem(void);
    static EbAudioStreamItem::pointer create(mycp::bigint userId);

    bool open(int srcSampleRate, int srcChannels);
    void close(void);
    void startPlay(void);
    void stopPlay(void);
//    size_t size(void) const {return m_playVoiceList.size();}

    void setFFTCallback(FFTdataFn cb, void *userData);
    void enqueueFrame(mycp::bigint Id, const unsigned char *buf, unsigned int len, unsigned int ts);
//    void ForwardFrame(mycp::bigint Id, const unsigned char *buf, unsigned int len, unsigned int ts);

    bool isLimited(void);
    bool write(const EbAudioBuffer::pointer &pAudioBuffer);
    void playBuffer(EbAudioBuffer::pointer &pOutBuffer);

protected:
    virtual void onEncodeInitOk(const unsigned char *decodeSCData, int size);
    virtual void onEncodeOk(const unsigned char */*data*/, int /*size*/, unsigned long /*param*/) {}
    virtual void onDecodeOk(const unsigned char *data, int size, unsigned long param);
    void writeVoiceData(void);
private:
    enum PLAY_STATE {
        STATE_STOP,
        STATE_PLAY
    };

    mycp::bigint m_userId;
    EbAacDecoder *m_aacDecoder;
    QAudioOutput *m_audioOutput;
    QIODevice *m_streamOut;

    unsigned int m_nTimestampOfLastFrame;
    unsigned short m_nLastSequenceOfUpperFrame;

    int m_srcSampleRate;
    int  m_srcChannels;
    int m_srcBitRate;

    PLAY_STATE m_nPlayState;
    FFTdataFn  m_fnFFTcb;
    void*     m_fnFFTcb_param;

    CLockList<EbAudioBuffer::pointer> m_audioList;

    int m_nHoldSize;
    CLockList<EbVoiceBuffer::pointer> m_holdVoiceList;  /// PCM voice data
    CLockList<EbVoiceBuffer::pointer> m_playVoiceList;  /// PCM voice data
};

}   /// namespace entboost

#endif // EBAUDIOSTREAMITEM_H
