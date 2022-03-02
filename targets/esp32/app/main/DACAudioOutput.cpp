#include "DACAudioOutput.h"

DACAudioOutput::DACAudioOutput()
{
}

DACAudioOutput::~DACAudioOutput()
{
}

void DACAudioOutput::setupBindings(std::shared_ptr<berry::VmState> berry) {
    berry->export_this("set_readable", this, &DACAudioOutput::setReadable, "i2s");
    berry->export_this("expand", this, &DACAudioOutput::setWriteExpand, "i2s");
    berry->export_this("disable_expand", this, &DACAudioOutput::disableWriteExpand, "i2s");
}

void DACAudioOutput::setReadable(bool readable) {
    this->isReading = readable;
    std::scoped_lock lock(this->readingMutex);
}

void DACAudioOutput::setWriteExpand(int srcBits, int dstBits) {
    this->doWriteExpand = true;
    this->srcBits = srcBits;
    this->dstBits = dstBits;
}

void DACAudioOutput::disableWriteExpand() {
    this->doWriteExpand = false;
}

void DACAudioOutput::feedPCMFrames(uint8_t* data, size_t nBytes) {
    size_t written = 0;
    while (written < nBytes)
    {
        if (isReading) {
            std::scoped_lock lock(readingMutex);
            if (doWriteExpand) {
                i2s_write_expand((i2s_port_t)0, data + written, nBytes - written, srcBits, dstBits, &written, portMAX_DELAY);
            } else {
                i2s_write((i2s_port_t)0, data + written, nBytes - written,
                          &written, portMAX_DELAY);
            }
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
