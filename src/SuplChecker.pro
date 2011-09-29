#-------------------------------------------------
#
# Project created by QtCreator 2010-11-28T18:32:00
#
#-------------------------------------------------

QT       += core gui webkit network

DESTDIR = ../bin
OBJECTS_DIR = ../build
MOC_DIR = ../build
RCC_DIR = ../build
UI_DIR = ../build

TARGET = suplchecker

TEMPLATE = app

SOURCES = main.cpp\
    parser.cpp \
    nastaveni.cpp \
    globalsettings.cpp \
    qtwin.cpp \
    debuglogger.cpp \
    aboutdialog.cpp \
    globalfunctions.cpp \
    serversdialog.cpp \
    suplchecker.cpp \
    updatechecker.cpp

HEADERS  += \
    parser.h \
    nastaveni.h \
    globalsettings.h \
    qtwin.h \
    debuglogger.h \
    aboutdialog.h \
    globalfunctions.h \
    serversdialog.h \
    suplchecker.h \
    updatechecker.h

FORMS    += mainwindow.ui \
    nastaveni.ui \
    debuglogger.ui \
    aboutdialog.ui \
    erroroverlay.ui \
    serversdialog.ui

RESOURCES += icons.qrc \
    html.qrc

OTHER_FILES += \
    appicon.rc \

win32:RC_FILE = appicon.rc

























