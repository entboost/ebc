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
    switch (m_format) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32: {
        m_bytesPerLine = m_width*4;
        m_bitCount = 32;
        break;
    }
    case QImage::Format_RGB888: {
        m_bytesPerLine = m_width*3;
        m_bitCount = 24;
        break;
    }
    case QImage::Format_RGB16: {
        m_bytesPerLine = m_width*2;
        m_bitCount = 16;
        break;
    }
    default:
        break;
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
//    static bool theSave = false;
//    if (!theSave) {
//        theSave = true;
//        m_currentImage.save("/Users/akee/Desktop/mac_videocapture.png");
//    }
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
