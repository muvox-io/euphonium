set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "ScriptsUpdater.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/GPIODriver.cpp"
    "driver/WiFiDriver.cpp"
    "driver/dns_server.c"
    "ota/OTAPlugin.cpp"
    "bluetooth/BluetoothPlugin.cpp"
    "bluetooth/bt_app_core.c"
    "bluetooth/bt_app_sink.c"
    "main.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS "." "driver/." "bluetooth/." "ota/.")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)

if (DEFINED ENV{EUPH_VER})
    message("Setting euphonium version")
    message("version = $ENV{EUPH_VER}")
    add_definitions(-DEUPH_VERSION="$ENV{EUPH_VER}")
else()
    add_definitions(-DEUPH_VERSION="debug")
endif()
