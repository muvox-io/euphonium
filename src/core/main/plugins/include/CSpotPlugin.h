#pragma once

#include <atomic>
#include <mutex>
// for std::hash
#include <functional>
#include <string>

#include "BellTask.h"
#include "CSpotContext.h"
#include "LoginBlob.h"
#include "Queue.h"
#include "SpircHandler.h"

#include "AudioSourcePlugin.h"
#include "EuphContext.h"
#include "EuphLogger.h"
#include "WrappedSemaphore.h"

namespace euph {
class CSpotPlugin : public AudioSourcePlugin,
                    public bell::Task,
                    public euph::EventSubscriber {
 public:
  CSpotPlugin(std::shared_ptr<euph::Context> ctx);
  ~CSpotPlugin();

  // --- AudioSourcePlugin implementation
  void initializeBindings() override;
  void shutdownAudio() override;
  std::string getName() override;

  // -- BellTask implementation
  void runTask() override;

  // -- EventSubscriber implementation
  void handleEvent(std::unique_ptr<Event>& event) override;

  void queryContextURI(std::string uri) override;

 private:
  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<cspot::LoginBlob> loginBlob;
  std::shared_ptr<cspot::SpircHandler> spircHandler;
  std::unique_ptr<bell::WrappedSemaphore> authenticatedSemaphore;

  bool isRunning = false;
  std::mutex runningMutex;

  int64_t expectedTrackHash = -1;

  // Hash function required by the player
  std::hash<std::string_view> hashFn;

  size_t handleAudioData(uint8_t* data, size_t len, std::string_view trackId);
  void handleCSpotEvent(std::unique_ptr<cspot::SpircHandler::Event>);

  // Return spotify's status, used for zeroconf auto discovery
  std::string _getSpotifyInfo();
  std::string _authenticateZeroconf(berry::map info);
  void _authenticateSaved(std::string authData);
  void _startPlugin(std::string name);
};

}  // namespace euph
