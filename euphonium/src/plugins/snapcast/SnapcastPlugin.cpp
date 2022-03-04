#include "SnapcastPlugin.h"
#include <HTTPStream.h>
#include <thread>

SnapcastPlugin::SnapcastPlugin() : bell::Task("snapcast", 6 * 1024, 0, 1) {
    name = "snapcast";
    connection = std::make_unique<Snapcast::Connection>();
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
    status = ModuleStatus::RUNNING;

    audioBuffer->lockAccess();

    connection->dataCallback = [this](uint8_t* data, uint32_t dataSize) {
        audioBuffer->write(data, dataSize);
    };

    connection->outputConfigCallback = [this](uint32_t sampleRate, uint16_t bitSize) {
        audioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                     sampleRate);
    };

    EUPH_LOG(info, "snapcast", "Starting...");
    connection->connectWithServer("");

    while (status == ModuleStatus::RUNNING) {
        connection->handleUpdate();
    }

}

void SnapcastPlugin::startAudioThread() { startTask(); }
