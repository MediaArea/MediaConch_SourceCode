AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CXXFLAGS = -std=c++11

bin_PROGRAMS = mediaconch
mediaconch_SOURCES = \
    ../../../Source/CLI/CLI_Main.cpp \
    ../../../Source/CLI/CLI.cpp \
    ../../../Source/CLI/CommandLine_Parser.cpp \
    ../../../Source/CLI/Help.cpp \
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
    ../../../Source/ThirdParty/tfsxml/tfsxml.cpp

#mediaconch_LDFLAGS     = -no-undefined -version-info 0:0:0
mediaconch_CPPFLAGS = $(XML_CFLAGS)
mediaconch_LDFLAGS= $(XML_LIBS)

INCLUDES = -I../../../Source -I../../../../MediaInfoLib/Source -I../../../../ZenLib/Source

AM_TESTS_FD_REDIRECT = 9>&2

TESTS = \
    test/simple.sh \
    test/filename.sh \
    test/test_mk.sh \
    test/test_ffv1.sh \
    test/test_policy.sh

SAMPLES_DIR = test/SampleFiles

clone_sample_files: $(SAMPLES_DIR)

$(SAMPLES_DIR):
	git clone https://github.com/MediaArea/MediaConch_SampleFiles test/SampleFiles
