#pragma once

#include <memory>
#include <string>

#include "civetweb.h"

namespace euph {

/**
 * @brief Enumerates the reasons why emergency mode can be active.
 */
enum class EmergencyModeReason {

  /**
     * @brief Emergency mode is not actually active.
     */
  NOT_ACTIVE = 0,

  BERRY_INIT_ERROR = 1,
};

/**
 * @brief Emergency mode provides a way to recover a device which suffered major filesystem corruption.
 * 
 * It serves a very simple one-file web page that can be used to connect to Wi-Fi,
 * wipe the filesystem, download new filesystem contents (or upload them from a local PC).
 *
 * The web page should not depend on any files stored on the FS.
 */
class EmergencyMode {
 public:
  EmergencyMode();

  bool isActive() const;

  /**
 * @brief Enable emergency mode with a specific reason.
 * 
 * @param reason The reason why emergency mode has been activated.
 */
  void trip(EmergencyModeReason reason);

  /**
    * @brief Serve the emergency mode page, if emergency mode is active.
    * 
    * @param conn The connection to serve the page to.
    * @return true If the page was served, caller should return from the request handler.
    * @return false If the page was not served, caller should continue processing the request.
    */
  bool tryServe(struct mg_connection* conn);

  /**
   * @brief Returns a human-readable string for the given emergency mode reason.
   * 
   * @param reason The reason to get a string for.
   * @return std::string The human-readable string.
   */
  static std::string getReasonString(EmergencyModeReason reason);

 private:
  EmergencyModeReason reason = EmergencyModeReason::NOT_ACTIVE;
};
}  // namespace euph
