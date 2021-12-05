set(COMPONENT_SRCS
    "SPIFFSScriptLoader.cpp"
    "DACAudioOutput.cpp"
    "driver/ESP32Platform.cpp"
    "driver/I2CDriver.cpp"
    "driver/I2SDriver.cpp"
    "driver/WiFiDriver.cpp"
    "main.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS "." "driver/.")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)
