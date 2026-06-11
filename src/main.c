#include "main.h"

#include "../external/c-libraries/drivers/stm32f4xx/inc/gpio.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/mcu.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/usart.h"
#include "clock.h"

#include <stdbool.h>
#include <stdint.h>

int main(void)
{
    clock_init(CLOCK_84MHZ);

    MCU_DEFINE(mcu, &stm32f4xx_mcu_ops);
    mcu.ops->initialize(&mcu);

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
