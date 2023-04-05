#pragma once

// forward declaration of Context
namespace euph {
struct Context;
class Connectivity;
}

#include <memory>

#include "CentralAudioBuffer.h"

#include "BerryBind.h"
#include "Connectivity.h"
#include "EventBus.h"
#include "StorageAccessor.h"

/**
 * @brief The main context of the application.
 * It holds pointers to a core set of utilities that are used through the entire system.
 * 
 * Notably:
 * - buffer -> Instance of CentralAudioBuffer
 * - storage -> Instance of the StorageAccessor, thats should be used to access the filesystem
 * - vm -> Instance of the VM, that should be used to run the scripts
 * - eventBus -> Instance of the EventBus, that should be used to communicate between the different parts of the system
*/
namespace euph {
// Used to control the state of playback. @TODO: Extend with loading state, error state, and such
struct PlaybackController {
  std::atomic<bool> isPaused = false;
  std::atomic<bool> requestPause = false;
  std::mutex playbackAccessMutex;
  int currentVolume = 0;

  std::function<void(const std::string&)> playbackLockedHandler;

  void pause() { this->requestPause = true; }

  void play() { this->isPaused = false; }

  void lockPlayback(const std::string& source) {
    if (playbackLockedHandler != NULL) {
      playbackLockedHandler(source);
    }

    playbackAccessMutex.lock();
  }

  void unlockPlayback() { playbackAccessMutex.unlock(); }
};

struct Context {
  std::shared_ptr<euph::StorageAccessor> storage;
  std::shared_ptr<berry::VmState> vm;
  std::shared_ptr<euph::EventBus> eventBus;
  std::shared_ptr<bell::CentralAudioBuffer> audioBuffer;
  std::shared_ptr<euph::PlaybackController> playbackController;
  std::shared_ptr<euph::Connectivity> connectivity;

  // Display name of the device, gets replaced by user setting later on
  std::string displayName = "Euphonium";

  /**
   * @brief Creates a context with the default utilities
   * 
   * @return std::shared_ptr<euph::Context> fresh context
   */
  static std::shared_ptr<euph::Context> create() {
    auto ctx = std::make_shared<euph::Context>();
    ctx->storage = std::make_shared<euph::StorageAccessor>();
    ctx->vm = std::make_shared<berry::VmState>();
    ctx->eventBus = std::make_shared<euph::EventBus>();
    ctx->audioBuffer = std::make_shared<bell::CentralAudioBuffer>(128);
    return ctx;
  }

  static std::shared_ptr<euph::Context> createWithBus(
      std::shared_ptr<euph::EventBus> bus) {
    auto ctx = std::make_shared<euph::Context>();
    ctx->storage = std::make_shared<euph::StorageAccessor>();
    ctx->vm = std::make_shared<berry::VmState>();
    ctx->audioBuffer = std::make_shared<bell::CentralAudioBuffer>(128);
    ctx->playbackController = std::make_shared<euph::PlaybackController>();
    ctx->eventBus = bus;

#ifdef ESP_PLATFORM
    ctx->rootPath = "/fs";
#endif
    return ctx;
  }

  // Path to the root of the filesystem
  std::string rootPath = "../../../core/fs";
};
}  // namespace euph
