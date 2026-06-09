#ifndef PINOUT_H
#define PINOUT_H

/* ========================================================================== */

#include "../external/c-libraries/drivers/stm32f4xx/inc/gpio.h"

/* ========================================================================== */

/* PORT A */
#define GPIO_A0  PIN_NC
#define GPIO_A1  PIN_NC
#define GPIO_A2  PIN_AF        /* USART2 TX */
#define GPIO_A3  PIN_AF        /* USART2 RX */
#define GPIO_A4  PIN_NC
#define GPIO_A5  PIN_DO        /* LED LD2 */
#define GPIO_A6  PIN_NC
#define GPIO_A7  PIN_NC
#define GPIO_A8  PIN_NC
#define GPIO_A9  PIN_NC
#define GPIO_A10 PIN_NC
#define GPIO_A11 PIN_NC
#define GPIO_A12 PIN_NC
#define GPIO_A13 PIN_NC
#define GPIO_A14 PIN_NC
#define GPIO_A15 PIN_NC

/* PORT B */
#define GPIO_B0  PIN_NC
#define GPIO_B1  PIN_NC
#define GPIO_B2  PIN_NC
#define GPIO_B3  PIN_NC
#define GPIO_B4  PIN_NC
#define GPIO_B5  PIN_NC
#define GPIO_B6  PIN_NC
#define GPIO_B7  PIN_NC
#define GPIO_B8  PIN_NC
#define GPIO_B9  PIN_NC
#define GPIO_B10 PIN_NC
#define GPIO_B11 PIN_NC
#define GPIO_B12 PIN_NC
#define GPIO_B13 PIN_NC
#define GPIO_B14 PIN_NC
#define GPIO_B15 PIN_NC

/* PORT C */
#define GPIO_C0  PIN_NC
#define GPIO_C1  PIN_NC
#define GPIO_C2  PIN_NC
#define GPIO_C3  PIN_NC
#define GPIO_C4  PIN_NC
#define GPIO_C5  PIN_NC
#define GPIO_C6  PIN_NC
#define GPIO_C7  PIN_NC
#define GPIO_C8  PIN_NC
#define GPIO_C9  PIN_NC
#define GPIO_C10 PIN_NC
#define GPIO_C11 PIN_NC
#define GPIO_C12 PIN_NC
#define GPIO_C13 PIN_NC
#define GPIO_C14 PIN_NC
#define GPIO_C15 PIN_NC

/* PORT D */
#define GPIO_D2  PIN_NC

/* PORT H */
#define GPIO_H0  PIN_NC
#define GPIO_H1  PIN_NC

/* ========================================================================== */

/* Alternate function values (0 for non-AF pins) */

#define GPIO_A0_AF  0
#define GPIO_A1_AF  0
#define GPIO_A2_AF  USART2_TX_AF
#define GPIO_A3_AF  USART2_RX_AF
#define GPIO_A4_AF  0
#define GPIO_A5_AF  0
#define GPIO_A6_AF  0
#define GPIO_A7_AF  0
#define GPIO_A8_AF  0
#define GPIO_A9_AF  0
#define GPIO_A10_AF 0
#define GPIO_A11_AF 0
#define GPIO_A12_AF 0
#define GPIO_A13_AF 0
#define GPIO_A14_AF 0
#define GPIO_A15_AF 0

#define GPIO_B0_AF  0
#define GPIO_B1_AF  0
#define GPIO_B2_AF  0
#define GPIO_B3_AF  0
#define GPIO_B4_AF  0
#define GPIO_B5_AF  0
#define GPIO_B6_AF  0
#define GPIO_B7_AF  0
#define GPIO_B8_AF  0
#define GPIO_B9_AF  0
#define GPIO_B10_AF 0
#define GPIO_B11_AF 0
#define GPIO_B12_AF 0
#define GPIO_B13_AF 0
#define GPIO_B14_AF 0
#define GPIO_B15_AF 0

#define GPIO_C0_AF  0
#define GPIO_C1_AF  0
#define GPIO_C2_AF  0
#define GPIO_C3_AF  0
#define GPIO_C4_AF  0
#define GPIO_C5_AF  0
#define GPIO_C6_AF  0
#define GPIO_C7_AF  0
#define GPIO_C8_AF  0
#define GPIO_C9_AF  0
#define GPIO_C10_AF 0
#define GPIO_C11_AF 0
#define GPIO_C12_AF 0
#define GPIO_C13_AF 0
#define GPIO_C14_AF 0
#define GPIO_C15_AF 0

#define GPIO_D2_AF  0

#define GPIO_H0_AF  0
#define GPIO_H1_AF  0

/* ========================================================================== */

#endif /* PINOUT_H */
