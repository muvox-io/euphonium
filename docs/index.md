# Euphonium documentation
Euphonium is a highly extensible audio streaming software. It's multiplatform, also supporting low cost MCU's like espressif ESP32 series.

## Features:
- Clean and lightweight web-ui
- Supports multiple plugins: Spotify streaming, Bluetooth (on esp32), Web Radio, Jellyfin
- Fully modular architecture
- Easily extendible and portable to new hardware, thanks to the internal Berry-lang powered scripting engine.

## Required hardware for esp32:
- Any ESP32 chip with at least 4MB of flash and 4MB of PSRAM. Wrover chips are usually a good call :)

## Installing a prebuilt version
Feel free to try Euphonium by flashing a newest [release](https://github.com/feelfreelinux/euphonium/releases). All you need to do is to have `esptool.py` installed on your system (comes with esp-idf). Unpack the `euphonium-{version}.tar.gz` and run:
`flash_all.sh {your serial port}` to install it.

## Building Euphonium yourself
[See here for development setup instructions](https://github.com/feelfreelinux/euphonium/wiki/Development-environment).
