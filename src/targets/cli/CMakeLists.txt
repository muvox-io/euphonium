project(euphoniumcli)
cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../core ${CMAKE_CURRENT_BINARY_DIR}/euphonium)

find_package(portaudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})
set(THINGS_TO_LINK ${THINGS_TO_LINK} ${PORTAUDIO_LIBRARIES})

file(GLOB SOURCES "*.cpp")
include_directories(".")

add_executable(euphoniumcli ${SOURCES})
target_link_libraries(euphoniumcli euphonium ${CMAKE_DL_LIBS} ${THINGS_TO_LINK})
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)