#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

WEB_MACHINE=

contains(USE_WEBKIT, yes|1) {
    WEB_MACHINE = webkit
}
contains(USE_WEBENGINE, yes|1) {
    WEB_MACHINE = webengine
}

isEmpty(WEB_MACHINE) {
    WEB_MACHINE = webengine
    lessThan(QT_MAJOR_VERSION, 5) {
        WEB_MACHINE = webkit
    }
    equals(QT_MAJOR_VERSION, 5) {
        lessThan(QT_MINOR_VERSION, 6) {
            WEB_MACHINE = webkit
        }
    }
}

!macx:TARGET = mediaconch-gui
macx:TARGET = MediaConch
TEMPLATE = app

CONFIG += qt release
CONFIG += no_keywords

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/MediaConchLib.cpp \
                    ../../Source/Common/Core.cpp \
                    ../../Source/Common/Schema.cpp \
                    ../../Source/Common/Schematron.cpp \
                    ../../Source/Common/Xslt.cpp \
                    ../../Source/Common/Policies.cpp \
                    ../../Source/Common/Policy.cpp \
                    ../../Source/Common/SchematronPolicy.cpp \
                    ../../Source/Common/XsltPolicy.cpp \
                    ../../Source/Common/JS_Tree.cpp \
                    ../../Source/Common/Database.cpp \
                    ../../Source/Common/NoDatabase.cpp \
                    ../../Source/Common/SQLLite.cpp \
                    ../../Source/Common/Json.cpp \
                    ../../Source/Common/Configuration.cpp \
                    ../../Source/Common/REST_API.cpp \
                    ../../Source/Common/Httpd.cpp \
                    ../../Source/Common/LibEventHttpd.cpp \
                    ../../Source/Common/Http.cpp \
                    ../../Source/Common/LibEventHttp.cpp \
                    ../../Source/Common/Scheduler.cpp \
                    ../../Source/Common/Queue.cpp \
                    ../../Source/Common/DaemonClient.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/helpwindow.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/menumainwindow.cpp \
                    ../../Source/GUI/Qt/displaywindow.cpp \
                    ../../Source/GUI/Qt/checkerwindow.cpp \
                    ../../Source/GUI/Qt/resultwindow.cpp \
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
                    ../../Source/GUI/Qt/displaymenu.cpp \
                    ../../Source/GUI/Qt/progressbar.cpp

HEADERS          += ../../Source/Common/MediaConchLib.h \
                    ../../Source/Common/Core.h \
                    ../../Source/Common/Schema.h \
                    ../../Source/Common/Schematron.h \
                    ../../Source/Common/Xslt.h \
                    ../../Source/Common/JS_Tree.h \
                    ../../Source/Common/Policies.h \
                    ../../Source/Common/Policy.h \
                    ../../Source/Common/SchematronPolicy.h \
                    ../../Source/Common/XsltPolicy.h \
                    ../../Source/Common/ImplementationReportXsl.h \
                    ../../Source/Common/ImplementationReportDisplayTextUnicodeXsl.h \
                    ../../Source/Common/ImplementationReportDisplayTextXsl.h \
                    ../../Source/Common/ImplementationReportDisplayHtmlXsl.h \
                    ../../Source/Common/ImplementationReportMatroskaSchema.h \
                    ../../Source/Common/Database.h \
                    ../../Source/Common/NoDatabase.h \
                    ../../Source/Common/SQLLite.h \
                    ../../Source/Common/Json.h \
                    ../../Source/Common/Container.h \
                    ../../Source/Common/NoContainer.h \
                    ../../Source/Common/Configuration.h \
                    ../../Source/Common/REST_API.h \
                    ../../Source/Common/Httpd.h \
                    ../../Source/Common/LibEventHttpd.h \
                    ../../Source/Common/Http.h \
                    ../../Source/Common/LibEventHttp.h \
                    ../../Source/Common/Scheduler.h \
                    ../../Source/Common/Queue.h \
                    ../../Source/Common/DaemonClient.h \
                    ../../Source/GUI/Qt/helpwindow.h \
                    ../../Source/GUI/Qt/WebPage.h \
                    ../../Source/GUI/Qt/WebView.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/menumainwindow.h \
                    ../../Source/GUI/Qt/checkerwindow.h \
                    ../../Source/GUI/Qt/resultwindow.h \
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
                    ../../Source/GUI/Qt/displaymenu.h \
                    ../../Source/GUI/Qt/progressbar.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui \
                    ../../Source/GUI/Qt/policiestree.ui \
                    ../../Source/GUI/Qt/policiesmenu.ui \
                    ../../Source/GUI/Qt/policymenu.ui \
                    ../../Source/GUI/Qt/groupofrules.ui \
                    ../../Source/GUI/Qt/rulemenu.ui \
                    ../../Source/GUI/Qt/ruleedit.ui \
                    ../../Source/GUI/Qt/xsltpolicymenu.ui \
                    ../../Source/GUI/Qt/xsltruleedit.ui \
                    ../../Source/GUI/Qt/displaymenu.ui \
                    ../../Source/GUI/Qt/progressbar.ui


equals(WEB_MACHINE, webengine) {
    QT += webenginewidgets webchannel
    SOURCES += ../../Source/GUI/Qt/WebEnginePage.cpp \
               ../../Source/GUI/Qt/WebEngineView.cpp
    HEADERS += ../../Source/GUI/Qt/WebEnginePage.h \
               ../../Source/GUI/Qt/WebEngineView.h
    QMAKE_CXXFLAGS   += -DWEB_MACHINE_ENGINE
} else {
    QT += webkit webkitwidgets
    SOURCES += ../../Source/GUI/Qt/WebKitPage.cpp \
               ../../Source/GUI/Qt/WebKitView.cpp
    HEADERS += ../../Source/GUI/Qt/WebKitPage.h \
               ../../Source/GUI/Qt/WebKitView.h
    QMAKE_CXXFLAGS   += -DWEB_MACHINE_KIT
}

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
    message("libzen      : custom")
}
else {
    LIBS             += -lzen
    message("libzen      : system")
}

exists(../../../libxml2/.libs/libxml2.a) {
    INCLUDEPATH      += ../../../libxml2/include
    LIBS             += ../../../libxml2/.libs/libxml2.a
    message("libxml2     : custom")
}
else {
    INCLUDEPATH      += /usr/include/libxml2
    LIBS             += -lxml2
    message("libxml2     : system")
}
exists(../../../libxslt/libxslt/.libs/libxslt.a) {
    INCLUDEPATH      += ../../../libxslt/libxslt
    INCLUDEPATH      += ../../../libxslt/libexslt
    LIBS             += ../../../libxslt/libxslt/.libs/libxslt.a
    LIBS             += ../../../libxslt/libexslt/.libs/libexslt.a
    message("libxslt     : custom")
}
else {
    INCLUDEPATH      += /usr/include/libxslt
    INCLUDEPATH      += /usr/include/libexslt
    LIBS             += -lxslt -lexslt
    message("libxslt     : system")
}


contains(NO_SQLITE, yes|1) {
    message("libsqlite3  : no")
} else {
exists(../../../sqlite/.libs/libsqlite3.a) {
    INCLUDEPATH      += ../../../sqlite
    LIBS             += ../../../sqlite/.libs/libsqlite3.a
    QMAKE_CXXFLAGS   += -DHAVE_SQLITE
    message("libsqlite3  : custom")
}
else {
    LIBS             += -lsqlite3
    QMAKE_CXXFLAGS   += -DHAVE_SQLITE
    message("libsqlite3  : system")
    }
}

contains(NO_JANSSON, yes|1) {
    message("libjansson  : no")
} else {
exists(../../../jansson/src/.libs/libjansson.a) {
    INCLUDEPATH      += ../../../jansson/src
    LIBS             += ../../../jansson/src/.libs/libjansson.a
    QMAKE_CXXFLAGS   += -DHAVE_JANSSON
    message("libjansson  : custom")
}
else {
    LIBS             += -ljansson
    QMAKE_CXXFLAGS   += -DHAVE_JANSSON
    message("libjansson  : system")
    }
}

contains(NO_LIBEVENT, yes|1) {
    message("libevent    : no")
} else {
exists(../../../libevent/.libs/libevent.a) {
    INCLUDEPATH      += ../../../libevent/include
    LIBS             += ../../../libevent/.libs/libevent.a
    QMAKE_CXXFLAGS   += -DHAVE_LIBEVENT
    message("libevent    : custom")
}
else {
    LIBS             += -levent
    QMAKE_CXXFLAGS   += -DHAVE_LIBEVENT
    message("libevent    : system")
}
}

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
LIBS             += -lz
!macx:LIBS       += -ldl -lrt

RESOURCES        += ../../Source/Resource/Resources.qrc

MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated
UI_DIR           =  _Automated
RCC_DIR          =  _Automated

macx:ICON = ../../Source/Resource/Image/MediaConch.icns
macx:QMAKE_LFLAGS += -framework CoreFoundation

target.path = /usr/bin
INSTALLS += target
