#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T14:05:54
#
#-------------------------------------------------

QT       += core gui opengl openglextensions xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MabiMe
TEMPLATE = app
CONFIG += c++11

RC_FILE = mabime.rc

SOURCES += main.cpp\
    mainwindow.cpp \
    PackReader/mabipackreader.cpp \
    PackReader/mabimt.cpp \
    mabimeglwidget.cpp \
    PackReader/pmgreader.cpp \
    PackReader/frmreader.cpp \
    PackReader/packmanager.cpp \
    bone.cpp \
    model.cpp \
    settingswindow.cpp \
    mabimelayerdelegate.cpp \
    Parsers/localemaphelper.cpp \
    Parsers/xmlparser.cpp \
    Parsers/characterstyleparser.cpp \
    Parsers/colourparser.cpp \
    mabimecolourdelegate.cpp \
    aboutwindow.cpp \
    Parsers/faceemotionparser.cpp

HEADERS  += mainwindow.h \
    PackReader/mabipackreader.h \
    PackReader/mabimt.h \
    mabimeglwidget.h \
    PackReader/pmgreader.h \
    PackReader/frmreader.h \
    PackReader/packmanager.h \
    bone.h \
    model.h \
    settingswindow.h \
    mabimelayerdelegate.h \
    Parsers/localemaphelper.h \
    Parsers/xmlparser.h \
    Parsers/characterstyleparser.h \
    Parsers/colourparser.h \
    mabimecolourdelegate.h \
    aboutwindow.h \
    Parsers/faceemotionparser.h

FORMS    += mainwindow.ui \
    settingswindow.ui \
    aboutwindow.ui

win32:LIBS += -lopengl32
unix:LIBS += -lGL

RESOURCES += \
    images.qrc
