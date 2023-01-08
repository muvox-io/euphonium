#include "DesktopAudioOutput.h"

using namespace euph;

euph::DesktopAudioOutput::DesktopAudioOutput() {
  this->audioSink = std::make_unique<PortAudioSink>();
}

void DesktopAudioOutput::setupBindings(std::shared_ptr<euph::Context> ctx) {

}

bool DesktopAudioOutput::supportsHardwareVolume() {
  return true;
}

void DesktopAudioOutput::configure(uint32_t sampleRate, uint8_t channels, uint8_t bitwidth) {
  this->audioSink->setParams(sampleRate, 2, 16);
}

void DesktopAudioOutput::setVolume(uint8_t volume) {

}

void DesktopAudioOutput::feedPCM(uint8_t *pcm, size_t size) {
  this->audioSink->feedPCMFrames(pcm, size);
}
