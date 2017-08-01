#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T16:31:56
#
#-------------------------------------------------

QT       -= gui

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../../build/CGCLibQtd"
}
else {
TARGET = "../../../build/CGCLibQt"
}
}
else {
CONFIG(debug,debug|release) {
TARGET = "../../build/CGCLibQtd"
}
else {
TARGET = "../../build/CGCLibQt"
}
}
#TARGET = CGCLib
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
INCLUDEPATH += F:/THIRDPARTY/zlib-1.2.8
}
else:unix: {
# for OSX
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
}

SOURCES += ../../CGCLib/CgcBaseClient.cpp \
    ../../CGCLib/CgcRtpClient.cpp \
    ../../CGCLib/CgcTcpClient.cpp \
    ../../CGCLib/CgcUdpClient.cpp \
    ../../CGCLib/SotpClient.cpp

HEADERS += ../../CGCLib/cgc_sotpclient.h \
    ../../CGCLib/CgcBaseClient.h \
    ../../CGCLib/CgcClientHandler.h \
    ../../CGCLib/CgcData.h \
    ../../CGCLib/CGCLib.h \
    ../../CGCLib/CgcRtpClient.h \
    ../../CGCLib/CgcTcpClient.h \
    ../../CGCLib/CgcUdpClient.h \
    ../../CGCLib/dlldefine.h \
    ../../CGCLib/IncludeBase.h \
    ../../CGCLib/SotpClient.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
