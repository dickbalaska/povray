# tiff.pro - qmake for povray/libraries/tiff
# Copyright 2018 BuckoSoft
#

QT       -= gui core

TEMPLATE = lib
TARGET = tiff
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
#QMAKE_CLEAN += tiff.lib


tDIR = ../../../libraries/tiff/libtiff

#INCLUDEPATH += "../../source"
#INCLUDEPATH += "../../platform/unix"
#INCLUDEPATH += "../../unix/povconfig"


#INCLUDEPATH += $$vDIR
#INCLUDEPATH += $$vqDIR

DEFINES += DWORD="unsigned long"

SOURCES += \
  $$tDIR/tif_aux.c \
  $$tDIR/tif_close.c \
  $$tDIR/tif_codec.c \
  $$tDIR/tif_color.c \
  $$tDIR/tif_compress.c \
  $$tDIR/tif_dir.c \
  $$tDIR/tif_dirinfo.c \
  $$tDIR/tif_dirread.c \
  $$tDIR/tif_dirwrite.c \
  $$tDIR/tif_dumpmode.c \
  $$tDIR/tif_error.c \
  $$tDIR/tif_fax3.c \
  $$tDIR/tif_fax3sm.c \
  $$tDIR/tif_flush.c \
  $$tDIR/tif_getimage.c \
  $$tDIR/tif_jpeg.c \
  $$tDIR/tif_luv.c \
  $$tDIR/tif_lzw.c \
  $$tDIR/tif_next.c \
  $$tDIR/tif_open.c \
  $$tDIR/tif_packbits.c \
  $$tDIR/tif_pixarlog.c \
  $$tDIR/tif_predict.c \
  $$tDIR/tif_print.c \
  $$tDIR/tif_read.c \
  $$tDIR/tif_strip.c \
  $$tDIR/tif_swab.c \
  $$tDIR/tif_thunder.c \
  $$tDIR/tif_tile.c \
  $$tDIR/tif_version.c \
  $$tDIR/tif_warning.c \
  $$tDIR/tif_win32.c \
  $$tDIR/tif_write.c \
  $$tDIR/tif_zip.c \

HEADERS += \
  $$tDIR/t4.h \
  $$tDIR/tif_config.h \
  $$tDIR/tif_dir.h \
  $$tDIR/tif_fax3.h \
  $$tDIR/tif_predict.h \
  $$tDIR/tiff.h \
  $$tDIR/tiffconf.h \
  $$tDIR/tiffio.h \
  $$tDIR/tiffiop.h \
  $$tDIR/tiffvers.h \
  $$tDIR/uvcode.h



