#!/usr/bin/env bash
set -Eeuo pipefail

export IDF_TOOLS_PATH=/home/oleg/.espressif/esp-idf-6.0.2
source /home/oleg/esp/esp-idf/export.sh >/tmp/esp32-wifi-flash.log
cd "$(dirname "$0")"

[[ -f build-6.0.2/esp32_wifi.bin ]] || {
    echo "Нет готовой прошивки. Сначала выполните ./build.sh"
    exit 1
}
read -r -p "Записать Wi-Fi тест в /dev/ttyUSB0? Введите FLASH: " answer
[[ "$answer" == FLASH ]] || { echo "Отменено"; exit 1; }
idf.py -B build-6.0.2 -p /dev/ttyUSB0 -b 115200 flash
echo "Готово: Wi-Fi тест записан"
