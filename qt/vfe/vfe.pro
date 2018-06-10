# source.pro - qmake for povray/source
# builds: libpovray.a - The main povray library
# Copyright 2018 BuckoSoft
#

QT       -= gui

TARGET = vfe
TEMPLATE = lib
CONFIG += staticlib warn_off
CONFIG += object_parallel_to_source

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug) {
	message(Build debug version)
	DEFINES += _DEBUG
}

DEFINES += OPENEXR_MISSING
DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
QMAKE_CLEAN += libvfe.a

vDIR = ../../vfe
vqDIR = ../../vfe/qt

INCLUDEPATH += "../../source"
INCLUDEPATH += "../../platform/unix"
INCLUDEPATH += "../../unix/povconfig"
INCLUDEPATH += $$vDIR
INCLUDEPATH += $$vqDIR

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

SOURCES += \
	$$vDIR/vfe.cpp \
	$$vDIR/vfecontrol.cpp \
	$$vDIR/vfedisplay.cpp \
	$$vDIR/vfepovms.cpp \
	$$vDIR/vfesession.cpp \
	$$vqDIR/qtvfe.cpp \
	$$vqDIR/vfeplatform.cpp \
	$$vqDIR/qtgraphics.cpp \
	$$vqDIR/qtoptions.cpp \
#	$$vqDIR/qtconsole.cpp \
#	$$vqDIR/qthandler.cpp \
#	$$vqDIR/qtserver.cpp \


HEADERS += \
	$$vDIR/vfe.h \
	$$vDIR/vfesession.h \
	$$vDIR/vfepovms.h \
	$$vqDIR/vfeplatform.h \
	$$vqDIR/qtvfe.h \
	$$vqDIR/qtoptions.h \
	$$vqDIR/qtgraphics.h \


#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

