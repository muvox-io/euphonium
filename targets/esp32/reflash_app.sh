#!/bin/bash
set -e
# sh rebuild_web.sh
cd app
idf.py build
cd ..
cp app/build/euphonium-esp32.bin build/euphonium-esp32.bin
esptool.py -p $1 -b 460800 --before default_reset --after hard_reset --chip esp32 write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x140000 build/euphonium-esp32.bin
