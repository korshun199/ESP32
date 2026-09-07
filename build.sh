#!/usr/bin/env bash
set -Eeuo pipefail

exec "$(dirname "$0")/examples/blink/build.sh"
