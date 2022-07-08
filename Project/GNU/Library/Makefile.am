AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CXXFLAGS = -std=c++11

lib_LTLIBRARIES = lib@MediaConchLib_LibName@.la
lib@MediaConchLib_LibName@_la_SOURCES = \
    ../../../Source/Lib/Lib.cpp \
    ../../../Source/Common/MediaConchLib.cpp \
    ../../../Source/Common/Core.cpp \
    ../../../Source/Common/Reports.cpp \
    ../../../Source/Common/DaemonClient.cpp \
    ../../../Source/Common/Schema.cpp \
    ../../../Source/Common/Xslt.cpp \
    ../../../Source/Common/JS_Tree.cpp \
    ../../../Source/Common/Policies.cpp \
    ../../../Source/Common/Policy.cpp \
    ../../../Source/Common/XsltPolicy.cpp \
    ../../../Source/Common/UnknownPolicy.cpp \
    ../../../Source/Common/Database.cpp \
    ../../../Source/Common/DatabaseReport.cpp \
    ../../../Source/Common/NoDatabaseReport.cpp \
    ../../../Source/Common/SQLLite.cpp \
    ../../../Source/Common/SQLLiteReport.cpp \
    ../../../Source/Common/Json.cpp \
    ../../../Source/Common/Configuration.cpp \
    ../../../Source/Common/REST_API.cpp \
    ../../../Source/Common/Httpd.cpp \
    ../../../Source/Common/LibEventHttpd.cpp \
    ../../../Source/Common/Http.cpp \
    ../../../Source/Common/LibEventHttp.cpp \
    ../../../Source/Common/Queue.cpp \
    ../../../Source/Common/Scheduler.cpp \
    ../../../Source/Common/PluginsConfig.cpp \
    ../../../Source/Common/PluginsManager.cpp \
    ../../../Source/Common/Plugin.cpp \
    ../../../Source/Common/VeraPDF.cpp \
    ../../../Source/Common/DpfManager.cpp \
    ../../../Source/Common/IMSC1.cpp \
    ../../../Source/Common/PluginPreHook.cpp \
    ../../../Source/Common/PluginFileLog.cpp \
    ../../../Source/Common/WatchFoldersManager.cpp \
    ../../../Source/Common/WatchFolder.cpp \
    ../../../Source/Checker/Checker.cpp \
    ../../../Source/Checker/Path.cpp \
    ../../../Source/IMSC1/IMSC1Plugin.cpp \
    ../../../Source/ThirdParty/tfsxml/tfsxml.c

#lib@MediaConchLib_LibName@includedir = $(includedir)/MediaConch
#lib@MediaConchLib_LibName@include_HEADERS = \
#    ../../../Source/Lib/MediaConchLib.h

if COMPILE_DLL
lib@MediaConchLib_LibName@_la_SOURCES += \
    ../../../Source/Lib/MediaConchDLL.cpp

lib@MediaConchLib_LibName@DLLincludedir = $(includedir)/MediaConch
lib@MediaConchLib_LibName@DLLinclude_HEADERS = \
    ../../../Source/Lib/MediaConchDLL.h
endif

lib@MediaConchLib_LibName@_la_LDFLAGS     = -no-undefined -version-info 0:0:0 $(XML_LIBS)
lib@MediaConchLib_LibName@_la_CPPFLAGS    = $(XML_CFLAGS) -I../../../Source -I../../../../MediaInfoLib/Source -I../../../../ZenLib/Source

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = libmediaconch.pc
