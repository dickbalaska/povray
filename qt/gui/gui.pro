#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T04:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpovray
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#CONFIG(debug) {
#	message(Build debug version)
#	DEFINES += _DEBUG
#}
#release {		# DIK: Includes this define in debug builds. ??
#	DEFINES += _NDEBUG
#}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
    vfeclient.cpp

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
    version.h \
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
    vfeclient.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libpovray/release/ -lpovray
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libpovray/debug/ -lpovray
else:unix: LIBS += -L$$OUT_PWD/../libpovray/ -lpovray

INCLUDEPATH += $$PWD/../libpovray
DEPENDPATH += $$PWD/../libpovray

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/release/libpovray.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/debug/libpovray.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/release/povray.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libpovray/debug/povray.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libpovray/libpovray.a

unix|win32: LIBS += -lz

unix|win32: LIBS += -lboost_system

unix|win32: LIBS += -lboost_thread

unix|win32: LIBS += -ljpeg

unix|win32: LIBS += -lpng

unix|win32: LIBS += -ltiff

unix|win32: LIBS += -lboost_date_time
