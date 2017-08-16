#-------------------------------------------------
#
# Project created by QtCreator 2017-04-29T11:00:32
#
#-------------------------------------------------

#QT       += core gui network
QT       -= gui
QT       += network
android {
QT       += quick
}
else {
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
}

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/ebumd"
}
else {
TARGET = "../../build/ebum"
}
}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/ebumd"
}
else {
TARGET = "../build/ebum"
}
}
TEMPLATE = lib

#DEFINES += LIBEBUM_LIBRARY
DEFINES += _QT_MAKE_
DEFINES += EB_APPCLIENT_EXPORTS
DEFINES += USES_CHATROOM
android {
DEFINES += OPENSSL_NO_ENGINE
}
else {
DEFINES += USES_VIDEOROOM
}
#DEFINES += USES_REMOTE_DESKTOP

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


win32 {
INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += "F:/THIRDPARTY/sqlite-3081002"
INCLUDEPATH += F:/THIRDPARTY/openssl-1.0.2k/inc32
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
INCLUDEPATH += "../../../libchatroom"
INCLUDEPATH += "../../../libebmm-2.0/libebmm"
INCLUDEPATH += "../../../include"

    Release {
LIBS += -L"../../../mycp/build" -lCGCLibQt -lCGCClassQt
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-1_62 -llibboost_thread-vc140-mt-1_62 -llibboost_filesystem-vc140-mt-1_62
LIBS += -L"../build" -lebcm
LIBS += -L"../../../libebmm-2.0/build" -lebmm
LIBS += -L"F:/THIRDPARTY/sqlite-3081002/lib" -lsqlite3s
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstat
    }
    Debug {
LIBS += -L"../../../mycp/build" -lCGCLibQtd -lCGCClassQtd
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-gd-1_62 -llibboost_thread-vc140-mt-gd-1_62 -llibboost_filesystem-vc140-mt-gd-1_62
#LIBS += -lWs2_32 -lshlwapi -lWinmm -lole32 -lOleaut32 -lWbemuuid
LIBS += -L"../build" -lebcmd
LIBS += -L"../../../libebmm-2.0/build" -lebmmd
LIBS += -L"F:/THIRDPARTY/sqlite-3081002/lib" -lsqlite3sd
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstatd
    }
LIBS += -lWs2_32 -lshlwapi
LIBS += -L"F:/THIRDPARTY/openssl-1.0.2k/vc2015" -llibeay32 -lssleay32

}
else:android {
INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += "F:/THIRDPARTY/sqlite-3081002"
INCLUDEPATH += F:/THIRDPARTY/openssl-android-master/include
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
INCLUDEPATH += "../../../libchatroom"
INCLUDEPATH += "../../../libebmm-2.0/libebmm"
INCLUDEPATH += "../../../include"

CONFIG(debug,debug|release) {
LIBS += -L"../build" -lebcmd
LIBS += "../../../mycp/build/libCGCLibQtAndroidd.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroidd.a"
LIBS += "F:/THIRDPARTY/sqlite-3081002/Qt/build/libsqliteandroidd.a"
}
else {
LIBS += -L"../build" -lebcm
LIBS += "../../../mycp/build/libCGCLibQtAndroid.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroid.a"
LIBS += "F:/THIRDPARTY/sqlite-3081002/Qt/build/libsqliteandroid.a"
}
LIBS += -L"F:/THIRDPARTY/openssl-android-master/libs/armeabi-v7a" -lcrypto -lssl
}
else:unix: {
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
INCLUDEPATH += "/usr/local/include/sqlite3"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
INCLUDEPATH += "../../../libchatroom"
INCLUDEPATH += "../../../libebmm-2.0/libebmm"
INCLUDEPATH += "../../../include"

CONFIG(debug,debug|release) {
LIBS += -L"../../../mycp/build" -lCGCClassQtd -lCGCLibQtd
LIBS += -L"../../../libebmm-2.0/build" -lebmmd
LIBS += -L"../build" -lebcmd
}
else {
LIBS += -L"../../../mycp/build" -lCGCClassQt -lCGCLibQt
LIBS += -L"../../../libebmm-2.0/build" -lebmm
LIBS += -L"../build" -lebcm
}
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_system.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_thread.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_filesystem.a"
LIBS += "/usr/local/lib/libsqlite3.a"
LIBS += -L"/usr/lib" -lcrypto -lssl -lz
}


SOURCES += ../../../libpopusermanager/EBAppClient.cpp \
    ../../../libpopusermanager/UserManagerApp.cpp \
    ../../../include/EBCAppCenter.cpp \
    ../../../include/POPCLogonCenter.cpp \
    ../../../include/POPCUserManager.cpp \
    ../../../include/EBParseSetting.cpp

HEADERS += ../../../libpopusermanager/EBAppClient.h \
    ../../../libpopusermanager/UserManagerApp.h \
    ../../../include/EBCAppCenter.h \
    ../../../include/POPCLogonCenter.h \
    ../../../include/POPCUserManager.h \
    ../../../include/EBParseSetting.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
