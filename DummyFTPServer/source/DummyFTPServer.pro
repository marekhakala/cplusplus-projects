# -------------------------------------------------
# Project created by QtCreator 2010-05-28T10:32:57
# -------------------------------------------------
QT += network
QT -= gui
TARGET = ../build/DummyFTPServer
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    userlist.cpp \
    user.cpp \
    core.cpp \
    confsection.cpp \
    configuration.cpp \
    serverthread.cpp \
    ftpserver.cpp \
    workpath.cpp \
    ftpcommandlist.cpp \
    ftpcommandstor.cpp \
    ftpcommandretr.cpp \
    ftpcommand.cpp \
    fvoiwejf.cpp
HEADERS += userlist.h \
    user.h \
    passwordtype.h \
    mainpage.h \
    core.h \
    confsection.h \
    configuration.h \
    config.h \
    serverthread.h \
    ftpserver.h \
    workpath.h \
    ftpcommandlist.h \
    ftpcommandstor.h \
    ftpcommandretr.h \
    errorexception.h \
    ftpcommand.h \
    fvoiwejf.h
