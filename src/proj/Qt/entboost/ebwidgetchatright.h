#ifndef EBWIDGETCHATRIGHT_H
#define EBWIDGETCHATRIGHT_H

#include "ebclientapp.h"
//#include <QWidget>
#include <ebdialogworkframe.h>

class EbWidgetChatRight : public EbDialogWorkFrame
{
    Q_OBJECT
public:
    explicit EbWidgetChatRight(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);
    virtual ~EbWidgetChatRight(void);

    void onExitChat(bool bHangup);
    void onUserExit(eb::bigint userId, bool exitGroup);
//    void lineStateChange(eb::bigint userId, EB_USER_LINE_STATE lineState);
    void setCallInfo(const EbcCallInfo::pointer &callInfo);
    void onMsgReceipt(const CCrRichInfo *crMsgInfo,int ackType);
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    void getProcessing(bool &outVideoProcessing, bool &outFileProcessing, bool &outDesktopProcessing) const;
    /// 文件传输
    void onSendingFile(const CCrFileInfo *fileInfo);
    void onReceivingFile(const CCrFileInfo *fileInfo);
    void onReceivedFile(const CCrFileInfo *fileInfo);
    void onFilePercent(const CChatRoomFilePercent *filePercent);
    void deleteTranFile(eb::bigint msgId);
    /// 聊天记录
    void showMsgRecord(void);
    /// 视频聊天
    void onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
signals:

public slots:
    void onOpenSubId(eb::bigint subId);
    virtual void resizeEvent(QResizeEvent *e);
private:
    EbWidgetVideoFrame *openVideoFrame(void);
    EbWidgetFileTranList *openTranFile(void);
//    void updateGroupUsers(void);
private:
    EbcCallInfo::pointer m_callInfo;
//    DialogWorkFrame *m_workFrame;
};

#endif // EBWIDGETCHATRIGHT_H
