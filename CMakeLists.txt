cmake_minimum_required(VERSION 2.8.12)

# Configurable options
option(EUPH_TARGET_CLI "Enable euphoniumcli target" OFF)
option(EUPH_TARGET_ESP32 "Enable euphonium-esp32 target" OFF)

# Enable euphoniumcli target
if(EUPH_TARGET_CLI)
    add_subdirectory(targets/cli)
    project(euphoniumcli)
endif()

# Enable euphonium-esp32 target
if(EUPH_TARGET_ESP32)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(IDF_NO_INCLUDE ON)
    add_subdirectory(targets/esp32)
    project(euphonium-esp32)
endif()