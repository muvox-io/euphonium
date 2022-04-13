project(euphonium)

set(CMAKE_BUILD_TYPE Debug)
cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 17)

set(CSPOT_EXTERNAL_BELL "bell"
        CACHE INTERNAL ""
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bell ${CMAKE_CURRENT_BINARY_DIR}/bell)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cspot/cspot ${CMAKE_CURRENT_BINARY_DIR}/cspot)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../berry ${CMAKE_CURRENT_BINARY_DIR}/berry)

file(GLOB SOURCES "src/*.cpp" "src/*.c" "src/plugins/webradio/*.cpp" "src/plugins/mqtt/*.cpp" "src/plugins/mqtt/*.c"  "src/plugins/cspot/*.cpp" "src/plugins/persistor/*.cpp" "src/plugins/youtube/*.cpp"  "src/plugins/http/*.cpp")

if(${ESP_PLATFORM})
else()
    set(EXTRA_REQ_LIBS Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    add_definitions(-Wno-sequence-point)
endif()

if(UNIX AND NOT APPLE)
    set(EXTRA_REQ_LIBS ${EXTRA_REQ_LIBS})
endif()


include_directories("include")
include_directories("include/plugins/cspot")
include_directories("include/plugins/http")
include_directories("include/plugins/persistor")
include_directories("include/plugins/webradio")
include_directories("include/plugins/youtube")
include_directories("include/plugins/mqtt")
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_library(euphonium STATIC ${SOURCES})
target_link_libraries(euphonium PRIVATE bell cspot berry ${EXTRA_REQ_LIBS})

target_include_directories(euphonium PUBLIC "include" "protos" bell cspot ${EXTRA_REQ_LIBS} ${CMAKE_CURRENT_BINARY_DIR})
