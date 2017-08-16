#include "mainwindow.h"
#include <QtCore/QCoreApplication>
#include "ebdialoglogin.h"
#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDesktopServices>
#include <QDir>
#include <QFontDatabase>
#include <QTranslator>
#include "ebclientapp.h"
#include "ebdialogmainframe.h"
//#include <QtWebEngine/QtWebEngine>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <ebwebengineurlschemehandler.h>

bool findSysFontFamily(void)
{
    bool ret = false;
    QFontDatabase pFontDatabase;
    foreach (const QString &family, pFontDatabase.families(QFontDatabase::SimplifiedChinese)) {
//        qDebug()<<family;
        const std::wstring sFamily(family.toStdWString());
        if ( sFamily==theFontFamily1 ) {
            theFontFamily = sFamily;
            ret = true;
        }
        else if ( sFamily==theFontFamily0 ) {
            theFontFamily = sFamily;
            return true;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if ( findSysFontFamily() ) {
        QFont font = a.font();
        font.setFamily(QString::fromStdWString(theFontFamily));
        a.setFont(font);
    }

    theApp = EbClientApp::create();

//    setLanguage(1);//调用全局函数
    /// 加载Qt标准对话框的中文翻译文件
    QTranslator tranMain;
    if (tranMain.load(QString(":/qm/qt_zh_CN.qm"))) {
        a.installTranslator(&tranMain);
    }
    QTranslator tranWidgets;
    if (tranWidgets.load(":/qm/qt_zh_CN_widgets.qm")) {
        a.installTranslator(&tranWidgets);
    }

    /// 加载语言列表
    const QString localeListFile = theApp->getAppLocalesPath()+"/locales.json";
    theLocales.loadLocaleList(localeListFile.toLocal8Bit().toStdString());
    /// 加载默认中文简体
    const QString localeFileName = theApp->localeLanguage();
    const QString localeFilePath = theApp->getAppLocalesPath()+"/"+localeFileName;
    theLocales.loadLocaleFile(localeFilePath.toLocal8Bit().toStdString());
    if (!theApp->initApp()) {
        return 1;
    }
    const QString settingLocaleFileName = theApp->localeLanguage(localeFileName);
    if (settingLocaleFileName!=localeFileName) {
        const QString settingLocaleFilePath = theApp->getAppLocalesPath()+"/"+settingLocaleFileName;
        theLocales.loadLocaleFile(settingLocaleFilePath.toLocal8Bit().toStdString());
    }
    {
        EbDialogLogin pDlgLogin;
        const int nret = pDlgLogin.exec();
        if (nret==QDialog::Rejected) {
            return 0;
        }
    }

    /// 登录成功
    /// 设置 chrome 缓存路径
//    const QString m_sysAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//    checkCreateDir(m_sysAppDataLocation);
//    QString m_sCefCachePath = m_sysAppDataLocation + "/cef_cache_temp";
//    checkCreateDir(m_sCefCachePath);
    QString m_sCefCachePath = theApp->appDataCefCacheTemp();
    if ( theApp->isLogonVisitor() ) {
        m_sCefCachePath += "/visitor";
    }
    else {
        m_sCefCachePath += QString::number(theApp->logonUserId());
    }
    checkCreateDir(m_sCefCachePath);
//    qputenv(“QTWEBENGINE_REMOTE_DEBUGGING”, 9000);
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler( QByteArray(thePlayVoice), new EbWebEngineUrlSchemeHandler() );
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler( QByteArray(theCallAccount), new EbWebEngineUrlSchemeHandler() );
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler( QByteArray(theCallGroup), new EbWebEngineUrlSchemeHandler() );
    QWebEngineProfile::defaultProfile()->setCachePath( m_sCefCachePath );
    QWebEngineProfile::defaultProfile()->setHttpCacheType( QWebEngineProfile::DiskHttpCache );
    QWebEngineSettings::defaultSettings()->setAttribute( QWebEngineSettings::PluginsEnabled, true );
    QWebEngineSettings::defaultSettings()->setAttribute( QWebEngineSettings::HyperlinkAuditingEnabled, true );

//    command_line->AppendSwitchWithValue("--ppapi-flash-path", "PepperFlash/pepflashplayer.dll");	// *单独加载可以使用，但Flash: 显示版本不对
//    /// manifest.json中的version
//    command_line->AppendSwitchWithValue("--ppapi-flash-version", "21.0.0.213");										// *测试过，替换其他版本的pepflashplayer.dll也能正常使用

    /// 不设置大小，由系统内部自动管理
//    const int nMaximumSize = 100*1024*1024; // 100MB
//    QWebEngineProfile::defaultProfile()->setHttpCacheMaximumSize( nMaximumSize );
//    QWebEngineProfile::defaultProfile()->settings()->setDefaultTextEncoding();
//    QWebEngineSettings::globalSettings()

    EbDialogMainFrame mainFrame;
    const int ret = mainFrame.exec();
#ifdef USES_CEF
    CefQuit()
#endif
    theApp.reset();
    if ( mainFrame.requestLogout() ) {
        const QString appFilePath = a.applicationFilePath();
        QProcess proc;
        proc.startDetached(appFilePath, QStringList());
    }
    return ret;
}
