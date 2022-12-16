#pragma once

#include <fmt/core.h>
#include <fstream>
#include <memory>

#include <nlohmann/json.hpp>
#include "EuphContext.h"

namespace euph {
class PackageLoader {
 public:
  PackageLoader(std::shared_ptr<euph::Context> ctx);

  struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string initHook;
  };

  /**
   * @brief Loads all the packages from the file system
   */
  void loadValidPackages();


  /**
   * @brief Loads a package under a given hook into the VM
   * 
   * @param hook the hook to load, one of the 'validHooks' vector
   */
  void loadWithHook(const std::string& hook);

 private:
  const std::string TAG = "package-loader";
  std::shared_ptr<euph::Context> ctx;
  std::vector<PackageInfo> packages;
  
  std::vector<std::string> validHooks = {"system", "plugin"};
};
}  // namespace euph