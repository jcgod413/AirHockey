#-------------------------------------------------
#
# Project created by QtCreator 2015-07-12T18:30:18
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets opengl serialport

greaterThan(QT_MAJOR_VERSION, 4)    {
    QT += widgets
}
else    {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = AirHockey
TEMPLATE = app

SOURCES += main.cpp\
        widget.cpp \
    imageglview.cpp \
    capturethread.cpp \
    imageprocessing.cpp \
    ball.cpp \
    bluetoothmaster.cpp

HEADERS  += widget.h \
    imageglview.h \
    capturethread.h \
    ahr.h \
    imageprocessing.h \
    ball.h \
    bluetoothmaster.h

FORMS    += widget.ui
