#pragma once

#include "EuphAudioOutput.h"

namespace euph {

class I2SAudioOutput : public euph::AudioOutput {
 public:
  I2SAudioOutput();
  ~I2SAudioOutput(){};

  void setupBindings(std::shared_ptr<euph::Context> ctx) override;

  bool supportsHardwareVolume() override;
  void configure(uint32_t sampleRate, uint8_t channels,
                 uint8_t bitwidth) override;
  void setVolume(uint8_t volume) override;
  void feedPCM(uint8_t* pcm, size_t size) override;
};
}  // namespace euph
