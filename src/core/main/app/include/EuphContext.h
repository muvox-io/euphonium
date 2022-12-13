#pragma once

#include <memory>

#include "StorageAccessor.h"

/**
 * @brief The main context of the application.
 * It holds pointers to a core set of utilities that are used through the entire system.
 * 
 * Notably:
 * - buffer -> Instance of CentralAudioBuffer
 * - storage -> Instance of the StorageAccessor, thats should be used to access the filesystem
*/
namespace euph {
struct Context {
  std::shared_ptr<euph::StorageAccessor> storage;
};
}  // namespace euph