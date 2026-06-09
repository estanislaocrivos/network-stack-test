#include "main.h"

#include "clock.h"

#include <stdbool.h>

int main(void)
{
    clock_init(CLOCK_84MHZ);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(3 << (5 * 2));
    GPIOA->MODER |= (1 << (5 * 2));

    while (1)
    {
        GPIOA->ODR ^= (1 << 5);
        for (volatile int i = 0; i < 1e6; i++)
        {
        }
    }
}
