#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T04:10:14
#
#-------------------------------------------------

# Used by make install for directory location
QPVERSION=3.8

QT       += core gui widgets


CONFIG += c++11

TARGET = qtpovray
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(debug, debug|release) {
	message(Build GUI debug version)
	DEFINES += _DEBUG
}
#release {		# DIK: Includes this define in debug builds. ??
#	DEFINES += _NDEBUG
#}


unix:isEmpty(PREFIX) {
 PREFIX = /usr/local
}

DESTDIR = ../../usr/bin
#DESTDIR = $$PREFIX

QMAKE_CXXFLAGS -= -Wdate-time

INCLUDEPATH += "../../vfe"

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dock/resourcedock.cpp \
    dock/dockman.cpp \
    dock/consoledock.cpp \
    dock/maintoolbar.cpp \
    workspace.cpp \
    editor/codeeditor.cpp \
    preferences.cpp \
    editor/highlighter.cpp \
    dock/renderdock.cpp \
    dock/statusbar.cpp \
    dock/filterdialog.cpp \
    dock/searchman.cpp \
    editor/bookmarkman.cpp \
    findman.cpp \
    insertmenuman.cpp \
    dock/dropdown.cpp \
    editor/coloreditor.cpp \
    editor/povcolor.cpp \
    editor/colormapeditor.cpp \
    vfeclient.cpp \
    helpman.cpp \
    editor/imagedisplayer.cpp

HEADERS += \
        mainwindow.h \
    dock/resourcedock.h \
    dock/dockman.h \
    dock/consoledock.h \
    dock/maintoolbar.h \
    workspace.h \
    editor/codeeditor.h \
    preferences.h \
    preferencedata.h \
    editor/highlighter.h \
    dock/renderdock.h \
    dock/statusbar.h \
    dock/filterdialog.h \
    dock/searchman.h \
    editor/bookmarkman.h \
    findman.h \
    insertmenuman.h \
    dock/dropdown.h \
    editor/coloreditor.h \
    editor/povcolor.h \
    editor/colormapeditor.h \
    vfeclient.h \
    qtpovrayversion.h \
    helpman.h \
    editor/imagedisplayer.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    qtpov.qrc

RC_FILE = qtpov.rc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../vfe/release/ -lvfe
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../vfe/debug/ -lvfe
else:unix: LIBS += -L$$OUT_PWD/../vfe/ -lvfe

INCLUDEPATH += $$PWD/../vfe
DEPENDPATH += $$PWD/../vfe

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../vfe/release/libvfe.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../vfe/debug/libvfe.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../vfe/release/vfe.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../vfe/debug/vfe.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../vfe/libvfe.a


INCLUDEPATH += $$PWD/../libpovray
DEPENDPATH += $$PWD/../libpovray

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/release/libpovray.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/debug/libpovray.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/release/povray.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/debug/povray.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpovray/libpovray.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpovray/release/ -lpovray
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpovray/debug/ -lpovray
else:unix: LIBS += -L$$OUT_PWD/../libpovray/ -lpovray

INCLUDEPATH += $$PWD/../platform
DEPENDPATH += $$PWD/../platform

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/release/libplatform.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/debug/libplatform.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/release/platform.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/debug/platform.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../platform/libplatform.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../platform/release/ -lplatform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../platform/debug/ -lplatform
else:unix: LIBS += -L$$OUT_PWD/../platform/ -lplatform

unix|win32: LIBS += -lz

unix|win32: LIBS += -lboost_system -lboost_thread

unix|win32: LIBS += -ljpeg

unix|win32: LIBS += -lpng

unix|win32: LIBS += -ltiff

#unix|win32: LIBS += -lIlmThread -lIlmImf -lHalf -lSDL -lIex
unix|win32: LIBS += -lIlmThread -lIlmImf -lHalf -lIex

unix|win32: LIBS += -lboost_date_time

###############################################################################
# make install options.
# I wanted these in their own .pro but I couldnt get qmake to cooperate.
#

#
# In debian package qtpovray
#

#TARGET.path = $$PREFIX/bin
#INSTALLS += TARGET

etc.path = /etc/qtpovray/$$QPVERSION
etc.files = ../../unix/povray.conf ../../distribution/ini/povray.ini
INSTALLS += etc

man.path = /usr/share/man/man1
man.files = ../../qt/install/qtpovray.1
INSTALLS += man

qpicons.path = /usr/share/icons/hicolor/48x48/apps
qpicons.files = ../../distribution/qt/icons/48x48/*
INSTALLS += qpicons

desktop.path = /usr/share/applications
desktop.files = ../../qt/install/qtpovray.desktop
INSTALLS += desktop

#
# In debian package qtpovray-includes
#
includes.path = /usr/share/qtpovray-$$QPVERSION
includes.files = ../../distribution/include
INSTALLS += includes

#
# In debian package qtpovray-extras
#
inserts.path = $$PREFIX/share/qtpovray-$$QPVERSION
inserts.files = "../../distribution/platform-specific/windows/Insert Menu"
INSTALLS += inserts

scenes.path = $$PREFIX/share/qtpovray-$$QPVERSION
scenes.files = ../../distribution/scenes
INSTALLS += scenes

help.path = $$PREFIX/share/qtpovray-$$QPVERSION
help.files = ../../doc/html
INSTALLS += help

qtphelp.path = $$PREFIX/share/qtpovray-$$QPVERSION
qtphelp.files = ../../distribution/qt/qtpovrayHelp
INSTALLS += qtphelp
