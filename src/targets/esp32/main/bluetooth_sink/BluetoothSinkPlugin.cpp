#include "BluetoothSinkPlugin.h"

using namespace euph;

BluetoothSinkPlugin::BluetoothSinkPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("bt_sink", 4 * 1024, 1, 0) {
  this->ctx = ctx;
  this->a2dpDriver = std::make_unique<euph::A2DPDriver>();

  // Handle incoming data
  this->a2dpDriver->dataHandler = [this](const uint8_t* data,
                                         size_t dataLength) {
    if (!this->canPlay) {
      return;
    }

    size_t toWrite = dataLength;
    size_t written = 0;

    while (toWrite > 0) {
      written = this->ctx->audioBuffer->writePCM(data + (dataLength - toWrite),
                                                 toWrite, 0);
      toWrite -= written;

      // Buffer full, wait
      if (written == 0) {
        BELL_SLEEP_MS(100);
      }
    }
  };

  // Handle A2DP events
  this->a2dpDriver->eventHandler = [this](A2DPDriver::Event& event) {
    switch (event.type) {
      case A2DPDriver::EventType::VOLUME: {
        if (this->canPlay) {
          uint8_t volume = std::get<uint8_t>(event.data);
          // Ensure no repeated events
          if (volume != this->lastVolume) {
            this->lastVolume = volume;
            // Prepare audio event
            auto event = std::make_unique<euph::AudioVolumeEvent>(
                volume, AudioVolumeEvent::REMOTE);
            this->ctx->eventBus->postEvent(std::move(event));
          }
        }
        break;
      }
      case A2DPDriver::EventType::PLAYBACK_STATE: {
        bool isActive = std::get<bool>(event.data);

        if (isActive) {
          // Lock playback
          this->ctx->playbackController->lockPlayback("bluetooth");
          this->ctx->audioBuffer->clearBuffer();

          this->canPlay = true;
        } else if (this->canPlay) {
          this->ctx->playbackController->unlockPlayback();
          this->canPlay = false;
        }
        break;
      }
      case A2DPDriver::EventType::PLAYBACK_METADATA: {
        A2DPDriver::PlaybackMetadata metadata =
            std::get<A2DPDriver::PlaybackMetadata>(event.data);

        // Prepare metadata event
        auto event = std::make_unique<MetadataEvent>(metadata);
        this->ctx->eventBus->postEvent(std::move(event));
        break;
      }
    }
  };
}

BluetoothSinkPlugin::~BluetoothSinkPlugin() {}

void BluetoothSinkPlugin::runPlugin(std::string name) {
  this->a2dpDriver->initialize(name);
  startTask();
}

std::string BluetoothSinkPlugin::getName() {
  return "bluetooth_sink";
}

void BluetoothSinkPlugin::shutdownAudio() {
  if (this->canPlay) {
    this->canPlay = false;
    this->ctx->playbackController->unlockPlayback();
    this->a2dpDriver->disconnect();
  }
  this->isRunning = false;
}

void BluetoothSinkPlugin::initializeBindings() {
  EUPH_LOG(info, TASK, "Initializing bluetooth bindings");
  this->ctx->vm->export_this("_run_plugin", this,
                             &BluetoothSinkPlugin::runPlugin, "bluetooth");
}

void BluetoothSinkPlugin::runTask() {
  while (true) {
    this->a2dpDriver->processQueue();
  }
}
