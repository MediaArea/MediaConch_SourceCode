#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:42:41
#
#-------------------------------------------------

QT     += core gui widgets

WEB_MACHINE=webengine

contains(USE_WEBKIT, yes|1) {
    WEB_MACHINE = webkit
}
contains(USE_WEBENGINE, yes|1) {
    WEB_MACHINE = webengine
}

!defined(packagesExist, test) {
    defineTest(packagesExist) {
        system(pkg-config $$ARGS): return(true)
        return(false)
    }
}

unix:!macx:TARGET = mediaconch-gui
else:TARGET = MediaConch
TEMPLATE = app

CONFIG += qt release c++11
CONFIG += no_keywords

unix:CONFIG += link_pkgconfig

DEFINES          +=  _UNICODE

SOURCES          += ../../Source/Common/MediaConchLib.cpp \
                    ../../Source/Common/Core.cpp \
                    ../../Source/Common/Reports.cpp \
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
                    ../../Source/Common/IMSC1.cpp \
                    ../../Source/Common/PluginPreHook.cpp \
                    ../../Source/Common/PluginFileLog.cpp \
                    ../../Source/Common/WatchFoldersManager.cpp \
                    ../../Source/Common/WatchFolder.cpp \
                    ../../Source/Checker/Checker.cpp \
                    ../../Source/Checker/Path.cpp \
                    ../../Source/IMSC1/IMSC1Plugin.cpp \
                    ../../Source/ThirdParty/tfsxml/tfsxml.cpp \
                    ../../Source/GUI/Qt/main.cpp \
                    ../../Source/GUI/Qt/commonwebwindow.cpp \
                    ../../Source/GUI/Qt/helpwindow.cpp \
                    ../../Source/GUI/Qt/mainwindow.cpp \
                    ../../Source/GUI/Qt/settingswindow.cpp \
                    ../../Source/GUI/Qt/displaywindow.cpp \
                    ../../Source/GUI/Qt/databasewindow.cpp \
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
                    ../../Source/Common/Reports.h \
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
                    ../../Source/Common/generated/St205212010smpteTtXsd.h \
                    ../../Source/Common/generated/Ttml1AnimationXsd.h \
                    ../../Source/Common/generated/Ttml1ContentXsd.h \
                    ../../Source/Common/generated/Ttml1CoreAttribsXsd.h \
                    ../../Source/Common/generated/Ttml1DatatypesXsd.h \
                    ../../Source/Common/generated/Ttml1DocumentXsd.h \
                    ../../Source/Common/generated/Ttml1HeadXsd.h \
                    ../../Source/Common/generated/Ttml1LayoutXsd.h \
                    ../../Source/Common/generated/Ttml1MetadataAttribsXsd.h \
                    ../../Source/Common/generated/Ttml1MetadataItemsXsd.h \
                    ../../Source/Common/generated/Ttml1MetadataXsd.h \
                    ../../Source/Common/generated/Ttml1ParameterAttribsXsd.h \
                    ../../Source/Common/generated/Ttml1ParameterItemsXsd.h \
                    ../../Source/Common/generated/Ttml1ParametersXsd.h \
                    ../../Source/Common/generated/Ttml1ProfileXsd.h \
                    ../../Source/Common/generated/Ttml1StylingAttribsXsd.h \
                    ../../Source/Common/generated/Ttml1StylingXsd.h \
                    ../../Source/Common/generated/Ttml1TimingAttribsXsd.h \
                    ../../Source/Common/generated/Ttml1Xsd.h \
                    ../../Source/Common/generated/TtmlWrapperXsd.h \
                    ../../Source/Common/generated/XmlXsd.h \
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
                    ../../Source/Common/IMSC1.cpp \
                    ../../Source/Common/PluginPreHook.h \
                    ../../Source/Common/PluginLog.h \
                    ../../Source/Common/PluginFileLog.h \
                    ../../Source/Common/WatchFoldersManager.h \
                    ../../Source/Checker/Checker.h \
                    ../../Source/Checker/Path.h \
                    ../../Source/IMSC1/IMSC1Plugin.h \
                    ../../Source/ThirdParty/tfsxml/tfsxml.h \
                    ../../Source/GUI/Qt/commonwebwindow.h \
                    ../../Source/GUI/Qt/helpwindow.h \
                    ../../Source/GUI/Qt/WebPage.h \
                    ../../Source/GUI/Qt/WebCommonPage.h \
                    ../../Source/GUI/Qt/WebView.h \
                    ../../Source/GUI/Qt/mainwindow.h \
                    ../../Source/GUI/Qt/settingswindow.h \
                    ../../Source/GUI/Qt/checkerwindow.h \
                    ../../Source/GUI/Qt/resulttable.h \
                    ../../Source/GUI/Qt/displaywindow.h \
                    ../../Source/GUI/Qt/databasewindow.h \
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
    QT += webkit webkitwidgets
    SOURCES += ../../Source/GUI/Qt/WebKitPage.cpp \
               ../../Source/GUI/Qt/WebKitView.cpp
    HEADERS += ../../Source/GUI/Qt/WebKitPage.h \
               ../../Source/GUI/Qt/WebKitView.h
    DEFINES += WEB_MACHINE_KIT
}

INCLUDEPATH      += ../../Source

win32 {
    INCLUDEPATH                     += ../../../zlib
    contains(QT_ARCH, i386): LIBS   += $${_PRO_FILE_PWD_}/../../../zlib/contrib/vstudio/vc17/x86/ZlibStatReleaseWithoutAsm/zlibstat.lib
    contains(QT_ARCH, x86_64): LIBS += $${_PRO_FILE_PWD_}/../../../zlib/contrib/vstudio/vc17/x64/ZlibStatReleaseWithoutAsm/zlibstat.lib
}


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
}else:win32 {
    INCLUDEPATH                     += ../../../MediaInfoLib/Source
    contains(QT_ARCH, i386): LIBS   += $${_PRO_FILE_PWD_}/../../../MediaInfoLib/Project/MSVC2022/Win32/Release/MediaInfo-Static.lib
    contains(QT_ARCH, x86_64): LIBS += $${_PRO_FILE_PWD_}/../../../MediaInfoLib/Project/MSVC2022/x64/Release/MediaInfo-Static.lib
}

unix:exists(../../../ZenLib/Project/GNU/Library/libzen-config) {
    INCLUDEPATH                    += ../../../ZenLib/Source
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
} else:win32 {
    INCLUDEPATH                     += ../../../ZenLib/Source
    contains(QT_ARCH, i386): LIBS   += $${_PRO_FILE_PWD_}/../../../MediaInfoLib/Project/MSVC2022/Win32/Release/ZenLib.lib
    contains(QT_ARCH, x86_64): LIBS += $${_PRO_FILE_PWD_}/../../../MediaInfoLib/Project/MSVC2022/x64/Release/ZenLib.lib
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
} else:win32 {
    DEFINES                        += LIBXML_STATIC
    INCLUDEPATH                    += ../../../libxml2/include
    contains(QT_ARCH, i386):LIBS   += $${_PRO_FILE_PWD_}/../../../libxml2/win32/VC17/Win32/Release/libxml2.lib
    contains(QT_ARCH, x86_64):LIBS += $${_PRO_FILE_PWD_}/../../../libxml2/win32/VC17/x64/Release/libxml2.lib
}

unix:exists(../../../libxslt/libxslt/.libs/libxslt.a) {
    INCLUDEPATH      += ../../../libxslt
    LIBS             += ../../../libxslt/libxslt/.libs/libxslt.a
    LIBS             += ../../../libxslt/libexslt/.libs/libexslt.a
    message("libxslt     : custom")
} else:unix {
    PKGCONFIG        += libxslt libexslt
    message("libxslt     : system")
} else:win32 {
    DEFINES                        += LIBXSLT_STATIC LIBEXSLT_STATIC
    INCLUDEPATH                    += ../../../libxslt
    contains(QT_ARCH, i386):LIBS   += $${_PRO_FILE_PWD_}/../../../libxslt/win32/VC17/libxslt/Win32/Release/libxslt.lib
    contains(QT_ARCH, x86_64):LIBS += $${_PRO_FILE_PWD_}/../../../libxslt/win32/VC17/libxslt/x64/Release/libxslt.lib
    contains(QT_ARCH, i386):LIBS   += $${_PRO_FILE_PWD_}/../../../libxslt/win32/VC17/libexslt/Win32/Release/libexslt.lib
    contains(QT_ARCH, x86_64):LIBS += $${_PRO_FILE_PWD_}/../../../libxslt/win32/VC17/libexslt/x64/Release/libexslt.lib
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
    } else:win32 {
        DEFINES     += HAVE_SQLITE
        INCLUDEPATH += ../../Source/ThirdParty/sqlite
        SOURCES     += ../../Source/ThirdParty/sqlite/sqlite3.c
        HEADERS     += ../../Source/ThirdParty/sqlite/sqlite3.h
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
    } else:win32 {
        DEFINES                        += HAVE_JANSSON
        INCLUDEPATH                    += ../../../jansson/Contrib/VC17/Jansson ../../../jansson/src
        contains(QT_ARCH, i386):LIBS   += $${_PRO_FILE_PWD_}/../../../jansson/Contrib/VC17/Jansson/Win32/Release/Jansson.lib
        contains(QT_ARCH, x86_64):LIBS += $${_PRO_FILE_PWD_}/../../../jansson/Contrib/VC17/Jansson/x64/Release/Jansson.lib
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
    } else:win32 {
        DEFINES                        += HAVE_LIBEVENT
        INCLUDEPATH                    += ../../../libevent/WIN32-Code/nmake ../../../libevent/include
        contains(QT_ARCH, i386):LIBS   += $${_PRO_FILE_PWD_}/../../../libevent/Contrib/VC17/event/Win32/Release/event.lib
        contains(QT_ARCH, x86_64):LIBS += $${_PRO_FILE_PWD_}/../../../libevent/Contrib/VC17/event/x64/Release/event.lib
    }
}

macx:contains(MACSTORE, yes|1) {
    QMAKE_LFLAGS+=-Wl,-ld_classic
    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
    QMAKE_INFO_PLIST = ../Mac/Info.plist
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
} else:macx {
    QMAKE_LFLAGS+=-Wl,-ld_classic
    QMAKE_INFO_PLIST = ../Mac/Info-ns.plist
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
}

win32 {
    RC_FILE = MediaConch.rc
    LIBS += winmm.lib ws2_32.lib imm32.lib ole32.lib
    contains(QT_ARCH, i386): DESTDIR = Win32
    contains(QT_ARCH, x86_64): DESTDIR = x64
}

unix:LIBS        += -lz
unix:!macx:LIBS  += -ldl -lrt

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
