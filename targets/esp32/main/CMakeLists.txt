set(COMPONENT_SRCS
    "main.cpp"
    "SPIFFSScriptLoader.cpp"
    "ac101.c"
    "AC101AudioOutput.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS ".")
register_component(REQUIRES "euphonium")
component_compile_options(-std=gnu++17)
