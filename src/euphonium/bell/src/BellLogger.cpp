#include "BellLogger.h"

std::shared_ptr<bell::AbstractLogger> bell::bellGlobalLogger;

void bell::setDefaultLogger() {
    bell::bellGlobalLogger = std::make_shared<bell::BellLogger>();
}

void bell::enableSubmoduleLogging() {
    bell::bellGlobalLogger->enableSubmodule = true;
}