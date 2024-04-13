project(euphonium)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Force cspot to use bell provided by us
set(CSPOT_EXTERNAL_BELL "bell"
  CACHE INTERNAL ""
)

# External libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/bell ${CMAKE_CURRENT_BINARY_DIR}/bell)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/berry ${CMAKE_CURRENT_BINARY_DIR}/berry)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/cspot/cspot ${CMAKE_CURRENT_BINARY_DIR}/cspot)

# Include external header libraries
include_directories("external/filesystem")

# Source glob
file(GLOB SOURCES "main/app/*.cpp" "main/app/*.c" "main/plugins/*.cpp" "main/plugins/*.c" "main/multiroom/*.c" "main/multiroom/*.cpp")

# Allow for custom HTTP_SERVER_PORT
if(NOT DEFINED HTTP_SERVER_PORT)
  set(HTTP_SERVER_PORT 80)
endif()

# Minify & pack emergency mode HTML into the binary

find_program(PROG_XXD xxd)
find_program(PROG_MINIFY minify)
if(NOT PROG_XXD)
  message(FATAL_ERROR "Could not find xxd")
endif()
if(NOT PROG_MINIFY)
  message(FATAL_ERROR "Could not find minify program. See https://github.com/tdewolff/minify")
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/emergency_html.cpp
  COMMAND echo "namespace euph {" > ${CMAKE_CURRENT_BINARY_DIR}/emergency_html.cpp
  COMMAND ${PROG_MINIFY} --type html ${CMAKE_CURRENT_SOURCE_DIR}/main/app/emergency.html | ${PROG_XXD} -i -n emergencyModeHtml >> ${CMAKE_CURRENT_BINARY_DIR}/emergency_html.cpp
  COMMAND echo "} // namespace euph" >> ${CMAKE_CURRENT_BINARY_DIR}/emergency_html.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main/app/emergency.html
)

list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/emergency_html.cpp)

if(DEFINED ENV{EUPH_VERSION})
  add_definitions(-DEUPH_VERSION="$ENV{EUPH_VERSION}")
else()
  add_definitions(-DEUPH_VERSION="debug")
  add_definitions(-DEUPH_DEBUG)
endif()

add_definitions(-DHTTP_SERVER_PORT=${HTTP_SERVER_PORT})

include_directories("main/app/include")
include_directories("main/plugins/include")
include_directories("main/multiroom/include")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(ESP_PLATFORM)
  list(APPEND EXTRA_REQ_LIBS "idf::app_update" "idf::esp_littlefs")
endif()

add_library(euphonium STATIC ${SOURCES})

target_link_libraries(euphonium PUBLIC cspot bell berry ${EXTRA_REQ_LIBS})
target_include_directories(euphonium PUBLIC "external/filesystem" "main/app/include" "main/plugins/include" "main/multiroom/include" "protos" bell cspot ${EXTRA_REQ_LIBS} ${CMAKE_CURRENT_BINARY_DIR})
