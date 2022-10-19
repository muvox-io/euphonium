#ifndef EUPH_LEDDRIVER_H
#define EUPH_LEDDRIVER_H

#include "BellLogger.h"
#include <memory>
#include <map>
#include "BerryBind.h"
#include "LedStrip.h"

extern std::map<uint8_t, std::unique_ptr<LedStrip>> registeredStrips;

void exportLEDDriver(std::shared_ptr<berry::VmState> berry);

#endif
