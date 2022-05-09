#include "BluetoothPlugin.h"

BluetoothPlugin::BluetoothPlugin()
    : bell::Task("bt_euph", 4 * 1024, 3, 0, false) {
    name = "bluetooth";
    this->btDriver = std::make_shared<BluetoothDriver>("bt");
    globalBtDriver = this->btDriver;

    this->btDriver->lockAccessCallback = [this](bool isLocked) {
        if (isLocked) {
            setStatus(ModuleStatus::RUNNING);
        }
    };

    this->btDriver->audioDataCallback = [this](const uint8_t *data, uint32_t len) {
        if (status == ModuleStatus::RUNNING) {
            size_t bytesWritten = 0;
            while (bytesWritten < len) {
                bytesWritten += mainAudioBuffer->write(data + bytesWritten,
                                                       len - bytesWritten);
            }
        }
    };

    this->btDriver->metadataUpdatedCallback = [this](const std::string &artist,
                                                     const std::string &album,
                                                     const std::string &title) {
        auto event = std::make_unique<SongChangedEvent>(
            std::string(title), std::string(album), std::string(artist),
            "bluetooth", "https://i.imgur.com/Fuu73lv.png");
        EUPH_LOG(info, "bluetooth", "Song name changed");
        mainEventBus->postEvent(std::move(event));
    };

    this->btDriver->volumeChangedCallback = [this](uint8_t volume) {
        auto event = std::make_unique<VolumeChangedEvent>(volume);
        EUPH_LOG(info, "bluetooth", "Volume changed");
        mainEventBus->postEvent(std::move(event));
    };
}

void BluetoothPlugin::shutdown() {
    EUPH_LOG(info, "bluetooth", "Shutting down...");
    setStatus(ModuleStatus::SHUTDOWN);
}

void BluetoothPlugin::runTask() {
    BTEvent event;

    while (true) {
        if (this->btEventQueue.wpop(event)) {
            if (event == BTEvent::Initialize) {
                btDriver->name = std::any_cast<std::string>(config["name"]);
                btDriver->start();
            }

            if (event == BTEvent::Deinitialize) {
            }

            if (event == BTEvent::Disconnect) {
                BELL_LOG(info, "bluetooth", "Disconnecting...");
                bt_disconnect();
                BELL_SLEEP_MS(1500);
                mainAudioBuffer->unlockAccess();
                status = ModuleStatus::SHUTDOWN;
            }

            if (event == BTEvent::LockAccess) {
                BELL_LOG(info, "bluetooth", "Locking access...");
                mainAudioBuffer->shutdownExcept(name);
                mainAudioBuffer->configureOutput(
                    AudioOutput::SampleFormat::INT16, 44100);
                mainAudioBuffer->lockAccess();
                status = ModuleStatus::RUNNING;
            }

        } else {
            BELL_SLEEP_MS(1000);
        }
    }
}

void BluetoothPlugin::startAudioThread() {
    startTask();
    btEventQueue.push(BTEvent::Initialize);
}

void BluetoothPlugin::setStatus(ModuleStatus status) {
    if (this->status == status)
        return;
    this->status = status;
    if (status == ModuleStatus::SHUTDOWN) {
        btEventQueue.push(BTEvent::Disconnect);
    } else {
        btEventQueue.push(BTEvent::LockAccess);
    }
}

void BluetoothPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/bluetooth", berry);
}

void BluetoothPlugin::setupBindings() {}
