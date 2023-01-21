#include "MemoryMonitorTask.h"

using namespace euph;

MemoryMonitorTask::MemoryMonitorTask()
    : bell::Task("MemoryMonitor", 1024 * 2, 0, 1) {
  startTask();
}

void MemoryMonitorTask::runTask() {
  while (true) {
    size_t memoryPSRam =
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    auto memoryIRam =
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    EUPH_LOG(debug, "core", "Free memory: (psram) %d, (internal) %d",
             memoryPSRam, memoryIRam);
    BELL_SLEEP_MS(5000);
  }
}
