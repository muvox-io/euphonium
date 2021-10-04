project(euphonium)

cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 17)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lua ${CMAKE_CURRENT_BINARY_DIR}/lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cspot/cspot ${CMAKE_CURRENT_BINARY_DIR}/cspot)

file(GLOB SOURCES "src/*.cpp" "src/*.c" "src/plugins/cspot/*.cpp"  "src/plugins/http/*.cpp")

if(${ESP_PLATFORM})
else()
    set(EXTRA_REQ_LIBS Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

if(UNIX AND NOT APPLE)
    set(EXTRA_REQ_LIBS ${EXTRA_REQ_LIBS})
endif()

include_directories("include")
include_directories("include/plugins/cspot")
include_directories("include/plugins/http")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories("sol")


add_library(euphonium STATIC ${SOURCES})
target_link_libraries(euphonium PRIVATE cspot lua ${EXTRA_REQ_LIBS})

target_include_directories(euphonium PUBLIC "include" "sol" "protos" cspot ${EXTRA_REQ_LIBS} ${CMAKE_CURRENT_BINARY_DIR})