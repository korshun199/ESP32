#!/usr/bin/env bash
set -Eeuo pipefail

export IDF_TOOLS_PATH=/home/oleg/.espressif/esp-idf-6.0.2
source /home/oleg/esp/esp-idf/export.sh >/tmp/esp32-build.log
cd /home/work/ESP32

idf.py -B build-6.0.2 build
echo "Готово: build-6.0.2/esp32_blink.bin"
