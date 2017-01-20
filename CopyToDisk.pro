#-------------------------------------------------
#
# Project created by QtCreator 2017-01-13T14:33:05
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CopyToDisk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    copyingdialog.cpp \
    copythread.cpp \
    copyprogress.cpp

HEADERS  += mainwindow.h \
    copyingdialog.h \
    copythread.h \
    copyprogress.h

FORMS    += mainwindow.ui \
    copyingdialog.ui \
    copyprogress.ui
