# vfe.pro - qmake for povray/vfe
# builds: libvfe.a - The vfe interface between libpovray and a frontend
# Copyright 2018 BuckoSoft
#

QT       -= gui

TEMPLATE = lib
TARGET = vfe
CONFIG += staticlib warn_off
CONFIG += c++11
CONFIG += precompile_header

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
QMAKE_CLEAN += libvfe.a


#DEFINES += OPENEXR_MISSING
#DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
#QMAKE_CLEAN += libvfe.a

vDIR = ../../vfe
vqDIR = ../../vfe/qt

INCLUDEPATH += "../../source"
unix {
	INCLUDEPATH += "../../platform/unix"
	INCLUDEPATH += "../../unix/povconfig"
}

win32 {
    INCLUDEPATH += "../../libraries/boost"
    INCLUDEPATH += "../../libraries/zlib"
    INCLUDEPATH += "../../platform/windows"
    INCLUDEPATH += "../../windows/povconfig"
    INCLUDEPATH += "../../vfe/win"
    INCLUDEPATH += "../../libraries/png"
    INCLUDEPATH += "../../libraries/jpeg"
    INCLUDEPATH += "../../libraries/tiff/libtiff"
    #DEFINES += BUILDING_AMD64
    DEFINES += _WINDOWS
    DEFINES += DONT_SHOW_IMAGE_LIB_VERSIONS
    DEFINES += OPENEXR_MISSING
}

INCLUDEPATH += $$vDIR
INCLUDEPATH += $$vqDIR

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter
#configh.input = $$vqDIR/config.h.in
#configh.output = $$vqDIR/config.h
#QMAKE_SUBSTITUTES += configh


SOURCES += \
	$$vDIR/vfe.cpp \
	$$vDIR/vfecontrol.cpp \
	$$vDIR/vfedisplay.cpp \
    $$vDIR/vfepovms.cpp \
    $$vDIR/vfesession.cpp \
    $$vqDIR/qtvfe.cpp \
    $$vqDIR/qtgraphics.cpp \
    $$vqDIR/qtoptions.cpp \
    $$vqDIR/vfeplatform.cpp


HEADERS += \
    $$vDIR/vfe.h \
    $$vDIR/vfesession.h \
    $$vDIR/vfepovms.h \
    $$vqDIR/qtvfe.h \
    $$vqDIR/qtoptions.h \
    $$vqDIR/qtgraphics.h \
    $$vqDIR/vfeplatform.h \
    $$vqDIR/config.h.in


#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

