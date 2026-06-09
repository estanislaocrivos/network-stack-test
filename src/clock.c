#include "clock.h"

#include "stm32f4xx.h"

static void clock_set_16mhz(void)
{
    /* Already running on HSI 16MHz after reset — nothing to do */
    SystemCoreClock = 16000000U;
}

static void clock_set_pll(
    uint32_t pllm,
    uint32_t plln,
    uint32_t pllp,
    uint32_t ahb_div,
    uint32_t apb1_div,
    uint32_t apb2_div,
    uint32_t core_clk)
{
    /* Enable HSI */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY))
        ;

    /* Switch to HSI before configuring PLL */
    RCC->CFGR &= ~RCC_CFGR_SW;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
        ;

    /* Disable PLL */
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY)
        ;

    /* Configure PLL: source = HSI, M/N/P divisors */
    RCC->PLLCFGR = (pllm << RCC_PLLCFGR_PLLM_Pos)
                   | (plln << RCC_PLLCFGR_PLLN_Pos)
                   | (((pllp >> 1U) - 1U) << RCC_PLLCFGR_PLLP_Pos)
                   | RCC_PLLCFGR_PLLSRC_HSI;

    /* Enable PLL and wait */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    /* Set flash latency before increasing clock */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN
                 | (core_clk <= 30000000U ? 0U :
                    core_clk <= 60000000U ? 1U :
                                            2U);

    /* Set bus prescalers */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2))
                | (ahb_div << RCC_CFGR_HPRE_Pos)
                | (apb1_div << RCC_CFGR_PPRE1_Pos)
                | (apb2_div << RCC_CFGR_PPRE2_Pos);

    /* Switch to PLL */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;

    SystemCoreClock = core_clk;
}

void clock_init(clock_freq_t freq)
{
    switch (freq)
    {
        case CLOCK_16MHZ:
            clock_set_16mhz();
            break;

        case CLOCK_48MHZ:
            /* HSI=16MHz, PLLM=16, PLLN=192, PLLP=4 -> 48MHz
             * APB1 max 42MHz -> /2, APB2 max 84MHz -> /1 */
            clock_set_pll(
                16,
                192,
                4,
                0, /* AHB  /1  */
                4, /* APB1 /2  */
                0, /* APB2 /1  */
                48000000U);
            break;

        case CLOCK_84MHZ:
            /* HSI=16MHz, PLLM=16, PLLN=336, PLLP=4 -> 84MHz
             * APB1 max 42MHz -> /2, APB2 max 84MHz -> /1 */
            clock_set_pll(
                16,
                336,
                4,
                0, /* AHB  /1  */
                4, /* APB1 /2  */
                0, /* APB2 /1  */
                84000000U);
            break;
    }
}
