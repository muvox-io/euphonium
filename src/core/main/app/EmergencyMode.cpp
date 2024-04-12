#include "EmergencyMode.h"
#include <string>
#include "EuphLogger.h"

using namespace euph;

EmergencyMode::EmergencyMode() {}

bool EmergencyMode::isActive() const {
  return this->reason != EmergencyModeReason::NOT_ACTIVE;
}

void EmergencyMode::trip(EmergencyModeReason reason) {
  EUPH_LOG(error, "EmergencyMode", "Tripped emergency mode with reason: %s",
           getReasonString(reason).c_str());
  this->reason = reason;
}

bool EmergencyMode::tryServe(struct mg_connection* conn) {
  if (!this->isActive()) {
    return false;
  }

  // Serve the emergency mode page
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "Emergency mode!");

  return true;
}

std::string EmergencyMode::getReasonString(EmergencyModeReason reason) {

#define CASE_RETURN_STR(x)     \
  case EmergencyModeReason::x: \
    return #x
  switch (reason) {
    CASE_RETURN_STR(NOT_ACTIVE);
    CASE_RETURN_STR(BERRY_INIT_ERROR);
  }

  return "__FIXME_UNKNOWN_ENUM_VALUE__";
}
