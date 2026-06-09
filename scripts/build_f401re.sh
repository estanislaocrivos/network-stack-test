#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

cd "$ROOT_DIR"

if ! command -v arm-none-eabi-gcc &>/dev/null; then
    echo "Error: arm-none-eabi-gcc not found in PATH" >&2
    exit 1
fi

rm -rf build/

cmake -B build/ \
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
    -DMCU=STM32F401xE \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build/ -- -j"$(nproc)"
