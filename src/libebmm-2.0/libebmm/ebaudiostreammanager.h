#ifndef EBAUDIOSTREAMMANAGER_H
#define EBAUDIOSTREAMMANAGER_H

#include <CGCLib/CGCLib.h>
#include <QObject>
#include <boost/thread.hpp>
#include "ebaudiobuffer.h"
#include "ebaudiostreamitem.h"

typedef void (*FFTdataFn) (mycp::bigint nId, const char *data, int size, void* userData);

namespace entboost {

class EbAudioStreamManager : public QObject
{
    Q_OBJECT
public:
    EbAudioStreamManager(int srcSampleRate, int srcChannels, QObject *parent=0);
    virtual ~EbAudioStreamManager(void);

    bool AddStream(mycp::bigint nId, int srcSampleRate = 0, int srcChannels = 0);
    void DelStream(mycp::bigint nId);
    void DelAllStream(void);
    bool IsExistStream(mycp::bigint nId) const {return m_streamlst.exist(nId);}

    void WritestreamData(mycp::bigint nId, unsigned char *buf,
                         unsigned int len, unsigned short seq, unsigned int ts);

    EbAudioBuffer::pointer getPool(void);
    void setPool(const EbAudioBuffer::pointer &pBuffer);

    void SetStreamFFTCallback(mycp::bigint nId, FFTdataFn cb, void* userData);

    /// start or stop the thread.
    void Run(void);
    void Stop(void);

    void StartPlay(mycp::bigint nId);
    void StopPlay(mycp::bigint nId);
    void SetDefaultConfig(int srcSampleRate, int srcChannels);
protected slots:
    void onRunProcessData(void);
private:
    /// process video buffer.
    void ProcessData(void);
    void ClearData();
private:
    bool m_isStoped;
    boost::shared_ptr<boost::thread> m_thread;

    int m_srcSampleRate;
    int m_srcChannels;
    int m_srcBitRate;

    CLockMap<mycp::bigint, EbAudioStreamItem::pointer> m_streamlst;
//    CLockMap<mycp::bigint,CAudioFaadWaveOutPlay::pointer> m_streamlst;
    CLockList<EbAudioBuffer::pointer> m_msgqlst;
    CLockList<EbAudioBuffer::pointer> m_pPool;
};

}   /// namespace entboost

#endif // EBAUDIOSTREAMMANAGER_H
