#-------------------------------------------------
#
# Project created by QtCreator 2016-06-19T22:05:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DSP_Synth
TEMPLATE = app

SOURCES += main.cpp\
    display.cpp \
    jackclient.cpp \
    ../qcustomplot/qcustomplot.cpp

HEADERS  += display.hpp \
    jackclient.hpp \
    ../qcustomplot/qcustomplot.h

#INCLUDEPATH += /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/targetNFS/usr/share/ti/cgt-c6x/include \
#    /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/targetNFS/usr/share/ti/opencl \
#    /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/targetNFS/usr/include

LIBS += -ljack ../lib-shared/libaudio-x15.so -lboost_iostreams -lboost_system -lboost_filesystem #-lOpenCL -locl_util #-l$$QCPLIB

FORMS += mainwindow.ui

CONFIG += c++11

