cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

if (DEFINED ENV{EUPH_VER})
    message("Setting euphonium version")
    message("version = $ENV{EUPH_VER}")
    add_definitions(-DEUPH_VERSION="$ENV{EUPH_VER}")
else()
    message("Euphonium version not set")
    add_definitions(-DEUPH_VERSION="debug")
endif()

set(CMAKE_COLOR_DIAGNOSTICS ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(euphonium-esp32)

littlefs_create_partition_image(storage fs)
