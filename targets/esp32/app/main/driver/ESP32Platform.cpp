#include "ESP32Platform.h"

ESP32PlatformPlugin::ESP32PlatformPlugin()
    : bell::Task("platform", 2048, 2, 0) {
    name = "platform";
}

void ESP32PlatformPlugin::loadScript(
    std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/platform", berry);
    scriptLoader->loadScript("esp32/wifi", berry);
    scriptLoader->loadScript("esp32/dac_plugin", berry);
    scriptLoader->loadScript("esp32/dacs/i2s_driver", berry);
    scriptLoader->loadScript("esp32/dacs/es8388_driver", berry);
    scriptLoader->loadScript("esp32/dacs/ac101_driver", berry);
    scriptLoader->loadScript("esp32/dacs/tas5711_driver", berry);
    scriptLoader->loadScript("esp32/dacs/ma12070p_driver", berry);
    scriptLoader->loadScript("esp32/dacs/internal_driver", berry);
}

void ESP32PlatformPlugin::registerButton(int gpio, bool highState) {
    buttonList.push_back(gpio);
    buttonStateMap[gpio] = {false, highState};
}

void ESP32PlatformPlugin::runTask() {
    // Quick and dirty button press detector. @TODO: Replace with proper implementation
    while (true) {
        for (auto button : buttonList) {
            bool buttonState = gpio_get_level((gpio_num_t)button);
            if (buttonState != buttonStateMap[button].first) {
                buttonStateMap[button].first = buttonState;
                if (buttonStateMap[button].first ==
                    buttonStateMap[button].second) {
                    auto event =
                        std::make_unique<ButtonInteractionEvent>("PRESS", button);
                    this->luaEventBus->postEvent(std::move(event));
                } else {
                    // berry->call("button_released", button);
                }
            }
        }
        BELL_SLEEP_MS(30);
    }
}

void ESP32PlatformPlugin::setupBindings() {
    // Export necessary drivers to berry
    exportWiFiDriver(berry);
    exportI2CDriver(berry);
    exportI2SDriver(berry);
    exportGPIODriver(berry);
    exportLEDDriver(berry);

    // Export platform functions to berry
    berry->export_this("register_button", this,
                       &ESP32PlatformPlugin::registerButton, "gpio");
}
