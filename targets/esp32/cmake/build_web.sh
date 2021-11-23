#!/usr/bin/env bash
set -e
cd ../../web
npm run build
rm -rf ../targets/esp32/spiffs
mkdir ../targets/esp32/spiffs

cp -R dist/. ../targets/esp32/spiffs/.
cp -R ../euphonium/lua/. ../targets/esp32/spiffs