#include "main.h"

/* ========================================================================== */

#include "clock.h"
#include "enc28j60.h"
#include "platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ========================================================================== */

int main(void)
{
    clock_init(CLOCK_84MHZ);

    struct mcu mcu = {.ops = PLATFORM_MCU_OPS, .was_initialized = false};
    mcu.ops->initialize(&mcu);

    struct gpio enc28j60_cs = {
        .ops             = PLATFORM_GPIO_OPS,
        .id              = GPIO_B6_ID,
        .direction       = GPIO_OUTPUT,
        .type            = GPIO_DIGITAL,
        .was_initialized = true,
    };
    enc28j60_cs.ops->set_state(&enc28j60_cs, true);

    struct serial serial_log
        = {.ops             = PLATFORM_USART2_OPS,
           .baud_rate       = 9600UL,
           .data_bits       = 8,
           .parity          = 'N',
           .stop_bits       = 1,
           .was_initialized = false};
    serial_log.ops->initialize(&serial_log);

    struct spi spi
        = {.ops             = PLATFORM_SPI1_OPS,
           .mode            = 0,
           .master          = true,
           .frequency       = 1000000UL,
           .msb_first       = true,
           .cs_active_high  = false,
           .hardware_cs     = false,
           .was_initialized = false};
    spi.ops->initialize(&spi);

    struct enc28j60 enc28j60
        = {.spi_bus = &spi, .spi_cs = &enc28j60_cs, .was_initialized = false};
    enc28j60_init(&enc28j60);

    uint8_t r_value = 0;
    enc28j60_read_register(&enc28j60, EREVID, &r_value);

    uint8_t w_value = 0;
    enc28j60_write_register(&enc28j60, ERDPTL, 0xAA);
    enc28j60_read_register(&enc28j60, ERDPTL, &w_value);

    char   buf[32];
    size_t len = snprintf(buf, sizeof(buf), "Read value: 0x%02X\r\n", r_value);
    serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);
    len = snprintf(buf, sizeof(buf), "Write value: 0x%02X\r\n", w_value);
    serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);

    /* ====================================================================== */

    while (1)
    {
        for (volatile int i = 0; i < 1000000; i++)
        {
        }
    }

    /* ====================================================================== */
}

/* ========================================================================== */
