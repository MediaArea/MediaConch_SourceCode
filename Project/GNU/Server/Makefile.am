AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = mediaconchd
mediaconchd_SOURCES = \
    ../../../Source/Daemon/Daemon_Main.cpp \
    ../../../Source/Daemon/Daemon.cpp \
    ../../../Source/Daemon/Help.cpp \
    ../../../Source/Common/MediaConchLib.cpp \
    ../../../Source/Common/Core.cpp \
    ../../../Source/Common/DaemonClient.cpp \
    ../../../Source/Common/Schema.cpp \
    ../../../Source/Common/Schematron.cpp \
    ../../../Source/Common/Xslt.cpp \
    ../../../Source/Common/JS_Tree.cpp \
    ../../../Source/Common/Policies.cpp \
    ../../../Source/Common/Policy.cpp \
    ../../../Source/Common/SchematronPolicy.cpp \
    ../../../Source/Common/XsltPolicy.cpp \
    ../../../Source/Common/UnknownPolicy.cpp \
    ../../../Source/Common/Database.cpp \
    ../../../Source/Common/NoDatabase.cpp \
    ../../../Source/Common/SQLLite.cpp \
    ../../../Source/Common/Json.cpp \
    ../../../Source/Common/Configuration.cpp \
    ../../../Source/Common/REST_API.cpp \
    ../../../Source/Common/Httpd.cpp \
    ../../../Source/Common/LibEventHttpd.cpp \
    ../../../Source/Common/Http.cpp \
    ../../../Source/Common/LibEventHttp.cpp \
    ../../../Source/Common/Queue.cpp \
    ../../../Source/Common/Scheduler.cpp \
    ../../../Source/Common/PluginsManager.cpp \
    ../../../Source/Common/PluginsConfig.cpp \
    ../../../Source/Common/Plugin.cpp \
    ../../../Source/Common/VeraPDF.cpp \
    ../../../Source/Common/DpfManager.cpp

#mediaconchd_LDFLAGS     = -no-undefined -version-info 0:0:0
mediaconchd_CPPFLAGS = $(XML_CFLAGS)
mediaconchd_LDFLAGS= $(XML_LIBS)

INCLUDES = -I../../../Source -I../../../../MediaInfoLib/Source -I../../../../ZenLib/Source
