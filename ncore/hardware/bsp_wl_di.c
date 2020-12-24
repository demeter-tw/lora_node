#include "bsp_wl_di.h"
#include <stdio.h>

#if CONFIG_DEMETER_SENSOR_WL_DI

#define NUM_WL	5

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
	uint8_t cm;
	uint8_t active;
} wl_t;

wl_t wl_arr[NUM_WL] = {
	{ NULL, 0, 0 },
	{ CONFIG_WL_DI_4_GPIO_PORT, CONFIG_WL_DI_4_GPIO_PIN, 0, 1 },
	{ CONFIG_WL_DI_1_GPIO_PORT, CONFIG_WL_DI_1_GPIO_PIN, 1, 0 },
	{ CONFIG_WL_DI_2_GPIO_PORT, CONFIG_WL_DI_2_GPIO_PIN, 3, 0 },
	{ CONFIG_WL_DI_3_GPIO_PORT, CONFIG_WL_DI_3_GPIO_PIN, 5, 0 }
};

uint32_t wl_val = 0;

/*
 * err1: 0 - OK, 1 - Error
 * err2: Which level is not working
 */
int bsp_wl_di_read(int *err1, int *err2)
{
	int pos;
	uint8_t val;
	int cm = -1;

	for (pos = 1; pos < NUM_WL; pos++) {
		val = GPIO_ReadInputDataBit(wl_arr[pos].port, wl_arr[pos].pin);
		if (val) {
			(wl_arr[pos].active) ? WL_BMP_SET(wl_val, pos) : WL_BMP_UNSET(wl_val, pos);
		} else {
			(!wl_arr[pos].active) ? WL_BMP_SET(wl_val, pos) : WL_BMP_UNSET(wl_val, pos);
		}
	}

	for (pos = NUM_WL - 1; pos > 0; pos--) {
		if (WL_BMP_ISSET(wl_val, pos)) {
			cm = wl_arr[pos].cm;
			break;
		}
	}

	// No value
	if (cm == -1) {
		*err1 = 1;
		*err2 = 99;
		return 99;
	}

	if (wl_val == (0x1 << (pos + 1)) - 2) {
		// Everything is all right.
		*err1 = 0;
		*err2 = 0;
	} else {
		// Some point is not working.
		*err1 = 1;
		for (--pos; pos > 0; pos--) {
			if (!WL_BMP_ISSET(wl_val, pos)) {
				*err2 = wl_arr[pos].cm;
			}
		}
	}

	return cm;
}

void bsp_wl_di_init(void)
{
	GPIO_InitTypeDef init;

	CONFIG_WL_DI_1_GPIO_CLK_FUN(CONFIG_WL_DI_1_GPIO_CLK, ENABLE);
	//CONFIG_WL_DI_2_GPIO_CLK_FUN(CONFIG_WL_DI_2_GPIO_CLK, ENABLE);
	//CONFIG_WL_DI_3_GPIO_CLK_FUN(CONFIG_WL_DI_3_GPIO_CLK, ENABLE);

	init.GPIO_Pin = CONFIG_WL_DI_1_GPIO_PIN;
	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_WL_DI_1_GPIO_PORT, &init);

	init.GPIO_Pin = CONFIG_WL_DI_2_GPIO_PIN;
	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_WL_DI_2_GPIO_PORT, &init);

	init.GPIO_Pin = CONFIG_WL_DI_3_GPIO_PIN;
	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_WL_DI_3_GPIO_PORT, &init);

	init.GPIO_Pin = CONFIG_WL_DI_4_GPIO_PIN;
	init.GPIO_Mode = GPIO_Mode_IPD;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_WL_DI_4_GPIO_PORT, &init);
}

#endif /* CONFIG_DEMETER_SENSOR_WL_DI */
