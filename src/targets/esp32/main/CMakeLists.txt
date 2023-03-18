# Find all source files with
file(GLOB COMPONENT_SRCS "*.cpp" "app/*.cpp" "drivers/*.cpp" "bluetooth_sink/*.cpp" "bluetooth_sink/*.c")

set(COMPONENT_ADD_INCLUDEDIRS
    .
    app/include
    drivers/include
    bluetooth_sink/include
    )

set(COMPONENT_REQUIRES
    nvs_flash
    bt
    driver
    vfs
    esp_netif
    lwip
    esp_timer
    app_update
    esp_littlefs
    espressif__mdns
    esp_wifi
    core)
register_component()

component_compile_options(-std=gnu++20 -Wno-format -Wformat-truncation -Wrestrict -Wswitch -fdiagnostics-color=always)
