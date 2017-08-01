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

//void TestHostAddressList(void)
//{
//    QList<QHostAddress> list = QNetworkInterface::allAddresses();
//    foreach (QHostAddress address, list)
//    {
//        if(address.protocol() == QAbstractSocket::IPv4Protocol)
//        {
//            //IPv4地址
//            const QString sIp = address.toString();
//            if (sIp.contains("127.0.") || sIp.contains("0.0.0.0"))
//            {
//                continue;
//            }

//        }
//    }
//}

//QTranslator g_tran;//必须设置成全局变量，如果是局部变量，设置将失效。
//void setLanguage(int lan)//全局函数，可供其他文件调用
//{
//    bool ok = false;
//    if (lan == 1)//中文
//    {
//        ok = g_tran.load("qt_zh_CN.qm", ":/lan/");//这里使用Qt资源文件,把.qm文件拷贝到项目路径/lan之下
//    }
//    else if (lan == 2)//英文
//    {
//        ok = g_tran.load("", ":/lan/");
//    }

//    QCoreApplication::installTranslator(&g_tran);
//    //或者写成qApp->installTranslator(&g_tran);
//}

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

//#define USES_CEF
int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    /// 加了这行代码，会有问题；
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//#if defined(Q_OS_OSX)
//    QApplication::setQuitOnLastWindowClosed(false);
//#else
//    QApplication::setQuitOnLastWindowClosed(true);
//#endif

//    QtWebEngine::initialize();

//#ifdef USES_CEF
////    CefExecuteProcess()
//    int result = CefInit(argc, argv);
//    if (result >= 0) {
//        return result;
//    }
//    CefLoadPlugins(IsWow64());
//#endif

//    const QString sQssFile = a.applicationDirPath()+"/datas/ebc.qss";
//    QFile qssFile(sQssFile);
//    if (qssFile.open(QFile::ReadOnly)) {
////        const QString qss = QLatin1String(qssFile.readAll());
//        const QString qss = QString::fromUtf8(qssFile.readAll());   // ** ok
//        a.setStyleSheet(qss);
//        qssFile.close();
//    }

    ///
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

//    TestDirFile();
//    TestStandardPaths();
//    TestHostAddressList();

    /// 加载默认中文
    const QString localFileName = theApp->getAppLocalesPath()+"/zh-CN.json";
    theLocales.loadLocaleFile(localFileName.toLocal8Bit().toStdString());
    if (!theApp->initApp()) {
        return 1;
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
//	//manifest.json中的version
//	command_line->AppendSwitchWithValue("--ppapi-flash-version", "21.0.0.213");										// *测试过，替换其他版本的pepflashplayer.dll也能正常使用

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
//    MainWindow w;
//    w.show();
//    const int ret = a.exec();
    // exit
//    theEBAppClient.EB_UnInit();
    theApp.reset();

    if ( mainFrame.requestLogout() ) {
        const QUrl entboostFile = QUrl::fromLocalFile(a.applicationFilePath());
        QDesktopServices::openUrl( entboostFile );
    }
    return ret;
}
