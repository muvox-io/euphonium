#ifndef EUPH_I2CDRIVER_H
#define EUPH_I2CDRIVER_H

#include "BellLogger.h"
#include "driver/i2c.h"
#include <memory>
#include "BerryBind.h"
#define i2c_port 0

void exportI2CDriver(std::shared_ptr<berry::VmState> berry);

#endif
