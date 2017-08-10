#include "ebrtpsinkinfo.h"
#ifdef USES_RTP_SINK_OBJECT
#include <QTimer>
#include <QCoreApplication>
#endif
#include "ebfourier.h"

namespace entboost {

#ifdef USES_RTP_SINK_OBJECT
EbRtpSinkInfo::EbRtpSinkInfo(bigint nDestId, QObject *parent)
    : QObject(parent)
    , m_bLocalP2POk(false), m_bRemoteP2POk(false)
#else
EbRtpSinkInfo::EbRtpSinkInfo(bigint nDestId)
    : m_bLocalP2POk(false), m_bRemoteP2POk(false)
#endif
    , m_nDestId(nDestId)
    , m_videoWindow(0)
    , m_nSampleLength(0)//, m_nVoicePos(0)
    , m_bPlayOrPause(true)
    , m_nUserData(0)
#ifdef USES_RTP_SINK_OBJECT
    , m_format(QImage::Format_Invalid)
    , m_nVideoWidth(0)
    , m_nVideoHeight(0)
    , m_parent(0)
#endif
{

}

EbRtpSinkInfo::~EbRtpSinkInfo()
{
    m_pRtpManager1.reset();
    m_pRtpManager2.reset();
    /// 退出不能调用，调用会导致异常，
//    StopGraph();
}

EbRtpSinkInfo::pointer EbRtpSinkInfo::create(bigint nDestId)
{
    return EbRtpSinkInfo::pointer(new EbRtpSinkInfo(nDestId));
}

void EbRtpSinkInfo::CreateWnd(QImage::Format format, int nVideoWidth, int nVideoHeight, QWidget *parent)
{
    if (parent==0) {
        return;
    }
    if (m_videoWindow==0) {
#ifdef USES_RTP_SINK_OBJECT
        m_format = format;
        m_nVideoWidth = nVideoWidth;
        m_nVideoHeight = nVideoHeight;
        m_parent = parent;
        /// 使用 postEvent 创建窗口，避免退出时，线程资源发生异常问题
        QEvent * event = new QEvent(QEvent::User);
        QCoreApplication::postEvent(this, event);
        while (m_videoWindow==0) {
#ifdef WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
            QCoreApplication::processEvents();
        }
#else
        m_videoWindow = new EbVideoWindow(format, nVideoWidth, nVideoHeight, parent);
        m_videoWindow->lower();
        m_videoWindow->setVisible(true);
        const QRect &rectParent = parent->geometry();
        m_videoWindow->setGeometry(0, 0, rectParent.width(), rectParent.height());
#endif
    }
}

bool EbRtpSinkInfo::DrawVideo(const uchar *pData, uint32 nLen)
{
    if (m_videoWindow==0 || !m_bPlayOrPause) {
        return false;
    }
//    else if (!m_videoWindow->IsGraphReady())
//    {
//        return false;
//    }else if (!m_videoWindow->IsGraphRunning())
//    {
//        m_videoWindow->StartGraph();
//    }
    m_videoWindow->drawVideo(pData, nLen);
    return true;
}

void EbRtpSinkInfo::BuildGraph(bool bDefaultRender, bool bStartGraph)
{
//    if (m_videoWindow!=NULL)
//    {
//        m_videoWindow->BuildGraph(bDefaultRender);
//        if (bStartGraph)
//            m_videoWindow->StartGraph();
//    }
}

bool EbRtpSinkInfo::IsGraphReady() const
{
    return m_videoWindow==0?false:true;
//    if (m_videoWindow!=NULL)
//    {
//        return m_videoWindow->IsGraphReady();
//    }
//    return false;
}

void EbRtpSinkInfo::StopGraph()
{
    if (m_videoWindow!=0) {
        delete m_videoWindow;
        m_videoWindow = 0;
    }
}

void EbRtpSinkInfo::ShowVideoWindow(bool bShow)
{
    if (m_videoWindow!=0) {
        m_videoWindow->setVisible(bShow);
    }
}

void EbRtpSinkInfo::MoveVideoWindow(int left, int top, int right, int bottom)
{
    if (m_videoWindow!=0) {
        m_videoWindow->setGeometry(left, top, right-left, bottom-top);
    }
}

int EbRtpSinkInfo::AddFFTSampleData(const char *pdata, int length)
{
    if (!m_bPlayOrPause) return 0;
    if (m_nSampleLength >= FFT_LEN)
        return 0;
    const int lenToWrite = (m_nSampleLength +length) > FFT_LEN ? (FFT_LEN - m_nSampleLength) : length;
    memcpy(m_SampleArrry, pdata, lenToWrite);
    m_nSampleLength +=lenToWrite;

    int result = -1;
    if (m_nSampleLength == FFT_LEN) {
        result = ProcessFFTData();
        m_nSampleLength = 0;
    }
    return result;
}

#ifdef USES_RTP_SINK_OBJECT
void EbRtpSinkInfo::createVideoWnd(void)
{
    if (m_videoWindow==0) {
        m_videoWindow = new EbVideoWindow(m_format, m_nVideoWidth, m_nVideoHeight, m_parent);
        m_videoWindow->lower();
        m_videoWindow->setVisible(true);
        const QRect &rectParent = m_parent->geometry();
        m_videoWindow->setGeometry(0, 0, rectParent.width(), rectParent.height());
    }
}

void EbRtpSinkInfo::customEvent(QEvent *e)
{
    if (e->type()==QEvent::User) {
        createVideoWnd();
    }
}
#endif  /// USES_RTP_SINK_OBJECT

int EbRtpSinkInfo::ProcessFFTData()
{
    int i;
    int limit = FFT_LEN/2;
    for(i =0; i< limit; ++i)
    {
        finleft[i] = (double)(m_SampleArrry[i]);
    }
    //Perform FFT on left channel
    fft_double(FFT_LEN/2,0,finleft,NULL,fout,foutimg);
    float re,im,fmax=-99999.9f,fmin=99999.9f;
    for(i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
    {
        re = (float)fout[i];
        im = (float)foutimg[i];
        //get amplitude and scale to 0..256 range
        double temp = GetFrequencyIntensity( re , im );
        temp = temp/(FFT_LEN/2);
        fdraw[i]=(int)temp%200;
        if (fdraw[i] > fmax)
        {
            fmax = (float)fdraw[i];
        }
        if (fdraw[i] < fmin)
        {
            fmin = (float)fdraw[i];
        }
        if( fdraw[i] != 0 )
        {
            //m_nVoicePos = (int)fdraw[i];
            return (int)fdraw[i];
            //break;
        }
    }
    return -1;
}


} /// namespace entboost
