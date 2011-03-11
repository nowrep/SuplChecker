#-------------------------------------------------
#
# Project created by QtCreator 2010-11-28T18:32:00
#
#-------------------------------------------------

QT       += core gui webkit sql network

TARGET = suplchecker

VERSION = 0.6

TEMPLATE = app

SOURCES = main.cpp\
        mainwindow.cpp \
    parser.cpp \
    nastaveni.cpp

HEADERS  += mainwindow.h \
    parser.h \
    nastaveni.h

FORMS    += mainwindow.ui \
    nastaveni.ui \
    mainwindowS60.ui

RESOURCES += icons.qrc

OTHER_FILES += \
    appicon.rc \

win32:RC_FILE = appicon.rc

symbian|simulator: {
    ICON = icons/icon.svg
    TARGET.CAPABILITY += NetworkServices
    TARGET.UID3 = 0xE334B602
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    TARGET = SuplChecker
    QT-=sql

    SOURCES-=nastaveni.cpp
    HEADERS-=nastaveni.h
    FORMS -= nastaveni.ui
}
