#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T20:25:15
#
#-------------------------------------------------

QT       += core gui serialport opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = serialDebugAssistant
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qcustomplot.cpp \
        serialscope.cpp \
        viewer3d.cpp \
        kfile.cpp

HEADERS += mainwindow.h \
        qcustomplot.h \
        viewer3d.h \
        serialscope.h \
        kfile.h

FORMS   += mainwindow.ui \
        serialscope.ui \
        viewer3d.ui
