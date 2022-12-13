#include <memory>

#include "Core.h"
#include "EuphLogger.h"

int main(int argc, char* argv[]) {
  // Initialize the custom logger
  initializeEuphoniumLogger();

  try {
    auto core = std::make_unique<euph::Core>();
  } catch (char* e) {
    EUPH_LOG(error, "main", "Exception: %s", e);
  }

  return 0;
}
