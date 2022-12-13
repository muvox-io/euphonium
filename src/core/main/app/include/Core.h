#pragma once

#include <memory>
#include <string>

#include "BellLogger.h"

namespace euph {
class Core {
  private:
    std::string TAG = "core";

  public:
    Core();
    ~Core();
};
} // namespace euph