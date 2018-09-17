#-------------------------------------------------
#
# Project created by QtCreator 2017-04-26T10:21:32
#
#-------------------------------------------------

QT       -= gui
#android {
#}
#else {
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#}

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/ebcmd"
}
else {
TARGET = "../../build/ebcm"
}

}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/ebcmd"
}
else {
TARGET = "../build/ebcm"
}
}

#OBJECTS_DIR = "../../build/"
#TARGET = libebcm
TEMPLATE = lib

#DEFINES += LIBEBCM_LIBRARY
DEFINES += _QT_MAKE_
DEFINES += CHATROOM_EXPORTS
DEFINES -= UNICODE
DEFINES -= _UNICODE

#android {
#DEFINES += __ANDROID__
#}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
android {
DEFINES += OPENSSL_NO_ENGINE
}

#DEFINES += OPENSSL_NO_ENGINE

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
INCLUDEPATH += F:/THIRDPARTY/zlib-1.2.8
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += F:/THIRDPARTY/openssl-1.0.2k/inc32
INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"

 Release {
LIBS += -L"../../../mycp/build" -lCGCLibQt -lCGCClassQt
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-1_62 -llibboost_thread-vc140-mt-1_62 -llibboost_filesystem-vc140-mt-1_62
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstat
  }
  Debug {
LIBS += -L"../../../mycp/build" -lCGCLibQtd -lCGCClassQtd
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-gd-1_62 -llibboost_thread-vc140-mt-gd-1_62 -llibboost_filesystem-vc140-mt-gd-1_62
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstatd
  }
LIBS += -lWs2_32 -lshlwapi -lWinmm
## 使用静态库会报错
LIBS += -L"F:/THIRDPARTY/openssl-1.0.2k/vc2015" -llibeay32 -lssleay32
}
else:android {
INCLUDEPATH += F:/THIRDPARTY/zlib-1.2.8
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += F:/THIRDPARTY/openssl-android-master/include
INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"

CONFIG(debug,debug|release) {
LIBS += "../../../mycp/build/libCGCLibQtAndroidd.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroidd.a"
}
else {
LIBS += "../../../mycp/build/libCGCLibQtAndroid.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroid.a"
}
#LIBS += "F:/THIRDPARTY/zlib-1.2.8/android/libzlib.a"
LIBS += -L"F:/THIRDPARTY/openssl-android-master/libs/armeabi-v7a" -lcrypto -lssl
#LIBS += -lz
}
else:unix: {
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
INCLUDEPATH += "/usr/local/opt/openssl/include"
#INCLUDEPATH += "/usr/local/opt/zlib/include"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"

CONFIG(debug,debug|release) {
LIBS += "../../../mycp/build/libCGCClassQtd.a"
LIBS += "../../../mycp/build/libCGCLibQtd.a"
}
else {
LIBS += "../../../mycp/build/libCGCClassQt.a"
LIBS += "../../../mycp/build/libCGCLibQt.a"
}
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_system.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_thread.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_filesystem.a"
LIBS += -L"/usr/lib" -lz
LIBS += -L"/usr/local/opt/openssl/lib" -lcrypto -lssl

}

android {
SOURCES += ../../../libchatroom/chatroom.cpp \
    ../../../libchatroom/md5.cpp \
    ../../../include/POPCChatManager.cpp \
    ../../../include/SendFileThread.cpp \
    ../boost_1.62/thread/pthread/once.cpp \
    ../boost_1.62/thread/pthread/once_atomic.cpp \
    ../boost_1.62/thread/pthread/thread.cpp \
    ../boost_1.62/thread/future.cpp \
    ../boost_1.62/thread/tss_null.cpp \
    ../boost_1.62/filesystem/codecvt_error_category.cpp \
    ../boost_1.62/filesystem/operations.cpp \
    ../boost_1.62/filesystem/path.cpp \
    ../boost_1.62/filesystem/path_traits.cpp \
    ../boost_1.62/filesystem/portability.cpp \
    ../boost_1.62/filesystem/unique_path.cpp \
    ../boost_1.62/filesystem/utf8_codecvt_facet.cpp \
    ../boost_1.62/filesystem/windows_file_codecvt.cpp \
    ../boost_1.62/system/error_code.cpp
}
else {
SOURCES += ../../../libchatroom/chatroom.cpp \
    ../../../libchatroom/md5.cpp \
    ../../../include/POPCChatManager.cpp \
    ../../../include/SendFileThread.cpp
}

HEADERS += ../../../libchatroom/chatroom.h \
    ../../../libchatroom/md5.h \
    ../../../libchatroom/pophandler.h \
    ../../../libchatroom/resource.h \
    ../../../include/POPCChatManager.h \
    ../../../include/SendFileThread.h \
    ../../../include/chatroomhandle.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
