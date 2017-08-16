#include "ebabstractvideosurface.h"

#ifdef __MACH__
#include "mach_uses.h"
#endif
#ifdef WIN32
#include <CGCLib/CGCLib.h>
#include <Mmsystem.h>
#else
#include <time.h>
inline unsigned long timeGetTime()
{
    unsigned long uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
        uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    return uptime;
}
#define _FILE_OFFSET_BITS 64
#endif

EbAbstractVideoSurface::EbAbstractVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_started(false)
    , m_videoFormat(QVideoFrame::Format_Invalid)
    , m_imageFormat(QImage::Format_Invalid)
    , m_fps(10)
    , m_lastTime(0)
    , m_videoProbe(0)
    , m_sizeFrame(320, 240)
    , m_bytesPerLine(320*3)
    , m_swsContext(0)
    , m_frameFrom(0)
    , m_frameTo(0)
{
    m_buffer = new uchar[2048];
}

EbAbstractVideoSurface::~EbAbstractVideoSurface()
{
    releaseConvert();
    delete[] m_buffer;
}

bool EbAbstractVideoSurface::setSource(QCamera *pCamera, const QSize &frameSize)
{
    bool ret = true;
    m_sizeFrame = frameSize;
#ifndef Q_OA_ANDROID
//#ifdef WIN32
       /// windows下,只能用下面方式捕获视频
    pCamera->setViewfinder(this);
#else
    /// android下,目前只能用probe捕获视频
    if (m_videoProbe==0) {
        m_videoProbe = new QVideoProbe(this);
    }
    ret = m_videoProbe->setSource(pCamera);
    if (ret) {
        connect(m_videoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), SLOT(present(QVideoFrame)));
    }
#endif
    return ret;
}

QList<QVideoFrame::PixelFormat> EbAbstractVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    QList<QVideoFrame::PixelFormat> list;
//    list << QVideoFrame::Format_BGRA32;   /// **这个显示颜色会有失真问题
    list << QVideoFrame::Format_RGB32;      /// windows OK
    list << QVideoFrame::Format_RGB24;
    list << QVideoFrame::Format_BGR32;
    list << QVideoFrame::Format_BGR24;
    list << QVideoFrame::Format_ARGB32;     /// mac OK
    list << QVideoFrame::Format_UYVY;   /// 20 mac ok
    list << QVideoFrame::Format_YUYV;   /// 21 mac ok
    list << QVideoFrame::Format_NV12;   /// 22 mac ok
    list << QVideoFrame::Format_NV21;   /// 23 x
//    list << QVideoFrame::Format_YUV420P;
    return list;
}

inline void BGR32_TO_RGB32(unsigned char *data, int width, int height)
{
    /// 转出来跟 FFMPEG 转出来效果是一样的
    unsigned char * buf = data;
    unsigned char tmp = 0;
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            tmp = buf[0];
            buf[0] = buf[2];
            buf[2] = tmp;
            buf += 4;
        }
    }
}
inline void BGR24_TO_RGB24(unsigned char *data, int width, int height)
{
    /// 转出来跟 FFMPEG 转出来效果是一样的
    unsigned char * buf = data;
    unsigned char tmp = 0;
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            tmp = buf[0];
            buf[0] = buf[2];
            buf[2] = tmp;
            buf += 3;
        }
    }
}

bool EbAbstractVideoSurface::start(const QVideoSurfaceFormat &format)
{
    /// 捕获视频帧。windows下格式是RGB32；android下是NV21
    m_videoFormat = format.pixelFormat();
//    const int width = m_sizeFrame.width();
//    const int height = m_sizeFrame.height();
    const int width = format.frameWidth();
    const int height = format.frameHeight();
    if (m_videoFormat==QVideoFrame::Format_UYVY) {
        if (!initConvert(AV_PIX_FMT_UYVY422, AV_PIX_FMT_RGB24, width, height)) {
            return false;
        }
        m_imageFormat = QImage::Format_RGB888;
        m_bytesPerLine = m_sizeFrame.width()*3;
    }
    else if (m_videoFormat==QVideoFrame::Format_YUYV) {
        if (!initConvert(AV_PIX_FMT_YUYV422, AV_PIX_FMT_RGB24, width, height)) {
            return false;
        }
        m_imageFormat = QImage::Format_RGB888;
        m_bytesPerLine = m_sizeFrame.width()*3;
    }
    else if (m_videoFormat==QVideoFrame::Format_NV12) {
        if (!initConvert(AV_PIX_FMT_NV12, AV_PIX_FMT_RGB24, width, height)) {
            return false;
        }
        m_imageFormat = QImage::Format_RGB888;
        m_bytesPerLine = m_sizeFrame.width()*3;
    }
    else if (m_videoFormat==QVideoFrame::Format_NV21) {
        if (!initConvert(AV_PIX_FMT_NV21, AV_PIX_FMT_RGB24, width, height)) {
            return false;
        }
        m_imageFormat = QImage::Format_RGB888;
        m_bytesPerLine = m_sizeFrame.width()*3;
    }
    else if (m_videoFormat==QVideoFrame::Format_RGB32) {
        if (width!=m_sizeFrame.width() || height!=m_sizeFrame.height()) {
            if (!initConvert(AV_PIX_FMT_RGB32, AV_PIX_FMT_RGB32, width, height)) {
                return false;
            }
            m_bytesPerLine = m_sizeFrame.width()*4;
        }
        m_imageFormat = QImage::Format_RGB32;
    }
    else if (m_videoFormat==QVideoFrame::Format_RGB24) {
        if (width!=m_sizeFrame.width() || height!=m_sizeFrame.height()) {
            if (!initConvert(AV_PIX_FMT_RGB24, AV_PIX_FMT_RGB24, width, height)) {
                return false;
            }
            m_bytesPerLine = m_sizeFrame.width()*3;
        }
        m_imageFormat = QImage::Format_RGB888;
    }
    else if (m_videoFormat==QVideoFrame::Format_ARGB32) {
        if (width!=m_sizeFrame.width() || height!=m_sizeFrame.height()) {
            if (!initConvert(AV_PIX_FMT_RGB32, AV_PIX_FMT_RGB32, width, height)) {
                return false;
            }
            m_bytesPerLine = m_sizeFrame.width()*4;
        }
        m_imageFormat = QImage::Format_ARGB32;
    }
    else if (m_videoFormat==QVideoFrame::Format_BGRA32) {
        if (width!=m_sizeFrame.width() || height!=m_sizeFrame.height()) {
            /// ?
            if (!initConvert(AV_PIX_FMT_BGR32, AV_PIX_FMT_BGR32, width, height)) {
                return false;
            }
            m_bytesPerLine = m_sizeFrame.width()*4;
        }
        // ?
        m_imageFormat = QImage::Format_RGB32;
    }
    else if (m_videoFormat==QVideoFrame::Format_BGR24) {
        if (width!=m_sizeFrame.width() || height!=m_sizeFrame.height()) {
            /// ?
            if (!initConvert(AV_PIX_FMT_BGR24, AV_PIX_FMT_BGR24, width, height)) {
                return false;
            }
            m_bytesPerLine = m_sizeFrame.width()*3;
        }
        // ?
        m_imageFormat = QImage::Format_RGB888;
    }
    else {
        m_imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_videoFormat);
    }

    m_started = true;
    const QSize size = format.frameSize();
    if (m_imageFormat != QImage::Format_Invalid && size.isValid()) {
        return QAbstractVideoSurface::start(format);
    }
    else {
        releaseConvert();
        return false;
    }
}

//void EbAbstractVideoSurface::stop()
//{
//    m_currentVideoFrame = QVideoFrame();
//    QAbstractVideoSurface::stop();
//}
bool EbAbstractVideoSurface::present(const QVideoFrame &frame)
{
    /// 处理捕获的帧
    const unsigned long timestamp = timeGetTime();
    if ( m_fps != 0) {
        /// 判断 FPS 时间间隔
        const float fpsTimerInter = (float)1000.0/(float)m_fps;
        const float now = (float)timestamp;
        if ((now - m_lastTime) < fpsTimerInter) {
            /// FPS 时间间隔未到，不处理当前图像
            return true;
        }
        m_lastTime = now;
    }

    if (frame.isMapped()) {
        setVideoFrame(frame, timestamp);
    }
    else {
        QVideoFrame f(frame);
        if (f.map(QAbstractVideoBuffer::ReadOnly)) {
            setVideoFrame(f, timestamp);
        }
    }
    return true;
}

bool EbAbstractVideoSurface::initConvert(AVPixelFormat from, AVPixelFormat to, int width, int height)
{
    int ret = 0;
    if (m_swsContext==0) {
        m_swsContext = sws_getContext(
                    width,
                    height,
                    from,
                    m_sizeFrame.width(),
                    m_sizeFrame.height(),
                    to,
                    SWS_BICUBIC,
                    NULL, NULL, NULL);
    }
    if (m_frameFrom==0) {
        m_frameFrom = av_frame_alloc();
        m_frameFrom->format  = from;
        m_frameFrom->width  = width;
        m_frameFrom->height = height;
        ret = av_frame_get_buffer(m_frameFrom, 32);
        if (ret<0) {
            av_frame_free(&m_frameFrom);
            return false;
        }
    }
    if (m_frameTo==0) {
        m_frameTo = av_frame_alloc();
        m_frameTo->format  = to;
        m_frameTo->width  = m_sizeFrame.width();
        m_frameTo->height = m_sizeFrame.height();
        ret = av_frame_get_buffer(m_frameTo, 32);
        if (ret<0) {
            av_frame_free(&m_frameTo);
            return false;
        }
    }
    return true;
}

void EbAbstractVideoSurface::releaseConvert()
{
    if (m_frameFrom!=0) {
        av_frame_free(&m_frameFrom);
    }
    if (m_frameTo!=0) {
        av_frame_free(&m_frameTo);
    }
    if (m_swsContext!=0) {
        sws_freeContext(m_swsContext);
        m_swsContext = 0;
    }
}

void EbAbstractVideoSurface::convertSendFrame(const QVideoFrame &frame, unsigned long timestamp)
{
    int ret = 0;
    if (!frame.isMapped()) return;
    if (m_frameFrom==0 || m_frameTo==0) return;

    const uchar *bits = frame.bits();
    ret = av_image_fill_arrays(m_frameFrom->data, m_frameFrom->linesize, bits,
                               (AVPixelFormat)m_frameFrom->format, frame.width(), frame.height(), 1);
    if (ret<0) {
        return;
    }
    ret = sws_scale(m_swsContext, m_frameFrom->data, m_frameFrom->linesize, 0,
                    frame.height(), m_frameTo->data, m_frameTo->linesize);
    if (ret<0) {
        return;
    }
    emit captureVideoFrame(m_frameTo->data[0], m_sizeFrame.height()*m_bytesPerLine, m_sizeFrame.width(), m_sizeFrame.height(), timestamp);
}

void EbAbstractVideoSurface::setVideoFrame(const QVideoFrame &frame, unsigned long timestamp)
{
    if (!frame.isMapped()) {
        return;
    }
    if (m_videoFormat==QVideoFrame::Format_YUYV ||
            m_videoFormat==QVideoFrame::Format_UYVY ||
            m_videoFormat==QVideoFrame::Format_NV12 ||
            m_videoFormat==QVideoFrame::Format_NV21) {
        convertSendFrame(frame, timestamp);
        return;
    }
    else if (frame.width()!=m_sizeFrame.width() || frame.height()!=m_sizeFrame.height()) {
        convertSendFrame(frame, timestamp);
        return;
    }

    const uchar *bits = frame.bits();
//    if (m_videoFormat==QVideoFrame::Format_NV12) {

//        QVideoFrame fromNew;
//        static bool theSave = false;
//        if (!theSave) {
//            theSave = true;


//            AVPixelFormat pixemFormatFrom = AV_PIX_FMT_NV12;
//            AVPixelFormat pixemFormatTo = AV_PIX_FMT_RGB24;
//            int ret = 0;
////            m_swsContext = sws_getContext(
////                        frame.width(),
////                        frame.height(),
////                        pixemFormatFrom,
////                        frame.width(),
////                        frame.height(),
////                        pixemFormatTo,
////                        SWS_BICUBIC,
////                        NULL, NULL, NULL);

////            m_frameFrom = av_frame_alloc();
////            m_frameFrom->format  = pixemFormatFrom;
////            m_frameFrom->width  = frame.width();
////            m_frameFrom->height = frame.height();
////            ret = av_frame_get_buffer(m_frameFrom, 32);
////            ret = av_image_fill_arrays(m_frameFrom->data, m_frameFrom->linesize, bits,
////                                       (AVPixelFormat)m_frameFrom->format, frame.width(), frame.height(), 1);


////            m_frameTo = av_frame_alloc();
////            m_frameTo->format  = pixemFormatTo;
////            m_frameTo->width  = frame.width();
////            m_frameTo->height = frame.height();
////            ret = av_frame_get_buffer(m_frameTo, 32);

//            /// RGB32->x
//            ret = sws_scale(m_swsContext, m_frameFrom->data, m_frameFrom->linesize, 0,
//                            frame.height(), m_frameTo->data, m_frameTo->linesize);

//            QImage m_currentImage = QImage(
//                        m_frameTo->data[0],
//                    frame.width(),
//                    frame.height(),
//                    frame.width()*3,
//                    QImage::Format_RGB888);
//            m_currentImage.save("/Users/akee/Desktop/mac_videocapture_nv12.png");


//        }

//    }
//    else if (m_videoFormat==QVideoFrame::Format_YUYV) {

//        QVideoFrame fromNew;
//        static bool theSave = false;
//        if (!theSave) {
//            theSave = true;


//            AVPixelFormat pixemFormatFrom = AV_PIX_FMT_YUYV422;
//            AVPixelFormat pixemFormatTo = AV_PIX_FMT_RGB24;
//            int ret = 0;
//            m_swsContext = sws_getContext(
//                        frame.width(),
//                        frame.height(),
//                        pixemFormatFrom,
//                        frame.width(),
//                        frame.height(),
//                        pixemFormatTo,
//                        SWS_BICUBIC,
//                        NULL, NULL, NULL);

//            AVFrame *m_frameFrom;        /// encoder=src; decoder=dest
//            m_frameFrom = av_frame_alloc();
//            m_frameFrom->format  = pixemFormatFrom;
//            m_frameFrom->width  = frame.width();
//            m_frameFrom->height = frame.height();
//            ret = av_frame_get_buffer(m_frameFrom, 32);
//            ret = av_image_fill_arrays(m_frameFrom->data, m_frameFrom->linesize, bits,
//                                       (AVPixelFormat)m_frameFrom->format, frame.width(), frame.height(), 1);


//            AVFrame *m_frameTo;        /// encoder=src; decoder=dest
//            m_frameTo = av_frame_alloc();
//            m_frameTo->format  = pixemFormatTo;
//            m_frameTo->width  = frame.width();
//            m_frameTo->height = frame.height();
//            ret = av_frame_get_buffer(m_frameTo, 32);

//            /// RGB32->x
//            ret = sws_scale(m_swsContext, m_frameFrom->data, m_frameFrom->linesize, 0,
//                            frame.height(), m_frameTo->data, m_frameTo->linesize);

//            QImage m_currentImage = QImage(
//                        m_frameTo->data[0],
//                    frame.width(),
//                    frame.height(),
//                    frame.width()*3,
//                    QImage::Format_RGB888);
//            m_currentImage.save("/Users/akee/Desktop/mac_videocapture_yuyv.png");

//            av_frame_free(&m_frameFrom);
//            av_frame_free(&m_frameTo);
//            if (m_swsContext!=0) {
//                sws_freeContext(m_swsContext);
//                m_swsContext = 0;
//            }
//        }

//    }
//    else if (m_videoFormat==QVideoFrame::Format_UYVY) {

//        QVideoFrame fromNew;
//        static bool theSave = false;
//        if (!theSave) {
//            theSave = true;


//            AVPixelFormat pixemFormatFrom = AV_PIX_FMT_UYVY422;
//            AVPixelFormat pixemFormatTo = AV_PIX_FMT_RGB24;
//            int ret = 0;
//            m_swsContext = sws_getContext(
//                        frame.width(),
//                        frame.height(),
//                        pixemFormatFrom,
//                        frame.width(),
//                        frame.height(),
//                        pixemFormatTo,
//                        SWS_BICUBIC,
//                        NULL, NULL, NULL);

//            AVFrame *m_frameFrom;        /// encoder=src; decoder=dest
//            m_frameFrom = av_frame_alloc();
//            m_frameFrom->format  = pixemFormatFrom;
//            m_frameFrom->width  = frame.width();
//            m_frameFrom->height = frame.height();
//            ret = av_frame_get_buffer(m_frameFrom, 32);
//            ret = av_image_fill_arrays(m_frameFrom->data, m_frameFrom->linesize, bits,
//                                       (AVPixelFormat)m_frameFrom->format, frame.width(), frame.height(), 1);


//            AVFrame *m_frameTo;        /// encoder=src; decoder=dest
//            m_frameTo = av_frame_alloc();
//            m_frameTo->format  = pixemFormatTo;
//            m_frameTo->width  = frame.width();
//            m_frameTo->height = frame.height();
//            ret = av_frame_get_buffer(m_frameTo, 32);

//            /// RGB32->x
//            ret = sws_scale(m_swsContext, m_frameFrom->data, m_frameFrom->linesize, 0,
//                            frame.height(), m_frameTo->data, m_frameTo->linesize);

//            QImage m_currentImage = QImage(
//                        m_frameTo->data[0],
//                    frame.width(),
//                    frame.height(),
//                    frame.width()*3,
//                    QImage::Format_RGB888);
//            m_currentImage.save("/Users/akee/Desktop/mac_videocapture_uyvy.png");

//            av_frame_free(&m_frameFrom);
//            av_frame_free(&m_frameTo);
//            if (m_swsContext!=0) {
//                sws_freeContext(m_swsContext);
//                m_swsContext = 0;
//            }
//        }

//    }
    /// 图像翻转
    const bool reversal = (m_imageFormat==QVideoFrame::Format_ARGB32 ||
                           m_imageFormat==QVideoFrame::Format_RGB32 ||
                           m_imageFormat==QVideoFrame::Format_RGB24)?true:false;
    if (reversal) {
        const int bytesPerLine = frame.bytesPerLine();
//        const int width = frame.width();
        const int height = frame.height();
        for (int i=0; i<height/2; i++) {
            /// 先把第上面行（如第一行），移到临时 buffer
            memcpy(m_buffer, bits+i*bytesPerLine, bytesPerLine);
            /// 把第下面行（如最后一行），移到最上面行
            memcpy((void*)(bits+i*bytesPerLine), bits+(height-i-1)*bytesPerLine, bytesPerLine);
            /// 再把临时 buffer 移到下面行（如最后一行）
            memcpy((void*)(bits+(height-i-1)*bytesPerLine), m_buffer, bytesPerLine);
        }
    }
    emit captureVideoFrame(bits, frame.height()*frame.bytesPerLine(), frame.width(), frame.height(), timestamp);
}
