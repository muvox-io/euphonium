#include "I2SAudioOutput.h"

using namespace euph;

I2SAudioOutput::I2SAudioOutput() {};

void I2SAudioOutput::setupBindings(std::shared_ptr<euph::Context> ctx) {

}

bool I2SAudioOutput::supportsHardwareVolume() {
  return true;
}

void I2SAudioOutput::configure(uint32_t sampleRate, uint8_t channels, uint8_t bitwidth) {

}

void I2SAudioOutput::setVolume(uint8_t volume) {

}

void I2SAudioOutput::feedPCM(uint8_t *pcm, size_t size) {

}
