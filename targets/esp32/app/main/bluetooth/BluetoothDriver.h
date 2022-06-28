#pragma once
extern "C" {

#include "bt_app_sink.h"
#include "bt_app_core.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_log.h"
#include "EuphoniumLog.h"
#include "Core.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_spp_api.h"

typedef std::function<void(bool)> bluetoothLockAccessCallback;
typedef std::function<void(uint8_t)> bluetoothVolumeChangedCallback;
typedef std::function<void(const std::string&, const std::string&, const std::string&)> bluetoothMetadataUpdatedCallback;
typedef std::function<void(const uint8_t*, uint32_t)> bluetoothAudioDataCallback;


extern "C" {
    void local_write_audio(const uint8_t *data, uint32_t len);
    void local_set_volume(uint8_t volume);
    void local_active_audio(bool active);
    void local_metadata_updated(char* artist, char* album, char* title, int duration);
}

class BluetoothDriver {
private:
public:
    BluetoothDriver(std::string name, std::string pin);
    // Callbacks
    bluetoothLockAccessCallback lockAccessCallback;
    bluetoothVolumeChangedCallback volumeChangedCallback;
    bluetoothAudioDataCallback audioDataCallback;
    bluetoothMetadataUpdatedCallback metadataUpdatedCallback;

    bool isLocked = false;
    std::string name;
    std::string pin;

    std::string getName();
    void setVolume(uint8_t volume);
    void writeBuffer(const uint8_t *buffer, uint32_t len);
    void setAudioActive(bool active);

    void start();
    void stop();
};


// esp-idf's bluetooths apis do not expose context APIs, so doing a dirty global seems to be an only way
extern std::shared_ptr<BluetoothDriver> globalBtDriver;