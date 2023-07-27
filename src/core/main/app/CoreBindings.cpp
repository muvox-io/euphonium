#include "CoreBindings.h"
#include <memory>
#include "BellUtils.h"
#include "CoreEvents.h"

#include <filesystem>
#include <fstream>
#include <streambuf>


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef ESP_PLATFORM
#include "esp_system.h"
#else
// execv
#include <unistd.h>

#endif

using namespace euph;

CoreBindings::CoreBindings(std::shared_ptr<euph::Context> ctx) {
  this->ctx = ctx;
}

CoreBindings::~CoreBindings() {}

void CoreBindings::setupBindings() {
  EUPH_LOG(debug, TAG, "Setting up core bindings");

  ctx->vm->export_this("sleep_ms", this, &CoreBindings::_sleepMS, "core");
  ctx->vm->export_this("version", this, &CoreBindings::_getVersion, "core");
  ctx->vm->export_this("platform", this, &CoreBindings::_getPlatform, "core");
  ctx->vm->export_this("load", this, &CoreBindings::_loadScript, "core");
  ctx->vm->export_this("load_config", this, &CoreBindings::_loadConfig, "core");
  ctx->vm->export_this("save_config", this, &CoreBindings::_saveConfig, "core");
  ctx->vm->export_this("get_time_ms", this, &CoreBindings::_getTimeMs, "core");
  ctx->vm->export_this("trigger_pause", this, &CoreBindings::_triggerPause,
                       "core");
  ctx->vm->export_this("get_timestamp", this, &CoreBindings::_getTimestamp,
                       "core");
  ctx->vm->export_this("get_mac", this, &CoreBindings::_getMac, "core");
  ctx->vm->export_this("set_display_name", this, &CoreBindings::_setDisplayName,
                       "core");
  ctx->vm->export_this("query_context_uri", this,
                       &CoreBindings::_queryContextURI, "core");
  ctx->vm->export_this("set_native_volume", this,
                       &CoreBindings::_setNativeVolume, "core");
  ctx->vm->export_this("delete_config_files", this,
                       &CoreBindings::_deleteConfigFiles, "core");
  ctx->vm->export_this("restart", this, &CoreBindings::_restart, "core");

  ctx->vm->export_this("clear_config", this, &CoreBindings::_clearWifiConfig,
                       "wifi");
}

std::string CoreBindings::_getPlatform() {
#ifdef ESP_PLATFORM
  return "esp32";
#endif
  return "desktop";
}

std::string CoreBindings::_getVersion() {
  return "0.0.1";
}

void CoreBindings::_triggerPause(bool isPaused) {
  if (isPaused) {
    this->ctx->playbackController->pause();
  } else {
    this->ctx->playbackController->play();
  }
}

void CoreBindings::_setDisplayName(std::string name) {
  this->ctx->displayName = name;
}

uint64_t CoreBindings::_getTimestamp() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

void CoreBindings::_loadScript(std::string pkg, std::string path) {
  std::string scriptPath =
      fmt::format("{}/pkgs/{}/{}", ctx->rootPath, pkg, path);

  EUPH_LOG(debug, TAG, "Loading script [%s]", scriptPath.c_str());

  try {

    std::ifstream t(scriptPath);
    std::string scriptBody((std::istreambuf_iterator<char>(t)),
                           std::istreambuf_iterator<char>());
    this->ctx->vm->execute_string(scriptBody, scriptPath);
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to load script: %s", scriptPath.c_str());
  }
}

std::string CoreBindings::_loadConfig(std::string pluginName) {
  std::string configPath =
      fmt::format("{}/cfg/{}.json", ctx->rootPath, pluginName);

  EUPH_LOG(debug, TAG, "Loading config for [%s]", pluginName.c_str());

  try {
    std::ifstream t(configPath);
    if (t.fail()) {
      EUPH_LOG(debug, TAG, "Config file not found: %s", configPath.c_str());
      return "{}";
    }
    std::string configBody((std::istreambuf_iterator<char>(t)),
                           std::istreambuf_iterator<char>());
    return configBody;
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to load config: %s", configPath.c_str());
  }

  return "{}";
}

bool CoreBindings::_saveConfig(std::string pluginName, std::string cfg) {
  std::string configPath =
      fmt::format("{}/cfg/{}.json", ctx->rootPath, pluginName);

  EUPH_LOG(debug, TAG, "Saving config for [%s]", pluginName.c_str());

  try {
    std::ofstream t(configPath);
    if (t.fail()) {
      EUPH_LOG(error, TAG, "Failed to open config file: %s",
               configPath.c_str());
      return false;
    }
    t << cfg;
    t.close();
    return true;
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to write config: %s", configPath.c_str());
  }

  return false;
}

std::string CoreBindings::_getMac() {
  return bell::getMacAddress();
}

void CoreBindings::_setNativeVolume(int volume) {
  this->ctx->playbackController->currentVolume = volume;
}

void CoreBindings::_queryContextURI(std::string uri) {
  auto uriEvent = std::make_unique<ContextURIEvent>(uri);
  this->ctx->eventBus->postEvent(std::move(uriEvent));
}

uint64_t CoreBindings::_getTimeMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

void CoreBindings::_sleepMS(int ms) {
  BELL_SLEEP_MS(ms);
}

void CoreBindings::_deleteConfigFiles() {
  std::string path = fmt::format("{}/cfg/", ctx->rootPath);
  BELL_LOG(debug, TAG, "Deleting config files in: %s", path.c_str());
   struct dirent *entry;
    DIR *dir = opendir(path.c_str());
  while ((entry = readdir(dir)) != NULL) {
    std::string filename = entry->d_name;
    if (filename == "." || filename == "..") {
      continue;
    }
    std::string filepath = path + filename;
   
    remove(filepath.c_str());
  }
}

void CoreBindings::_restart() {
  EUPH_LOG(info, TAG, "Restarting the application...");
#ifdef ESP_PLATFORM
  esp_restart();
#else
  execv("/proc/self/exe", NULL);
#endif
}

void CoreBindings::_clearWifiConfig() {
  // @TODO bring back
  // this->ctx->storage->executeFromTask(
  //     [this]() { this->ctx->connectivity->clearConfig(); });
}
