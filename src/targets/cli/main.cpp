#include <memory>

#include "EuphLogger.h"
#include "Core.h"

int main(int argc, char *argv[]) {
    // Initialize the custom logger
    initializeEuphoniumLogger();

    auto core = std::make_unique<euph::Core>();

    return 0;
}
