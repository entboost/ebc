#include "ebvideowindow.h"

EbVideoWindow::EbVideoWindow(QImage::Format format, int width, int height, QWidget *parent)
    : QLabel(parent)
    , m_format(format)
    , m_width(width)
    , m_height(height)
    , m_bytesPerLine(0)
    , m_bitCount(0)
    , m_isReady(false)
{
//    list << QVideoFrame::Format_RGB32;
//    list << QVideoFrame::Format_RGB24;
//    list << QVideoFrame::Format_ARGB32;
//    list << QVideoFrame::Format_ARGB32_Premultiplied;
//    list << QVideoFrame::Format_RGB565;
//    list << QVideoFrame::Format_RGB555;
//    list << QVideoFrame::Format_UYVY;
//    list << QVideoFrame::Format_Y8;
//    list << QVideoFrame::Format_YUYV;
    if (m_format==QImage::Format_RGB32) {
        m_bytesPerLine = m_width*4;
        m_bitCount = 32;
    }
    else if (m_format==QImage::Format_RGB888) {
        m_bytesPerLine = m_width*3;
        m_bitCount = 24;
    }
    else if (m_format==QImage::Format_RGB16) {
        m_bytesPerLine = m_width*2;
        m_bitCount = 16;
    }
}

bool EbVideoWindow::drawVideo(const uchar *data, int size)
{
    if (size!=(m_bytesPerLine*m_height)) {
        return false;
    }
    m_currentImage = QImage(
                data,
                m_width,
                m_height,
                m_bytesPerLine,
                m_format);
    const QRect &rect = this->geometry();
    if (rect.width()==m_width && rect.height()==m_height)
        this->setPixmap( QPixmap::fromImage(m_currentImage) );
    else
        this->setPixmap( QPixmap::fromImage(m_currentImage).scaled(rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    return true;
}

bool EbVideoWindow::saveImageFile(const QString &imageFile)
{
    return m_currentImage.save(imageFile);
}
