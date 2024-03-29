#include "RadioPlugin.h"
#include "CoreEvents.h"

using namespace euph;

RadioPlugin::RadioPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("radio", 32 * 1024, 0, 1) {
  this->ctx = ctx;

  // Can handle context playback via URIs
  this->supportsContextPlayback = true;
}

RadioPlugin::~RadioPlugin() {}

void RadioPlugin::runPlugin() {
  startTask();
}

void RadioPlugin::queryContextURI(std::string uri) {
  this->isRunning = false;
  std::scoped_lock lock(this->runningMutex);
  this->isRunning = true;

  // In case of radio, the URI is simply the playback URL
  this->playbackURLQueue.push(uri);
}

std::string RadioPlugin::getName() {
  return "radio";
}

void RadioPlugin::shutdownAudio() {
  this->isRunning = false;
  std::scoped_lock lock(this->runningMutex);
}

void RadioPlugin::initializeBindings() {
  EUPH_LOG(info, TASK, "Initializing bindings");
  ctx->vm->export_this("_run_plugin", this, &RadioPlugin::runPlugin, "radio");
}

void RadioPlugin::runTask() {
  std::string playbackUrl;

  // Create a string -> int hash function
  auto hashFunc = std::hash<std::string>();

  EUPH_LOG(info, TASK, "Waiting for playback requests");
  // Queue playback
  while (true) {
    playbackURLQueue.wpop(playbackUrl);
    EUPH_LOG(info, TASK, "Received URL %s", playbackUrl.c_str());
    std::scoped_lock playbackLock(runningMutex);
    // Mark as loading
    this->ctx->playbackController->setLoading();

    try {
      auto req = bell::HTTPClient::get(playbackUrl);

      auto container =
          bell::AudioContainers::guessAudioContainer(req->stream());
      if (container == nullptr) {
        throw std::runtime_error("Unsupported codec container");
      }

      auto codec = bell::AudioCodecs::getCodec(container.get());

      if (codec == nullptr) {
        throw std::runtime_error("Requested codec unsupported");
      }

      this->ctx->playbackController->lockPlayback("radio");
      this->ctx->audioBuffer->clearBuffer();

      uint32_t trackHash = hashFunc(playbackUrl);
      uint32_t outlen, toWrite, written = 0;
      uint8_t* data;

      // Guard playback only while song is requested
      while (isRunning) {
        data = codec->decode(container.get(), outlen);
        if (data == nullptr) {
          continue;
        }
        toWrite = outlen;

        while (toWrite > 0) {
          written = ctx->audioBuffer->writePCM(
              data + (outlen - toWrite), toWrite, trackHash, codec->sampleRate,
              codec->channelCount, bell::BitWidth::BW_16);
          toWrite -= written;

          // Buffer full, wait
          if (written == 0) {
            BELL_SLEEP_MS(100);
          }
        }
      }

      ctx->audioBuffer->unlockAccess();
      this->ctx->playbackController->unlockPlayback();
      EUPH_LOG(info, TASK, "Playback finished, buffer unlocked");
    } catch (std::exception& ex) {
      std::string exceptionMessage = ex.what();
      this->ctx->playbackController->setStopped();
      EUPH_LOG(info, TASK, "Cannot play requested url, [%s]", ex.what());

      // Report notification to the UI
      this->ctx->eventBus->postEvent(std::make_unique<NotificationEvent>(
          NotificationEvent::Type::ERROR, "radio", "Cannot play requested url",
          exceptionMessage));
    }
  }
}
