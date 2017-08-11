#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T15:27:38
#
#-------------------------------------------------

QT       += core gui network
#QT += webengine
QT += webenginewidgets
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/entboostd"
}
else {
TARGET = "../../build/entboost"
}
}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/entboostd"
}
else {
TARGET = "../build/entboost"
}
}
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _QT_MAKE_
DEFINES += _QT_USES_WEBENGINE_
DEFINES += _EB_USES_VIDEO_ROOM

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
INCLUDEPATH += "../../../libpopusermanager"
INCLUDEPATH += "../../../libebmm-2.0/libebmm"
INCLUDEPATH += "../../../include"

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
LIBS += -lWs2_32 -lshlwapi
LIBS += -L"../build" -lebcmd -lebumd
LIBS += -L"../../../libebmm-2.0/build" -lebmmd
LIBS += -L"F:/THIRDPARTY/sqlite-3081002/lib" -lsqlite3sd
LIBS += -L"F:/THIRDPARTY/zlib-1.2.8/contrib/vstudio/vc14/lib" -lzlibstatd
    }
LIBS += -L"F:/THIRDPARTY/openssl-1.0.2k/vc2015" -llibeay32 -lssleay32
}
else:unix: {
INCLUDEPATH += "/Users/akee/src/boost_1_62_0"
#INCLUDEPATH += "/usr/local/opt/zlib/include"
INCLUDEPATH += "/usr/local/include/sqlite3"
INCLUDEPATH += "../../../mycp"
INCLUDEPATH += "../../../mycp/ThirdParty"
INCLUDEPATH += "../../../libchatroom"
INCLUDEPATH += "../../../libpopusermanager"
INCLUDEPATH += "../../../libebmm-2.0/libebmm"
INCLUDEPATH += "../../../include"

CONFIG(debug,debug|release) {
LIBS += -L"../../../mycp/build" -lCGCClassQtd -lCGCLibQtd
LIBS += -L"../../../libebmm-2.0/build" -lebmmd
LIBS += -L"../build" -lebcmd -lebumd
}
else {
LIBS += -L"../../../mycp/build" -lCGCClassQt -lCGCLibQt
LIBS += -L"../../../libebmm-2.0/build" -lebmm
LIBS += -L"../build" -lebcm -lebum
}
LIBS += -L"/Users/akee/src/boost_1_62_0/stage/lib" -lboost_system -lboost_thread -lboost_filesystem
}

SOURCES += main.cpp\
        ../../../include/ebc_public.cpp \
    ebclientapp.cpp \
    ebclocales.cpp \
    ebmessagebox.cpp \
    ebtextedit.cpp \
    ebwidgetchatinput.cpp \
    ebtextbrowser.cpp \
    ebwidgetchatright.cpp \
    ebframeitem.cpp \
    ebframelist.cpp \
    ebccallinfo.cpp \
    eblistwidgetitem.cpp \
    ebwidgetmygroup.cpp \
    ebtreewidgetitem.cpp \
    eblabel.cpp \
    ebwidgetaccountinfo.cpp \
    ebwidgetmysetting.cpp \
    ebwidgetmodifypassword.cpp \
    ebwidgetsearchbar.cpp \
    ebwidgetsearchresult.cpp \
    ebwidgetworkview.cpp \
    ebworkitem.cpp \
    ebworklist.cpp \
    ebwebengineview.cpp \
    ebwebenginepage.cpp \
    ebwebengineprofile.cpp \
    ebwebenginedownloaditem.cpp \
    eblineedit.cpp \
    eblistwidget.cpp \
    ebwebengineurlrequestinterceptor.cpp \
    ebframefindtext.cpp \
    ebwidgetappbar.cpp \
    ebappbuttonitem.cpp \
    ebwebengineurlschemehandler.cpp \
    ebtextblockuserdata.cpp \
    ebframechattoolbar.cpp \
    ebwidgetuserinfo.cpp \
    ebwidgetuserlist.cpp \
    ebwidgetiteminfo.cpp \
    ebcontextmenu.cpp \
    ebwidgetfiletranlist.cpp \
    ebwidgetfiletranitem.cpp \
    ebwidgetchatrecord.cpp \
    ebdialogfilemanager.cpp \
    ebwidgetmycontact.cpp \
    ebdialogcontactinfo.cpp \
    ebwidgetmysession.cpp \
    ebcallrecordinfo.cpp \
    ebdialogrequestaddcontact.cpp \
    ebwidgetmyenterprise.cpp \
    ebdialogselectuser.cpp \
    ebwidgettreeselectbase.cpp \
    ebdialogviewecard.cpp \
    ebhttpfiledownload.cpp \
    ebdialogbase.cpp \
    ebdialoglogin.cpp \
    ebdialogregister.cpp \
    ebdialogchangehead.cpp \
    ebdialogchatbase.cpp \
    ebdialogconnectsetting.cpp \
    ebdialogemotionselect.cpp \
    ebdialogmainframe.cpp \
    ebdialogworkframe.cpp \
    ebdialogframelist.cpp \
    ebiconhelper.cpp \
    ebdialoggroupinfo.cpp \
    ebdialogmemberinfo.cpp \
    ebdialogmessagetip.cpp \
    ebdialogmycenter.cpp \
    ebdialogpoptip.cpp \
    ebwebenginedownloadrequested.cpp \
    ebwidgetvideosetting.cpp \
    ebwidgetvideowindow.cpp \
    ebwidgetvoicebar.cpp \
    ebwidgetvideoframe.cpp

HEADERS  += ../../../include/ebc_public.h \
    ebclientapp.h \
    ebclocales.h \
    ebmessagebox.h \
    ebtextedit.h \
    ebwidgetchatinput.h \
    ebtextbrowser.h \
    ebwidgetchatright.h \
    ebframeitem.h \
    ebframelist.h \
    ebccallinfo.h \
    eblistwidgetitem.h \
    ebwidgetmygroup.h \
    ebtreewidgetitem.h \
    eblabel.h \
    ebwidgetaccountinfo.h \
    ebwidgetmysetting.h \
    ebwidgetmodifypassword.h \
    ebwidgetsearchbar.h \
    ebwidgetsearchresult.h \
    ebwidgetworkview.h \
    ebworkitem.h \
    ebworklist.h \
    ebwebengineview.h \
    ebwebenginepage.h \
    ebwebengineprofile.h \
    ebwebenginedownloaditem.h \
    eblineedit.h \
    eblistwidget.h \
    ebwebengineurlrequestinterceptor.h \
    ebframefindtext.h \
    ebwidgetappbar.h \
    ebappbuttonitem.h \
    ebwebengineurlschemehandler.h \
    ebtextblockuserdata.h \
    ebframechattoolbar.h \
    ebwidgetuserinfo.h \
    ebwidgetuserlist.h \
    ebwidgetiteminfo.h \
    ebcontextmenu.h \
    ebwidgetfiletranlist.h \
    ebwidgetfiletranitem.h \
    ebwidgetchatrecord.h \
    ebdialogfilemanager.h \
    ebwidgetmycontact.h \
    ebdialogcontactinfo.h \
    ebwidgetmysession.h \
    ebcallrecordinfo.h \
    ebdialogrequestaddcontact.h \
    ebwidgetmyenterprise.h \
    ebdialogselectuser.h \
    ebcallback.h \
    ebdefines.h \
    ebwidgettreeselectbase.h \
    ebdialogviewecard.h \
    ebhttpfiledownload.h \
    ebdialogbase.h \
    ebdialoglogin.h \
    ebdialogregister.h \
    ebdialogchangehead.h \
    ebdialogchatbase.h \
    ebdialogconnectsetting.h \
    ebdialogemotionselect.h \
    ebdialogmainframe.h \
    ebdialogworkframe.h \
    ebdialogframelist.h \
    ebiconhelper.h \
    ebdialoggroupinfo.h \
    ebdialogmemberinfo.h \
    ebdialogmessagetip.h \
    ebdialogmycenter.h \
    ebdialogpoptip.h \
    ebwebenginedownloadrequested.h \
    ebwidgetvideosetting.h \
    ebwidgetvideowindow.h \
    ebwidgetvoicebar.h \
    ebwidgetvideoframe.h

FORMS    += ebmessagebox.ui \
    ebdialogfilemanager.ui \
    ebdialogcontactinfo.ui \
    ebdialogrequestaddcontact.ui \
    ebdialogselectuser.ui \
    ebdialogviewecard.ui \
    ebdialoglogin.ui \
    ebdialogregister.ui \
    ebdialogchangehead.ui \
    ebdialogchatbase.ui \
    ebdialogconnectsetting.ui \
    ebdialogemotionselect.ui \
    ebdialogmainframe.ui \
    ebdialogworkframe.ui \
    ebdialogframelist.ui \
    ebdialoggroupinfo.ui \
    ebdialogmemberinfo.ui \
    ebdialogmessagetip.ui \
    ebdialogmycenter.ui \
    ebdialogpoptip.ui

RESOURCES += \
    entboost.qrc

win32 {
RC_FILE = entboost.rc
}
else:unix: {
ICON = entboost.icns
}
