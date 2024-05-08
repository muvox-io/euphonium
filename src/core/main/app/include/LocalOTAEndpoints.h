#pragma once

#include <memory>
#include "EuphContext.h"
#include "BellHTTPServer.h"

namespace euph {

void registerLocalOTAEndpoints(bell::BellHTTPServer& server,
                               std::weak_ptr<euph::Context> ctx);
}

