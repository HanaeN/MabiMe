#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T14:05:54
#
#-------------------------------------------------

QT       += core gui opengl openglextensions

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MabiMe
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
    mainwindow.cpp \
    PackReader/mabipackreader.cpp \
    PackReader/mabimt.cpp \
    mabimeglwidget.cpp \
    PackReader/pmgreader.cpp \
    PackReader/frmreader.cpp \

HEADERS  += mainwindow.h \
    PackReader/mabipackreader.h \
    PackReader/mabimt.h \
    mabimeglwidget.h \
    PackReader/pmgreader.h \
    PackReader/frmreader.h \

FORMS    += mainwindow.ui

win32:LIBS += -lopengl32
unix:LIBS += -lGL
