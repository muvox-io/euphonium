#include "AudioTask.h"

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
    defaultPipeline = this->ctx->storage->readFile(this->pipelinePath);
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

  this->ctx->storage->writeFile(this->ctx->rootPath + this->pipelinePath, str);
}

void AudioTask::_configureDSP(std::string dspPreset) {
  this->loadDSPFromString(dspPreset);
}

AudioTask::~AudioTask() {}

void AudioTask::runTask() {
  EUPH_LOG(info, TASK, "Audio thread running");
  bell::CentralAudioBuffer::AudioChunk currentChunk = {.pcmSize = 0};

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

      if (currentChunk.pcmSize > 0) {
        std::scoped_lock lock(this->dspMutex);
        // Pass data to DSP
        size_t dataSize = this->dsp->process(
            currentChunk.pcmData, currentChunk.pcmSize, 2,
            bell::SampleRate::SR_44100, bell::BitWidth::BW_16);

        this->audioOutput->feedPCM(currentChunk.pcmData, dataSize);
      } else {
        EUPH_LOG(info, TASK, "No audio?");
        BELL_SLEEP_MS(100);
      }
    } else {
      BELL_SLEEP_MS(100);
    }
  }
}
