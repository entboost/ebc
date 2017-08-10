#include "ebrtproominfolist.h"

namespace entboost {

EbRtpRoomInfoList::EbRtpRoomInfoList()
{

}

EbRtpRoomInfoList::~EbRtpRoomInfoList()
{
    DelAllSinkWnd();
}

EbRtpRoomInfoList::pointer EbRtpRoomInfoList::create()
{
    return EbRtpRoomInfoList::pointer(new EbRtpRoomInfoList());
}

EbRtpSinkInfo::pointer EbRtpRoomInfoList::AddSinkWnd(
        mycp::bigint nDestId, QImage::Format format, int nVideoWidth, int nVideoHeight, QWidget *hWndParent)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nDestId,pSinkWnd)) {
        pSinkWnd = EbRtpSinkInfo::create(nDestId);
        if (hWndParent!=0) {
            /// 视频流
            pSinkWnd->CreateWnd(format, nVideoWidth, nVideoHeight, hWndParent);
        }
        m_pSinkWndList.insert(nDestId,pSinkWnd);
    }
    return pSinkWnd;
}

bool EbRtpRoomInfoList::IsExistSinkWnd(mycp::bigint nDestId) const
{
    return m_pSinkWndList.exist(nDestId);
}

void EbRtpRoomInfoList::DelSinkWnd(bigint nDestId)
{
    m_pSinkWndList.remove(nDestId);
}

void EbRtpRoomInfoList::DelAllSinkWnd()
{
    m_pSinkWndList.clear();
}

EbRtpSinkInfo::pointer EbRtpRoomInfoList::GetSinkWnd(bigint nDestId) const
{
    EbRtpSinkInfo::pointer pSinkWnd;
    m_pSinkWndList.find(nDestId,pSinkWnd);
    return pSinkWnd;
}

bool EbRtpRoomInfoList::BuildGraph(bigint nUserId, bool bDefaultRender, bool bStartGraph)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd))
    {
        return false;
    }
    pSinkWnd->BuildGraph(bDefaultRender, bStartGraph);
    return true;
}

bool EbRtpRoomInfoList::IsGraphReady(bigint nUserId) const
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd))
    {
        return false;
    }
    return pSinkWnd->IsGraphReady();
}

void EbRtpRoomInfoList::StopGraph(bigint nUserId)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return;
    }
    pSinkWnd->StopGraph();

}

bool EbRtpRoomInfoList::DrawVideo(bigint nUserId, const unsigned char *pData, uint32 nLen)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return false;
    }
    pSinkWnd->DrawVideo(pData, nLen);
    return true;
}

bool EbRtpRoomInfoList::SetPlay(bigint nUserId, bool bPlay)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return false;
    }
    pSinkWnd->SetPlay(bPlay);
    return true;
}

bool EbRtpRoomInfoList::isPlay(bigint nUserId) const
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return false;
    }
    return pSinkWnd->GetPlay();
}

bool EbRtpRoomInfoList::MoveVideoWindow(bigint nDestId, int x, int y, int width, int height)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nDestId,pSinkWnd)) {
        return false;
    }
    pSinkWnd->MoveVideoWindow(x,y,x+width,y+height);
    return true;
}

bool EbRtpRoomInfoList::ShowVideoWindow(bigint nDestId, bool bShow)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nDestId,pSinkWnd)) {
        return false;
    }
    pSinkWnd->ShowVideoWindow(bShow);
    return true;
}

bool EbRtpRoomInfoList::saveImageFile(bigint nUserId, const QString &sSaveToFile)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return false;
    }
    return pSinkWnd->saveImageFile(sSaveToFile);
}

int EbRtpRoomInfoList::AddFFTSampleData(bigint nUserId, const char *pdata, int length)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return -1;
    }
    return pSinkWnd->AddFFTSampleData(pdata, length);
}

bool EbRtpRoomInfoList::SetUserData(bigint nUserId, unsigned long nUserData)
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return false;
    }
    pSinkWnd->SetUserData(nUserData);
    return true;
}

unsigned long EbRtpRoomInfoList::GetUserData(bigint nUserId) const
{
    EbRtpSinkInfo::pointer pSinkWnd;
    if (!m_pSinkWndList.find(nUserId,pSinkWnd)) {
        return 0;
    }
    return pSinkWnd->GetUserData();
}

}   /// namespace entboost
