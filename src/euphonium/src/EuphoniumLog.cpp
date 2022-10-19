#include "EuphoniumLog.h"
#include <memory>

void initializeEuphoniumLogger() {
    bell::bellGlobalLogger = std::make_shared<EuphoniumLogger>();
}

