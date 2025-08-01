add_library(MediaConchLib
        ../Common/Configuration.cpp
        ../Common/Core.cpp
        ../Common/DaemonClient.cpp
        ../Common/Database.cpp
        ../Common/DatabaseReport.cpp
        ../Common/DpfManager.cpp
        ../Common/Http.cpp
        ../Common/Httpd.cpp
        ../Common/IMSC1.cpp
        ../Common/JS_Tree.cpp
        ../Common/Json.cpp
        ../Common/LibEventHttp.cpp
        ../Common/LibEventHttpd.cpp
        ../Common/MediaConchLib.cpp
        ../Common/NoDatabaseReport.cpp
        ../Common/Plugin.cpp
        ../Common/PluginFileLog.cpp
        ../Common/PluginPreHook.cpp
        ../Common/PluginsConfig.cpp
        ../Common/PluginsManager.cpp
        ../Common/Policy.cpp
        ../Common/Policies.cpp
        ../Common/Queue.cpp
        ../Common/Reports.cpp
        ../Common/REST_API.cpp
        ../Common/Scheduler.cpp
        ../Common/Schema.cpp
        ../Common/SQLLite.cpp
        ../Common/SQLLiteReport.cpp
        ../Common/UnknownPolicy.cpp
        ../Common/VeraPDF.cpp
        ../Common/WatchFolder.cpp
        ../Common/WatchFoldersManager.cpp
        ../Common/Xslt.cpp
        ../Common/XsltPolicy.cpp
        ../IMSC1/IMSC1Plugin.cpp
        ../Checker/Checker.cpp
        ../Checker/Path.cpp
        ../ThirdParty/tfsxml/tfsxml.cpp
        MediaConchDLL.cpp
        Lib.cpp
)

target_link_libraries(MediaConchLib
    PRIVATE
        zen
        LibXml2::LibXml2
        libxslt::libxslt
        ZLIB::ZLIB
        mediainfo
)

add_library(MediaConch:lib ALIAS MediaConchLib)

set_target_properties(MediaConchLib PROPERTIES
    OUTPUT_NAME mediaconch
    PUBLIC_HEADER MediaConchDLL.h
)

target_include_directories(MediaConchLib
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Lib>
)

if (MEDIACONCH_WITH_SQLITE)
    target_sources(MediaConchLib PRIVATE ../ThirdParty/sqlite/sqlite3.c)
    target_include_directories(MediaConchLib PRIVATE ../ThirdParty/sqlite)
    target_compile_definitions(MediaConchLib PRIVATE HAVE_SQLITE)
endif ()

if(Libevent_FOUND AND MEDIACONCH_WITH_LIBEVENT)
    target_link_libraries(MediaConchLib PRIVATE libevent::libevent)
    target_compile_definitions(MediaConchLib PRIVATE HAVE_LIBEVENT)
endif ()

if(jansson_FOUND AND MEDIACONCH_WITH_JANSSON)
    target_link_libraries(MediaConchLib PRIVATE jansson::jansson)
    target_compile_definitions(MediaConchLib PRIVATE HAVE_JANSSON)
endif ()

install(TARGETS MediaConchLib EXPORT MediaConchTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/MediaConch
)
install(EXPORT MediaConchTargets
        FILE MediaConchTargets.cmake
        NAMESPACE MediaConch::
        DESTINATION share/cmake/${PROJECT_NAME}
)
