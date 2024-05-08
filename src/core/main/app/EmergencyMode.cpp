#include "EmergencyMode.h"
#include <memory>
#include <shared_mutex>
#include <string>
#include "CoreEvents.h"
#include "EuphLogger.h"

using namespace euph;

namespace euph {
extern unsigned char emergencyModeHtml[];
extern unsigned int emergencyModeHtml_len;
}  // namespace euph

static const char* TAG = "EmergencyMode";

EmergencyMode::EmergencyMode(std::weak_ptr<euph::EventBus> eventBus)
    : eventBus(eventBus) {}

bool EmergencyMode::isActive() const {
  return this->reason != EmergencyModeReason::NOT_ACTIVE;
}

void EmergencyMode::trip(EmergencyModeReason reason,
                         const std::string& message) {
  EUPH_LOG(error, TAG, "===============================");
  EUPH_LOG(error, TAG, "Tripped emergency mode with reason: %s",
           getReasonString(reason).c_str());
  EUPH_LOG(error, TAG, "===============================");
  this->reason = reason;
  if (std::shared_ptr<euph::EventBus> bus = this->eventBus.lock()) {
    EUPH_LOG(error, TAG, "Posting EmergencyModeTrippedEvent");
    auto evt = std::make_unique<EmergencyModeTrippedEvent>(reason);
    bus->postEvent(std::move(evt));
  }
  std::unique_lock<std::shared_mutex> lock(this->messageMutex);
  this->message = message;
}

bool EmergencyMode::tryServe(struct mg_connection* conn) {
  if (!this->isActive()) {
    return false;
  }
  std::shared_lock<std::shared_mutex> lock(this->messageMutex);
  // Serve the emergency mode page
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n",
            emergencyModeHtml_len);
  mg_write(conn, emergencyModeHtml, emergencyModeHtml_len);

  return true;
}

nlohmann::json EmergencyMode::getJsonStatus() {
  nlohmann::json status;
  bool isActive = this->isActive();
  status["active"] = isActive;

  if (isActive) {
    status["reason"] = getReasonString(this->reason);
    std::shared_lock<std::shared_mutex> lock(this->messageMutex);
    status["message"] = this->message;
  } else {
    status["reason"] = nullptr;
    status["message"] = nullptr;
  }

  return status;
}

std::string EmergencyMode::getReasonString(EmergencyModeReason reason) {

#define CASE_RETURN_STR(x)     \
  case EmergencyModeReason::x: \
    return #x
  switch (reason) {
    CASE_RETURN_STR(NOT_ACTIVE);
    CASE_RETURN_STR(BERRY_INIT_ERROR);
    CASE_RETURN_STR(MANUAL_TRIP);
    CASE_RETURN_STR(LOADING_BERRY_HOOK_FAILED);
  }

  return "__FIXME_UNKNOWN_ENUM_VALUE__";
}
