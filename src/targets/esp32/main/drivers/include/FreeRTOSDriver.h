#pragma once

#include <memory>

#include "BellLogger.h"
#include "BerryBind.h"

void exportFreeRTOSDriver(std::shared_ptr<berry::VmState> berry);

