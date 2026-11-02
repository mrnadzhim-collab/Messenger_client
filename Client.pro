QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += console c++11

DESTDIR= /usr/local/bin

TARGET = client

TEMPLATE = app

SOURCES += \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    netclient.cpp

HEADERS += \
    loginwindow.h \
    mainwindow.h \
    netclient.h \
    protocol.h

FORMS += \
    loginwindow.ui \
    mainwindow.ui
