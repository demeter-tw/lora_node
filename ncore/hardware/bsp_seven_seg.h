#ifndef __BSP_SEVEN_SEG_H__
#define __BSP_SEVEN_SEG_H__

#include "config.h"

#if CONFIG_MODULE_SEVEN_SEG

typedef void (*SEVEN_SEG_SET_NUM_FUNC) (void);
typedef void (*SEVEN_SEG_CS_FUNC) (void);

void seven_seg_init(void);
SEVEN_SEG_SET_NUM_FUNC *seven_seg_num_func_get(void);
SEVEN_SEG_CS_FUNC *seven_seg_cs_func_get(void);

#endif /* CONFIG_MODULE_SEVEN_SEG */

#endif /* __BSP_SEVEN_SEG_H__ */

