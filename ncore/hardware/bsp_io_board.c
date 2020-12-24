#include "bsp_io_board.h"

#include "bsp_rcc.h"
#include <stdlib.h>

#if CONFIG_MODULE_IO_BOARD

void bsp_io_board_power_on(void)
{
	GPIO_SetBits(CONFIG_IO_BOARD_POWER_SET_GPIO_PORT, CONFIG_IO_BOARD_POWER_SET_GPIO_PIN);
	mdelay(500);
	GPIO_ResetBits(CONFIG_IO_BOARD_POWER_SET_GPIO_PORT, CONFIG_IO_BOARD_POWER_SET_GPIO_PIN);
}

void bsp_io_board_power_off(void)
{
	GPIO_SetBits(CONFIG_IO_BOARD_POWER_RESET_GPIO_PORT, CONFIG_IO_BOARD_POWER_RESET_GPIO_PIN);
	mdelay(500);
	GPIO_ResetBits(CONFIG_IO_BOARD_POWER_RESET_GPIO_PORT, CONFIG_IO_BOARD_POWER_RESET_GPIO_PIN);
}

void bsp_io_board_init(void)
{
	GPIO_InitTypeDef gpio_init;

	CONFIG_IO_BOARD_POWER_SET_CLK_FUN(CONFIG_IO_BOARD_POWER_SET_CLK, ENABLE);
	gpio_init.GPIO_Pin= CONFIG_IO_BOARD_POWER_SET_GPIO_PIN;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_IO_BOARD_POWER_SET_GPIO_PORT, &gpio_init);

	CONFIG_IO_BOARD_POWER_RESET_CLK_FUN(CONFIG_IO_BOARD_POWER_RESET_CLK, ENABLE);
	gpio_init.GPIO_Pin = CONFIG_IO_BOARD_POWER_RESET_GPIO_PIN;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CONFIG_IO_BOARD_POWER_RESET_GPIO_PORT, &gpio_init);

	GPIO_ResetBits(CONFIG_IO_BOARD_POWER_SET_GPIO_PORT, CONFIG_IO_BOARD_POWER_SET_GPIO_PIN);
	GPIO_ResetBits(CONFIG_IO_BOARD_POWER_RESET_GPIO_PORT, CONFIG_IO_BOARD_POWER_RESET_GPIO_PIN);
}

#endif /* CONFIG_MODULE_IO_BOARD */


