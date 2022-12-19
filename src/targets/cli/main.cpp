#include <memory>
#include "BellHTTPServer.h"
#include "Core.h"
#include "EuphLogger.h"
#include "BellUtils.h"
#include "civetweb.h"

int main() {
  initializeEuphoniumLogger();

  auto core = std::make_unique<euph::Core>();
  core->handleEventLoop();

  return 0;
}