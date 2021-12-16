set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/WiFiDriver.cpp"
    "bluetooth/BluetoothPlugin.cpp"
    "bluetooth/bt_app_core.c"
    "bluetooth/bt_app_sink.c"
    "main.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS "." "driver/." "bluetooth/.")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)
