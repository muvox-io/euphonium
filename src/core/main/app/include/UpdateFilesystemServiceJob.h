#pragma once

#include <string>
#include "ServiceTask.h"

namespace euph {

/**
 * @brief Unpacks a tar archive with a filesystem OTA update.
 * 
 */
class UpdateFilesystemServiceJob : public euph::ServiceJob {
 public:
  UpdateFilesystemServiceJob(std::string archivePath);

  virtual std::string jobTypeName() override;
  virtual void run(std::shared_ptr<euph::Context> ctx) override;

 private:
  std::string archivePath;
};

}  // namespace euph
