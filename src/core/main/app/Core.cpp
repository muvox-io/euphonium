#include "Core.h"
#include "BellUtils.h"

using namespace euph;

Core::Core() {
  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::create();
}

Core::~Core() {}