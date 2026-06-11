#ifndef PLATFORM_H
#define PLATFORM_H

/* ========================================================================== */

#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_gpio.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_mcu.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_spi.h"
#include "../external/c-libraries/drivers/stm32f4xx/inc/stm32f4xx_usart.h"

/* ========================================================================== */

#define PLATFORM_MCU_OPS    &stm32f4xx_mcu_ops
#define PLATFORM_GPIO_OPS   &stm32f4xx_gpio_ops
#define PLATFORM_USART1_OPS &stm32f4xx_usart1_ops
#define PLATFORM_USART2_OPS &stm32f4xx_usart2_ops
#define PLATFORM_USART6_OPS &stm32f4xx_usart6_ops
#define PLATFORM_SPI_OPS    &stm32f4xx_spi_ops

/* ========================================================================== */

#endif  // PLATFORM_H
