#ifndef EUPH_I2SDRIVER_H
#define EUPH_I2SDRIVER_H
#include "driver/i2s.h"
#include <driver/gpio.h>
#include <memory>
#include "BerryBind.h"

void i2sInstall(int channelFormatInt, int commFormat, int sampleRate, bool autoClear, int bck, int ws, int dataOut);

void i2sDelete();

void i2sEnableMCLK();

void exportI2SDriver(std::shared_ptr<berry::VmState> berry);

#endif
