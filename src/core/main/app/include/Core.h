#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "BellUtils.h"

#include "CoreBindings.h"
#include "EuphContext.h"
#include "EuphLogger.h"
#include "EventBus.h"

#include "CoreEvents.h"
#include "HTTPDispatcher.h"
#include "PackageLoader.h"
#include "StorageAccessor.h"
#include "Connectivity.h"
#include "RadioPlugin.h"
#include "AudioTask.h"
#include "AudioSourcePlugin.h"

namespace euph {
class Core : public euph::EventSubscriber {
 private:
  // TAG for logging
  std::string TAG = "core";

  std::shared_ptr<euph::EventBus> eventBus;
  std::shared_ptr<euph::Connectivity> connectivity;

  std::shared_ptr<euph::Context> ctx = nullptr;

  std::shared_ptr<euph::HTTPDispatcher> http;
  std::shared_ptr<euph::PackageLoader> pkgLoader;
  std::shared_ptr<euph::AudioTask> audioTask;

  std::unique_ptr<euph::CoreBindings> bindings;
  std::vector<std::unique_ptr<euph::AudioSourcePlugin>> audioSources = {};

 public:
  Core(std::shared_ptr<euph::Connectivity> connectivity);
  ~Core();

  void initialize();
  void handleEvent(std::unique_ptr<Event>& event) override;
  void handleEventLoop();
};
}  // namespace euph
