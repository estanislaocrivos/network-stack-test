# Toolchain file for cross-compiling to ARM Cortex-M
# CMake uses this to know which compiler to use

set(CMAKE_SYSTEM_NAME Generic)        # "Generic" = bare metal (no host OS)
set(CMAKE_SYSTEM_PROCESSOR arm)       # Target architecture

# Verify ARM toolchain is available in PATH before CMake tries to use it
find_program(ARM_GCC arm-none-eabi-gcc)
if(NOT ARM_GCC)
    message(FATAL_ERROR
        "arm-none-eabi-gcc not found in PATH.\n"
        "Install the ARM GNU Toolchain and add its 'bin/' directory to your PATH:\n"
        "  https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads")
endif()

# Specify compilers
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

# Additional tools
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP "arm-none-eabi-objdump")
set(CMAKE_SIZE "arm-none-eabi-size")

# Common flags for Cortex-M4 with FPU
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# -mcpu=cortex-m4   -> generate code for Cortex-M4
# -mthumb           -> use Thumb instructions (16/32 bit, more compact)
# -mfpu=fpv4-sp-d16 -> use single-precision FPU
# -mfloat-abi=hard  -> pass floats in FPU registers (faster)

set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS}")

# Prevent CMake from trying to link a test executable
# (would fail because we don't have libc or startup code configured yet)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
