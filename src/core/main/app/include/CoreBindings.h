#pragma once

#include <memory>
#include <fmt/core.h>

#include "BellUtils.h"
#include "EuphContext.h"
#include "EuphLogger.h"

namespace euph {

class CoreBindings {
 public:
  CoreBindings(std::shared_ptr<euph::Context> ctx);
  ~CoreBindings();

  void setupBindings();

  // all the functions starting with '_' are exported to the VM
  void _sleepMS(int ms);
  std::string _getVersion();
  std::string _getPlatform();
  void _loadScript(std::string plg, std::string path);
  long _getTimeMs();

 private:
  const std::string TAG = "core-bindings";
  std::shared_ptr<euph::Context> ctx;
};
}  // namespace euph