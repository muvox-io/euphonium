#include "ServiceTask.h"

#include "EuphLogger.h"

using namespace euph;

void ServiceJob::reportProgress(std::shared_ptr<euph::Context> ctx,
                                float progress) {

  EUPH_LOG(info, jobTypeName(), "Progress: %f", progress);
  // TODO: broadcast event bus messages
}

ServiceTask::ServiceTask(std::weak_ptr<euph::Context> ctx)
    : bell::Task("ServiceTask", 1024 * 16, 1, 0), ctx(ctx), jobSemaphore(0) {}

void ServiceTask::runTask() {
  while (true) {
    jobSemaphore.acquire();
    busyMutex.lock();
    if (currentJob) {
      try {
        EUPH_LOG(info, TAG, "Running job %s",
                 currentJob->jobTypeName().c_str());
        currentJob->run(ctx.lock());
        if (currentJob->nextJob) {
          submitJob(std::move(currentJob->nextJob));
        }
      } catch (ServiceJobFailedException& e) {
        EUPH_LOG(error, TAG, "Job failed: %s", e.what());
        // TODO: broadcast event bus messages
      }
    }
    busyMutex.unlock();
  }
}

bool ServiceTask::submitJob(std::unique_ptr<ServiceJob> job) {
  if (busyMutex.try_lock()) {
    currentJob = std::move(job);
    busyMutex.unlock();
    jobSemaphore.release();
    return true;
  }
  return false;
}
