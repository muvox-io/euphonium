#include "GPIODriver.h"

void gpioDigitalWrite(int pin, int value) {
  gpio_set_level((gpio_num_t)pin, value);
}

int gpioDigitalRead(int pin) {
  return gpio_get_level((gpio_num_t)pin);
}

// @TODO
int gpioAnalogRead(int pin) {
  return 0;
}

void gpioPinMode(int pin, int mode) {
  gpio_reset_pin((gpio_num_t)pin);
  switch (mode) {
    case 1:
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
      break;
    case 2:
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
      gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLUP_ONLY);
      break;
    case 3:
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
      gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLDOWN_ONLY);
      break;
    case 4:
      gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
      break;
  }
}

void exportGPIODriver(std::shared_ptr<berry::VmState> berry) {
  berry->export_function("digital_write", &gpioDigitalWrite, "gpio");
  berry->export_function("digital_read", &gpioDigitalRead, "gpio");
  berry->export_function("pin_mode", &gpioPinMode, "gpio");
  berry->export_function("analog_read", &gpioAnalogRead, "gpio");
}
