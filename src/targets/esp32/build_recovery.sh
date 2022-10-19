#!/bin/bash
set -e
mkdir -p build
cd recovery/
idf.py build
cd ..

# copy build firmware
rm -rf build
mkdir -p build/bootloader
mkdir -p build/partition_table
cp -r recovery/build/bootloader/bootloader.bin build/bootloader/bootloader.bin
cp -r recovery/build/partition_table/partition-table.bin build/partition_table/partition-table.bin
cp -r recovery/build/recovery.bin build/recovery.bin

