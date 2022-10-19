#include "BluetoothDriver.h"

uint32_t spp_handle = 0;
static void esp_bt_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);
enum {
    BT_APP_EVT_STACK_UP = 0,
};

std::shared_ptr<BluetoothDriver> globalBtDriver;

BluetoothDriver::BluetoothDriver(std::string name, std::string pin) {
    this->name = name;
    this->pin = pin;
}

void local_write_audio(const uint8_t *data, uint32_t len) {
    globalBtDriver->writeBuffer(data, len);
}

void local_set_volume(uint8_t volume) { globalBtDriver->setVolume(volume); }

void local_active_audio(bool active) {
    globalBtDriver->lockAccessCallback(active);
    globalBtDriver->setAudioActive(active);
}

void local_metadata_updated(char *artist, char *album, char *title,
                            int duration) {
    auto artistStr = std::string(artist);
    auto albumStr = std::string(album);
    auto titleStr = std::string(title);
    globalBtDriver->metadataUpdatedCallback(artist, album, title);
}

std::string BluetoothDriver::getName() { return name; }

void BluetoothDriver::setVolume(uint8_t volume) {
    this->volumeChangedCallback(volume);
}

void BluetoothDriver::setAudioActive(bool active) {
    isLocked = active;
    lockAccessCallback(active);
}

void BluetoothDriver::writeBuffer(const uint8_t *data, uint32_t len) {
    audioDataCallback(data, len);
}

void BluetoothDriver::start() {
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

    esp_err_t err;

    esp_bt_controller_config_t btCfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    if ((err = esp_bt_controller_init(&btCfg)) != ESP_OK) {
        EUPH_LOG(error, "Bluetooth", "Bluetooth controller init failed: %s",
                 esp_err_to_name(err));
        return;
    }

    if ((err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        EUPH_LOG(error, "Bluetooth", "Bluetooth controller enable failed: %s",
                 esp_err_to_name(err));
        return;
    }

    if ((err = esp_bluedroid_init()) != ESP_OK) {
        EUPH_LOG(error, "Bluetooth", "Bluetooth bluedroid init failed: %s",
                 esp_err_to_name(err));
        return;
    }

    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        EUPH_LOG(error, "Bluetooth", "Bluetooth bluedroid enable failed: %s",
                 esp_err_to_name(err));
        return;
    }

    // if ((err = esp_spp_register_callback(esp_bt_spp_cb)) != ESP_OK) {
    //     EUPH_LOG(error, "Bluetooth", "Bluetooth spp register failed: %s",
    //              esp_err_to_name(err));
    //     return;
    // }

    // if ((err = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK) {
    //     EUPH_LOG(error, "Bluetooth", "Bluetooth spp init failed: %s",
    //              esp_err_to_name(err));
    //     return;
    // }

    bt_app_task_start_up();
    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0,
                         NULL);

/* Set default parameters for Secure Simple Pairing */
#if (CONFIG_BT_SSP_ENABLED == true)

    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_OUT;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;

    // check if this->pin is a number

    bool stringIsPin = true;

    esp_bt_pin_code_t pinCode;

    for (int i = 0; i < this->pin.length(); i++) {
        if (!std::isdigit(this->pin[i])) {
            stringIsPin = false;
            break;
        } else {
            pinCode[i] = this->pin[i];
        }
    }

    if (stringIsPin && this->pin.length() <= 16) {
        esp_bt_gap_set_pin(pin_type, this->pin.length(), pinCode);
    } else {
        pinCode[0] = '1';
        pinCode[1] = '2';
        pinCode[2] = '3';
        pinCode[3] = '4';
        esp_bt_gap_set_pin(pin_type, 4, pinCode);
    }
}

void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(BT_AV_TAG, "authentication success: %s",
                     param->auth_cmpl.device_name);
            esp_log_buffer_hex(BT_AV_TAG, param->auth_cmpl.bda,
                               ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(BT_AV_TAG, "authentication failed, status:%d",
                     param->auth_cmpl.stat);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(BT_AV_TAG,
                 "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d",
                 param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d",
                 param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif
    default: {
        ESP_LOGI(BT_AV_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

#define SPP_TAG "SPP_TAG"
static void esp_bt_spp_cb(esp_spp_cb_event_t event,
                          esp_spp_cb_param_t *param) { // esp_err_t err;
    switch (event) {
    case ESP_SPP_INIT_EVT: {
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        auto name = globalBtDriver->getName();
        esp_bt_dev_set_device_name(name.c_str());
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
                                 ESP_BT_GENERAL_DISCOVERABLE);
        // esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0,
        //                   "SPP_SERVER");
    } break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT: {
    }

    break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        spp_handle = param->open.handle;
        printf("%d spp handle \n", spp_handle);
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        break;
    default:
        break;
    }
}

static void bt_av_hdl_stack_evt(uint16_t event, void *p_param) {
    ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_APP_EVT_STACK_UP: {
        /* set up device name */
        auto name = globalBtDriver->getName();
        esp_bt_dev_set_device_name(name.c_str());

        esp_bt_gap_register_callback(bt_app_gap_cb);

        /* initialize AVRCP controller */
        esp_avrc_ct_init();
        esp_avrc_ct_register_callback(bt_app_rc_ct_cb);

        /* initialize AVRCP target */
        assert(esp_avrc_tg_init() == ESP_OK);
        esp_avrc_tg_register_callback(bt_app_rc_tg_cb);

        esp_avrc_rn_evt_cap_mask_t evt_set = {0};
        esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set,
                                           ESP_AVRC_RN_VOLUME_CHANGE);
        assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);

        /* initialize A2DP sink */
        if (esp_a2d_register_callback(&bt_app_a2d_cb) != ESP_OK) {
            printf("A2DP Register callback failed..\n");
        }

        if (esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb) != ESP_OK) {
            printf("A2DP Sink data-callback register failed..\n");
        }

        if (esp_a2d_sink_init() != ESP_OK) {
            printf("A2DP Sink init. failed..\n");
        }

        /* set discoverable and connectable mode, wait to be connected */
        if (esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
                                     ESP_BT_GENERAL_DISCOVERABLE) != ESP_OK) {
            printf(
                "Bluetooth set mode, Connectable & Discoverable, failed..\n");
        }
        break;
    }
    default:
        ESP_LOGE(BT_AV_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
