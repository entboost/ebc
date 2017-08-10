#-------------------------------------------------
#
# Project created by QtCreator 2017-08-04T17:35:46
#
#-------------------------------------------------

QT       -= core gui

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/ebaacencd"
}
else {
TARGET = "../../build/ebaacenc"
}

}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/ebaacencd"
}
else {
TARGET = "../build/ebaacenc"
}
}
#TARGET = faacenc
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "./include"
INCLUDEPATH += "./libfaac"

SOURCES += \
    libfaac/kiss_fft/kiss_fft.c \
    libfaac/kiss_fft/kiss_fftr.c \
    libfaac/aacquant.c \
    libfaac/backpred.c \
    libfaac/bitstream.c \
    libfaac/channels.c \
    libfaac/fft.c \
    libfaac/filtbank.c \
    libfaac/frame.c \
    libfaac/huffman.c \
    libfaac/ltp.c \
    libfaac/midside.c \
    libfaac/psychkni.c \
    libfaac/tns.c \
    libfaac/util.c \
    ebaacencoder.cpp

HEADERS += \
    include/faac.h \
    include/faaccfg.h \
    libfaac/kiss_fft/_kiss_fft_guts.h \
    libfaac/kiss_fft/kiss_fft.h \
    libfaac/kiss_fft/kiss_fftr.h \
    libfaac/aacquant.h \
    libfaac/backpred.h \
    libfaac/bitstream.h \
    libfaac/channels.h \
    libfaac/coder.h \
    libfaac/fft.h \
    libfaac/filtbank.h \
    libfaac/frame.h \
    libfaac/huffman.h \
    libfaac/hufftab.h \
    libfaac/ltp.h \
    libfaac/midside.h \
    libfaac/psych.h \
    libfaac/tns.h \
    libfaac/util.h \
    libfaac/version.h \
    ebaacencoder.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
