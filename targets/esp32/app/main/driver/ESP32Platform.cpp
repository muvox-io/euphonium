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

static QueueHandle_t buttonEventQueue = NULL;
static QueueHandle_t encoderEventQueue = NULL;


static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t gpio = (uint32_t) arg;
    xQueueSendFromISR(buttonEventQueue, &gpio, NULL);
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
    return;
    buttonList.push_back(gpio);

    std::cout << "Registering button " << gpio << std::endl;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = ((1ULL << 18));
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    io_conf.pull_up_en = (gpio_pullup_t)1;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)1;

    uint64_t pinBitMask = (1ULL << buttonList[0]);

    for (int x = 1; x < buttonList.size(); x++) {
        pinBitMask = pinBitMask | (1ULL << buttonList[x]);
    }
    io_conf.pin_bit_mask = pinBitMask;

    gpio_config(&io_conf);
    gpio_isr_handler_add((gpio_num_t)gpio, gpio_isr_handler, (void *)gpio);
}

void ESP32PlatformPlugin::registerEncoder(int gpioA, int gpioB) {
    this->registeredEncodersList.push_back(std::make_unique<Rotary>(gpioA, gpioB));
}

void ESP32PlatformPlugin::runTask() {
    uint32_t gpioNumber;
    for (;;) {
        if (xQueueReceive(buttonEventQueue, &gpioNumber, portMAX_DELAY)) {
            std::cout << "Received interrupt " << gpioNumber << std::endl;
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
    berry->export_this("register_encoder", this, &ESP32PlatformPlugin::registerEncoder, "gpio");

    buttonEventQueue = xQueueCreate(10, sizeof(uint32_t));
    gpio_install_isr_service(0);
}
