#-------------------------------------------------
#
# Project created by QtCreator 2010-11-28T18:32:00
#
#-------------------------------------------------

QT       += core gui webkit sql network

TARGET = suplchecker

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

RESOURCES += icons.qrc

OTHER_FILES += \
    appicon.rc \

win32:RC_FILE = appicon.rc
