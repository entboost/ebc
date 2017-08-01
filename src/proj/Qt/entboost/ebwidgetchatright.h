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

    void exitChat(bool bHangup);
    void onUserExit(eb::bigint userId, bool exitDep);
//    void lineStateChange(eb::bigint userId, EB_USER_LINE_STATE lineState);
    void setCallInfo(const EbcCallInfo::pointer &callInfo);
    void onMsgReceipt(const CCrRichInfo *crMsgInfo,int ackType);
    void onSendingFile(const CCrFileInfo *fileInfo);
    void onReceivingFile(const CCrFileInfo *fileInfo);
    void onReceivedFile(const CCrFileInfo *fileInfo);
    void onFilePercent(const CChatRoomFilePercent *filePercent);
    void deleteTranFile(eb::bigint msgId);
    void onMemberInfo(const EB_MemberInfo* memberInfo, bool changeLineState);
    void getProcessing(bool &outVideoProcessing, bool &outFileProcessing, bool &outDesktopProcessing) const;
    void showMsgRecord(void);
signals:

public slots:
    void onOpenSubId(eb::bigint subId);
    virtual void resizeEvent(QResizeEvent *e);
private:
    EbWidgetFileTranList * openTranFile(void);
//    void updateGroupUsers(void);
private:
    EbcCallInfo::pointer m_callInfo;
//    DialogWorkFrame * m_workFrame;
};

#endif // EBWIDGETCHATRIGHT_H
