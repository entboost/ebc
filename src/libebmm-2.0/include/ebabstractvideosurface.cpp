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
    , m_imageFormat(QImage::Format_Invalid)
    , m_fps(10)
    , m_lastTime(0)
{
    m_buffer = new uchar[2048];
}

EbAbstractVideoSurface::~EbAbstractVideoSurface()
{
    delete[] m_buffer;
}

QList<QVideoFrame::PixelFormat> EbAbstractVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    QList<QVideoFrame::PixelFormat> list;
    list << QVideoFrame::Format_RGB32;
    list << QVideoFrame::Format_RGB24;
//    list << QVideoFrame::Format_ARGB32;
//    list << QVideoFrame::Format_ARGB32_Premultiplied;
//    list << QVideoFrame::Format_RGB565;
//    list << QVideoFrame::Format_RGB555;
//    list << QVideoFrame::Format_UYVY;
//    list << QVideoFrame::Format_Y8;
    list << QVideoFrame::Format_YUYV;
    list << QVideoFrame::Format_YUV420P;
    return list;
}
bool EbAbstractVideoSurface::start(const QVideoSurfaceFormat &format)
{
    /// 捕获视频帧。windows下格式是RGB32；android下是NV21
    m_videoFormat = format.pixelFormat();
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_videoFormat);
    const QSize size = format.frameSize();
    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->m_imageFormat = imageFormat;
        QAbstractVideoSurface::start(format);
        return true;
    }
    else {
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

void EbAbstractVideoSurface::setVideoFrame(const QVideoFrame &frame, unsigned long timestamp)
{
    if (!frame.isMapped()) {
        return;
    }
    const uchar *bits = frame.bits();
    /// 图像翻转
    const bool reversal = (m_imageFormat==QVideoFrame::Format_RGB32||QVideoFrame::Format_RGB24)?true:false;
    if (reversal) {
        const int bytesPerLine = frame.bytesPerLine();
        const int width = frame.width();
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
    emit captureVideoFrame(frame, timestamp);
}
