#-------------------------------------------------
#
# Project created by QtCreator 2016-07-21T12:16:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = test-libaudio-x15
TEMPLATE = app

LIBS += -ljack ../lib-shared/libaudio-x15.so

SOURCES += main.cpp\
        mainwindow.cpp\
    ../qcustomplot/qcustomplot.cpp

HEADERS  += mainwindow.h \
    ../qcustomplot/qcustomplot.h

FORMS    += mainwindow.ui
