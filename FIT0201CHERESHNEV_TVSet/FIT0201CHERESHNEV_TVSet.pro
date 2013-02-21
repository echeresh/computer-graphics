#-------------------------------------------------
#
# Project created by QtCreator 2013-02-14T13:32:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201CHERESHNEV_TVSet
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    renderthread.cpp \
    converserec.cpp \
    graphicsscene.cpp \
    graphicshelper.cpp

HEADERS  += mainwindow.h \
    renderthread.h \
    converserec.h \
    graphicsscene.h \
    graphicshelper.h

FORMS    += mainwindow.ui

RESOURCES += \
    tvset.qrc
