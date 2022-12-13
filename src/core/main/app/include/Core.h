#pragma once

#include <memory>
#include <string>

#include "EuphContext.h"
#include "EuphLogger.h"

namespace euph {
class Core {
 private:
  // TAG for logging
  std::string TAG = "core";

  std::shared_ptr<euph::Context> ctx;

 public:
  Core();
  ~Core();
};
}  // namespace euph