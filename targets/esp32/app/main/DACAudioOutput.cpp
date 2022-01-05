#include "DACAudioOutput.h"

DACAudioOutput::DACAudioOutput()
{
}

DACAudioOutput::~DACAudioOutput()
{
}

void DACAudioOutput::setupBindings(std::shared_ptr<berry::VmState> berry) {
    berry->export_this("dac_set_readable", this, &DACAudioOutput::setReadable);
}

void DACAudioOutput::setReadable(bool readable) {
    this->isReading = readable;
    std::scoped_lock lock(this->readingMutex);
}


void DACAudioOutput::feedPCMFrames(uint8_t* data, size_t nBytes) {
    size_t written = 0;
    while (written < nBytes)
    {
        if (isReading) {
            std::scoped_lock lock(readingMutex);
            i2s_write((i2s_port_t)0, data+written, nBytes - written, &written, portMAX_DELAY);
        } else {
            BELL_SLEEP_MS(10);
        }
    }
}

void DACAudioOutput::configureOutput(SampleFormat format, int sampleRate) {
    setReadable(false);
    i2s_set_sample_rates((i2s_port_t) 0, sampleRate);
    setReadable(true);
}