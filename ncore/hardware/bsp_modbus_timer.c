#include "bsp_modbus_timer.h"
#include <stdlib.h>
#include "bsp_led.h"

#if CONFIG_MODULE_MODBUS

__IO uint32_t now_time = 0;

MODBUS_TIMER_EXPIRED_CB cc1_cb_fn;
MODBUS_TIMER_EXPIRED_CB cc2_cb_fn;
MODBUS_TIMER_EXPIRED_CB cc3_cb_fn;
MODBUS_TIMER_EXPIRED_CB cc4_cb_fn;

void CONFIG_MODBUS_TIM_IRHANDLER(void)
{
        if (TIM_GetITStatus(CONFIG_MODBUS_TIM, TIM_IT_CC1)) {
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC1);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC1, DISABLE);

                if (cc1_cb_fn != NULL) {
                        cc1_cb_fn();
                }
        } else if (TIM_GetITStatus(CONFIG_MODBUS_TIM, TIM_IT_CC2)) {
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC2);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC2, DISABLE);

                if (cc2_cb_fn != NULL) {
                        cc2_cb_fn();
                }
        } else if (TIM_GetITStatus(CONFIG_MODBUS_TIM, TIM_IT_CC3)) {
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC3);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC3, DISABLE);

                if (cc3_cb_fn != NULL) {
                        cc3_cb_fn();
                }
        } else if (TIM_GetITStatus(CONFIG_MODBUS_TIM, TIM_IT_CC4)) {
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC4);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC4, DISABLE);

                if (cc4_cb_fn != NULL) {
                        cc4_cb_fn();
                }
        } else if (TIM_GetITStatus(CONFIG_MODBUS_TIM, TIM_IT_Update)) {
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_Update);
                //LED1_TOGGLE;
                //LED2_TOGGLE;
        }
}

void modbus_timer_tick(void)
{
        ++now_time;
}

uint32_t modbus_timer_get(void)
{
        return now_time;
}

static void nvic_config(void)
{
        NVIC_InitTypeDef nvic_init;

        nvic_init.NVIC_IRQChannel = CONFIG_MODBUS_TIM_IRQ;
        nvic_init.NVIC_IRQChannelPreemptionPriority = CONFIG_MODBUS_TIM_IRQ_PRIO;
        nvic_init.NVIC_IRQChannelSubPriority = 0;
        nvic_init.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_init);
}

uint32_t modbus_tim_count_get(void)
{
        return TIM_GetCounter(TIM2);
}

void modbus_timer_start(uint8_t cc, uint32_t timeout, void *cb)
{
        uint32_t cnt_now;
        uint32_t cnt_target;

        cnt_now = TIM_GetCounter(TIM2);
        cnt_target = cnt_now + timeout;

        //printf("cnt_now = %d, cnt_target = %d\r\n", cnt_now, cnt_target);

        if (cc == 1) {
                cc1_cb_fn = (void (*)(void)) cb;

                TIM_SetCompare1(CONFIG_MODBUS_TIM, cnt_target);
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC1);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC1, ENABLE);
        } else if (cc == 2) {
                cc2_cb_fn = (void (*)(void)) cb;

                TIM_SetCompare2(CONFIG_MODBUS_TIM, cnt_target);
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC2);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC2, ENABLE);
        } else if (cc == 3) {
                cc3_cb_fn = (void (*)(void)) cb;

                TIM_SetCompare3(CONFIG_MODBUS_TIM, cnt_target);
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC3);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC3, ENABLE);
        } else if (cc == 4) {
                cc4_cb_fn = (void (*)(void)) cb;

                TIM_SetCompare4(CONFIG_MODBUS_TIM, cnt_target);
                TIM_ClearITPendingBit(CONFIG_MODBUS_TIM, TIM_IT_CC4);
                TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_CC4, ENABLE);
        }
}

void modbus_timer_init(void)
{
        TIM_TimeBaseInitTypeDef init;
        //uint32_t us_prescaler;

        CONFIG_MODBUS_TIM_CLK_FUN(CONFIG_MODBUS_TIM_CLK, ENABLE);

        //us_prescaler = 36000000 / 1000;

        init.TIM_Period = 0xFFFF;
        init.TIM_Prescaler = 72 - 1;
        init.TIM_ClockDivision = 0;
        init.TIM_CounterMode = TIM_CounterMode_Up;

        TIM_TimeBaseInit(CONFIG_MODBUS_TIM, &init);

        //TIM_ClearFlag(CONFIG_MODBUS_TIM, TIM_FLAG_Update);

        //TIM_ITConfig(CONFIG_MODBUS_TIM, TIM_IT_Update, ENABLE);

        nvic_config();

        TIM_Cmd(CONFIG_MODBUS_TIM, ENABLE);
}

#endif /* CONFIG_MODULE_MODBUS */

