#pragma once

#include <memory>

#include "driver/gpio.h"

#include "BellLogger.h"
#include "BerryBind.h"

#define GP_INPUT           GPIO_MODE_INPUT|(GPIO_FLOATING << 8)
#define GP_INPUT_PULLUP    GPIO_MODE_INPUT|(GPIO_PULLUP_ONLY << 8)
#define GP_INPUT_PULLDOWN  GPIO_MODE_INPUT|(GPIO_PULLDOWN_ONLY << 8)
#define GP_OUTPUT          GPIO_MODE_OUTPUT|(GPIO_FLOATING << 8)

void gpioDigitalWrite(int pin, int value);

int gpioDigitalRead(int pin);

int gpioAnalogRead(int pin);

void gpioPinMode(int pin, int mode);

void exportGPIODriver(std::shared_ptr<berry::VmState> berry);

