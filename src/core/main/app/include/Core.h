#pragma once

#include <memory>
#include <string>

#include "EuphLogger.h"

namespace euph {
class Core {
  private:
    std::string TAG = "core";

  public:
    Core();
    ~Core();
};
} // namespace euph