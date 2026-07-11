#ifndef SNTP_H
#define SNTP_H

/* ========================================================================== */

#include <stdint.h>

/* ========================================================================== */

#define NTP_SERVER_IPV4_ADDRESS "162.159.200.1"
#define NTP_SERVER_PORT         123

/* ========================================================================== */

struct sntp_packet_header
{
    uint8_t li;
    uint8_t vn;
    uint8_t mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
};

struct timespec
{
    uint32_t tv_sec;
    uint32_t tv_nsec;
};

struct sntp_tx_metadata
{
    uint16_t                  port;
    struct timespec           ts;
    struct sntp_packet_header header;
    uint32_t                  root_delay;
    uint32_t                  root_dispersion;
    uint32_t                  ref_identifier;
    uint32_t                  ref_timestamp_int;
    uint32_t                  ref_timestamp_frac;
    uint32_t                  orig_timestamp_int;
    uint32_t                  orig_timestamp_frac;
    uint32_t                  rx_timestamp_int;
    uint32_t                  rx_timestamp_frac;
    uint32_t                  tx_timestamp_int;
    uint32_t                  tx_timestamp_frac;
};

/* ========================================================================== */

int8_t sntp_build_frame(
    struct sntp_tx_metadata* mdata, uint8_t* tx_frame, uint16_t* tx_frame_size);

/* ========================================================================== */

#endif /* SNTP_H */
