#-------------------------------------------------
#
# Project created by QtCreator 2013-03-30T11:50:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201CHERESHNEV_Teapot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    widget.cpp \
    utils.cpp \
    beziergrid.cpp \
    painter.cpp \
    beziersurfacemodel.cpp \
    matrix.cpp

HEADERS  += mainwindow.h \
    widget.h \
    utils.h \
    beziergrid.h \
    matrix.h \
    painter.h \
    beziersurfacemodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
