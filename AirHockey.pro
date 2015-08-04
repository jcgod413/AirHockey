#-------------------------------------------------
#
# Project created by QtCreator 2015-07-12T18:30:18
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirHockey
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    imageglview.cpp \
    capturethread.cpp \
    imageprocessing.cpp

HEADERS  += widget.h \
    imageglview.h \
    capturethread.h \
    ahr.h \
    imageprocessing.h

FORMS    += widget.ui
