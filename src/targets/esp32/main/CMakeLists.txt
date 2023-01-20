# Find all source files with
file(GLOB COMPONENT_SRCS "*.cpp" "app/*.cpp" "drivers/*.cpp")

set(COMPONENT_ADD_INCLUDEDIRS
    .
    app/include
    drivers/include
    )

set(COMPONENT_REQUIRES
    nvs_flash
    bt
    driver
    vfs
    esp_netif
    lwip
    esp_timer
    esp_wifi
    core)
register_component()

component_compile_options(-std=gnu++17 -Wformat-truncation -Wrestrict -Wswitch)
