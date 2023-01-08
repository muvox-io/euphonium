#include "ESP32Platform.h"

using namespace euph;

void exportDrivers(std::shared_ptr<berry::VmState> vm) {
  // Export necessary drivers
  exportI2CDriver(vm);
  exportGPIODriver(vm);
}
