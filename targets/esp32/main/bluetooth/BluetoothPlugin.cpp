#include "BluetoothPlugin.h"

static void sink_data_handler(const uint8_t *data, uint32_t len) {
    size_t bytesWritten = 0;
    while (bytesWritten < len) {
        bytesWritten +=
            mainAudioBuffer->write(data + bytesWritten, len - bytesWritten);
    }
}

static bool bt_sink_cmd_handler(bt_sink_cmd_t cmd, va_list args) {
    switch (cmd) {
    case BT_SINK_AUDIO_STARTED: {
        mainAudioBuffer->shutdownExcept("bluetooth");
        auto event = std::make_unique<AudioTakeoverEvent>("bluetooth");
        mainEventBus->postEvent(std::move(event));
        BELL_LOG(info, "bluetooth", "Audio sink started");
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

        // LOG_INFO("BT stopped");
        break;
    case BT_SINK_PAUSE: {
        auto event = std::make_unique<PauseChangedEvent>(true);
        mainEventBus->postEvent(std::move(event));
        // LOG_INFO("BT paused, just silence");
        break;
    }
    case BT_SINK_RATE:
        // output.next_sample_rate = output.current_sample_rate = va_arg(args,
        // u32_t); LOG_INFO("Setting BT sample rate %u",
        // output.next_sample_rate);
        break;
    case BT_SINK_METADATA: {
        char *artist = va_arg(args, char *);
        char *album = va_arg(args, char *);
        char *title = va_arg(args, char *);
        auto event = std::make_unique<SongChangedEvent>(
            std::string(title), std::string(album), std::string(artist),
            "bluetooth", "https://i.imgur.com/F1b2xkj.png");
        EUPH_LOG(info, "bluetooth", "Song name changed");
        mainEventBus->postEvent(std::move(event));

        break;
    }
    case BT_SINK_VOLUME: {
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

BluetoothPlugin::BluetoothPlugin() : bell::Task("bt_euph", 4 * 1024, 0, false) {
    name = "bluetooth";
    // bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
}

void BluetoothPlugin::shutdown() {
    bt_disconnect();
}

void BluetoothPlugin::runTask() {
    auto memUsage =
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    auto memUsage2 = heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    BELL_LOG(info, "bluetooth", "Free RAM %d | %d", memUsage, memUsage2);
    bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
}

void BluetoothPlugin::startAudioThread() {
    auto memUsage =
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    auto memUsage2 = heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    BELL_LOG(info, "bluetooth", "Free RAM %d | %d", memUsage, memUsage2);
    startTask();
}

void BluetoothPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    // scriptLoader->loadScript("esp32/bluetooth", berry);
}

void BluetoothPlugin::setupBindings() {}
