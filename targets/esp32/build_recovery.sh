#!/bin/bash
mkdir -p build
cd recovery/
idf.py build
cd ..

# copy build firmware
rm -rf build
cp -r recovery/build build
