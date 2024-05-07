#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <semaphore>
#include <string>

#include "BellTask.h"
#include "EuphContext.h"

namespace euph {

class ServiceTask;
class Context;

class ServiceJob {
 public:
  virtual ~ServiceJob() = default;

  /**
   * @brief Perform the job.
   * 
   * @param ctx 
   */
  virtual void run(std::shared_ptr<euph::Context> ctx) = 0;

  /**
   * @brief Unique identifier for the job.
   */
  virtual std::string jobTypeName() = 0;

  /**
   * @brief Return whether the WebUI should be blocked by an uncancellable 
   * loading screen.
   * The return value shall not change during the lifetime of the job.
   */
  virtual bool isBlockingUi() { return false; }

  /**
   * @brief Pointer to a job to be executed after this one finishes successfully.
   * 
   */
  std::unique_ptr<ServiceJob> nextJob = nullptr;

 protected:
  /**
  * @brief To be called by the job to report progress.
  * 
  * @param ctx Context pointer passed to the job.
  * @param progress Progress value between 0 and 1.
  */
  void reportProgress(std::shared_ptr<euph::Context> ctx, float progress);
};

class ServiceJobFailedException : public std::exception {
 public:
  ServiceJobFailedException(std::string message) : message(message) {}

  const char* what() const noexcept override { return message.c_str(); }

 private:
  std::string message;
};

/**
 * @brief Performs background operations.
 * They can access the flash directly. 
 *
 * Currently used for OTA.
 * 
 */
class ServiceTask : public bell::Task {
 public:
  ServiceTask(std::weak_ptr<euph::Context> ctx);

  void runTask() override;

  /**
   * @brief Run a job in the service task.
   * 
   * @param job Pointer to the job to be executed. It will be destroyed after the job is done.
   * @return true The job was successfully started.
   * @return false The job could not be started, because the service task is already busy.
   */
  bool submitJob(std::unique_ptr<ServiceJob> job);

 private:
  const char* TAG = "ServiceTask";
  std::weak_ptr<euph::Context> ctx;
  std::unique_ptr<ServiceJob> currentJob;
  std::mutex busyMutex;
  std::binary_semaphore jobSemaphore;
};

}  // namespace euph
