#include "bsp_rcc.h"

static __IO uint32_t time_delay;

NCORE_RESULT_t rcc_init(void)
{
// HSI
#if CONFIG_RCC_HSI
        __IO uint32_t hsi_startup_status = 0;

        RCC_DeInit();

        /* Enable HSI */
        RCC_HSICmd(ENABLE);

        /* Wait HSI ready */
        hsi_startup_status = RCC->CR & RCC_CR_HSIRDY;
        if (hsi_startup_status == RCC_CR_HSIRDY) {
                /* Enable Prefetch Buffer */
                FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

                /* Flash 2 wait state */
                FLASH_SetLatency(FLASH_Latency_2);

                /* HCLK = SYSCLK */
                RCC_HCLKConfig(RCC_SYSCLK_Div1);

                /* PCLK2 = HCLK */
                RCC_PCLK2Config(RCC_HCLK_Div1);

                /* PCLK1 = HCLK/2 */
                RCC_PCLK1Config(RCC_HCLK_Div2);

#if 0
                /* PLLCLK = 4MHz * 16 = 64MHz (MAX) */
                RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);

                /* Enable PLL */
                RCC_PLLCmd(ENABLE);

                /* Wait till PLL is ready */
                while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
                }
#endif

                /* Select PLL as system colock source */
                RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

                /* Wait till PLL is used as system clock source */
                while (RCC_GetSYSCLKSource() != 0x00) {
                }

                SystemCoreClockUpdate();
        } else {
                while (1) {}
        }
#endif /* CONFIG_RCC_HSI */

// HSE
#if CONFIG_RCC_HSE
        __IO uint32_t hse_startup_status = 0;

        RCC_DeInit();

        /* Enable HSE */
        RCC_HSEConfig(RCC_HSE_ON);

        hse_startup_status = RCC_WaitForHSEStartUp();
        if (hse_startup_status == SUCCESS) {
                /* Enable Prefetch Buffer */
                FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

                /* Flash 2 wait state */
                FLASH_SetLatency(FLASH_Latency_2);

                /* HCLK = SYSCLK */
                RCC_HCLKConfig(RCC_SYSCLK_Div1);

                /* PCLK2 = HCLK */
                RCC_PCLK2Config(RCC_HCLK_Div1);

                /* PCLK1 = HCLK */
                RCC_PCLK1Config(RCC_HCLK_Div1);

#if 0
                /* PLLCLK = 8MHz * 9 = 72MHz */
                RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

                /* Enable PLL */
                RCC_PLLCmd(ENABLE);

                /* Wait till PLL is ready */
                while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
                }
#endif

                /* Select PLL as system colock source */
                RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

                /* Wait till PLL is used as system clock source */
                while (RCC_GetSYSCLKSource() != 0x04) {
                }

                SystemCoreClockUpdate();
        } else {
                while (1) {}
        }
#endif /* CONFIG_RCC_HSE */

// Clock source: PLLCLK
#if 0
        __IO uint32_t hse_startup_status = 0;

        RCC_DeInit();

        /* Enable HSE */
        RCC_HSEConfig(RCC_HSE_ON);

        hse_startup_status = RCC_WaitForHSEStartUp();
        if (hse_startup_status == SUCCESS) {
                /* Enable Prefetch Buffer */
                FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

                /* Flash 2 wait state */
                FLASH_SetLatency(FLASH_Latency_2);

                /* HCLK = SYSCLK */
                RCC_HCLKConfig(RCC_SYSCLK_Div1);

                /* PCLK2 = HCLK */
                RCC_PCLK2Config(RCC_HCLK_Div1);

                /* PCLK1 = HCLK/2 */
                RCC_PCLK1Config(RCC_HCLK_Div2);

                /* PLLCLK = 8MHz * 9 = 72MHz */
                RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

                /* Enable PLL */
                RCC_PLLCmd(ENABLE);

                /* Wait till PLL is ready */
                while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
                }

                /* Select PLL as system colock source */
                RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

                /* Wait till PLL is used as system clock source */
                while (RCC_GetSYSCLKSource() != 0x08) {
                }
        } else {
                while (1) {}
        }
#endif

#if !CONFIG_MODULE_CONTIKI
        if (SysTick_Config(SystemCoreClock / 1000)) {
                while (1);
        }
#endif /* !CONFIG_MODULE_CONTIKI */

        return NCORE_ERR_OK;
}


void rcc_mco_config(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        RCC_MCOConfig(RCC_MCO_SYSCLK);
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void mdelay(__IO uint32_t nTime)
{
    time_delay = nTime;

    while(time_delay != 0);
}


/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void delay_decrement(void)
{
    if (time_delay != 0x00) {
      time_delay--;
    }
}



