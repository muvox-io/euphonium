#include "BluetoothPlugin.h"

// @TODO Refactor all of this to proper C++. Global shared_ptr is not ideal.
std::shared_ptr<BluetoothPlugin> mainBluetoothPlugin;

static void sink_data_handler(const uint8_t *data, uint32_t len) {
    if (mainBluetoothPlugin->status == ModuleStatus::RUNNING) {
        size_t bytesWritten = 0;
        while (bytesWritten < len) {
            bytesWritten +=
                mainAudioBuffer->write(data + bytesWritten, len - bytesWritten);
        }
    }
}

static bool bt_sink_cmd_handler(bt_sink_cmd_t cmd, va_list args) {
    switch (cmd) {
    case BT_SINK_AUDIO_STARTED: {

        //mainAudioBuffer->lockAccess();
        BELL_LOG(info, "bluetooth", "Audio sink started");
        mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
        break;
    }
    case BT_SINK_AUDIO_STOPPED: {
        BELL_LOG(info, "bluetooth", "Audio sink stopped");

        break;
    }
    case BT_SINK_PLAY: {
        auto event = std::make_unique<PauseChangedEvent>(false);
        mainEventBus->postEvent(std::move(event));
        // LOG_INFO("BT playing");
        break;
    }
    case BT_SINK_STOP:

        BELL_LOG(info, "bluetooth", "BT stopped");
        break;
    case BT_SINK_PAUSE: {
        auto event = std::make_unique<PauseChangedEvent>(true);
        mainEventBus->postEvent(std::move(event));
        // LOG_INFO("BT paused, just silence");
        break;
    }
    case BT_SINK_RATE: {
        uint32_t sampleRate = va_arg(args, u32_t);
        mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
        BELL_LOG(info, "bluetooth", "Sample rate changed to %d", sampleRate);
        mainAudioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                         sampleRate);
        break;
    }
    case BT_SINK_METADATA: {
        char *artist = va_arg(args, char *);
        char *album = va_arg(args, char *);
        char *title = va_arg(args, char *);
        auto event = std::make_unique<SongChangedEvent>(
            std::string(title), std::string(album), std::string(artist),
            "bluetooth", "https://i.imgur.com/Fuu73lv.png");
        EUPH_LOG(info, "bluetooth", "Song name changed");
        mainEventBus->postEvent(std::move(event));

        break;
    }
    case BT_SINK_VOLUME: {
        EUPH_LOG(info, "bluetooth", "Volume changed");
        u32_t volume = va_arg(args, u32_t);
        volume = 100 * powf(volume / 128.0f, 3);
        auto event = std::make_unique<VolumeChangedEvent>(volume);
        EUPH_LOG(info, "bluetooth", "Volume changed");
        mainEventBus->postEvent(std::move(event));
        break;
    }
    default:
        break;
    }

    return true;
}

BluetoothPlugin::BluetoothPlugin() : bell::Task("bt_euph", 4 * 1024, 3, 0, false) {
    name = "bluetooth";
    // bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
}

void BluetoothPlugin::shutdown() {
    EUPH_LOG(info, "bluetooth", "Shutting down...");
    //mainAudioBuffer->unlockAccess();
    setStatus(ModuleStatus::SHUTDOWN);
}

void BluetoothPlugin::runTask() {
    BTEvent event;

    while (true) {
        if (this->btEventQueue.wpop(event)) {
            if (event == BTEvent::Initialize) {
                bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
            }

            if (event == BTEvent::Deinitialize) {
                bt_sink_deinit();
            }

            if (event == BTEvent::Disconnect) {
                BELL_LOG(info, "bluetooth", "Disconnecting...");
                bt_disconnect();
                BELL_SLEEP_MS(1500);
                //bt_sink_deinit();
                mainAudioBuffer->unlockAccess();
                status = ModuleStatus::SHUTDOWN;
                //bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
            }

            if (event == BTEvent::LockAccess) {
                BELL_LOG(info, "bluetooth", "Locking access...");
                mainAudioBuffer->shutdownExcept(name);
                mainAudioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                                 44100);
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
    if (this->status == status) return;
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
