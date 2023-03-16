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
#include "ManufacuringShell.h"

namespace euph {
class EuphoniumApp : public bell::Task {
 public:
  EuphoniumApp();
  ~EuphoniumApp(){};

  void runTask() override;

 private:
  void printBuildInfo();
  void initializeStorage();
  std::shared_ptr<euph::ESP32Connectivity> connectivity;
  std::shared_ptr<euph::EventBus> eventBus;
  std::shared_ptr<euph::I2SAudioOutput> audioOutput;
  std::shared_ptr<euph::StatusLED> statusTask;
  std::shared_ptr<euph::MemoryMonitorTask> memoryMonitor;
  std::shared_ptr<euph::ManufacuringShell> manufacuringShell;

  std::unique_ptr<euph::Core> core;
};
}  // namespace euph
