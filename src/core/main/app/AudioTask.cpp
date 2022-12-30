#include "AudioTask.h"

using namespace euph;

AudioTask::AudioTask(std::shared_ptr<euph::Context> ctx): bell::Task("AudioTask", 1024 * 16, 2, 0) {
  this->ctx = ctx;
  this->dsp = std::make_shared<bell::BellDSP>(ctx->audioBuffer);

  // Start Audio thread
  startTask();
}

AudioTask::~AudioTask() {
}

void AudioTask::runTask() {
  EUPH_LOG(info, TASK, "Audio thread running");
  while (true) {
    //this->ctx->audioBuffer->chunkReady->wait();

  }
}
