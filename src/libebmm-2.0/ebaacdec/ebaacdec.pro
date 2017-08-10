#-------------------------------------------------
#
# Project created by QtCreator 2017-08-04T18:16:58
#
#-------------------------------------------------

QT       -= core gui

win32 {
CONFIG(debug,debug|release) {
TARGET = "../../build/ebaacdecd"
}
else {
TARGET = "../../build/ebaacdec"
}

}
else {
CONFIG(debug,debug|release) {
TARGET = "../build/ebaacdecd"
}
else {
TARGET = "../build/ebaacdec"
}
}
#TARGET = ebaacdec
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

INCLUDEPATH += "./include"
INCLUDEPATH += "./libfaad"

SOURCES += \
        ebaacdecoder.cpp \
    libfaad/bits.c \
    libfaad/cfft.c \
    libfaad/common.c \
    libfaad/decoder.c \
    libfaad/drc.c \
    libfaad/drm_dec.c \
    libfaad/error.c \
    libfaad/filtbank.c \
    libfaad/hcr.c \
    libfaad/huffman.c \
    libfaad/ic_predict.c \
    libfaad/is.c \
    libfaad/lt_predict.c \
    libfaad/mdct.c \
    libfaad/mp4.c \
    libfaad/ms.c \
    libfaad/output.c \
    libfaad/pns.c \
    libfaad/ps_dec.c \
    libfaad/ps_syntax.c \
    libfaad/pulse.c \
    libfaad/rvlc.c \
    libfaad/sbr_dct.c \
    libfaad/sbr_dec.c \
    libfaad/sbr_e_nf.c \
    libfaad/sbr_fbt.c \
    libfaad/sbr_hfadj.c \
    libfaad/sbr_hfgen.c \
    libfaad/sbr_huff.c \
    libfaad/sbr_qmf.c \
    libfaad/sbr_syntax.c \
    libfaad/sbr_tf_grid.c \
    libfaad/specrec.c \
    libfaad/ssr.c \
    libfaad/ssr_fb.c \
    libfaad/ssr_ipqf.c \
    libfaad/syntax.c \
    libfaad/tns.c

HEADERS += \
        ebaacdecoder.h \
    include/faad.h \
    include/neaacdec.h \
    libfaad/codebook/hcb.h \
    libfaad/codebook/hcb_1.h \
    libfaad/codebook/hcb_10.h \
    libfaad/codebook/hcb_11.h \
    libfaad/codebook/hcb_2.h \
    libfaad/codebook/hcb_3.h \
    libfaad/codebook/hcb_4.h \
    libfaad/codebook/hcb_5.h \
    libfaad/codebook/hcb_6.h \
    libfaad/codebook/hcb_7.h \
    libfaad/codebook/hcb_8.h \
    libfaad/codebook/hcb_9.h \
    libfaad/codebook/hcb_sf.h \
    libfaad/analysis.h \
    libfaad/bits.h \
    libfaad/cfft.h \
    libfaad/cfft_tab.h \
    libfaad/common.h \
    libfaad/decoder.h \
    libfaad/drc.h \
    libfaad/drm_dec.h \
    libfaad/error.h \
    libfaad/filtbank.h \
    libfaad/fixed.h \
    libfaad/huffman.h \
    libfaad/ic_predict.h \
    libfaad/iq_table.h \
    libfaad/is.h \
    libfaad/kbd_win.h \
    libfaad/lt_predict.h \
    libfaad/mdct.h \
    libfaad/mdct_tab.h \
    libfaad/mp4.h \
    libfaad/ms.h \
    libfaad/output.h \
    libfaad/pns.h \
    libfaad/ps_dec.h \
    libfaad/ps_tables.h \
    libfaad/pulse.h \
    libfaad/rvlc.h \
    libfaad/sbr_dct.h \
    libfaad/sbr_dec.h \
    libfaad/sbr_e_nf.h \
    libfaad/sbr_fbt.h \
    libfaad/sbr_hfadj.h \
    libfaad/sbr_hfgen.h \
    libfaad/sbr_huff.h \
    libfaad/sbr_noise.h \
    libfaad/sbr_qmf.h \
    libfaad/sbr_qmf_c.h \
    libfaad/sbr_syntax.h \
    libfaad/sbr_tf_grid.h \
    libfaad/sine_win.h \
    libfaad/specrec.h \
    libfaad/ssr.h \
    libfaad/ssr_fb.h \
    libfaad/ssr_ipqf.h \
    libfaad/ssr_win.h \
    libfaad/structs.h \
    libfaad/syntax.h \
    libfaad/tns.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
