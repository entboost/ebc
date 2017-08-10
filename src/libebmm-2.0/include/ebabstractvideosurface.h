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
signals:
    void captureVideoFrame(const QVideoFrame &frame, unsigned long timestamp);
protected:
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool start(const QVideoSurfaceFormat &format);
//    virtual void stop(void);
    virtual bool present(const QVideoFrame &frame);
private:
    void setVideoFrame(const QVideoFrame &frame, unsigned long timestamp);

private:
    QVideoFrame::PixelFormat m_videoFormat;
    QImage::Format m_imageFormat;
    uchar *m_buffer;
    int m_fps;  /// default 10
    unsigned long m_lastTime;
};

#endif // EBABSTRACTVIDEOSURFACE_H
