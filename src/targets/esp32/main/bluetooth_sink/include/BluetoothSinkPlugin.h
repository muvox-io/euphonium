#pragma once

#include <atomic>
#include <mutex>
// for std::hash
#include <functional>
#include <string>
#include <utility>

#include "BellTask.h"

#include "A2DPDriver.h"
#include "AudioSourcePlugin.h"
#include "CoreEvents.h"
#include "EuphContext.h"
#include "EuphLogger.h"

namespace euph {
class BluetoothSinkPlugin : public AudioSourcePlugin, public bell::Task {
 public:
  BluetoothSinkPlugin(std::shared_ptr<euph::Context> ctx);
  ~BluetoothSinkPlugin();

  // Event used to notify the scripting language of metadata changes
  class MetadataEvent : public Event {
  public:
    A2DPDriver::PlaybackMetadata metadata;
    
    MetadataEvent(A2DPDriver::PlaybackMetadata metadata) {
      this->eventType = EventType::VM_MAIN_EVENT;
      this->subType = "bluetooth_metadata";
      this->metadata = metadata;
    }

    berry::map toBerry() override {
      berry::map result;

      // Map to berry
      result["artist"] = metadata.artist;
      result["title"] = metadata.title;
      result["album"] = metadata.album;
      result["duration"] = metadata.duration;

      return result;
    }
  };

  void runPlugin(std::string name);

  // --- AudioSourcePlugin implementation
  void initializeBindings() override;
  void shutdownAudio() override;
  std::string getName() override;

  // -- BellTask implementation
  void runTask() override;

 private:
  std::unique_ptr<euph::A2DPDriver> a2dpDriver;
  std::shared_ptr<euph::Context> ctx;

  std::atomic<bool> isRunning = false;
  std::atomic<bool> canPlay = false;
  int lastVolume = 0;
  std::mutex runningMutex;
};

}  // namespace euph
