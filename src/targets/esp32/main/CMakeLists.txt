# Find all source files with
file(GLOB COMPONENT_SRCS "*.cpp")

set(COMPONENT_ADD_INCLUDEDIRS
    .
    )

set(COMPONENT_REQUIRES
    nvs_flash
    esp_littlefs
    bt
    core)

register_component()
component_compile_options(-std=gnu++17 -Wformat-truncation -Wrestrict -Wswitch)
