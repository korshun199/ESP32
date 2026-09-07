#!/usr/bin/env bash
set -Eeuo pipefail

export IDF_TOOLS_PATH=/home/oleg/.espressif/esp-idf-6.0.2
export PATH=/home/oleg/.espressif/esp-idf-6.0.2/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf/bin:$PATH
source /home/work/ESP-IDF/export.sh >/tmp/esp32-hello-build.log
cd "$(dirname "$0")"

idf.py -B build-6.0.2 build
echo "Готово: examples/hello/build-6.0.2/esp32_hello.bin"
