#include "Core.h"
#include "BellUtils.h"

using namespace euph;

Core::Core() {
  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::create();
  this->http = std::make_shared<euph::HTTPDispatcher>(this->ctx);

  this->http->initialize();

  while (true) {
    BELL_SLEEP_MS(500);
  }
}

Core::~Core() {}