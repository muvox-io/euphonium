#include "ESP32Platform.h"

void euph::exportDrivers(std::shared_ptr<berry::VmState> vm) {
  // Export necessary drivers
  exportI2CDriver(vm);
  exportGPIODriver(vm);
}
