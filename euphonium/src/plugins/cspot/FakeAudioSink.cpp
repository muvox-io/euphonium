#include "FakeAudioSink.h"

FakeAudioSink::FakeAudioSink(std::shared_ptr<CircularBuffer> buffer)
{
    this->buffer = buffer;
}

void FakeAudioSink::feedPCMFrames(std::vector<uint8_t> &data)
{
    size_t bytesWritten = 0;
    while (bytesWritten < data.size()) {
        bytesWritten += buffer->write(data.data() + bytesWritten, data.size() - bytesWritten);
    }
}
