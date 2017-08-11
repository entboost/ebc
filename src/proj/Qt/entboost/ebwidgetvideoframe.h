#ifndef EBWIDGETVIDEOFRAME_H
#define EBWIDGETVIDEOFRAME_H

#include <ebclientapp.h>
#include <QWidget>
#include <QPushButton>
#include <boost/thread/shared_mutex.hpp>
#include "ebwidgetvideowindow.h"

#define MAX_VIDEO_COUNT 12

class  EbWidgetVideoFrame : public QWidget
{
    Q_OBJECT
public:
    explicit  EbWidgetVideoFrame(const EbcCallInfo::pointer &callInfo, QWidget *parent = nullptr);

    void updateLocaleInfo(void);
    bool IsInFVideoRequest(void) const {return m_bInFVideoRequest;}
    bool isEmpty(void) const;
    int videoCount(void) const;
    void onExitChat(bool hangup);
    bool requestClose(void) const;

    void onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
signals:
    void closeFrame(void);
public slots:
    void onClickedButtonOn(void);
    void onClickedButtonAccept(void);
    void onClickedButtonEnd(void);
    void onClickedButtonCancel(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    bool isMyOnVideo(void) const;
    int getVideoIndex(eb::bigint nVideoUserId) const;
    int closeUserVideo(eb::bigint nVideoUserId);
    int getNullVideoIndex(eb::bigint userId);
    void openUserVideo(eb::bigint nVideoUserId);
    void doVideoDisonnecte(bool bHideOnly=false, eb::bigint nOnlyHideUid=0);
    void doVideoConnected(void);
    static void EBVideoDataCallBack(eb::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam, void *pUserData);
private:
    EbcCallInfo::pointer m_callInfo;
    QPushButton *m_buttonOn;        /// for 群组多人视频聊天
    QPushButton *m_buttonAccept;    /// for 一对一视频聊天
    QPushButton *m_buttonEnd;
    QPushButton *m_buttonCancel;

    bool m_bInFVideoRequest;
    bool m_bInVideoConference;
    std::vector<QString> m_pVideoDevices;
    boost::shared_mutex m_mutex;
    EbWidgetVideoWindow::pointer m_pUserVideo[MAX_VIDEO_COUNT];
//    eb::bigint m_nVideoUserId[MAX_VIDEO_COUNT];
//    QRect m_rectUser[MAX_VIDEO_COUNT];
};

#endif // EBWIDGETVIDEOFRAME_H
