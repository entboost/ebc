#ifndef EBAACDECODER_H
#define EBAACDECODER_H

class EbAacDecoderCallback
{
public:
    virtual void onDecodeOk(const unsigned char *data, int size, unsigned long param) = 0;
};

class EbAacDecoder
{
public:
    EbAacDecoder(EbAacDecoderCallback *callback);
    virtual ~EbAacDecoder(void);

    void setCallback(EbAacDecoderCallback *cb) {m_callback=cb;}
    EbAacDecoderCallback *callback(void) const {return m_callback;}

    bool init(const unsigned char *data, unsigned int size);
    bool isInited(void) const;
    int decode(const unsigned char *data, unsigned int size, unsigned long param=0);
    void stop(void);
private:
    void *m_handle;
    EbAacDecoderCallback *m_callback;
    int m_samplesPerFrame;
    int m_srcSampleRate;
    int m_srcChannels;
};

#endif // EBAACDECODER_H
