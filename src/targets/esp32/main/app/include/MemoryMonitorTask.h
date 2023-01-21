#pragma once

#include <string>
#include "esp_heap_caps.h"

#include "BellTask.h"
#include "BellUtils.h"

#include "EuphLogger.h"

namespace euph {
class MemoryMonitorTask : public bell::Task {
public:
  MemoryMonitorTask();
  ~MemoryMonitorTask() {}

  void runTask() override;
private:
  std::string TAG = "MemoryMonitor";

};
}
