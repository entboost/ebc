#ifndef EBABSTRACTVIDEOSURFACE_H
#define EBABSTRACTVIDEOSURFACE_H

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QVideoProbe>
#include <QCamera>

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

class EbAbstractVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    EbAbstractVideoSurface(QObject *parent=0);
    virtual ~EbAbstractVideoSurface(void);

    void setFps(int fps) {this->m_fps=fps;}
    int fps(void) const {return this->m_fps;}
    QVideoFrame::PixelFormat videoFormat(void) const {return m_videoFormat;}
    QImage::Format imageFormat(void) const {return m_imageFormat;}

    bool setSource(QCamera *pCamera, const QSize &frameSize);
    bool isStarted(void) const {return m_started;}
signals:
    void captureVideoFrame(const uchar *frameData, int frameSize, int width, int height, unsigned long timestamp);
protected:
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool start(const QVideoSurfaceFormat &format);
//    virtual void stop(void);
    virtual bool present(const QVideoFrame &frame);
private:
    bool initConvert(AVPixelFormat from, AVPixelFormat to, int width, int height);
    void releaseConvert(void);
    void convertSendFrame(const QVideoFrame &frame, unsigned long timestamp);
    void setVideoFrame(const QVideoFrame &frame, unsigned long timestamp);

private:
    bool m_started;
    QVideoFrame::PixelFormat m_videoFormat;
    QImage::Format m_imageFormat;
    uchar *m_buffer;
    int m_fps;  /// default 10
    unsigned long m_lastTime;
    QVideoProbe *m_videoProbe;
    QSize m_sizeFrame;
    int m_bytesPerLine; /// 转换才需要这个参数

    SwsContext *m_swsContext;
    AVFrame *m_frameFrom;
    AVFrame *m_frameTo;
};

#endif // EBABSTRACTVIDEOSURFACE_H
