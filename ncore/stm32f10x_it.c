/**
  ******************************************************************************
  * @file    SysTick/TimeBase/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

#include "config.h"
#include "bsp_rcc.h"
#include "ntimer.h"
#include "bsp_modbus_timer.h"
#include "bsp_seven_seg.h"
#include <stdio.h>
extern int seven_seg_num[10];

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
        printf("\r\n##### NMI_Handler #####\r\n");
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  printf("\r\n##### HardFault_Handler #####\r\n");

  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  printf("\r\n##### MemManage_Handler #####\r\n");

  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  printf("\r\n##### BusFault_Handler #####\r\n");

  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  printf("\r\n##### UsageFault_Handler #####\r\n");

  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

#if 1
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
        printf("\r\n##### SVC_Handler #####\r\n");
}
#endif

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
        printf("\r\n##### DebugMon_Handler #####\r\n");
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
        printf("\r\n##### PendSV_Handler #####\r\n");
}

#if !CONFIG_MODULE_CONTIKI
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
        delay_decrement();
        ntimer_tick();
#if CONFIG_MODULE_MODBUS
        modbus_timer_tick();
#endif /* CONFIG_MODULE_MODBUS */
}
#endif /* !CONFIG_MODULE_CONTIKI */

#ifdef CONFIG_SEVEN_SEG_TIM_IRQHANDLER
void TIM4_IRQHandler(void)
{
        static int i = 0;

        if (TIM_GetITStatus(CONFIG_SEVEN_SEG_TIM, TIM_IT_Update) != RESET) {
                ((SEVEN_SEG_SET_NUM_FUNC *) seven_seg_num_func_get())[seven_seg_num[i]]();
                ((SEVEN_SEG_CS_FUNC *) seven_seg_cs_func_get())[i]();
                i++;
                i = i % 10;
                TIM_ClearITPendingBit(CONFIG_SEVEN_SEG_TIM , TIM_FLAG_Update);
        }
}
#endif /* CONFIG_SEVEN_SEG_TIM_IRQHANDLER */


void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		// Clear the RTC second interrupt
		RTC_ClearITPendingBit(RTC_IT_SEC);

		// Wait until last write operation on RTC registers has finished
		RTC_WaitForLastTask();
	}

	if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
	}

	RTC_ClearITPendingBit(RTC_IT_ALR | RTC_IT_SEC);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
