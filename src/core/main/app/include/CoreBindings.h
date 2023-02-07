#pragma once

#include <memory>
#include <chrono>
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
  void _loadScript(std::string pkg, std::string path);
  std::string _loadConfig(std::string pkg);
  uint64_t _getTimestamp();
  bool _saveConfig(std::string pkg, std::string cfg);
  void _confirmOnboarding();
  void _triggerPause(bool isPaused);
  std::string _getMac();
  long _getTimeMs();

 private:
  const std::string TAG = "core-bindings";
  std::shared_ptr<euph::Context> ctx;
};
}  // namespace euph
