#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T14:26:06
#
#-------------------------------------------------
QT       += core network
QT       -= gui

TARGET = UdpMulticastClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp\
        udpmulticastclient.cpp \
    receivefileserver.cpp \
    receivefilethread.cpp

HEADERS  += udpmulticastclient.h \
    CommonSetting.h \
    receivefileserver.h \
    receivefilethread.h

DESTDIR=bin
MOC_DIR=temp/moc
RCC_DIR=temp/rcc
UI_DIR=temp/ui
OBJECTS_DIR=temp/obj
