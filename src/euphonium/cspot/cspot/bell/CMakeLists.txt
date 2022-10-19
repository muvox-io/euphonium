cmake_minimum_required(VERSION 2.8.12)

project(bell)

# Configurable options
option(BELL_DISABLE_CODECS "Disable libhelix AAC and MP3 codecs" OFF)
option(BELL_DISABLE_SINKS "Disable built-in audio sink implementations" OFF)
option(BELL_USE_ALSA "Enable ALSA sink" OFF)
option(BELL_USE_PORTAUDIO "Enable PortAudio sink" OFF)
set(BELL_EXTERNAL_CJSON "" CACHE STRING "External cJSON library target name, optional")
set(BELL_EXTERNAL_TREMOR "" CACHE STRING "External tremor library target name, optional")

# Include nanoPB library
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/nanopb/extra)
find_package(Nanopb REQUIRED)
list(APPEND EXTRA_INCLUDES ${NANOPB_INCLUDE_DIRS})

# CMake options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
add_definitions(-DUSE_DEFAULT_STDLIB=1)

# Main library sources
file(GLOB SOURCES "src/*.cpp" "src/*.c" "nanopb/*.c")

# Add platform specific sources
if(ESP_PLATFORM)
    file(GLOB ESP_PLATFORM_SOURCES "src/platform/esp/*.cpp" "src/platform/esp/*.c" "src/asm/biquad_f32_ae32.S")
    list(APPEND SOURCES ${ESP_PLATFORM_SOURCES})
endif()

if(UNIX)
    file(GLOB UNIX_PLATFORM_SOURCES "src/platform/unix/*.cpp" "src/platform/linux/TLSSocket.cpp" "src/platform/unix/*.c")
    list(APPEND SOURCES ${UNIX_PLATFORM_SOURCES})
endif()

if(APPLE)
    file(GLOB APPLE_PLATFORM_SOURCES "src/platform/apple/*.cpp" "src/platform/linux/TLSSocket.cpp"  "src/platform/apple/*.c")
    list(APPEND SOURCES ${APPLE_PLATFORM_SOURCES})
endif()

if(UNIX AND NOT APPLE)
    file(GLOB LINUX_PLATFORM_SOURCES "src/platform/linux/*.cpp" "src/platform/linux/*.c")
    list(APPEND SOURCES ${LINUX_PLATFORM_SOURCES})
endif()

if(ESP_PLATFORM)
    # Use MBedTLS on ESP32
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/CryptoOpenSSL.cpp)
    idf_build_set_property(COMPILE_DEFINITIONS "-DBELL_USE_MBEDTLS" APPEND)
    list(APPEND EXTRA_LIBS idf::mbedtls idf::pthread idf::mdns)
    add_definitions(-Wunused-const-variable -Wchar-subscripts -Wunused-label -Wmaybe-uninitialized -Wmisleading-indentation)
else()
    # Use OpenSSL elsewhere
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/CryptoMbedTLS.cpp)
    find_package(OpenSSL REQUIRED)
    find_package(Threads REQUIRED)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    if(OPENSSL_FOUND)
        set(OPENSSL_USE_STATIC_LIBS TRUE)
    endif()
    list(APPEND EXTRA_LIBS OpenSSL::Crypto OpenSSL::SSL Threads::Threads)
endif()

if(BELL_DISABLE_CODECS)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/DecoderGlobals.cpp)
else()
    file(GLOB LIBHELIX_AAC_SOURCES "libhelix-aac/*.c")
    file(GLOB LIBHELIX_MP3_SOURCES "libhelix-mp3/*.c")
    list(APPEND EXTRA_INCLUDES "libhelix-aac" "libhelix-mp3")
    list(APPEND SOURCES ${LIBHELIX_MP3_SOURCES} ${LIBHELIX_AAC_SOURCES})

    if(CYGWIN)
        # Both Cygwin and ESP are Unix-like so this seems to work (or, at least, compile)
        set_source_files_properties(src/DecoderGlobals.cpp PROPERTIES COMPILE_FLAGS -DESP_PLATFORM)
        set_source_files_properties(${LIBHELIX_AAC_SOURCES} PROPERTIES COMPILE_FLAGS -DESP_PLATFORM)
        set_source_files_properties(${LIBHELIX_MP3_SOURCES} PROPERTIES COMPILE_FLAGS -DESP_PLATFORM)
    endif()
endif()

if(NOT BELL_DISABLE_SINKS)
    set(PLATFORM "unix")
    if(ESP_PLATFORM)
        set(PLATFORM "esp")
    endif()
    # Add all built-in audio sinks
    file(GLOB SINK_SOURCES "src/sinks/${PLATFORM}/*.cpp" "src/sinks/${PLATFORM}/*.c")
    # Find ALSA if required, else remove the sink
    if(BELL_USE_ALSA)
        find_package(ALSA REQUIRED)
        list(APPEND EXTRA_INCLUDES ${ALSA_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${ALSA_LIBRARIES})
    else()
        list(REMOVE_ITEM SINK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/unix/ALSAAudioSink.cpp)
    endif()
    # Find PortAudio if required, else remove the sink
    if(BELL_USE_PORTAUDIO)
        find_package(portaudio REQUIRED)
        list(APPEND EXTRA_INCLUDES ${PORTAUDIO_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${PORTAUDIO_LIBRARIES})
    else()
        list(REMOVE_ITEM SINK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/unix/PortAudioSink.cpp)
    endif()
    list(APPEND SOURCES ${SINK_SOURCES})
    list(APPEND EXTRA_INCLUDES "include/sinks/${PLATFORM}")
endif()

if(BELL_EXTERNAL_CJSON)
    list(APPEND EXTRA_LIBS ${BELL_EXTERNAL_CJSON})
else()
    list(APPEND EXTRA_INCLUDES "cJSON")
    list(APPEND SOURCES "cJSON/cJSON.c")
endif()

if(BELL_EXTERNAL_TREMOR)
    list(APPEND EXTRA_LIBS ${BELL_EXTERNAL_TREMOR})
else()
    file(GLOB TREMOR_SOURCES "tremor/*.c")
    list(REMOVE_ITEM TREMOR_SOURCES "tremor/ivorbisfile_example.c")
    list(APPEND EXTRA_INCLUDES "tremor")
    list(APPEND SOURCES ${TREMOR_SOURCES})
endif()

add_library(bell STATIC ${SOURCES})
# PUBLIC to propagate esp-idf includes to bell dependents
target_link_libraries(bell PUBLIC ${EXTRA_LIBS})
target_include_directories(bell PUBLIC "include" "include/platform" ${EXTRA_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(bell PUBLIC PB_ENABLE_MALLOC)
