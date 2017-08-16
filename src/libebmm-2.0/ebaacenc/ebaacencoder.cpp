#include "ebaacencoder.h"
#include "frame.h"

EbAacEncoder::EbAacEncoder(EbAacEncoderCallback *callback)
    : m_handle(0)
    , m_callback(callback)
    , m_buffer(0)
    , m_samplesPerFrame(1024)
    , m_srcSampleRate(0)
    , m_srcChannels(0)
    , m_srcBitRate(0)
{
}

EbAacEncoder::~EbAacEncoder()
{
    stop();
}

bool EbAacEncoder::init(int channels, int sampleRate, int bitRate)
{
    if(m_handle!=0) {
        return true;
    }

    m_srcSampleRate = sampleRate;
    m_srcChannels = channels;
    m_srcBitRate = bitRate;

    m_handle = (void*)faacEncOpen(m_srcSampleRate,
        m_srcChannels,
        (unsigned long*)&m_samplesPerFrame,
        (unsigned long*)&m_aacFrameMaxSize);
    if (m_handle==0) {
        return false;
    }
    if (m_buffer==0) {
        m_buffer = new unsigned char[m_aacFrameMaxSize*6];
    }

    m_samplesPerFrame /= m_srcChannels;
    faacEncConfigurationPtr m_faacConfig = faacEncGetCurrentConfiguration((faacEncHandle)m_handle);

    m_faacConfig->mpegVersion = MPEG4;
    m_faacConfig->aacObjectType = LOW;
    m_faacConfig->allowMidside = 0;
    m_faacConfig->useLfe = 0;
    m_faacConfig->useTns = 0;
//    m_faacConfig->useTns = 1;
    m_faacConfig->inputFormat = FAAC_INPUT_16BIT;
    m_faacConfig->outputFormat = 0;    // raw
    m_faacConfig->quantqual = 0;    // use abr

    m_faacConfig->bitRate = m_srcBitRate/m_srcChannels;
    faacEncSetConfiguration((faacEncHandle)m_handle,m_faacConfig);

    unsigned char*ASC = 0;
    unsigned long ASCLength = 0;
    faacEncGetDecoderSpecificInfo((faacEncHandle)m_handle, &ASC, &ASCLength);
    if (m_callback!=0)
        m_callback->onEncodeInitOk(ASC, (unsigned int)ASCLength);
    return true;
}

bool EbAacEncoder::isInited() const
{
    return m_handle==0?false:true;
}

inline int SamplesToBytes(int numSamples, int channels) {
    return (numSamples * channels * sizeof(short));
}

inline int BytesToSamples(int numBytes, int channels) {
    return (numBytes / (channels * sizeof(short)));
}

bool EbAacEncoder::encode(const short *samples, int length, unsigned long param)
{
    if (m_srcChannels !=1 && m_srcChannels !=2) {
        return false;
    }
    if (m_handle==0 || samples==0 || length<=0) {
        return false;
    }

    short *pInputbuffer = (short*)samples;
    const int numSamplesPerChannel = BytesToSamples(length, m_srcChannels);
    const int rc = faacEncEncode((faacEncHandle)m_handle,
                           (int*)pInputbuffer,
                           numSamplesPerChannel*m_srcChannels,
                           m_buffer,
                           m_aacFrameMaxSize);
    if (rc< 0) {
        return false;
    }
    if (m_callback!=0) {
//    if (m_callback!=0 && rc>0) {
        m_callback->onEncodeOk(m_buffer, rc, param);
    }
    return true;
}

void EbAacEncoder::stop()
{
    if (m_handle!=0) {
        faacEncClose((faacEncHandle)m_handle);
        m_handle = 0;
    }
    if (m_buffer!=0) {
        delete[] m_buffer;
        m_buffer = 0;
    }
    m_callback = 0;
}

int constThreshold = 7;
float constHitratioHigh = 0.6;
float constHitratioLow = 0.11;
int constNoiseCons = 40;	/// 50
int constVoiceCons = 5;		/// 5
unsigned long EbAacEncoder::DetectSilence(const unsigned char *data, unsigned long size)
{
    static   bool   bSpeakingState   =   true; ///   指明目前的状态是通话还是静默
    static   unsigned   int   nConsecutive   =   0; ///   用来计算连续出现的某种类型的包

    bool   bNoisePacket; ///   当前检测的语音包类型（判断）
    long   hit   =   0; ///   要来计算语音包中符合条件的位数。long足够了

    unsigned   int   i; ///   循环变量

    ///   推测当前语音包类型
    for (i=0; i<size;i++) {
        if ((data[i]<= 0 +constThreshold)
            ||   (data[i]   >=   255   -   constThreshold))
            hit++;
    }

    const float ratio = (float)hit/size;
    if   (ratio > constHitratioHigh || ratio < constHitratioLow || ratio == 0.5 || ratio == 1.0)
    {
        bNoisePacket = true;
    }else
    {
        bNoisePacket = false;
    }

    if (bSpeakingState)                     ///   如果目前是通话状态
    {
        if (false==bNoisePacket) {  ///   如果目前的包断定为语音包
            nConsecutive   =   0;   ///   计数清零
            return   size;
        }
        else { ///   如果目前是静音包
            nConsecutive++;
            if (constNoiseCons < nConsecutive) {   ///   如果已经连续出现了足够的静音包
                nConsecutive   =   0   ; //   计数清零
                //TRACE("从通话状态转化到静音状态！**********\n");
                bSpeakingState   =   false; ///   转换到静音状态
                return 0;
            }
            else {
                return   size; ///   仍然提交
            }
        }
    }
    else { ///   如果当前是静音状态
        if (bNoisePacket) { ///   如果目前的包断定为静音包
            nConsecutive   =   0; ///   计数清零
            return 0;
        }
        else {  ///   如果目前是语音包
            nConsecutive++;
            if (constVoiceCons<(int)nConsecutive) {   ///   如果已经连续出现了足够的语音包
                nConsecutive   =   0   ; ///   计数清零
                //TRACE("*********从静音状态转化到通话状态！\n");
                bSpeakingState   =   true; ///   转换到静音状态
                return size;
            }
            else {
                return   0; ///   否则仍然丢弃
            }
        }
    }

}
