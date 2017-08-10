#ifndef EBAACENCODER_H
#define EBAACENCODER_H

class EbAacEncoderCallback
{
public:
    virtual void onEncodeInitOk(const unsigned char *decoderSCData, int size) = 0;
    virtual void onEncodeOk(const unsigned char *data, int size, unsigned long param) = 0;
};

class EbAacEncoder
{
public:
    EbAacEncoder(EbAacEncoderCallback *callback);
    virtual ~EbAacEncoder(void);

    void setCallback(EbAacEncoderCallback *cb) {m_callback=cb;}
    EbAacEncoderCallback *callback(void) const {return m_callback;}

    bool init(int channels, int sampleRate,	int bitRate);
    bool isInited(void) const;
    bool encode(const short *samples, int length, unsigned long param=0);
    void stop(void);
    /// 返回0表示静音数据，可以不需要发送
    static unsigned long DetectSilence(const unsigned char *data, unsigned long size);
private:
    void *m_handle;
    EbAacEncoderCallback *m_callback;
    unsigned char * m_buffer;
    int         m_samplesPerFrame;
    int         m_srcSampleRate;
    int         m_srcChannels;
    int         m_srcBitRate;
    int         m_aacFrameMaxSize;
};

#endif // EBAACENCODER_H
