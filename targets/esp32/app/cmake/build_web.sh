#!/usr/bin/env bash
set -e
cd ../../../web
npm run build
rm -rf ../targets/esp32/app/spiffs
mkdir ../targets/esp32/app/spiffs

cp -R dist/. ../targets/esp32/app/spiffs/.
cp -R ../euphonium/scripts/. ../targets/esp32/app/spiffs
