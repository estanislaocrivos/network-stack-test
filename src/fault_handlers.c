#include "fault_handlers.h"

void HardFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while (1)
        ;
}

void MemManage_Handler(void)
{
    __asm volatile("bkpt #0");
    while (1)
        ;
}

void BusFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while (1)
        ;
}

void UsageFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while (1)
        ;
}
