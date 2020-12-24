#include "bsp_rtc.h"
#include "date.h"

#include <stdio.h>

#if CONFIG_MODULE_RTC

void bsp_rtc_set_time(struct rtc_time *tm)
{
	// Set timestamp ( calculate from date)
	RTC_SetCounter(_mktimev(tm));

	// Wait last RTC op
	RTC_WaitForLastTask();
}

void bsp_rtc_set_alarm(struct rtc_time *tm)
{
        RTC_SetAlarm(_mktimev(tm));
}

static void _rtc_config(void)
{
	// Enable PWR and Backup clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	// Permit access Backup zone
	PWR_BackupAccessCmd(ENABLE);

	// Re-init Backup zone
	BKP_DeInit();

#ifdef RTC_CLOCK_SOURCE_LSE
	// Enable LSE
	RCC_LSEConfig(RCC_LSE_ON);

	// Wait LSE ready
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
	}

	// Choose LSE as RTC clock source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	// Enable RTC clock
	RCC_RTCCLKCmd(ENABLE);

	// Wait RTC register sync done, because RTC clock speed is lower than APB1
	RTC_WaitForSynchro();

	// Confirm last RTC op
	RTC_WaitForLastTask();

	// Enable RTC second interrupt
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	// Confirm last RTC op
	RTC_WaitForLastTask();

	// Set RTC prescaler: Make RTC period is 1 second
	// RTC period = RTCCLK / RTC_PR = (32.768KHaz) / (32767 + 1) = 1 Hz */
	RTC_SetPrescaler(32767);

	// Confirm last RTC op
	RTC_WaitForLastTask();
#else
	// Enable LSI
	RCC_LSICmd(ENABLE);

	// Wait LSI ready
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
	}

	//  Choose LSI as RTC clock source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	// Enable RTC clock
	RCC_RTCCLKCmd(ENABLE);

	// Wait RTC register sync done, because RTC clock speed is lower than APB1
	RTC_WaitForSynchro();

	// Confirm last RTC op
	RTC_WaitForLastTask();

	// Enable RTC second interrupt
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	// Confirm last RTC op
	RTC_WaitForLastTask();

	// Set RTC prescaler: Make RTC period is one second, LSI almost 40KHz
	// RTC period =  RTCCLK / RTC_PR = (40KHz) / (40000 - 1 + 1) = 1Hz
	RTC_SetPrescaler(40000 - 1);

	// Confirm last RTC op
	RTC_WaitForLastTask();
#endif
}

void bsp_rtc_init(void)
{
	// Check BKP_DR1, if the data is not 0xA5A5, need to reconfig
	if (BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA) {
		printf("RTC is not set, config...\r\n");

		// RTC config
		_rtc_config();

		//Set time

		// Write flag to BKP_DR1
		BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	} else {
		printf("RTC is ready\r\n");

		// Enable PWR and Backup clock
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

		// Permit to access backup zone
		PWR_BackupAccessCmd(ENABLE);

#ifdef RTC_CLOCK_SOURCE_LSI
		// Enable LSI
		RCC_LSICmd(ENABLE);

		// Wait LSI ready
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
		}
#endif
		// Check PORRST
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
			printf("Power on Reset\r\n");
		}
		// Check if the pin reset flag is set
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
			printf("Pin Reset\r\n");
		}

		// Sync
		RTC_WaitForSynchro();

		// Enable RTC second interrupt
		RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);

		// Wait last RTC op
		RTC_WaitForLastTask();
	}

 #ifdef RTC_CLOCK_OUTPUT_ENABLE
 	// Disable Tamper pin
 	// Output RTCLCK / 64 to Tamper pin
 	BKP_TamperPinCmd(DISABLE);

 	// Enable RTC Clock to Tamper
 	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
 #endif /* RTC_CLOCK_OUTPUT_ENABLE */

 	// Clear flag
 	RCC_ClearFlag();

 	NVIC_SetPriority(RTC_IRQn, 2);
	NVIC_EnableIRQ(RTC_IRQn);
}

#endif /* CONFIG_MODULE_RTC */

