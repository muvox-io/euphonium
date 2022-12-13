#pragma once

#include <memory>
#include <string>

#include "BellUtils.h"

#include "HTTPDispatcher.h"
#include "StorageAccessor.h"
#include "EuphContext.h"
#include "EuphLogger.h"

namespace euph {
class Core {
 private:
  // TAG for logging
  std::string TAG = "core";

  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<euph::HTTPDispatcher> http;

 public:
  Core();
  ~Core();
};
}  // namespace euph