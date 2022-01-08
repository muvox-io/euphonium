#!/bin/bash
set -e
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}### building recovery.bin ${NC}"
sh build_recovery.sh

echo -e "${YELLOW}### building the app + web assets ${NC}"
sh build_app.sh

echo -e "${YELLOW}### copy scripts to storage ${NC}"
cp -r ../../euphonium/scripts/. app/spiffs/

echo -e "${YELLOW}### make scripts.tar file ${NC}"
cd app/spiffs
tar -cvf scripts.tar *
cd ../../
cp app/spiffs/scripts.tar scripts.tar

echo -e "${YELLOW}### building release file ${NC}"
tar -czvf euphonium-$EUPH_VER.tar.gz build/*.bin build/partition_table/partition-table.bin build/bootloader/bootloader.bin 
#flash_all.sh

echo -e "${YELLOW}### building OTA file ${NC}"
cp build/euphonium-esp32.bin ota.bin
sha256sum ota.bin | cut -d " " -f 1 > ota_checksum.txt
sha256sum scripts.tar | cut -d " " -f 1 > scripts_checksum.txt
