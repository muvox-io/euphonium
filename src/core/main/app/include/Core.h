#pragma once

#include <memory>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

#include "BellUtils.h"
#include "DecoderGlobals.h"

#include "CoreBindings.h"
#include "EuphAudioOutput.h"
#include "EuphContext.h"
#include "EuphLogger.h"
#include "EventBus.h"

#include "CoreEvents.h"
#include "HTTPDispatcher.h"
#include "OTAHandler.h"
#include "PackageLoader.h"
#include "StorageAccessor.h"
#include "Connectivity.h"
#include "RadioPlugin.h"
#include "CSpotPlugin.h"
#include "SnapcastPlugin.h"
#include "AudioTask.h"
#include "AudioSourcePlugin.h"
#include "DecoderGlobals.h"

namespace euph {
class Core : public euph::EventSubscriber {
 private:
  // TAG for logging
  std::string TAG = "core";

  std::shared_ptr<euph::EventBus> eventBus;
  std::shared_ptr<euph::Connectivity> connectivity;

  std::shared_ptr<euph::Context> ctx = nullptr;

  std::shared_ptr<euph::HTTPDispatcher> http;
  std::shared_ptr<euph::OTAHandler> otaHandler;
  std::shared_ptr<euph::PackageLoader> pkgLoader;
  std::shared_ptr<euph::AudioTask> audioTask;
  std::shared_ptr<euph::AudioOutput> audioOutput;

  std::unique_ptr<euph::CoreBindings> bindings;
  std::vector<std::unique_ptr<euph::AudioSourcePlugin>> audioSources = {};

  std::atomic<bool> pluginsInitialized = false;

 public:
  Core(std::shared_ptr<euph::Connectivity> connectivity, std::shared_ptr<euph::EventBus> eventBus, std::shared_ptr<euph::AudioOutput> euphAudioOutput);
  ~Core();

  void initialize();
  void handleEvent(std::unique_ptr<Event>& event) override;
  void handleEventLoop();
  void registerAudioSource(std::unique_ptr<euph::AudioSourcePlugin> source);
  void initializePlugins();

  // Used in platform implementations to export platform-specific bindings to the vm
  std::function<void(std::shared_ptr<euph::Context>)> exportPlatformBindings = nullptr;
};
}  // namespace euph
