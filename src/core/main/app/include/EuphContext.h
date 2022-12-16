#pragma once

#include <memory>

#include "StorageAccessor.h"
#include "BerryBind.h"

/**
 * @brief The main context of the application.
 * It holds pointers to a core set of utilities that are used through the entire system.
 * 
 * Notably:
 * - buffer -> Instance of CentralAudioBuffer
 * - storage -> Instance of the StorageAccessor, thats should be used to access the filesystem
 * - vm -> Instance of the VM, that should be used to run the scripts
*/
namespace euph {
struct Context {
  std::shared_ptr<euph::StorageAccessor> storage;
  std::shared_ptr<berry::VmState> vm;

  /**
   * @brief Creates a context with the default utilities
   * 
   * @return std::shared_ptr<euph::Context> fresh context
   */
  static std::shared_ptr<euph::Context> create() {
    auto ctx = std::make_shared<euph::Context>();
    ctx->storage = std::make_shared<euph::StorageAccessor>();
    ctx->vm = std::make_shared<berry::VmState>();
    return ctx;
  }

  // Path to the root of the filesystem
  std::string rootPath = "../src/core/fs";
};
}  // namespace euph