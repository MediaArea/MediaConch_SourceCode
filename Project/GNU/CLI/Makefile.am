AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = mediaconch
mediaconch_SOURCES = \
    ../../../Source/CLI/CLI_Main.cpp \
    ../../../Source/CLI/CLI.cpp \
    ../../../Source/CLI/CommandLine_Parser.cpp \
    ../../../Source/CLI/Help.cpp \
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
    ../../../Source/Common/Scheduler.cpp

#mediaconch_LDFLAGS     = -no-undefined -version-info 0:0:0
mediaconch_CPPFLAGS = $(XML_CFLAGS)
mediaconch_LDFLAGS= $(XML_LIBS)

INCLUDES = -I../../../Source -I../../../../MediaInfoLib/Source -I../../../../ZenLib/Source
