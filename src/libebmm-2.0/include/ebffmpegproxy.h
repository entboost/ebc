#ifndef EBFFMPEGPROXY_H
#define EBFFMPEGPROXY_H

extern "C"
{
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
//#include "libavfilter/buffersink.h"
}

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class EbFfmpegCallback
{
public:
    virtual void onVideoEncodeData(const unsigned char */*videoData*/, int /*dataSize*/) {}
    virtual void onVideoDecodeData(const unsigned char */*videoData*/, int /*dataSize*/) {}
    virtual void onAudioEncodeData(const unsigned char */*audioData*/, int /*dataSize*/) {}
    virtual void onAudioDecodeData(const unsigned char */*audioData*/, int /*dataSize*/) {}
};
class EbFfmpegProxy
{
public:
    EbFfmpegProxy(void);
    virtual ~EbFfmpegProxy(void);

    void setCallback(EbFfmpegCallback *cb) {m_callback=cb;}

    void setBitRate(int v) {m_bitRate=v;}
    int bitRate(void) const {return m_bitRate;}
    /// video
    void setVideoSize(int width, int height);
    int videoWidth(void) const {return m_width;}
    int videoHeight(void) const {return m_height;}
    void setKeyFrameInterval(int v=10) {m_keyFrameInterval=v;}
    int keyFrameInterval(void) const {return m_keyFrameInterval;}
    void setFormat(enum AVPixelFormat format=AV_PIX_FMT_RGBA);
    enum AVPixelFormat format(void) const {return m_format;}
    /// audio default 16000
    void setAudioSampleRate(int sampleRate=16000) {m_sampleRate=sampleRate;}
    int audioSampleRate(void) const {return m_sampleRate;}
    void setAudioChannels(int channels=1) {m_audioChannels=channels;}
    int audioChannels(void) const {return m_audioChannels;}

    unsigned int frameCount(void) const {return m_frameCount;}

    /// encoder=false: decoder
    /// AV_PIX_FMT_RGBA=RGB32
    bool open(enum AVCodecID id, bool encoder);
    bool isOpened(void) const;
    void close(void);

    /// video 视频编解码
    bool encodeVideo(const unsigned char *data,
                     unsigned char *destBuffer=0, int bufferSize=0, int *decodeSize=0, bool *pOutKeyFrame=0);
    bool decodeVideo(const unsigned char *data, int size,
                     unsigned char *destBuffer=0, int bufferSize=0, int *decodeSize=0, bool *pOutKeyFrame=0);
    /// audio 音频编解码
    bool encodeAudio(const unsigned char *data, int size);
    bool decodeAudio(const unsigned char *data, int size);

private:
    bool fillAVPacket(const unsigned char *data, int size, AVPacket *avpkt);
    bool fillBufferData(int requestSize);
private:
    AVCodecContext *m_context;
    AVFrame *m_frameFfmpep;
    AVFrame *m_frameUser;        /// encoder=src; decoder=dest
    SwsContext *m_swsContext;
    EbFfmpegCallback * m_callback;

    int m_bitRate;              /// video:600000 audio:64000
    /// video
    int m_width;
    int m_height;
    int m_keyFrameInterval;     /// default 10
    int64_t m_frameCount;
    AVPixelFormat m_format;    /// default AV_PIX_FMT_RGBA=RGB32
    int m_videoBitCount;
    /// audio only
    int m_sampleRate;           /// default 16000
    int m_audioChannels;         /// default 1

    unsigned char *m_bufferData;    /// for video decode
    int m_bufferSize;
};

#endif // EBFFMPEGPROXY_H
