#include "RadioPlugin.h"

using namespace euph;

RadioPlugin::RadioPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("radio", 4 * 1024, 0, 1) {
  this->ctx = ctx;
}

RadioPlugin::~RadioPlugin() {}

void RadioPlugin::runPlugin() {
  startTask();
}

void RadioPlugin::queryUrl(std::string url) {
  this->isRunning = false;
  std::scoped_lock lock(this->runningMutex);

  this->isRunning = true;
  this->playbackURLQueue.push(url);
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
  ctx->vm->export_this("_query_url", this, &RadioPlugin::queryUrl, "radio");
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

    try {
      auto req = bell::HTTPStream::get(playbackUrl);

      auto stream = std::make_unique<bell::EncodedAudioStream>();
      stream->openWithStream(std::move(req));

      // Lock access to the central audio buffer.
      ctx->audioBuffer->lockAccess();

      uint32_t trackHash = hashFunc(playbackUrl);
      size_t written, toWrite = 0;

      // Guard playback only while song is requested
      while (isRunning) {
        size_t readSize = stream->decodeFrame(dataTmp.data());
        toWrite = readSize;

        while (toWrite > 0) {
          written = ctx->audioBuffer->writePCM(
              dataTmp.data() + (readSize - toWrite), toWrite, trackHash);
          toWrite -= written;

          // Buffer full, wait
          if (written == 0) {
            BELL_SLEEP_MS(100);
          }
        }
      }

      ctx->audioBuffer->unlockAccess();
      EUPH_LOG(info, TASK, "Playback finished, buffer unlocked");
    } catch (...) {
      EUPH_LOG(info, TASK, "Cannot play requested url");
    }
  }
}
