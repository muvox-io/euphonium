#include "FreeRTOSDriver.h"

#include <freertos/task.h>

static berry::list getRuntimeStatistics(int waitTimeMs) {
  berry::list result;
  std::vector<TaskStatus_t> startTaskStatuses, endTaskStatuses;

  // Allocate array to store task states
  startTaskStatuses.resize(uxTaskGetNumberOfTasks() + 5);
  uint32_t startRunTime, endRunTime;
  uint32_t totalTime;

  // Get current task states
  size_t tasks = uxTaskGetSystemState(
      startTaskStatuses.data(), startTaskStatuses.size() * sizeof(TaskStatus_t),
      &startRunTime);

  if (tasks == 0) {
    // Error
    return result;
  }

  vTaskDelay(pdMS_TO_TICKS(waitTimeMs));

  endTaskStatuses.resize(uxTaskGetNumberOfTasks() + 5);

  tasks = uxTaskGetSystemState(endTaskStatuses.data(),
                               endTaskStatuses.size() * sizeof(TaskStatus_t),
                               &endRunTime);

  // Calculate total_elapsed_time in units of run time stats clock period.
  totalTime = (endRunTime - startRunTime);
  if (totalTime == 0) {
    // Err
    return result;
  }

  for (auto& startTask : startTaskStatuses) {
    // Match task handles
    if (startTask.xHandle == NULL) {
      continue;
    }

    auto endTask = std::find_if(endTaskStatuses.begin(), endTaskStatuses.end(),
                                [&startTask](const TaskStatus_t& task) {
                                  return task.xHandle == startTask.xHandle;
                                });
    if (endTask == endTaskStatuses.end() ||
        endTask->xHandle != startTask.xHandle) {
      continue;
    }

    int taskElapsedTime =
        (endTask->ulRunTimeCounter - startTask.ulRunTimeCounter);
    // Task matched, add stats
    result.push_back(berry::map{
        {"name", std::string(startTask.pcTaskName)},
        {"runtime", taskElapsedTime},
        {"core", (int)endTask->xCoreID},
        {
            "percentage",
            (int)((int)(taskElapsedTime * 100UL) /
                  (totalTime * portNUM_PROCESSORS)),
        },
    });
  }

  // Sort task list by percentage
  std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
    auto left = std::any_cast<berry::map>(lhs);
    auto right = std::any_cast<berry::map>(rhs);

    return std::any_cast<int>(left["percentage"]) >
           std::any_cast<int>(right["percentage"]);
  });
  return result;
}

void exportFreeRTOSDriver(std::shared_ptr<berry::VmState> berry) {
  berry->export_function("get_task_stats", &getRuntimeStatistics, "devtools");
}
