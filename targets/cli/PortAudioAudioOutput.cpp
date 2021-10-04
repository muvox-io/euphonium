#include "PortAudioAudioOutput.h"

PortAudioAudioOutput::PortAudioAudioOutput() {

}

void PortAudioAudioOutput::update(std::shared_ptr<CircularBuffer> audioBuffer) {
    auto buffer = std::vector<uint8_t>(4096);
    auto readNumber = audioBuffer->read(buffer.data(), 4096);
    std::cout << "read " << readNumber << std::endl;
}