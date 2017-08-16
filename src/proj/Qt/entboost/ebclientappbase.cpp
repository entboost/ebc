#include "ebclientappbase.h"
#ifndef Q_OS_ANDROID
#include <QApplication>
#include "ebmessagebox.h"
#endif

#ifndef Q_OS_ANDROID
EbcLocales theLocales;
#endif

EbClientAppBase::EbClientAppBase(QObject *parent) : QObject(parent)
  , m_mainColor(0,162,232)
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

{
#ifdef Q_OS_ANDROID
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    checkCreateDir(writablePath);
    qDebug() << "writablePath: " << writablePath;
    const QString absolutePath = QDir(writablePath).absolutePath();
#else
    const QString absolutePath = qApp->applicationDirPath();
#endif
    checkCreateDir(absolutePath);
    m_appDataPath = absolutePath + "/datas";
    checkCreateDir(m_appDataPath);
    m_appUsersPath = absolutePath + "/users";
    checkCreateDir(m_appUsersPath);
    m_appImgPath = absolutePath + "/img";
    checkCreateDir(m_appImgPath);
    m_appBodbPath = m_appDataPath + "/bodb";
    checkCreateDir(m_appBodbPath);
    m_appLocalesPath = m_appDataPath + "/locales";
    checkCreateDir(m_appLocalesPath);
    m_settingFile = m_appDataPath + "/setting";
    m_settingIniFile = m_appDataPath + "/setting.ini";
    m_ebcIniFile = m_appDataPath + "/ebc.ini";

}

EbClientAppBase::~EbClientAppBase()
{

}

bool EbClientAppBase::initApp(void)
{
    this->m_setting.load(this->getSettingFile().toLocal8Bit().constData());
    /// default this->setMainColor(0, 162, 232);
#ifndef Q_OS_ANDROID
    this->setMainColor( this->m_setting.GetDefaultColor(),false );
#endif
    this->m_sProductName = QString::fromStdString( this->m_setting.GetEnterprise() );

    if (m_sqliteEbc.get()==NULL) {
        m_sqliteEbc = CSqliteCdbc::create();
        const QString sSqliteEbcFile = m_appBodbPath + "/ebcdatas";
#ifdef Q_OS_ANDROID
        if (!QFile::exists(sSqliteEbcFile)) {
            QFile::copy("assets:/datas/bodb/ebcdatas", sSqliteEbcFile);
        }
#endif
        if (!QFile::exists(sSqliteEbcFile)) {
#ifndef Q_OS_ANDROID
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
#ifndef Q_OS_ANDROID
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
#ifndef Q_OS_ANDROID
        this->setMainColor(GetRValue(nMainColor),GetGValue(nMainColor),GetBValue(nMainColor),false);
#endif
    }
    else {
#ifndef Q_OS_ANDROID
        char sql[256];
        const QColor color(this->m_setting.GetDefaultColor());
        sprintf(sql, "INSERT INTO sys_value_t(name,value1,value2) VALUES('main-color','',%lld)",(mycp::bigint)RGB(color.red(),color.green(),color.blue()));
        m_sqliteEbc->execute(sql);
#endif
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
    m_isAppIdResponseOk = false;
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

void EbClientAppBase::setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase)
{
    m_mainColor.setRed(r);
    m_mainColor.setGreen(g);
    m_mainColor.setBlue(b);
    updateColor(bUpdateDatabase);
}
void EbClientAppBase::setMainColor(const QColor& pColor, bool bUpdateDatabase)
{
    m_mainColor = pColor;
    updateColor(bUpdateDatabase);
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
//    float h,s,l;
//    ::RGBtoHSL(r, g, b,&h,&s,&l);
//    // 按钮 hot,pre 二个状态
//    const unsigned int m_hotColorRgb = ::HSLtoRGB(h, s, std::min(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
//    const unsigned int m_preColorRgb = ::HSLtoRGB(h, s, std::min(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
//    m_hotColor.setRed(GetRValue(m_hotColorRgb));
//    m_hotColor.setGreen(GetGValue(m_hotColorRgb));
//    m_hotColor.setBlue(GetBValue(m_hotColorRgb));
//    m_preColor.setRed(GetRValue(m_preColorRgb));
//    m_preColor.setGreen(GetGValue(m_preColorRgb));
//    m_preColor.setBlue(GetBValue(m_preColorRgb));

#ifndef Q_OS_ANDROID
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
