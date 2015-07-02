#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

!macx:TARGET = mediaconch-gui
macx:TARGET = MediaConch
TEMPLATE = app

CONFIG += qt release
CONFIG += no_keywords

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/Core.cpp \
                    ../../Source/Common/Schematron.cpp \
                    ../../Source/Common/Policies.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/policiesmenu.cpp \
                    ../../Source/GUI/Qt/ruleedit.cpp

HEADERS          += ../../Source/Common/Core.h \
                    ../../Source/Common/Schematron.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/policiesmenu.h \
                    ../../Source/GUI/Qt/ruleedit.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui \
                    ../../Source/GUI/Qt/policiesmenu.ui \
                    ../../Source/GUI/Qt/ruleedit.ui

INCLUDEPATH      += ../../Source

exists(../../../MediaInfoLib/Project/GNU/Library/.libs/libmediainfo.a) {
INCLUDEPATH      += ../../../MediaInfoLib/Source
LIBS             += ../../../MediaInfoLib/Project/GNU/Library/.libs/libmediainfo.a
message("custom libmediainfo: yes")
}
else {
LIBS             += -lmediainfo
}
exists(../../../ZenLib/Project/GNU/Library/.libs/libzen.a) {
INCLUDEPATH      += ../../../ZenLib/Source
LIBS             += ../../../ZenLib/Project/GNU/Library/.libs/libzen.a
message("custom libzen      : yes")
}
else {
LIBS             += -lzen
}
exists(../../../libxml2/.libs/libxml2.a) {
INCLUDEPATH      += ../../../libxml2/include
LIBS             += ../../../libxml2/.libs/libxml2.a
message("custom libxml2      : yes")
}
else {
INCLUDEPATH      += /usr/include/libxml2
LIBS             += -lxml2
}
LIBS             += -lz
!macx:LIBS       += -ldl -lrt

RESOURCES        += ../../Source/Resource/Resources.qrc

MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated

macx:ICON = ../../Source/Resource/Image/MediaConch.icns
macx:QMAKE_LFLAGS += -framework CoreFoundation

target.path = /usr/bin
INSTALLS += target
