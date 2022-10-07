#-------------------------------------------------
#
# Project created by QtCreator 2022-09-01T16:46:23
#
#-------------------------------------------------

QT       += core gui serialbus serialport widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UI_display1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        ui_display.cpp \
    DataProcessing.cpp \
    src/modbus-data.c \
    src/modbus-rtu.c \
    src/modbus-tcp.c \
    src/modbus.c \
    qcustomplot.cpp

HEADERS  += ui_display.h \
    DataProcessing.h \
    src/modbus-private.h \
    src/modbus-rtu-private.h \
    src/modbus-rtu.h \
    src/modbus-tcp-private.h \
    src/modbus-tcp.h \
    src/modbus-version.h \
    src/modbus.h \
    config.h \
    qcustomplot.h

INCLUDEPATH += $$PWD/src #包含目录

#LIBS += -lws2_32 # 这个是libmodbus配置win32环境必须的

FORMS    += ui_display.ui

DISTFILES += \
    src/win32/modbus.dll.manifest.in
