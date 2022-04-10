#include "LEDDriver.h"
#include "BellUtils.h"

static struct led_color_t led_strip_buf_1[19];
static struct led_color_t led_strip_buf_2[19];

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    std::cout << "INITIALIZING LED" << std::endl;
    struct led_strip_t led_strip = {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .led_strip_length = 12,
        .rmt_channel = RMT_CHANNEL_0,
        .rmt_interrupt_num = 19U,
        .gpio = GPIO_NUM_21,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
    };
    led_strip.access_semaphore = xSemaphoreCreateBinary();
    BELL_SLEEP_MS(500);

    bool led_init_ok = led_strip_init(&led_strip);
    BELL_SLEEP_MS(500);
    led_strip_set_pixel_rgb(&led_strip, 1, 100, 10, 10);
    BELL_SLEEP_MS(10);
    led_strip_show(&led_strip);
}
