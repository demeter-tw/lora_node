#ifndef __BSP_WL_DI_H
#define __BSP_WL_DI_H

#include "config.h"

#if CONFIG_DEMETER_SENSOR_WL_DI

#define WL_BMP(i)		(0x1 << i)
#define WL_BMP_SET(bmp, i)	((bmp) |= WL_BMP(i))
#define WL_BMP_UNSET(bmp, i)	((bmp) &= ~(WL_BMP(i)))
#define WL_BMP_ISSET(bmp, i)	((bmp) & WL_BMP(i))

void bsp_wl_di_init(void);
int bsp_wl_di_read(int *err1, int *err2);

#endif /* CONFIG_DEMETER_SENSOR_WL_DI */

#endif /* __BSP_WL_DI_H  */

