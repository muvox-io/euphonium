#pragma once

#include <string>

namespace euph {
class AudioSourcePlugin {
 public:
  AudioSourcePlugin() = default;
  ~AudioSourcePlugin(){};

  bool supportsContextPlayback = false;

  // Export's plugins bindings to the VM
  virtual void initializeBindings() = 0;

  // If playing, unlocks the audio buffer
  virtual void shutdownAudio() = 0;

  // Return's its name
  virtual std::string getName() = 0;

  virtual void queryContextURI(std::string uri) {};
};
}  // namespace euph
