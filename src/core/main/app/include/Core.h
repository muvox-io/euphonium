#pragma once

#include <memory>
#include <string>

#include "BellUtils.h"

#include "CoreBindings.h"
#include "EuphContext.h"
#include "EuphLogger.h"
#include "EventBus.h"
#include "HTTPDispatcher.h"
#include "PackageLoader.h"
#include "StorageAccessor.h"

namespace euph {
class Core : public euph::EventSubscriber {
 private:
  // TAG for logging
  std::string TAG = "core";

  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<euph::HTTPDispatcher> http;
  std::shared_ptr<euph::PackageLoader> pkgLoader;

  std::unique_ptr<euph::CoreBindings> bindings;

 public:
  Core();
  ~Core();

  void handleEvent(std::unique_ptr<Event>& event) override;
  void handleEventLoop();
};
}  // namespace euph