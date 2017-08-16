#ifndef EBAUDIOINPUTIODEVICE_H
#define EBAUDIOINPUTIODEVICE_H

#include <QIODevice>

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class EbAudioInputIODevice : public QIODevice
{
    Q_OBJECT
public:
    EbAudioInputIODevice(QObject *parent=0);
    virtual ~EbAudioInputIODevice(void);

    void setDeleteSilenceData(bool v) {m_detectSilenceData=v;}
    bool deleteSilenceData(void) const {return m_detectSilenceData;}
signals:
    void captureAudioData(const char *data, qint64 len, unsigned long timestamp);
protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
private:
    bool m_detectSilenceData;   /// 是否检测静音数据，默认true
    short *m_buffer;
};

#endif // EBAUDIOINPUTIODEVICE_H
