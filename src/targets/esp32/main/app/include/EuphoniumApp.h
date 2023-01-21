#pragma once

#include <memory>
#include "esp_littlefs.h"
#include "esp_vfs.h"

#include "BellTask.h"
#include "BellUtils.h"

#include "Core.h"
#include "ESP32Connectivity.h"
#include "EuphLogger.h"
#include "ESP32Platform.h"
#include "BluetoothSinkPlugin.h"
#include "MemoryMonitorTask.h"
#include "StatusLED.h"
#include "I2SAudioOutput.h"

namespace euph {
class EuphoniumApp : public bell::Task {
 public:
  EuphoniumApp();
  ~EuphoniumApp(){};

  void runTask() override;

 private:
  void initializeStorage();
};
}  // namespace euph
