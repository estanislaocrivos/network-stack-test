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

#define MAC_ADDR_BYTE_1 0x02
#define MAC_ADDR_BYTE_2 0x00
#define MAC_ADDR_BYTE_3 0x00
#define MAC_ADDR_BYTE_4 0x00
#define MAC_ADDR_BYTE_5 0x00
#define MAC_ADDR_BYTE_6 0x01

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
        = {.spi_bus = &spi,
           .spi_cs  = &enc28j60_cs,
           .mac_address
           = {MAC_ADDR_BYTE_1,
              MAC_ADDR_BYTE_2,
              MAC_ADDR_BYTE_3,
              MAC_ADDR_BYTE_4,
              MAC_ADDR_BYTE_5,
              MAC_ADDR_BYTE_6},
           .was_initialized = false};
    enc28j60_init(&enc28j60);

#if 0
    uint8_t w_value = 0;
    enc28j60_write_register(&enc28j60, ERDPTL, 0xAA);
    enc28j60_read_eth_register(&enc28j60, ERDPTL, &w_value);

    char   buf[32];
    size_t len = snprintf(buf, sizeof(buf), "Read value: 0x%02X\r\n", r_value);
    serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);
    len = snprintf(buf, sizeof(buf), "Write value: 0x%02X\r\n", w_value);
    serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);
#endif

    /* ====================================================================== */

    uint8_t frame[1518];

    while (1)
    {
        uint8_t epktcnt = 0;
        enc28j60_get_epktcnt(&enc28j60, &epktcnt);
        if (epktcnt == 0)
        {
            continue;
        }

        int8_t ret = enc28j60_receive_packet(&enc28j60, frame, sizeof(frame));
        if (ret != 0)
        {
            continue;
        }

        char   buf[64];
        size_t len = snprintf(
            buf,
            sizeof(buf),
            "DA: %02X:%02X:%02X:%02X:%02X:%02X "
            "SA: %02X:%02X:%02X:%02X:%02X:%02X "
            "Type: %02X%02X\r\n",
            frame[0],
            frame[1],
            frame[2],
            frame[3],
            frame[4],
            frame[5],
            frame[6],
            frame[7],
            frame[8],
            frame[9],
            frame[10],
            frame[11],
            frame[12],
            frame[13]);
        serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);
    }

    /* ====================================================================== */
}

/* ========================================================================== */
