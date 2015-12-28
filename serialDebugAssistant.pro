#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T20:25:15
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = serialDebugAssistant
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    serialScope.cpp \
    cubeViewer.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    serialScope.h \
    cubeViewer.h

FORMS    += mainwindow.ui \
    serialscope.ui \
    cubeViewer.ui
