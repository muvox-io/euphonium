#include "AudioTask.h"
#include "PortAudioSink.h"

using namespace euph;

AudioTask::AudioTask(std::shared_ptr<euph::Context> ctx): bell::Task("AudioTask", 1024 * 16, 2, 0) {
  this->ctx = ctx;
  this->dsp = std::make_shared<bell::BellDSP>(ctx->audioBuffer);

  this->audioSink = std::make_unique<PortAudioSink>();
  this->audioSink->setParams(44100, 2, 16);

  // Start Audio thread
  startTask();
}

AudioTask::~AudioTask() {
}

void AudioTask::runTask() {
  EUPH_LOG(info, TASK, "Audio thread running");
  bell::CentralAudioBuffer::AudioChunk currentChunk = { .pcmSize = 0 };

  while (true) {
    // Wait until next chunk arrives
    this->ctx->audioBuffer->chunkReady->wait();

    currentChunk = this->ctx->audioBuffer->readChunk();

    if (currentChunk.pcmSize > 0) {
      this->audioSink->feedPCMFrames(currentChunk.pcmData, currentChunk.pcmSize);
    }
  }
}
