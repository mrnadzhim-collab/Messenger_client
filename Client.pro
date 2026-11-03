QT       += core gui network

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
    messagewidget.cpp \
    netclient.cpp

HEADERS += \
    avatar.h \
    chatitemwidget.h \
    loginwindow.h \
    mainwindow.h \
    messagewidget.h \
    messegedata.h \
    netclient.h \
    protocol.h \
    types.h

FORMS += \
    loginwindow.ui \
    mainwindow.ui
