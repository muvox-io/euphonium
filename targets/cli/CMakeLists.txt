project(picoaudiocli)
cmake_minimum_required(VERSION 3.18) 
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../picoaudio ${CMAKE_CURRENT_BINARY_DIR}/picoaudio)

file(GLOB SOURCES "*.cpp")
include_directories(".")

add_executable(picoaudiocli ${SOURCES})
target_link_libraries(picoaudiocli picoaudio ${CMAKE_DL_LIBS} ${THINGS_TO_LINK} )
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)