#include "ebclientappbase.h"
#include <QFileInfo>
#ifndef _QT_QML_
#include <QApplication>
#include "ebmessagebox.h"
#endif
#include "../include/boost_ini.h"
#ifndef _QT_QML_
EbcLocales theLocales;
#endif
#include "ebdefines.h"

EbClientAppBase::EbClientAppBase(QObject *parent) : QObject(parent)
  , m_mainColor(0,162,232)
  , m_mainHoverColor(m_mainColor)
  , m_mainPressedColor(m_mainColor)
  , m_nEnterpriseCreateUserId(0)
  , m_isAppIdResponseOk(false)
  , m_bLicenseUser(false)
  , m_bSendRegMail(false)
  , m_nSaveConversations(3)
  , m_bAuthContact(false)
  , m_nDeployId(0)
  , m_nLicenstType(0)
  , m_nEBServerVersion(0)
  , m_nGroupMsgSubId(0)
  , m_nFindAppSubId(0)
  , m_nAutoOpenSubId(0)
  , m_myCollectionSubId(0)
  , m_bAutoHideMainFrame(false)
  , m_bHideMainFrame(false)
  , m_bDisableUserCloudDrive(false)
  , m_bDisableGroupShareCloud(false)
  , m_bDisableModifyPasswd(false)
  , m_bDisableVideo(false)
  , m_bDisableRemoteDesktop(false)
  , m_bDisableAccountEdit(false)
  , m_bDisableMsgReceipt(false)
  , m_bStatSubGroupMember(false)
  , m_nOpenRegister(0)
  , m_bOpenVisitor(false)

  , m_receiver(0)
  , m_requestLogout(false)
{
#ifdef Q_OS_ANDROID
//    const QString writablePathTemp1 = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//    const QString writablePathTemp2 = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
//    const QString writablePathTemp3 = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
//    const QString writablePathTemp4 = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    /// 	writablePathTemp5	"/storage/emulated/0"	QString
//    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
//    checkCreateDir(writablePath);
//    writablePath += "/entboost";
//    checkCreateDir(writablePath);
//    QString writablePath = "/entboost";
//    checkCreateDir(writablePath);
//    writablePath += "/ebquick";
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    checkCreateDir(writablePath);
    qDebug() << "writablePath: " << writablePath;
    const QString absolutePath = QDir(writablePath).absolutePath();
#else
    const QString absolutePath = qApp->applicationDirPath();
#endif
    checkCreateDir(absolutePath);
    m_appDatasPath = absolutePath + "/datas";
    checkCreateDir(m_appDatasPath);
    m_appUsersPath = absolutePath + "/users";
    checkCreateDir(m_appUsersPath);
    m_appImgPath = absolutePath + "/img";
    checkCreateDir(m_appImgPath);
    m_appResPath = absolutePath + "/res";
    checkCreateDir(m_appResPath);
    m_appBodbPath = m_appDatasPath + "/bodb";
    checkCreateDir(m_appBodbPath);
    m_appLocalesPath = m_appDatasPath + "/locales";
    checkCreateDir(m_appLocalesPath);
    m_settingFile = m_appDatasPath + "/setting";
    m_settingIniFile = m_appDatasPath + "/setting.ini";
    m_ebcIniFile = m_appDatasPath + "/ebc.ini";

}

EbClientAppBase::~EbClientAppBase()
{

}

bool EbClientAppBase::initApp(void)
{
#ifdef Q_OS_ANDROID
    if (!QFile::exists(m_settingFile)) {
        QFile::copy("assets:/datas/setting", m_settingFile);
    }
    /// *** 修改版本号，修改以下二个地方以及“AndroidManifest.xml”文件
    this->m_setting.SetSDKVersion(517);
    this->m_setting.SetVersion("2.0.0.517");
#endif
    this->m_setting.load(this->getSettingFile().toLocal8Bit().constData());
    /// default this->setMainColor(0, 162, 232);
    this->setMainColor( this->m_setting.GetDefaultColor(),false );
    this->m_sProductName = QString::fromStdString( this->m_setting.GetEnterprise() );
    this->m_sProductVersion = QString::fromStdString( this->m_setting.GetVersion() );

    if (m_sqliteEbc.get()==NULL) {
        m_sqliteEbc = CSqliteCdbc::create();
        const QString sSqliteEbcFile = m_appBodbPath + "/ebcdatas";
#ifdef Q_OS_ANDROID
        if (!QFile::exists(sSqliteEbcFile)) {
            /// 使用以下新建文件 方法 ，避免 assets 文件只读属性，
            /// 导致数据库不能操作数据问题。
            QFile existFile("assets:/datas/bodb/ebcdatas");
            if (!existFile.open(QFile::ReadOnly)) {
                return false;
            }
            QFile copyToFile(sSqliteEbcFile);
            if (copyToFile.open(QFile::WriteOnly)) {
                copyToFile.write(existFile.readAll());
                copyToFile.close();
            }
            existFile.close();
        }
#endif

        if (!QFile::exists(sSqliteEbcFile)) {
#ifndef _QT_QML_
            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
            if (title.isEmpty())
                title = m_sProductName;
            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
#endif
            return false;
        }
        if (!m_sqliteEbc->open(sSqliteEbcFile.toStdString().c_str())) {
            m_sqliteEbc.reset();
            /// 当前安装目录文件损坏：<br>请重新安装或联系公司客服！
#ifndef _QT_QML_
            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
            if (title.isEmpty())
                title = m_sProductName;
            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
#endif
            return false;
        }
    }
    /// 获取本地上次产品名称
    int nCookie = 0;
    m_sqliteEbc->select("SELECT value1 FROM sys_value_t WHERE name='product-name'", nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
    if (pRecord.get()!=0) {
        const tstring productName(pRecord->getVector()[0]->getStr());
        m_sqliteEbc->reset(nCookie);
        if ( !productName.empty() )
            this->m_sProductName = QString::fromStdString( productName.string() );
    }
    else {
        m_sqliteEbc->execute("INSERT INTO sys_value_t(name,value1,value2) VALUES('product-name','',0)");
    }
    /// 获取本地设置色调
    m_sqliteEbc->select("SELECT value2 FROM sys_value_t WHERE name='main-color'", nCookie);
    pRecord = m_sqliteEbc->first(nCookie);
    if (pRecord.get()!=0) {
        const unsigned int nMainColor = (unsigned int)pRecord->getVector()[0]->getBigIntValue();
        m_sqliteEbc->reset(nCookie);
        this->setMainColor(GetRValue(nMainColor),GetGValue(nMainColor),GetBValue(nMainColor),false);
    }
    else {
        char sql[256];
        const QColor color(this->m_setting.GetDefaultColor());
        sprintf(sql, "INSERT INTO sys_value_t(name,value1,value2) VALUES('main-color','',%lld)",(mycp::bigint)RGB(color.red(),color.green(),color.blue()));
        m_sqliteEbc->execute(sql);
    }
    m_appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    checkCreateDir(m_appDataLocation);
    m_appDataTempLocation = m_appDataLocation + "/temp";
    checkCreateDir(m_appDataTempLocation);
    m_appDataCefCacheTemp = m_appDataLocation + "/cef_cache_temp";
    checkCreateDir(m_appDataCefCacheTemp);
    m_appDataImageTempLocation = m_appDataLocation + "/image_temp";
    checkCreateDir(m_appDataImageTempLocation);

//#ifndef Q_OS_ANDROID
//    if (m_webWngineDownloadRequested==0) {
//        m_webWngineDownloadRequested = new EbWebEngineDownloadRequested(this);
//    }
//#endif
    return true;
}

void EbClientAppBase::exitApp(bool bResetEbumOnly)
{
    if (!bResetEbumOnly) {
        this->m_ebum.EB_SetMsgReceiver(0);
    }
    this->m_ebum.EB_UnInit();
    if (!bResetEbumOnly) {
        m_sqliteEbc.reset();
        m_sqliteUser.reset();
    }
    if (m_isAppIdResponseOk) {
        m_isAppIdResponseOk = false;
        emit appidResponseOkChanged();
    }
}


bool EbClientAppBase::fileExist(const QString &filePath)
{
    return QFile::exists(filePath);
}

qint64 EbClientAppBase::fileSize(const QString &filePath)
{
    QFile file(filePath);
    if ( !file.open(QFile::ReadOnly) ) {
        return -1;
    }
    const qint64 result = file.size();
    file.close();
    return result;
}


bool EbClientAppBase::setDevAppId(QObject* receiver)
{
    m_receiver = receiver;
    const CEBString sAddress = this->m_setting.GetServerAddress();
    //entboost::GetAccountAddress(lpszAccount, sAddress);
    //this->m_ebum.Stop();
    this->m_ebum.EB_Init(sAddress.c_str());
    //this->m_ebum.EB_SetMsgHwnd(this->GetSafeHwnd());
    if (!this->m_ebum.EB_IsInited()) {
        return false;
    }
    this->m_ebum.EB_SetMsgReceiver(this);
    if (this->m_ebum.EB_SetDevAppId(278573612908LL, "ec1b9c69094db40d9ada80d657e08cc6", true)!=0) {
        return false;
    }
    return true;
}


void EbClientAppBase::deleteDbRecord(eb::bigint sId, bool bIsAccount)
{
    /// 先删除图片，语音，文件...
    if ( this->m_sqliteUser.get()==0 ) {
        return;
    }
    char sSql[256];
    if (bIsAccount) {
        sprintf(sSql,"select msg_name,msg_text FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) AND msg_type>=%d",sId,sId,(int)MRT_JPG);
    }
    else {
        sprintf(sSql,"select msg_name,msg_text FROM msg_record_t WHERE dep_code=%lld AND msg_type>=%d",sId,(int)MRT_JPG);
    }
    const QString sUserImagePath(userImagePath());
    const QString sUserFilePath(userFilePath());
    int nCookie = 0;
    m_sqliteUser->select( sSql, nCookie );
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const QString sMsgName( pRecord->getVector()[0]->getStrValue().c_str() );
        const QString sMsgText( pRecord->getVector()[1]->getStrValue().c_str() );
        pRecord = m_sqliteUser->next(nCookie);
        /// 判断是临时图片，语音，文件目录
        if ( !sMsgName.isEmpty() && sMsgName.indexOf( sUserImagePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgName);
        }
        if ( !sMsgText.isEmpty() && sMsgText.indexOf( sUserFilePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgText);
        }
    }
    m_sqliteUser->reset(nCookie);
    /// 删除数据
    if (bIsAccount) {
        sprintf(sSql,"DELETE FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld)",sId,sId);
    }
    else {
        sprintf(sSql, "DELETE FROM msg_record_t WHERE dep_code=%lld",sId);
    }
    m_sqliteUser->execute(sSql);
    /// 删除会话
    if (bIsAccount) {
        sprintf(sSql,"DELETE FROM call_record_t WHERE dep_code=0 AND from_uid=%lld", sId);
    }
    else {
        sprintf(sSql, "DELETE FROM call_record_t WHERE dep_code=%lld",sId);
    }
    m_sqliteUser->execute(sSql);
}

void EbClientAppBase::deleteDbRecord(qint64 sMsgId)
{
    /// 先删除图片，语音，文件...
    if ( this->m_sqliteUser.get()==0 ) {
        return;
    }
    char sSql[256];
    sprintf( sSql, "select msg_name,msg_text FROM msg_record_t WHERE msg_id=%lld AND msg_type>=%d", sMsgId, (int)MRT_JPG);
    const QString sUserImagePath(userImagePath());
    const QString sUserFilePath(userFilePath());
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=NULL)
    {
        const QString sMsgName( pRecord->getVector()[0]->getStrValue().c_str() );
        const QString sMsgText( pRecord->getVector()[1]->getStrValue().c_str() );
        pRecord = m_sqliteUser->next(nCookie);
        /// 判断是临时图片，语音，文件目录
        if ( !sMsgName.isEmpty() && sMsgName.indexOf( sUserImagePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgName);
        }
        if ( !sMsgText.isEmpty() && sMsgText.indexOf( sUserFilePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgText);
        }
    }
    m_sqliteUser->reset(nCookie);
    /// 删除数据
    sprintf( sSql, "DELETE FROM msg_record_t WHERE msg_id=%lld", sMsgId );
    m_sqliteUser->execute(sSql);
}

int EbClientAppBase::memberSubType(const EB_MemberInfo *memberInfo)
{
    EB_GROUP_TYPE groupType = EB_GROUP_TYPE_DEPARTMENT;
    m_ebum.EB_GetGroupType(memberInfo->m_sGroupCode, groupType);
    return memberSubType(groupType, memberInfo->m_sGroupCode, memberInfo->m_nMemberUserId);
}

int EbClientAppBase::memberSubType(EB_GROUP_TYPE groupType, eb::bigint groupId, eb::bigint memberUserId)
{
    int nSubType = 0;
    if (groupType<=EB_GROUP_TYPE_PROJECT && isEnterpriseCreateUserId(memberUserId))
        nSubType = 11;
    else if (m_ebum.EB_IsGroupCreator(groupId, memberUserId))
        nSubType = 10;
    else if (m_ebum.EB_IsGroupManager(groupId, memberUserId))
        nSubType = 10;
    else if (m_ebum.EB_IsGroupAdminLevel(groupId, memberUserId))
        nSubType = 9;
    else if (logonUserId()==memberUserId)
        nSubType = 1;
    else
        nSubType = 0;
    return nSubType;
}

bool EbClientAppBase::setLocaleLanguage(const QString &file)
{
    const QByteArray byteArray = file.toLocal8Bit();
    char sql[256];
    sprintf(sql, "UPDATE sys_value_t SET value1='%s' WHERE name='locale-file' AND value1<>'%s'",
            byteArray.constData(), byteArray.constData());
    return m_sqliteEbc->execute(sql)==1?true:false;
}

QString EbClientAppBase::localeLanguage(const QString &defaultFile)
{
    QString result(defaultFile);
    if (m_sqliteEbc.get()==0) {
        return result;
    }
    int nCookie = 0;
    m_sqliteEbc->select("SELECT value1 FROM sys_value_t WHERE name='locale-file'", nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
    if (pRecord.get()!=0) {
        result = QString::fromStdString(pRecord->getVector()[0]->getStrValue().string());
        m_sqliteEbc->reset(nCookie);
    }
    else {
        char sql[256];
        sprintf(sql, "INSERT INTO sys_value_t(name,value1) VALUES('locale-file','%s')", defaultFile.toStdString().c_str());
        m_sqliteEbc->execute(sql);
    }
    return result;
}

void EbClientAppBase::setEbcIniFileServer(const QString &newServer)
{
    const QString& sEbcIniFileSetting = getEbcIniFile();
    WritePrivateProfileStringABoost("system", "server", newServer.toStdString().c_str(), sEbcIniFileSetting.toLocal8Bit().constData());

}

QString EbClientAppBase::ebcIniFileServer() const
{
    const QString& sEbcIniFileSetting = getEbcIniFile();
    if (QFile::exists(sEbcIniFileSetting)) {
        char lpszBuffer[512];
        memset(lpszBuffer,0,sizeof(lpszBuffer));
        GetPrivateProfileStringABoost("system","server","",lpszBuffer,sizeof(lpszBuffer),sEbcIniFileSetting.toLocal8Bit().constData());
        return QString::fromUtf8(lpszBuffer);
    }
    else {
        return "";
    }
}

void EbClientAppBase::setNewMessageNotify(bool v)
{
    WritePrivateProfileIntABoost(
                "notify",
                "new-message",
                (int)(v?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
}

bool EbClientAppBase::newMessageNotify(bool bDefault) const
{
    const int ret = (int)GetPrivateProfileIntABoost(
                "notify",
                "new-message",
                (int)(bDefault?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
    return ret==1?true:false;
}

void EbClientAppBase::setDetailMessageNotify(bool v)
{
    WritePrivateProfileIntABoost(
                "notify",
                "detail-message",
                (int)(v?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
}

bool EbClientAppBase::detailMessageNotify(bool bDefault) const
{
    const int ret = (int)GetPrivateProfileIntABoost(
                "notify",
                "detail-message",
                (int)(bDefault?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
    return ret==1?true:false;
}

void EbClientAppBase::setVoiceNotify(bool v)
{
    WritePrivateProfileIntABoost(
                "notify",
                "voice",
                (int)(v?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
}

bool EbClientAppBase::voiceNotify(bool bDefault) const
{
    const int ret = (int)GetPrivateProfileIntABoost(
                "notify",
                "voice",
                (int)(bDefault?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
    return ret==1?true:false;
}

void EbClientAppBase::setVibrateNotify(bool v)
{
    WritePrivateProfileIntABoost(
                "notify",
                "vibrate",
                (int)(v?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
}

bool EbClientAppBase::vibrateNotify(bool bDefault) const
{
    const int ret = (int)GetPrivateProfileIntABoost(
                "notify",
                "vibrate",
                (int)(bDefault?1:0),
                m_userSettingIniFile.toLocal8Bit().constData());
    return ret==1?true:false;
}

void EbClientAppBase::setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase)
{
    m_mainColor.setRed(r);
    m_mainColor.setGreen(g);
    m_mainColor.setBlue(b);
    updateColor(bUpdateDatabase);
}

void EbClientAppBase::setMainColor(const QColor &pColor)
{
    setMainColor(pColor, true);
}
void EbClientAppBase::setMainColor(const QColor& pColor, bool bUpdateDatabase)
{
    m_mainColor = pColor;
    updateColor(bUpdateDatabase);
}

void EbClientAppBase::updateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType)
{
    /// nAckType=0 对方收到消息
    /// nAckType=4 请求撤回消息
    /// nAckType=6 请求“个人收藏”消息
    /// nAckType=7 请求“群收藏”消息
    if (this->m_sqliteUser.get()==0) {
        return;
    }
    char sSql[256];
    if (nAckType==6) {
        /// 个人收藏
        return;
    }
    else if (nAckType==7) {
        /// 群收藏
        return;
    }
    else if (nAckType==4) {
        sprintf( sSql, "UPDATE msg_record_t SET msg_name='',msg_text='',read_flag=read_flag|%d WHERE msg_id=%lld AND from_uid=%lld AND (read_flag&%d)=0",EBC_READ_FLAG_WITHDRAW,nMsgId,nFromUserId,EBC_READ_FLAG_WITHDRAW);
    }
    else if (nAckType==0) {
        /// ?
        sprintf(sSql, "UPDATE msg_record_t SET read_flag=read_flag|%d WHERE msg_id=%lld AND dep_code=0 AND to_uid=%lld AND (read_flag&%d)=0",EBC_READ_FLAG_RECEIPT,nMsgId,nFromUserId,EBC_READ_FLAG_RECEIPT);
    }
    m_sqliteUser->execute(sSql);
}

eb::bigint EbClientAppBase::groupMsgSubId()
{
    if (m_nGroupMsgSubId==0) {
        unsigned long pGroupMsgSubId = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
        if (pGroupMsgSubId!=0 && strlen((const char*)pGroupMsgSubId)>0) {
            m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
        }
    }
    return m_nGroupMsgSubId;
}

eb::bigint EbClientAppBase::findAppSubId()
{
    if (m_nFindAppSubId==0) {
        unsigned long pFindAppSubId = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID,&pFindAppSubId);
        if (pFindAppSubId!=0 && strlen((const char*)pFindAppSubId)>0) {
            m_nFindAppSubId = eb_atoi64((const char*)pFindAppSubId);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FIND_APP_SUBID,pFindAppSubId);
        }
    }
    return m_nFindAppSubId;
}

mycp::bigint EbClientAppBase::myCollectionSugId()
{
    if (m_myCollectionSubId==0) {
        unsigned long pMyCollectionSubId = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID,&pMyCollectionSubId);
        if (pMyCollectionSubId!=0 && strlen((const char*)pMyCollectionSubId)>0) {
            m_myCollectionSubId = eb_atoi64((const char*)pMyCollectionSubId);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_MY_COLLECTION_SUBID,pMyCollectionSubId);
        }
    }
    return m_myCollectionSubId;
}

void EbClientAppBase::onAppIdSuccess(QEvent *e)
{
    unsigned long pAccountPrefix = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,&pAccountPrefix);
    if (pAccountPrefix!=0 && strlen((const char*)pAccountPrefix)>0)
    {
        m_sAccountPrefix = (const char*)pAccountPrefix;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
    }

    unsigned long nLicenseUser = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER,&nLicenseUser);
    m_bLicenseUser = nLicenseUser==0?false:true;
    if (m_bLicenseUser) {
        unsigned long pProductName = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,&pProductName);
        if (pProductName!=0 && strlen((const char*)pProductName)>0) {
            const std::string productName((const char*)pProductName);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
            m_sProductName = QString::fromStdString(productName);
            char sql[256];
            sprintf(sql, "UPDATE sys_value_t SET value1='%s' WHERE name='product-name'",productName.c_str());
            m_sqliteEbc->execute(sql);
        }
        else {
            /// 没有配置，使用默认
            m_sqliteEbc->execute("UPDATE sys_value_t SET value1='' WHERE name='product-name'");
            this->m_sProductName = QString::fromStdString( this->m_setting.GetEnterprise() );
        }
    }
    unsigned long nSendRegMail = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
    m_bSendRegMail = nSendRegMail==1?true:false;
    unsigned long nSaveConversatios = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS,&nSaveConversatios);
    m_nSaveConversations = nSaveConversatios;
    unsigned long nAuthContact = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT,&nAuthContact);
    m_bAuthContact = nAuthContact==1?true:false;

    unsigned long pDeployId = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,&pDeployId);
    if (pDeployId!=0 && strlen((const char*)pDeployId)>0)
    {
        m_nDeployId = eb_atoi64((const char*)pDeployId);
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
    }
    unsigned long nLicenseType = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_TYPE,&nLicenseType);
    m_nLicenstType = nLicenseType;
    unsigned long nEBServerVersion = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_EB_SERVER_VERSION,&nEBServerVersion);
    m_nEBServerVersion = nEBServerVersion;
    //unsigned long pGroupMsgSubId = 0;
    //this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
    //if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
    //{
    //	m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
    //	this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
    //}
    unsigned long pAutoOpenSubId = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,&pAutoOpenSubId);
    if (pAutoOpenSubId!=0 && strlen((const char*)pAutoOpenSubId)>0)
    {
        m_nAutoOpenSubId = eb_atoi64((const char*)pAutoOpenSubId);
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,pAutoOpenSubId);
    }
    unsigned long nAutoHideMainFrame = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME,&nAutoHideMainFrame);
    m_bAutoHideMainFrame = nAutoHideMainFrame==1?true:false;
    unsigned long nHideMainFrame = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_HIDE_MAINFRAME,&nHideMainFrame);
    m_bHideMainFrame = nHideMainFrame==1?true:false;
    unsigned long nDisableUserCloudDrive = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE,&nDisableUserCloudDrive);
    m_bDisableUserCloudDrive = nDisableUserCloudDrive==1?true:false;
    unsigned long nDisableGroupShareCloud = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD,&nDisableGroupShareCloud);
    m_bDisableGroupShareCloud = nDisableGroupShareCloud==1?true:false;
    unsigned long nDisableModifyPasswd = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD,&nDisableModifyPasswd);
    m_bDisableModifyPasswd = nDisableModifyPasswd==1?true:false;
    unsigned long nDisableVideo = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_VIDEO,&nDisableVideo);
    m_bDisableVideo = nDisableVideo==1?true:false;
    unsigned long nDisableRD = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP,&nDisableRD);
    m_bDisableRemoteDesktop = nDisableRD==1?true:false;
    unsigned long nDisableAccountEdit = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT,&nDisableAccountEdit);
    m_bDisableAccountEdit = nDisableAccountEdit==1?true:false;
//    unsigned long nDefaultBrowserType = 0;
//    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE,&nDefaultBrowserType);
//    m_nDefaultBrowserType = nDefaultBrowserType==1?EB_BROWSER_TYPE_IE:EB_BROWSER_TYPE_CEF;
    unsigned long nDisableMsgReceipt = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MSG_RECEIPT,&nDisableMsgReceipt);
    m_bDisableMsgReceipt = nDisableMsgReceipt==1?true:false;
    unsigned long nStatSubGroupMember = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_STAT_SUB_GROUP_MEMBER,&nStatSubGroupMember);
    m_bStatSubGroupMember = nStatSubGroupMember==1?true:false;
    unsigned long pDefaultUrl = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_URL,&pDefaultUrl);
    if (pDefaultUrl!=0 && strlen((const char*)pDefaultUrl)>0)
    {
        m_sDefaultUrl = (const char*)pDefaultUrl;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_URL,pDefaultUrl);
    }

    unsigned long nOpenRegisiter = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_REGISTER,&nOpenRegisiter);
    m_nOpenRegister = nOpenRegisiter;
    unsigned long nOpenVisitor = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_VISITOR,&nOpenVisitor);
    m_bOpenVisitor = nOpenVisitor==0?false:true;
    unsigned long pForgetPwdUrl = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,&pForgetPwdUrl);
    if (pForgetPwdUrl!=0 && strlen((const char*)pForgetPwdUrl)>0)
    {
        m_sForgetPwdUrl = (const char*)pForgetPwdUrl;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,pForgetPwdUrl);
    }

    const QString sEntLogoImagePath = m_appImgPath + "/entlogo";			/// 企业定制LOGO
    const QString sEntLogoImagePathTemp = m_appImgPath + "/entlogotemp";	/// 先保存到临时中间文件
    if (m_bLicenseUser) {
        /// 企业LOGO
        unsigned long pEntLogoUrl = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,&pEntLogoUrl);
        const std::string sImageTempIniPath = m_appImgPath.toLocal8Bit().toStdString() + "/temp.ini";
        if (pEntLogoUrl!=0 && strlen((const char*)pEntLogoUrl)>0) {
            /// http://test-um.entboost.com/images/entlogo.png
            const std::string sEntLogoUrl((const char*)pEntLogoUrl);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,pEntLogoUrl);
            char lpszEntLogoLastModified[64];
            memset(lpszEntLogoLastModified,0,64);
            GetPrivateProfileStringABoost( "entlogo", "last_modified", "",lpszEntLogoLastModified,64,sImageTempIniPath.c_str());
            if (!QFile::exists(sEntLogoImagePath)) {
                memset(lpszEntLogoLastModified,0,64);
            }
            const QString sOldLastModified(lpszEntLogoLastModified);
            m_httpFileDownload.downloadHttpFile( QString::fromStdString(sEntLogoUrl), sEntLogoImagePathTemp, sOldLastModified );
            if (m_httpFileDownload.getDownloadFinished() && m_httpFileDownload.getLastErrorCode()==QNetworkReply::NoError) {
                if (QFile::exists(sEntLogoImagePathTemp)) {
                    QFile::remove(sEntLogoImagePath);
                    QFile::rename(sEntLogoImagePathTemp, sEntLogoImagePath);
                    WritePrivateProfileStringABoost("entlogo", "last_modified",
                                                    m_httpFileDownload.getLastModified().toStdString().c_str(),
                                                    sImageTempIniPath.c_str());
                }
            }
            else if ( m_httpFileDownload.getDownloadFinished() && m_httpFileDownload.getLastModified()==sOldLastModified) {
                /// 下载文件没有改变
            }
            else if (QFile::exists(sEntLogoImagePath)) {
                QFile::remove(sEntLogoImagePath);
                QFile::remove(sEntLogoImagePathTemp);
            }
        }
        else if (QFile::exists(sEntLogoImagePath)) {
            /// 没有设置企业LOGO，但有企业LOGO，应该是其他系统，删除并通知事件更新界面
            QFile::remove(sEntLogoImagePath);
            QFile::remove(sEntLogoImagePathTemp);
        }
    }
    else {
        const std::string sEnterprise( this->m_setting.GetEnterprise() );
        if ( sEnterprise.find("恩布")==std::string::npos ) {
            m_sProductName = QString::fromUtf8("恩布互联");
        }
        else {
            m_sProductName = sEnterprise.c_str();
        }

        if (QFile::exists(sEntLogoImagePath)) {
            QFile::remove(sEntLogoImagePath);   // ?
        }
    }
    if (!m_isAppIdResponseOk) {
        m_isAppIdResponseOk = true;
        emit appidResponseOkChanged();
    }

    if (m_receiver!=0) {
        const EB_Event * pEvent = (EB_Event*)e;
        QCoreApplication::postEvent( m_receiver, new EB_Event(*pEvent) );
    }
}
void EbClientAppBase::onAppIdError(QEvent *e)
{
    if (m_isAppIdResponseOk) {
        m_isAppIdResponseOk = false;
        emit appidResponseOkChanged();
    }
    if (m_receiver!=0) {
        const EB_Event * pEvent = (EB_Event*)e;
        QCoreApplication::postEvent( m_receiver, new EB_Event(*pEvent) );
    }
}

bool EbClientAppBase::onLogonSuccess(void)
{
    const mycp::tstring sLogonAccount = this->m_ebum.EB_GetLogonAccount();
#ifdef Q_OS_ANDROID
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    checkCreateDir(writablePath);
    const QString absolutePath = QDir(writablePath).absolutePath();
    m_userMainPath = absolutePath + "/users";
#else
    m_userMainPath = qApp->applicationDirPath() + "/users";
#endif
    checkCreateDir(m_userMainPath);
//    return m_userMainPath + _T("\\setting.ini");
    if (this->m_ebum.EB_IsLogonVisitor()) {
        m_userMainPath += "/visitor";
    }
    else {
        m_userMainPath = m_userMainPath + "/" + QString::number(logonUserId());
//        m_userMainPath = m_userMainPath + "/" + sLogonAccount.c_str();
    }
    checkCreateDir(m_userMainPath);
    m_userSettingIniFile = m_userMainPath + "/setting.ini";
//    m_userSettingIniFile = m_userMainPath + "/" + sLogonAccount.c_str() + "/setting.ini";
    m_userImagePath = m_userMainPath + "/image";
    checkCreateDir(m_userImagePath);
    m_userFilePath = m_userMainPath + "/file";
    checkCreateDir(m_userFilePath);

    if (this->m_sqliteUser.get()==0) {
        QString sBoPath = QString("%1/%2").arg(m_userMainPath).arg(this->deployId());
        if (!QFile::exists(sBoPath)) {
            QDir dir(sBoPath);
            dir.mkdir(sBoPath);
        }
        sBoPath += "/bodb";
#ifdef Q_OS_ANDROID
        const QString sDefaultUserBoFile = "assets:/datas/bodb/userdatas";
#else
        const QString sDefaultUserBoFile = m_appBodbPath + "/userdatas";
#endif
        const QString sUserBoFile = sBoPath + "/userdatas";
        if (!QFile::exists(sBoPath)) {
            QDir dir(sBoPath);
            dir.mkdir(sBoPath);
            QFile::remove(sUserBoFile);
#ifdef Q_OS_ANDROID
            /// 使用以下新建文件 方法 ，避免 assets 文件只读属性，
            /// 导致数据库不能操作数据问题。
            QFile existFile(sDefaultUserBoFile);
            if (!existFile.open(QFile::ReadOnly)) {
                return false;
            }
            QFile copyToFile(sUserBoFile);
            if (copyToFile.open(QFile::WriteOnly)) {
                copyToFile.write(existFile.readAll());
                copyToFile.close();
            }
            existFile.close();
#else
            QFile::copy(sDefaultUserBoFile, sUserBoFile);
#endif
        }
        else if (fileSize(sUserBoFile)<=0) {
            QFile::remove(sUserBoFile);
#ifdef Q_OS_ANDROID
            /// 使用以下新建文件 方法 ，避免 assets 文件只读属性，
            /// 导致数据库不能操作数据问题。
            QFile existFile(sDefaultUserBoFile);
            if (!existFile.open(QFile::ReadOnly)) {
                return false;
            }
            QFile copyToFile(sUserBoFile);
            if (copyToFile.open(QFile::WriteOnly)) {
                copyToFile.write(existFile.readAll());
                copyToFile.close();
            }
            existFile.close();
#else
            QFile::copy(sDefaultUserBoFile, sUserBoFile);
#endif
        }

        m_sqliteUser = CSqliteCdbc::create();
        if (!m_sqliteUser->open(sUserBoFile.toStdString().c_str())) {
            m_sqliteUser.reset();
#ifndef _QT_QML_
            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
            if (title.isEmpty())
                title = m_sProductName;
            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
#endif
            return false;
        }
    }
    /// for test
    /// 2.0.0.570 增加 display_index 字段，用于表情/截图和文本的显示排序
    if (m_sqliteUser->select("SELECT display_index FROM msg_record_t limit 1")==-1)
    {
        m_sqliteUser->execute("ALTER TABLE msg_record_t ADD display_index TINYINT DEFAULT 0");
    }
    return true;
}

const float    theDefaultBtnHot					= 0.15;					// 比正常颜色亮
const float    theDefaultBtnPre					= -0.07;				// 比正常颜色深
const float    theDefaultMaxBtnHot				= 0.8;					// 亮-最大亮度
const float    theDefaultMaxBtnPre				= 0.6;					// 深-最大亮度

void EbClientAppBase::updateColor(bool bUpdateDatabase)
{
    const int r = m_mainColor.red();
    const int g = m_mainColor.green();
    const int b = m_mainColor.blue();
    if (bUpdateDatabase) {
        char sql[256];
        sprintf(sql, "UPDATE sys_value_t SET value2=%lld WHERE name='main-color'",(mycp::bigint)RGB(r,g,b));
        m_sqliteEbc->execute(sql);
    }
    float h,s,l;
    ::RGBtoHSL(r, g, b,&h,&s,&l);
    /// 按钮 hot,pre 二个状态
    const unsigned int m_hotColorRgb = ::HSLtoRGB(h, s, MIN(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
    const unsigned int m_preColorRgb = ::HSLtoRGB(h, s, MIN(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
    m_mainHoverColor.setRed(GetRValue(m_hotColorRgb));
    m_mainHoverColor.setGreen(GetGValue(m_hotColorRgb));
    m_mainHoverColor.setBlue(GetBValue(m_hotColorRgb));
    m_mainPressedColor.setRed(GetRValue(m_preColorRgb));
    m_mainPressedColor.setGreen(GetGValue(m_preColorRgb));
    m_mainPressedColor.setBlue(GetBValue(m_preColorRgb));

#ifndef _QT_QML_
    const QString sQssFile = qApp->applicationDirPath()+"/entboost.qss";
    QFile qssFile(sQssFile);
    if (qssFile.open(QFile::ReadOnly)) {
        m_styleSheet = QString::fromUtf8(qssFile.readAll());   // ** ok
        qssFile.close();
        char lpszBuffer[64];
        sprintf( lpszBuffer,"%d,%d,%d",r,g,b );
        m_styleSheet.replace( "[EB_MAIN_RGB]", lpszBuffer );
//        sprintf( lpszBuffer,"%d,%d,%d",m_hotColor.red(),m_hotColor.green(),m_hotColor.blue() );
//        qss.replace( "[EB_HOT_RGB]", lpszBuffer );
//        sprintf( lpszBuffer,"%d,%d,%d",m_preColor.red(),m_preColor.green(),m_preColor.blue() );
//        qss.replace( "[EB_PRE_RGB]", lpszBuffer );
        qApp->setStyleSheet(m_styleSheet);
    }
#endif
}
