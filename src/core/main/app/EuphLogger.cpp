#include "EuphLogger.h"

void initializeEuphoniumLogger() {
    bell::bellGlobalLogger = new EuphoniumLogger();
}