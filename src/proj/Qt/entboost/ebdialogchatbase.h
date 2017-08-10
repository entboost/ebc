#ifndef EBDIALOGCHATBASE_H
#define EBDIALOGCHATBASE_H

#include "ebclientapp.h"
#include <QSplitter>
#include <QKeyEvent>
#include <ebdialogbase.h>

namespace Ui {
class EbDialogChatBase;
}
class EbTextBrowser;
class EbWidgetChatInput;
class EbWidgetChatRight;
class EbDialogSelectUser;

class EbDialogChatBase : public EbDialogBase
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbDialogChatBase> pointer;
    explicit EbDialogChatBase(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);
    virtual ~EbDialogChatBase(void);
    static EbDialogChatBase::pointer create(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);

    void updateLocaleInfo(void);
    void timerCheckState(void);
    /// 设置输入框 focus
    void setFocusInput(void);
    void scrollToEnd(void);
    void setCallInfo(const EbcCallInfo::pointer &callInfo);    /// 主要用于更新 CALLID
    EbcCallInfo::pointer callInfo(void) const {return m_callInfo;}
    mycp::bigint callId(void) const {return m_callInfo->callId();}
    mycp::bigint groupId(void) const {return m_callInfo->groupId();}
    mycp::bigint fromUserId(void) const {return m_callInfo->fromUserId();}
    void setOwnerCall(bool b) {m_bOwnerCall = b;}
    bool isOwnerCall(void) const {return m_bOwnerCall;}
    bool clearUnreadMsg(bool fromUserClick);
    int getUnreadMsgCount(void) const;

    void onUserExitRoom(eb::bigint userId, bool exitDep);
    void onAlertingCall(void);
    /// RICH聊天消息
    void onSendRich(const CCrRichInfo *crMsgInfo, EB_STATE_CODE nState);
    void onReceiveRich(const CCrRichInfo *crMsgInfo, QString *sOutFirstMsg1, QString *sOutFirstMsg2);
    void onUserLineStateChange(eb::bigint groupId, eb::bigint userId, EB_USER_LINE_STATE bLineState);
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    bool onMemberHeadChange(const EB_MemberInfo *memberInfo);
    bool onContactHeadChange(const EB_ContactInfo *contactInfo);
    void onGroupInfo(const EB_GroupInfo *groupInfo);
    void onRemoveGroup(const EB_GroupInfo *groupInfo);
    void onRemoveMember(const EB_GroupInfo *groupInfo, mycp::bigint memberId, mycp::bigint memberUserId);
    void onMsgReceipt(const CCrRichInfo *crMsgInfo, int nAckType);

    /// 文件传输
    void onSendingFile(const CCrFileInfo *fileInfo);
    bool onSentFile(const CCrFileInfo *fileInfo);
    void onCancelFile(const CCrFileInfo *fileInfo, bool changeP2PSending);
    void onReceivingFile(const CCrFileInfo *fileInfo, QString *sOutFirstMsg=0);
    void onReceivedFile(const CCrFileInfo *fileInfo);
    void onFilePercent(const CChatRoomFilePercent *filePercent);
    void onSave2Cloud(const CCrFileInfo *fileInfo);
    /// 视频聊天
    void onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue);
    void onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoTimeout(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);
    void onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo);

    const QString &fromName(void) const {return m_sFromName;}
    const QString &fullName(void) const {return m_sFullName;}
    const QString &shortName(void) const {return m_sShortName;}
    const QString &fromDescription(void) const {return m_sFromDescription;}
    const QString &imageMd5(void) const {return m_imageMd5;}
    const QImage &fromImage(void) const {return m_fromImage;}
    EB_USER_LINE_STATE fromLineState(void) const {return m_nFromLineState;}
//    bool fromIsOffLineState(void) const {return m_nFromLineState==EB_LINE_STATE_OFFLINE || m_nFromLineState==EB_LINE_STATE_UNKNOWN;}
    bool isOnlineState(void) const {return m_nFromLineState>=EB_LINE_STATE_BUSY?true:false;}
    bool isGroupChat(void) const {return (m_callInfo->m_pCallInfo.m_sGroupCode>0)?true:false;}
    EB_GROUP_TYPE groupType(void) const {return m_nGroupType;}
public slots:
    void onClickedInputClose(void);
    void onClickedInputMsgRecord(void);
    void onClickedButtonAddUser(void);
    void onClickedButtonSendFile(void);
    void onClickedButtonVideoChat(void);
    void onTriggeredActionSendECard(void);
    void onTriggeredActionChatApps(void);
    void onClickedButtonChatApps(void);
    void exitChat(bool hangup);
    void onClickedButtonExitChat(void);
    void onOpenSubId(eb::bigint subId);
signals:
    void clickedClose(void);
protected:
    virtual bool onBeforeClickedPushButtonSysMin(void);
    virtual bool onBeforeClickedPushButtonSysMax(void);
    bool requestClose(bool checkOnly);
    virtual void reject(void);

    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
private:
    void updateSize(void);
    void processMsg(bool bReceive,const CCrRichInfo *pCrMsgInfo,EB_STATE_CODE nState,QString *sOutFirstMsg1=0,QString *sOutFirstMsg2=0);
    bool processFile(bool bReceive,const CCrFileInfo *fileInfo);

    void updateFromInfo(void);
    void createDialogSelectUser(void);
private:
    Ui::EbDialogChatBase *ui;
    QSplitter *m_splitterMain;     /// 先把主界面，分左右二边
    QSplitter *m_splitterInput;    /// 再把左边分上下
    EbDialogSelectUser *m_dialogSelectUser;
    EbTextBrowser *m_textBrowserMessage;
    EbWidgetChatInput *m_widgetChatInput;
    EbWidgetChatRight *m_widgetChatRight;
    EbcCallInfo::pointer m_callInfo;
    bool m_bOwnerCall;
    QString m_sFromName;
    QString m_sFullName;
    QString m_sShortName;
    QString m_sFromDescription;
    QString m_imageMd5;
    QImage m_fromImage;
    EB_USER_LINE_STATE m_nFromLineState;
    EB_GROUP_TYPE m_nGroupType;
//    CLockMap<mycp::bigint,bool> m_pPrevReceivedFileMsgIdList;   /// msgid->
    QMenu *m_menuChatApps;
};
const EbDialogChatBase::pointer EbDialogChatBaseNull;

#endif // EBDIALOGCHATBASE_H
