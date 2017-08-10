#-------------------------------------------------
#
# Project created by QtCreator 2017-04-29T11:00:32
#
#-------------------------------------------------

#QT       += core gui network
QT       -= gui
QT       += network
QT +=multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/ebmmd"
}
else {
TARGET = "../../build/ebmm"
}
}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/ebmmd"
}
else {
TARGET = "../build/ebmm"
}
}
TEMPLATE = lib

#DEFINES += LIBEBUM_LIBRARY
DEFINES += _QT_MAKE_
DEFINES += EBMM_EXPORTS

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
INCLUDEPATH += "C:/ffmpeg/include"
INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += "F:/THIRDPARTY/boost_1_62_0"
INCLUDEPATH += "F:/THIRDPARTY/openssl-1.0.2k/inc32"
INCLUDEPATH += "../../mycp"
INCLUDEPATH += "../../mycp/ThirdParty"
INCLUDEPATH += "../ebaacenc"
INCLUDEPATH += "../ebaacdec"

    Release {
LIBS += -L"../../mycp/build" -lCGCLibQt -lCGCClassQt
LIBS += -L"../build" -lebaacenc -lebaacdec
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-1_62 -llibboost_thread-vc140-mt-1_62 -llibboost_filesystem-vc140-mt-1_62
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstat
    }
    Debug {
LIBS += -L"../../mycp/build" -lCGCLibQtd -lCGCClassQtd
LIBS += -L"../build" -lebaacencd -lebaacdecd
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-gd-1_62 -llibboost_thread-vc140-mt-gd-1_62 -llibboost_filesystem-vc140-mt-gd-1_62
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstatd
    }
LIBS += -L"C:/ffmpeg/bin" -lavcodec -lavformat -lavutil -lswscale
LIBS += -L"F:/THIRDPARTY/openssl-1.0.2k/vc2015" -llibeay32 -lssleay32
LIBS += -lWinmm

}
else:unix: {
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
#INCLUDEPATH += "/usr/local/opt/zlib/include"
#INCLUDEPATH += C:/ffmpeg/include
INCLUDEPATH += "../../mycp"
INCLUDEPATH += "../../mycp/ThirdParty"

CONFIG(debug,debug|release) {
LIBS += "../../mycp/build/libCGCClassQtd.a"
LIBS += "../../mycp/build/libCGCLibQtd.a"
}
else {
LIBS += "../../mycp/build/libCGCClassQt.a"
LIBS += "../../mycp/build/libCGCLibQt.a"
}

LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_system.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_thread.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_filesystem.a"
#LIBS += -L"/usr/lib" -lz
#LIBS += -L"/usr/lib" -lcrypto -lssl
}


SOURCES += ebmm.cpp \
    ../../include/EBRtpManager.cpp \
    ebmmmanager.cpp \
    ../include/ebabstractvideosurface.cpp \
    ../include/ebaudioinputiodevice.cpp \
    ../include/ebffmpegproxy.cpp \
    ebrtpsinkinfo.cpp \
    ebrtproominfolist.cpp \
    ebp2ptaskinfo.cpp \
    ebvideostreammanager.cpp \
    ebvideobuffer.cpp \
    ebaudiobuffer.cpp \
    ebaudiostreammanager.cpp \
    ebvideowindow.cpp \
    ebvideostreamitem.cpp \
    ebvideoframe.cpp \
    ebaudiostreamitem.cpp \
    ebvoicebufrer.cpp \
    ebfourier.cpp

HEADERS += ebmm.h \
    ebmmhandle.h \
    ../../include/EBRtpManager.h \
    ebmmmanager.h \
    ../include/ebabstractvideosurface.h \
    ../include/ebaudioinputiodevice.h \
    ../include/ebffmpegproxy.h \
    ebrtpsinkinfo.h \
    ebrtproominfolist.h \
    ebp2ptaskinfo.h \
    ebvideostreammanager.h \
    ebvideobuffer.h \
    ebaudiobuffer.h \
    ebaudiostreammanager.h \
    ebvideowindow.h \
    ebvideostreamitem.h \
    ebvideoframe.h \
    ebtimegettime.h \
    ebvideodefine.h \
    ebaudiostreamitem.h \
    ebvoicebuffer.h \
    ebfourier.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
