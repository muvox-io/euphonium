#pragma once

#include <mutex>
#include <string>
#include "esp_heap_caps.h"

#include "BellTask.h"
#include "BellUtils.h"

#include "EuphLogger.h"
#include "BerryBind.h"

namespace euph {
class MemoryMonitorTask : public bell::Task {
 public:
  MemoryMonitorTask(std::shared_ptr<berry::VmState> berry);
  ~MemoryMonitorTask() {}

  void runTask() override;
  static const unsigned int POLL_INTERVAL = 1000;  // ms
  static const unsigned int KEEP_HISTORY = 60;

  uint32_t internalRamTotal;
  uint32_t psRamTotal;

  std::mutex historyMutex;
  std::vector<uint32_t> internalRamHistory;
  std::vector<uint32_t> psRamHistory;

  size_t internalRamHistoryPos = 0;
  size_t psRamHistoryPos = 0;
  void setupBindings();

  berry::map _getMemoryStats();

 private:
  std::shared_ptr<berry::VmState> berry;
  std::string TAG = "MemoryMonitor";
};
}  // namespace euph
