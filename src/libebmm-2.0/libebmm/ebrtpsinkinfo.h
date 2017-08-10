#ifndef EBRTPSINKINFO_H
#define EBRTPSINKINFO_H

#include "../include/EBRtpManager.h"
#include <CGCBase/cgcobject.h>
#include <boost/shared_ptr.hpp>
#include "ebvideowindow.h"

#define FFT_LEN  1024
//#define USES_RTP_SINK_OBJECT

namespace entboost {

class EbRtpSinkInfo
#ifdef USES_RTP_SINK_OBJECT
: public QObject
#endif
{
#ifdef USES_RTP_SINK_OBJECT
    Q_OBJECT
#endif
public:
#ifdef USES_RTP_SINK_OBJECT
    EbRtpSinkInfo(mycp::bigint nDestId, QObject * parent=0);
#else
    EbRtpSinkInfo(mycp::bigint nDestId);
#endif
    virtual ~EbRtpSinkInfo(void);
    typedef boost::shared_ptr<EbRtpSinkInfo> pointer;
    static EbRtpSinkInfo::pointer create(mycp::bigint nDestId);

    bool m_bLocalP2POk;
    bool m_bRemoteP2POk;
    CEBRtpManager::pointer m_pRtpManager1;	/// 内网
    CEBRtpManager::pointer m_pRtpManager2;	/// 公网
    void SetLocalAddress(const std::string & value) {m_sLocalAddress = value;}
    const tstring & GetLocalAddress(void) const {return m_sLocalAddress;}
    void SetRemoteAddress(const std::string & value) {m_sRemoteAddress = value;}
    const tstring & GetRemoteAddress(void) const {return m_sRemoteAddress;}

    mycp::bigint GetSinkDestId(void) const {return m_nDestId;}
    void CreateWnd(QImage::Format format, int nVideoWidth, int nVideoHeight, QWidget *hWndParent);
    bool DrawVideo(const uchar* pData, mycp::uint32 nLen);
    void BuildGraph(bool bDefaultRender = false, bool bStartGraph = false);
    bool IsGraphReady(void) const;
    void StopGraph(void);
    void ShowVideoWindow(bool bShow);
    void MoveVideoWindow(int left, int top, int right, int bottom);
    // ????
    bool saveImageFile(const QString &sSaveToFile) {return (m_videoWindow==0)?false:m_videoWindow->saveImageFile(sSaveToFile);}
    void SetPlay(bool v) {m_bPlayOrPause = v;}
    bool GetPlay(void) const {return m_bPlayOrPause;}
    void SetUserData(unsigned long v) {m_nUserData = v;}
    unsigned long GetUserData(void) const {return m_nUserData;}

    int AddFFTSampleData(const char* pdata, int length);
protected:
#ifdef USES_RTP_SINK_OBJECT
    void createVideoWnd(void);
    virtual void customEvent(QEvent *e);
#endif
    double finleft[FFT_LEN/2],fout[FFT_LEN], foutimg[FFT_LEN],fdraw[FFT_LEN/2];
    inline double GetFrequencyIntensity(double re, double im) const {return sqrt((re*re)+(im*im));}
    int ProcessFFTData(void);
    short m_SampleArrry[FFT_LEN];
    int   m_nSampleLength;
    //int		m_nVoicePos;
protected:
    mycp::bigint m_nDestId;
    tstring m_sLocalAddress;
    tstring m_sRemoteAddress;
    EbVideoWindow *m_videoWindow;
    bool m_bPlayOrPause;
    unsigned long m_nUserData;
#ifdef USES_RTP_SINK_OBJECT
    /// 用于 createWnd
    QImage::Format m_format;
    int m_nVideoWidth;
    int m_nVideoHeight;
    QWidget *m_parent;
#endif
};

} /// namespace entboost

#endif // EBRTPSINKINFO_H
