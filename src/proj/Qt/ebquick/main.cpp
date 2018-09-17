#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QQmlContext>
//#include <QtWebView/QtWebView>
//#include <QtSql/QSqlDatabase>
//#ifdef Q_OS_ANDROID
//#include <QSqlError>
//#endif  /// Q_OS_ANDROID
#include "../entboost/ebclocales.h"
#include "ebclientapp.h"
#include "ebkeyfilter.h"
#ifdef ANDROID
#include <QtAndroid>
//#include <QtAndroidExtras/QAndroidJniEnvironment>
//#include <QtAndroidExtras/QAndroidJniObject>
//#include <QtAndroidExtras/QtAndroid>
#endif
//EbcLocales theLocales;

QString getFilePath(const QString &filename)
{
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    checkCreateDir(writablePath);
    qDebug() << "writablePath: " << writablePath;

    const QString absolutePath = QDir(writablePath).absolutePath();
    checkCreateDir(absolutePath);
    qDebug() << "absoluePath: " << absolutePath;

    const QString path = absolutePath + "/" + filename;
    qDebug() << "path: " << path;

    return path;
}

EbcLocales * theLocales=0;
//QSqlDatabase theSqlite;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
//    QtWebView::initialize();

    QFont font = app.font();
    font.setPointSize(13);
    app.setFont(font);

    QCoreApplication::setOrganizationName(QLatin1String("Entboost"));
    QCoreApplication::setApplicationName(QLatin1String("entboost"));
    QCoreApplication::setApplicationVersion(QLatin1String("1.0"));
    QFontDatabase::addApplicationFont(":/res/fontawesome-webfont.ttf");

    qmlRegisterType<EbKeyFilter>("Entboost.KeyFilter", 1, 0, "EbKeyFilter");
    qmlRegisterType<EbGroupTypeName>("Entboost.GroupTypeName", 1, 0, "EbGroupTypeName");
    qmlRegisterType<EbQuickMemberInfo>("Entboost.MemberInfo", 1, 0, "EbQuickMemberInfo");
//    qmlRegisterType<EbQuickEmotionInfo>("Entboost.EmotionInfo", 1, 0, "EbQuickEmotionInfo");
//    qmlRegisterType<EbQuickAccountInfo>("Entboost.AccountInfo", 1, 0, "EbQuickAccountInfo");
    qmlRegisterType<EbQuickSubscribeFuncInfo>("Entboost.SubscribeFuncInfo", 1, 0, "EbQuickSubscribeFuncInfo");
    qmlRegisterType<EbQuickUGInfo>("Entboost.UGInfo", 1, 0, "EbQuickUGInfo");
    qmlRegisterType<EbQuickRichInfo>("Entboost.RichInfo", 1, 0, "EbQuickRichInfo");
    qmlRegisterType<EbQuickECardInfo>("Entboost.ECardInfo", 1, 0, "EbQuickECardInfo");

//    qmlRegisterType<EbQuickAccountInfo>("Entboost.AccountInfo", 1, 0, "EbQuickAccountInfo");
//    qmlRegisterType<EbQuickGroupInfo>("Entboost.GroupInfo", 1, 0, "EbQuickGroupInfo");

//    theLocales.loadLocaleListByQrc(":/locales/locales.json");
//    const QString localeListFile = theApp->getAppLocalesPath()+"/locales.json";
//    theLocales.loadLocaleList(localeListFile.toLocal8Bit().toStdString());

//    qmlRegisterType<EbcLocales>("Entboost.Locales", 1, 0, "EbLocales");

//    QString path = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).first();

//    QVariantList TodoCpp::getItems()
//    {
//        QVariantList list;

//        int size = settings->beginReadArray("items");
//        for (int i = 0; i < size; ++i) {
//            settings->setArrayIndex(i);
//            QVariantMap m;
//            m.insert("text", settings->value("text", "").toString());
//            m.insert("pri", settings->value("pri", 99).toInt());
//            m.insert("done", settings->value("done", false).toBool());

//            if(!m.value("text").toString().isEmpty())
//                list.push_back(m);
//        }
//        settings->endArray();

//        return list;
//    }

//    Component.onCompleted: {
//            var l  = todocpp.getItems();
//            console.debug(JSON.stringify(l));
//            for(var i=0; i<l.length; ++i){
//                //这里直接使用l[i].text， l[i].pri拿属性
//            }
//        }
//    function saveItems(){
//    var l = [];
//    for(var i=0; i<listmodel.count; ++i){
//        l.push({'pri': listmodel.get(i).iPri,
//                   'text': listmodel.get(i).iText,
//                   'done': listmodel.get(i).iDone });
//    }

//    todocpp.saveItems(l);

    EbClientApp *theApp = new EbClientApp();
    if (!theApp->initApp()) {
        delete theApp;
        return -1;
    }
//    QFile file;
//    file.fileName()
//    if (QFile::exists("assets:/datas/bodb/ebcdatas")) {

//        theSqlite = QSqlDatabase::addDatabase("QSQLITE");
//        theSqlite.setDatabaseName("assets:/datas/bodb/ebcdatas2");
////            theSqlite.setConnectOptions("QSQLITE_OPEN_URI");
//        if (!theSqlite.open()) {
//            //        return 1;
//            theApp->setLastError(theSqlite.lastError().text());
//        }
//        else {
//            theApp->setLastError("database open ok.");
//            theSqlite.close();
//        }
//    }
//    else {
//        theApp->setLastError("database file not exist.");
//    }
    /// 为空
//    theApp->setLastError(QFile::readLink("assets:/datas/bodb/ebcdatas"));
    theLocales = new EbcLocales();
    theApp->setLocales(theLocales);
    theLocales->loadLocaleListByQrc(":/locales/locales.json");
    const QString localeFileName = theApp->localeLanguage();
    const QString localeFilePath = ":/locales/"+localeFileName;
    theLocales->loadLocaleFileByQrc(localeFilePath);

//#ifdef Q_OS_ANDROID
//    theLocales->loadLocaleListByQrc("assets:/datas/locales/locales.json");
//    const QString localeFileName = theApp->localeLanguage();
//    const QString localeFilePath = "assets:/datas/locales/"+localeFileName;
//    theLocales->loadLocaleFileByQrc(localeFilePath);
//#else
//    theLocales->loadLocaleListByQrc(theApp->appDatasPath()+"/locales/locales.json");
//    const QString localeFileName = theApp->localeLanguage();
//    const QString localeFilePath = theApp->appDatasPath()+"/locales/"+localeFileName;
//    theLocales->loadLocaleFileByQrc(localeFilePath);
//#endif

//    theLocales->loadLocaleListByQrc(":/locales/locales.json");
//    const QString localeListFile = theApp->getAppLocalesPath()+"/zh-CN.json";
//    theLocales->loadLocaleFile(localeListFile.toLocal8Bit().toStdString());

//    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
//    directory.clear();
//    if (directory.isEmpty())
//        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();

    QQmlApplicationEngine engine;
//        engine.setOfflineStoragePath("/ebquick");
//    engine.setOfflineStoragePath("./data");
//    const QString filePath = getFilePath("ebdata");
//    QString text = engine.offlineStoragePath();
//    const QString filePath = engine.offlineStorageDatabaseFilePath("entboost");
//    QFile file(filePath);
//    QString text(filePath);
//    if (file.open(QFile::WriteOnly|QFile::ReadOnly)) {
//        text += "open file ok";
//        file.write("test", 4);
//        file.close();
//    }
//    else {
//        text += "open file false";
//    }

//    QString text = qApp->applicationDirPath() + "/locales.json";

//    checkCreateDir(text);
//    theApp->setLastError(text);
//    const QString file = text+"/datas";
//    checkCreateDir(text);
    //    const QString newFile = "assets:/datas/locales.json";
//    const QString newFile = text;
//    text += ";";
//    text += QFile::exists(newFile)?"exist:":"not-exist;";
//    QFile::copy(":/locales/locales.json", newFile);
    /// 以下OK
//    QFile::copy("assets:/datas/bodb/ebcdatas", filePath);
//    QString text;
//    if (QFile::exists(filePath)) {
//        text += "file exist;";
//        theSqlite = QSqlDatabase::addDatabase("QSQLITE");
//        theSqlite.setDatabaseName(filePath);
//        if (!theSqlite.open()) {
//            text += theSqlite.lastError().text();
//        }
//        else {
//            text += "database open ok.";
//            theSqlite.close();
//        }
//    }
//    else {
//        text += "file not-exist";
//    }
//    theApp->setLastError(text);
//    engine.rootContext()->setContextProperty("theKeyFilter", EbKeyFilter::instance());

    engine.rootContext()->setContextProperty("theApp", theApp);
    engine.rootContext()->setContextProperty("theLocales", theLocales);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    const int ret = app.exec();
    theApp->logout(false, true);
    theApp->exitApp();
    if (theApp->isRequestLogout()) {
#ifdef ANDROID
        /// 1
//        Intent intent = new Intent(this, Object.class);
//        startActivity(intent);
        /// 2
//        ActivityManager manager = （ActivityManager）this.getSystemService（Context.ACTIVITY_SERVICE）;
//        manager.restartPackage("com.example.test");
//        QAndroidJniObject activity = QtAndroid::androidActivity();
//        QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
//                    "android/content/Context",
//                    "ACTIVITY_SERVICE",
//                    "Ljava/lang/String;"
//                    );
//        QAndroidJniObject vibrateService = activity.callObjectMethod(
//                    "getSystemService",
//                    "(Ljava/lang/String;)Ljava/lang/Object;",
//                    name.object<jstring>());
//        QString packageName = "org.qtproject.entboost";
//        QAndroidJniObject param1 = QAndroidJniObject::fromString(packageName);
//        vibrateService.callMethod<void>("restartPackage",
//                                        "(J)V",
//                                        param1.object<jstring>());


//        vibrateService.callMethod<void>("restartPackage",
//                                        "(Ljava/lang/String;)Ljava/lang/Object;",
//                                        param1.object<jstring>());

//        QString packageName = "org.qtproject.entboost";
////        QString className = "org.qtproject.entboost.android.intent.action.MAIN";
////        QString className = "org.qtproject.entboost.android.intent.category.LAUNCHER";
////        QString className = "android.intent.category.LAUNCHER";
////        QString className = "android.intent.action.MAIN";
////        QString className = "org.qtproject.qt5.android.bindings.QtActivity";
////            QString className = "org.qtproject.entboost.entboost";
////        QString packageName = "an.qt.xx";
////        QString className = "an.qt.xx.MainActivity";
//        QAndroidJniObject param1 = QAndroidJniObject::fromString(packageName);
////        QAndroidJniObject param2 = QAndroidJniObject::fromString(className);
//        QAndroidJniObject param2 = QtAndroid::androidActivity();

//        QAndroidJniObject intent("android/content/Intent","()V");
//        intent.callObjectMethod("setClassName",
//                                "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
//                                param1.object<jstring>(), param2.object<jstring>());
//        QtAndroid::startActivity(intent, 0);
#else
        const QString appFilePath = app.applicationFilePath();
        QProcess proc;
        proc.startDetached(appFilePath, QStringList());
#endif
    }
    delete theApp;
    theApp = 0;
    return ret;
}
