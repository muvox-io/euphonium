/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

#include "bt_app_av.h"
#include "bt_app_core.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sys/lock.h"

/* AVRCP used transaction labels */
#define APP_RC_CT_TL_GET_CAPS (0)
#define APP_RC_CT_TL_GET_META_DATA (1)
#define APP_RC_CT_TL_RN_TRACK_CHANGE (2)
#define APP_RC_CT_TL_RN_PLAYBACK_CHANGE (3)
#define APP_RC_CT_TL_RN_PLAY_POS_CHANGE (4)

static int tl;

/* Application layer causes delay value */
#define APP_DELAY_VALUE 50  // 5ms

#define METADATA_LEN 128

static EXT_RAM_BSS_ATTR struct {
  char artist[METADATA_LEN + 1];
  char album[METADATA_LEN + 1];
  char title[METADATA_LEN + 1];
  int duration;
  bool updated;
} s_metadata;

extern void local_set_volume(uint8_t volume);
extern void local_active_audio(bool active);
extern void local_metadata_updated(char* artist, char* album, char* title,
                                   int duration);

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* allocate new meta buffer */
static void bt_app_alloc_meta_buffer(esp_avrc_ct_cb_param_t* param);
/* handler for new track is loaded */
static void bt_av_new_track(void);
/* handler for track status change */
static void bt_av_playback_changed(void);
/* handler for track playing position change */
static void bt_av_play_pos_changed(void);
/* notification event handler */
static void bt_av_notify_evt_handler(uint8_t event_id,
                                     esp_avrc_rn_param_t* event_parameter);
/* set volume by remote controller */
static void volume_set_by_controller(uint8_t volume);
/* set volume by local host */
static void volume_set_by_local_host(uint8_t volume);
/* simulation volume change */
static void volume_change_simulation(void* arg);
/* a2dp event handler */
static void bt_av_hdl_a2d_evt(uint16_t event, void* p_param);
/* avrc controller event handler */
static void bt_av_hdl_avrc_ct_evt(uint16_t event, void* p_param);
/* avrc target event handler */
static void bt_av_hdl_avrc_tg_evt(uint16_t event, void* p_param);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

static uint32_t s_pkt_cnt = 0; /* count for audio packet */
static esp_a2d_audio_state_t s_audio_state = ESP_A2D_AUDIO_STATE_STOPPED;
/* audio stream datapath state */
static const char* s_a2d_conn_state_str[] = {"Disconnected", "Connecting",
                                             "Connected", "Disconnecting"};
/* connection state in string */
static const char* s_a2d_audio_state_str[] = {"Suspended", "Stopped",
                                              "Started"};
/* audio stream datapath state in string */
static esp_avrc_rn_evt_cap_mask_t s_avrc_peer_rn_cap;
/* AVRC target notification capability bit mask */
static _lock_t s_volume_lock;
static uint8_t s_volume = 0; /* local volume value */
static bool s_volume_notify; /* notify volume change or not */

/********************************
 * STATIC FUNCTION DEFINITIONS
 *******************************/

static void bt_app_alloc_meta_buffer(esp_avrc_ct_cb_param_t* param) {
  esp_avrc_ct_cb_param_t* rc = (esp_avrc_ct_cb_param_t*)(param);
  uint8_t* attr_text = (uint8_t*)malloc(rc->meta_rsp.attr_length + 1);

  memcpy(attr_text, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
  attr_text[rc->meta_rsp.attr_length] = 0;
  rc->meta_rsp.attr_text = attr_text;
}

static void bt_av_new_track(void) {
  /* request metadata */
  uint8_t attr_mask = ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST |
                      ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_GENRE;
  esp_avrc_ct_send_metadata_cmd(APP_RC_CT_TL_GET_META_DATA, attr_mask);

  /* register notification if peer support the event_id */
  if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST,
                                         &s_avrc_peer_rn_cap,
                                         ESP_AVRC_RN_TRACK_CHANGE)) {
    esp_avrc_ct_send_register_notification_cmd(APP_RC_CT_TL_RN_TRACK_CHANGE,
                                               ESP_AVRC_RN_TRACK_CHANGE, 0);
  }
}

static void bt_av_playback_changed(void) {
  /* register notification if peer support the event_id */
  if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST,
                                         &s_avrc_peer_rn_cap,
                                         ESP_AVRC_RN_PLAY_STATUS_CHANGE)) {
    esp_avrc_ct_send_register_notification_cmd(
        APP_RC_CT_TL_RN_PLAYBACK_CHANGE, ESP_AVRC_RN_PLAY_STATUS_CHANGE, 0);
  }
}

static void bt_av_play_pos_changed(void) {
  /* register notification if peer support the event_id */
  if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST,
                                         &s_avrc_peer_rn_cap,
                                         ESP_AVRC_RN_PLAY_POS_CHANGED)) {
    esp_avrc_ct_send_register_notification_cmd(
        APP_RC_CT_TL_RN_PLAY_POS_CHANGE, ESP_AVRC_RN_PLAY_POS_CHANGED, 10);
  }
}

static void bt_av_notify_evt_handler(uint8_t event_id,
                                     esp_avrc_rn_param_t* event_parameter) {
  switch (event_id) {
    /* when new track is loaded, this event comes */
    case ESP_AVRC_RN_TRACK_CHANGE:
      bt_av_new_track();
      break;
    /* when track status changed, this event comes */
    case ESP_AVRC_RN_PLAY_STATUS_CHANGE:
      ESP_LOGI(BT_AV_TAG, "Playback status changed: 0x%x",
               event_parameter->playback);
      bt_av_playback_changed();
      break;
    /* when track playing position changed, this event comes */
    case ESP_AVRC_RN_PLAY_POS_CHANGED:
      ESP_LOGI(BT_AV_TAG, "Play position changed: %d-ms",
               event_parameter->play_pos);
      bt_av_play_pos_changed();
      break;
    /* others */
    default:
      ESP_LOGI(BT_AV_TAG, "unhandled event: %d", event_id);
      break;
  }
}

static void volume_set_by_controller(uint8_t volume) {
  uint8_t volume_percent = (uint8_t) ((uint32_t)volume * 100 / 0x7f);

  ESP_LOGI(BT_RC_TG_TAG, "Volume is set by remote controller to: %d%%",
           volume_percent);

  local_set_volume(volume_percent);
  /* set the volume in protection of lock */
  _lock_acquire(&s_volume_lock);
  s_volume = volume;
  _lock_release(&s_volume_lock);
}

static void volume_set_by_local_host(uint8_t volume) {
  ESP_LOGI(BT_RC_TG_TAG, "Volume is set locally to: %d%%",
           (uint32_t)volume * 100 / 0x7f);
  /* set the volume in protection of lock */
  _lock_acquire(&s_volume_lock);
  s_volume = volume;
  _lock_release(&s_volume_lock);

  /* send notification response to remote AVRCP controller */
  if (s_volume_notify) {
    esp_avrc_rn_param_t rn_param;
    rn_param.volume = s_volume;
    esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_CHANGED,
                            &rn_param);
    s_volume_notify = false;
  }
}

static void bt_av_hdl_a2d_evt(uint16_t event, void* p_param) {
  ESP_LOGD(BT_AV_TAG, "%s event: %d", __func__, event);

  esp_a2d_cb_param_t* a2d = NULL;

  switch (event) {
    /* when connection state changed, this event comes */
    case ESP_A2D_CONNECTION_STATE_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      uint8_t* bda = a2d->conn_stat.remote_bda;
      ESP_LOGI(BT_AV_TAG,
               "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
               s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1],
               bda[2], bda[3], bda[4], bda[5]);
      if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
                                 ESP_BT_GENERAL_DISCOVERABLE);
        // SHUTDOWN I2S @TODO
      } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        // START I2S
        esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE,
                                 ESP_BT_NON_DISCOVERABLE);
      } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTING) {
        // CONNECTED
      }
      break;
    }
    /* when audio stream transmission state changed, this event comes */
    case ESP_A2D_AUDIO_STATE_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      ESP_LOGI(BT_AV_TAG, "A2DP audio state: %s",
               s_a2d_audio_state_str[a2d->audio_stat.state]);
      s_audio_state = a2d->audio_stat.state;
      if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
        local_active_audio(true);
        s_pkt_cnt = 0;
      }
      if (ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND == a2d->audio_stat.state) {
        local_active_audio(false);
      }

      break;
    }
    /* when audio codec is configured, this event comes */
    case ESP_A2D_AUDIO_CFG_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      ESP_LOGI(BT_AV_TAG, "A2DP audio stream configuration, codec type: %d",
               a2d->audio_cfg.mcc.type);
      /* for now only SBC stream is supported */
      if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
        int sample_rate = 16000;
        int ch_count = 2;
        char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
        if (oct0 & (0x01 << 6)) {
          sample_rate = 32000;
        } else if (oct0 & (0x01 << 5)) {
          sample_rate = 44100;
        } else if (oct0 & (0x01 << 4)) {
          sample_rate = 48000;
        }

        if (oct0 & (0x01 << 3)) {
          ch_count = 1;
        }

        // i2s_set_clk(0, sample_rate, 16, ch_count);

        ESP_LOGI(BT_AV_TAG, "Configure audio player: %x-%x-%x-%x",
                 a2d->audio_cfg.mcc.cie.sbc[0], a2d->audio_cfg.mcc.cie.sbc[1],
                 a2d->audio_cfg.mcc.cie.sbc[2], a2d->audio_cfg.mcc.cie.sbc[3]);
        ESP_LOGI(BT_AV_TAG, "Audio player configured, sample rate: %d",
                 sample_rate);
      }
      break;
    }
    /* when a2dp init or deinit completed, this event comes */
    case ESP_A2D_PROF_STATE_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      if (ESP_A2D_INIT_SUCCESS == a2d->a2d_prof_stat.init_state) {
        ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Init Complete");
      } else {
        ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Deinit Complete");
      }
      break;
    }
    /* When protocol service capabilities configured, this event comes */
    case ESP_A2D_SNK_PSC_CFG_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      ESP_LOGI(BT_AV_TAG, "protocol service capabilities configured: 0x%x ",
               a2d->a2d_psc_cfg_stat.psc_mask);
      if (a2d->a2d_psc_cfg_stat.psc_mask & ESP_A2D_PSC_DELAY_RPT) {
        ESP_LOGI(BT_AV_TAG, "Peer device support delay reporting");
      } else {
        ESP_LOGI(BT_AV_TAG, "Peer device unsupport delay reporting");
      }
      break;
    }
    /* when set delay value completed, this event comes */
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      if (ESP_A2D_SET_INVALID_PARAMS ==
          a2d->a2d_set_delay_value_stat.set_state) {
        ESP_LOGI(BT_AV_TAG, "Set delay report value: fail");
      } else {
        ESP_LOGI(BT_AV_TAG,
                 "Set delay report value: success, delay_value: %u * 1/10 ms",
                 a2d->a2d_set_delay_value_stat.delay_value);
      }
      break;
    }
    /* when get delay value completed, this event comes */
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
      a2d = (esp_a2d_cb_param_t*)(p_param);
      ESP_LOGI(BT_AV_TAG, "Get delay report value: delay_value: %u * 1/10 ms",
               a2d->a2d_get_delay_value_stat.delay_value);
      /* Default delay value plus delay caused by application layer */
      esp_a2d_sink_set_delay_value(a2d->a2d_get_delay_value_stat.delay_value +
                                   APP_DELAY_VALUE);
      break;
    }
    /* others */
    default:
      ESP_LOGE(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
      break;
  }
}

static void bt_av_hdl_avrc_ct_evt(uint16_t event, void* p_param) {
  ESP_LOGD(BT_RC_CT_TAG, "%s event: %d", __func__, event);

  esp_avrc_ct_cb_param_t* rc = (esp_avrc_ct_cb_param_t*)(p_param);

  switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
      uint8_t* bda = rc->conn_stat.remote_bda;
      ESP_LOGI(
          BT_RC_CT_TAG,
          "AVRC conn_state event: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
          rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4],
          bda[5]);

      if (rc->conn_stat.connected) {
        /* get remote supported event_ids of peer AVRCP Target */
        esp_avrc_ct_send_get_rn_capabilities_cmd(APP_RC_CT_TL_GET_CAPS);
      } else {
        /* clear peer notification capability record */
        s_avrc_peer_rn_cap.bits = 0;
      }
      break;
    }
    /* when passthrough responsed, this event comes */
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
      ESP_LOGI(BT_RC_CT_TAG,
               "AVRC passthrough rsp: key_code 0x%x, key_state %d",
               rc->psth_rsp.key_code, rc->psth_rsp.key_state);
      break;
    }
    /* when metadata responsed, this event comes */
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
      ESP_LOGI(BT_RC_CT_TAG, "AVRC metadata rsp: attribute id 0x%x, %s",
               rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);

      if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_PLAYING_TIME)
        s_metadata.duration = atoi((char*)rc->meta_rsp.attr_text);
      else if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_TITLE)
        strncpy(s_metadata.title, (char*)rc->meta_rsp.attr_text, METADATA_LEN);
      else if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_ARTIST)
        strncpy(s_metadata.artist, (char*)rc->meta_rsp.attr_text, METADATA_LEN);
      else if (rc->meta_rsp.attr_id == ESP_AVRC_MD_ATTR_ALBUM)
        strncpy(s_metadata.album, (char*)rc->meta_rsp.attr_text, METADATA_LEN);

      local_metadata_updated(s_metadata.artist, s_metadata.album,
                             s_metadata.title, s_metadata.duration);
      free(rc->meta_rsp.attr_text);
      break;
    }
    /* when notified, this event comes */
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
      ESP_LOGI(BT_RC_CT_TAG, "AVRC event notification: %d",
               rc->change_ntf.event_id);
      bt_av_notify_evt_handler(rc->change_ntf.event_id,
                               &rc->change_ntf.event_parameter);
      break;
    }
    /* when feature of remote device indicated, this event comes */
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
      ESP_LOGI(BT_RC_CT_TAG, "AVRC remote features %x, TG features %x",
               rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
      break;
    }
    /* when notification capability of peer device got, this event comes */
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
      ESP_LOGI(BT_RC_CT_TAG, "remote rn_cap: count %d, bitmask 0x%x",
               rc->get_rn_caps_rsp.cap_count, rc->get_rn_caps_rsp.evt_set.bits);
      s_avrc_peer_rn_cap.bits = rc->get_rn_caps_rsp.evt_set.bits;
      bt_av_new_track();
      bt_av_playback_changed();
      bt_av_play_pos_changed();
      break;
    }
    /* others */
    default:
      ESP_LOGE(BT_RC_CT_TAG, "%s unhandled event: %d", __func__, event);
      break;
  }
}

static void bt_av_hdl_avrc_tg_evt(uint16_t event, void* p_param) {
  ESP_LOGD(BT_RC_TG_TAG, "%s event: %d", __func__, event);

  esp_avrc_tg_cb_param_t* rc = (esp_avrc_tg_cb_param_t*)(p_param);

  switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_TG_CONNECTION_STATE_EVT: {
      uint8_t* bda = rc->conn_stat.remote_bda;
      ESP_LOGI(BT_RC_TG_TAG,
               "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
               rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4],
               bda[5]);
      if (rc->conn_stat.connected) {

      } else {
        ESP_LOGI(BT_RC_TG_TAG, "Stop volume change simulation");
      }
      break;
    }
    /* when passthrough commanded, this event comes */
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT: {
      ESP_LOGI(BT_RC_TG_TAG,
               "AVRC passthrough cmd: key_code 0x%x, key_state %d",
               rc->psth_cmd.key_code, rc->psth_cmd.key_state);
      break;
    }
    /* when absolute volume command from remote device set, this event comes */
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
      ESP_LOGI(BT_RC_TG_TAG, "AVRC set absolute volume: %d%%",
               (int)rc->set_abs_vol.volume * 100 / 0x7f);
      volume_set_by_controller(rc->set_abs_vol.volume);
      break;
    }
    /* when notification registered, this event comes */
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
      ESP_LOGI(BT_RC_TG_TAG,
               "AVRC register event notification: %d, param: 0x%x",
               rc->reg_ntf.event_id, rc->reg_ntf.event_parameter);
      if (rc->reg_ntf.event_id == ESP_AVRC_RN_VOLUME_CHANGE) {
        s_volume_notify = true;
        esp_avrc_rn_param_t rn_param;
        rn_param.volume = s_volume;
        esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE,
                                ESP_AVRC_RN_RSP_INTERIM, &rn_param);
      }
      break;
    }
    /* when feature of remote device indicated, this event comes */
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT: {
      ESP_LOGI(BT_RC_TG_TAG, "AVRC remote features: %x, CT features: %x",
               rc->rmt_feats.feat_mask, rc->rmt_feats.ct_feat_flag);
      break;
    }
    /* others */
    default:
      ESP_LOGE(BT_RC_TG_TAG, "%s unhandled event: %d", __func__, event);
      break;
  }
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* param) {
  switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    case ESP_A2D_PROF_STATE_EVT:
    case ESP_A2D_SNK_PSC_CFG_EVT:
    case ESP_A2D_SNK_SET_DELAY_VALUE_EVT:
    case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
      bt_app_work_dispatch(bt_av_hdl_a2d_evt, event, param,
                           sizeof(esp_a2d_cb_param_t), NULL);
      break;
    }
    default:
      ESP_LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
      break;
  }
}

void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event,
                     esp_avrc_ct_cb_param_t* param) {
  switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT:
      bt_app_alloc_meta_buffer(param);
      /* fall through */
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
      bt_app_work_dispatch(bt_av_hdl_avrc_ct_evt, event, param,
                           sizeof(esp_avrc_ct_cb_param_t), NULL);
      break;
    }
    default:
      ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
      break;
  }
}

void bt_app_rc_tg_cb(esp_avrc_tg_cb_event_t event,
                     esp_avrc_tg_cb_param_t* param) {
  switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT:
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT:
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT:
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT:
    case ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT:
      bt_app_work_dispatch(bt_av_hdl_avrc_tg_evt, event, param,
                           sizeof(esp_avrc_tg_cb_param_t), NULL);
      break;
    default:
      ESP_LOGE(BT_RC_TG_TAG, "Invalid AVRC event: %d", event);
      break;
  }
}

void bt_disconnect(void) {
  esp_avrc_ct_send_passthrough_cmd(tl++ & 0x0f, ESP_AVRC_PT_CMD_STOP,
                                   ESP_AVRC_PT_CMD_STATE_PRESSED);
}
