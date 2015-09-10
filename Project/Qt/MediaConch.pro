#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

!macx:TARGET = mediaconch-gui
macx:TARGET = MediaConch
TEMPLATE = app

CONFIG += qt release
CONFIG += no_keywords

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/Core.cpp \
                    ../../Source/Common/Schema.cpp \
                    ../../Source/Common/Schematron.cpp \
                    ../../Source/Common/Policies.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/WebPage.cpp \
                    ../../Source/GUI/Qt/WebView.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/policiestree.cpp \
                    ../../Source/GUI/Qt/policiesmenu.cpp \
                    ../../Source/GUI/Qt/policymenu.cpp \
                    ../../Source/GUI/Qt/groupofrules.cpp \
                    ../../Source/GUI/Qt/rulemenu.cpp \
                    ../../Source/GUI/Qt/ruleedit.cpp

HEADERS          += ../../Source/Common/Core.h \
                    ../../Source/Common/Schema.h \
                    ../../Source/Common/Schematron.h \
                    ../../Source/GUI/Qt/WebPage.h \
                    ../../Source/GUI/Qt/WebView.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/policiestree.h \
                    ../../Source/GUI/Qt/policiesmenu.h \
                    ../../Source/GUI/Qt/policymenu.h \
                    ../../Source/GUI/Qt/groupofrules.h \
                    ../../Source/GUI/Qt/rulemenu.h \
                    ../../Source/GUI/Qt/ruleedit.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui \
                    ../../Source/GUI/Qt/policiestree.ui \
                    ../../Source/GUI/Qt/policiesmenu.ui \
                    ../../Source/GUI/Qt/policymenu.ui \
                    ../../Source/GUI/Qt/groupofrules.ui \
                    ../../Source/GUI/Qt/rulemenu.ui \
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

contains(DEFINES, "MEDIAINFO_LIBCURL_YES") {
exists(../../../curl/libcurl.pc) {
INCLUDEPATH      += ../../../curl/include
LIBS             += ../../../curl/libcurl.a
message("custom curl         : yes")
}
else {
CONFIG           += link_pkgconfig
PKGCONFIG        += libcurl
}
}

RESOURCES        += ../../Source/Resource/Resources.qrc

MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated

macx:ICON = ../../Source/Resource/Image/MediaConch.icns
macx:QMAKE_LFLAGS += -framework CoreFoundation

target.path = /usr/bin
INSTALLS += target
