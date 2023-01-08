#include "AudioTask.h"

using namespace euph;

AudioTask::AudioTask(std::shared_ptr<euph::Context> ctx, std::shared_ptr<euph::AudioOutput> euphAudioOutput)
    : bell::Task("AudioTask", 1024 * 16, 2, 0) {
  this->ctx = ctx;
  this->dsp = std::make_shared<bell::BellDSP>(ctx->audioBuffer);

  this->audioOutput = euphAudioOutput;
  this->audioOutput->configure(44100, 2, 16);

  // Start Audio thread
  startTask();
}

AudioTask::~AudioTask() {}

void AudioTask::runTask() {
  EUPH_LOG(info, TASK, "Audio thread running");
  bell::CentralAudioBuffer::AudioChunk currentChunk = {.pcmSize = 0};

  while (true) {
    if (!this->ctx->playbackController->isPaused) {
      // Handle pause request
      if (this->ctx->playbackController->requestPause) {
        this->ctx->playbackController->requestPause = false;

        // Prepare an effect, that will fade out the audio and trigger playback pause in one SR of samples since now
        auto effect = std::make_unique<bell::BellDSP::FadeEffect>(
            44100 / 2, false,
            [this]() { this->ctx->playbackController->isPaused = true; });
        this->dsp->queryInstantEffect(std::move(effect));
      }
      // Wait until next chunk arrives
      this->ctx->audioBuffer->chunkReady->twait(100);

      currentChunk = this->ctx->audioBuffer->readChunk();

      if (currentChunk.pcmSize > 0) {

        // Pass data to DSP
        size_t dataSize = this->dsp->process(
            currentChunk.pcmData, currentChunk.pcmSize, 2,
            bell::SampleRate::SR_44100, bell::BitWidth::BW_16);

        this->audioOutput->feedPCM(currentChunk.pcmData, dataSize);
      }
    } else {
      BELL_SLEEP_MS(100);
    }
  }
}
