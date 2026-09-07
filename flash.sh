#!/usr/bin/env bash
set -Eeuo pipefail

export IDF_TOOLS_PATH=/home/oleg/.espressif/esp-idf-6.0.2
source /home/oleg/esp/esp-idf/export.sh >/tmp/esp32-flash.log
cd /home/work/ESP32

idf.py -B build-6.0.2 build
read -r -p "Записать прошивку в /dev/ttyUSB0? Введите FLASH: " answer
[[ "$answer" == FLASH ]] || { echo "Отменено"; exit 1; }
idf.py -B build-6.0.2 -p /dev/ttyUSB0 -b 115200 flash
echo "Готово: плата прошита"
