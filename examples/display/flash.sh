#!/usr/bin/env bash
set -Eeuo pipefail

export IDF_TOOLS_PATH=/home/oleg/.espressif/esp-idf-6.0.2
export PATH=/home/oleg/.espressif/esp-idf-6.0.2/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf/bin:$PATH
source /home/work/ESP-IDF/export.sh >/tmp/esp32-display-flash.log
cd "$(dirname "$0")"

[[ -f build-6.0.2/esp32_display.bin ]] || {
    echo "Нет готовой прошивки. Сначала выполните ./build.sh в examples/display"
    exit 1
}
read -r -p "Записать OLED-тест в /dev/ttyUSB0? Введите FLASH: " answer
[[ "$answer" == FLASH ]] || { echo "Отменено"; exit 1; }
idf.py -B build-6.0.2 -p /dev/ttyUSB0 -b 115200 flash
echo "Готово: OLED-тест записан"
