#!/bin/bash
set -e
cd recovery
idf.py build
cd ..
cp recovery/build/recovery.bin build/recovery.bin
esptool.py -p $1 -b 460800 --before default_reset --after hard_reset --chip esp32 write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x90000 build/recovery.bin
