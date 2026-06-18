# Bare Metal Template for STM32F4xx Microcontroller Family 📟

Clone this repository and follow the instructions in the README.md file to set up your development environment.

## Flashing the MCU

```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "program build/bare-metal-stm32f4.elf verify reset exit"
```

## Debugging with OpenOCD and GDB

Run this command in one terminal:

```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg
```

Then run this command in another terminal:

```bash
gdb-multiarch build/bare-metal-stm32f4.elf -ex "target extended-remote localhost:3333" -ex "monitor reset halt" -ex "load" -ex "break main" -ex "continue"
```
