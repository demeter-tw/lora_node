#ifndef __BSP_HW_I2C_H
#define __BSP_HW_I2C_H

#include "config.h"

#if CONFIG_MODULE_I2C

#define I2C_SPEED                               CONFIG_I2C_SPEED
#define I2C_OWN_ADDRESS                         0x01

#define I2C_FLAG_TIMEOUT_MS                     ((uint32_t) 1000)
#define I2C_LONG_TIMEOUT_MS                     ((uint32_t) (300 * I2C_FLAG_TIMEOUT_MS))


#define I2C_DEBUG_ON                            1
#define I2C_DEBUG_FUNC_ON                       0

#define I2C_INFO(fmt, arg...)                   printf("[I2C INFO] "fmt"\r\n", ##arg)
#define I2C_ERROR(fmt, arg...)                  printf("[I2C ERROR] "fmt"\r\n", ##arg) 
#define I2C_DEBUG(fmt, arg...)                  do {\
                                                        if (I2C_DEBUG_ON)\
                                                        printf("[I2C DEBUG] [%d]"fmt"\r\n", __LINE__, ##arg);\
                                                        } while (0)
#define I2C_ERR_CB(rv)            do {  \
                printf("Error: I2C error (%s, %d), rv = %d\r\n", __func__, __LINE__, rv);       \
                return rv;      \
        } while (0)

void sensor_i2c_init(void);
int sensor_i2c_byte_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data);
int sensor_i2c_buffer_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
int sensor_i2c_byte_write_16bit(unsigned char slave_addr, unsigned short reg_addr, unsigned char length, unsigned char const *data);
int sensor_i2c_buffer_read_16bit(unsigned char slave_addr, unsigned short reg_addr, unsigned char length, unsigned char *data);

#endif /* CONFIG_MODULE_I2C */

#endif /* __BSP_HW_I2C_H */

