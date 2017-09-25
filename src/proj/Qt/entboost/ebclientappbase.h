#ifndef EBCLIENTAPPBASE_H
#define EBCLIENTAPPBASE_H

#include <QtGui>
#include <QObject>
#include <QDir>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
//#include "../../../include/ebc_public.h"
//#include "../../../include/colorconver.h"
#include "../../../include/EBParseSetting.h"
#include "../../../libpopusermanager/EBAppClient.h"
#include "../../../include/SqliteExt.h"
//#include "../include/boost_ini.h"
using namespace entboost;
#ifndef _QT_QML_
#include "ebclocales.h"
#endif
#include "ebhttpfiledownload.h"
#include "../include/colorconver.h"

//#ifndef RGB
//#define RGB(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
//#endif
//#ifndef LOBYTE
//#define LOBYTE(w)           ((unsigned char)(((unsigned int)(w)) & 0xff))
//#endif
//#ifndef GetRValue
//#define GetRValue(rgb)      (LOBYTE(rgb))
//#endif
//#ifndef GetGValue
//#define GetGValue(rgb)      (LOBYTE(((unsigned short)(rgb)) >> 8))
//#endif
//#ifndef GetBValue
//#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
//#endif

//#ifndef MAX
//#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
//#endif

//#ifndef MIN
//#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

inline bool checkCreateDir(const QString &dirName)
{
    QDir pDir1(dirName);
    if (!pDir1.exists()) {
        return pDir1.mkdir(dirName);
//        return pDir1.mkpath(dirName);
    }
    return true;
}

class EbClientAppBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userImagePath READ userImagePath)
    Q_PROPERTY(QColor mainColor READ getMainColor WRITE setMainColor NOTIFY mainColorChanged)
    Q_PROPERTY(QColor mainHoverColor READ getMainHoverColor)
    Q_PROPERTY(QColor mainPressedColor READ getMainPressedColor)
    Q_PROPERTY(bool isAppIdResponseOk READ isAppIdResponseOk NOTIFY appidResponseOkChanged)
    Q_PROPERTY(QString logonUserId READ logonUserIdString)
//    Q_PROPERTY(qint64 logonUserId READ logonUserId)
    Q_PROPERTY(QString logonAccount READ logonAccount)
    Q_PROPERTY(bool isLicenseUser READ isLicenseUser)
    Q_PROPERTY(bool isAuthContact READ isAuthContact)
    Q_PROPERTY(QString productName READ productName NOTIFY productNameChanged)
    Q_PROPERTY(QString productVersion READ productVersion)
    Q_PROPERTY(bool isOpenVisitor READ isOpenVisitor)
    Q_PROPERTY(bool isOpenRegister READ isOpenRegister)
    Q_PROPERTY(bool isOpenRegisterEnterprise READ isOpenRegisterEnterprise)
    Q_PROPERTY(QString forgetPwdUrl READ forgetPwdUrl)
    Q_PROPERTY(QString connectSettingServer READ ebcIniFileServer WRITE setEbcIniFileServer)

    Q_PROPERTY(QString lastError READ lastError WRITE setLastError)
    Q_PROPERTY(QString appDataLocation READ appDataLocation)
    Q_PROPERTY(QString appDataTempLocation READ appDataTempLocation)
    Q_PROPERTY(QString appDataImageTempLocation READ appDataImageTempLocation)
    Q_PROPERTY(QString appDataCefCacheTemp READ appDataCefCacheTemp)
    Q_PROPERTY(QString appDatasPath READ appDatasPath)

    Q_PROPERTY(QString appDatasPath READ appDatasPath)
    Q_PROPERTY(QString appUsersPath READ appUsersPath)
    Q_PROPERTY(QString appImgPath READ appImgPath)

public:
    explicit EbClientAppBase(QObject *parent = nullptr);
    virtual ~EbClientAppBase(void);

    CEBParseSetting m_setting;
    CEBAppClient m_ebum;
    CSqliteCdbc::pointer m_sqliteEbc;
    CSqliteCdbc::pointer m_sqliteUser;

    Q_INVOKABLE bool initApp(void);
    Q_INVOKABLE void exitApp(bool bResetEbumOnly=false);
    Q_INVOKABLE bool fileExist(const QString &filePath);
    Q_INVOKABLE qint64 fileSize(const QString &filePath);
    bool setDevAppId(QObject *receiver);

    void setLastError(const QString &v) {m_lastError=v;}
    const QString &lastError(void) const {return m_lastError;}

    void deleteDbRecord(eb::bigint sId, bool bIsAccount);
    Q_INVOKABLE void deleteDbRecord(qint64 sMsgId);
    int memberSubType(const EB_MemberInfo *memberInfo);
    int memberSubType(EB_GROUP_TYPE groupType, eb::bigint groupId, eb::bigint memberUserId);

    bool setLocaleLanguage(const QString &file);
   QString localeLanguage(const QString &defaultFile="zh-CN.json");

    const QString &appDataLocation(void) const {return m_appDataLocation;}
    const QString &appDataTempLocation(void) const {return m_appDataTempLocation;}
    const QString &appDataImageTempLocation(void) const {return m_appDataImageTempLocation;}
    const QString &appDataCefCacheTemp(void) const {return m_appDataCefCacheTemp;}

    const QString &appDatasPath(void) const {return m_appDatasPath;}
    const QString &appUsersPath(void) const {return m_appUsersPath;}
    const QString &appImgPath(void) const {return m_appImgPath;}
    const QString &appResPath(void) const {return m_appResPath;}
    const QString &getAppLocalesPath(void) const {return m_appLocalesPath;}
    const QString &getSettingFile(void) const {return m_settingFile;}
    const QString &getSettingIntFile(void) const {return m_settingIniFile;}
    const QString &getEbcIniFile(void) const {return m_ebcIniFile;}
    void setEbcIniFileServer(const QString &newServer);
    QString ebcIniFileServer(void) const;
    const QString &userMainPath(void) const {return m_userMainPath;}
    const QString &userImagePath(void) const {return m_userImagePath;}
    const QString &userFilePath(void) const {return m_userFilePath;}
    const QString &userSettingIniFile(void) const {return m_userSettingIniFile;}
    Q_INVOKABLE void setNewMessageNotify(bool v);
    Q_INVOKABLE bool newMessageNotify(bool bDefault=true) const;
    Q_INVOKABLE void setDetailMessageNotify(bool v);
    Q_INVOKABLE bool detailMessageNotify(bool bDefault=true) const;
    Q_INVOKABLE void setVoiceNotify(bool v);
    Q_INVOKABLE bool voiceNotify(bool bDefault=true) const;
    Q_INVOKABLE void setVibrateNotify(bool v);
    Q_INVOKABLE bool vibrateNotify(bool bDefault=true) const;

    const QColor &getMainColor(void) const {return this->m_mainColor;}
    const QColor &getMainHoverColor(void) const {return this->m_mainHoverColor;}
    const QColor &getMainPressedColor(void) const {return this->m_mainPressedColor;}
    void setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase);
    void setMainColor(const QColor& pColor);
    void setMainColor(const QColor& pColor, bool bUpdateDatabase);

    void setEntManagerUrl(const QString &v) {m_sEntManagerUrl=v;}
    const QString& entManagerUrl(void) const {return m_sEntManagerUrl;}
    void setEnterpriseCreateUserId(eb::bigint nUserId) {m_nEnterpriseCreateUserId=nUserId;}
    eb::bigint enterpriseCreateUserId(void) const {return m_nEnterpriseCreateUserId;}
    bool isEnterpriseCreateUserId(eb::bigint nUserId) {return (nUserId>0 && nUserId==m_nEnterpriseCreateUserId)?true:false;}

    Q_INVOKABLE bool isLogoned(void) const {return m_ebum.EB_IsLogoned();}
    Q_INVOKABLE bool isLogonVisitor(void) const {return m_ebum.EB_IsLogonVisitor();}
    qint64 logonUserId(void) const {return m_ebum.EB_GetLogonUserId();}
    QString logonUserIdString(void) const {return QString::number(m_ebum.EB_GetLogonUserId());}
    QString logonAccount(void) const {return QString::fromStdString(m_ebum.EB_GetLogonAccount().string());}
    tstring logonUserAccount(void) const {return m_ebum.EB_GetLogonAccount();}

    void updateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType);

    bool isAppIdResponseOk(void) const {return m_isAppIdResponseOk;}
    bool isLicenseUser(void) const {return m_bLicenseUser;}
    const QString &productName(void) const {return m_sProductName;}
    const QString &productVersion(void) const {return m_sProductVersion;}
    bool isSendRegMail(void) const {return m_bSendRegMail;}
    int saveConversations(void) const {return m_nSaveConversations;}
    bool isSaveConversationLocal(void) const {return (m_nSaveConversations&1)==1;}
    bool isSaveConversationServer(void) const {return (m_nSaveConversations&2)==2;}
    bool isAuthContact(void) const {return m_bAuthContact;}
    eb::bigint deployId(void) const {return m_nDeployId;}
    int licenseType(void) const {return m_nLicenstType;}
    int ebServerVersion(void) const {return m_nEBServerVersion;}
    eb::bigint groupMsgSubId(void);
    eb::bigint findAppSubId(void);
    eb::bigint autoOpenSubId(void) const {return m_nAutoOpenSubId;}
    eb::bigint myCollectionSugId(void);
    bool isAutoHideMainFrame(void) const {return m_bAutoHideMainFrame;}
    bool isHideMainFrame(void) const {return m_bHideMainFrame;}
    bool isDisableuserCloudDrive(void) const {return m_bDisableUserCloudDrive;}
    bool isDisableGroupShareCloud(void) const {return m_bDisableGroupShareCloud;}
    Q_INVOKABLE bool isDisableModifyPasswd(void) const {return m_bDisableModifyPasswd;}
    bool isDisableVideo(void) const {return m_bDisableVideo;}
    bool isDisableRemoteDesktop(void) const {return m_bDisableRemoteDesktop;}
    bool isDisableAccountEdit(void) const {return m_bDisableAccountEdit;}

    bool isStatSubGroupMember(void) const {return m_bStatSubGroupMember;}
    const QString &defaultUrl(void) const {return m_sDefaultUrl;}
    int openRegister(void) const {return m_nOpenRegister;}
    bool isOpenRegister(void) const {return m_nOpenRegister>0?true:false;}
    bool isOpenRegisterEnterprise(void) const {return (m_nOpenRegister&0x2)==0x2?true:false;}
    bool isOpenVisitor(void) const {return m_bOpenVisitor;}
    const QString &forgetPwdUrl(void) const {return m_sForgetPwdUrl;}

    Q_INVOKABLE void setRequestLogout(bool v) {m_requestLogout=v;}
    Q_INVOKABLE bool isRequestLogout(void) const {return m_requestLogout;}

signals:
    void mainColorChanged(void);
    void appidResponseOkChanged(void);
    void productNameChanged(void);

public slots:

protected:
    EbHttpFileDownload m_httpFileDownload;
    virtual void onAppIdSuccess(QEvent *e);
    virtual void onAppIdError(QEvent *e);
    bool onLogonSuccess(void);

    void updateColor(bool bUpdateDatabase);
protected:
    QString m_styleSheet;
    QString m_lastError;
    QString m_appDataLocation;      ///
    QString m_appDataTempLocation;      /// "[m_appDataLocation]/temp"
    QString m_appDataImageTempLocation;      /// "[m_appDataLocation]/image_temp"
    QString m_appDataCefCacheTemp;      /// "[m_appDataLocation]/cef_cache_temp"
    QString m_appDatasPath;      /// "/datas"
    QString m_appUsersPath;
    QString m_appImgPath;
    QString m_appResPath;
    QString m_appLocalesPath;
    QString m_appBodbPath;
    QString m_settingFile;      /// setting
    QString m_settingIniFile;   /// setting.ini
    QString m_ebcIniFile;       /// ebc.ini
    QString m_userMainPath;
    QString m_userImagePath;
    QString m_userFilePath;
    QString m_userSettingIniFile;
    QColor m_mainColor;
    QColor m_mainHoverColor;
    QColor m_mainPressedColor;

    QString m_sEntManagerUrl;
    eb::bigint m_nEnterpriseCreateUserId;
    QString m_sProductName;
    QString m_sProductVersion;
    bool m_isAppIdResponseOk;
    std::string m_sAccountPrefix;
    bool m_bLicenseUser;
    bool m_bSendRegMail;
    int m_nSaveConversations;
    bool m_bAuthContact;
    eb::bigint m_nDeployId;
    int m_nLicenstType;         /// 许可类型；0=未授权；1=终身授权；2=时间授权
    int m_nEBServerVersion;
    eb::bigint m_nGroupMsgSubId;
    eb::bigint m_nFindAppSubId;
    eb::bigint m_nAutoOpenSubId;
    eb::bigint m_myCollectionSubId;

    bool m_bAutoHideMainFrame;
    bool m_bHideMainFrame;
    bool m_bDisableUserCloudDrive;
    bool m_bDisableGroupShareCloud;
    bool m_bDisableModifyPasswd;
    bool m_bDisableVideo;
    bool m_bDisableRemoteDesktop;
    bool m_bDisableAccountEdit;
    bool m_bDisableMsgReceipt;
    bool m_bStatSubGroupMember;
    QString m_sDefaultUrl;
    int m_nOpenRegister;    /// 0x1=支持注册个人；0x2=支持注册企业；0x3=支持注册个人和企业
    bool m_bOpenVisitor;
    QString m_sForgetPwdUrl;

    QObject *m_receiver;
    bool m_requestLogout;
};

#ifndef _QT_QML_
extern EbcLocales theLocales;
#endif

#endif // EBCLIENTAPPBASE_H
