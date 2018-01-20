#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T04:10:14
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpov
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
    wsclient.cpp \
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
    editor/colormapeditor.cpp

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
    wsclient.h \
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
    editor/colormapeditor.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    qtpov.qrc

RC_FILE = qtpov.rc
