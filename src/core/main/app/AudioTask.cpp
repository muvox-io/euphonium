#include "AudioTask.h"
#include <memory>
#include "CoreEvents.h"

#include "AudioPipeline.h"
#include "BellDSP.h"
#include "Gain.h"

using namespace euph;

AudioTask::AudioTask(std::shared_ptr<euph::Context> ctx,
                     std::shared_ptr<euph::AudioOutput> euphAudioOutput)
    : bell::Task("AudioTask", 1024 * 16, 1, 0) {
  this->ctx = ctx;
  this->dsp = std::make_shared<bell::BellDSP>(ctx->audioBuffer);

  this->audioOutput = euphAudioOutput;
  this->audioOutput->configure(44100, 2, 16);

  // Try to load DSP pipeline;
  std::string defaultPipeline = "{ \"transforms\": [] }";

  try {
    defaultPipeline =
        this->ctx->storage->readFile(this->ctx->rootPath + this->pipelinePath);
  } catch (...) {}

  this->loadDSPFromString(defaultPipeline);

  this->exportBindings();

  // Start Audio thread
  startTask();
}

void AudioTask::exportBindings() {
  this->ctx->vm->export_this("configure_dsp", this, &AudioTask::_configureDSP,
                             "playback");
}

void AudioTask::loadDSPFromString(const std::string& str) {
  std::scoped_lock lock(this->dspMutex);
  // cJSON parse buffer
  cJSON* root = cJSON_Parse(str.c_str());
  cJSON* array = cJSON_GetObjectItem(root, "transforms");
  this->currentPipeline = std::make_shared<bell::AudioPipeline>();
  cJSON* iterator = NULL;
  cJSON_ArrayForEach(iterator, array) {
    cJSON* type = cJSON_GetObjectItem(iterator, "type");
    if (type == NULL || !cJSON_IsString(type))
      continue;

    auto typeStr = std::string(type->valuestring);

    if (typeStr == "gain") {
      auto filter = std::make_shared<bell::Gain>();
      filter->config = std::make_unique<bell::JSONTransformConfig>(iterator);
      filter->reconfigure();
      this->currentPipeline->addTransform(filter);
    } else if (typeStr == "compressor") {
      auto filter = std::make_shared<bell::Compressor>();
      filter->config = std::make_unique<bell::JSONTransformConfig>(iterator);
      filter->reconfigure();
      this->currentPipeline->addTransform(filter);
    } else if (typeStr == "mixer") {
      auto filter = std::make_shared<bell::AudioMixer>();
      filter->config = std::make_unique<bell::JSONTransformConfig>(iterator);
      filter->fromJSON(iterator);
      this->currentPipeline->addTransform(filter);
    } else if (typeStr == "biquad") {
      auto filter = std::make_shared<bell::Biquad>();
      filter->config = std::make_unique<bell::JSONTransformConfig>(iterator);
      filter->reconfigure();
      this->currentPipeline->addTransform(filter);
    } else if (typeStr == "biquad_combo") {
      auto filter = std::make_shared<bell::BiquadCombo>();
      filter->config = std::make_unique<bell::JSONTransformConfig>(iterator);
      filter->reconfigure();
      this->currentPipeline->addTransform(filter);
    } else {
      throw std::invalid_argument(
          "No filter type found for configuration field " + typeStr);
    }
  }

  currentPipeline->volumeUpdated(50);
  dsp->applyPipeline(this->currentPipeline);
  cJSON_Delete(root);

  try {
    this->ctx->storage->writeFile(this->ctx->rootPath + this->pipelinePath,
                                  str);
  } catch (...) {
    EUPH_LOG(error, TASK, "Failed to write pipeline to disk");
  }
}

void AudioTask::_configureDSP(std::string dspPreset) {
  this->loadDSPFromString(dspPreset);
}

AudioTask::~AudioTask() {}

void AudioTask::runTask() {
  EUPH_LOG(info, TASK, "Audio thread running");
  bell::CentralAudioBuffer::AudioChunk* currentChunk;
  uint32_t lastSampleRate = 44100;

  // Used to detect track changes
  size_t lastTrackHash = 0;

  while (true) {
    if (!this->ctx->playbackController->isPaused) {
      // Handle pause request
      if (this->ctx->playbackController->requestPause) {
        std::scoped_lock lock(this->dspMutex);
        this->ctx->playbackController->requestPause = false;

        // Prepare an effect, that will fade out the audio and trigger playback pause in one SR of samples since now
        auto effect = std::make_unique<bell::BellDSP::FadeEffect>(
            44100 / 2, false,
            [this]() { this->ctx->playbackController->isPaused = true; });
        this->dsp->queryInstantEffect(std::move(effect));
      }

      currentChunk = this->ctx->audioBuffer->readChunk();

      if (currentChunk && currentChunk->pcmSize > 0) {
        if (currentChunk->trackHash != lastTrackHash) {
          lastTrackHash = currentChunk->trackHash;

          // Track hash changed, enqueue a track change event
          auto event = std::make_unique<TrackHashChangeEvent>(lastTrackHash);
          this->ctx->eventBus->postEvent(std::move(event));
        }

        std::scoped_lock lock(this->dspMutex);
        if (lastSampleRate != currentChunk->sampleRate) {
          lastSampleRate = currentChunk->sampleRate;
          this->audioOutput->configure(static_cast<uint32_t>(lastSampleRate), 2,
                                       16);
        }
        // Pass data to DSP
        size_t dataSize =
            this->dsp->process(currentChunk->pcmData, currentChunk->pcmSize, 2,
                               currentChunk->sampleRate, bell::BitWidth::BW_16);
        // Handle audio sync
        // @TODO actually do it properly
        if (currentChunk->sec != 0 || currentChunk->usec != 0) {
          bell::tv scheduledTimestamp(currentChunk->sec, currentChunk->usec);

          int64_t diff = (scheduledTimestamp - bell::tv::now()).ms();
          while (diff > 25) {
            BELL_SLEEP_MS(2);
            diff = (scheduledTimestamp - bell::tv::now()).ms();
          }
        }

        this->audioOutput->feedPCM(currentChunk->pcmData, dataSize);
      } else {
        BELL_SLEEP_MS(100);
      }
    } else {
      printf("Audio thread paused\n");
      BELL_SLEEP_MS(100);
    }
  }
}
