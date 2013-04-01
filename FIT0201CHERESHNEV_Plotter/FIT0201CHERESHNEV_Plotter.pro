#-------------------------------------------------
#
# Project created by QtCreator 2013-03-03T18:52:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201CHERESHNEV_Plotter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cassiniwidget.cpp \
    cassini.cpp \
    curvewidget.cpp \
    curve.cpp \
    drawer.cpp \
    curvedrawer.cpp \
    axisdrawer.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    cassiniwidget.h \
    cassini.h \
    drawer.h \
    utils.h \
    curvewidget.h \
    curve.h \
    curvedrawer.h \
    axisdrawer.h

FORMS    += mainwindow.ui
