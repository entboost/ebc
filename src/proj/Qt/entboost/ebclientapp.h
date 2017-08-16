#ifndef EBCLIENTAPP_H
#define EBCLIENTAPP_H

#include <QtGui>
#include <QColor>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QListWidget>
#include <QListWidgetItem>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
#include "../../../include/ebc_public.h"
#include "../../../include/colorconver.h"
#include "../../../include/EBParseSetting.h"
#include "../../../libpopusermanager/EBAppClient.h"
#include "../../../include/SqliteExt.h"
#include "../include/boost_ini.h"
using namespace entboost;
//#include "ebclocales.h"
#include "ebdefines.h"
#include "ebhttpfiledownload.h"
#include "ebccallinfo.h"
#include "ebclientappbase.h"
#include <boost/shared_ptr.hpp>
//#include <QtWebEngine/QtWebEngine>

class EbDialogEmotionSelect;
class EbDialogMainFrame;
class EbDialogViewECard;
class EbWebEngineDownloadRequested;

class EbClientApp : public EbClientAppBase
{
//    Q_OBJECT
public:
    typedef boost::shared_ptr<EbClientApp> pointer;
    explicit EbClientApp(QObject *parent=0);
    ~EbClientApp(void);
    static EbClientApp::pointer create(QObject *parent=0);

    /// 记录当前所有会话
    CLockMap<eb::bigint,EbcCallInfo::pointer> m_pCallList;
    /// 记录自动 CALL group id
    CLockMap<eb::bigint,bool> m_pAutoCallGroupIdList;
    /// 记录自动 CALL from user id
    CLockMap<eb::bigint,bool> m_pAutoCallFromUserIdList;
    CLockMap<eb::bigint,bool> m_pCancelFileList;
    eb::bigint m_nSelectCallId;
    eb::bigint m_nSelectGroupId;
    void clearSubscribeSelectInfo(void);
//    void triggeredApps(eb::bigint subId);
//    std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;

    void setMainWnd(EbDialogMainFrame *mainWnd) {m_mainWnd=mainWnd;}
    EbDialogMainFrame *mainWnd(void) const {return m_mainWnd;}
    bool setDevAppId(QObject *receiver);

    bool initApp(void);
//    void exitApp(bool bResetEbumOnly=false);
    bool onLogonSuccess(void);

//     bool setLocaleLanguage(const QString &file);
//    QString localeLanguage(const QString &defaultFile="zh-CN.json");
    void setLocalVideoIndex(int index);
    int localVideoIndex(int defaultIndex=0) const;
//    void setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase);
//    void setMainColor(const QColor& pColor, bool bUpdateDatabase);
    void updateStyleSheet(void);    /// 实现某些组件，隐藏变为显示，自动刷新UI界面

    QString urlIconFilePath(const QUrl &url);
    QString subscribeFuncUrl(eb::bigint subId, const std::string &sParameters="");
    void setEntManagerUrl(const QString &v) {m_sEntManagerUrl=v;}
    const QString& entManagerUrl(void) const {return m_sEntManagerUrl;}
    void setEnterpriseCreateUserId(eb::bigint nUserId) {m_nEnterpriseCreateUserId=nUserId;}
    eb::bigint enterpriseCreateUserId(void) const {return m_nEnterpriseCreateUserId;}
    bool isEnterpriseCreateUserId(eb::bigint nUserId) {return (nUserId>0 && nUserId==m_nEnterpriseCreateUserId)?true:false;}

    QString lineStateText(EB_USER_LINE_STATE lineState) const;
    QString lineStateText(void) const;

    bool isStatSubGroupMember(void) const {return m_bStatSubGroupMember;}
    const QString & defaultUrl(void) const {return m_sDefaultUrl;}
    int openRegister(void) const {return m_nOpenRegister;}
    bool isOpenVisitor(void) const {return m_bOpenVisitor;}
    const QString& forgetPwdUrl(void) const {return m_sForgetPwdUrl;}

    void setDefaultBrowserType(EB_BROWSER_TYPE newValue) {m_nDefaultBrowserType = newValue;}
    EB_BROWSER_TYPE defaultBrowserType(void) const {return m_nDefaultBrowserType;}
    bool setDefaultUIStyleType(EB_UI_STYLE_TYPE newValue);
    EB_UI_STYLE_TYPE defaultUIStyleType(void) const {return m_nDefaultUIStyleType;}
    void setSendKeyType(EB_SEND_KEY_TYPE v);
    EB_SEND_KEY_TYPE sendKeyType(void) const {return m_sendKeyType;}

    const QRect& deskRect(void) const {return m_deskRect;}
    const QRect& screenRect(void) const {return m_screenRect;}

    EbDialogEmotionSelect* showDialogEmotionSelect(const QPoint &pt, QObject *receiver=0);
    EbDialogViewECard * dialgoViewECard(const QWidget *wdigetValid, const QRect &rectValid, bool showImmediate=false);
    void hideViewECard(const QWidget *wdigetValid);

    bool isLogoned(void) const {return m_ebum.EB_IsLogoned();}
    bool isLogonVisitor(void) const {return m_ebum.EB_IsLogonVisitor();}
    eb::bigint logonUserId(void) const {return m_ebum.EB_GetLogonUserId();}
    tstring logonUserAccount(void) const {return m_ebum.EB_GetLogonAccount();}
    void editContactInfo(eb::bigint contactId, QWidget *parent=0);
    void editGroupInfo(eb::bigint groupId, QWidget *parent=0);
    void newGroupInfo(EB_GROUP_TYPE groupType, eb::bigint enterpriseId, eb::bigint parentGroupId, QWidget *parent=0);
    void newMemberInfo(EB_GROUP_TYPE groupType, eb::bigint groupId, const QString &groupName, QWidget *parent=0);
    void editMemberInfo(const EB_MemberInfo* pMemberInfo, QWidget *parent=0);
    void deleteDbRecord(eb::bigint sId, bool bIsAccount);
    void deleteDbRecord(eb::bigint sMsgId);
    void updateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType);
    QImage userHeadImage(eb::bigint userId, eb::bigint memberId, const tstring &account) const;
    QString userHeadFilePath(eb::bigint userId, eb::bigint memberId, const tstring &account) const;
    QImage funcImage(const EB_SubscribeFuncInfo *funcInfo) const;
    QImage memberHeadImage(const EB_MemberInfo *memberInfo) const;
    QImage contactHeadImage(const EB_ContactInfo *contactInfo) const;
    QString contactHeadFilePath(const EB_ContactInfo *contactInfo) const;
    QImage fromHeadImage(const QString &imagePath, EB_USER_LINE_STATE lineState) const;
    QString memberHeadFilePath(const EB_MemberInfo *memberInfo) const;
//    tstring groupTypeHeadFilePath(EB_GROUP_TYPE groupType) const;
    QImage groupHeadImage(eb::bigint groupId, EB_GROUP_TYPE groupType) const;
protected:
//    virtual bool event(QEvent *e);
    virtual void customEvent(QEvent *e);
public slots:

private:
    void onAppIdSuccess(QEvent *e);
    void onAppIdError(QEvent *e);
    void loadUserSetting(void);

    EbHttpFileDownload m_httpFileDownload;
//    void updateColor(bool bUpdateDatabase);
private:
//    QColor m_hotColor;      // 按钮 hot 状态
//    QColor m_preColor;      // 按钮 pre 状态

    //
    EB_BROWSER_TYPE m_nDefaultBrowserType;
    EB_UI_STYLE_TYPE m_nDefaultUIStyleType;
    EB_SEND_KEY_TYPE m_sendKeyType;

    QRect m_deskRect;
    QRect m_screenRect;

    EbDialogMainFrame *m_mainWnd;
    QObject *m_receiver;
    EbDialogEmotionSelect *m_dialogEmotionSelect;
    EbDialogViewECard *m_dialogViewECard;
    EbWebEngineDownloadRequested *m_webWngineDownloadRequested;
};

extern EbClientApp::pointer theApp;
//extern EbcLocales theLocales;

#endif // EBCLIENTAPP_H
