project(picoaudio)

cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp" "src/*.c")

if(${ESP_PLATFORM})
else()
    set(EXTRA_REQ_LIBS Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

if(UNIX AND NOT APPLE)
    set(EXTRA_REQ_LIBS ${EXTRA_REQ_LIBS} dns_sd)
endif()

include_directories("include")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories("protos")

execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_protos.sh" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES ${SOURCES} "protos/AnyRefImpl.cpp" "protos/ReflectTypeInfo.cpp")

add_library(picoaudio STATIC ${SOURCES})
target_link_libraries(picoaudio PRIVATE ${EXTRA_REQ_LIBS})

target_include_directories(picoaudio PUBLIC "include" "protos" ${EXTRA_REQ_LIBS} ${CMAKE_CURRENT_BINARY_DIR})