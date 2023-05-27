#include "ESP32Platform.h"
#include "FreeRTOSDriver.h"

void euph::exportDrivers(std::shared_ptr<berry::VmState> vm) {
  // Export necessary drivers
  exportI2CDriver(vm);
  exportGPIODriver(vm);
  exportFreeRTOSDriver(vm);
}
