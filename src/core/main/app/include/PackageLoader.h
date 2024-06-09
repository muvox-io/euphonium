#pragma once

#include <ghc_filesystem.h>
#include <exception>
#include <fstream>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include "EuphContext.h"

namespace euph {

class PackageLoaderFileNotFoundException : public std::exception {
 public:
  PackageLoaderFileNotFoundException(const std::string& message)
      : message(message) {}

  const char* what() const noexcept override { return message.c_str(); }

 private:
  std::string message;
};

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
   * @throws PackageLoaderFileNotFoundException if the package file is not found
   *
   * @param hook the hook to load, one of the 'validHooks' vector
   * @param packageRequired if true, throws an exception if zero packages for this hook were loaded
   */
  void loadWithHook(const std::string& hook, bool packageRequired = false);

 private:
  const std::string TAG = "package-loader";
  std::shared_ptr<euph::Context> ctx;
  std::vector<PackageInfo> packages;

  std::vector<std::string> validHooks = {"system", "plugin"};
};
}  // namespace euph
