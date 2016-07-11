#-------------------------------------------------
#
# Project created by QtCreator 2016-03-29T10:50:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT-Audio
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    page.cpp

HEADERS  += mainwindow.h \
    page.h \
    positiontoint.h

FORMS    +=

RESOURCES += \
    pic.qrc
