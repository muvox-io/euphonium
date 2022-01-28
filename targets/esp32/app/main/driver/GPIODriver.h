#ifndef EUPH_GPIODRIVER_H
#define EUPH_GPIODRIVER_H

#include "BellLogger.h"
#include "driver/gpio.h"
#include <memory>
#include "BerryBind.h"

void gpioDigitalWrite(int gpio, int value);

int gpioDigitalRead(int pin);

int gpioAnalogRead(int pin);

void gpioPinMode(int pin);

void exportGPIODriver(std::shared_ptr<berry::VmState> berry);

#endif
