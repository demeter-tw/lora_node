#include "bsp_iwdg.h"
#include <stdio.h>

#if CONFIG_MODULE_WATCHDOG

void iwdg_feed(void)
{
        IWDG_ReloadCounter();
}


/*
 * @brief
 * Tout = prv /40 * rlv (s)
 * prv can be [4,8,16,32,64,128,256]
 * prv:
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 * rlv: 12 bit
 * Ex. IWDG_Config(IWDG_Prescaler_64 ,625); 
 *	(64/40) * 625 = 1s
 */
void iwdg_config(uint8_t prv, uint16_t rlv)
{
        // Make Prv and ARR be writeable
        IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

        // Set Presacler
        IWDG_SetPrescaler(prv);

        // Set RLR value
        IWDG_SetReload(rlv);

        // Load RLR value to CNT
        IWDG_ReloadCounter();

        // Enable IWDG
        IWDG_Enable();
}


void iwdg_init(void)
{
        printf("Watchdog init\r\n");

        iwdg_config(CONFIG_WATCHDOG_PRV, CONFIG_WATCHDOG_RLV);
}

#endif /* CONFIG_MODULE_WATCHDOG */

