#pragma once

#include <atomic>
#include <memory>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <string>
#include "civetweb.h"
#include "EventBus.h"

namespace euph {

/**
 * @brief Enumerates the reasons why emergency mode can be active.
 */
enum class EmergencyModeReason {

  /**
     * @brief Emergency mode is not actually active.
     */
  NOT_ACTIVE = 0,

  /**
   * @brief A berry error has occured while initializing scripts.
   * 
   */
  BERRY_INIT_ERROR = 1,

  /**
    * @brief Could not load packages for a given berry hook.
    * 
    */
  LOADING_BERRY_HOOK_FAILED = 2,

  /**
   * @brief Emeregency mode tripped manually by the user.
   */
  MANUAL_TRIP = 3,
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
  EmergencyMode(std::weak_ptr<euph::EventBus> eventBus);

  bool isActive() const;

  /**
 * @brief Enable emergency mode with a specific reason.
 * 
 * @param reason The reason why emergency mode has been activated.
 * @param message Optional message displayed on the emergency mode page, with additional information.
 */
  void trip(EmergencyModeReason reason, const std::string& message = "");

  /**
    * @brief Serve the emergency mode page, if emergency mode is active.
    *
    * To be used as a hook in the HTTP server, before trying to serve files from the filesystem.
    * 
    * @param conn The connection to serve the page to.
    * @return true If the page was served, caller should return from the request handler.
    * @return false If the page was not served, caller should continue processing the request.
    */
  bool tryServe(struct mg_connection* conn);

  /**
   * @brief Get the status of the emergency mode as a JSON object.
   * To be used as part of the /system/info endpoint.
   *
   * The json object contains the following fields:
   * - active: true if emergency mode is active, false otherwise
   * - reason: the reason why emergency mode is active (null if not active)
   * - message: an optional message with additional information (null if not active)
   * 
   * @return nlohmann::json Status information.
   */
  nlohmann::json getJsonStatus();

  /**
   * @brief Returns a human-readable string for the given emergency mode reason.
   * 
   * @param reason The reason to get a string for.
   * @return std::string The human-readable string.
   */
  static std::string getReasonString(EmergencyModeReason reason);

 private:
  std::weak_ptr<euph::EventBus> eventBus;
  std::atomic<EmergencyModeReason> reason = EmergencyModeReason::NOT_ACTIVE;
  std::string message;
  std::shared_mutex messageMutex;
};
}  // namespace euph
