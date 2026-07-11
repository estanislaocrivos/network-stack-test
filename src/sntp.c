#include "sntp.h"

/* ========================================================================== */

#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */

#define SNTP_HDR_LI_BIT_OFST             6
#define SNTP_HDR_VN_BIT_OFST             3
#define SNTP_HDR_MODE_BIT_OFST           0

#define SNTP_HDR_BYTE0_FRAME_OFST        0  /* LI | VN | Mode       */
#define SNTP_HDR_BYTE1_FRAME_OFST        1  /* Stratum              */
#define SNTP_HDR_BYTE2_FRAME_OFST        2  /* Poll                 */
#define SNTP_HDR_BYTE3_FRAME_OFST        3  /* Precision            */
#define SNTP_ROOT_DELAY_FRAME_OFST       4  /* 4 bytes              */
#define SNTP_ROOT_DISPERSION_FRAME_OFST  8  /* 4 bytes              */
#define SNTP_REF_IDENTIFIER_FRAME_OFST   12 /* 4 bytes              */
#define SNTP_REF_TSTAMP_INT_FRAME_OFST   16 /* 4 bytes              */
#define SNTP_REF_TSTAMP_FRAC_FRAME_OFST  20 /* 4 bytes              */
#define SNTP_ORIG_TSTAMP_INT_FRAME_OFST  24 /* 4 bytes              */
#define SNTP_ORIG_TSTAMP_FRAC_FRAME_OFST 28 /* 4 bytes              */
#define SNTP_RX_TSTAMP_INT_FRAME_OFST    32 /* 4 bytes              */
#define SNTP_RX_TSTAMP_FRAC_FRAME_OFST   36 /* 4 bytes              */
#define SNTP_TX_TSTAMP_INT_FRAME_OFST    40 /* 4 bytes              */
#define SNTP_TX_TSTAMP_FRAC_FRAME_OFST   44 /* 4 bytes              */
#define SNTP_FRAME_SIZE                  48

/* ========================================================================== */

static uint64_t sntp_convert_timestamp_to_u64(
    uint32_t timestamp_int, uint32_t timestamp_frac)
{
    return ((uint64_t)(timestamp_int) << 32) | (timestamp_frac);
}

static void sntp_convert_timespec_to_timestamp(
    struct timespec* ts,
    uint32_t*        tstamp_int,
    uint32_t*        tstamp_frac,
    uint64_t*        tstamp)
{
    /* Convert Unix epoch (1970) to NTP epoch (1900) */
    *tstamp_int  = ts->tv_sec + 2208988800ULL;
    *tstamp_frac = (uint32_t)(((uint64_t)ts->tv_nsec << 32) / 1000000000UL);

    /* Full timestamp is 64 bits where MS 32 bits correspond to integer and LS
     * 32 bits correspond to fractional */
    *tstamp = sntp_convert_timestamp_to_u64(*tstamp_int, *tstamp_frac);
}

/* ========================================================================== */

static void write_u32_to_frame(uint32_t value, uint8_t* buffer)
{
    buffer[0] = (uint8_t)(value >> 24);
    buffer[1] = (uint8_t)(value >> 16);
    buffer[2] = (uint8_t)(value >> 8);
    buffer[3] = (uint8_t)(value);
}

static uint32_t read_u32_from_frame(const uint8_t* buffer)
{
    return ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16)
           | ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];
}

/* ========================================================================== */

static void sntp_build_packet_header(
    const struct sntp_packet_header fields, uint8_t* frame)
{
    frame[SNTP_HDR_BYTE0_FRAME_OFST]
        = (uint8_t)((fields.li << SNTP_HDR_LI_BIT_OFST)
                    | (fields.vn << SNTP_HDR_VN_BIT_OFST)
                    | (fields.mode << SNTP_HDR_MODE_BIT_OFST));
    frame[SNTP_HDR_BYTE1_FRAME_OFST] = fields.stratum;
    frame[SNTP_HDR_BYTE2_FRAME_OFST] = fields.poll;
    frame[SNTP_HDR_BYTE3_FRAME_OFST] = fields.precision;
}

/* ========================================================================== */

int8_t sntp_process_frame(
    uint8_t* rx_frame, uint16_t rx_frame_size, struct sntp_rx_metadata* mdata)
{
    if (rx_frame == NULL || mdata == NULL)
    {
        return -1;
    }
    if (rx_frame_size < SNTP_FRAME_SIZE)
    {
        return -1;
    }

    mdata->rx_timestamp_int
        = read_u32_from_frame(rx_frame + SNTP_RX_TSTAMP_INT_FRAME_OFST);
    mdata->rx_timestamp_frac
        = read_u32_from_frame(rx_frame + SNTP_RX_TSTAMP_FRAC_FRAME_OFST);
    mdata->tx_timestamp_int
        = read_u32_from_frame(rx_frame + SNTP_TX_TSTAMP_INT_FRAME_OFST);
    mdata->tx_timestamp_frac
        = read_u32_from_frame(rx_frame + SNTP_TX_TSTAMP_FRAC_FRAME_OFST);

    return 0;
}

/* ========================================================================== */

int8_t sntp_build_frame(
    const struct sntp_tx_metadata* mdata,
    uint8_t*                       tx_frame,
    uint16_t*                      tx_frame_size)
{
    if (mdata == NULL || tx_frame == NULL || tx_frame_size == NULL)
    {
        return -1;
    }

    sntp_build_packet_header(mdata->header, tx_frame);

    write_u32_to_frame(
        mdata->root_delay, tx_frame + SNTP_ROOT_DELAY_FRAME_OFST);
    write_u32_to_frame(
        mdata->root_dispersion, tx_frame + SNTP_ROOT_DISPERSION_FRAME_OFST);
    write_u32_to_frame(
        mdata->ref_identifier, tx_frame + SNTP_REF_IDENTIFIER_FRAME_OFST);
    write_u32_to_frame(
        mdata->ref_timestamp_int, tx_frame + SNTP_REF_TSTAMP_INT_FRAME_OFST);
    write_u32_to_frame(
        mdata->ref_timestamp_frac, tx_frame + SNTP_REF_TSTAMP_FRAC_FRAME_OFST);
    write_u32_to_frame(
        mdata->orig_timestamp_int, tx_frame + SNTP_ORIG_TSTAMP_INT_FRAME_OFST);
    write_u32_to_frame(
        mdata->orig_timestamp_frac,
        tx_frame + SNTP_ORIG_TSTAMP_FRAC_FRAME_OFST);
    write_u32_to_frame(
        mdata->rx_timestamp_int, tx_frame + SNTP_RX_TSTAMP_INT_FRAME_OFST);
    write_u32_to_frame(
        mdata->rx_timestamp_frac, tx_frame + SNTP_RX_TSTAMP_FRAC_FRAME_OFST);
    write_u32_to_frame(
        mdata->tx_timestamp_int, tx_frame + SNTP_TX_TSTAMP_INT_FRAME_OFST);
    write_u32_to_frame(
        mdata->tx_timestamp_frac, tx_frame + SNTP_TX_TSTAMP_FRAC_FRAME_OFST);

    *tx_frame_size = SNTP_FRAME_SIZE;
    return 0;
}

/* ========================================================================== */
