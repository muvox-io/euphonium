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
IRAM_ATTR std::vector<Rotary> registeredEncodersList;

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t gpio = (uint32_t)arg;

    // Find all encoders that might be related to the pin
    for (auto &encoder : registeredEncodersList) {
        if (encoder.pin1 == gpio || encoder.pin2 == gpio) {
            // Encoder found, process greycode
            auto encoderState = encoder.process();
            if (encoderState == DIR_CW) {
                InterruptEvent interruptEvent = {
                    .buttonEventType = ButtonEventType::ENCODER_CW,
                    .gpioNumber = gpio};

                xQueueSendFromISR(buttonEventQueue, &interruptEvent, NULL);
                return;
            }

            if (encoderState == DIR_CCW) {
                InterruptEvent interruptEvent = {
                    .buttonEventType = ButtonEventType::ENCODER_CCW,
                    .gpioNumber = gpio};

                xQueueSendFromISR(buttonEventQueue, &interruptEvent, NULL);
                return;
            }

            return;
        }
    }
    InterruptEvent interruptEvent = {.buttonEventType = ButtonEventType::BUTTON,
                                     .gpioNumber = gpio};

    xQueueSendFromISR(buttonEventQueue, &interruptEvent, NULL);
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

void ESP32PlatformPlugin::registerButtonRaw(int gpio) {
    rawButtonList.push_back(gpio);
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    io_conf.pull_up_en = (gpio_pullup_t)1;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)1;

    uint64_t pinBitMask = (1ULL << rawButtonList[0]);

    for (int x = 1; x < rawButtonList.size(); x++) {
        pinBitMask = pinBitMask | (1ULL << rawButtonList[x]);
    }
    io_conf.pin_bit_mask = pinBitMask;

    gpio_config(&io_conf);
    gpio_isr_handler_add((gpio_num_t)gpio, gpio_isr_handler, (void *)gpio);
}

void ESP32PlatformPlugin::registerButton(int gpio) {
    buttonList.push_back(gpio);
    std::cout << "Registering button " << gpio << std::endl;
    registerButtonRaw(gpio);
}

void ESP32PlatformPlugin::registerEncoder(int gpioA, int gpioB) {
    this->registerButtonRaw(gpioA);
    this->registerButtonRaw(gpioB);
    std::cout << "Registering encoder " << gpioA << " " << gpioB << std::endl;
    registeredEncodersList.push_back(Rotary(gpioA, gpioB));
}

void ESP32PlatformPlugin::runTask() {
    InterruptEvent interruptEvent;
    for (;;) {
        if (xQueueReceive(buttonEventQueue, &interruptEvent, portMAX_DELAY)) {
            std::string eventType = "";
            if (interruptEvent.buttonEventType == ButtonEventType::BUTTON) {
                if (gpio_get_level((gpio_num_t)interruptEvent.gpioNumber)) {
                    eventType = "button_high";
                } else {
                    eventType = "button_low";
                }
            } else if (interruptEvent.buttonEventType ==
                       ButtonEventType::ENCODER_CW) {
                eventType = "encoder_cw";
            } else if (interruptEvent.buttonEventType ==
                       ButtonEventType::ENCODER_CCW) {
                eventType = "encoder_ccw";
            }

            auto event = std::make_unique<ButtonInteractionEvent>(
                eventType, interruptEvent.gpioNumber);
            mainEventBus->postEvent(std::move(event));
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
    berry->export_this("register_encoder", this,
                       &ESP32PlatformPlugin::registerEncoder, "gpio");

    buttonEventQueue = xQueueCreate(10, sizeof(InterruptEvent));
    gpio_install_isr_service(0);
}
