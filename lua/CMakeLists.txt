project(lua)

cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.c")

include_directories("src")

add_library(lua STATIC ${SOURCES})
target_link_libraries(lua PRIVATE)

target_include_directories(lua PUBLIC "src" ${CMAKE_CURRENT_BINARY_DIR})