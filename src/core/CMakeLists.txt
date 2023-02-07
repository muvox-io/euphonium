project(euphonium)

set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/bell ${CMAKE_CURRENT_BINARY_DIR}/bell)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/berry ${CMAKE_CURRENT_BINARY_DIR}/berry)

# Source glob
file(GLOB SOURCES "main/app/*.cpp" "main/app/*.c" "main/plugins/*.cpp" "main/plugins/*.c" "main/multiroom/*.c" "main/multiroom/*.cpp")

# Allow for custom HTTP_SERVER_PORT
if (NOT DEFINED HTTP_SERVER_PORT)
  set(HTTP_SERVER_PORT 80)
endif()

add_definitions(-DHTTP_SERVER_PORT=${HTTP_SERVER_PORT})

include_directories("main/app/include")
include_directories("main/plugins/include")
include_directories("main/multiroom/include")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(euphonium STATIC ${SOURCES})

target_link_libraries(euphonium PUBLIC bell berry ${EXTRA_REQ_LIBS})
target_include_directories(euphonium PUBLIC "main/app/include" "main/plugins/include" "main/multiroom/include" "protos" bell ${EXTRA_REQ_LIBS} ${CMAKE_CURRENT_BINARY_DIR})
