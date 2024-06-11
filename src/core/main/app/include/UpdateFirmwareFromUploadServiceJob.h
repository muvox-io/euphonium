#pragma once

#include <semaphore>
#include <string>
#include "ServiceTask.h"
#include "civetweb.h"

namespace euph {

/**
 * @brief Updates the firmware image from an uploaded file using a http POST request.
 */
class UpdateFirmwareFromUploadServiceJob : public euph::ServiceJob {
 public:
  UpdateFirmwareFromUploadServiceJob(mg_connection* conn,
                                     std::binary_semaphore& done);

  virtual std::string jobTypeName() override;
  virtual void run(std::shared_ptr<euph::Context> ctx) override;

  std::binary_semaphore& done;

 private:
  mg_connection* conn;
};

}  // namespace euph
