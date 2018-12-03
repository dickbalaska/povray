# vfe.pro - qmake for povray/vfe
# builds: libvfe.a - The vfe interface between libpovray and a frontend
# Copyright 2018 BuckoSoft
#

QT       -= gui core

TEMPLATE = lib
TARGET = zlib
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
QMAKE_CLEAN += zlib.lib


#DEFINES += OPENEXR_MISSING
#DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
#QMAKE_CLEAN += libvfe.a

zDIR = ../../../libraries/zlib

#INCLUDEPATH += "../../source"
#INCLUDEPATH += "../../platform/unix"
#INCLUDEPATH += "../../unix/povconfig"


#INCLUDEPATH += $$vDIR
#INCLUDEPATH += $$vqDIR



SOURCES += \
  $$zDIR/adler32.c \
  $$zDIR/compress.c \
  $$zDIR/crc32.c \
  $$zDIR/deflate.c \
  $$zDIR/gzclose.c \
  $$zDIR/gzlib.c \
  $$zDIR/gzread.c \
  $$zDIR/gzwrite.c \
  $$zDIR/infback.c \
  $$zDIR/inffast.c \
  $$zDIR/inflate.c \
  $$zDIR/inftrees.c \
  $$zDIR/trees.c \
  $$zDIR/uncompr.c \
  $$zDIR/zutil.c

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



