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
    ASTDiff.cpp \
    ASTNode.cpp \
    ASTNodeDiff.cpp \
    Diff.cpp \
    GitRepository.cpp

QMAKE_CXXFLAGS += -std=c++11 -pedantic-errors -Werror -Wextra -O2 -g -fno-omit-frame-pointer

unix:LIBS += -lgit2

HEADERS += \
    ASTDiff.h \
    ASTNode.h \
    ASTNodeDiff.h \
    Diff.h \
    GitRepository.h

INCLUDEPATH += /usr/local/lib/libgit2-0.20.0/include/
