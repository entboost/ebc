#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T12:26:08
#
#-------------------------------------------------

QT       -= gui

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../../build/CGCClassQtd"
}
else {
TARGET = "../../../build/CGCClassQt"
}
}
else:android {
CONFIG(debug,debug|release) {
TARGET = "../../build/CGCClassQtAndroidd"
}
else {
TARGET = "../../build/CGCClassQtAndroid"
}
}
else {
CONFIG(debug,debug|release) {
TARGET = "../../build/CGCClassQtd"
}
else {
TARGET = "../../build/CGCClassQt"
}
}
#TARGET = CGCClass
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _QT_MAKE_

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
INCLUDEPATH += F:/THIRDPARTY/zlib-1.2.8
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += F:/THIRDPARTY/openssl-1.0.2k/inc32
#LIBS +=F:/THIRDPARTY/boost_1_62_0/stage/lib/libboost_system-mgw53-mt-1_62.a
#LIBS +=F:/THIRDPARTY/boost_1_62_0/stage/lib/libboost_filesystem-mgw53-mt-1_62.a
#INCLUDEPATH += D:/openssl/include
#  LIBS+=-L/usr/lib  -lz -lpcap -lm -lnsl  -lmysqlclient
# libeay32.lib,ssleay32.lib,Ws2_32.lib
 release {
     #CONFIG += console
  }
  else {
     #CONFIG += console
  }
}
else:android {
INCLUDEPATH += F:/THIRDPARTY/zlib-1.2.8
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += F:/THIRDPARTY/openssl-android-master/include
}
else:unix: {
# for OSX
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
}


SOURCES += ../../CGCClass/AttributesImpl.cpp \
    ../../CGCClass/Base64.cpp \
    ../../CGCClass/MethodItem.cpp \
    ../../CGCClass/ModuleItem.cpp \
    ../../CGCClass/ParseCgcSotp2.cpp \
    ../../CGCClass/PpHttp.cpp \
    ../../CGCClass/PpSotp2.cpp \
    ../../CGCClass/SotpCallTable2.cpp \
    ../../CGCClass/SotpRtpRoom.cpp \
    ../../CGCClass/SotpRtpSession.cpp \
    ../../CGCClass/SotpRtpSource.cpp

HEADERS += ../../CGCClass/AttributesImpl.h \
    ../../CGCClass/Base64.h \
    ../../CGCClass/CGCClass.h \
    ../../CGCClass/cgcclassinclude.h \
    ../../CGCClass/clustersvr.h \
    ../../CGCClass/dlldefine.h \
    ../../CGCClass/IpFormat.h \
    ../../CGCClass/md5.h \
    ../../CGCClass/MethodItem.h \
    ../../CGCClass/ModuleItem.h \
    ../../CGCClass/ParseCgcSotp2.h \
    ../../CGCClass/PpHttp.h \
    ../../CGCClass/PpSotp2.h \
    ../../CGCClass/SotpCallTable2.h \
    ../../CGCClass/SotpRtpRoom.h \
    ../../CGCClass/SotpRtpSession.h \
    ../../CGCClass/SotpRtpSource.h \
    ../../CGCClass/tchar.h \
    ../../CGCClass/XmlParseUpload.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
