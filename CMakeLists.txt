cmake_minimum_required(VERSION 3.20)

# Project name and languages
project(bare-metal-stm32f4 LANGUAGES C ASM)

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# =============================================================================
# MCU Configuration
# =============================================================================
# Usage: cmake -DMCU=STM32F407xx ..
#
# Supported MCUs (add more as needed):
# STM32F401xE
# =============================================================================

set(MCU "STM32F401xE" CACHE STRING "Target MCU (e.g., STM32F401xE, STM32F407xx)")

# Convert to lowercase for file matching
string(TOLOWER ${MCU} MCU_LOWER)

# Validate MCU and set properties
# Format: MCU -> startup file suffix, linker script name
if(MCU MATCHES "^STM32F401x[CE]$")
    set(STARTUP_SUFFIX "stm32f401xe")
    set(LINKER_SUFFIX "stm32f401xe")
else()
    message(FATAL_ERROR "Unsupported MCU: ${MCU}\n"
        "Add support in CMakeLists.txt and create linker script in linker/")
endif()

message(STATUS "Target MCU: ${MCU}")

# =============================================================================
# Paths
# =============================================================================
set(CMSIS_PATH      ${CMAKE_SOURCE_DIR}/external/CMSIS_5/CMSIS/Core/Include)
set(DEVICE_PATH     ${CMAKE_SOURCE_DIR}/external/cmsis_device_f4)
set(LINKER_PATH     ${CMAKE_SOURCE_DIR}/linker)

# =============================================================================
# Source files
# =============================================================================

# Application sources
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.c
)

if(NOT APP_SOURCES)
    message(FATAL_ERROR "No source files found in src/")
endif()

# Startup file (from CMSIS device)
set(STARTUP_FILE ${DEVICE_PATH}/Source/Templates/gcc/startup_${STARTUP_SUFFIX}.s)

if(NOT EXISTS ${STARTUP_FILE})
    message(FATAL_ERROR "Startup file not found: ${STARTUP_FILE}")
endif()

# System init (clock configuration)
set(SYSTEM_FILE ${DEVICE_PATH}/Source/Templates/system_stm32f4xx.c)

# Linker script
set(LINKER_SCRIPT ${LINKER_PATH}/${LINKER_SUFFIX}.ld)

if(NOT EXISTS ${LINKER_SCRIPT})
    message(FATAL_ERROR "Linker script not found: ${LINKER_SCRIPT}\n"
        "Create it in linker/ directory")
endif()

# =============================================================================
# Include directories
# =============================================================================
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMSIS_PATH}
    ${DEVICE_PATH}/Include
)

# =============================================================================
# Executable
# =============================================================================
add_executable(${PROJECT_NAME}.elf
    ${APP_SOURCES}
    ${STARTUP_FILE}
    ${SYSTEM_FILE}
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${INCLUDE_DIRS})

# Define MCU for ST headers (required to select correct register definitions)
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    ${MCU}
)

# =============================================================================
# Compiler flags
# =============================================================================
# -fdata-sections -ffunction-sections: puts each function/variable in its
# own section, allowing the linker to remove unused code

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -Wall
    -Wextra
    -fdata-sections
    -ffunction-sections
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-gdwarf-2>
    $<$<CONFIG:Release>:-Os>
)

# =============================================================================
# Linker flags
# =============================================================================
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections              # Remove unused sections
    -Wl,--print-memory-usage       # Show memory usage
    -Wl,-Map=${PROJECT_NAME}.map   # Generate memory map
    --specs=nano.specs             # Use newlib-nano (smaller)
    --specs=nosys.specs            # Bare metal (no syscalls)
    -lc                            # Link with libc
    -lm                            # Link with libm (math)
    -lnosys                        # Stubs for syscalls
)

# Post-build: generate .bin and .hex, show size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    # Generate .hex file (for some programmers)
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex

    # Generate .bin file (for ST-Link, OpenOCD, etc)
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin

    # Show section sizes
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf

    COMMENT "Generating ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
)

# Flash target (requires st-flash installed)
add_custom_target(flash
    COMMAND st-flash --connect-under-reset write ${PROJECT_NAME}.bin 0x08000000
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing ${PROJECT_NAME}.bin to STM32..."
)

# Debugging targets
# Start OpenOCD (run in a separate terminal)
add_custom_target(openocd
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
    COMMENT "Starting OpenOCD... (Ctrl+C to stop)"
)

# Start GDB and connect to OpenOCD (requires OpenOCD running)
find_program(CMAKE_GDB NAMES gdb-multiarch arm-none-eabi-gdb REQUIRED)

add_custom_target(debug
    COMMAND ${CMAKE_GDB}
        ${PROJECT_NAME}.elf
        -ex "target extended-remote localhost:3333"
        -ex "monitor reset halt"
        -ex "load"
        -ex "break main"
        -ex "continue"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Starting GDB..."
)

# Flash using OpenOCD (alternative to st-flash)
add_custom_target(flash-openocd
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
        -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing with OpenOCD..."
)
