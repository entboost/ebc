#include "ebffmpegproxy.h"

EbFfmpegProxy::EbFfmpegProxy()
    : m_context(0)
    , m_frameFfmpep(0)
    , m_frameUser(0)
    , m_swsContext(0)
    , m_callback(0)
    , m_bitRate(0)
    , m_width(320)
    , m_height(240)
    , m_keyFrameInterval(10)
    , m_frameCount(0)
    , m_format(AV_PIX_FMT_RGBA)
    , m_videoBitCount(0)
    , m_sampleRate(16000)
    , m_audioChannels(1)
    , m_bufferData(0)
    , m_bufferSize(0)
{

}

EbFfmpegProxy::~EbFfmpegProxy()
{
    close();
    if (m_bufferData!=0) {
        delete[] m_bufferData;
        m_bufferData = 0;
    }
}

void EbFfmpegProxy::setVideoSize(int width, int height)
{
    this->m_width = width;
    this->m_height = height;
}
/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}
/* just pick the highest supported samplerate */
//static int select_sample_rate(const AVCodec *codec)
//{
//    const int *p;
//    int best_samplerate = 0;

//    if (!codec->supported_samplerates)
//        return 44100;

//    p = codec->supported_samplerates;
//    while (*p) {
//        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
//            best_samplerate = *p;
//        p++;
//    }
//    return best_samplerate;
//}

/* select layout with the highest channel count */
static int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

void EbFfmpegProxy::setFormat(enum AVPixelFormat format)
{
    m_format = format;
    switch (m_format) {
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_BGRA:
        m_videoBitCount = 32;
        break;
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
        m_videoBitCount = 24;
        break;
    default:
        m_videoBitCount = 0;
        break;
    }
}

bool EbFfmpegProxy::open(AVCodecID id, bool encoder)
{
    if (m_context!=0) {
        return true;
    }
    int ret = 0;
    const AVCodec *codec;
    avcodec_register_all();

    if (encoder)
        codec = avcodec_find_encoder(id);
    else
        codec = avcodec_find_decoder(id);
    if (!codec) {
        return false;
    }

    m_context = avcodec_alloc_context3(codec);
    if (!m_context) {
        return false;
    }

    if (codec->type == AVMEDIA_TYPE_VIDEO) {
        if (m_bitRate==0) {
            m_bitRate = 600000;
        }

        m_context->pix_fmt = AV_PIX_FMT_YUV420P;
        if (encoder) {
//            m_context->rc_min_rate = ;  /// 最小码率
//            m_context->rc_max_rate = ;  /// 最大码率
            m_context->width = this->m_width;
            m_context->height = this->m_height;
            m_context->time_base.num = 1;
            m_context->time_base.den = 25;
            m_context->framerate.num = 25;
            m_context->framerate.den = 1;
            m_context->gop_size = 100;
            m_context->delay = 0;
            m_context->max_b_frames = 10;
//            m_context->i_quant_factor = 0.8;    /// i_quant_factor
//            m_context->i_quant_offset = 1;
//            m_context->global_quality = global_quality;
//            m_context->compression_level = global_quality;

//            /// for test
//            m_context->sample_aspect_ratio.num = 25;
//            m_context->sample_aspect_ratio.den = 1;
//            m_context->gop_size = 240;
//            /// OK
//            m_context->qmin = 10;
//            m_context->qmax = 51;
//            m_context->max_qdiff = 4;
//            m_context->me_range = 16;
            m_context->keyint_min = m_keyFrameInterval; /// 最小关键帧间隔
//            m_context->qcompress = 0.6;
//            m_context->me_method = ME_FULL;

            if (codec->id == AV_CODEC_ID_H264) {
//                av_opt_set(m_context->priv_data, "preset", "slow", 0);
                av_opt_set(m_context->priv_data, "preset", "superfast", 0);
                av_opt_set(m_context->priv_data, "tune", "zerolatency", 0);
            }
        }
        else {
            /// ** for decode video only
            if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
                /// we do not send complete frames
                m_context->flags |= AV_CODEC_FLAG_TRUNCATED;
            }
        }
    }
    else if (codec->type == AVMEDIA_TYPE_AUDIO) {
        if (m_bitRate==0) {
            m_bitRate = 64000;
        }
        m_context->sample_fmt = AV_SAMPLE_FMT_FLTP; /// AAC
        if (!check_sample_fmt(codec, m_context->sample_fmt)) {
            close();
            return false;
        }
        /* select other audio parameters supported by the encoder */
        m_context->sample_rate    = m_sampleRate;///select_sample_rate(codec);
//        m_context->channel_layout = 1;//select_channel_layout(codec);
        m_context->channels       = m_audioChannels;///av_get_channel_layout_nb_channels(m_context->channel_layout);
    }
    m_context->bit_rate = this->m_bitRate;
    AVCodecContext * test = m_context;

    /* open it */
    ret = avcodec_open2(m_context, codec, NULL);
    if (ret<0) {
        close();
        char buf[256];
        av_strerror(ret, buf, 256);
        return false;
    }

    m_frameFfmpep = av_frame_alloc();
    if (!m_frameFfmpep) {
        close();
        return false;
    }

    if (codec->type == AVMEDIA_TYPE_VIDEO) {
        m_frameFfmpep->format = m_context->pix_fmt;
        if (encoder) {
            m_frameFfmpep->width  = m_context->width;
            m_frameFfmpep->height = m_context->height;
            ret = av_frame_get_buffer(m_frameFfmpep, 32);
            if (ret < 0) {
                close();
                return false;
            }
        }
    }
    else if (codec->type == AVMEDIA_TYPE_AUDIO) {
        m_frameFfmpep->nb_samples     = m_context->frame_size;
        m_frameFfmpep->format         = m_context->sample_fmt;
        m_frameFfmpep->channel_layout = m_context->channel_layout;
        /// 解码需要 buffer ，编码不需要
        if (!encoder) {
            ret = av_frame_get_buffer(m_frameFfmpep, 0);
            if (ret < 0) {
                close();
                return false;
            }
        }
    }

    return true;
}

bool EbFfmpegProxy::isOpened() const
{
    return m_context==0?false:true;
}

void EbFfmpegProxy::close()
{
    if (m_context!=0) {
        avcodec_free_context(&m_context);
        m_context = 0;
    }
    if (m_frameFfmpep!=0) {
        av_frame_free(&m_frameFfmpep);
        m_frameFfmpep = 0;
    }
    if (m_frameUser!=0) {
        av_frame_free(&m_frameUser);
    }
    if (m_swsContext!=0) {
        sws_freeContext(m_swsContext);
        m_swsContext = 0;
    }
}

#define USES_SEND_FRAME
bool EbFfmpegProxy::encodeVideo(const unsigned char *data,
                                unsigned char *destBuffer, int bufferSize, int *decodeSize, bool *pOutKeyFrame)
{
    if (!isOpened() || data==0) {
        return false;
    }

    int ret = 0;
    int got_output = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;    /// packet data will be allocated by the encoder
    pkt.size = 0;

    if (m_frameUser==0) {
        /// 解码前的帧
        m_frameUser = av_frame_alloc();
        m_frameUser->format  = m_format;
        m_frameUser->width  = m_context->width;
        m_frameUser->height = m_context->height;
        ret = av_frame_get_buffer(m_frameUser, 32);
        if (ret < 0) {
            return false;
        }
    }
    if (m_swsContext==0) {
        m_swsContext = sws_getContext(
                    m_context->width,
                    m_context->height,
                    (AVPixelFormat)m_frameUser->format,
                    m_context->width,
                    m_context->height,
                    (AVPixelFormat)m_frameFfmpep->format,
                    SWS_BICUBIC,
                    NULL, NULL, NULL);
    }
    ret = av_image_fill_arrays(m_frameUser->data, m_frameUser->linesize, data,
                               (AVPixelFormat)m_frameUser->format, m_context->width, m_context->height, 1);
    if (ret<0) {
        return false;
    }
    /// RGB32->x
    ret = sws_scale(m_swsContext, m_frameUser->data, m_frameUser->linesize, 0,
              m_context->height, m_frameFfmpep->data, m_frameFfmpep->linesize);
    if (ret<0) {
        return false;
    }
    m_frameFfmpep->pts = (++m_frameCount);
    /* encode the image */
#ifdef USES_SEND_FRAME
    ret = avcodec_send_frame(m_context, m_frameFfmpep);
    if (ret < 0) {
//        char buf[256];
//        av_strerror(ret, buf, 256);
        return false;
    }
    ret = avcodec_receive_packet(m_context, &pkt);
    if (ret < 0) {
        return false;
    }
    if (decodeSize!=0) {
        *decodeSize = pkt.size;
    }
    if (destBuffer!=0 && bufferSize>0) {
        memcpy(destBuffer, pkt.data, MIN(pkt.size, bufferSize));
    }
    if (pOutKeyFrame!=0) {
        *pOutKeyFrame = m_frameFfmpep->key_frame==1?true:false;
    }
    if (m_callback!=0) {
        m_callback->onVideoEncodeData(pkt.data, pkt.size);
    }
    av_packet_unref(&pkt);
#else
    const AVFrame * from = m_frameFfmpep;
    ret = avcodec_encode_video2(m_context, &packet, m_frameFfmpep, &got_output);
    if (ret < 0) {
        char buf[256];
        av_strerror(ret, buf, 256);
        return false;
    }

    if (got_output) {
//            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
//            fwrite(pkt.data, 1, pkt.size, f);
        callback->onVideoEncodeData(packet.data, packet.size);
        av_packet_unref(&packet);
    }

    /* get the delayed frames */
    int i=0;
    for (got_output = 1; got_output; i++) {
        ret = avcodec_encode_video2(m_context, &packet, NULL, &got_output);
        if (ret < 0) {
            return false;
        }

        if (got_output) {
            callback->onVideoEncodeData(packet.data, packet.size);
            av_packet_unref(&packet);
        }
    }
#endif /// USES_SEND_FRAME
    return true;
}


#define USES_RECEIVE_FRAME    /// 有问题
bool EbFfmpegProxy::decodeVideo(const unsigned char *data, int size,
                                unsigned char *destBuffer, int bufferSize, int *decodeSize, bool *pOutKeyFrame)
{
    if (!isOpened()) {
        return false;
    }

    int ret = 0;
    AVPacket avpkt;
    av_init_packet(&avpkt);

    /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
       and this is the only method to use them because you cannot
       know the compressed data size before analysing it.

       BUT some other codecs (msmpeg4, mpeg4) are inherently frame
       based, so you must call them with all the data for one
       frame exactly. You must also initialize 'width' and
       'height' before initializing them. */

    /* NOTE2: some codecs allow the raw parameters (frame size,
       sample rate) to be changed at any frame. We handle this, so
       you should also take care of it */

    /* here, we use a stream based decoder (mpeg1video), so we
       feed decoder and see if it could decode a frame */
    if (!fillAVPacket(data, size, &avpkt)) {
        return false;
    }
#ifdef USES_RECEIVE_FRAME
    ret = avcodec_send_packet(m_context, &avpkt);
    if (ret<0) {
        return false;
    }
    ret = avcodec_receive_frame(m_context, m_frameFfmpep);
    if (ret<0) {
        return false;
    }
    if (this->m_width!=m_frameFfmpep->width) {
        this->m_width = m_frameFfmpep->width;
    }
    if (this->m_height!=m_frameFfmpep->height) {
        this->m_height = m_frameFfmpep->height;
    }
    m_frameCount++;

    if (m_frameUser==0) {
        /// 解码后的帧
        m_frameUser = av_frame_alloc();
        m_frameUser->format  = m_format;
        m_frameUser->width  = m_context->width;
        m_frameUser->height = m_context->height;
        ret = av_frame_get_buffer(m_frameUser, 32);
        if (ret < 0) {
            avpkt.data = NULL;
            avpkt.size = 0;
            return false;
        }
    }
    if (m_swsContext==0) {
        m_swsContext = sws_getContext(
            m_context->width,
            m_context->height,
            (AVPixelFormat)m_frameFfmpep->format,
            m_context->width,
            m_context->height,
            (AVPixelFormat)m_frameUser->format,
            SWS_BICUBIC,
            NULL, NULL, NULL);
    }
    /// x->RGB32
    sws_scale(m_swsContext, m_frameFfmpep->data, m_frameFfmpep->linesize, 0,
              m_context->height, m_frameUser->data, m_frameUser->linesize);

    if (pOutKeyFrame!=0) {
        *pOutKeyFrame = m_frameFfmpep->key_frame==1?true:false;
    }
    const int nOutLen = (this->m_width*this->m_height * m_videoBitCount)/8;
    if (decodeSize!=0) {
        *decodeSize = nOutLen;
    }
    if (destBuffer!=0 && bufferSize>0) {
        memcpy(destBuffer, m_frameUser->data[0], MIN(nOutLen, bufferSize));
    }
    if (m_callback!=0) {
        m_callback->onVideoDecodeData(m_frameUser->data[0], nOutLen);
    }
#else
    int len, got_frame;
    while (avpkt.size > 0) {
        len = avcodec_decode_video2(m_context, m_frameFfmpep, &got_frame, &avpkt);
        if (len < 0) {
            return false;
        }
        if (got_frame) {
            /* the picture is allocated by the decoder, no need to free it */
            if (this->m_width!=m_frameFfmpep->width) {
                this->m_width = m_frameFfmpep->width;
            }
            if (this->m_height!=m_frameFfmpep->height) {
                this->m_height = m_frameFfmpep->height;
            }
            m_frameCount++;

            if (m_frameUser==0) {
                /// 解码后的帧
                m_frameUser = av_frame_alloc();
                m_frameUser->format  = m_format;
                m_frameUser->width  = m_context->width;
                m_frameUser->height = m_context->height;
                ret = av_frame_get_buffer(m_frameUser, 32);
                if (ret < 0) {
                    avpkt.data = NULL;
                    avpkt.size = 0;
                    return false;
                }
            }
            if (m_swsContext==0) {
                m_swsContext = sws_getContext(
                    m_context->width,
                    m_context->height,
                    (AVPixelFormat)m_frameFfmpep->format,
                    m_context->width,
                    m_context->height,
                    (AVPixelFormat)m_frameUser->format,
                    SWS_BICUBIC,
                    NULL, NULL, NULL);
            }
            /// 移动数组指针，实现图像翻转；
//            unsigned char *data[4];
//            int linesize[4];
//            int i;
//            const bool reversal = false;
//            if (reversal) {
//                /// 先记录数组指针
//                for(i = 0; i < 4; i++) {
//                    data[ i ] = m_frameFfmpep->data[ i ];
//                    linesize[ i ] = m_frameFfmpep->linesize[ i ];
//                }
//                const int height = m_context->height;
//                m_frameFfmpep->data[0] += m_frameFfmpep->linesize[0] * (height - 1);
//                m_frameFfmpep->linesize[0] *= -1;
//                m_frameFfmpep->data[1] += m_frameFfmpep->linesize[1] * (height / 2 - 1);
//                m_frameFfmpep->linesize[1] *= -1;
//                m_frameFfmpep->data[2] += m_frameFfmpep->linesize[2] * (height / 2 - 1);
//                m_frameFfmpep->linesize[2] *= -1;
//            }
            /// x->RGB32
            sws_scale(m_swsContext, m_frameFfmpep->data, m_frameFfmpep->linesize, 0,
                      m_context->height, m_frameUser->data, m_frameUser->linesize);
            /// 恢复数组指针，避免发生内存泄漏；
//            if (reversal) {
//                for(i = 0; i < 4; i++) {
//                    m_frameFfmpep->data[ i ] = data[ i ];
//                    m_frameFfmpep->linesize[ i ] = linesize[ i ];
//                }
//            }
            if (pOutKeyFrame!=0) {
                *pOutKeyFrame = m_frameFfmpep->key_frame==1?true:false;
            }
            const int nOutLen = (this->m_width*this->m_height * m_videoBitCount)/8;
            if (decodeSize!=0) {
                *decodeSize = nOutLen;
            }
            if (destBuffer!=0 && bufferSize>0) {
                memcpy(destBuffer, m_frameUser->data[0], MIN(nOutLen, bufferSize));
            }
            if (m_callback!=0) {
                m_callback->onVideoDecodeData(m_frameUser->data[0], nOutLen);
            }
        }
        if (avpkt.data) {
            avpkt.size -= len;
            avpkt.data += len;
        }
    }
#endif

    /* Some codecs, such as MPEG, transmit the I- and P-frame with a
   latency of one frame. You must do the following to have a
   chance to get the last frame of the video. */
    avpkt.data = NULL;
    avpkt.size = 0;
    return true;
}

bool EbFfmpegProxy::encodeAudio(const unsigned char *data, int size)
{
    if (!isOpened() || data==0) {
        return false;
    }

    int ret = 0;
    int got_output = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;    /// packet data will be allocated by the encoder
    pkt.size = 0;

    m_frameFfmpep->data[0] = (uint8_t*)data;  /// PCM Data
    m_frameFfmpep->linesize[0] = size;

    /* encode the samples */
//    ret = avcodec_send_frame(m_context, m_frameFfmpep);
//    if (ret < 0) {
//        return false;
//    }
//    ret = avcodec_receive_packet(m_context, &pkt);
//    if (ret < 0) {
//        return false;
//    }
//    callback->onAudioEncodeData(pkt.data, pkt.size);
//    av_packet_unref(&pkt);
    ret = avcodec_encode_audio2(m_context, &pkt, m_frameFfmpep, &got_output);
    if (ret < 0) {
        return false;
    }
    if (got_output) {
        if (m_callback!=0)
            m_callback->onAudioEncodeData(pkt.data, pkt.size);
        av_packet_unref(&pkt);
    }
    m_frameFfmpep->data[0] = 0;
    m_frameFfmpep->linesize[0] = 0;
    return true;
}

bool EbFfmpegProxy::decodeAudio(const unsigned char *data, int size)
{
    if (!isOpened() || data==0) {
        return false;
    }

    int ret = 0;
    AVPacket avpkt;
    av_init_packet(&avpkt);

    avpkt.data = (uint8_t*)data;
    avpkt.size = size;

    int i, ch;
    int len, got_frame;
    while (avpkt.size > 0) {
        len = avcodec_decode_audio4(m_context, m_frameFfmpep, &got_frame, &avpkt);
        if (len < 0) {
            return false;
        }
        if (got_frame) {
//            m_frameCount++;
            /* if a frame has been decoded, output it */
            int data_size = av_get_bytes_per_sample(m_context->sample_fmt);
            if (data_size < 0) {
                avpkt.data = NULL;
                avpkt.size = 0;
                return false;
            }
            for (i=0; i<m_frameFfmpep->nb_samples; i++)
                for (ch=0; ch<m_context->channels; ch++) {
                    if (m_callback!=0)
                        m_callback->onAudioDecodeData(m_frameFfmpep->data[ch] + data_size*i, data_size);
                }
        }
        if (avpkt.data) {
            avpkt.size -= len;
            avpkt.data += len;
        }
    }

    avpkt.data = NULL;
    avpkt.size = 0;
    return true;
}

bool EbFfmpegProxy::fillAVPacket(const unsigned char *data, int size, AVPacket *avpkt)
{
    if (data==0 || size<=0) {
        return false;
    }
    if (m_bufferData==0) {
        m_bufferSize = size+4096;
        m_bufferData = new unsigned char[m_bufferSize];
    }
    else if (m_bufferSize<size) {
        delete[] m_bufferData;
        m_bufferSize = size+2048;
        m_bufferData = new unsigned char[m_bufferSize];
    }
    if (m_bufferData==0) {
        return false;
    }
    memcpy(m_bufferData, data, size);
    memset(m_bufferData+size, 0, 1);
    avpkt->data = (uint8_t*)m_bufferData;
    avpkt->size = size;
    return true;
}

bool EbFfmpegProxy::fillBufferData(int requestSize)
{
    if (m_bufferData==0) {
        m_bufferSize = requestSize+1024;
        m_bufferData = new unsigned char[m_bufferSize];
    }
    else if (m_bufferSize<requestSize) {
        delete[] m_bufferData;
        m_bufferSize = requestSize+1024;
        m_bufferData = new unsigned char[m_bufferSize];
    }
    if (m_bufferData==0) {
        return false;
    }
    return true;
}
