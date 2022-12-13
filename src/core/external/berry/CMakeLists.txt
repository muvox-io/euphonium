project(berry)

set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.c" "default/*.c")

include_directories("src")
include_directories("default")
include_directories("generate")

add_library(berry STATIC ${SOURCES})
target_link_libraries(berry PRIVATE)

target_include_directories(berry PUBLIC "src" "generate" "default" ${CMAKE_CURRENT_BINARY_DIR})