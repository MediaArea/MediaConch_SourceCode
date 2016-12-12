#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

contains(QT_CONFIG, no-gui) {
    error("qt module gui not found")
}

QT     += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    !qtHaveModule(widgets) {
        error("qt module widgets not found")
    }
    QT += widgets
}

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

unix:CONFIG += link_pkgconfig

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/MediaConchLib.cpp \
                    ../../Source/Common/Core.cpp \
                    ../../Source/Common/Schema.cpp \
                    ../../Source/Common/Xslt.cpp \
                    ../../Source/Common/Policies.cpp \
                    ../../Source/Common/Policy.cpp \
                    ../../Source/Common/XsltPolicy.cpp \
                    ../../Source/Common/UnknownPolicy.cpp \
                    ../../Source/Common/JS_Tree.cpp \
                    ../../Source/Common/Database.cpp \
                    ../../Source/Common/DatabaseReport.cpp \
                    ../../Source/Common/NoDatabaseReport.cpp \
                    ../../Source/Common/SQLLite.cpp \
                    ../../Source/Common/SQLLiteReport.cpp \
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
                    ../../Source/Common/PluginsManager.cpp \
                    ../../Source/Common/PluginsConfig.cpp \
                    ../../Source/Common/Plugin.cpp \
                    ../../Source/Common/VeraPDF.cpp \
                    ../../Source/Common/DpfManager.cpp \
                    ../../Source/Common/FFmpeg.cpp \
                    ../../Source/Common/PluginFileLog.cpp \
                    ../../Source/Common/WatchFoldersManager.cpp \
                    ../../Source/Common/WatchFolder.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/helpwindow.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/settingswindow.cpp \
                    ../../Source/GUI/Qt/displaywindow.cpp \
                    ../../Source/GUI/Qt/checkerwindow.cpp \
                    ../../Source/GUI/Qt/resulttable.cpp \
                    ../../Source/GUI/Qt/policieswindow.cpp \
                    ../../Source/GUI/Qt/WebCommonPage.cpp \
                    ../../Source/GUI/Qt/progressbar.cpp \
                    ../../Source/GUI/Qt/workerfiles.cpp \
                    ../../Source/GUI/Qt/DatabaseUi.cpp \
                    ../../Source/GUI/Qt/NoDatabaseUi.cpp \
                    ../../Source/GUI/Qt/SQLLiteUi.cpp \
                    ../../Source/GUI/Qt/uisettings.cpp \
                    ../../Source/GUI/Qt/publicpolicieswindow.cpp

HEADERS          += ../../Source/Common/MediaConchLib.h \
                    ../../Source/Common/Core.h \
                    ../../Source/Common/Schema.h \
                    ../../Source/Common/Xslt.h \
                    ../../Source/Common/JS_Tree.h \
                    ../../Source/Common/Policies.h \
                    ../../Source/Common/Policy.h \
                    ../../Source/Common/XsltPolicy.h \
                    ../../Source/Common/UnknownPolicy.h \
                    ../../Source/Common/generated/ImplementationReportXsl.h \
                    ../../Source/Common/generated/ImplementationReportDisplayTextUnicodeXsl.h \
                    ../../Source/Common/generated/ImplementationReportDisplayTextXsl.h \
                    ../../Source/Common/generated/ImplementationReportDisplayHtmlXsl.h \
                    ../../Source/Common/generated/ImplementationReportMatroskaSchema.h \
                    ../../Source/Common/generated/MicroMediaTraceToMediaTraceXsl.h \
                    ../../Source/Common/generated/PolicyTransformXml.h \
                    ../../Source/Common/generated/GeneratedCSVVideos.h \
                    ../../Source/Common/Database.h \
                    ../../Source/Common/DatabaseReport.h \
                    ../../Source/Common/NoDatabaseReport.h \
                    ../../Source/Common/SQLLite.h \
                    ../../Source/Common/SQLLiteReport.h \
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
                    ../../Source/Common/FileRegistered.h \
                    ../../Source/Common/PluginsManager.h \
                    ../../Source/Common/PluginsConfig.h \
                    ../../Source/Common/Plugin.h \
                    ../../Source/Common/PluginFormat.h \
                    ../../Source/Common/VeraPDF.h \
                    ../../Source/Common/DpfManager.h \
                    ../../Source/Common/FFmpeg.h \
                    ../../Source/Common/PluginLog.h \
                    ../../Source/Common/PluginFileLog.h \
                    ../../Source/Common/WatchFoldersManager.h \
                    ../../Source/GUI/Qt/helpwindow.h \
                    ../../Source/GUI/Qt/WebPage.h \
                    ../../Source/GUI/Qt/WebCommonPage.h \
                    ../../Source/GUI/Qt/WebView.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/settingswindow.h \
                    ../../Source/GUI/Qt/checkerwindow.h \
                    ../../Source/GUI/Qt/resulttable.h \
                    ../../Source/GUI/Qt/displaywindow.h \
                    ../../Source/GUI/Qt/policieswindow.h \
                    ../../Source/GUI/Qt/progressbar.h \
                    ../../Source/GUI/Qt/workerfiles.h \
                    ../../Source/GUI/Qt/DatabaseUi.h \
                    ../../Source/GUI/Qt/NoDatabaseUi.h \
                    ../../Source/GUI/Qt/SQLLiteUi.h \
                    ../../Source/GUI/Qt/uisettings.h \
                    ../../Source/GUI/Qt/publicpolicieswindow.h

FORMS            += ../../Source/GUI/Qt/mainwindow.ui \
                    ../../Source/GUI/Qt/progressbar.ui


equals(WEB_MACHINE, webengine) {
    !qtHaveModule(webenginewidgets) {
        error("qt module webenginewidgets not found")
    }

    !qtHaveModule(webchannel) {
        error("qt module webchannel not found")
    }

    QT += webenginewidgets webchannel
    SOURCES += ../../Source/GUI/Qt/WebEnginePage.cpp \
               ../../Source/GUI/Qt/WebEngineView.cpp
    HEADERS += ../../Source/GUI/Qt/WebEnginePage.h \
               ../../Source/GUI/Qt/WebEngineView.h
    DEFINES += WEB_MACHINE_ENGINE
} else {
    greaterThan(QT_MAJOR_VERSION, 4) {
        !qtHaveModule(webkit) {
            error("qt module webkit not found")
        }
    } else {
        # Ubuntu build QtWebKit from separate sources therefore QT_CONFIG contains
        # neither webkit nor no-webkit, so we also check for pkg-config module
        !contains(QT_CONFIG, webkit):!packagesExist(QtWebKit) {
            error("qt module webkit not found")
        }
    }

    QT += webkit webkitwidgets
    SOURCES += ../../Source/GUI/Qt/WebKitPage.cpp \
               ../../Source/GUI/Qt/WebKitView.cpp
    HEADERS += ../../Source/GUI/Qt/WebKitPage.h \
               ../../Source/GUI/Qt/WebKitView.h
    DEFINES += WEB_MACHINE_KIT
}

INCLUDEPATH      += ../../Source

unix:exists(../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config) {
    INCLUDEPATH      += ../../../MediaInfoLib/Source
    contains(STATIC_LIBS, yes|1) {
        LIBS             += $$system(../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS_Static)
        message("custom libmediainfo: yes (static)")
    } else {
        LIBS             += $$system(../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS)
        message("custom libmediainfo: yes (shared)")
    }
} else:unix {
    !packagesExist(libmediainfo) {
        error("libmediainfo not found on system")
    }
    LIBS += $$system(pkg-config --libs libmediainfo)
}

unix:exists(../../../ZenLib/Project/GNU/Library/libzen-config) {
    INCLUDEPATH      += ../../../ZenLib/Source
    contains(STATIC_LIBS, yes|1) {
        LIBS             += $$system(../../../ZenLib/Project/GNU/Library/libzen-config LIBS_Static)
        message("custom libzen       : yes (static)")
    } else {
        LIBS             += $$system(../../../ZenLib/Project/GNU/Library/libzen-config LIBS)
        message("custom libzen       : yes (shared)")
    }
} else:unix {
    PKGCONFIG        += libzen
    message("libzen      : system")
}

unix:exists(../../../libxml2/.libs/libxml2.a) {
    INCLUDEPATH      += ../../../libxml2/include
    LIBS             += ../../../libxml2/.libs/libxml2.a
    message("libxml2     : custom")
} else:unix {
    packagesExist(libxml2) {
        PKGCONFIG += libxml2
    } else {
        PKGCONFIG += libxml-2.0
    }
    message("libxml2     : system")
}

unix:exists(../../../libxslt/libxslt/.libs/libxslt.a) {
    INCLUDEPATH      += ../../../libxslt/libxslt
    INCLUDEPATH      += ../../../libxslt/libexslt
    LIBS             += ../../../libxslt/libxslt/.libs/libxslt.a
    LIBS             += ../../../libxslt/libexslt/.libs/libexslt.a
    message("libxslt     : custom")
} else:unix {
    PKGCONFIG        += libxslt libexslt
    message("libxslt     : system")
}


contains(NO_SQLITE, yes|1) {
    message("libsqlite3  : no")
} else {
    DEFINES              += HAVE_SQLITE
    unix:exists(../../../sqlite/.libs/libsqlite3.a) {
        INCLUDEPATH      += ../../../sqlite
        LIBS             += ../../../sqlite/.libs/libsqlite3.a
        message("libsqlite3  : custom")
    } else:unix {
        PKGCONFIG        += sqlite3
        message("libsqlite3  : system")
    }
}

contains(NO_JANSSON, yes|1) {
    message("libjansson  : no")
} else {
    DEFINES              += HAVE_JANSSON
    unix:exists(../../../jansson/src/.libs/libjansson.a) {
        INCLUDEPATH      += ../../../jansson/src
        LIBS             += ../../../jansson/src/.libs/libjansson.a
        message("libjansson  : custom")
    } else:unix {
        PKGCONFIG        += jansson
        message("libjansson  : system")
    }
}

contains(NO_LIBEVENT, yes|1) {
    message("libevent    : no")
} else {
    DEFINES              += HAVE_LIBEVENT
    unix:exists(../../../libevent/.libs/libevent.a) {
        INCLUDEPATH      += ../../../libevent/include
        LIBS             += ../../../libevent/.libs/libevent.a
        message("libevent    : custom")
    } else:unix {
        PKGCONFIG        += libevent
        message("libevent    : system")
    }
}

contains(NO_LIBCURL, yes|1) {
    message("use libcurl : no")
} else {
    message("use libcurl : yes (from libmediainfo)")
    DEFINES += MEDIAINFO_LIBCURL_YES
}

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
LIBS             += -lz
!macx:LIBS       += -ldl -lrt

RESOURCES        += ../../Source/Resource/Resources.qrc

OBJECTS_DIR      =  _Automated
MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated
UI_DIR           =  _Automated
RCC_DIR          =  _Automated

macx:ICON = ../../Source/Resource/Image/MediaConch.icns
macx:QMAKE_LFLAGS += -framework CoreFoundation

target.path = /usr/bin
INSTALLS += target
