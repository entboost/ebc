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
#ifndef Q_OS_ANDROID
#include "ebclocales.h"
#endif

#ifndef RGB
#define RGB(r,g,b)          ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((unsigned char)(((unsigned int)(w)) & 0xff))
#endif
#ifndef GetRValue
#define GetRValue(rgb)      (LOBYTE(rgb))
#endif
#ifndef GetGValue
#define GetGValue(rgb)      (LOBYTE(((unsigned short)(rgb)) >> 8))
#endif
#ifndef GetBValue
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

inline bool checkCreateDir(const QString &dirName)
{
    QDir pDir1(dirName);
    if (!pDir1.exists()) {
        return pDir1.mkdir(dirName);
    }
    return true;
}

class EbClientAppBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError WRITE setLastError)
    Q_PROPERTY(QString appDataLocation READ appDataLocation)
    Q_PROPERTY(QString appDataTempLocation READ appDataTempLocation)
    Q_PROPERTY(QString appDataImageTempLocation READ appDataImageTempLocation)
    Q_PROPERTY(QString appDataCefCacheTemp READ appDataCefCacheTemp)
    Q_PROPERTY(QString appDataPath READ getAppDataPath)
public:
    explicit EbClientAppBase(QObject *parent = nullptr);
    virtual ~EbClientAppBase(void);

    CEBParseSetting m_setting;
    CEBAppClient m_ebum;
    CSqliteCdbc::pointer m_sqliteEbc;
    CSqliteCdbc::pointer m_sqliteUser;

    bool initApp(void);
    void exitApp(bool bResetEbumOnly=false);

    void setLastError(const QString &v) {m_lastError=v;}
    const QString &lastError(void) const {return m_lastError;}

    bool setLocaleLanguage(const QString &file);
   QString localeLanguage(const QString &defaultFile="zh-CN.json");

    const QString &appDataLocation(void) const {return m_appDataLocation;}
    const QString &appDataTempLocation(void) const {return m_appDataTempLocation;}
    const QString &appDataImageTempLocation(void) const {return m_appDataImageTempLocation;}
    const QString &appDataCefCacheTemp(void) const {return m_appDataCefCacheTemp;}

    const QString &getAppDataPath(void) const {return m_appDataPath;}
    const QString &getAppUsersPath(void) const {return m_appUsersPath;}
    const QString &getAppImgPath(void) const {return m_appImgPath;}
    const QString &getAppLocalesPath(void) const {return m_appLocalesPath;}
    const QString &getSettingFile(void) const {return m_settingFile;}
    const QString &getSettingIntFile(void) const {return m_settingIniFile;}
    const QString &getEbcIniFile(void) const {return m_ebcIniFile;}
    const QString &userMainPath(void) const {return m_userMainPath;}
    const QString &userImagePath(void) const {return m_userImagePath;}
    const QString &userFilePath(void) const {return m_userFilePath;}
    const QString &userSettingIniFile(void) const {return m_userSettingIniFile;}

    const QColor& getMainColor(void) const {return this->m_mainColor;}
//    const QColor& getHotColor(void) const {return this->m_hotColor;}
//    const QColor& getPreColor(void) const {return this->m_preColor;}
    void setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase);
    void setMainColor(const QColor& pColor, bool bUpdateDatabase);

    bool isAppIdResponseOk(void) const {return m_isAppIdResponseOk;}
    bool isLicenseUser(void) const {return m_bLicenseUser;}
    const QString & productName(void) const {return m_sProductName;}
    bool sendRegMail(void) const {return m_bSendRegMail;}
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
    bool isDisableModifyPasswd(void) const {return m_bDisableModifyPasswd;}
    bool isDisableVideo(void) const {return m_bDisableVideo;}
    bool isDisableRemoteDesktop(void) const {return m_bDisableRemoteDesktop;}
    bool isDisableAccountEdit(void) const {return m_bDisableAccountEdit;}

signals:

public slots:

protected:
    void updateColor(bool bUpdateDatabase);
protected:
    QString m_styleSheet;
    QString m_lastError;
    QString m_appDataLocation;      ///
    QString m_appDataTempLocation;      /// "[m_appDataLocation]/temp"
    QString m_appDataImageTempLocation;      /// "[m_appDataLocation]/image_temp"
    QString m_appDataCefCacheTemp;      /// "[m_appDataLocation]/cef_cache_temp"
    QString m_appDataPath;      /// "/datas"
    QString m_appUsersPath;
    QString m_appImgPath;
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

    QString m_sEntManagerUrl;
    eb::bigint m_nEnterpriseCreateUserId;
    QString m_sProductName;
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
    int m_nOpenRegister;
    bool m_bOpenVisitor;
    QString m_sForgetPwdUrl;

};

#ifndef Q_OS_ANDROID
extern EbcLocales theLocales;
#endif

#endif // EBCLIENTAPPBASE_H
