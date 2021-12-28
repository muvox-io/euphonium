#ifndef EUPH_I2CDRIVER_H
#define EUPH_I2CDRIVER_H

#include "BellLogger.h"
#include "driver/i2c.h"
#include <memory>
#include "BerryBind.h"
#define i2c_port 0

void i2cInstall(bool isMaster, int sda, int scl, int clkSpeed);

void i2cDelete();

bool i2cWriteRegValueByte(int address, int reg, int value);

bool i2cWriteRegValueInt16(int address, int reg, int value);

int i2cReadRegValueInt16(int address, int reg);

bool i2cWriteReg(int address, int reg);

int i2cReadReg(int address, int reg);

void exportI2CDriver(std::shared_ptr<berry::VmState> berry);

#endif
