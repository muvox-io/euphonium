cmake_minimum_required(VERSION 3.5)


message(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_web.sh)
execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_web.sh" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(euphonium-esp32)

littlefs_create_partition_image(storage spiffs)
