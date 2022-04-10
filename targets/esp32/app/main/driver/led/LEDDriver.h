#ifndef EUPH_LEDDRIVER_H
#define EUPH_LEDDRIVER_H

#include "BellLogger.h"
#include <memory>
#include "BerryBind.h"
#include "led_strip.h"

void exportLEDDriver(std::shared_ptr<berry::VmState> berry);

#endif
