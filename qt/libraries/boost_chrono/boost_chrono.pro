# boost_chrono.pro - qmake for povray/libraries/boost_chrono
# Copyright 2018 BuckoSoft
#
# This is a dummy library to satisfy boost itself that looks for it.
# We just include boost/error_code which will wash out as a duplicate during link.

QT       -= gui core

TEMPLATE = lib
TARGET = libboost_chrono-vc140-mt-gd-1_62
CONFIG(release, debug|release): TARGET = libboost_chrono-vc140-mt-1_62

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
#QMAKE_CLEAN += jpeg.lib

INCLUDEPATH += "../../../libraries/boost"

DEFINES += BOOST_ALL_NO_LIB

bDIR = ../../../libraries/boost/libs/system/src

#INCLUDEPATH += $$zDIR

SOURCES += \
  $$bDIR/error_code.cpp


HEADERS += \

