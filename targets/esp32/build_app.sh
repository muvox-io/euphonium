#!/bin/bash
mkdir -p build
cd app/
idf.py build
cd ..

# copy build firmware
cp app/build/storage.bin build/storage.bin
cp app/build/euphonium-esp32.bin build/euphonium-esp32.bin


