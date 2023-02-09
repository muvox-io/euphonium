#pragma once

#include <atomic>
#include <mutex>
// for std::hash
#include <functional>
#include <string>

#include "BellTask.h"
#include "EncodedAudioStream.h"
#include "Queue.h"

#include "AudioSourcePlugin.h"
#include "EuphContext.h"
#include "EuphLogger.h"

#include "SnapcastConnection.h"

namespace euph {
class SnapcastPlugin : public AudioSourcePlugin, public bell::Task {
 public:
  SnapcastPlugin(std::shared_ptr<euph::Context> ctx);
  ~SnapcastPlugin();

  void _connect(std::string url, int port);
  void _disconnect();

  // --- AudioSourcePlugin implementation
  void initializeBindings() override;
  void shutdownAudio() override;
  std::string getName() override;

  // -- BellTask implementation
  void runTask() override;

 private:
  std::shared_ptr<euph::Context> ctx;
  std::unique_ptr<snapcast::Connection> snapcastConnection;

  std::atomic<bool> isRunning = false;
  std::mutex runningMutex;
};
}  // namespace euph
