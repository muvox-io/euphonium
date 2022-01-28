#include "GPIODriver.h"

void gpioSetState(int gpio, int state) {
    gpio_config_t  io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BIT64(gpio);
    io_conf.pull_down_en = (gpio_pulldown_t) 0;
    io_conf.pull_up_en = (gpio_pullup_t) 0;
    gpio_config(&io_conf);
    gpio_set_level((gpio_num_t) gpio, state);
}

void exportGPIODriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("gpio_digital_write", &gpioSetState);
}