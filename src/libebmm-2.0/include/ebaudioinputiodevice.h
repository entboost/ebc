#ifndef EBAUDIOINPUTIODEVICE_H
#define EBAUDIOINPUTIODEVICE_H

#include <QIODevice>
//#include <QFile>
//#include <QTimerEvent>
//#include <ebaacencoder.h>
//#include <ebaacdecoder.h>
//#include <boost/shared_ptr.hpp>
//#include <stl/locklist.h>

//class EbData
//{
//public:
//    typedef boost::shared_ptr<EbData> pointer;
//    EbData(const uchar *data, unsigned int size)
//        : m_data(0)
//        , m_size(size)
//    {
//        if (data!=0 && m_size>0) {
//            m_data = new uchar[m_size];
//            memcpy(m_data, data, m_size);
//        }
//    }
//    virtual ~EbData(void)
//    {
//        if (m_data!=0)
//            delete[] m_data;
//    }

//    static EbData::pointer create(const uchar *data, unsigned int size)
//    {
//        return EbData::pointer(new EbData(data, size));
//    }

//    const uchar *data(void) const {return m_data;}
//    unsigned int size(void) const {return m_size;}
//private:
//    uchar *m_data;
//    unsigned int m_size;
//};

class EbAudioInputIODevice : public QIODevice
//        , public EbAacEncoderCallback
//        , public EbAacDecoderCallback
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
//    virtual void timerEvent(QTimerEvent *e);

//    virtual void onEncodeInitOk(const unsigned char * decodeSCData, int size);
//    virtual void onEncodeOk(const unsigned char * data, int size);
//    virtual void onDecodeOk(const unsigned char * data, int size);
private:
    bool m_detectSilenceData;   /// 是否检测静音数据，默认true
    short * m_buffer;
//    CLockList<EbData::pointer> m_list;
//    int m_timerId;
//    QFile m_file;
//    EbAacEncoder m_aacEncoder;
//    EbAacDecoder m_aacDecoder;
};

#endif // EBAUDIOINPUTIODEVICE_H
