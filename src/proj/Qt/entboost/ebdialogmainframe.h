#ifndef EBDIALOGMAINFRAME_H
#define EBDIALOGMAINFRAME_H

#include <ebdialogbase.h>
#include <QMenu>
#include <QSystemTrayIcon>
#include "ebwidgetmygroup.h"
#include "ebwidgetmycontact.h"
#include "ebwidgetmysession.h"
#include "ebwidgetmyenterprise.h"
#include "ebdialogchatbase.h"
#include <eblabel.h>

namespace Ui {
class EbDialogMainFrame;
}
class EbDialogFrameList;
class EbDialogMessageTip;
class EbDialogMyCenter;
class EbWidgetAppBar;
class EbLineEdit;
class EbWidgetSearchResult;
class EbDialogFileManager;
class EbDialogPopTip;

class EbDialogMainFrame : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogMainFrame(QWidget *parent = 0);
    ~EbDialogMainFrame();

    void updateLocaleInfo(void);
    void refreshSkin(void);
    bool requestLogout(void) const {return m_requestLogout;}
public slots:
    void onClickedPushButtonSetting(void);
    void onTriggeredActionSelectColor(void);
    void updateLineState(void);
    void onClickedLabelUserImage(void);
    void onClickedLineState(void);
    void onClickedMenuLineState(void);
    void onTriggeredActionOpenWorkFrame(void);
    void onTriggeredActionMyCollection(void);
    void onTriggeredActionLogout(void);
    void onTriggeredActionExitApp(void);
    void onClickedPushButtonMyCenter(void);
    void onClickedPushButtonFileManager(void);
    void onClickedPushButtonApps(void);
    void onTriggeredActionApps(void);
    void onClickedPushButtonMyGroup(void);
    void onClickedPushButtonMyContact(void);
    void onClickedPushButtonMySession(void);
    void onClickedPushButtonMyEnterprise(void);
    void onClickedPushButtonMyApp(void);
    void updateMyButton(const QPushButton* fromButton);

    void onClickedSubApp(const EB_SubscribeFuncInfo &funcInfo);
    void openUrl(bool saveBrowseTitle, const QString &appUrl, const QString &postData="", int insertOffset=-1);
    bool openSubscribeFuncWindow(eb::bigint subscribeId, const std::string &postData="", const std::string &parameters="");
    bool openSubscribeFuncWindow(const EB_SubscribeFuncInfo &funcInfo, const std::string &postData="", const std::string &parameters="");

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void onSearchEditTextChange(const QString &text);
    void onSearchEditKeyPressEsc(void);
    void onSearchEditKeyPressEnter(const QString &text);
    void onSearchEditKeyPressDown(void);

    void onSearchFirst(const QString &url);
    void onClickedSearchResultUrl(const QString &url);
    void onListResultsKeyPressFirstItemUp(void);
    void onListResultsKeyPressEsc(void);

    void onLoadLocalUnreadMsg(void);
    void onClearAutoCallInfo(void);
    void processDatas(void);
protected:
    virtual void accept(void);
    virtual void reject(void);
    virtual void resizeEvent(QResizeEvent *e);
//    virtual void closeEvent(QCloseEvent *e);

//    virtual bool event(QEvent *e);
    virtual void customEvent(QEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual bool eventFilter(QObject *obj, QEvent *e);
    virtual void timerEvent( QTimerEvent *e);

    void createMenuData(void);
    bool creatTrayIcon(void);
    void changeTrayTooltip(void);
private:
    void BuildHeadRect(void);

    void addSubUnreadMsg(mycp::bigint subId, bool sendToWorkFrame);
    void setSubUnreadMsg(mycp::bigint subId, size_t unreadMsgCount, bool sendToWorkFrame);
    void onBroadcastMsg(QEvent *e);
    void onAreaInfo(QEvent *e);

    void onUserStateChange(QEvent *e);
    void onMemberHeadChange(QEvent *e);
    void onContactHeadChange(QEvent *e);
    /// 视频聊天
    void onVRequestResponse(QEvent *e);
    void onVAckResponse(QEvent *e);
    void onVideoRequest(QEvent *e);
    void onVideoAccept(QEvent *e);
    void onVideoReject(QEvent *e);
    void onVideoTimeout(QEvent *e);
    void onVideoClose(QEvent *e);
    /// 聊天消息
    void onMsgReceipt(QEvent *e);
    void onSendRich(QEvent *e);
    bool onReceiveRich(QEvent *e);
    void onSendingFile(QEvent *e);
    void onSentFile(QEvent *e);
    void onCancelFile(QEvent *e);
    bool onReceivingFile(QEvent *e);
    void onReceivedFile(QEvent *e);
    void onFilePercent(QEvent *e);
    void onSave2Cloud(QEvent *e);
    /// 聊天会话
    void CreateFrameList(bool show);
    EbDialogChatBase::pointer getDialogChatBase(const EbcCallInfo::pointer &callInfo, bool show=true, bool ownerCall=false);
    EbDialogChatBase::pointer getDialogChatBase(eb::bigint callId, bool remove=false);
    void onCallConnected(QEvent *e);
    void onCallError(QEvent *e);
    void onCallHangup(QEvent *e);
    void onCallAlerting(QEvent *e);
    void onCallIncoming(QEvent *e);
    /// 联系人
    void onUGInfo(QEvent *e);
    void onUGDelete(QEvent *e);
    void onContactDelete(QEvent *e);
    void onContactInfo(QEvent *e);
    void onContactStateChanged(QEvent *e);
    void onAcceptAddContact(QEvent *e);
    void onRejectAddContact(QEvent *e);
    void onRequestAddContact(QEvent *e);
    /// 组织结构
    void onEditInfoResponse(QEvent *e);
    void onMemberEditResponse(QEvent *e);
    void onMemberDelete(QEvent *e);
    void setWindowTitleAndTrayInfo(void);
    void onMemberInfo(QEvent *e);
    void onRejectAdd2Group(QEvent *e);
    void onInviteAdd2Group(QEvent *e);
    void onRequestAdd2Group(QEvent *e);
    void onExitGroup(QEvent *e);
    void onRemoveGroup(QEvent *e);
    void onGroupEditResponse(QEvent *e);
    void onGroupDelete(QEvent *e);
    void onGroupInfo(QEvent *e);
    void onEnterpriseInfo(QEvent *e);
    /// 登录
    void onLogonSuccess(QEvent *e);
    void onLogonTimeout(QEvent *e);
    void onLogonError(QEvent *e);
    void onOnlineAnother(QEvent *e);

    bool checkEventData(QEvent *e);
    void checkOneSecond(void);
    void checkCallExit(void);
    void saveCallRecord(eb::bigint callId, eb::bigint groupId, const EB_AccountInfo &fromAccountInfo);
private:
    Ui::EbDialogMainFrame *ui;
    EbLabel* m_labelUserImage;
    EbLabel* m_labelLinState;
    QMenu* m_menuSetting;
    QMenu* m_menuLineState;
    QMenu* m_menuContext;
    QMenu * m_menuApps;
    QAction * m_actionMyCollection;
    QRect m_rectHead;
    EbDialogMyCenter *m_pDlgMyCenter;
    EbDialogFrameList *m_pDlgFrameList;
    EbDialogMessageTip *m_pDlgMsgTip;
    EbDialogPopTip *m_dialogBroadcaseMsg;
    EbDialogFileManager *m_dialogFileManager;
    EbWidgetMyGroup *m_widgetMyGroup;
    EbWidgetMyContact *m_widgetMyContact;
    EbWidgetMySession *m_widgetMySession;
    EbWidgetMyEnterprise *m_widgetMyEnterprise;
    EbWidgetAppBar *m_widgetMainAppBar;
    EbLineEdit *m_lineEditSearch;
    EbWidgetSearchResult *m_widgetSearchResult;
//    bool m_canSearch;
    bool m_canUpdateSearchFirst;
    int m_timerOneSecond;
    QSystemTrayIcon * m_trayIcon;
    bool m_requestLogout;
};

#endif // EBDIALOGMAINFRAME_H
