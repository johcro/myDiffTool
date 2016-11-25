#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T10:33:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myDiffTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    diffdialog.cpp \
    resultsdata.cpp \
    exportdialog.cpp

HEADERS  += mainwindow.h \
    diffdialog.h \
    resultsdata.h \
    exportdialog.h

FORMS    += mainwindow.ui \
    diffdialog.ui \
    exportdialog.ui

QMAKE_LIBDIR += /usr/lib64
