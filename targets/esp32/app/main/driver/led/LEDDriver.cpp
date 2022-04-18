#include "LEDDriver.h"
#include "BellUtils.h"

std::map<uint8_t, std::unique_ptr<LedStrip>> registeredStrips;

void setPixelRgb(int channel, int index, int r, int g, int b) {
    (*registeredStrips[channel].get())[index] =
        Rgb{static_cast<uint8_t>(r), static_cast<uint8_t>(g),
            static_cast<uint8_t>(b)};
}

void showPixels(int channel) { registeredStrips[channel]->show(); }

void createStrip(int channel, int type, int length, int pin, int brightness) {
    auto ledType = LED_WS2812;
    switch (type) {
        case 1:
            ledType = LED_WS2812B;
            break;
        case 2:
            ledType = LED_SK6812;
            break;
        case 3:
            ledType = LED_WS2813;
            break;
        default:
            ledType = LED_WS2812;
    }

    registeredStrips.insert(
        {channel, std::make_unique<LedStrip>(ledType, length, pin, channel, brightness,
                                             DoubleBuffer)});
}

void exportLEDDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("create_strip", &createStrip, "led");
    berry->export_function("set_pixel_rgb", &setPixelRgb, "led");
    berry->export_function("show", &showPixels, "led");
}
