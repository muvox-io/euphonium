/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "bt_app_core.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

#define RINGBUF_HIGHEST_WATER_LEVEL (32 * 1024)
#define RINGBUF_PREFETCH_WATER_LEVEL (20 * 1024)

enum {
  RINGBUFFER_MODE_PROCESSING, /* ringbuffer is buffering incoming audio data, I2S is working */
  RINGBUFFER_MODE_PREFETCHING, /* ringbuffer is buffering incoming audio data, I2S is waiting */
  RINGBUFFER_MODE_DROPPING /* ringbuffer is not buffering (dropping) incoming audio data, I2S is working */
};

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* handler for application task */
static void bt_app_task_handler(void* arg);
/* message sender */
static bool bt_app_send_msg(bt_app_msg_t* msg);
/* handle dispatched messages */
static void bt_app_work_dispatched(bt_app_msg_t* msg);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

static QueueHandle_t s_bt_app_task_queue = NULL; /* handle of work queue */
static TaskHandle_t s_bt_app_task_handle =
    NULL; /* handle of application task  */
static uint16_t ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static bool bt_app_send_msg(bt_app_msg_t* msg) {
  if (msg == NULL) {
    return false;
  }

  /* send the message to work queue */
  if (xQueueSend(s_bt_app_task_queue, msg, 10 / portTICK_PERIOD_MS) != pdTRUE) {
    ESP_LOGE(BT_APP_CORE_TAG, "%s xQueue send failed", __func__);
    return false;
  }
  return true;
}

static void bt_app_work_dispatched(bt_app_msg_t* msg) {
  if (msg->cb) {
    msg->cb(msg->event, msg->param);
  }
}

void bt_app_handle_queue(void) {
  bt_app_msg_t msg;

  /* receive message from work queue and handle it */
  if (pdTRUE ==
      xQueueReceive(s_bt_app_task_queue, &msg, (TickType_t)portMAX_DELAY)) {
    ESP_LOGD(BT_APP_CORE_TAG, "%s, signal: 0x%x, event: 0x%x", __func__,
             msg.sig, msg.event);

    switch (msg.sig) {
      case BT_APP_SIG_WORK_DISPATCH:
        bt_app_work_dispatched(&msg);
        break;
      default:
        ESP_LOGW(BT_APP_CORE_TAG, "%s, unhandled signal: %d", __func__,
                 msg.sig);
        break;
    } /* switch (msg.sig) */

    if (msg.param) {
      free(msg.param);
    }
  }
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void* p_params,
                          int param_len, bt_app_copy_cb_t p_copy_cback) {
  ESP_LOGD(BT_APP_CORE_TAG, "%s event: 0x%x, param len: %d", __func__, event,
           param_len);

  bt_app_msg_t msg;
  memset(&msg, 0, sizeof(bt_app_msg_t));

  msg.sig = BT_APP_SIG_WORK_DISPATCH;
  msg.event = event;
  msg.cb = p_cback;

  if (param_len == 0) {
    return bt_app_send_msg(&msg);
  } else if (p_params && param_len > 0) {
    if ((msg.param = malloc(param_len)) != NULL) {
      memcpy(msg.param, p_params, param_len);
      /* check if caller has provided a copy callback to do the deep copy */
      if (p_copy_cback) {
        p_copy_cback(msg.param, p_params, param_len);
      }
      return bt_app_send_msg(&msg);
    }
  }

  return false;
}

void bt_app_task_init_queue(void) {
  s_bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
}

void bt_app_task_shut_down(void) {
  if (s_bt_app_task_handle) {
    vTaskDelete(s_bt_app_task_handle);
    s_bt_app_task_handle = NULL;
  }
  if (s_bt_app_task_queue) {
    vQueueDelete(s_bt_app_task_queue);
    s_bt_app_task_queue = NULL;
  }
}

size_t write_ringbuf(const uint8_t* data, size_t size) {
  /* 
    size_t item_size = 0;
    BaseType_t done = pdFALSE;

    if (ringbuffer_mode == RINGBUFFER_MODE_DROPPING) {
        ESP_LOGW(BT_APP_CORE_TAG, "ringbuffer is full, drop this packet!");
        vRingbufferGetInfo(s_ringbuf_i2s, NULL, NULL, NULL, NULL, &item_size);
        if (item_size <= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(BT_APP_CORE_TAG, "ringbuffer data decreased! mode changed: RINGBUFFER_MODE_PROCESSING");
            ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
        }
        return 0;
    }

    done = xRingbufferSend(s_ringbuf_i2s, (void *)data, size, (TickType_t)0);

    if (!done) {
        ESP_LOGW(BT_APP_CORE_TAG, "ringbuffer overflowed, ready to decrease data! mode changed: RINGBUFFER_MODE_DROPPING");
        ringbuffer_mode = RINGBUFFER_MODE_DROPPING;
    }

    if (ringbuffer_mode == RINGBUFFER_MODE_PREFETCHING) {
        vRingbufferGetInfo(s_ringbuf_i2s, NULL, NULL, NULL, NULL, &item_size);
        if (item_size >= RINGBUF_PREFETCH_WATER_LEVEL) {
            ESP_LOGI(BT_APP_CORE_TAG, "ringbuffer data increased! mode changed: RINGBUFFER_MODE_PROCESSING");
            ringbuffer_mode = RINGBUFFER_MODE_PROCESSING;
            if (pdFALSE == xSemaphoreGive(s_i2s_write_semaphore)) {
                ESP_LOGE(BT_APP_CORE_TAG, "semphore give failed");
            }
        }
    }

  */
  return size;
}
