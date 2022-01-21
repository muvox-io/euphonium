# Getting started with Euphonium
The fastest way to give euphonium a go is to use prebuilt version from github releases.

## Required hardware for esp32:
- Any ESP32 chip with at least 4MB of flash and 4MB of PSRAM. Wrover chips are usually a good call :)

## Installing a prebuilt version
Flashing a prebuilt version only consists of a few steps:
- Download [the newest release](https://github.com/feelfreelinux/euphonium/releases). (File with name `euphonium-vX.X.X.tar.gz`)
- Unpack it
- Flash it by running `sh flash_all.sh {your serial port}`
- 
## Configuring the system
- Reboot the board after flashing.
- After few seconds "Euphonium" network should appear.
- Connect to it. The password is `euphonium`.
- You will now be taken to the WiFi configuration page. _In case the page does not open automatically - connect to `192.168.4.1`_

## DAC configuration
In the "DAC configuration" section you can configure the DAC. Euphonium provides an automated configurator for more popular systems. But you can also manually select, and configure the DAC pins.


## Enjoy your music
This is all that's required to get euphonium up and running.