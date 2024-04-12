#pragma once

// forward declaration of Context
#include "CoreEvents.h"
#include "EmergencyMode.h"
#include "WrappedSemaphore.h"
namespace euph {
struct Context;
class Connectivity;
}  // namespace euph

#include <memory>

#include "CentralAudioBuffer.h"

#include "BerryBind.h"
#include "Connectivity.h"
#include "EventBus.h"
#include "EmergencyMode.h"

/**
 * @brief The main context of the application.
 * It holds pointers to a core set of utilities that are used through the entire system.
 * 
 * Notably:
 * - buffer -> Instance of CentralAudioBuffer
 * - vm -> Instance of the VM, that should be used to run the scripts
 * - eventBus -> Instance of the EventBus, that should be used to communicate between the different parts of the system
*/
namespace euph {
// Used to control the state of playback. @TODO: Extend with loading state, error state, and such
struct PlaybackController {
  // Loosely bound from ctx, so that we can send out playback state events
  std::shared_ptr<euph::EventBus> eventBus;

  PlaybackStateEvent::State state = PlaybackStateEvent::State::STOPPED;

  std::atomic<bool> isPaused = false;
  std::atomic<bool> requestPause = false;
  std::atomic<bool> isLoading = false;
  std::atomic<bool> isLocked = false;

  std::unique_ptr<bell::WrappedSemaphore> playbackAccessSemaphore;
  int currentVolume = 0;

  std::function<void(const std::string&)> playbackLockedHandler;

  void setState(PlaybackStateEvent::State state) {
    this->state = state;

    // Send out event
    this->eventBus->postEvent(std::make_unique<PlaybackStateEvent>(state));
  }

  void pause() {
    if (!this->isPaused) {
      this->requestPause = true;
    }

    setState(PlaybackStateEvent::State::PAUSED);
  }

  void play() {
    // Always cancel incoming pause request
    this->requestPause = false;

    if (this->isPaused) {
      this->isPaused = false;
    }

    setState(PlaybackStateEvent::State::PLAYING);
  }

  void setLoading() {
    setState(PlaybackStateEvent::State::LOADING);
  }

  void setStopped() {
    setState(PlaybackStateEvent::State::STOPPED);
  }

  void lockPlayback(const std::string& source) {
    if (isLocked) return;
    playbackAccessSemaphore->wait();

    isLocked = true;

    this->play();

    if (playbackLockedHandler != NULL) {
      playbackLockedHandler(source);
    }
  }

  void unlockPlayback() {
    if (!isLocked) return;
    isLocked = false;
    playbackAccessSemaphore->give();
  }
};

struct Context {
  std::shared_ptr<berry::VmState> vm;
  std::shared_ptr<euph::EventBus> eventBus;
  std::shared_ptr<bell::CentralAudioBuffer> audioBuffer;
  std::shared_ptr<euph::PlaybackController> playbackController;
  std::shared_ptr<euph::Connectivity> connectivity;
  std::shared_ptr<euph::EmergencyMode> emergencyMode;

  // Display name of the device, gets replaced by user setting later on
  std::string displayName = "Euphonium";

  /**
   * @brief Creates a context with the default utilities
   * 
   * @return std::shared_ptr<euph::Context> fresh context
   */
  static std::shared_ptr<euph::Context> create() {
    auto ctx = std::make_shared<euph::Context>();
    ctx->vm = std::make_shared<berry::VmState>();
    ctx->eventBus = std::make_shared<euph::EventBus>();
    ctx->audioBuffer = std::make_shared<bell::CentralAudioBuffer>(128);
    ctx->playbackController->eventBus = ctx->eventBus;
    ctx->playbackController->playbackAccessSemaphore = std::make_unique<bell::WrappedSemaphore>(1);
    ctx->playbackController->playbackAccessSemaphore->give();
    ctx->emergencyMode = std::make_shared<euph::EmergencyMode>();
    return ctx;
  }

  static std::shared_ptr<euph::Context> createWithBus(
      std::shared_ptr<euph::EventBus> bus) {
    auto ctx = std::make_shared<euph::Context>();
    ctx->vm = std::make_shared<berry::VmState>();
    ctx->audioBuffer = std::make_shared<bell::CentralAudioBuffer>(128);
    ctx->playbackController = std::make_shared<euph::PlaybackController>();
    ctx->playbackController->eventBus = bus;
    ctx->playbackController->playbackAccessSemaphore = std::make_unique<bell::WrappedSemaphore>(1);
    ctx->playbackController->playbackAccessSemaphore->give();
    ctx->eventBus = bus;
    ctx->emergencyMode = std::make_shared<euph::EmergencyMode>();

#ifdef ESP_PLATFORM
    ctx->rootPath = "/fs";
#endif
    return ctx;
  }

  // Path to the root of the filesystem
  std::string rootPath = "../../../core/fs";
};
}  // namespace euph
