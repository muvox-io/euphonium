# ESP32 Euphonium integration

Integrates Euphonium with esp32 platform. To configure WiFi SSID and password use idf.py menuconfig.


## Selecting your ESP32

Euphonium has been tested to be working on AI-Thinker ESP32 Audio kit board: https://www.aliexpress.com/wholesale?SearchText=esp32-a1s

If you want to use another board, notice that the typical ESP32 RAM size is 160-520kb, which is not enough to run Euphonium.

To run Euphonium, select a ESP32 with PSRAM. ESP32-WROVER are confirmed to work.
Euphonium will not run on a ESP32-WROOM unless the board has external PSRAM.

ref. https://products.espressif.com/#/product-selector

