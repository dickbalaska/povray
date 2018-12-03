# jpeg.pro - qmake for povray/libraries/jpeg
# Copyright 2018 BuckoSoft
#

QT       -= gui core

TEMPLATE = lib
TARGET = jpeg
CONFIG += staticlib warn_off
CONFIG += c++11
#CONFIG += precompile_header

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG(debug, debug|release) {
#	message(Build vfe debug version)
#	DEFINES += _DEBUG
#}
QMAKE_CLEAN += jpeg.lib


jDIR = ../../../libraries/jpeg

#INCLUDEPATH += "../../source"
#INCLUDEPATH += "../../platform/unix"
#INCLUDEPATH += "../../unix/povconfig"


#INCLUDEPATH += $$vDIR
#INCLUDEPATH += $$vqDIR



SOURCES += \
  $$jDIR/jaricom.c \
  $$jDIR/jcapimin.c \
  $$jDIR/jcapistd.c \
  $$jDIR/jcarith.c \
  $$jDIR/jccoefct.c \
  $$jDIR/jccolor.c \
  $$jDIR/jcdctmgr.c \
  $$jDIR/jchuff.c \
  $$jDIR/jcinit.c \
  $$jDIR/jcmainct.c \
  $$jDIR/jcmarker.c \
  $$jDIR/jcmaster.c \
  $$jDIR/jcomapi.c \
  $$jDIR/jcparam.c \
  $$jDIR/jcprepct.c \
  $$jDIR/jcsample.c \
  $$jDIR/jctrans.c \
  $$jDIR/jdapimin.c \
  $$jDIR/jdapistd.c \
  $$jDIR/jdarith.c \
  $$jDIR/jdatadst.c \
  $$jDIR/jdatasrc.c \
  $$jDIR/jdcoefct.c \
  $$jDIR/jdcolor.c \
  $$jDIR/jddctmgr.c \
  $$jDIR/jdhuff.c \
  $$jDIR/jdinput.c \
  $$jDIR/jdmainct.c \
  $$jDIR/jdmarker.c \
  $$jDIR/jdmaster.c \
  $$jDIR/jdmerge.c \
  $$jDIR/jdpostct.c \
  $$jDIR/jdsample.c \
  $$jDIR/jdtrans.c \
  $$jDIR/jerror.c \
  $$jDIR/jfdctflt.c \
  $$jDIR/jfdctfst.c \
  $$jDIR/jfdctint.c \
  $$jDIR/jidctflt.c \
  $$jDIR/jidctfst.c \
  $$jDIR/jidctint.c \
  $$jDIR/jmemmgr.c \
  $$jDIR/jmemnobs.c \
  $$jDIR/jquant1.c \
  $$jDIR/jquant2.c \
  $$jDIR/jutils.c \

HEADERS += \
  $$zDIR/crc32.h \
  $$zDIR/deflate.h \
  $$zDIR/gzguts.h \
  $$zDIR/inffast.h \
  $$zDIR/inffixed.h \
  $$zDIR/inflate.h \
  $$zDIR/inftrees.h \
  $$zDIR/trees.h \
  $$zDIR/zconf.h \
  $$zDIR/zlib.h \
  $$zDIR/zutil.h



