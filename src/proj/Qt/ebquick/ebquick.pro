QT += qml quick network multimedia
android {
QT += androidextras
}

# sql
#QT += webengine

#TARGET = "entboost"

CONFIG += c++11

SOURCES += main.cpp \
    ../entboost/ebclocales.cpp \
    ebclientapp.cpp \
    ../entboost/ebclientappbase.cpp \
    boost_1.62/system/detail/error_code.cpp \
    ../entboost/ebhttpfiledownload.cpp \
    ebkeyfilter.cpp \
    ebccallinfo.cpp \
    ebfileinfo.cpp \
    ebvoicerecorder.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _QT_MAKE_
DEFINES += _QT_QML_

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../entboost/ebclocales.h \
    ebclientapp.h \
    ebclientapp.h \
    ../entboost/ebclientappbase.h \
    ../entboost/ebhttpfiledownload.h \
    ebkeyfilter.h \
    ebquickiteminfo.h \
    ebccallinfo.h \
    ebfileinfo.h \
    ebvoicerecorder.h

#OTHER_FILES += $${CONF_FILES} \
#               $${QML_FILES}
#qml_files.path = /assets

win32 {
#INCLUDEPATH += "F:/THIRDPARTY/zlib-1.2.8"
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += "F:/THIRDPARTY/sqlite-3081002"
#INCLUDEPATH += F:/THIRDPARTY/openssl-1.0.2k/inc32
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
#INCLUDEPATH += "../../../libchatroom"
#INCLUDEPATH += "../../../libpopusermanager"
#INCLUDEPATH += "../../../libebmm-2.0/libebmm"
#INCLUDEPATH += "../../../include"

    Release {
LIBS += -L"../../../mycp/build" -lCGCLibQt -lCGCClassQt
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-1_62 -llibboost_thread-vc140-mt-1_62 -llibboost_filesystem-vc140-mt-1_62
LIBS += -lWs2_32 -lshlwapi
LIBS += -L"../build" -lebcm -lebum
LIBS += -L"../../../libebmm-2.0/build" -lebmm
LIBS += -L"F:/THIRDPARTY/sqlite-3081002/lib" -lsqlite3s
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstat
    }
    Debug {
LIBS += -L"../../../mycp/build" -lCGCLibQtd -lCGCClassQtd
LIBS += -L"F:/THIRDPARTY/boost_1_62_0/stage/lib" -llibboost_system-vc140-mt-gd-1_62 -llibboost_thread-vc140-mt-gd-1_62 -llibboost_filesystem-vc140-mt-gd-1_62
#LIBS += -lWs2_32 -lshlwapi
LIBS += -L"../build" -lebcmd -lebumd
#LIBS += -L"../../../libebmm-2.0/build" -lebmmd
LIBS += -L"F:/THIRDPARTY/sqlite-3081002/lib" -lsqlite3sd
#LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstatd
    }
#LIBS += -L"F:/THIRDPARTY/openssl-1.0.2k/vc2015" -llibeay32 -lssleay32
}
else:android {
INCLUDEPATH += F:/THIRDPARTY/boost_1_62_0
INCLUDEPATH += "F:/THIRDPARTY/sqlite-3081002"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"

CONFIG(debug,debug|release) {
LIBS += -L"../build" -lebcmd -lebumd
LIBS += "../../../mycp/build/libCGCLibQtAndroidd.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroidd.a"
LIBS += "F:/THIRDPARTY/sqlite-3081002/Qt/build/libsqliteandroidd.a"
}
else {
LIBS += -L"../build" -lebcm -lebum
LIBS += "../../../mycp/build/libCGCLibQtAndroid.a"
LIBS += "../../../mycp/build/libCGCClassQtAndroid.a"
LIBS += "F:/THIRDPARTY/sqlite-3081002/Qt/build/libsqliteandroid.a"
}
LIBS += -L"F:/THIRDPARTY/openssl-android-master/libs/armeabi-v7a" -lcrypto -lssl
LIBS += -lz
}
else:unix: {
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
#INCLUDEPATH += "/usr/local/include/sqlite3"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
#INCLUDEPATH += "../../../libchatroom"
#INCLUDEPATH += "../../../libpopusermanager"
#INCLUDEPATH += "../../../libebmm-2.0/libebmm"
#INCLUDEPATH += "../../../include"

CONFIG(debug,debug|release) {
LIBS += -L"../../../mycp/build" -lCGCClassQtd -lCGCLibQtd
#LIBS += -L"../../../libebmm-2.0/build" -lebmmd
#LIBS += -L"../build" -lebcmd -lebumd
}
else {
LIBS += -L"../../../mycp/build" -lCGCClassQt -lCGCLibQt
#LIBS += -L"../../../libebmm-2.0/build" -lebmm
#LIBS += -L"../build" -lebcm -lebum
}
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_system.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_thread.a"
LIBS += "/Users/akee/src/boost_1_62_0/stage/lib/libboost_filesystem.a"
}

#android {
#assets = assets
##assets.source = assets
##assets.target = .
#DEPLOYMENTFOLDERS = assets
#}

#ANDROID_PACKAGE_SOURCE_DIR = "D/dev/entboost/src/proj/Qt/ebquick/android"
#ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
#ANDROID_DEPLOYMENT_DEPENDENCIES += "D/dev/entboost/src/proj/Qt/ebquick/android/libs/libebumd.so"

#OTHER_FILES += \
#    android/AndroidManifest.xml \
#    android/project.properties

OTHER_FILES += \
    android/src/org/qtproject/entboost/notification/NotificationClient.java

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../build/libcrypto.so \
        $$PWD/../build/libssl.so \
        $$PWD/../build/libebcm.so \
        $$PWD/../build/libebum.so
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

#android {
#    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
#    ANDROID_JAVA_SOURCES.path = /src/org/qtproject/entboost
#    ANDROID_JAVA_SOURCES.files = $$files($$PWD/java/*.java)
#    INSTALLS += ANDROID_JAVA_SOURCES
#}
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
