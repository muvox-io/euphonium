#include "LEDDriver.h"
#include "BellUtils.h"

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    led_strip_t strip = {
        .type = LED_STRIP_WS2812,
        .brightness = 255,
        .length = 12,
        .gpio = (gpio_num_t) 21,
        .buf = NULL,
    };
    led_strip_install();
    led_strip_init(&strip);
    led_strip_fill(&strip, 0, 12, 100, 10, 10);
    led_strip_flush(&strip);
    // std::cout << "INITIALIZING LED" << std::endl;
    // struct led_strip_t led_strip = {
    //     .rgb_led_type = RGB_LED_TYPE_WS2812,
    //     .led_strip_length = 12,
    //     .rmt_channel = RMT_CHANNEL_0,
    //     .rmt_interrupt_num = 19U,
    //     .gpio = GPIO_NUM_21,
    //     .led_strip_buf_1 = led_strip_buf_1,
    //     .led_strip_buf_2 = led_strip_buf_2,
    // };
    // led_strip.access_semaphore = xSemaphoreCreateBinary();
    // BELL_SLEEP_MS(500);

    // bool led_init_ok = led_strip_init(&led_strip);
    // BELL_SLEEP_MS(500);
    // led_strip_set_pixel_rgb(&led_strip, 1, 100, 10, 10);
    // BELL_SLEEP_MS(10);
    // led_strip_show(&led_strip);
}
