cmake_minimum_required(VERSION 3.5)
project(mediaconch)
set(CMAKE_CXX_STANDARD 11)
cmake_policy(SET CMP0135 NEW)

option(MEDIACONCH_WITH_SQLITE      "Enable SQLite DB"             OFF)
option(MEDIACONCH_WITH_JANSSON     "Enable Jansson Library"       OFF)
option(MEDIACONCH_WITH_LIBEVENT    "Enable Libevent"              OFF)

find_package(ZLIB REQUIRED)
find_package(ZenLib REQUIRED)
find_package(libxml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(MediaInfoLib REQUIRED)

find_package(jansson QUIET)
find_package(Libevent QUIET)

add_subdirectory(Source)