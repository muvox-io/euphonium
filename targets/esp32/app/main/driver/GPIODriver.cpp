#include "GPIODriver.h"

void gpioDigitalWrite(int pin, int value) {
    gpio_set_level((gpio_num_t) pin, value);
}

int gpioDigitalRead(int pin) {
    return gpio_get_level((gpio_num_t) pin);
}

// @TODO
int gpioAnalogRead(int pin) {
    return 0;
}

void gpioPinMode(int pin, int mode) {
    int selectedMode = 0;
    switch (mode) {
    case 1:
        selectedMode = GP_INPUT;
        break;
    case 2:
        selectedMode = GP_INPUT_PULLUP;
        break;
    case 3:
        selectedMode = GP_INPUT_PULLDOWN;
        break;
    case 4:
        selectedMode = GP_OUTPUT;
        break;
    }

    gpio_set_direction((gpio_num_t)pin, (gpio_mode_t)(selectedMode & 0xFF));
    gpio_set_pull_mode((gpio_num_t)pin, (gpio_pull_mode_t)(selectedMode >> 8));
}

void exportGPIODriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("digital_write", &gpioDigitalWrite, "gpio");
    berry->export_function("digital_read", &gpioDigitalRead, "gpio");
    berry->export_function("pin_mode", &gpioPinMode, "gpio");
    berry->export_function("analog_read", &gpioAnalogRead, "gpio");
}