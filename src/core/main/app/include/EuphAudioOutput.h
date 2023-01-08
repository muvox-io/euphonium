#pragma once

#include <memory>

#include "EuphContext.h"

namespace euph {
class AudioOutput {
public:
  AudioOutput() {};
  ~AudioOutput() {};

  virtual void setupBindings(std::shared_ptr<euph::Context> ctx) = 0;

  virtual bool supportsHardwareVolume() = 0;
  virtual void configure(uint32_t sampleRate, uint8_t channels, uint8_t bitwidth) = 0;
  virtual void setVolume(uint8_t volume) = 0;
  virtual void feedPCM(uint8_t* pcm, size_t size) = 0;
};
}
