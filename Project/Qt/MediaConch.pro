#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaConch
TEMPLATE = app

CONFIG += qt release
CONFIG += no_keywords

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/Core.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp

HEADERS          += ../../Source/Common/Core.h \
                    ../../Source/GUI/Qt/mainwindow.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui

INCLUDEPATH      += ../../Source \
                    ../../../MediaInfoLib/Source \
                    ../../../ZenLib/Source

LIBS             += ../../../MediaInfoLib/Project/GNU/Library/.libs/libmediainfo.a \
                    ../../../ZenLib/Project/GNU/Library/.libs/libzen.a \
                    -lz
!macx:LIBS       += -ldl -lrt

RESOURCES        += ../../Source/Resource/Resources.qrc

MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated

macx:ICON = ../../Source/Resource/Image/MediaConch.icns
macx:QMAKE_LFLAGS += -framework CoreFoundation

target.path = /usr/bin
INSTALLS += target
