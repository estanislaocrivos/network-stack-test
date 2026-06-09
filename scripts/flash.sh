#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

cd "$ROOT_DIR"

if ! command -v st-flash &>/dev/null; then
    echo "Error: st-flash not found in PATH" >&2
    exit 1
fi

if [ ! -f build/bare-metal-stm32f4.bin ]; then
    echo "Error: build/bare-metal-stm32f4.bin not found — run build_f401re.sh first" >&2
    exit 1
fi

cmake --build build --target flash
