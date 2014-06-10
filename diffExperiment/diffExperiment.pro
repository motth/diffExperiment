#-------------------------------------------------
#
# Project created by QtCreator 2014-05-30T14:12:02
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = diffExperiment
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += qt warn_on thread debug_and_release depend_includepath

TEMPLATE = app


SOURCES += main.cpp \
    gitrepository.cpp \
    diff.cpp \
    astnode.cpp \
    astnodediff.cpp \
    astdiff.cpp

QMAKE_CXXFLAGS += -std=c++11 -pedantic-errors -Werror -Wextra -O2 -g -fno-omit-frame-pointer

unix:LIBS += -lgit2

HEADERS += \
    gitrepository.h \
    diff.h \
    astnode.h \
    astnodediff.h \
    astdiff.h

INCLUDEPATH += /usr/local/lib/libgit2-0.20.0/include/
