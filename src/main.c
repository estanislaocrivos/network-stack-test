#include "main.h"

/* ========================================================================== */

#include "clock.h"
#include "enc28j60.h"
#include "errno.h"
#include "platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ========================================================================== */

#define MAC_ADDR_BYTE_1         0x02
#define MAC_ADDR_BYTE_2         0x00
#define MAC_ADDR_BYTE_3         0x00
#define MAC_ADDR_BYTE_4         0x00
#define MAC_ADDR_BYTE_5         0x00
#define MAC_ADDR_BYTE_6         0x01

#define ENC28J60_RXBUF_END_ADDR 0x19BF

#define IPV4_ADDR_BYTE_4        192
#define IPV4_ADDR_BYTE_3        168
#define IPV4_ADDR_BYTE_2        1
#define IPV4_ADDR_BYTE_1        250

bool arp_is_request_for_me(uint8_t* frame, uint16_t size)
{
    if (frame == NULL)
    {
        return -EFAULT;
    }
    if (size < 42)
    {
        return false;
    }

    uint16_t eth_type = (uint16_t)frame[12] << 8 | frame[13];
    if (eth_type != 0x0806) /* Check if ARP */
    {
        return false;
    }

    uint16_t op_type = (uint16_t)frame[20] << 8 | frame[21];
    if (op_type != 0x0001) /* Check if request */
    {
        return false;
    }

    return (
        frame[38] == IPV4_ADDR_BYTE_4 && frame[39] == IPV4_ADDR_BYTE_3
        && frame[40] == IPV4_ADDR_BYTE_2 && frame[41] == IPV4_ADDR_BYTE_1);
}

int8_t arp_reply_request(uint8_t* frame, uint16_t size)
{
    if (frame == NULL)
    {
        return -EFAULT;
    }

    /* DST field */
    frame[0] = frame[22];
    frame[1] = frame[23];
    frame[2] = frame[24];
    frame[3] = frame[25];
    frame[4] = frame[26];
    frame[5] = frame[27];

    /* SRC field */
    frame[6]  = MAC_ADDR_BYTE_1;
    frame[7]  = MAC_ADDR_BYTE_2;
    frame[8]  = MAC_ADDR_BYTE_3;
    frame[9]  = MAC_ADDR_BYTE_4;
    frame[10] = MAC_ADDR_BYTE_5;
    frame[11] = MAC_ADDR_BYTE_6;

    frame[20] = 0;
    frame[21] = 2;

    frame[32] = frame[0];
    frame[33] = frame[1];
    frame[34] = frame[2];
    frame[35] = frame[3];
    frame[36] = frame[4];
    frame[37] = frame[5];

    frame[22] = frame[6];
    frame[23] = frame[7];
    frame[24] = frame[8];
    frame[25] = frame[9];
    frame[26] = frame[10];
    frame[27] = frame[11];

    frame[38] = frame[28];
    frame[39] = frame[29];
    frame[40] = frame[30];
    frame[41] = frame[31];

    frame[28] = IPV4_ADDR_BYTE_4;
    frame[29] = IPV4_ADDR_BYTE_3;
    frame[30] = IPV4_ADDR_BYTE_2;
    frame[31] = IPV4_ADDR_BYTE_1;

    return 0;
}

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

    struct gpio enc28j60_rst = {
        .ops             = PLATFORM_GPIO_OPS,
        .id              = GPIO_A9_ID,
        .direction       = GPIO_OUTPUT,
        .type            = GPIO_DIGITAL,
        .was_initialized = true,
    };
    enc28j60_rst.ops->set_state(&enc28j60_rst, true);

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

    /* Reset ENC28J60 */
    enc28j60_rst.ops->set_state(&enc28j60_rst, false);
    for (volatile uint32_t i = 0; i < 84000; i++)
        __NOP();
    enc28j60_rst.ops->set_state(&enc28j60_rst, true);
    for (volatile uint32_t i = 0; i < 84000; i++)
        __NOP();

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
           .rx_buf_end_addr = ENC28J60_RXBUF_END_ADDR, /* Buffer has 8KB. Only
                                         1600B are left for TX buffer */
           .erdpt           = 0,
           .was_initialized = false};
    enc28j60_init(&enc28j60);

    /* ====================================================================== */

    uint8_t frame[1518]; /* Where eth frame will be stored */

    while (1)
    {
        uint8_t epktcnt = 0;
        enc28j60_get_epktcnt(&enc28j60, &epktcnt);
        if (epktcnt == 0)
        {
            continue;
        }

        uint16_t eth_pkt_size = 0;
        int8_t   ret = enc28j60_receive_packet(&enc28j60, frame, &eth_pkt_size);
        if (ret != 0)
        {
            continue;
        }

        uint8_t arp_req = (uint8_t)arp_is_request_for_me(frame, sizeof(frame));

        char   buf[128];
        size_t len = snprintf(
            buf,
            sizeof(buf),
            "DA: %02X:%02X:%02X:%02X:%02X:%02X "
            "SA: %02X:%02X:%02X:%02X:%02X:%02X "
            "Type: %02X%02X (request: %u)\r\n",
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
            frame[13],
            arp_req);
        serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);

        if (arp_req)
        {
            arp_reply_request(frame, sizeof(frame));
            enc28j60_transmit_packet(&enc28j60, frame, eth_pkt_size);
        }
    }

    /* ====================================================================== */
}

/* ========================================================================== */
