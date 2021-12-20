#!/bin/bash
sh build_recovery.sh
sh build_app.sh
cd ../../euphonium/scripts
tar -cvf scripts.tar *
cd ../../targets/esp32
cp ../../euphonium/scripts/scripts.tar build/scripts.tar

