#include "main.h"

#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_gpio.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_mcu.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_usart.h"
#include "clock.h"
#include "platform.h"

#include <stdbool.h>
#include <stdint.h>

int main(void)
{
    clock_init(CLOCK_84MHZ);

    struct mcu mcu = {.ops = PLATFORM_MCU_OPS, .was_initialized = false};
    mcu.ops->initialize(&mcu);

    struct serial serial_log
        = {.ops             = PLATFORM_USART2_OPS,
           .baud_rate       = 9600,
           .data_bits       = 8,
           .parity          = 'N',
           .stop_bits       = 1,
           .was_initialized = false};
    serial_log.ops->initialize(&serial_log);

    const uint8_t msg[] = "Hello, world!\r\n";

    while (1)
    {
        serial_log.ops->transmit(&serial_log, msg, sizeof(msg) - 1);

        GPIOA->ODR ^= (1U << 5);
        for (volatile int i = 0; i < 1000000; i++)
        {
        }
    }
}
