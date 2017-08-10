#ifndef EBVIDEODEFINE_H
#define EBVIDEODEFINE_H


#define EB_DEFAULT_CHANNELS   1
#define EB_DEFAULT_SAMPLERATE 16000					// 11025 12000 16000（同步） 22050(会不同步)
#define EB_DEFAULT_AUDIO_BITRATE EB_DEFAULT_SAMPLERATE

#define EB_MAX_VIDEO_FRAME_SIZE 320*240*4*2

/// 自定义的RTP载体类型
#define EB_RTP_PAYLOAD_TYPE_RGB  23
#define EB_RTP_PAYLOAD_TYPE_XVID 99
#define EB_RTP_PAYLOAD_TYPE_H264 102

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif // EBVIDEODEFINE_H
