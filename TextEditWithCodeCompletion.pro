-------------------------------------------------

QT       += core gui
CONFIG += console

include(deployment.pri)
qtcAddDeployment()

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeCompletion
TEMPLATE = app

SOURCES += main.cpp \
    CodeCompletion.cpp

HEADERS  += \
    CodeCompletion.h

DEFINES += NDEBUG
DEFINES += _GNU_SOURCE
DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += "/usr/local/opt/llvm/include"
INCLUDEPATH += "/Users/bell/Documents/community-sdk-master/include"

LIBS += -L /usr/local/opt/llvm/lib -lclang
LIBS += -framework ApplicationServices
LIBS += -framework OpenGL
LIBS += -framework GLUT
