#include "ESP32Platform.h"

ESP32PlatformPlugin::ESP32PlatformPlugin() : bell::Task("platform", 512, 0, 0)
{
}

void ESP32PlatformPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/wifi", berry);
    scriptLoader->loadScript("esp32/dac_plugin", berry);
    scriptLoader->loadScript("esp32/dacs/i2s_driver", berry);
    scriptLoader->loadScript("esp32/dacs/es8388_driver", berry);
    scriptLoader->loadScript("esp32/dacs/ac101_driver", berry);
    scriptLoader->loadScript("esp32/dacs/tas5711_driver", berry);
}

void ESP32PlatformPlugin::reportRAM(std::string from) {
    auto memUsage = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    auto memUsage2 = heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    BELL_LOG(info, from.c_str(), "Free RAM %d | %d", memUsage, memUsage2);
}

void ESP32PlatformPlugin::setupBindings() {
    // Export necessary drivers to berry
    berry->export_this("report_ram", this, &ESP32PlatformPlugin::reportRAM);
    exportWiFiDriver(berry);
    exportI2CDriver(berry);
    exportI2SDriver(berry);
    exportGPIODriver(berry);
}
