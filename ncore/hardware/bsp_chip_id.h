#ifndef __BSP_CHIP_ID_H
#define __BSP_CHIP_ID_H

#define SID_GET_UNIQUE32(x)             ((x >= 0 && x < 3) ? (*(__IO uint32_t *) (0x1FFFF7E8 + 4 * (x))) : 0)

#endif /* __BSP_CHIP_ID_H */

