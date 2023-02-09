#pragma once

#include <atomic>
#include <mutex>
// for std::hash
#include <functional>
#include <string>

#include "AudioContainers.h"
#include "AudioCodecs.h"
#include "BellTask.h"
#include "HTTPClient.h"
#include "Queue.h"

#include "AudioSourcePlugin.h"
#include "EuphContext.h"
#include "EuphLogger.h"

namespace euph {
class RadioPlugin : public AudioSourcePlugin, public bell::Task {
 public:
  RadioPlugin(std::shared_ptr<euph::Context> ctx);
  ~RadioPlugin();

  void runPlugin();

  // --- AudioSourcePlugin implementation
  void initializeBindings() override;
  void shutdownAudio() override;
  std::string getName() override;

  // -- BellTask implementation
  void runTask() override;

  void queryContextURI(std::string uri) override;

 private:
  std::shared_ptr<euph::Context> ctx;
  bell::Queue<std::string> playbackURLQueue;

  std::atomic<bool> isRunning = false;
  std::mutex runningMutex;

  // Reserve 8Kb for audio frames. More than necessary
  std::vector<uint8_t> dataTmp = std::vector<uint8_t>(1024 * 8);
};

}  // namespace euph
