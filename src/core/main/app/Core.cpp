#include "Core.h"
#include <memory>
#include "BellUtils.h"
#include "CoreBindings.h"

using namespace euph;

Core::Core() {
  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::create();
  this->http = std::make_shared<euph::HTTPDispatcher>(this->ctx);
  this->pkgLoader = std::make_shared<euph::PackageLoader>(this->ctx);
  this->bindings = std::make_unique<euph::CoreBindings>(this->ctx);

  this->http->initialize();
  this->pkgLoader->loadValidPackages();

  // setup bindings
  this->bindings->setupBindings();

  this->pkgLoader->loadWithHook("system");

  while (true) {
    BELL_SLEEP_MS(500);
  }
}

Core::~Core() {}