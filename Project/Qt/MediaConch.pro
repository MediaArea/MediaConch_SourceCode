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
                    ../../Source/Common/Xslt.cpp \
                    ../../Source/Common/Policies.cpp \
                    ../../Source/Common/Policy.cpp \
                    ../../Source/Common/SchematronPolicy.cpp \
                    ../../Source/Common/XsltPolicy.cpp \
                    ../../Source/Common/JS_Tree.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/WebPage.cpp \
                    ../../Source/GUI/Qt/WebView.cpp \
                    ../../Source/GUI/Qt/helpwindow.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/menumainwindow.cpp \
                    ../../Source/GUI/Qt/displaywindow.cpp \
                    ../../Source/GUI/Qt/checkerwindow.cpp \
                    ../../Source/GUI/Qt/policieswindow.cpp \
                    ../../Source/GUI/Qt/policywindow.cpp \
                    ../../Source/GUI/Qt/schematronwindow.cpp \
                    ../../Source/GUI/Qt/xsltwindow.cpp \
                    ../../Source/GUI/Qt/policiestree.cpp \
                    ../../Source/GUI/Qt/policiesmenu.cpp \
                    ../../Source/GUI/Qt/policymenu.cpp \
                    ../../Source/GUI/Qt/groupofrules.cpp \
                    ../../Source/GUI/Qt/rulemenu.cpp \
                    ../../Source/GUI/Qt/ruleedit.cpp \
                    ../../Source/GUI/Qt/xsltpolicymenu.cpp \
                    ../../Source/GUI/Qt/xsltruleedit.cpp \
                    ../../Source/GUI/Qt/displaymenu.cpp

HEADERS          += ../../Source/Common/Core.h \
                    ../../Source/Common/Schema.h \
                    ../../Source/Common/Schematron.h \
                    ../../Source/Common/Xslt.h \
                    ../../Source/Common/JS_Tree.h \
                    ../../Source/Common/Policies.h \
                    ../../Source/Common/Policy.h \
                    ../../Source/Common/SchematronPolicy.h \
                    ../../Source/Common/XsltPolicy.h \
                    ../../Source/GUI/Qt/WebPage.h \
                    ../../Source/GUI/Qt/WebView.h \
                    ../../Source/GUI/Qt/helpwindow.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/menumainwindow.h \
                    ../../Source/GUI/Qt/checkerwindow.h \
                    ../../Source/GUI/Qt/displaywindow.h \
                    ../../Source/GUI/Qt/policieswindow.h \
                    ../../Source/GUI/Qt/policywindow.h \
                    ../../Source/GUI/Qt/schematronwindow.h \
                    ../../Source/GUI/Qt/xsltwindow.h \
                    ../../Source/GUI/Qt/policiestree.h \
                    ../../Source/GUI/Qt/policiesmenu.h \
                    ../../Source/GUI/Qt/policymenu.h \
                    ../../Source/GUI/Qt/groupofrules.h \
                    ../../Source/GUI/Qt/rulemenu.h \
                    ../../Source/GUI/Qt/ruleedit.h \
                    ../../Source/GUI/Qt/xsltpolicymenu.h \
                    ../../Source/GUI/Qt/xsltruleedit.h \
                    ../../Source/GUI/Qt/displaymenu.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui \
                    ../../Source/GUI/Qt/policiestree.ui \
                    ../../Source/GUI/Qt/policiesmenu.ui \
                    ../../Source/GUI/Qt/policymenu.ui \
                    ../../Source/GUI/Qt/groupofrules.ui \
                    ../../Source/GUI/Qt/rulemenu.ui \
                    ../../Source/GUI/Qt/ruleedit.ui \
                    ../../Source/GUI/Qt/xsltpolicymenu.ui \
                    ../../Source/GUI/Qt/xsltruleedit.ui \
                    ../../Source/GUI/Qt/displaymenu.ui

INCLUDEPATH      += ../../Source

exists(../../../MediaInfoLib/Project/GNU/Library/.libs/libmediainfo.a) {
INCLUDEPATH      += ../../../MediaInfoLib/Source
LIBS             += $$system(../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS_Static)
message("custom libmediainfo: yes (static)")
}
else {
exists(../../../MediaInfoLib/Project/GNU/Library/.libs/libmediainfo.so) {
INCLUDEPATH      += ../../../MediaInfoLib/Source
LIBS             += $$system(../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS)
message("custom libmediainfo: yes (shared)")
}
else {
#CONFIG           += link_pkgconfig
#PKGCONFIG        += libmediainfo
#LIBS             += $(pkg-config --libs libmediainfo)
LIBS             += $$system(pkg-config --libs libmediainfo)
}
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
exists(../../../libxslt/libxslt/.libs/libxslt.a) {
INCLUDEPATH      += ../../../libxslt/libxslt
LIBS             += ../../../libxslt/libxslt/.libs/libxslt.a
message("custom libxslt      : yes")
}
else {
INCLUDEPATH      += /usr/include/libxslt
LIBS             += -lxslt
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
