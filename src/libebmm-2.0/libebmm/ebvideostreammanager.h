#ifndef EBVIDEOSTREAMMANAGER_H
#define EBVIDEOSTREAMMANAGER_H

#include <QObject>
#include <boost/thread.hpp>
#include <CGCLib/CGCLib.h>
#include <stl/locklist.h>
#include <stl/lockmap.h>
#include "ebvideobuffer.h"
#include "ebvideoframe.h"
#include "ebvideostreamitem.h"
#include "ebvideodefine.h"

namespace entboost {

/// 是否使用缓存，效果一样
//#define _EB_USES_VIDEO_STREAM_CACHE
typedef void (*VideoStreamCallBack) (mycp::bigint nId, const VIDEOFRAME *pFrame, void *param, unsigned long userData);

class EbVideoStreamManager : public QObject
{
    Q_OBJECT
public:
    EbVideoStreamManager(int nWidth , int nHeight, QObject * parent=0);
    virtual ~EbVideoStreamManager();

    void AddUser(mycp::bigint nUserId, int delaytime, int codec_type = 0);
    void DelUser(mycp::bigint nUserId);
    void DelAllUser(void);

    bool SetUserData(mycp::bigint nUserId, unsigned long userData);
    unsigned long GetUserData(mycp::bigint nUserId);

    bool Write(mycp::bigint nUserId, unsigned long size, const unsigned char* data,
               unsigned short seq, unsigned long ts, unsigned char pdtype, int pt);  //put data to stream

    void Run(void);
    void Stop(void);

    bool isExistUser(mycp::bigint userId) const {return m_userList.exist(userId);}
    void clearVideoLst(void);

    /// clear video buffer cache list.
//    void clearCacheLst(void) {m_cachelst.clear();}

    /// clear self data.
    void clearSelf(void);

    /// set callback function.
    void SetVideoStreamCallBack(VideoStreamCallBack cb, void *param);
protected slots:
    void onRunProcessData(void);

private:
    /// process video buffer.
#ifdef _EB_USES_VIDEO_STREAM_CACHE
    void readCache(void);
#endif
    void ProcessData(void);

    EbVideoBuffer::pointer getVideoBufferPool(void);
    void setVideoBufferPool(const EbVideoBuffer::pointer &pVideoBuffer);
    EbVideoFrame::pointer getVideoFramePool(void);
    void setVideoFramePool(const EbVideoFrame::pointer &pVideoFrame);

protected:
    CLockMap<mycp::bigint, EbVideoStreamItem::pointer> m_userList;
    CLockList<EbVideoBuffer::pointer> m_videoBufferPool;
    CLockList<EbVideoFrame::pointer> m_videoFramePool;
#ifdef _EB_USES_VIDEO_STREAM_CACHE
    CLockList<EbVideoBuffer::pointer> m_videoBufferCache;
#endif

    bool       m_bReady;
    bool       m_isStoped;       /// stop running
    boost::shared_ptr<boost::thread> m_thread;

    VideoStreamCallBack m_callback;
    void     *m_cbParam;         ///call back user data.

    int			m_nWidth;
    int			m_nHeight;
};

}   /// namespace entboost

#endif // EBVIDEOSTREAMMANAGER_H
