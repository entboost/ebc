#ifndef EBRTPROOMINFOLIST_H
#define EBRTPROOMINFOLIST_H

#include "ebrtpsinkinfo.h"

namespace entboost {

class EbRtpRoomInfoList
{
public:
    EbRtpRoomInfoList(void);
    virtual ~EbRtpRoomInfoList(void);
    typedef boost::shared_ptr<EbRtpRoomInfoList> pointer;
    static EbRtpRoomInfoList::pointer create(void);

    EbRtpSinkInfo::pointer AddSinkWnd(
            mycp::bigint nDestId, QImage::Format format, int nVideoWidth, int nVideoHeight, QWidget *hWndParent);
    bool IsExistSinkWnd(mycp::bigint nDestId) const;
    void DelSinkWnd(mycp::bigint nDestId);
    void DelAllSinkWnd(void);
    EbRtpSinkInfo::pointer GetSinkWnd(mycp::bigint nDestId) const;
    bool BuildGraph(mycp::bigint nUserId, bool bDefaultRender = false, bool bStartGraph = false);
    bool IsGraphReady(mycp::bigint nUserId) const;
    void StopGraph(mycp::bigint nUserId);
    bool DrawVideo(mycp::bigint nUserId, const unsigned char* pData, mycp::uint32 nLen);
    bool SetPlay(mycp::bigint nUserId, bool bPlay);
    bool isPlay(mycp::bigint nUserId) const;
    bool MoveVideoWindow(mycp::bigint nDestId, int x, int y, int width, int height);
    bool ShowVideoWindow(mycp::bigint nDestId, bool bShow);
    bool saveImageFile(mycp::bigint nUserId, const QString &sSaveToFile);
    int AddFFTSampleData(mycp::bigint nUserId,const char* pdata, int length);
    bool SetUserData(mycp::bigint nUserId, unsigned long nUserData);
    unsigned long GetUserData(mycp::bigint nUserId) const;
private:
    CLockMap<mycp::bigint,EbRtpSinkInfo::pointer> m_pSinkWndList;
};

}   /// namespace entboost

#endif // EBRTPROOMINFOLIST_H
