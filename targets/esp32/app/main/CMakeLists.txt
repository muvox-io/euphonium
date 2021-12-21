set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "ScriptsUpdater.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/WiFiDriver.cpp"
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
    set(EUPH_VERSION, ENV{EUPH_VER})
    add_definitions(-DEUPH_VERSION="${EUPH_VERSION}")
endif()
