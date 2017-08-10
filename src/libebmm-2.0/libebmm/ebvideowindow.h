#ifndef EBVIDEOWINDOW_H
#define EBVIDEOWINDOW_H

#include <QLabel>
#include <QImage>

class EbVideoWindow : public QLabel
{
    Q_OBJECT
public:
    explicit EbVideoWindow(QImage::Format format, int width, int height, QWidget *parent=0);

    void setReady(bool v) {m_isReady=v;}
    bool isReady(void) const {return m_isReady;}

    bool drawVideo(const uchar *data, int size);
    bool saveImageFile(const QString &imageFile);
private:
    QImage::Format m_format;
    int m_width;
    int m_height;
    int m_bytesPerLine;
    int m_bitCount;
    bool m_isReady;
    QImage m_currentImage;
};

#endif // EBVIDEOWINDOW_H
