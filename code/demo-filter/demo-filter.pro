#
#  Demo Filter
#

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = demo-filter
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    ../qcustomplot/qcustomplot.cpp \
    audioprocessor.cpp

HEADERS  += mainwindow.h \
    ../qcustomplot/qcustomplot.h \
    audioprocessor.h

FORMS    += mainwindow.ui

LIBS += ../libdsp-x15/libdsp-x15.so -lOpenCL -locl_util -ljack

QMAKE_CXXFLAGS += -std=c++11

