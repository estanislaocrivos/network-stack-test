#ifndef PLATFORM_H
#define PLATFORM_H

/* ========================================================================== */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_mcu.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_usart.h"

/* ========================================================================== */

#define PLATFORM_MCU_OPS    &stm32f4xx_mcu_ops
#define PLATFORM_GPIO_OPS   &stm32f4xx_gpio_ops
#define PLATFORM_USART1_OPS &stm32f4xx_usart1_ops
#define PLATFORM_USART2_OPS &stm32f4xx_usart2_ops
#define PLATFORM_USART6_OPS &stm32f4xx_usart6_ops
#define PLATFORM_SPI1_OPS   &stm32f4xx_spi1_ops
#define PLATFORM_SPI2_OPS   &stm32f4xx_spi2_ops
#define PLATFORM_SPI3_OPS   &stm32f4xx_spi3_ops
#define PLATFORM_SPI4_OPS   &stm32f4xx_spi4_ops
#define PLATFORM_I2C1_OPS   &stm32f4xx_i2c1_ops
#define PLATFORM_I2C2_OPS   &stm32f4xx_i2c2_ops
#define PLATFORM_I2C3_OPS   &stm32f4xx_i2c3_ops

/* ========================================================================== */

#endif /* PLATFORM_H */
