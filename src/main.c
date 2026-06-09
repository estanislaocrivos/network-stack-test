#include "main.h"

#include "../external/c-libraries/drivers/stm32f4xx/inc/gpio.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/usart.h"
#include "../external/c-libraries/libraries/embedded-hal/inc/embedded_hal.h"
#include "clock.h"
#include "pinout.h"

#include <stdbool.h>
#include <stdint.h>

int main(void)
{
    clock_init(CLOCK_84MHZ);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER  = GPIOA_MODER_CONFIG;
    GPIOA->AFR[0] = GPIOA_AFRL_CONFIG;

    SERIAL_DEFINE(serial_log, &USART2_OPS, 9600, 8, 'N', 1);
    serial_log.ops->initialize(&serial_log);

    const uint8_t msg[] = "hello\r\n";

    while (1)
    {
        serial_log.ops->transmit(&serial_log, msg, sizeof(msg) - 1);

        GPIOA->ODR ^= (1U << 5);
        for (volatile int i = 0; i < 1000000; i++)
        {
        }
    }
}
