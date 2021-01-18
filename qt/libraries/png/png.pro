# png.pro - qmake for povray/libraries/png
# Copyright 2018 BuckoSoft
#

QT       -= gui core

TEMPLATE = lib
TARGET = png
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


pDIR = ../../../libraries/png
zDIR = ../../../libraries/zlib

INCLUDEPATH += $$zDIR

SOURCES += \
  $$pDIR/png.c \
  $$pDIR/pngerror.c \
  $$pDIR/pngget.c \
  $$pDIR/pngmem.c \
  $$pDIR/pngpread.c \
  $$pDIR/pngread.c \
  $$pDIR/pngrio.c \
  $$pDIR/pngrtran.c \
  $$pDIR/pngrutil.c \
  $$pDIR/pngset.c \
  $$pDIR/pngtrans.c \
  $$pDIR/pngwio.c \
  $$pDIR/pngwrite.c \
  $$pDIR/pngwtran.c \
  $$pDIR/pngwutil.c \


HEADERS += \
  $$pDIR/png.h \
	../../../libraries/png/pnglibconf.h

