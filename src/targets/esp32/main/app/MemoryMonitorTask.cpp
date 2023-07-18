#include "MemoryMonitorTask.h"

using namespace euph;

MemoryMonitorTask::MemoryMonitorTask(std::shared_ptr<berry::VmState> berry)
    : bell::Task("MemoryMonitor", 1024 * 4, 0, 1) {
  this->berry = berry;
  this->internalRamHistory.resize(KEEP_HISTORY);
  this->psRamHistory.resize(KEEP_HISTORY);
  this->internalRamTotal =
      heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  this->psRamTotal =
      heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  startTask();
}

void MemoryMonitorTask::runTask() {
  size_t i = 0;
  while (true) {
    // update history
    uint32_t internalRamUsed =
        this->internalRamTotal -
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint32_t psRamUsed =
        this->psRamTotal -
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // modify history
    {
      std::scoped_lock lock(this->historyMutex);
      this->internalRamHistory[this->internalRamHistoryPos] = internalRamUsed;
      this->psRamHistory[this->psRamHistoryPos] = psRamUsed;
      this->psRamHistoryPos = (this->psRamHistoryPos + 1) % KEEP_HISTORY;
      this->internalRamHistoryPos =
          (this->internalRamHistoryPos + 1) % KEEP_HISTORY;
    }
    if (i % 5 == 0) {

      // print to console
      uint32_t freePsram =
          heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
      uint32_t freeIram =
          heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
      EUPH_LOG(debug, "core", "Free memory: (psram) %d, (internal) %d",
               freePsram, freeIram);
    }
    i++;
    BELL_SLEEP_MS(POLL_INTERVAL);
  }
}

void MemoryMonitorTask::setupBindings() {
  this->berry->export_this("get_memory_stats", this,
                           &MemoryMonitorTask::_getMemoryStats,
                           "memory_monitor");
}

berry::map MemoryMonitorTask::_getMemoryStats() {
  berry::map ret;
  ret["internal_ram_total"] = (int)this->internalRamTotal;
  ret["psram_total"] = (int)this->psRamTotal;
  ret["internal_ram_used"] =
      (int)(this->internalRamTotal -
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  ret["psram_used"] =
      (int)(this->psRamTotal -
            heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  berry::list internalRamHistory;
  for (size_t i = 1; i <= KEEP_HISTORY; i++) {
    internalRamHistory.push_back(
        (int)this
            ->internalRamHistory[(internalRamHistoryPos - i) % KEEP_HISTORY]);
  }
  berry::list psRamHistory;
  for (size_t i = 1; i <= KEEP_HISTORY; i++) {
    psRamHistory.push_back(
        (int)this->psRamHistory[(psRamHistoryPos - i) % KEEP_HISTORY]);
  }
  ret["internal_ram_history"] = internalRamHistory;
  ret["psram_history"] = psRamHistory;
  return ret;
}
