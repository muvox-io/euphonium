#ifndef EUPH_GPIODRIVER_H
#define EUPH_GPIODRIVER_H

#include "BellLogger.h"
#include "driver/gpio.h"
#include <memory>
#include "BerryBind.h"

void gpioSetState(int gpio, int state);


void exportGPIODriver(std::shared_ptr<berry::VmState> berry);

#endif
