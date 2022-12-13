#include <memory>
#include "Core.h"

int main(int argc, char *argv[]) {
    // initializeEuphoniumLogger();
    std::unique_ptr<euph::Core> core = std::make_unique<euph::Core>();

    return 0;
}
