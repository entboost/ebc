#include "ebaacdecoder.h"
#include "string.h"
#include "neaacdec.h"

#define DEFAULT_SAMPLERATE 16000					// 11025 12000 16000* 22050 24000, 32000, 44100, 48000, 64000, 88200, 96000

EbAacDecoder::EbAacDecoder(EbAacDecoderCallback *callback)
    : m_handle(0)
    , m_callback(callback)
    , m_samplesPerFrame(1024)
    , m_srcSampleRate(DEFAULT_SAMPLERATE)
    , m_srcChannels(1)
{

}

EbAacDecoder::~EbAacDecoder()
{
    stop();
}

bool EbAacDecoder::init(const unsigned char *data, unsigned int size)
{
    if (m_handle!=0) {
        return true;
    }
    if (data==0 || size<=0) {
        return false;
    }
    unsigned long srate;
    unsigned char chan;

    NeAACDecHandle m_faadHandle = faacDecOpen();
    if( m_faadHandle == 0) {
        return false;
    }

    /* Set default config. */
    faacDecConfigurationPtr	config=faacDecGetCurrentConfiguration(m_faadHandle);
    config->defObjectType=LC;
    config->outputFormat=FAAD_FMT_16BIT;
    config->defSampleRate=DEFAULT_SAMPLERATE;
    config->downMatrix=0;
    config->useOldADTSFormat=0;
    //config->dontUpSampleImplicitSBR=1;
    config->dontUpSampleImplicitSBR=0; //not use SBR.
    faacDecSetConfiguration(m_faadHandle, config);

    const int ret = faacDecInit2(m_faadHandle, (unsigned char *)data, size, &srate, &chan);
    if( ret != 0) {
        faacDecClose(m_faadHandle);
        return false;
    }

    mp4AudioSpecificConfig mp4ASC;
    int m_samplesPerFrame = 1024;
    if (AudioSpecificConfig((unsigned char *)data,
                            size,
                            &mp4ASC)) {
        if (mp4ASC.frameLengthFlag) {
            m_samplesPerFrame = 960;
        }
    }
    m_handle = (void*)m_faadHandle;
    m_srcSampleRate = srate;
    m_srcChannels   = chan;
    return true;
}

bool EbAacDecoder::isInited() const
{
    return m_handle==0?false:true;
}

int EbAacDecoder::decode(const unsigned char *data, unsigned int size, unsigned long param)
{
    if (m_handle==0) {
        return 0;
    }

    if (data==0 || size==0) {
        return 0;
    }

    faacDecFrameInfo frame_info;
    unsigned int bytes_consummed = size;
    unsigned char *buff = (unsigned char *)faacDecDecode (
                (NeAACDecHandle)m_handle, &frame_info, (unsigned char *)data, size);
    if (buff!=0) {
        bytes_consummed = frame_info.bytesconsumed;
        if (frame_info.channels == 0) {
            return bytes_consummed;
        }

//        if (frame_info.num_lfe_channels > 0) {
//            m_srcChannels = 6;
//        }
//        else {
//            m_srcChannels = frame_info.num_front_channels + frame_info.num_back_channels;
//        }
//        m_srcSampleRate = frame_info.samplerate;
        if (frame_info.samples != 0) {
            if (m_srcChannels <=2) {
                if (m_callback!=0) {
                    const int m_buffersize = frame_info.samples*sizeof(short);   /// 2:sizeof (short)
//                    memcpy(m_buffer, buff, m_buffersize);
//                    m_callback->onDecodeOk((const unsigned char*)m_buffer, m_buffersize, param);
                    m_callback->onDecodeOk((const unsigned char*)buff, m_buffersize, param);
                }
            }
        }
        return bytes_consummed;
    }
    return 0;
}

void EbAacDecoder::stop()
{
    if (m_handle!=0) {
        faacDecClose ((NeAACDecHandle)m_handle);
        m_handle = 0;
    }
}
