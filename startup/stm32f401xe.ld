/*
 * Linker script for STM32F401xE
 *
 * STM32F401RE Memory:
 * - Flash: 512KB @ 0x08000000
 * - SRAM:   96KB @ 0x20000000
 */

/* Entry point - symbol where execution begins */
ENTRY(Reset_Handler)

/* Initial stack - at the end of RAM (grows downward) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Minimum heap and stack sizes */
_Min_Heap_Size = 0x200;   /* 512 bytes - FreeRTOS uses its own heap */
_Min_Stack_Size = 0x400;  /* 1KB for initial stack (before scheduler) */

/* Memory region definitions */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

/* Sections - how to organize code/data in memory */
SECTIONS
{
    /* Vector table - MUST be at the beginning of Flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))  /* KEEP prevents linker from removing it */
        . = ALIGN(4);
    } >FLASH

    /* Program code */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* Normal code */
        *(.text*)          /* Code with suffixes (.text.startup, etc) */
        *(.glue_7)         /* ARM/Thumb glue */
        *(.glue_7t)
        *(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;        /* Symbol: end of code */
    } >FLASH

    /* Read-only data (const, strings) */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* C++ constructors */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } >FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } >FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } >FLASH

    /*
     * Initialized data (.data)
     * Values stored in Flash, copied to RAM at startup
     */
    _sidata = LOADADDR(.data);  /* Address in Flash where values are stored */

    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* Start of .data in RAM */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;        /* End of .data in RAM */
    } >RAM AT> FLASH       /* Goes in RAM, but loaded from Flash */

    /*
     * Uninitialized data (.bss)
     * Zeroed at startup
     */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* Start of .bss */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;         /* End of .bss */
        __bss_end__ = _ebss;
    } >RAM

    /* Space for heap and stack */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    /* Discard unnecessary sections */
    /DISCARD/ :
    {
        *(.ARM.attributes)
        *(.comment)
        *(.note*)
    }
}
