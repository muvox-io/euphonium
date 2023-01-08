#pragma once


#include <memory>
#include "driver/i2c.h"

#include "BellLogger.h"
#include "BerryBind.h"

#define i2c_port 0

void exportI2CDriver(std::shared_ptr<berry::VmState> berry);

