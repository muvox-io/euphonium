#include "CoreBindings.h"

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

void CoreBindings::_loadScript(std::string pkg, std::string path) {
  std::string scriptPath =
      fmt::format("{}/pkgs/{}/{}", ctx->rootPath, pkg, path);

  EUPH_LOG(debug, TAG, "Loading script [%s]", scriptPath.c_str());

  try {
    std::string scriptBody = this->ctx->storage->readFile(scriptPath);

    this->ctx->vm->execute_string(scriptBody, pkg);
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to load script: %s", scriptPath.c_str());
  }
}

std::string CoreBindings::_loadConfig(std::string pkg) {
  std::string configPath =
      fmt::format("{}/cfg/{}.json", ctx->rootPath, pkg);

  EUPH_LOG(debug, TAG, "Loading config for [%s]", pkg.c_str());

  try {
    std::string configBody = this->ctx->storage->readFile(configPath);
    return configBody;
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to load config: %s", configPath.c_str());
  }

  return "{}";
}

bool CoreBindings::_saveConfig(std::string pkg, std::string cfg) {
  std::string configPath =
      fmt::format("{}/cfg/{}.json", ctx->rootPath, pkg);

  EUPH_LOG(debug, TAG, "Loading config for [%s]", pkg.c_str());

  try {
    this->ctx->storage->writeFile(configPath, cfg);
    return true;
  } catch (...) {
    EUPH_LOG(error, TAG, "Failed to write config: %s", configPath.c_str());
  }

  return false;
}


long CoreBindings::_getTimeMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

void CoreBindings::_sleepMS(int ms) {
  BELL_SLEEP_MS(ms);
}
