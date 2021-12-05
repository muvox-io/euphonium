#include "ESP32Platform.h"

ESP32PlatformPlugin::ESP32PlatformPlugin() : bell::Task("platform", 512, 0)
{
}

void ESP32PlatformPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/wifi", berry);
    scriptLoader->loadScript("esp32/dac_plugin", berry);
    scriptLoader->loadScript("esp32/dacs/i2s_driver", berry);
    scriptLoader->loadScript("esp32/dacs/ac101_driver", berry);
    scriptLoader->loadScript("esp32/dacs/tas5711_driver", berry);
}

void ESP32PlatformPlugin::setupBindings() {
    // Export necessary drivers to berry
    applyWiFiEventBus(this->luaEventBus);
    exportWiFiDriver(berry);
    exportI2CDriver(berry);
    exportI2SDriver(berry);
}