set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "ScriptsUpdater.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/GPIODriver.cpp"
    "driver/led/LEDDriver.cpp"
    "driver/led/Color.cpp"
    "driver/led/LedStrip.cpp"
    "driver/Rotary.cpp"
    "driver/WiFiDriver.cpp"
    "driver/dns_server.c"
    "ota/OTAPlugin.cpp"
    "bluetooth/BluetoothPlugin.cpp"
    "bluetooth/bt_app_core.c"
    "bluetooth/bt_app_sink.c"
    "main.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS "." "driver/." "driver/led/." "bluetooth/." "ota/.")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)