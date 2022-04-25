#include "ESP32Platform.h"
#include "Rotary.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

ESP32PlatformPlugin::ESP32PlatformPlugin()
    : bell::Task("platform", 2048, 5, 0) {
    name = "platform";
}

static QueueHandle_t gpio_evt_queue = NULL;

Rotary encoder(2, 20);

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    unsigned char result = encoder.process();
    xQueueSendFromISR(gpio_evt_queue, &result, NULL);
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
}
auto volume = 0;
void ESP32PlatformPlugin::runTask() {
    // Quick and dirty button press detector. @TODO: Replace with proper
    // implementation
    unsigned char dir;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &dir, portMAX_DELAY)) {
            if (dir == DIR_CCW) {
                if (volume < 100) {
                    volume += 5;
                }
                auto event = std::make_unique<VolumeChangedEvent>(volume);
                this->luaEventBus->postEvent(std::move(event));
            } else if (dir == DIR_CW) {
                if (volume > 0) {
                    volume -= 5;
                }
                auto event = std::make_unique<VolumeChangedEvent>(volume);
                this->luaEventBus->postEvent(std::move(event));
            }
        }
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

    gpio_config_t io_conf = {};
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = ((1ULL << 20) | (1ULL << 2));
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    io_conf.pull_up_en = (gpio_pullup_t)1;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)1;
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // install gpio isr service
    gpio_install_isr_service(0);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)2, gpio_isr_handler, (void *)2);
    gpio_isr_handler_add((gpio_num_t)20, gpio_isr_handler, (void *)20);
}
