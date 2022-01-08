#!/bin/bash
set -e
esptool.py -p $1 -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x90000 build/recovery.bin 0x140000 build/euphonium-esp32.bin 0x10000 build/storage.bin

