#include "main.h"

/* ========================================================================== */

#include "arp.h"
#include "clock.h"
#include "enc28j60.h"
#include "eth.h"
#include "icmp.h"
#include "ip.h"
#include "platform.h"
#include "utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ========================================================================== */

#define MAC_ADDR_BYTE_1         0x02 /* first transmitted / MSB */
#define MAC_ADDR_BYTE_2         0x00
#define MAC_ADDR_BYTE_3         0x00
#define MAC_ADDR_BYTE_4         0x00
#define MAC_ADDR_BYTE_5         0x00
#define MAC_ADDR_BYTE_6         0x01

#define IPV4_ADDR_BYTE_1        192 /* MSB */
#define IPV4_ADDR_BYTE_2        168
#define IPV4_ADDR_BYTE_3        1
#define IPV4_ADDR_BYTE_4        250

#define ENC28J60_RXBUF_END_ADDR 0x19BF

#define MAX_ICMP_PACKET_SIZE    1472

#define MAX_IP_PACKET_SIZE \
    MAX_ETH_FRAME_SIZE - MAX_ETH_HEADER_SIZE - MAX_IP_HEADER_SIZE

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
           .rx_buf_end_addr = ENC28J60_RXBUF_END_ADDR,
           .erdpt           = 0,
           .was_initialized = false};
    enc28j60_init(&enc28j60);

    const struct eth eth
        = {.ip_addr
           = {IPV4_ADDR_BYTE_1,
              IPV4_ADDR_BYTE_2,
              IPV4_ADDR_BYTE_3,
              IPV4_ADDR_BYTE_4},
           .mac_addr
           = {MAC_ADDR_BYTE_1,
              MAC_ADDR_BYTE_2,
              MAC_ADDR_BYTE_3,
              MAC_ADDR_BYTE_4,
              MAC_ADDR_BYTE_5,
              MAC_ADDR_BYTE_6},
           .calc_crc = false};

    const struct arp arp
        = {.ip_addr
           = {IPV4_ADDR_BYTE_1,
              IPV4_ADDR_BYTE_2,
              IPV4_ADDR_BYTE_3,
              IPV4_ADDR_BYTE_4},
           .mac_addr
           = {MAC_ADDR_BYTE_1,
              MAC_ADDR_BYTE_2,
              MAC_ADDR_BYTE_3,
              MAC_ADDR_BYTE_4,
              MAC_ADDR_BYTE_5,
              MAC_ADDR_BYTE_6}};

    const struct ip ip
        = {.ip_addr
           = {IPV4_ADDR_BYTE_1,
              IPV4_ADDR_BYTE_2,
              IPV4_ADDR_BYTE_3,
              IPV4_ADDR_BYTE_4}};

    const struct icmp icmp = {.unused = 0};

    /* ====================================================================== */

    uint8_t  eth_frame[MAX_ETH_FRAME_SIZE];
    uint16_t eth_frame_size = 0;

    while (1)
    {
        uint8_t epktcnt = 0;
        enc28j60_get_epktcnt(&enc28j60, &epktcnt);
        if (epktcnt == 0)
        {
            continue;
        }

        uint16_t eth_pkt_size = 0;
        if (enc28j60_receive_packet(&enc28j60, eth_frame, &eth_pkt_size))
        {
            continue;
        }

        struct eth_rx_metadata eth_rx_mdata = {0};
        if (eth_process_frame(&eth, eth_frame, eth_pkt_size, &eth_rx_mdata))
        {
            continue;
        }

#if 0
        char   buf[128];
        size_t len = snprintf(
            buf,
            sizeof(buf),
            "DA: %02X:%02X:%02X:%02X:%02X:%02X "
            "SA: %02X:%02X:%02X:%02X:%02X:%02X "
            "Type: %02X%02X\r\n",
            eth_frame[0],
            eth_frame[1],
            eth_frame[2],
            eth_frame[3],
            eth_frame[4],
            eth_frame[5],
            eth_frame[6],
            eth_frame[7],
            eth_frame[8],
            eth_frame[9],
            eth_frame[10],
            eth_frame[11],
            eth_frame[12],
            eth_frame[13]);
        serial_log.ops->transmit(&serial_log, (uint8_t*)buf, len);
#endif

        /* ================================================================== */

        if (eth_rx_mdata.payload_type == ETH_PLD_IPV4)
        {
            struct ip_rx_metadata ip_rx_mdata = {0};
            if (ip_process_frame(
                    &ip,
                    eth_rx_mdata.payload,
                    eth_rx_mdata.payload_size,
                    &ip_rx_mdata))
            {
                continue;
            }

            if (ip_is_pkt_for_me(&ip, &ip_rx_mdata))
            {
                struct icmp_rx_metadata icmp_rx_mdata = {0};
                icmp_process_frame(
                    &icmp,
                    ip_rx_mdata.payload,
                    ip_rx_mdata.payload_size,
                    &icmp_rx_mdata);

                if (icmp_rx_mdata.type == ICMP_ECHO_REQUEST)
                {
                    uint8_t  icmp_packet[MAX_ICMP_PACKET_SIZE] = {0};
                    uint16_t icmp_packet_size                  = 0;

                    struct icmp_tx_metadata icmp_tx_mdata
                        = {.type         = ICMP_ECHO_REPLY,
                           .code         = icmp_rx_mdata.code,
                           .id           = icmp_rx_mdata.id,
                           .payload      = icmp_rx_mdata.payload,
                           .payload_size = icmp_rx_mdata.payload_size,
                           .seq_num      = icmp_rx_mdata.seq_num};

                    icmp_build_frame(
                        &icmp, &icmp_tx_mdata, icmp_packet, &icmp_packet_size);

                    uint8_t  ip_packet[MAX_IP_PACKET_SIZE] = {0};
                    uint16_t ip_packet_size                = 0;

                    struct ip_tx_metadata ip_tx_mdata
                        = {.payload       = icmp_packet,
                           .payload_size  = icmp_packet_size,
                           .pld_prot_type = IP_PLD_ICMP,
                           .version       = IP_VER_4};
                    memcpy(ip_tx_mdata.dest_ip, ip_rx_mdata.src_ip, 4);

                    ip_build_frame(
                        &ip, &ip_tx_mdata, ip_packet, &ip_packet_size);

                    struct eth_tx_metadata eth_tx_mdata
                        = {.payload_type = ETH_PLD_IPV4,
                           .payload      = ip_packet,
                           .payload_size = ip_packet_size};
                    memcpy(
                        eth_tx_mdata.dest_mac_addr,
                        eth_rx_mdata.src_mac_addr,
                        6);

                    eth_build_frame(
                        &eth, &eth_tx_mdata, eth_frame, &eth_frame_size);

                    enc28j60_transmit_packet(
                        &enc28j60, eth_frame, eth_frame_size);
                }
            }
        }

        /* ================================================================== */

        if (eth_rx_mdata.payload_type == ETH_PLD_ARP)
        {
            struct arp_rx_metadata arp_rx_mdata = {0};
            if (arp_process_frame(
                    &arp,
                    eth_rx_mdata.payload,
                    (uint8_t)eth_rx_mdata.payload_size,
                    &arp_rx_mdata))
            {
                continue;
            }

            if (arp_is_request_for_me(&arp, &arp_rx_mdata))
            {
                uint8_t                arp_packet[ARP_PACKET_SIZE];
                struct arp_tx_metadata arp_tx_mdata = {.op_type = ARP_REPLY};
                memcpy(
                    arp_tx_mdata.dest_mac_addr, arp_rx_mdata.src_mac_addr, 6);
                memcpy(arp_tx_mdata.dest_ip_addr, arp_rx_mdata.src_ip_addr, 4);

                uint8_t arp_packet_size = 0;
                arp_build_frame(
                    &arp, &arp_tx_mdata, arp_packet, &arp_packet_size);

                struct eth_tx_metadata eth_tx_mdata
                    = {.payload_type = ETH_PLD_ARP,
                       .payload      = arp_packet,
                       .payload_size = arp_packet_size};
                memcpy(
                    eth_tx_mdata.dest_mac_addr, arp_rx_mdata.src_mac_addr, 6);

                eth_build_frame(
                    &eth, &eth_tx_mdata, eth_frame, &eth_frame_size);

                enc28j60_transmit_packet(&enc28j60, eth_frame, eth_frame_size);
            }
        }
    }

    /* ====================================================================== */
}

/* ========================================================================== */
