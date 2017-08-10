#include "ebwidgetvideoframe.h"
#include <QTimer>
#include "ebmessagebox.h"

const QSize const_btn_size(58, 26);
 EbWidgetVideoFrame:: EbWidgetVideoFrame(const EbcCallInfo::pointer &callInfo, QWidget *parent) : QWidget(parent)
  , m_callInfo(callInfo)
{
    assert ( m_callInfo.get()!=0 );

    m_buttonOn = new QPushButton(this);
    m_buttonOn->resize(const_btn_size);
    m_buttonOn->setObjectName("VideoButton");
    connect(m_buttonOn, SIGNAL(clicked()), this, SLOT(onClickedButtonOn()));
    m_buttonAccept = new QPushButton(this);
    m_buttonAccept->resize(const_btn_size);
    m_buttonAccept->setObjectName("VideoButton");
    connect(m_buttonAccept, SIGNAL(clicked()), this, SLOT(onClickedButtonAccept()));
    m_buttonEnd = new QPushButton(this);
    m_buttonEnd->resize(const_btn_size);
    m_buttonEnd->setObjectName("VideoButton");
    connect(m_buttonEnd, SIGNAL(clicked()), this, SLOT(onClickedButtonEnd()));
    m_buttonCancel = new QPushButton(this);
    m_buttonCancel->resize(const_btn_size);
    m_buttonCancel->setObjectName("VideoButton");
    connect(m_buttonCancel, SIGNAL(clicked()), this, SLOT(onClickedButtonCancel()));

    /// 群组多人视频会话，使用 m_buttonOn
    m_buttonOn->setVisible(m_callInfo->isGroupCall()?true:false);
    /// 一对一视频聊天，使用 m_buttonAccept
    m_buttonAccept->setVisible(m_callInfo->isGroupCall()?false:true);

    CEBAppClient::EB_GetVideoDeviceList(m_pVideoDevices);
    m_buttonEnd->setEnabled(false);
    m_buttonCancel->setEnabled(false);

    theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(), EBVideoDataCallBack);
    updateLocaleInfo();
}

void  EbWidgetVideoFrame::updateLocaleInfo()
{
    if (m_callInfo->isGroupCall()) {
        /// 一对一视频聊天使用 m_buttonOn
        m_buttonAccept->setVisible(false);

        /// 打开本地视频
        m_buttonOn->setText( theLocales.getLocalText("video-frame.button-on-group.text","Open Video") );
        m_buttonOn->setToolTip( theLocales.getLocalText("video-frame.button-on-group.tooltip","") );
        /// 关闭本地视频
        m_buttonEnd->setText( theLocales.getLocalText("video-frame.button-end-group.text","Close Video") );
        m_buttonEnd->setToolTip( theLocales.getLocalText("video-frame.button-end-group.tooltip","") );
        if (m_bInVideoConference) {
            /// 退出会议 & 退出视频会议
            m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-exit.text","Exit Video") );
            m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-exit.tooltip","") );
        }
        else {
            /// 加入会议 & 加入视频会议
            m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-join.text","Join Video") );
            m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-join.tooltip","") );
        }
    }
    else {
        /// 一对一视频聊天使用 m_buttonAccept
        m_buttonOn->setVisible(false);
        m_buttonCancel->setEnabled(false);
        /// 接受 & 接受视频通话
        m_buttonAccept->setText( theLocales.getLocalText("video-frame.button-accept.text","Accept") );
        m_buttonAccept->setToolTip( theLocales.getLocalText("video-frame.button-accept.tooltip","") );
        /// 结束 & 结束视频通话
        m_buttonEnd->setText( theLocales.getLocalText("video-frame.button-end.text","End") );
        m_buttonEnd->setToolTip( theLocales.getLocalText("video-frame.button-end.tooltip","") );
        /// 拒绝 & 拒绝视频通话
        m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-reject.text","Reject") );
        m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-reject.tooltip","") );
    }

}

bool EbWidgetVideoFrame::isEmpty() const
{
    BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()!=0) {
            return false;
        }
    }
    return true;
}

int  EbWidgetVideoFrame::videoCount() const
{
    BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
    int result = 0;
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()!=0) {
            result++;
        }
    }
    return result;
}

void  EbWidgetVideoFrame::onExitChat(bool hangup)
{
    if (m_bInFVideoRequest) {
        onClickedButtonCancel();
    }
    onClickedButtonEnd();
}

void  EbWidgetVideoFrame::onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    const bool bOk = nStateValue==EB_STATE_OK;
    if (bOk) {
        theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(), EBVideoDataCallBack);
        if (m_callInfo->isGroupCall()) {
            /// 群组视频，打开视频
            if (!m_bInVideoConference) {
                theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), true, 0);
                m_bInVideoConference = true;
                /// 退出会议 & 退出视频会议
                m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-exit.text","Exit Video") );
                m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-exit.tooltip","") );
            }
//            SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
            doVideoConnected();
        }
        else {
            //m_staDesc.SetWindowText(_T("正在请求视频通话，等待对方连接..."));
            //theApp.InvalidateParentRect(&m_staDesc);
        }
    }
    else {
        //m_staDesc.SetWindowText(_T("请求失败！"));
        //theApp.InvalidateParentRect(&m_staDesc);
    }

    if (m_callInfo->isGroupCall()) {
        //m_btnVideoAccept.EnableWindow(TRUE);
        //m_btnVideoCancel.EnableWindow(TRUE);
    }
    else {
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(true);
        m_buttonCancel->setEnabled(false);
    }
}

void  EbWidgetVideoFrame::onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    /// 接受才发送窗口消息
//    SetTimer(TIMER_ID_DO_VIDEO_CONNECTE,100,NULL);
    doVideoConnected();
    if (m_callInfo->isGroupCall()) {
        /// 只有接收才有VAckResponse消息；
        /// 这里有可能是第一次进入视频会议；
        if (!m_bInVideoConference) {
            theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(), EBVideoDataCallBack);
            m_bInVideoConference = true;
            /// 退出会议 & 退出视频会议
            m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-exit.text","Exit Video") );
            m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-exit.tooltip","") );
        }
        //m_btnVideoAccept.EnableWindow(TRUE);
        //m_btnVideoCancel.EnableWindow(TRUE);
        //m_btnVideoEnd.EnableWindow(FALSE);
    }
    else {
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(true);
        m_buttonCancel->setEnabled(false);
    }
}

void  EbWidgetVideoFrame::onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (m_callInfo->isGroupCall()) {
        //if (this->GetVideoCount()>0)
        if (m_bInVideoConference) {
            /// 自动接收视频
            theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(), EBVideoDataCallBack);
            theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), true, pUserVideoInfo->m_sUserAccount);
        }
    }
    else {
        if (!isMyOnVideo()) {
            m_bInFVideoRequest = true;
            m_buttonAccept->setEnabled(true);
            m_buttonEnd->setEnabled(false);
            m_buttonCancel->setEnabled(true);
        }
    }
}

void  EbWidgetVideoFrame::onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (!m_callInfo->isGroupCall()) {
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(true);
        m_buttonCancel->setEnabled(false);
//        QEvent *event = new QEvent(QEvent::User);
//        QCoreApplication::postEvent(this, event);
        doVideoConnected();
    }
}

void  EbWidgetVideoFrame::onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (!m_callInfo->isGroupCall()) {
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(false);
        m_buttonCancel->setEnabled(false);
    }
}

void  EbWidgetVideoFrame::onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (m_callInfo->isGroupCall()) {
        /// 有人退出视频；
        const mycp::bigint nVideoUserId = pUserVideoInfo->m_sUserAccount;
        if (nVideoUserId==0) {
            return;
        }
        closeUserVideo(nVideoUserId);
        if (videoCount() == 0) {
//            QEvent *event = new QEvent((QEvent::Type)(QEvent::User+1));
//            QCoreApplication::postEvent(this, event);
            doVideoDisonnecte();
        }
    }
    else {
        /// 一对一会话，或者本端关闭
//        QEvent *event = new QEvent((QEvent::Type)(QEvent::User+1));
//        QCoreApplication::postEvent(this, event);
        doVideoDisonnecte();
    }

    if (m_callInfo->isGroupCall()) {
        //// 群组可以继续请求查看视频
        //m_btnVideoAccept.EnableWindow(TRUE);
        //m_btnVideoCancel.EnableWindow(TRUE);
    }
    else {
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(false);
        m_buttonCancel->setEnabled(false);
    }
}

void EbWidgetVideoFrame::onClickedButtonOn()
{
    m_buttonOn->setEnabled(false);
    m_buttonEnd->setEnabled(true);

    theApp->m_ebum.EB_VideoRequest(m_callInfo->callId(), EB_VIDEO_BOTH);
}

void EbWidgetVideoFrame::onClickedButtonAccept()
{
    theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(), EBVideoDataCallBack);
    theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), true, 0);
}

void EbWidgetVideoFrame::onClickedButtonEnd()
{
    if (m_callInfo->isGroupCall()) {
        doVideoDisonnecte(true, theApp->logonUserId());
        m_buttonOn->setEnabled(true);
        m_buttonEnd->setEnabled(false);
    }
    else {
        doVideoDisonnecte(true);
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(false);
        m_buttonCancel->setEnabled(false);
//        this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
    }
    theApp->m_ebum.EB_VideoEnd(m_callInfo->callId());
}

void EbWidgetVideoFrame::onClickedButtonCancel()
{
    if (m_callInfo->isGroupCall()) {
        if (m_bInVideoConference) {
            /// 退出会议
            m_bInVideoConference = false;
            doVideoDisonnecte(true);
            theApp->m_ebum.EB_VideoEnd(m_callInfo->callId());
            theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), false, 0);
            doVideoDisonnecte();
            m_buttonOn->setEnabled(true);
            m_buttonEnd->setEnabled(false);

            /// 加入会议 & 加入视频会议
            m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-join.text","Join Video") );
            m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-join.tooltip","") );
//            this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
        }
        else {
            m_bInVideoConference = true;
            theApp->m_ebum.EB_SetVideoCallback(m_callInfo->callId(),EBVideoDataCallBack);
            theApp->m_ebum.EB_VideoAck(m_callInfo->callId(),true,0);
            m_buttonOn->setEnabled(true);

            /// 退出会议 & 退出视频会议
            m_buttonCancel->setText( theLocales.getLocalText("video-frame.button-group-exit.text","Exit Video") );
            m_buttonCancel->setToolTip( theLocales.getLocalText("video-frame.button-group-exit.tooltip","") );
        }
    }
    else {
        doVideoDisonnecte();
        m_buttonAccept->setEnabled(false);
        m_buttonEnd->setEnabled(false);
        m_buttonCancel->setEnabled(false);
        theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), false, 0);
        //theApp->m_ebum.EB_VideoEnd(m_callInfo->callId());
//        this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
    }

}

void  EbWidgetVideoFrame::resizeEvent(QResizeEvent *e)
{
    int x = 2;
    int y = 5;
    m_buttonOn->move(x, y);
    m_buttonAccept->move(x, y);
    x += (const_btn_size.width()+5);
    m_buttonEnd->move(x, y);
    x += (const_btn_size.width()+5);
    m_buttonCancel->move(x, y);
    QWidget::resizeEvent(e);
}

void EbWidgetVideoFrame::customEvent(QEvent *e)
{
    if (e->type()==QEvent::User) {
        doVideoConnected();
    }
    else if (e->type()==QEvent::User+1) {
        doVideoDisonnecte();
    }
}

bool  EbWidgetVideoFrame::isMyOnVideo() const
{
    BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()==0) {
            continue;
        }
        if (m_pUserVideo[i]->userId()==theApp->logonUserId()) {
            return true;
        }
    }
    return false;
}

int  EbWidgetVideoFrame::getVideoIndex(eb::bigint nVideoUserId) const
{
    BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()==0) {
            continue;
        }
        if (m_pUserVideo[i]->userId()==nVideoUserId) {
            return i;
        }
    }
    return -1;
}

int  EbWidgetVideoFrame::closeUserVideo(eb::bigint nVideoUserId)
{
    BoostWriteLock wtlock(m_mutex);
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()==0) {
            continue;
        }
        if (m_pUserVideo[i]->userId()==nVideoUserId) {
            m_pUserVideo[i].reset();
//            SetTimer(TIMERID_CHECK_CLOSE_VIDEO,2000,NULL);
            return i;
        }
    }
    return -1;
}

int  EbWidgetVideoFrame::getNullVideoIndex(eb::bigint userId)
{
    BoostWriteLock wtlock(m_mutex);
    for (int i=0; i<MAX_VIDEO_COUNT; i++) {
        if (m_pUserVideo[i].get()==0) {
            m_pUserVideo[i] = EbWidgetVideoWindow::create(userId, this);
            return i;
        }
    }
    return -1;
}

const int const_view_width = 216;
const int const_view_height = 168;
void  EbWidgetVideoFrame::openUserVideo(eb::bigint nVideoUserId)
{
    if (getVideoIndex(nVideoUserId)>=0) {
        return;
    }
    const int index = getNullVideoIndex(nVideoUserId);
    if (index < 0) {
        return;
    }
    const bool bLocalUser = nVideoUserId==theApp->logonUserId()?true:false;
    BoostWriteLock wtlock(m_mutex);
//    m_nVideoUserId[index] = nVideoUserId;

    const int x = 2+((const_view_width+6)*(index/3));		/// 6=左右间隔
    const int y = 38+(const_view_height+6)*(index%3);		/// 6=上下间隔
    const QRect rectVideo(x, y, const_view_width, const_view_height);

    EbWidgetVideoWindow::pointer videoWindow = m_pUserVideo[index];
    assert(videoWindow.get()!=0);
    videoWindow->setCallId(this->m_callInfo->callId());
    videoWindow->setGeometry(rectVideo);
    videoWindow->setVisible(true);
//    videoWindow->setVisible(false);
    if (bLocalUser) {
        int nLocalVideoIndex = (size_t)theApp->localVideoIndex();
        if (nLocalVideoIndex >= m_pVideoDevices.size())
            nLocalVideoIndex = 0;
        const int ret = theApp->m_ebum.EB_OpenLocalVideo(
                    m_callInfo->callId(),
                    nLocalVideoIndex,
                    videoWindow.get(),
                    (void*)videoWindow.get());
        if (ret==10) {
            /// 打开视频摄像头失败：\r\n请检查摄像头是否被占用或其他硬件故障！
            const QString text = theLocales.getLocalText("message-show.open-video-device-error", "Open local video device error");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        }
    }
    else {
        theApp->m_ebum.EB_OpenUserVideo(
                    m_callInfo->callId(),
                    nVideoUserId,
                    videoWindow.get(),
                    (void*)videoWindow.get());
    }
}

void  EbWidgetVideoFrame::doVideoDisonnecte(bool bHideOnly, eb::bigint nOnlyHideUid)
{
    m_bInFVideoRequest = false;
    {
        BoostReadLock rdlock(m_mutex);
        for (int i=0; i<MAX_VIDEO_COUNT; i++) {
            if (m_pUserVideo[i].get()==0) {
                continue;
            }
            if (nOnlyHideUid==0 || nOnlyHideUid==m_pUserVideo[i]->userId()) {
                m_pUserVideo[i]->setVisible(false);
            }
        }
        if (bHideOnly) {
            return;
        }
    }

    {
        BoostWriteLock wtlock(m_mutex);
        for (int i=0; i<MAX_VIDEO_COUNT; i++) {
            if (m_pUserVideo[i].get()!=0) {
                m_pUserVideo[i].reset();
            }
        }
    }
}

void  EbWidgetVideoFrame::doVideoConnected()
{
    std::vector<EB_UserVideoInfo> pUserVideoInfo;
    theApp->m_ebum.EB_GetUserVideoInfo(m_callInfo->callId(), pUserVideoInfo);
    /// 生成上麦视频
    for (size_t i=0; i<pUserVideoInfo.size(); i++) {
        if (pUserVideoInfo[i].m_bOnVideo) {
            openUserVideo(pUserVideoInfo[i].m_sUserAccount);
        }
    }
}

void  EbWidgetVideoFrame::EBVideoDataCallBack(eb::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam, void *pUserData)
{
    switch (nCallBackType)
    {
//    case EB_CALLBACK_FIRST_VIDEO_FPS: {
//        EbWidgetVideoWindow *videoWindow = (EbWidgetVideoWindow*)pUserData;
//        const mycp::bigint userId = videoWindow->userId();
////        ((EbWidgetVideoWindow*)pUserData)->setVisible(true);
//        break;
//    }
    case EB_CALLBACK_VOICE_VOLUME:
        ((EbWidgetVideoWindow*)pUserData)->onFPMMCallBack(nUserId, nCallBackType, lParam, wParam);
//        ((EbWidgetVideoWindow*)pUserData)->SetVoicePos(lParam);
        break;
    default:
        break;
    }
}
