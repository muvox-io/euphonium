#include "SnapcastPlugin.h"
#include <HTTPStream.h>
#include <thread>

SnapcastPlugin::SnapcastPlugin() : bell::Task("snapcast", 6 * 1024, 0, 1) {
    name = "snapcast";
}

void SnapcastPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
}

void SnapcastPlugin::setupBindings() {
}

void SnapcastPlugin::configurationUpdated() {}

void SnapcastPlugin::shutdown() {
    EUPH_LOG(info, "snapcast", "Shutting down...");
    status = ModuleStatus::SHUTDOWN;
}

void SnapcastPlugin::runTask() {
}

void SnapcastPlugin::startAudioThread() { startTask(); }
