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

QMAKE_CXXFLAGS -= -Wdate-time -Wsign-conversion

INCLUDEPATH += "../../vfe"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
	debugger/debuggerconsole.cpp \
	debugger/debuggerman.cpp \
	debugger/debuggerpanel.cpp \
	debugger/symboltreeitem.cpp \
	debugger/symboltreemodel.cpp \
	debugger/symboltreeview.cpp \
    dock/consoledock.cpp \
    dock/dockman.cpp \
    dock/dropdown.cpp \
    dock/filterdialog.cpp \
    dock/maintoolbar.cpp \
    dock/resourcedock.cpp \
    dock/renderdock.cpp \
    dock/statusbar.cpp \
    dock/searchman.cpp \
    editor/bookmarkman.cpp \
    editor/codeeditor.cpp \
    editor/imagedisplayer.cpp \
    editor/highlighter.cpp \
    editor/coloreditor.cpp \
    editor/povcolor.cpp \
    editor/colormapeditor.cpp \
    findman.cpp \
    insertmenuman.cpp \
    helpman.cpp \
    preferences.cpp \
    vfeclient.cpp \
    workspace.cpp

HEADERS += \
	debugger/debuggerconsole.h \
	debugger/debuggerman.h \
	debugger/debuggerpanel.h \
	debugger/symboltreeitem.h \
	debugger/symboltreemodel.h \
	debugger/symboltreeview.h \
    dock/consoledock.h \
    dock/dockman.h \
    dock/dropdown.h \
    dock/filterdialog.h \
    dock/maintoolbar.h \
    dock/renderdock.h \
    dock/resourcedock.h \
    dock/statusbar.h \
    dock/searchman.h \
    editor/bookmarkman.h \
    editor/codeeditor.h \
    editor/coloreditor.h \
    editor/colormapeditor.h \
    editor/highlighter.h \
    editor/imagedisplayer.h \
    editor/povcolor.h \
    findman.h \
    helpman.h \
    insertmenuman.h \
    mainwindow.h \
    preferences.h \
    preferencedata.h \
    qtpovrayversion.h \
    vfeclient.h \
    workspace.h 

FORMS += \
        mainwindow.ui

RESOURCES += \
    qtpov.qrc

RC_FILE = qtpov.rc

win32 {
    INCLUDEPATH += "../../libraries/boost"
    #DEFINES += BUILDING_AMD64
    DEFINES += _WINDOWS
    DEFINES += BOOST_ALL_NO_LIB
#    DEFINES += DONT_SHOW_IMAGE_LIB_VERSIONS
#    DEFINES += OPENEXR_MISSING
    POVLIBDIR = release
    CONFIG(debug, debug|release): POVLIBDIR = debug
}

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

unix: LIBS += -ljpeg
win32 {
    PRE_TARGETDEPS += $$OUT_PWD/../libraries/jpeg/$$POVLIBDIR/jpeg.lib
    LIBS += $$OUT_PWD/../libraries/jpeg/$$POVLIBDIR/jpeg.lib
}

#unix|win32: LIBS += -lpng
unix: LIBS += -lpng
win32 {
    PRE_TARGETDEPS += $$OUT_PWD/../libraries/png/$$POVLIBDIR/png.lib
    LIBS += $$OUT_PWD/../libraries/png/$$POVLIBDIR/png.lib
}

unix: LIBS += -lz
win32 {
    PRE_TARGETDEPS += $$OUT_PWD/../libraries/zlib/$$POVLIBDIR/zlib.lib
    LIBS += $$OUT_PWD/../libraries/zlib/$$POVLIBDIR/zlib.lib
}

unix: LIBS += -lboost_system -lboost_thread
#win32: CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/boost_thread/debug/boost_thread.lib
win32: LIBS += -L$$OUT_PWD/../libraries/boost_thread/$$POVLIBDIR
win32: LIBS += -L$$OUT_PWD/../libraries/boost_date_time/$$POVLIBDIR
win32: LIBS += -L$$OUT_PWD/../libraries/boost_system/$$POVLIBDIR
win32: LIBS += -L$$OUT_PWD/../libraries/boost_chrono/$$POVLIBDIR

#unix|win32: LIBS += -ltiff
unix: LIBS += -ltiff

#unix|win32: LIBS += -lIlmThread -lIlmImf -lHalf -lIex
unix: LIBS += -lIlmThread -lIlmImf -lHalf -lIex

#unix|win32: LIBS += -lboost_date_time
unix: LIBS += -lboost_date_time

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

qpfonts.path = /usr/share/fonts/truetype/SourceCodePro
qpfonts.files = ../../qt/install/fonts/SourceCodePro/*
INSTALLS += qpfonts

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
