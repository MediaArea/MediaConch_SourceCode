AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = mediaconch
mediaconch_SOURCES = \
    ../../../Source/CLI/CLI_Main.cpp \
    ../../../Source/CLI/CommandLine_Parser.cpp \
    ../../../Source/CLI/Help.cpp \
    ../../../Source/Common/Core.cpp \
    ../../../Source/Common/Schema.cpp \
    ../../../Source/Common/Schematron.cpp \
    ../../../Source/Common/Xslt.cpp \
    ../../../Source/Common/JS_Tree.cpp \
    ../../../Source/Common/Policies.cpp \
    ../../../Source/Common/Policy.cpp \
    ../../../Source/Common/SchematronPolicy.cpp \
    ../../../Source/Common/XsltPolicy.cpp \
    ../../../Source/Common/Database.cpp \
    ../../../Source/Common/SQLLite.cpp \
    ../../../Source/Common/Json.cpp

#mediaconch_LDFLAGS     = -no-undefined -version-info 0:0:0
mediaconch_CPPFLAGS = $(XML_CFLAGS)
mediaconch_LDFLAGS= $(XML_LIBS)

INCLUDES = -I../../../Source -I../../../../MediaInfoLib/Source -I../../../../ZenLib/Source
