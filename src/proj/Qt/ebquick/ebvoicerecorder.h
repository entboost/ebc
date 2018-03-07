#ifndef EBVOICERECORDER_H
#define EBVOICERECORDER_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QFile>
#include <QTimerEvent>

struct WAVFILEHEADER
{
    // RIFF 头
    char RiffName[4];
    unsigned long nRiffLength;

    // 数据类型标识符
    char WavName[4];

    // 格式块中的块头
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头
    char    DATANAME[4];
    unsigned long   nDataLength;
};
const int const_wav_file_header_size = sizeof(WAVFILEHEADER);

class EbVoiceRecorder : public QIODevice///QObject
{
    Q_OBJECT
public:
    explicit EbVoiceRecorder(QObject *parent = nullptr);
    virtual ~EbVoiceRecorder(void);

    bool startRecorder(const QString &wavFilePath, int maxSeconds=30);
    bool isStarted(void) const;
    void stopRecorder();
    unsigned long dataLength() const;       /// 数据长度
    unsigned int dataSeconds() const;       /// 录制秒数
    const QString &filePath(void) const {return m_wavFilePath;}
signals:

public slots:

protected:
    virtual void timerEvent(QTimerEvent *e);
    virtual qint64 readData(char *data, qint64 maxlen) {return 0;}
    virtual qint64 writeData(const char *data, qint64 len);
private:
    int m_timerIdStop;
    QAudioInput * m_audioInput;
    QString m_wavFilePath;
    QFile m_wavFile;
    WAVFILEHEADER m_wavFileHeader;
};

#endif // EBVOICERECORDER_H
