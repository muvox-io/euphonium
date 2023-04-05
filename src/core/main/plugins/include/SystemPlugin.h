#pragma once

#include <string>

namespace euph {
class SystemPlugin {
 public:
  SystemPlugin() = default;
  ~SystemPlugin(){};

  // Export's plugins bindings to the VM
  virtual void initializeBindings() = 0;

  // Return's its name
  virtual std::string getName() = 0;
};
}  // namespace euph
